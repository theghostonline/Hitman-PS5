-- Local database schema, README.md section 7 of the network map.
-- Shaped by what the client sends and reads. Extensions stay as JSON blobs because the client
-- treats them that way and UpdateExtensions writes them per key.
-- Idempotent: safe to run on every start.

CREATE TABLE IF NOT EXISTS users (
  id               TEXT PRIMARY KEY,        -- profile Id / Init.profileid (uuid)
  psn_account_id   TEXT UNIQUE NOT NULL,    -- external_psn psn_userid, LinkedAccounts.ps4
  psn_online_id    TEXT,                    -- psn_onlineid, LinkedAccounts.ps4onlineid, Gamertag
  issuer_id        TEXT,                    -- psn_issuerid / ?issuer=
  region           TEXT,                    -- rgn
  locale           TEXT,
  created_at       INTEGER NOT NULL,
  updated_at       INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS auth_tokens (
  access_token     TEXT PRIMARY KEY,        -- opaque to the client
  refresh_token    TEXT UNIQUE NOT NULL,
  user_id          TEXT NOT NULL REFERENCES users(id),
  expires_at       INTEGER NOT NULL         -- now + expires_in
);

CREATE TABLE IF NOT EXISTS profile_extensions (
  user_id          TEXT NOT NULL REFERENCES users(id),
  name             TEXT NOT NULL,           -- inventory, progression, transactions, contracts, gameclient,
                                            -- gamepersistentdata, opportunityprogression, achievements, entitlements
  data             TEXT NOT NULL,           -- JSON
  updated_at       INTEGER NOT NULL,
  PRIMARY KEY (user_id, name)
);

CREATE TABLE IF NOT EXISTS profile_meta (
  user_id          TEXT PRIMARY KEY REFERENCES users(id),
  etag             TEXT,
  version          INTEGER NOT NULL DEFAULT 1
);

CREATE TABLE IF NOT EXISTS events (
  user_id              TEXT NOT NULL REFERENCES users(id),
  ticks                INTEGER NOT NULL,    -- server ticks, answers lastEventTicks
  id                   TEXT,
  name                 TEXT NOT NULL,
  contract_session_id  TEXT,
  contract_id          TEXT,
  timestamp            REAL,
  origin               TEXT,
  value                TEXT,                -- JSON
  xp_gain              INTEGER,
  PRIMARY KEY (user_id, ticks)
);
CREATE INDEX IF NOT EXISTS events_session ON events(contract_session_id);
-- A retransmitted event (same Id, e.g. after a timed-out SaveAndSynchronizeEvents4) is stored
-- once and answered with its original ticks (db.c db_save_events). Older databases may hold
-- duplicates from before this index; the first copy is kept.
DELETE FROM events WHERE id IS NOT NULL AND rowid NOT IN
  (SELECT MIN(rowid) FROM events WHERE id IS NOT NULL GROUP BY user_id, id);
CREATE UNIQUE INDEX IF NOT EXISTS events_user_event_id ON events(user_id, id) WHERE id IS NOT NULL;

CREATE TABLE IF NOT EXISTS contract_sessions (
  id               TEXT PRIMARY KEY,        -- ContractSessionId
  user_id          TEXT NOT NULL REFERENCES users(id),
  contract_id      TEXT NOT NULL,
  difficulty_level INTEGER,
  started_at       INTEGER,
  state            TEXT                     -- JSON scoring state
);
