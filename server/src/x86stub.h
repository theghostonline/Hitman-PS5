// x86-64 code emitter for the hooks the patcher writes into the game, and the add-on entitlement
// stub. Header-only so test/stub_test.c can run the generated code on the host (x86-64, or
// Rosetta on Apple silicon) against a fake system function.
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define STUB_AREA_LEN 0x320

typedef struct {
  uint8_t b[STUB_AREA_LEN];
  size_t  n;
} code_t;

static inline void emit(code_t *c, const void *p, size_t n) {
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
static inline void e32(code_t *c, uint32_t v) { emit(c, &v, 4); }
static inline void e64(code_t *c, uint64_t v) { emit(c, &v, 8); }
static inline size_t align16(code_t *c) {
  while (c->n % 16) E(0xCC);
  return c->n;
}

// Short jumps: jcc(c, op) emits "op 00" and returns the offset to patch; land(c, at) points it at
// the current position.
static inline size_t jcc(code_t *c, uint8_t op) {
  E(op, 0x00);
  return c->n - 1;
}
static inline void land(code_t *c, size_t at) {
  if (at < c->n && c->n - (at + 1) < 0x80) c->b[at] = (uint8_t)(c->n - (at + 1));
  else c->n = sizeof(c->b) + 1;  // out of rel8 range: fail the fit check
}
static inline void jback(code_t *c, uint8_t op, size_t target) {
  E(op, (uint8_t)(target - (c->n + 2)));
}

// ---------------------------------------------------------------- add-on entitlements
//
// int sceNpEntitlementAccessGetAddcontEntitlementInfoList(SceNpServiceLabel label,
//     SceNpEntitlementAccessAddcontEntitlementInfo *list, uint32_t listNum, uint32_t *hitNum)
// Entries are 28 bytes: a 17-byte label (16 characters and a NUL), 3 bytes padding, then two
// 32-bit fields (package type, download status). HITMAN reads only the label (sub_121971a),
// first with list = NULL for the count, then with a buffer of that many entries, and loops over
// the count from the first call: both calls must report the same number.
//
// The stub calls the real function into a stack buffer, appends the labels from a table the
// patcher wrote into the game's memory that the real list lacks (with the type and status of the
// first real entry, when there is one), then answers either call from that merged list. It
// always returns 0: the game stops with a trap when this call fails.
//
// Table at `table`: "HSEN", uint32 count, then count 28-byte entries.

#define ENT_ENTRY     28
#define ENT_REAL_MAX  64   // entries asked from the real function
#define ENT_SCRATCH   96   // stack buffer, real + appended
// On entry rsp is 8 (mod 16). 6 pushes (rbp, rbx, r12-r15) = 48 bytes keep that, so the frame is
// chosen 8 (mod 16) to leave rsp 16-aligned (0 mod 16) before the call.
#define ENT_FRAME_RAW (16 + ENT_SCRATCH * ENT_ENTRY)
#define ENT_FRAME     (ENT_FRAME_RAW + ((8 - ENT_FRAME_RAW) & 15))

static inline size_t stub_entitlements(code_t *c, uint64_t real, uint64_t table) {
  size_t at = align16(c);
  E(0x55);                    // push rbp
  E(0x48, 0x89, 0xE5);        // mov rbp, rsp
  E(0x53);                    // push rbx
  E(0x41, 0x54);              // push r12
  E(0x41, 0x55);              // push r13
  E(0x41, 0x56);              // push r14
  E(0x41, 0x57);              // push r15
  E(0x48, 0x81, 0xEC);        // sub rsp, ENT_FRAME
  e32(c, ENT_FRAME);
  E(0x89, 0xFB);              // mov ebx, edi        service label
  E(0x49, 0x89, 0xF4);        // mov r12, rsi        list
  E(0x41, 0x89, 0xD5);        // mov r13d, edx       listNum
  E(0x49, 0x89, 0xCE);        // mov r14, rcx        hitNum
  E(0x45, 0x31, 0xFF);        // xor r15d, r15d      entries in the buffer
  E(0xFC);                    // cld                 rep movsb below needs DF clear
  if (real) {
    E(0xC7, 0x44, 0x24, 0x08, 0, 0, 0, 0);  // mov dword [rsp+8], 0
    E(0x48, 0x8D, 0x74, 0x24, 0x10);        // lea rsi, [rsp+16]
    E(0xBA);                                // mov edx, ENT_REAL_MAX
    e32(c, ENT_REAL_MAX);
    E(0x48, 0x8D, 0x4C, 0x24, 0x08);        // lea rcx, [rsp+8]
    E(0x89, 0xDF);                          // mov edi, ebx
    E(0x48, 0xB8);                          // mov rax, real
    e64(c, real);
    E(0xFF, 0xD0);                          // call rax
    E(0x85, 0xC0);                          // test eax, eax
    size_t failed = jcc(c, 0x75);           // jnz: no real entries
    E(0x44, 0x8B, 0x7C, 0x24, 0x08);        // mov r15d, [rsp+8]
    E(0x41, 0x83, 0xFF, ENT_REAL_MAX);      // cmp r15d, ENT_REAL_MAX
    size_t fits = jcc(c, 0x76);             // jbe
    E(0x41, 0xBF);                          // mov r15d, ENT_REAL_MAX
    e32(c, ENT_REAL_MAX);
    land(c, failed);
    land(c, fits);
  }
  E(0x44, 0x89, 0x3C, 0x24);  // mov [rsp], r15d     real count
  E(0x49, 0xBA);              // mov r10, table
  e64(c, table);
  E(0x45, 0x8B, 0x5A, 0x04);  // mov r11d, [r10+4]   table count
  E(0x49, 0x83, 0xC2, 0x08);  // add r10, 8

  size_t extra_loop = c->n;
  E(0x45, 0x85, 0xDB);        // test r11d, r11d
  size_t done1 = jcc(c, 0x74);  // jz extras_done
  E(0x41, 0x83, 0xFF, ENT_SCRATCH);  // cmp r15d, ENT_SCRATCH
  size_t done2 = jcc(c, 0x73);  // jae extras_done
  E(0x49, 0x8B, 0x02);        // mov rax, [r10]
  E(0x49, 0x8B, 0x52, 0x08);  // mov rdx, [r10+8]
  E(0x31, 0xC9);              // xor ecx, ecx
  size_t dup_loop = c->n;
  E(0x3B, 0x0C, 0x24);        // cmp ecx, [rsp]
  size_t to_append = jcc(c, 0x73);  // jae append
  E(0x44, 0x6B, 0xC1, ENT_ENTRY);   // imul r8d, ecx, 28
  E(0x4C, 0x8D, 0x4C, 0x24, 0x10);  // lea r9, [rsp+16]
  E(0x4D, 0x01, 0xC1);              // add r9, r8
  E(0x49, 0x39, 0x01);              // cmp [r9], rax
  size_t dup_next1 = jcc(c, 0x75);  // jne dup_next
  E(0x49, 0x39, 0x51, 0x08);        // cmp [r9+8], rdx
  size_t already = jcc(c, 0x74);    // je extra_next
  land(c, dup_next1);
  E(0xFF, 0xC1);                    // inc ecx
  jback(c, 0xEB, dup_loop);         // jmp dup_loop
  land(c, to_append);
  E(0x45, 0x6B, 0xC7, ENT_ENTRY);   // imul r8d, r15d, 28
  E(0x4C, 0x8D, 0x4C, 0x24, 0x10);  // lea r9, [rsp+16]
  E(0x4D, 0x01, 0xC1);              // add r9, r8
  E(0x49, 0x89, 0x01);              // mov [r9], rax
  E(0x49, 0x89, 0x51, 0x08);        // mov [r9+8], rdx
  E(0x49, 0x8B, 0x42, 0x10);        // mov rax, [r10+16]
  E(0x49, 0x89, 0x41, 0x10);        // mov [r9+16], rax
  E(0x41, 0x8B, 0x42, 0x18);        // mov eax, [r10+24]
  E(0x41, 0x89, 0x41, 0x18);        // mov [r9+24], eax
  E(0x83, 0x3C, 0x24, 0x00);        // cmp dword [rsp], 0
  size_t no_real = jcc(c, 0x74);    // je: keep the table's type and status
  E(0x48, 0x8B, 0x44, 0x24, 0x24);  // mov rax, [rsp+16+20]   first real entry's type, status
  E(0x49, 0x89, 0x41, 0x14);        // mov [r9+20], rax
  land(c, no_real);
  E(0x41, 0xFF, 0xC7);              // inc r15d
  land(c, already);
  E(0x49, 0x83, 0xC2, ENT_ENTRY);   // add r10, 28
  E(0x41, 0xFF, 0xCB);              // dec r11d
  jback(c, 0xEB, extra_loop);       // jmp extra_loop
  land(c, done1);
  land(c, done2);

  E(0x4D, 0x85, 0xE4);              // test r12, r12
  size_t count_only = jcc(c, 0x74);  // jz count_only
  E(0x44, 0x89, 0xF9);              // mov ecx, r15d
  E(0x44, 0x39, 0xE9);              // cmp ecx, r13d
  size_t have_n = jcc(c, 0x76);     // jbe
  E(0x44, 0x89, 0xE9);              // mov ecx, r13d
  land(c, have_n);
  E(0x4D, 0x85, 0xF6);              // test r14, r14
  size_t no_hit = jcc(c, 0x74);     // jz
  E(0x41, 0x89, 0x0E);              // mov [r14], ecx
  land(c, no_hit);
  E(0x6B, 0xC9, ENT_ENTRY);         // imul ecx, ecx, 28
  E(0x48, 0x8D, 0x74, 0x24, 0x10);  // lea rsi, [rsp+16]
  E(0x4C, 0x89, 0xE7);              // mov rdi, r12
  E(0xF3, 0xA4);                    // rep movsb
  size_t to_ret = jcc(c, 0xEB);     // jmp ret0
  land(c, count_only);
  E(0x4D, 0x85, 0xF6);              // test r14, r14
  size_t no_hit2 = jcc(c, 0x74);    // jz ret0
  E(0x45, 0x89, 0x3E);              // mov [r14], r15d
  land(c, no_hit2);
  land(c, to_ret);
  E(0x31, 0xC0);                    // xor eax, eax
  E(0x48, 0x81, 0xC4);              // add rsp, ENT_FRAME
  e32(c, ENT_FRAME);
  E(0x41, 0x5F, 0x41, 0x5E, 0x41, 0x5D, 0x41, 0x5C);  // pop r15, r14, r13, r12
  E(0x5B, 0x5D, 0xC3);              // pop rbx; pop rbp; ret
  return at;
}
