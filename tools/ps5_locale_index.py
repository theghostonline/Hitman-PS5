#!/usr/bin/env python3
"""Reads every LOCR (localization text table) of the PS5 game over FTP byte ranges and writes a
JSON map of CRC32 key hash -> English text, so server data can be checked against the strings
the console actually has.

usage: python3 -I tools/ps5_locale_index.py ftp://host:port/.../runtime/ out.json [--count-only]
The output is derived from the user's own game files: keep it in runtime/, do not commit it.
"""
import json
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
_src = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "ps5_dynamic_resources.py")).read()
_ns = {"__name__": "ps5dr"}
exec(compile(_src.replace("\nmain()\n", "\n"), "ps5_dynamic_resources.py", "exec"), _ns)
read_range, parse_header, parse_table_entry = _ns["read_range"], _ns["parse_header"], _ns["parse_table_entry"]
lz4, XOR_KEY = _ns["lz4_block_decompress"], _ns["XOR_KEY"]

LOCR_KEY = (0x53527737, 0x7506499E, 0xBD39AEE3, 0xA59E7268)
# H3 language slots, in LOCR order (Peacock's resources/rebuildLocale.cjs; same 10 on PS5)
LANGS = ["xx", "en", "fr", "it", "de", "es", "ru", "cn", "tc", "jp"]


def xtea_dec(b):
    out = bytearray()
    for i in range(0, len(b) - len(b) % 8, 8):
        v0, v1 = struct.unpack_from("<2I", b, i)
        delta, s = 0x9E3779B9, (0x9E3779B9 * 32) & 0xFFFFFFFF
        for _ in range(32):
            v1 = (v1 - ((((v0 << 4) ^ (v0 >> 5)) + v0) ^ (s + LOCR_KEY[(s >> 11) & 3]))) & 0xFFFFFFFF
            s = (s - delta) & 0xFFFFFFFF
            v0 = (v0 - ((((v1 << 4) ^ (v1 >> 5)) + v1) ^ (s + LOCR_KEY[s & 3]))) & 0xFFFFFFFF
        out += struct.pack("<2I", v0, v1)
    return bytes(out).rstrip(b"\0").decode("utf-8", "replace")


def locr_strings(d, lang="en"):
    if not d or d[0] != 0:
        return {}
    n = (struct.unpack_from("<I", d, 1)[0] - 1) // 4
    if n != len(LANGS):
        return {}
    o = struct.unpack_from(f"<{n}I", d, 1)[LANGS.index(lang)]
    if o == 0xFFFFFFFF or o + 4 > len(d):
        return {}
    cnt = struct.unpack_from("<I", d, o)[0]
    p, out = o + 4, {}
    for _ in range(cnt):
        if p + 8 > len(d):
            return None  # not the layout this reads
        h, ln = struct.unpack_from("<II", d, p)
        if p + 8 + ln > len(d):
            return None
        out[h] = xtea_dec(d[p + 8:p + 8 + ln])
        p += 8 + ln + 1  # each string is followed by a NUL outside its encrypted length
    return out


def locrs(url):
    head = read_range(url, 0, 64)
    index_at, count, index_size, table_size = parse_header(head)
    index = read_range(url, index_at, index_size)
    table = read_range(url, index_at + index_size, table_size)
    off, found = 0, []
    for i in range(count):
        entry, off = parse_table_entry(table, off)
        if entry["type"] == "LOCR":
            h, doff, dsize = struct.unpack_from("<QQI", index, i * 20)
            found.append((h, doff, dsize, entry["size"]))
    return found


def main(base, out, count_only):
    chunks = [f"chunk{i}.rpkg" for i in range(30)]
    texts, total, skipped = {}, 0, 0
    for name in chunks:
        url = base + name
        items = locrs(url)
        size = sum((d & 0x3FFFFFFF) or s for _, _, d, s in items)
        total += size
        print(f"{name}: {len(items)} LOCR, {size} bytes stored", file=sys.stderr)
        if count_only:
            continue
        for h, doff, dsize, size in items:
            packed, xored = dsize & 0x3FFFFFFF, bool(dsize & 0x80000000)
            raw = read_range(url, doff, packed or size)
            if xored:
                raw = bytes(b ^ XOR_KEY[k % 8] for k, b in enumerate(raw))
            data = lz4(raw, size) if packed else raw
            strings = locr_strings(data)
            if strings is None:
                skipped += 1
                continue
            for k, v in strings.items():
                texts.setdefault(f"{k:08X}", v)
    print(f"total {total} bytes, {skipped} LOCR in another layout skipped", file=sys.stderr)
    if not count_only:
        os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
        with open(out, "w") as f:
            json.dump(texts, f, ensure_ascii=False)
        print(f"{len(texts)} English strings -> {out}", file=sys.stderr)


main(sys.argv[1], sys.argv[2], "--count-only" in sys.argv)
