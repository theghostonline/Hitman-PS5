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

#include <ps5/kernel.h>
#include <ps5/mdbg.h>

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

// HITMAN 3 PPSA16387 01.017.000, eboot.bin sha256 4a7aad32...5bed. Eboot virtual addresses.
#define VA_SCHEME      0x1c88dc5
#define VA_AUTH_HOST   0x1c558a0
#define VA_DOMAIN_DEF  0x1c240d4
#define VA_DOMAIN_VAR  0x3c6b510
#define VA_ONLINE_STATE 0x3c6b7d8  // ZOnlineManager+0x18, the connection state the gate in 2 checks

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
// small stubs written into the unused tail of its last code page. Only the game's own calls go
// through these slots, the system libraries are untouched.
#define VA_STUB_AREA  0x1c03ce0  // text ends at 0x1c03cdc, the code page runs to 0x1c04000
#define STUB_AREA_LEN 0x320

typedef struct {
  uint8_t b[STUB_AREA_LEN];
  size_t  n;
} code_t;

static void emit(code_t *c, const void *p, size_t n) {
  if (c->n + n > sizeof(c->b)) {
    c->n = sizeof(c->b) + 1;  // force the fit check to fail
    return;
  }
  memcpy(c->b + c->n, p, n);
  c->n += n;
}
#define E(...)                          \
  do {                                  \
    const uint8_t b_[] = {__VA_ARGS__}; \
    emit(c, b_, sizeof(b_));            \
  } while (0)
static void e32(code_t *c, uint32_t v) { emit(c, &v, 4); }
static void e64(code_t *c, uint64_t v) { emit(c, &v, 8); }
static size_t align16(code_t *c) {
  while (c->n % 16) E(0xCC);
  return c->n;
}

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
  const char *name;
  intptr_t    got;   // eboot VA of the GOT slot (from the PLT stub's jmp [rip+x])
  size_t      stub;  // offset of the stub in the code buffer
} hook_t;

static hook_t g_hooks[] = {
    {"sceNpGetOnlineId", 0x25645c0, 0},
    {"sceNpGetAccountIdA", 0x25645c8, 0},
    {"sceNpGetAccountCountryA", 0x25645d0, 0},
    {"sceNpGetState", 0x2564a28, 0},
    {"sceNpHasSignedUp", 0x2564968, 0},
    {"sceNpRegisterStateCallbackA", 0x2564888, 0},
    {"sceNpCreateAsyncRequest", 0x2564600, 0},
    {"sceNpCheckNpReachability", 0x2564608, 0},
    {"sceNpPollAsync", 0x25645f8, 0},
    {"sceNpDeleteRequest", 0x25645f0, 0},
    {"sceNpAuthCreateAsyncRequest", 0x2564648, 0},
    {"sceNpAuthGetAuthorizationCodeV3", 0x2564650, 0},
    {"sceNpAuthPollAsync", 0x2564658, 0},
    {"sceNpAuthDeleteRequest", 0x2564660, 0},
};
#define NHOOKS (sizeof(g_hooks) / sizeof(g_hooks[0]))

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
    if (!stub_area_free(pid, base + VA_STUB_AREA, code.b, code.n)) {
      LOGE("patcher: the stub area at 0x%lx is not empty padding, PSN bypass not installed", (long)(base + VA_STUB_AREA));
      return -1;
    }
    if (code_write(pid, base + VA_STUB_AREA, code.b, code.n)) {
      LOGE("patcher: writing PSN stubs failed");
      return -1;
    }
    for (size_t i = 0; i < NHOOKS; i++) g_stub_addr[i] = base + VA_STUB_AREA + (intptr_t)g_hooks[i].stub;
    g_hooked_pid = pid;
    LOGI("patcher: %zu bytes of PSN stubs at 0x%lx", code.n, (long)(base + VA_STUB_AREA));
  }

  int set = 0;
  for (size_t i = 0; i < NHOOKS; i++) {
    intptr_t slot = base + g_hooks[i].got, now = 0;
    if (mem_read(pid, slot, &now, sizeof(now))) continue;
    if (now == g_stub_addr[i]) continue;
    if (mem_write(pid, slot, &g_stub_addr[i], sizeof(intptr_t)) == 0) {
      set++;
      LOGI("patcher: %s -> stub (was 0x%lx)", g_hooks[i].name, (long)now);
    } else {
      LOGE("patcher: hooking %s failed", g_hooks[i].name);
    }
  }
  return set;
}

typedef enum { PATCH_OK, PATCH_RETRY, PATCH_WRONG_BUILD, PATCH_GONE } patch_rc_t;

// Patches pid. PATCH_OK sets *base_out; PATCH_WRONG_BUILD is final for this process;
// PATCH_RETRY means it did not complete in time and may be tried again.
static patch_rc_t patch_process(pid_t pid, intptr_t *base_out) {
  patch_t patches[3] = {
      {"config scheme", VA_SCHEME, "https://{0}", "http://{0}"},
      {"auth fallback host", VA_AUTH_HOST, "https://auth.hitman.io", ""},
      {"default config domain", VA_DOMAIN_DEF, "config.hitman.io", ""},
  };
  snprintf(patches[1].repl, sizeof(patches[1].repl), "http://%s", g_cfg.patch_domain);
  snprintf(patches[2].repl, sizeof(patches[2].repl), "%s", g_cfg.patch_domain);

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
      int state[3];
      bool ready = true;
      for (int i = 0; i < 3; i++) {
        state[i] = check(pid, base, &patches[i]);
        if (state[i] == -1) ready = false;
        if (state[i] == -2) {
          LOGE("patcher: unexpected bytes for %s at 0x%lx. This is not the 01.017.000 eboot the server was "
               "built for, so nothing was patched", patches[i].what, (long)patches[i].va);
          notify("HITMAN local server: unknown game version, not patched");
          set_status("pid %d: unexpected bytes for %s, not patched", pid, patches[i].what);
          return PATCH_WRONG_BUILD;
        }
      }
      if (!ready) continue;
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
    if (mem_read(pid, base + VA_DOMAIN_VAR, cur, sizeof(cur))) continue;
    size_t dn = strlen(g_cfg.patch_domain) + 1;
    if (!memcmp(cur, g_cfg.patch_domain, dn)) {
      LOGI("patcher: online_VersionConfigDomain = %s", g_cfg.patch_domain);
    } else if (!memcmp(cur, "config.hitman.io", 17)) {
      if (mem_write(pid, base + VA_DOMAIN_VAR, g_cfg.patch_domain, dn)) {
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
  patch_t patches[3] = {
      {"config scheme", VA_SCHEME, "https://{0}", "http://{0}"},
      {"auth fallback host", VA_AUTH_HOST, "https://auth.hitman.io", ""},
      {"default config domain", VA_DOMAIN_DEF, "config.hitman.io", ""},
  };
  snprintf(patches[1].repl, sizeof(patches[1].repl), "http://%s", g_cfg.patch_domain);
  snprintf(patches[2].repl, sizeof(patches[2].repl), "%s", g_cfg.patch_domain);
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
      if (++ticks % 50 == 0) verify_patch(pid, base);  // every ~5 s
    }
    // Follow the game's connection state so the log shows how far the online flow gets.
    int32_t state;
    if (base && mem_read(pid, base + VA_ONLINE_STATE, &state, sizeof(state)) == 0 && state != last_state) {
      // Re-read what the config URL is built from, in case the game rewrote it after the patch.
      char scheme[12] = {0}, domain[33] = {0};
      mem_read(pid, base + VA_SCHEME, scheme, sizeof(scheme) - 1);
      mem_read(pid, base + VA_DOMAIN_VAR, domain, sizeof(domain) - 1);
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
