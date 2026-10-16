// Points the running game at this server, in memory, with no file changes (network map 6.1/6.2):
//   "https://{0}"            -> "http://{0}"          config URL scheme (sub_c44010)
//   "https://auth.hitman.io" -> "http://<domain>"     fallback global auth host (sub_c50246)
//   "config.hitman.io"       -> "<domain>"            default of online_VersionConfigDomain
//   online_VersionConfigDomain (char[256])            set to "<domain>" once static init filled it
// Every target is compared with the expected bytes first; nothing is written to another build.
#include "hs.h"

#ifndef __PROSPERO__

void patcher_start(void) {
  LOGI("patcher: host build, not patching anything");
}

void patcher_status(char *out, size_t n) {
  snprintf(out, n, "not available in host builds");
}

#else

#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/user.h>
#include <unistd.h>

#include <dirent.h>
#include <sys/stat.h>

#include <ps5/kernel.h>
#include <ps5/mdbg.h>

#include "x86stub.h"

// The title id starts at byte 16 (seen on FW 13.60: the SDK ps sample's struct, with a 64-bit
// field at offset 8, reads it 4 bytes late). Oversized so the kernel never writes past it.
typedef struct {
  uint32_t app_id;
  uint32_t unknown1[2];
  uint32_t app_type;
  char     title_id[10];
  char     rest[0x200];
} app_info_t;

int sceKernelGetAppInfo(pid_t pid, app_info_t *info);

#define NHOOKS 15

// Eboot virtual addresses of one game build. The build is recognised by the three strings at
// scheme/auth_host/domain_def, so a build not listed here is never written to.
typedef struct {
  const char *version;
  intptr_t    scheme, auth_host, domain_def;
  intptr_t    domain_var;    // online_VersionConfigDomain (char[256])
  intptr_t    online_state;  // ZOnlineManager+0x18, the connection state the gate in 2 checks
  intptr_t    stub_area;     // after the last function of the text segment, to the end of its page
  intptr_t    ent_table;     // unused padding at the end of the RELRO segment
  intptr_t    span;          // end of the eboot's highest segment
  size_t      nlabels;       // add-on labels this build knows (a prefix of KNOWN_LABELS)
  intptr_t    got[NHOOKS];   // GOT slots, in g_hooks order (from the PLT stubs' jmp [rip+x])
  intptr_t    res_disable;   // OnlineResources_Disable console variable (name pointer; int value at +0x18), 0 if none
} build_t;

static const build_t BUILDS[] = {
    // 01.017.000, eboot.bin sha256 4a7aad32...5bed. Text ends at 0x1c03cdc; the RELRO segment ends
    // at 0x25719b0 in the file, its pages run to 0x2574000 (TLS template 0x25719c0-0x2571a68).
    {"01.017.000", 0x1c88dc5, 0x1c558a0, 0x1c240d4, 0x3c6b510, 0x3c6b7d8, 0x1c03ce0, 0x2572000, 0x5718000, 23,
     {0x25645c0, 0x25645c8, 0x25645d0, 0x2564a28, 0x2564968, 0x2564888, 0x2564600, 0x2564608, 0x25645f8, 0x25645f0,
      0x2564648, 0x2564650, 0x2564658, 0x2564660, 0x25645b8}, 0},
    // 01.041.000 (client 8.23.0), eboot.bin (fself) sha256 7a2626a4...1bca. Text ends at 0x1b7821c;
    // the RELRO segment's file part ends at 0x24a4440 (SCE_PROCPARAM last), its pages run to
    // 0x24a8000, and nothing in the eboot points into that tail.
    {"01.041.000", 0x20f6945, 0x20bad89, 0x2105374, 0x3bbe9d8, 0x3c648b8, 0x1b78220, 0x24a5000, 0x567c000, 34,
     {0x2494c08, 0x2494c10, 0x2494c40, 0x2494f98, 0x2494fc8, 0x2494f80, 0x2494c60, 0x2494c68, 0x2494c70, 0x2494c58,
      0x2494c18, 0x2494c20, 0x2494c30, 0x2494c28, 0x2494c78},
     // OnlineResources_Disable defaults to 1 here (static init at 0xcbcfc3), which skips the dynamic
     // resources fetch (0xcf9067), so the hitman-server package never mounts.
     0x3bbf258},
    // The "v01.041 -4xx" backport (fakelib + libc for low firmware) ships a newer eboot than
    // 01.041.000: client 8.25.0, eboot.bin (fself) sha256 7064b7d4...7d07. Text ends at 0x1bd073c;
    // the RELRO segment's file part ends at 0x25245e0 (SCE_PROCPARAM last), its pages run to
    // 0x2528000. OnlineResources_Disable defaults to 1 here too (static init at 0xce6423).
    {"01.041.000 backport", 0x21670dc, 0x212ae9d, 0x2175d38, 0x3c46e58, 0x3cedcf8, 0x1bd0740, 0x2525000, 0x5720000, 38,
     {0x25147d0, 0x25147d8, 0x2514808, 0x2514b60, 0x2514b90, 0x2514b48, 0x2514828, 0x2514830, 0x2514838, 0x2514820,
      0x25147e0, 0x25147e8, 0x25147f8, 0x25147f0, 0x2514840},
     0x3c476d8},
};
#define NBUILDS (sizeof(BUILDS) / sizeof(BUILDS[0]))

static const build_t *g_build;  // the build of the process being patched

#define PAGE           0x4000
#define POLL_US        20000
#define GIVE_UP_US     (90 * 1000000)

static pthread_mutex_t g_status_lock = PTHREAD_MUTEX_INITIALIZER;
static char            g_status[256] = "not started";

static void set_status(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static void set_status(const char *fmt, ...) {
  char buf[sizeof(g_status)];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  pthread_mutex_lock(&g_status_lock);
  bool changed = strcmp(buf, g_status) != 0;
  memcpy(g_status, buf, sizeof(g_status));
  pthread_mutex_unlock(&g_status_lock);
  if (changed) LOGD("patcher: %s", buf);
}

void patcher_status(char *out, size_t n) {
  pthread_mutex_lock(&g_status_lock);
  snprintf(out, n, "%s", g_status);
  pthread_mutex_unlock(&g_status_lock);
}

typedef struct {
  const char *what;
  intptr_t    va;
  const char *orig;      // expected bytes, compared including the NUL
  char        repl[32];  // written over the same length, NUL padded
} patch_t;

// UI_CONNECTION_STATUS_* order from the network map (section 2). That the field above indexes
// this table is inferred; the raw number is always reported next to the name.
static const char *state_name(int s) {
  static const char *const N[40] = {
      "OFFLINE", "OFFLINE", "BEGIN_CONNECTION_FLOW", "DISCONNECTED", "BEGIN_CONNECTION_FLOW", "PREAUTHENTICATE",
      "FETCH_API_VERSION", "FETCH_API_VERSION", "API_VERSION_RECEIVED", "FETCHING_CONFIGURATION",
      "CONFIGURATION_RECEIVED", "ONLINE_CONSENT_CONFIRMATION", NULL, NULL, "AUTHENTICATING",
      "AUTHENTICATION_RECEIVED", "ENTITLEMENTS_SYNCHRONIZE", "ENTITLEMENTS_SYNCHRONIZE", "AUTHENTICATING_GAMESERVICE",
      "AUTHENTICATING_GAMESERVICE_RECEIVED", "FSP_IOI_ACCOUNT_SIGNUP", "FETCHING_OFFLINE_CACHE_DB_DIFF",
      "OFFLINE_CACHE_DB_DIFF_RECEIVED", "FETCHING_DYNAMIC_RESOURCES", "DYNAMIC_RESOURCES_MOUNTED", NULL,
      "FETCHING_PLATFORM_USERINFO", "PLATFORM_USERINFO_RECEIVED", NULL, NULL, "FETCHING_PROFILE", "PROFILE_RECEIVED",
      "SYNCHRONIZING_PROFILE", "PROFILE_SYNCHRONIZED", "RETRIEVING_EVENTS", "EVENTS_RETRIEVED",
      "WAITING_FOR_SYNCHRONIZING_EVENTS", NULL, NULL, "CONNECTED"};
  return s >= 0 && s < 40 && N[s] ? N[s] : "?";
}

static pid_t find_game(void) {
  int    mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PROC, 0};
  size_t size = 0;
  if (sysctl(mib, 4, NULL, &size, NULL, 0)) return -1;
  size += 32 * sizeof(struct kinfo_proc);
  char *buf = malloc(size);
  if (!buf) return -1;
  pid_t found = -1;
  int   seen = 0;
  if (sysctl(mib, 4, buf, &size, NULL, 0) != 0) {
    set_status("process scan failed: %s", strerror(errno));
  } else {
    for (char *p = buf; p < buf + size;) {
      struct kinfo_proc *ki = (struct kinfo_proc *)p;
      if (ki->ki_structsize <= 0) break;
      p += ki->ki_structsize;
      if (strcmp(ki->ki_comm, "eboot.bin")) continue;
      app_info_t info;
      memset(&info, 0, sizeof(info));
      int rc = sceKernelGetAppInfo(ki->ki_pid, &info);
      seen++;
      if (rc == 0 && !strncmp(info.title_id, g_cfg.title_id, sizeof(info.title_id))) {
        found = ki->ki_pid;
        break;
      }
      set_status("waiting for %s; eboot.bin pid %d has title \"%.9s\" (app info rc 0x%x)", g_cfg.title_id,
                 ki->ki_pid, info.title_id, (unsigned)rc);
    }
    if (!seen && found < 0) set_status("waiting for %s; no game running", g_cfg.title_id);
  }
  free(buf);
  return found;
}

static int mem_read(pid_t pid, intptr_t addr, void *buf, size_t n) {
  if (mdbg_copyout(pid, addr, buf, n) == 0) return 0;
  return kernel_proc_copyout(pid, addr, buf, n) == 0 ? 0 : -1;
}

static bool mem_equals(pid_t pid, intptr_t addr, const void *want, size_t n) {
  char got[64];
  return n <= sizeof(got) && mem_read(pid, addr, got, n) == 0 && !memcmp(got, want, n);
}

// Debugger write first (copy-on-write into read-only pages); if it did not take, open the page
// up for a kernel copy and put its protection back. The page's protection must be known before
// it is changed, and a failed restore is reported: a page left writable is not silently accepted.
static int mem_write(pid_t pid, intptr_t addr, const void *buf, size_t n) {
  if (mdbg_copyin(pid, buf, addr, n) == 0 && mem_equals(pid, addr, buf, n)) return 0;
  intptr_t page = addr & ~(intptr_t)(PAGE - 1);
  size_t   span = (size_t)(((addr + (intptr_t)n + PAGE - 1) & ~(intptr_t)(PAGE - 1)) - page);
  int      prot = kernel_get_vmem_protection(pid, page, span);
  if (prot <= 0) {
    LOGW("patcher: protection of 0x%lx unknown (%d), not changing it", (long)page, prot);
    return -1;
  }
  if (kernel_mprotect(pid, page, span, prot | PROT_READ | PROT_WRITE)) return -1;
  int rc = kernel_proc_copyin(pid, buf, addr, n);
  if (kernel_mprotect(pid, page, span, prot)) {
    LOGE("patcher: could not restore protection %d on 0x%lx after writing", prot, (long)page);
    rc = -1;
  }
  return rc == 0 && mem_equals(pid, addr, buf, n) ? 0 : -1;
}

// 1 already patched, 0 original bytes, -1 unreadable (not mapped yet), -2 unexpected bytes.
static int check(pid_t pid, intptr_t base, const patch_t *p) {
  size_t n = strlen(p->orig) + 1;
  char cur[32], repl[32];
  if (mem_read(pid, base + p->va, cur, n)) return -1;
  memset(repl, 0, sizeof(repl));
  memcpy(repl, p->repl, strlen(p->repl));
  if (!memcmp(cur, repl, n)) return 1;
  if (!memcmp(cur, p->orig, n)) return 0;
  return -2;
}

static int write_patch(pid_t pid, intptr_t base, const patch_t *p) {
  size_t n = strlen(p->orig) + 1;
  char repl[32];
  memset(repl, 0, sizeof(repl));
  memcpy(repl, p->repl, strlen(p->repl));
  return mem_write(pid, base + p->va, repl, n);
}

static bool still_running(pid_t pid) {
  return find_game() == pid;
}

// ---------------------------------------------------------------- PSN bypass
// Before the online flow ever fetches the config it needs PSN: the game's cached sign-in state
// (sceNpGetState and the NP state callback, sub_103bd2a / sub_1039b9e), sceNpCheckNpReachability
// (sub_c824f0), the online id (sub_c83062, which opens the sign-in dialog when the cached state
// isn't 2) and a PSN auth code for /oauth/token (sub_c83194). With the console offline from PSN
// these fail and the game shows NW-102261-2. The game's GOT slots for those imports are pointed at
// small stubs written into the unused tail of its last code page (build_t.stub_area). Only the
// game's own calls go through these slots, the system libraries are untouched.

// mov eax, value; ret
static size_t stub_return(code_t *c, uint32_t value) {
  size_t at = align16(c);
  if (value) {
    E(0xB8);
    e32(c, value);
  } else {
    E(0x31, 0xC0);
  }
  E(0xC3);
  return at;
}

// mov dword [rsi], value; xor eax, eax; ret
static size_t stub_out32(code_t *c, uint32_t value) {
  size_t at = align16(c);
  E(0xC7, 0x06);
  e32(c, value);
  E(0x31, 0xC0, 0xC3);
  return at;
}

// mov byte [rsi], value; xor eax, eax; ret
static size_t stub_out8(code_t *c, uint8_t value) {
  size_t at = align16(c);
  E(0xC6, 0x06, value, 0x31, 0xC0, 0xC3);
  return at;
}

// sceNpAuthGetAuthorizationCodeV3(req, param, code*, issuer*): write the code (into the buffer the
// caller zeroed) and the issuer id, report success.
static size_t stub_auth_code(code_t *c, const char code[16], uint32_t issuer) {
  size_t at = align16(c);
  E(0x48, 0xB8);
  emit(c, code, 8);
  E(0x48, 0x89, 0x02);  // mov [rdx], rax
  E(0x48, 0xB8);
  emit(c, code + 8, 8);
  E(0x48, 0x89, 0x42, 0x08);  // mov [rdx+8], rax
  E(0xC7, 0x01);
  e32(c, issuer);  // mov dword [rcx], issuer
  E(0x31, 0xC0, 0xC3);
  return at;
}

typedef enum { FILL_ONLINE_ID, FILL_ACCOUNT_ID, FILL_COUNTRY } fill_t;

// Calls the real function first (when its address is known); if that fails, writes the fallback
// value to *rsi and reports success.
static size_t stub_with_fallback(code_t *c, intptr_t real, fill_t fill, const void *value) {
  size_t at = align16(c);
  size_t jz = 0;
  if (real > 0) {
    E(0x56);  // push rsi (keeps the stack 16-byte aligned for the call)
    E(0x48, 0xB8);
    e64(c, (uint64_t)real);  // mov rax, real
    E(0xFF, 0xD0);           // call rax
    E(0x5E);                 // pop rsi
    E(0x85, 0xC0, 0x74, 0x00);
    jz = c->n - 1;  // jz done, patched below
  }
  const uint8_t *v = value;
  switch (fill) {
    case FILL_ONLINE_ID:  // SceNpOnlineId { char data[16]; char term; char dummy[3]; }
      E(0x48, 0xB8);
      emit(c, v, 8);
      E(0x48, 0x89, 0x06);
      E(0x48, 0xB8);
      emit(c, v + 8, 8);
      E(0x48, 0x89, 0x46, 0x08);
      E(0xC7, 0x46, 0x10);
      e32(c, 0);
      break;
    case FILL_ACCOUNT_ID:  // SceNpAccountId, uint64
      E(0x48, 0xB8);
      emit(c, v, 8);
      E(0x48, 0x89, 0x06);
      break;
    case FILL_COUNTRY:  // SceNpCountryCode { char data[2]; char term; char padding; }
      E(0xC7, 0x06);
      emit(c, v, 4);
      break;
  }
  E(0x31, 0xC0);
  if (jz) c->b[jz] = (uint8_t)(c->n - (jz + 1));
  E(0xC3);
  return at;
}

typedef struct {
  const char *name;  // the GOT slot is build_t.got[] at the same index
  size_t      stub;  // offset of the stub in the code buffer
} hook_t;

static hook_t g_hooks[NHOOKS] = {
    {"sceNpGetOnlineId", 0},
    {"sceNpGetAccountIdA", 0},
    {"sceNpGetAccountCountryA", 0},
    {"sceNpGetState", 0},
    {"sceNpHasSignedUp", 0},
    {"sceNpRegisterStateCallbackA", 0},
    {"sceNpCreateAsyncRequest", 0},
    {"sceNpCheckNpReachability", 0},
    {"sceNpPollAsync", 0},
    {"sceNpDeleteRequest", 0},
    {"sceNpAuthCreateAsyncRequest", 0},
    {"sceNpAuthGetAuthorizationCodeV3", 0},
    {"sceNpAuthPollAsync", 0},
    {"sceNpAuthDeleteRequest", 0},
    {"sceNpEntitlementAccessGetAddcontEntitlementInfoList", 0},  // only with add-ons to add
};
#define HOOK_ENTITLEMENTS 14

int sceUserServiceInitialize(void *params);
int sceUserServiceGetForegroundUser(int *user_id);
int sceUserServiceGetUserName(int user_id, char *name, size_t size);

// The identity used when the real NP calls fail: config values, else the PS5 user name.
static void psn_identity(char online_id[17], uint64_t *account_id, char country[4]) {
  memset(online_id, 0, 17);
  if (g_cfg.psn_online_id[0]) {
    snprintf(online_id, 17, "%s", g_cfg.psn_online_id);
  } else {
    int  user = -1;
    char name[64] = "";
    sceUserServiceInitialize(NULL);
    if (sceUserServiceGetForegroundUser(&user) == 0) sceUserServiceGetUserName(user, name, sizeof(name));
    size_t o = 0;
    for (const char *p = name; *p && o < 16; p++) {
      char ch = *p;
      if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '-' || ch == '_')
        online_id[o++] = ch;
    }
    if (o < 3) snprintf(online_id, 17, "Agent47");
  }
  *account_id = strtoull(g_cfg.psn_account_id, NULL, 10);
  if (!*account_id) {  // stable per online id: FNV-1a, kept positive as a signed 64-bit value
    uint64_t h = 1469598103934665603ULL;
    for (const char *p = online_id; *p; p++) h = (h ^ (uint8_t)*p) * 1099511628211ULL;
    *account_id = (h & 0x0fffffffffffffffULL) | 0x1000000000000000ULL;
  }
  memset(country, 0, 4);
  memcpy(country, g_cfg.psn_country, strlen(g_cfg.psn_country) < 2 ? strlen(g_cfg.psn_country) : 2);
}

static pid_t    g_hooked_pid = -1;
static intptr_t g_stub_addr[NHOOKS];
static size_t   g_stub_used;       // bytes of the stub area the PSN stubs use
static int      g_ent_state;       // 0 not tried, 1 installed, -1 gave up (fits or table failed)

// ---------------------------------------------------------------- add-on entitlements
// The game asks sceNpEntitlementAccessGetAddcontEntitlementInfoList which add-ons it owns and
// knows these labels (the table in sub_122fcd0: 23 in 01.017.000; 01.041.000 adds the last 11, in
// its own order, at 0x126fa6f). The system only lists add-ons installed for
// the running title, so add-ons installed for another HITMAN 3 title (PPSA01768 and the other
// SKUs the eboot names) are invisible to PPSA16387. The patcher looks for those under
// /user/addcont/<title>/<label>/ac.pkg and has the hook add them (x86stub.h).
static const char *const KNOWN_LABELS[] = {
    "H1LEGACYSTANDARD", "H1LEGACYEXPANSIO", "H2LEGACYSTANDARD", "H2LEGACYEXPANSIO", "00000H3PREORDERB",
    "HITMANDELXPACK03", "DLC0000000000020", "DLC0000000000021", "DLC0000000000022", "DLC0000000000023",
    "DLC0000000000024", "DLC0000000000025", "0000000007SINS01", "0000000007SINS02", "0000000007SINS03",
    "0000000007SINS04", "0000000007SINS05", "0000000007SINS06", "0000000007SINS07", "0000000007SINSSP",
    "H3TRILOGYPREMIUM", "000000000000EG01", "000000000000EG02",
    // 01.041.000
    "000000H3SAPIENZA", "00000000000VR2AC", "000000H3SARAJEVO", "00000H3ELUSIVE01", "00000H3ELUSIVE02",
    "00000H3ELUSIVE03", "00000H3ELUSIVE04", "00000H3ELUSIVE05", "00000H3ELUSIVE06", "00000H3ELUSIVE07",
    "00000H3ELUSIVE08",
    // client 8.25.0
    "000000000000EG03", "000000000000EG04", "00000H3ELUSIVE09", "00000H3ELUSIVE10",
};
#define NKNOWN (sizeof(KNOWN_LABELS) / sizeof(KNOWN_LABELS[0]))

// The stub's label table lives in unused, mapped, readable padding at the end of the RELRO
// segment (build_t.ent_table), because the code page is execute-only.
#define ENT_TABLE_MAX 64

typedef struct {
  char     magic[4];  // "HSEN"
  uint32_t count;
  uint8_t  entries[ENT_TABLE_MAX][ENT_ENTRY];
} ent_table_t;

static bool known_label(const char *s) {
  for (size_t i = 0; i < g_build->nlabels && i < NKNOWN; i++)
    if (!strcmp(s, KNOWN_LABELS[i])) return true;
  return false;
}

// Add-ons installed for the other titles in dlc_titles, as hook table entries. Returns the count.
static uint32_t collect_addons(ent_table_t *t, char *names, size_t names_len) {
  memset(t, 0, sizeof(*t));
  memcpy(t->magic, "HSEN", 4);
  names[0] = '\0';
  size_t no = 0;
  char titles[sizeof(g_cfg.dlc_titles)];
  snprintf(titles, sizeof(titles), "%s", g_cfg.dlc_titles);
  for (char *save = NULL, *title = strtok_r(titles, ", ", &save); title; title = strtok_r(NULL, ", ", &save)) {
    if (!strcmp(title, g_cfg.title_id) || strlen(title) != 9) continue;  // the system lists those itself
    char dir[64];
    snprintf(dir, sizeof(dir), "/user/addcont/%s", title);
    DIR *d = opendir(dir);
    if (!d) continue;
    for (struct dirent *e; (e = readdir(d)) != NULL;) {
      if (strlen(e->d_name) != 16 || !known_label(e->d_name)) continue;
      char pkg[128];
      struct stat st;
      snprintf(pkg, sizeof(pkg), "%s/%s/ac.pkg", dir, e->d_name);
      if (stat(pkg, &st) || !S_ISREG(st.st_mode)) continue;
      bool dup = false;
      for (uint32_t i = 0; i < t->count; i++)
        if (!memcmp(t->entries[i], e->d_name, 16)) dup = true;
      if (dup || t->count >= ENT_TABLE_MAX) continue;
      memcpy(t->entries[t->count++], e->d_name, 16);  // label + NUL; type and status 0
      if (no + 20 < names_len) no += (size_t)snprintf(names + no, names_len - no, "%s%s/%s", no ? " " : "", title, e->d_name);
    }
    closedir(d);
  }
  return t->count;
}

// Writes the table into the game's padding, after checking the padding is unused (zero) or holds
// an earlier table. Returns false when it is neither.
static bool write_ent_table(pid_t pid, intptr_t base, const ent_table_t *t) {
  uint8_t cur[sizeof(*t)];
  if (mem_read(pid, base + g_build->ent_table, cur, sizeof(cur))) {
    LOGE("patcher: add-on table area 0x%lx is not readable", (long)(base + g_build->ent_table));
    return false;
  }
  bool ours = !memcmp(cur, "HSEN", 4), zero = true;
  for (size_t i = 0; i < sizeof(cur) && zero; i++) zero = cur[i] == 0;
  if (!ours && !zero) {
    LOGE("patcher: add-on table area 0x%lx is in use, add-ons not added", (long)(base + g_build->ent_table));
    return false;
  }
  return mem_write(pid, base + g_build->ent_table, t, sizeof(*t)) == 0;
}

// Code writes go through the debugger path, which keeps the page executable. The PLT shares this
// page, so its protection must keep execute. XOM pages may not read back; a successful debug write
// is accepted then.
static int code_write(pid_t pid, intptr_t addr, const void *buf, size_t n) {
  if (mdbg_copyin(pid, buf, addr, n) != 0) {
    intptr_t page = addr & ~(intptr_t)(PAGE - 1);
    size_t   span = (size_t)(((addr + (intptr_t)n + PAGE - 1) & ~(intptr_t)(PAGE - 1)) - page);
    int      prot = kernel_get_vmem_protection(pid, page, span);
    if (prot <= 0 || !(prot & PROT_EXEC)) {
      LOGE("patcher: code page 0x%lx protection %d is not the expected executable mapping", (long)page, prot);
      return -1;
    }
    if (kernel_mprotect(pid, page, span, prot | PROT_READ | PROT_WRITE | PROT_EXEC)) return -1;
    int rc = kernel_proc_copyin(pid, buf, addr, n);
    if (kernel_mprotect(pid, page, span, prot)) {
      LOGE("patcher: could not restore protection %d on code page 0x%lx", prot, (long)page);
      rc = -1;
    }
    if (rc) return -1;
  }
  uint8_t back[STUB_AREA_LEN];
  if (n <= sizeof(back) && mem_read(pid, addr, back, n) == 0 && memcmp(back, buf, n)) return -1;
  return 0;
}

// The stubs go into the padding after the last function of the text segment. Only write there
// when it still is padding (zero or int3 bytes) or already holds these exact stubs; anything
// else means this is not the memory layout the addresses were taken from.
static bool stub_area_free(pid_t pid, intptr_t addr, const uint8_t *code, size_t n) {
  uint8_t cur[STUB_AREA_LEN];
  if (mem_read(pid, addr, cur, STUB_AREA_LEN)) return true;  // execute-only: cannot check, the debug write decides
  if (!memcmp(cur, code, n)) return true;
  // Written by an earlier hitman-server (a handoff reload, maybe with another identity): it
  // starts with the first stub's "push rsi; mov rax, imm64".
  if (n >= 3 && !memcmp(cur, code, 3) && code[0] == 0x56 && code[1] == 0x48 && code[2] == 0xB8) return true;
  for (size_t i = 0; i < STUB_AREA_LEN; i++)
    if (cur[i] != 0x00 && cur[i] != 0xCC) return false;
  return true;
}

// Writes the stubs once per game process and (re)points the GOT slots at them. Safe to call on
// every tick: a slot the dynamic linker rebinds is pointed back. Returns the number of slots set.
static int psn_hooks(pid_t pid, intptr_t base) {
  if (!g_cfg.psn_bypass) return 0;
  if (g_hooked_pid != pid) {
    char     online_id[17], country[4];
    uint64_t account_id;
    psn_identity(online_id, &account_id, country);

    uint32_t np = 0;
    bool     have_np = kernel_dynlib_handle(pid, "libSceNpManager.sprx", &np) == 0;
    intptr_t real_online = have_np ? kernel_dynlib_dlsym(pid, np, "sceNpGetOnlineId") : 0;
    intptr_t real_account = have_np ? kernel_dynlib_dlsym(pid, np, "sceNpGetAccountIdA") : 0;
    intptr_t real_country = have_np ? kernel_dynlib_dlsym(pid, np, "sceNpGetAccountCountryA") : 0;
    LOGI("patcher: PSN bypass, fallback identity %s / %llu / %s; libSceNpManager %s, real getters %lx %lx %lx",
         online_id, (unsigned long long)account_id, country, have_np ? "found" : "not found", (long)real_online,
         (long)real_account, (long)real_country);

    static code_t code;
    memset(&code, 0, sizeof(code));
    code_t *c = &code;
    static const char auth[16] = {'h', 'i', 't', 'm', 'a', 'n', '-', 'p', 's', '5', '-', 'l', 'o', 'c', 'a', 'l'};
    g_hooks[0].stub = stub_with_fallback(c, real_online, FILL_ONLINE_ID, online_id);
    g_hooks[1].stub = stub_with_fallback(c, real_account, FILL_ACCOUNT_ID, &account_id);
    g_hooks[2].stub = stub_with_fallback(c, real_country, FILL_COUNTRY, country);
    g_hooks[3].stub = stub_out32(c, 2);      // SCE_NP_STATE_SIGNED_IN
    g_hooks[4].stub = stub_out8(c, 1);       // has signed up
    g_hooks[5].stub = stub_return(c, 0);     // no state callback, so nothing reports "signed out"
    g_hooks[6].stub = stub_return(c, 0x1);   // request id
    g_hooks[7].stub = stub_return(c, 0);     // reachability started
    g_hooks[8].stub = stub_out32(c, 0);      // finished, result 0 (reachable)
    g_hooks[9].stub = stub_return(c, 0);     // delete request
    g_hooks[10].stub = stub_return(c, 0x1);  // auth request id
    g_hooks[11].stub = stub_auth_code(c, auth, 256);
    g_hooks[12].stub = stub_out32(c, 0);  // auth poll finished, result 0
    g_hooks[13].stub = stub_return(c, 0);

    if (code.n >= STUB_AREA_LEN) {
      LOGE("patcher: PSN stubs do not fit (%zu bytes)", code.n);
      return -1;
    }
    if (!stub_area_free(pid, base + g_build->stub_area, code.b, code.n)) {
      LOGE("patcher: the stub area at 0x%lx is not empty padding, PSN bypass not installed",
           (long)(base + g_build->stub_area));
      return -1;
    }
    if (code_write(pid, base + g_build->stub_area, code.b, code.n)) {
      LOGE("patcher: writing PSN stubs failed");
      return -1;
    }
    for (size_t i = 0; i < NHOOKS; i++) g_stub_addr[i] = base + g_build->stub_area + (intptr_t)g_hooks[i].stub;
    g_stub_addr[HOOK_ENTITLEMENTS] = 0;  // installed later, once the game binds the real function
    g_stub_used = code.n;
    g_ent_state = 0;  // the entitlement hook is installed later, once the game binds that function
    g_hooked_pid = pid;
    LOGI("patcher: %zu bytes of PSN stubs at 0x%lx", code.n, (long)(base + g_build->stub_area));
  }

  int set = 0;
  for (size_t i = 0; i < NHOOKS; i++) {
    if (!g_stub_addr[i]) continue;
    intptr_t slot = base + g_build->got[i], now = 0;
    if (mem_read(pid, slot, &now, sizeof(now))) continue;
    if (now == g_stub_addr[i]) continue;
    if (mem_write(pid, slot, &g_stub_addr[i], sizeof(intptr_t)) == 0) {
      set++;
      // A slot still pointing into the eboot is unresolved (lazy binding): the game has not called
      // it yet. One pointing into a library means it was called before the hook.
      bool early = now >= base && now < base + g_build->stub_area;
      LOGI("patcher: %s -> stub (was 0x%lx, %s)", g_hooks[i].name, (long)now,
           early ? "not called yet" : "already called before the hook");
    } else {
      LOGE("patcher: hooking %s failed", g_hooks[i].name);
    }
  }
  return set;
}

// Installs the add-on entitlement hook once the game has bound the real function (its GOT slot
// points into a library, not the eboot's lazy stub). Called on each tick until it succeeds or is
// ruled out, because the game binds that function late (ENTITLEMENTS_SYNCHRONIZE), after the PSN
// stubs go in. The entitlement stub is written after the PSN stubs in the same padding.

static void entitlement_hook(pid_t pid, intptr_t base) {
  if (!g_cfg.psn_bypass || !g_cfg.dlc_titles[0] || g_ent_state != 0) return;

  intptr_t slot = base + g_build->got[HOOK_ENTITLEMENTS], real = 0;
  if (mem_read(pid, slot, &real, sizeof(real))) return;
  // A library address (outside the eboot) means the function is bound; an in-eboot address is the
  // lazy stub, bound once the game first calls it (ENTITLEMENTS_SYNCHRONIZE). Log the wait once.
  static bool logged_wait;
  if (real == 0 || (real >= base && real < base + g_build->span)) {
    if (!logged_wait) {
      LOGD("patcher: entitlement slot 0x%lx not bound yet (0x%lx), waiting", (long)slot, (long)real);
      logged_wait = true;
    }
    return;
  }

  ent_table_t table;
  char        names[512];
  if (!collect_addons(&table, names, sizeof(names))) {
    LOGI("patcher: no add-ons installed for the titles in dlc_titles (%s)", g_cfg.dlc_titles);
    g_ent_state = -1;
    return;
  }
  if (!write_ent_table(pid, base, &table)) {
    g_ent_state = -1;
    return;
  }

  static code_t ent;
  memset(&ent, 0, sizeof(ent));
  size_t off = stub_entitlements(&ent, (uint64_t)real, (uint64_t)(base + g_build->ent_table));
  if (g_stub_used + ent.n > STUB_AREA_LEN) {
    LOGE("patcher: entitlement stub does not fit (%zu + %zu bytes)", g_stub_used, ent.n);
    g_ent_state = -1;
    return;
  }
  intptr_t at = base + g_build->stub_area + (intptr_t)g_stub_used;
  if (code_write(pid, at, ent.b, ent.n)) {
    LOGE("patcher: writing the entitlement stub failed");
    g_ent_state = -1;
    return;
  }
  intptr_t stub = at + (intptr_t)off;
  if (mem_write(pid, slot, &stub, sizeof(stub))) {
    LOGE("patcher: pointing the entitlement slot at the stub failed");
    g_ent_state = -1;
    return;
  }
  g_stub_used += ent.n;
  g_stub_addr[HOOK_ENTITLEMENTS] = stub;  // psn_hooks re-points it if the game rebinds the slot
  g_ent_state = 1;
  LOGI("patcher: %u add-on(s) from other titles listed to the game (real fn 0x%lx): %s", table.count, (long)real,
       names);
}

typedef enum { PATCH_OK, PATCH_RETRY, PATCH_WRONG_BUILD, PATCH_GONE } patch_rc_t;

static void build_patches(const build_t *b, patch_t p[3]) {
  patch_t def[3] = {
      {"config scheme", b->scheme, "https://{0}", "http://{0}"},
      {"auth fallback host", b->auth_host, "https://auth.hitman.io", ""},
      {"default config domain", b->domain_def, "config.hitman.io", ""},
  };
  snprintf(def[1].repl, sizeof(def[1].repl), "http://%s", g_cfg.patch_domain);
  snprintf(def[2].repl, sizeof(def[2].repl), "%s", g_cfg.patch_domain);
  memcpy(p, def, sizeof(def));
}

// Patches pid. PATCH_OK sets *base_out and g_build; PATCH_WRONG_BUILD is final for this process;
// PATCH_RETRY means it did not complete in time and may be tried again.
static patch_rc_t patch_process(pid_t pid, intptr_t *base_out) {
  patch_t patches[3];

  LOGI("patcher: %s is running as pid %d", g_cfg.title_id, pid);
  set_status("patching pid %d", pid);
  intptr_t base = 0;
  bool     rodata_done = false;
  for (long waited = 0; waited < GIVE_UP_US; waited += POLL_US, usleep(POLL_US)) {
    if (waited % 1000000 == 0 && waited && !still_running(pid)) {
      LOGW("patcher: pid %d exited before it was patched", pid);
      set_status("pid %d exited before it was patched", pid);
      return PATCH_GONE;
    }
    if (!base) {
      base = kernel_dynlib_mapbase_addr(pid, 0);
      if (base <= 0) { base = 0; continue; }
      LOGI("patcher: eboot base 0x%lx", (long)base);
    }

    if (!rodata_done) {
      // The build is the one whose three strings are all original or already patched. Until one
      // build's sites are all readable, rodata is not mapped yet.
      int            state[3];
      const build_t *match = NULL;
      bool           readable = false;
      for (size_t b = 0; b < NBUILDS && !match; b++) {
        patch_t p[3];
        build_patches(&BUILDS[b], p);
        bool all_read = true, ours = true;
        for (int i = 0; i < 3; i++) {
          state[i] = check(pid, base, &p[i]);
          if (state[i] == -1) all_read = false;
          if (state[i] < 0) ours = false;
        }
        readable |= all_read;
        if (ours) {
          match = &BUILDS[b];
          memcpy(patches, p, sizeof(p));
        }
      }
      if (!match && !readable) continue;
      if (!match) {
        char known[128] = "";
        for (size_t b = 0, o = 0; b < NBUILDS && o < sizeof(known); b++)
          o += (size_t)snprintf(known + o, sizeof(known) - o, "%s%s", b ? ", " : "", BUILDS[b].version);
        LOGE("patcher: the game strings are not where any known build has them (%s), so nothing was patched",
             known);
        notify("HITMAN local server: unknown game version, not patched");
        set_status("pid %d: unknown game build (known: %s), not patched", pid, known);
        return PATCH_WRONG_BUILD;
      }
      if (g_build != match) LOGI("patcher: game build %s", match->version);
      g_build = match;
      // A write can fail transiently when the page is not yet committed. Retry on the next poll
      // rather than aborting; a stuck target shows up as the "gave up" timeout below.
      bool all_written = true;
      for (int i = 0; i < 3; i++) {
        if (state[i] == 1) continue;
        if (write_patch(pid, base, &patches[i])) {
          set_status("pid %d: writing %s not ready, retrying", pid, patches[i].what);
          all_written = false;
          break;
        }
        LOGI("patcher: %s -> %s", patches[i].what, patches[i].repl);
      }
      if (!all_written) continue;
      if (psn_hooks(pid, base) < 0) notify("HITMAN local server: PSN bypass failed, see server.log");
      rodata_done = true;
    }

    // online_VersionConfigDomain is filled from the default during static init. If init ran
    // before the default was patched, it still holds "config.hitman.io": overwrite it.
    char cur[17];
    if (mem_read(pid, base + g_build->domain_var, cur, sizeof(cur))) continue;
    size_t dn = strlen(g_cfg.patch_domain) + 1;
    if (!memcmp(cur, g_cfg.patch_domain, dn)) {
      LOGI("patcher: online_VersionConfigDomain = %s", g_cfg.patch_domain);
    } else if (!memcmp(cur, "config.hitman.io", 17)) {
      if (mem_write(pid, base + g_build->domain_var, g_cfg.patch_domain, dn)) {
        set_status("pid %d: writing online_VersionConfigDomain not ready, retrying", pid);
        continue;  // retry on the next poll
      }
      LOGI("patcher: online_VersionConfigDomain config.hitman.io -> %s", g_cfg.patch_domain);
    } else if (cur[0] == '\0') {
      continue;  // static init has not run yet
    } else {
      cur[16] = '\0';
      LOGW("patcher: online_VersionConfigDomain is already \"%s\" (set elsewhere), leaving it", cur);
    }
    // OnlineResources_Disable: its static init stores the name pointer, then the default value.
    if (g_build->res_disable) {
      uint64_t name = 0;
      int32_t  val = 0, zero = 0;
      if (mem_read(pid, base + g_build->res_disable, &name, sizeof(name)) || !name) continue;
      if (mem_read(pid, base + g_build->res_disable + 0x18, &val, sizeof(val))) continue;
      if (val) {
        if (mem_write(pid, base + g_build->res_disable + 0x18, &zero, sizeof(zero))) {
          set_status("pid %d: writing OnlineResources_Disable not ready, retrying", pid);
          continue;
        }
        LOGI("patcher: OnlineResources_Disable %d -> 0", val);
      }
    }
    notify("HITMAN 3 online -> local server (%s)", g_cfg.patch_domain);
    set_status("pid %d patched, game uses http://%s", pid, g_cfg.patch_domain);
    *base_out = base;
    return PATCH_OK;
  }
  LOGW("patcher: pid %d not patched within %d s (online_VersionConfigDomain never initialised or a write kept "
       "failing)", pid, GIVE_UP_US / 1000000);
  set_status("pid %d: not patched within %d s", pid, GIVE_UP_US / 1000000);
  return PATCH_RETRY;
}

// Re-checks the three string sites of a patched process. A site back at its original bytes is
// written again; anything else is only reported.
static void verify_patch(pid_t pid, intptr_t base) {
  patch_t patches[3];
  build_patches(g_build, patches);
  for (int i = 0; i < 3; i++) {
    int st = check(pid, base, &patches[i]);
    if (st == 0) {
      LOGW("patcher: %s was back to its original bytes, writing it again", patches[i].what);
      if (write_patch(pid, base, &patches[i])) LOGE("patcher: rewriting %s failed", patches[i].what);
    } else if (st == -2) {
      LOGE("patcher: %s holds unexpected bytes after patching", patches[i].what);
    }
  }
}

#define MAX_ATTEMPTS 3

static void *patcher_main(void *arg) {
  (void)arg;
  pid_t    handled = -1;
  intptr_t base = 0;
  int      last_state = -1000, attempts = 0, ticks = 0;
  bool     final = false;
  for (;;) {
    pid_t pid = find_game();
    if (pid <= 0) {
      handled = -1;
      base = 0;
      usleep(500 * 1000);
      continue;
    }
    if (pid != handled) {  // a new game process
      handled = pid;
      base = 0;
      attempts = 0;
      final = false;
      last_state = -1000;
    }
    // A patch that did not complete is tried again (a few times, a few seconds apart) instead
    // of leaving this process unpatched until the game restarts. A wrong build is never retried.
    if (!base && !final && attempts < MAX_ATTEMPTS) {
      attempts++;
      patch_rc_t rc = patch_process(pid, &base);
      if (rc == PATCH_WRONG_BUILD || rc == PATCH_GONE) {
        final = true;
      } else if (rc == PATCH_RETRY) {
        if (attempts < MAX_ATTEMPTS) {
          LOGW("patcher: attempt %d of %d on pid %d did not complete, retrying in 5 s", attempts, MAX_ATTEMPTS, pid);
          usleep(5 * 1000 * 1000);
        } else {
          LOGE("patcher: pid %d not patched after %d attempts; restart the game to try again", pid, MAX_ATTEMPTS);
          notify("HITMAN local server: game not patched, see server.log");
          set_status("pid %d: not patched after %d attempts", pid, MAX_ATTEMPTS);
        }
      }
    }
    if (base) {
      psn_hooks(pid, base);
      entitlement_hook(pid, base);  // installs once the game binds the entitlement function
      if (++ticks % 50 == 0) verify_patch(pid, base);  // every ~5 s
    }
    // Follow the game's connection state so the log shows how far the online flow gets.
    int32_t state;
    if (base && mem_read(pid, base + g_build->online_state, &state, sizeof(state)) == 0 && state != last_state) {
      // Re-read what the config URL is built from, in case the game rewrote it after the patch.
      char scheme[12] = {0}, domain[33] = {0};
      mem_read(pid, base + g_build->scheme, scheme, sizeof(scheme) - 1);
      mem_read(pid, base + g_build->domain_var, domain, sizeof(domain) - 1);
      LOGI("patcher: game online state %d (%s), url base \"%s\" with \"%s\"", state, state_name(state), scheme,
           domain);
      set_status("pid %d patched for http://%s; online state %d (%s)", pid, g_cfg.patch_domain, state,
                 state_name(state));
      last_state = state;
    }
    usleep(100 * 1000);
  }
  return NULL;
}

void patcher_start(void) {
  pthread_t th;
  if (pthread_create(&th, NULL, patcher_main, NULL) == 0) {
    pthread_detach(th);
    LOGI("patcher: watching for %s (domain %s)", g_cfg.title_id, g_cfg.patch_domain);
  } else {
    LOGE("patcher: cannot start thread");
  }
}

#endif
