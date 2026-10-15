#!/usr/bin/env python3
"""Searches the hash indexes of RPKGs on the console (FTP byte ranges, nothing large is
downloaded) for runtime ids.

usage: python3 -I tools/rpkg_find_remote.py ftp://host:port/dir/ chunk0.rpkg [...] -- HASH [HASH...]
"""
import struct
import subprocess
import sys


def fetch(url, start, length):
    r = subprocess.run(["curl", "-s", "-m", "300", "-r", f"{start}-{start + length - 1}", url], capture_output=True)
    return r.stdout


def index(url):
    head = fetch(url, 0, 64)
    if head[:4] == b"2KPR":
        off = 13
    elif head[:4] == b"GKPR":
        off = 4
    else:
        raise RuntimeError(f"{url}: not an rpkg ({head[:4]!r})")
    count, index_size, table_size = struct.unpack_from("<III", head, off)
    off += 12
    # Patch packages carry a deleted-hash list (count, then hashes) before the index.
    data = fetch(url, off, 4)
    return head, count, off


def main(argv):
    sep = argv.index("--")
    base, files, wanted = argv[0], argv[1:sep], {int(h, 16) for h in argv[sep + 1:]}
    for name in files:
        url = base + name
        head, count, off = index(url)
        raw = fetch(url, off, count * 20)
        if len(raw) < count * 20:
            print(f"{name}: short read ({len(raw)} of {count * 20})")
            continue
        found = []
        for i in range(count):
            h, doff, size = struct.unpack_from("<QQI", raw, i * 20)
            if h in wanted:
                found.append(f"{h:016X} at {doff:#x} size {size & 0x3FFFFFFF}")
        print(f"{name}: {count} resources, chunk {head[9] if head[:4] == b'2KPR' else '?'}"
              f" patch {head[11] if head[:4] == b'2KPR' else '?'}: {found if found else 'none of them'}")


main(sys.argv[1:])
