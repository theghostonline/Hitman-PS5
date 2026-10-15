// SQLite storage for the section 7 schema. One connection, serialised by a mutex: the SDK's
// libsqlite3 may be built single-threaded, and the request rate here is tiny.
#include <pthread.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hs.h"
#include "schema_sql.h"  // generated from db/schema.sql by the Makefile

static sqlite3        *g_db;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

static int exec_sql(const char *sql) {
  char *err = NULL;
  if (sqlite3_exec(g_db, sql, NULL, NULL, &err) != SQLITE_OK) {
    LOGE("sqlite: %s", err ? err : "unknown error");
    sqlite3_free(err);
    return -1;
  }
  return 0;
}

static sqlite3_stmt *prep(const char *sql) {
  sqlite3_stmt *st = NULL;
  if (sqlite3_prepare_v2(g_db, sql, -1, &st, NULL) != SQLITE_OK) {
    LOGE("sqlite prepare: %s", sqlite3_errmsg(g_db));
    return NULL;
  }
  return st;
}

static void bind_text(sqlite3_stmt *st, int i, const char *s) {
  if (s) sqlite3_bind_text(st, i, s, -1, SQLITE_TRANSIENT);
  else sqlite3_bind_null(st, i);
}

// Steps a statement that returns no rows, then finalizes it.
static int run(sqlite3_stmt *st) {
  if (!st) return -1;
  int rc = sqlite3_step(st);
  if (rc != SQLITE_DONE && rc != SQLITE_ROW) LOGE("sqlite step: %s", sqlite3_errmsg(g_db));
  sqlite3_finalize(st);
  return rc == SQLITE_DONE || rc == SQLITE_ROW ? 0 : -1;
}

static void copy_col(sqlite3_stmt *st, int col, char *out, size_t n) {
  const unsigned char *s = sqlite3_column_text(st, col);
  snprintf(out, n, "%s", s ? (const char *)s : "");
}

static int  begin(void) { return exec_sql("BEGIN IMMEDIATE"); }
static int  commit(void) { return exec_sql("COMMIT"); }
static void rollback(void) { exec_sql("ROLLBACK"); }

int db_open(const char *path) {
  if (sqlite3_open_v2(path, &g_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) != SQLITE_OK) {
    LOGE("cannot open database %s: %s", path, g_db ? sqlite3_errmsg(g_db) : "out of memory");
    return -1;
  }
  sqlite3_busy_timeout(g_db, 5000);
  if (exec_sql("PRAGMA foreign_keys = ON") || exec_sql(SCHEMA_SQL)) return -1;
  LOGI("database %s ready (sqlite %s)", path, sqlite3_libversion());
  return 0;
}

void db_close(void) {
  pthread_mutex_lock(&g_lock);
  if (g_db) sqlite3_close(g_db);
  g_db = NULL;
  pthread_mutex_unlock(&g_lock);
}

// ------------------------------------------------------------------ users

static int insert_meta_if_missing(const char *user_id) {
  char etag[25];
  random_hex(etag, 12);
  sqlite3_stmt *st = prep("INSERT OR IGNORE INTO profile_meta (user_id, etag, version) VALUES (?, ?, 1)");
  if (!st) return -1;
  bind_text(st, 1, user_id);
  bind_text(st, 2, etag);
  return run(st);
}

static int upsert_extension(const char *user_id, const char *name, json_t *value, int64_t t) {
  char *data = json_dumps(value, JSON_COMPACT | JSON_ENCODE_ANY);
  if (!data) return -1;
  sqlite3_stmt *st = prep(
      "INSERT INTO profile_extensions (user_id, name, data, updated_at) VALUES (?, ?, ?, ?) "
      "ON CONFLICT (user_id, name) DO UPDATE SET data = excluded.data, updated_at = excluded.updated_at");
  if (!st) { free(data); return -1; }
  bind_text(st, 1, user_id);
  bind_text(st, 2, name);
  bind_text(st, 3, data);
  sqlite3_bind_int64(st, 4, t);
  free(data);
  return run(st);
}

int db_login_psn(const char *account_id, const char *online_id, const char *issuer_id,
                 const char *region, const char *locale, json_t *seed,
                 char user_id[HS_ID_LEN + 1], bool *created) {
  int64_t t = now_seconds();
  int rc = -1;
  pthread_mutex_lock(&g_lock);
  if (begin()) goto out;

  sqlite3_stmt *st = prep("SELECT id FROM users WHERE psn_account_id = ?");
  if (!st) goto fail;
  bind_text(st, 1, account_id);
  bool found = sqlite3_step(st) == SQLITE_ROW;
  if (found) copy_col(st, 0, user_id, HS_ID_LEN + 1);
  sqlite3_finalize(st);

  if (found) {
    // A changed PSN online id, region or locale follows the account.
    st = prep("UPDATE users SET psn_online_id = COALESCE(?, psn_online_id), issuer_id = COALESCE(?, issuer_id), "
              "region = COALESCE(?, region), locale = COALESCE(?, locale), updated_at = ? WHERE id = ?");
    if (!st) goto fail;
    bind_text(st, 1, online_id);
    bind_text(st, 2, issuer_id);
    bind_text(st, 3, region);
    bind_text(st, 4, locale);
    sqlite3_bind_int64(st, 5, t);
    bind_text(st, 6, user_id);
    if (run(st) || insert_meta_if_missing(user_id)) goto fail;
    *created = false;
  } else {
    uuid_v4(user_id);
    st = prep("INSERT INTO users (id, psn_account_id, psn_online_id, issuer_id, region, locale, created_at, updated_at) "
              "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    if (!st) goto fail;
    bind_text(st, 1, user_id);
    bind_text(st, 2, account_id);
    bind_text(st, 3, online_id);
    bind_text(st, 4, issuer_id);
    bind_text(st, 5, region);
    bind_text(st, 6, locale);
    sqlite3_bind_int64(st, 7, t);
    sqlite3_bind_int64(st, 8, t);
    if (run(st) || insert_meta_if_missing(user_id)) goto fail;
    if (seed) {
      const char *name;
      json_t *value;
      json_object_foreach(seed, name, value) {
        if (upsert_extension(user_id, name, value, t)) goto fail;
      }
    }
    *created = true;
  }
  if (commit()) goto fail;
  rc = 0;
  goto out;
fail:
  rollback();
out:
  pthread_mutex_unlock(&g_lock);
  return rc;
}

// ------------------------------------------------------------------ tokens

// Caller holds the lock. Rows live until expires_at + refresh_ttl so the refresh token
// still works after the access token has expired.
static int issue_tokens_locked(const char *user_id, char access[HS_TOKEN_LEN + 1], char refresh[HS_TOKEN_LEN + 1]) {
  int64_t t = now_seconds();
  sqlite3_stmt *st = prep("DELETE FROM auth_tokens WHERE expires_at < ?");
  if (!st) return -1;
  sqlite3_bind_int64(st, 1, t - g_cfg.refresh_ttl);
  if (run(st)) return -1;

  random_hex(access, HS_TOKEN_LEN / 2);
  random_hex(refresh, HS_TOKEN_LEN / 2);
  st = prep("INSERT INTO auth_tokens (access_token, refresh_token, user_id, expires_at) VALUES (?, ?, ?, ?)");
  if (!st) return -1;
  bind_text(st, 1, access);
  bind_text(st, 2, refresh);
  bind_text(st, 3, user_id);
  sqlite3_bind_int64(st, 4, t + g_cfg.token_ttl);
  return run(st);
}

int db_issue_tokens(const char *user_id, char access[HS_TOKEN_LEN + 1], char refresh[HS_TOKEN_LEN + 1]) {
  pthread_mutex_lock(&g_lock);
  int rc = issue_tokens_locked(user_id, access, refresh);
  pthread_mutex_unlock(&g_lock);
  return rc;
}

int db_refresh_tokens(const char *refresh_in, char user_id[HS_ID_LEN + 1],
                      char access[HS_TOKEN_LEN + 1], char refresh[HS_TOKEN_LEN + 1]) {
  int rc = -1;
  pthread_mutex_lock(&g_lock);
  if (begin()) goto out;
  sqlite3_stmt *st = prep("SELECT user_id, access_token, expires_at FROM auth_tokens WHERE refresh_token = ?");
  if (!st) goto fail;
  bind_text(st, 1, refresh_in);
  char old_access[HS_TOKEN_LEN + 1] = "";
  bool live = false;
  if (sqlite3_step(st) == SQLITE_ROW) {
    copy_col(st, 0, user_id, HS_ID_LEN + 1);
    copy_col(st, 1, old_access, sizeof(old_access));
    live = sqlite3_column_int64(st, 2) + g_cfg.refresh_ttl >= now_seconds();
  }
  sqlite3_finalize(st);
  if (!live) {
    rollback();
    rc = 1;
    goto out;
  }
  st = prep("DELETE FROM auth_tokens WHERE access_token = ?");
  if (!st) goto fail;
  bind_text(st, 1, old_access);
  if (run(st) || issue_tokens_locked(user_id, access, refresh) || commit()) goto fail;
  rc = 0;
  goto out;
fail:
  rollback();
out:
  pthread_mutex_unlock(&g_lock);
  return rc;
}

int db_user_for_token(const char *access, char user_id[HS_ID_LEN + 1]) {
  int rc = 1;
  pthread_mutex_lock(&g_lock);
  sqlite3_stmt *st = prep("SELECT user_id FROM auth_tokens WHERE access_token = ? AND expires_at > ?");
  if (!st) {
    rc = -1;
  } else {
    bind_text(st, 1, access);
    sqlite3_bind_int64(st, 2, now_seconds());
    if (sqlite3_step(st) == SQLITE_ROW) {
      copy_col(st, 0, user_id, HS_ID_LEN + 1);
      rc = 0;
    }
    sqlite3_finalize(st);
  }
  pthread_mutex_unlock(&g_lock);
  return rc;
}

int db_user_for_latest_token(char user_id[HS_ID_LEN + 1]) {
  int rc = 1;
  pthread_mutex_lock(&g_lock);
  sqlite3_stmt *st = prep("SELECT user_id FROM auth_tokens WHERE expires_at > ? ORDER BY expires_at DESC LIMIT 1");
  if (!st) {
    rc = -1;
  } else {
    sqlite3_bind_int64(st, 1, now_seconds());
    if (sqlite3_step(st) == SQLITE_ROW) {
      copy_col(st, 0, user_id, HS_ID_LEN + 1);
      rc = 0;
    }
    sqlite3_finalize(st);
  }
  pthread_mutex_unlock(&g_lock);
  return rc;
}

// ------------------------------------------------------------------ profile

static bool name_wanted(json_t *names, const char *name) {
  if (!names) return true;
  size_t i;
  json_t *v;
  json_array_foreach(names, i, v) {
    if (json_is_string(v) && !strcmp(json_string_value(v), name)) return true;
  }
  return false;
}

json_t *db_get_profile(const char *user_id, json_t *names) {
  json_t *profile = NULL;
  pthread_mutex_lock(&g_lock);

  sqlite3_stmt *st = prep("SELECT psn_account_id, psn_online_id FROM users WHERE id = ?");
  if (!st) goto out;
  bind_text(st, 1, user_id);
  if (sqlite3_step(st) != SQLITE_ROW) {
    sqlite3_finalize(st);
    goto out;
  }
  // LinkedAccounts uses the console's "ps4" keys even on PS5 (sub_c4b834).
  json_t *linked = json_object();
  json_object_set_new(linked, "ps4", json_string((const char *)sqlite3_column_text(st, 0)));
  const unsigned char *online = sqlite3_column_text(st, 1);
  if (online) json_object_set_new(linked, "ps4onlineid", json_string((const char *)online));
  profile = json_object();
  json_object_set_new(profile, "Id", json_string(user_id));
  json_object_set_new(profile, "LinkedAccounts", linked);
  // A missing Gamertag shows as UI_USER_ANONYMOUS in the game.
  if (online) json_object_set_new(profile, "Gamertag", json_string((const char *)online));
  sqlite3_finalize(st);

  json_t *ext = json_object();
  st = prep("SELECT name, data FROM profile_extensions WHERE user_id = ? ORDER BY name");
  if (st) {
    bind_text(st, 1, user_id);
    while (sqlite3_step(st) == SQLITE_ROW) {
      const char *name = (const char *)sqlite3_column_text(st, 0);
      if (!name_wanted(names, name)) continue;
      json_error_t err;
      json_t *v = json_loads((const char *)sqlite3_column_text(st, 1), JSON_DECODE_ANY, &err);
      if (!v) {
        LOGW("profile %s: extension %s is not valid JSON (%s), sending null", user_id, name, err.text);
        v = json_null();
      }
      json_object_set_new(ext, name, v);
    }
    sqlite3_finalize(st);
  }
  json_object_set_new(profile, "Extensions", ext);

  json_t *etag = json_null();
  json_int_t version = 1;
  st = prep("SELECT etag, version FROM profile_meta WHERE user_id = ?");
  if (st) {
    bind_text(st, 1, user_id);
    if (sqlite3_step(st) == SQLITE_ROW) {
      const unsigned char *e = sqlite3_column_text(st, 0);
      if (e) {
        json_decref(etag);
        etag = json_string((const char *)e);
      }
      version = sqlite3_column_int64(st, 1);
    }
    sqlite3_finalize(st);
  }
  json_object_set_new(profile, "ETag", etag);
  json_object_set_new(profile, "Version", json_integer(version));
out:
  pthread_mutex_unlock(&g_lock);
  return profile;
}

int db_update_extensions(const char *user_id, json_t *extensions) {
  int64_t t = now_seconds();
  int rc = -1;
  pthread_mutex_lock(&g_lock);
  if (begin()) goto out;
  const char *name;
  json_t *value;
  json_object_foreach(extensions, name, value) {
    if (upsert_extension(user_id, name, value, t)) goto fail;
  }
  if (insert_meta_if_missing(user_id)) goto fail;
  char etag[25];
  random_hex(etag, 12);
  sqlite3_stmt *st = prep("UPDATE profile_meta SET etag = ?, version = version + 1 WHERE user_id = ?");
  if (!st) goto fail;
  bind_text(st, 1, etag);
  bind_text(st, 2, user_id);
  if (run(st) || commit()) goto fail;
  rc = 0;
  goto out;
fail:
  rollback();
out:
  pthread_mutex_unlock(&g_lock);
  return rc;
}

// ------------------------------------------------------------------ events

int db_save_events(const char *user_id, const hs_event_t *events, size_t n, int64_t *ticks_out) {
  if (n == 0) return 0;
  int rc = -1;
  int64_t started_at = now_seconds();
  pthread_mutex_lock(&g_lock);
  if (begin()) goto out;

  // Server ticks are microseconds since the epoch, kept unique and increasing per user.
  int64_t last = 0;
  sqlite3_stmt *st = prep("SELECT MAX(ticks) FROM events WHERE user_id = ?");
  if (!st) goto fail;
  bind_text(st, 1, user_id);
  if (sqlite3_step(st) == SQLITE_ROW) last = sqlite3_column_int64(st, 0);
  sqlite3_finalize(st);
  int64_t start = now_micros();
  if (start <= last) start = last + 1;

  for (size_t i = 0; i < n; i++) {
    const hs_event_t *e = &events[i];
    ticks_out[i] = start + (int64_t)i;
    st = prep("INSERT INTO events (user_id, ticks, id, name, contract_session_id, contract_id, timestamp, origin, "
              "value, xp_gain) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    if (!st) goto fail;
    bind_text(st, 1, user_id);
    sqlite3_bind_int64(st, 2, ticks_out[i]);
    bind_text(st, 3, e->id);
    bind_text(st, 4, e->name ? e->name : "");
    bind_text(st, 5, e->contract_session_id);
    bind_text(st, 6, e->contract_id);
    if (e->has_timestamp) sqlite3_bind_double(st, 7, e->timestamp);
    else sqlite3_bind_null(st, 7);
    bind_text(st, 8, e->origin);
    bind_text(st, 9, e->value_json);
    if (e->has_xp_gain) sqlite3_bind_int64(st, 10, e->xp_gain);
    else sqlite3_bind_null(st, 10);
    if (run(st)) goto fail;

    if (e->contract_session_id && e->contract_id) {
      st = prep("INSERT OR IGNORE INTO contract_sessions (id, user_id, contract_id, started_at) VALUES (?, ?, ?, ?)");
      if (!st) goto fail;
      bind_text(st, 1, e->contract_session_id);
      bind_text(st, 2, user_id);
      bind_text(st, 3, e->contract_id);
      sqlite3_bind_int64(st, 4, started_at);
      if (run(st)) goto fail;
    }
  }
  if (commit()) goto fail;
  rc = 0;
  goto out;
fail:
  rollback();
out:
  pthread_mutex_unlock(&g_lock);
  return rc;
}
