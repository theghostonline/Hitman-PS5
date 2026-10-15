#!/usr/bin/env python3
"""Builds the PS5 dynamic resources package (dynamic_resources_ps5_release_rpkg).

After mounting the package it downloads, the PS5 client loads
[assembly:/repository/pro.repo](fullgame,dynamic).ps5_repo (sub_130ba38) and goes offline with
"Loading Online Repository FAILED" when it cannot (sub_c49776). That resource is in none of the
30 PS5 game packages: IOI's own PS5 package delivered it. Peacock's package only carries a LOCR
and a menu JSON, because PC builds ship their dynamic repository themselves.

This tool reads the console's base repository ([assembly:/repository/pro.repo].ps5_repo) from the
game's own chunk0.rpkg, over FTP byte ranges, and writes a package with that repository under the
dynamic id plus Peacock's resources, stored under the ids of their ".ps5_" paths (Peacock keys them
by their ".pc_" paths, which the console never looks up; that showed as Peacock's "dynamic
resource package has not been loaded" modal). The output is derived from the user's own game
files: it is written to the runtime directory and must not be committed or shared.

usage: python3 -I tools/ps5_dynamic_resources.py \
          --chunk0 ftp://192.168.0.37:2121/data/homebrew/PPSA16387-app0_HITMAN_3/runtime/chunk0.rpkg \
          --peacock upstream/Peacock/resources/dynamic_resources_h3.rpkg \
          --out runtime/peacock/ps5/dynamic_resources_ps5.rpkg
"""
import argparse
import hashlib
import json
import os
import struct
import subprocess
import sys

XOR_KEY = bytes([0xDC, 0x45, 0xA6, 0x9C, 0xD3, 0x72, 0x4C, 0xAB])


def runtime_id(path):
    return int.from_bytes(hashlib.md5(path.lower().encode()).digest()[:8], "big") & 0x00FFFFFFFFFFFFFF


BASE_REPO = runtime_id("[assembly:/repository/pro.repo].ps5_repo")
DYNAMIC_REPO = runtime_id("[assembly:/repository/pro.repo](fullgame,dynamic).ps5_repo")

# Peacock's package overrides base-game resources by their PC ids. The same resources have
# ".ps5_" paths on the console (both found in its chunk0.rpkg), so they are stored under those.
# Paths from Peacock's resources/rebuildLocale.cjs and the Glacier hash list.
PEACOCK_PATHS = [
    # Peacock's strings, H3 LOCR (same 10-language layout as the PS5 original)
    "[assembly:/localization/hitman6/conversations/ui/pro/menutext_drp.sweetmenutext].{}_localized-textlist",
    # debrief buttons without the elusive restrictions (otherwise identical to the PS5 original)
    "[assembly:/_pro/online/default/cloudstorage/resources/pages/common/views/debriefing/"
    "debriefingfailedbuttons.json].{}_json",
]
REKEY = {runtime_id(p.format("pc")): (runtime_id(p.format("ps5")), p.format("ps5")) for p in PEACOCK_PATHS}


def read_range(src, start, length):
    if src.startswith("ftp://"):
        out = subprocess.run(["curl", "-fsS", "-m", "600", "-r", f"{start}-{start + length - 1}", src],
                             capture_output=True)
        if out.returncode:
            raise SystemExit(f"FTP read failed: {out.stderr.decode().strip()}")
        return out.stdout
    with open(src, "rb") as f:
        f.seek(start)
        return f.read(length)


def parse_header(head):
    if head[:4] == b"2KPR":
        off = 13
    elif head[:4] == b"GKPR":
        off = 4
    else:
        raise SystemExit("not an RPKG")
    count, index_size, table_size = struct.unpack_from("<III", head, off)
    return off + 12, count, index_size, table_size


def parse_table_entry(table, off):
    """One resource table entry at off: (dict, next offset). raw keeps the exact bytes."""
    rtype = table[off:off + 4]
    ref_size, states, size, sysmem, vidmem = struct.unpack_from("<IIIII", table, off + 4)
    end = off + 24 + ref_size
    refs = []
    if ref_size:
        n = struct.unpack_from("<I", table, off + 24)[0] & 0x3FFFFFFF
        flags = table[off + 28:off + 28 + n]
        hashes = struct.unpack_from(f"<{n}Q", table, off + 28 + n)
        refs = list(zip(hashes, flags))
    return {"type": rtype[::-1].decode("latin-1"), "raw": table[off:end], "size": size, "refs": refs}, end


def lz4_block_decompress(src, size):
    out = bytearray()
    i = 0
    while i < len(src):
        token = src[i]
        i += 1
        lit = token >> 4
        if lit == 15:
            while True:
                b = src[i]
                i += 1
                lit += b
                if b != 255:
                    break
        out += src[i:i + lit]
        i += lit
        if i >= len(src):
            break
        offset = src[i] | (src[i + 1] << 8)
        i += 2
        match = (token & 15) + 4
        if (token & 15) == 15:
            while True:
                b = src[i]
                i += 1
                match += b
                if b != 255:
                    break
        start = len(out) - offset
        if offset == 0 or start < 0:
            raise ValueError("bad lz4 offset")
        for k in range(match):
            out.append(out[start + k])
    if len(out) != size:
        raise ValueError(f"lz4: got {len(out)} bytes, expected {size}")
    return bytes(out)


_index_cache = {}


def load_index(src):
    if src not in _index_cache:
        head = read_range(src, 0, 64)
        index_at, count, index_size, table_size = parse_header(head)
        _index_cache[src] = (index_at, count, index_size, table_size, read_range(src, index_at, index_size))
    return _index_cache[src]


def contains(src, wanted):
    index_at, count, index_size, table_size, index = load_index(src)
    return any(struct.unpack_from("<Q", index, i * 20)[0] == wanted for i in range(count))


def extract(src, wanted):
    index_at, count, index_size, table_size, index = load_index(src)
    position = None
    for i in range(count):
        h, doff, dsize = struct.unpack_from("<QQI", index, i * 20)
        if h == wanted:
            position, data_off, packed, xored = i, doff, dsize & 0x3FFFFFFF, bool(dsize & 0x80000000)
            break
    if position is None:
        raise SystemExit(f"{wanted:016X} is not in {src}")
    table = read_range(src, index_at + index_size, table_size)
    off = 0
    for i in range(position + 1):
        entry, off = parse_table_entry(table, off)
    stored = read_range(src, data_off, packed if packed else entry["size"])
    if xored:
        stored = bytes(b ^ XOR_KEY[k % 8] for k, b in enumerate(stored))
    data = lz4_block_decompress(stored, entry["size"]) if packed else stored
    return entry, data


def read_package(path):
    blob = open(path, "rb").read()
    index_at, count, index_size, table_size = parse_header(blob[:64])
    table = blob[index_at + index_size:index_at + index_size + table_size]
    out, toff = [], 0
    for i in range(count):
        h, doff, dsize = struct.unpack_from("<QQI", blob, index_at + i * 20)
        entry, toff = parse_table_entry(table, toff)
        n = dsize & 0x3FFFFFFF
        out.append({"hash": h, "size_field": dsize, "stored": blob[doff:doff + (n if n else entry["size"])],
                    "raw": entry["raw"], "type": entry["type"]})
    return out


def write_package(path, resources):
    """resources: hash, size_field (packed size | flags), stored bytes, raw table entry."""
    count = len(resources)
    index_size = 20 * count
    table = b"".join(r["raw"] for r in resources)
    head_len = 4 + 4 + 5 + 12
    data_at = head_len + index_size + len(table)
    index, data = b"", b""
    for r in resources:
        index += struct.pack("<QQI", r["hash"], data_at + len(data), r["size_field"])
        data += r["stored"]
    head = b"2KPR" + struct.pack("<I", 1) + bytes([0, 0, 0]) + b"xx" + struct.pack("<III", count, index_size,
                                                                                        len(table))
    with open(path + ".tmp", "wb") as f:
        f.write(head + index + table + data)
    os.replace(path + ".tmp", path)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--chunk0", required=True, help="PS5 chunk0.rpkg (path or ftp:// URL)")
    ap.add_argument("--peacock", required=True, help="Peacock's dynamic_resources_h3.rpkg")
    ap.add_argument("--out", required=True)
    a = ap.parse_args()

    entry, repo = extract(a.chunk0, BASE_REPO)
    if entry["type"] != "REPO":
        raise SystemExit(f"{BASE_REPO:016X} is a {entry['type']}, not a REPO")
    json.loads(repo)  # the repository is JSON; refuse anything else
    print(f"base repository {BASE_REPO:016X}: {len(repo)} bytes, {len(entry['refs'])} references, "
          f"sha256 {hashlib.sha256(repo).hexdigest()[:16]}")

    # Same table entry (type, sizes, references) and data, under the dynamic id, stored plain.
    dyn = {"hash": DYNAMIC_REPO, "size_field": 0, "stored": repo, "raw": entry["raw"], "type": "REPO"}
    peacock = read_package(a.peacock)
    for r in peacock:
        if r["hash"] not in REKEY:
            print(f"  {r['hash']:016X}.{r['type']}: no known PS5 path, kept under its PC id (the console "
                  f"will not use it)", file=sys.stderr)
            continue
        ps5_id, ps5_path = REKEY[r["hash"]]
        if not contains(a.chunk0, ps5_id):
            raise SystemExit(f"{ps5_path} ({ps5_id:016X}) is not in the console's chunk0: not this game build?")
        ps5_entry, _ = extract(a.chunk0, ps5_id)
        if ps5_entry["type"] != r["type"]:
            raise SystemExit(f"{ps5_id:016X} is a {ps5_entry['type']} on the console, Peacock's is a {r['type']}")
        print(f"  {r['hash']:016X}.{r['type']} -> {ps5_id:016X} {ps5_path}")
        r["hash"] = ps5_id
    os.makedirs(os.path.dirname(os.path.abspath(a.out)), exist_ok=True)
    write_package(a.out, [dyn] + peacock)

    check = read_package(a.out)
    assert [r["hash"] for r in check] == [DYNAMIC_REPO] + [r["hash"] for r in peacock]
    assert check[0]["stored"] == repo and all(c["stored"] == p["stored"] for c, p in zip(check[1:], peacock))
    print(f"wrote {a.out}: {len(check)} resources "
          f"({', '.join(f'{r['hash']:016X}.{r['type']}' for r in check)}), "
          f"{os.path.getsize(a.out)} bytes, sha256 {hashlib.sha256(open(a.out, 'rb').read()).hexdigest()}")


main()
