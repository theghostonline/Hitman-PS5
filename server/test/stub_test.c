// Runs the add-on entitlement stub (src/x86stub.h) on the host against a fake system function.
// x86-64 only: on Apple silicon, build with -arch x86_64 and it runs under Rosetta.
//   make stubtest
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "x86stub.h"

#if !defined(__x86_64__)
#error "build with -arch x86_64"
#endif

typedef int (*list_fn)(uint32_t label, uint8_t *list, uint32_t list_num, uint32_t *hit_num);

static int passed;
#define CHECK(cond, what)                                    \
  do {                                                       \
    if (!(cond)) {                                           \
      fprintf(stderr, "FAIL %s (line %d)\n", what, __LINE__); \
      exit(1);                                               \
    }                                                        \
    passed++;                                                \
    printf("  ok  %s\n", what);                              \
  } while (0)

// ---------------------------------------------------------------- the fake system function
static const char *g_real[80];  // labels it owns
static size_t      g_nreal;
static int         g_fail;      // return this error instead
static int         g_calls, g_misaligned;
static uint32_t    g_seen_label;
static uintptr_t   g_entry_sp;  // rsp captured at the trampoline, before any prologue

__attribute__((noinline)) static int fake_real(uint32_t label, uint8_t *list, uint32_t list_num, uint32_t *hit_num) {
  g_calls++;
  g_seen_label = label;
  // At a correctly aligned call site rsp is 8 (mod 16) right after the return-address push, which
  // the naked trampoline recorded before this function's prologue ran.
  if (g_entry_sp % 16 != 8) g_misaligned++;
  if (g_fail) return g_fail;
  uint32_t n = 0;
  for (size_t i = 0; i < g_nreal; i++) {
    if (list && n < list_num) {
      memset(list + n * ENT_ENTRY, 0, ENT_ENTRY);
      memcpy(list + n * ENT_ENTRY, g_real[i], 16);
      uint32_t type = 2, status = 3;
      memcpy(list + n * ENT_ENTRY + 20, &type, 4);
      memcpy(list + n * ENT_ENTRY + 24, &status, 4);
    }
    if (!list || n < list_num) n++;
  }
  *hit_num = n;
  return 0;
}

// Records rsp exactly as the stub's `call` leaves it, then tail-calls fake_real unchanged.
__attribute__((naked)) static int real_trampoline(uint32_t label, uint8_t *list, uint32_t list_num,
                                                   uint32_t *hit_num) {
  __asm__ volatile("mov %%rsp, %0\n\t"
                   "jmp %P1\n"
                   : "=m"(g_entry_sp)
                   : "i"(fake_real));
}

// ---------------------------------------------------------------- harness
static uint8_t g_table[8 + 64 * ENT_ENTRY];

static void set_table(const char *const *labels, size_t n) {
  memset(g_table, 0, sizeof(g_table));
  memcpy(g_table, "HSEN", 4);
  uint32_t c = (uint32_t)n;
  memcpy(g_table + 4, &c, 4);
  for (size_t i = 0; i < n; i++) memcpy(g_table + 8 + i * ENT_ENTRY, labels[i], 16);
}

static list_fn build(int with_real) {
  static code_t code;
  memset(&code, 0, sizeof(code));
  code_t *c = &code;
  E(0x90);  // something before it, like the other stubs
  size_t at = stub_entitlements(c, with_real ? (uint64_t)(uintptr_t)real_trampoline : 0, (uint64_t)(uintptr_t)g_table);
  if (code.n >= STUB_AREA_LEN) {
    fprintf(stderr, "stub does not fit: %zu bytes\n", code.n);
    exit(1);
  }
  void *mem = mmap(NULL, 0x4000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
  memcpy(mem, code.b, code.n);
  mprotect(mem, 0x4000, PROT_READ | PROT_EXEC);
  printf("  (stub %zu bytes, %zu total)\n", code.n - at, code.n);
  return (list_fn)((uint8_t *)mem + at);
}

// Calls the stub with known values in the callee-saved registers and reports whether it kept them.
static int call_checked(list_fn f, uint32_t label, uint8_t *list, uint32_t list_num, uint32_t *hit, int *regs_ok) {
  int rc;
  uint64_t bad = 0;
  // rbp is clobbered while the callee runs, so nothing may touch rbp-relative memory in between:
  // the mismatch flags accumulate in rbx (which doubles as the first test value) and are written
  // out only after rbp is restored.
  __asm__ volatile(
      "push %%rbx; push %%r12; push %%r13; push %%r14; push %%r15; push %%rbp\n"
      "mov $0x1111, %%rbx; mov $0x1212, %%r12; mov $0x1313, %%r13\n"
      "mov $0x1414, %%r14; mov $0x1515, %%r15; mov $0x1616, %%rbp\n"
      "call *%%rax\n"
      "xor %%r8, %%r8\n"
      "cmp $0x1111, %%rbx; setne %%r8b\n"
      "cmp $0x1212, %%r12; setne %%r9b; or %%r9b, %%r8b\n"
      "cmp $0x1313, %%r13; setne %%r9b; or %%r9b, %%r8b\n"
      "cmp $0x1414, %%r14; setne %%r9b; or %%r9b, %%r8b\n"
      "cmp $0x1515, %%r15; setne %%r9b; or %%r9b, %%r8b\n"
      "cmp $0x1616, %%rbp; setne %%r9b; or %%r9b, %%r8b\n"
      "pop %%rbp; pop %%r15; pop %%r14; pop %%r13; pop %%r12; pop %%rbx\n"
      "mov %%r8, %[bad]\n"
      : "=a"(rc), [bad] "=r"(bad)
      : "a"(f), "D"(label), "S"(list), "d"(list_num), "c"(hit)
      : "r8", "r9", "r10", "r11", "memory", "cc");
  *regs_ok = bad == 0;
  return rc;
}

static int has(const uint8_t *list, uint32_t n, const char *label) {
  for (uint32_t i = 0; i < n; i++)
    if (!memcmp(list + i * ENT_ENTRY, label, 17)) return 1;
  return 0;
}

int main(void) {
  static const char *const REAL[] = {"DLC0000000000020", "H1LEGACYSTANDARD", "H2LEGACYSTANDARD"};
  static const char *const EXTRA[] = {"H1LEGACYSTANDARD", "0000000007SINS01", "H3TRILOGYPREMIUM", "000000000000EG01"};
  setvbuf(stdout, NULL, _IONBF, 0);
  uint8_t  list[100 * ENT_ENTRY];
  uint32_t hit;
  int      regs_ok;

  printf("real list plus table\n");
  memcpy(g_real, REAL, sizeof(REAL));
  g_nreal = 3;
  set_table(EXTRA, 4);
  list_fn f = build(1);
  hit = 0xdead;
  int rc = call_checked(f, 0x77, NULL, 0, &hit, &regs_ok);
  CHECK(rc == 0 && hit == 6, "count query: 3 real + 3 new (the duplicate is not counted twice)");
  CHECK(regs_ok, "callee-saved registers kept");
  CHECK(g_seen_label == 0x77, "real function got the service label");
  CHECK(!g_misaligned, "stack aligned at the call");
  memset(list, 0xAB, sizeof(list));
  hit = 0;
  rc = call_checked(f, 0x77, list, 6, &hit, &regs_ok);
  CHECK(rc == 0 && hit == 6 && regs_ok, "list query returns the same count");
  CHECK(!memcmp(list, REAL[0], 17) && !memcmp(list + ENT_ENTRY, REAL[1], 17) && !memcmp(list + 2 * ENT_ENTRY, REAL[2], 17),
        "real entries first, in order");
  CHECK(has(list, 6, "0000000007SINS01") && has(list, 6, "H3TRILOGYPREMIUM") && has(list, 6, "000000000000EG01"),
        "installed add-ons from the table appended");
  uint32_t type, status;
  memcpy(&type, list + 3 * ENT_ENTRY + 20, 4);
  memcpy(&status, list + 3 * ENT_ENTRY + 24, 4);
  CHECK(type == 2 && status == 3, "appended entries copy the first real entry's type and status");
  CHECK(list[6 * ENT_ENTRY] == 0xAB, "nothing written past the entries");

  memset(list, 0xAB, sizeof(list));
  rc = call_checked(f, 0x77, list, 2, &hit, &regs_ok);
  CHECK(rc == 0 && hit == 2 && list[2 * ENT_ENTRY] == 0xAB, "a short buffer gets only what fits");
  rc = call_checked(f, 0x77, list, 6, NULL, &regs_ok);
  CHECK(rc == 0 && regs_ok, "no hitNum pointer: no crash");

  printf("system call fails\n");
  g_fail = (int)0x80550001;
  rc = call_checked(f, 0x77, NULL, 0, &hit, &regs_ok);
  CHECK(rc == 0 && hit == 4, "still succeeds, with the table's add-ons only (the game traps on failure)");
  rc = call_checked(f, 0x77, list, 4, &hit, &regs_ok);
  memcpy(&type, list + 20, 4);
  CHECK(rc == 0 && hit == 4 && has(list, 4, "H1LEGACYSTANDARD") && type == 0, "table entries with type 0");
  g_fail = 0;

  printf("no real function address\n");
  g_calls = 0;
  list_fn g = build(0);
  rc = call_checked(g, 0x77, NULL, 0, &hit, &regs_ok);
  CHECK(rc == 0 && hit == 4 && g_calls == 0 && regs_ok, "table only, real never called");

  printf("limits\n");
  static char many[70][17];
  for (int i = 0; i < 70; i++) snprintf(many[i], 17, "REALLABEL%07d", i);
  for (int i = 0; i < 70; i++) g_real[i] = many[i];
  g_nreal = 70;
  rc = call_checked(f, 0x77, NULL, 0, &hit, &regs_ok);
  CHECK(rc == 0 && hit == 64 + 4, "real list capped at 64, table still added");
  rc = call_checked(f, 0x77, list, 100, &hit, &regs_ok);
  CHECK(rc == 0 && hit == 68 && regs_ok, "list query agrees");
  set_table(NULL, 0);
  g_nreal = 3;
  rc = call_checked(f, 0x77, NULL, 0, &hit, &regs_ok);
  CHECK(rc == 0 && hit == 3, "empty table: the real list unchanged");

  printf("\n%d checks passed\n", passed);
  return 0;
}
