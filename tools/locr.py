#!/usr/bin/env python3
"""Reads and writes H3 LOCR (localization text list) resources.

Layout (checked against the PS5 game's own tables and Peacock's prebuilt one): a 0 byte, one
uint32 offset per language slot, then per language a uint32 count and per string
uint32 CRC32(key) | uint32 length | XTEA-encrypted UTF-8 padded to 8 bytes | one 0 byte.
H3 language slots: xx en fr it de es ru cn tc jp.

usage: python3 -I tools/locr.py roundtrip <file.LOCR>    decode, re-encode, compare bytes
"""
import struct
import sys
import zlib

KEY = (0x53527737, 0x7506499E, 0xBD39AEE3, 0xA59E7268)
DELTA = 0x9E3779B9
LANGS = ["xx", "en", "fr", "it", "de", "es", "ru", "cn", "tc", "jp"]


def _dec_block(v0, v1):
    s = (DELTA * 32) & 0xFFFFFFFF
    for _ in range(32):
        v1 = (v1 - ((((v0 << 4) ^ (v0 >> 5)) + v0) ^ (s + KEY[(s >> 11) & 3]))) & 0xFFFFFFFF
        s = (s - DELTA) & 0xFFFFFFFF
        v0 = (v0 - ((((v1 << 4) ^ (v1 >> 5)) + v1) ^ (s + KEY[s & 3]))) & 0xFFFFFFFF
    return v0, v1


def _enc_block(v0, v1):
    s = 0
    for _ in range(32):
        v0 = (v0 + ((((v1 << 4) ^ (v1 >> 5)) + v1) ^ (s + KEY[s & 3]))) & 0xFFFFFFFF
        s = (s + DELTA) & 0xFFFFFFFF
        v1 = (v1 + ((((v0 << 4) ^ (v0 >> 5)) + v0) ^ (s + KEY[(s >> 11) & 3]))) & 0xFFFFFFFF
    return v0, v1


def decrypt(b):
    out = bytearray()
    for i in range(0, len(b) - len(b) % 8, 8):
        out += struct.pack("<2I", *_dec_block(*struct.unpack_from("<2I", b, i)))
    return bytes(out)


def encrypt(b):
    b = b + b"\0" * (-len(b) % 8)
    out = bytearray()
    for i in range(0, len(b), 8):
        out += struct.pack("<2I", *_enc_block(*struct.unpack_from("<2I", b, i)))
    return bytes(out)


def key_hash(key):
    return zlib.crc32(key.upper().encode()) & 0xFFFFFFFF


def decode(data):
    """[(lang, [(hash, text)])] in file order."""
    n = (struct.unpack_from("<I", data, 1)[0] - 1) // 4
    offsets = struct.unpack_from(f"<{n}I", data, 1)
    out = []
    for slot, o in enumerate(offsets):
        cnt = struct.unpack_from("<I", data, o)[0]
        p, items = o + 4, []
        for _ in range(cnt):
            h, ln = struct.unpack_from("<II", data, p)
            text = decrypt(data[p + 8:p + 8 + ln]).rstrip(b"\0").decode("utf-8")
            items.append((h, text))
            p += 8 + ln + 1
        out.append((LANGS[slot] if slot < len(LANGS) else str(slot), items))
    return out


def encode(langs):
    sections = []
    for _, items in langs:
        sec = struct.pack("<I", len(items))
        for h, text in items:
            enc = encrypt(text.encode("utf-8"))
            sec += struct.pack("<II", h, len(enc)) + enc + b"\0"
        sections.append(sec)
    offset = 1 + 4 * len(sections)
    head = b"\0"
    for sec in sections:
        head += struct.pack("<I", offset)
        offset += len(sec)
    return head + b"".join(sections)


if __name__ == "__main__" and len(sys.argv) == 3 and sys.argv[1] == "roundtrip":
    raw = open(sys.argv[2], "rb").read()
    again = encode(decode(raw))
    print("identical" if again == raw else f"DIFFERENT ({len(raw)} vs {len(again)} bytes)")
    sys.exit(0 if again == raw else 1)
