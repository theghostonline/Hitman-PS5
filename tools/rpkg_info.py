#!/usr/bin/env python3
"""Lists the resources of a Glacier 2 RPKG (v1 "GKPR" or v2 "2KPR"): hash, type, sizes, and
each resource's reference hashes with flags.

usage: python3 -I tools/rpkg_info.py file.rpkg
"""
import struct
import sys


def parse(data):
    off = 0
    magic = data[:4]
    off = 4
    if magic == b"2KPR":
        unk, = struct.unpack_from("<I", data, off)
        off += 4
        chunk, chunk_type, patch_id, lang = data[off], data[off + 1], data[off + 2], data[off + 3:off + 5]
        off += 5
    elif magic == b"GKPR":
        chunk = chunk_type = patch_id = 0
        lang = b""
    else:
        raise SystemExit(f"not an RPKG: {magic!r}")
    count, index_size, table_size = struct.unpack_from("<III", data, off)
    off += 12
    # Patch packages list deleted hashes here; this tool expects a base package (the patch
    # count is only present when the index does not start right away).
    entries = []
    for i in range(count):
        h, doff, dsize = struct.unpack_from("<QQI", data, off)
        off += 20
        entries.append({"hash": h, "offset": doff, "packed": dsize & 0x3FFFFFFF, "xor": bool(dsize & 0x80000000)})
    for e in entries:
        rtype = data[off:off + 4][::-1].decode("latin-1")
        ref_size, states_size, size, sysmem, vidmem = struct.unpack_from("<IIIII", data, off + 4)
        off += 24
        refs = []
        if ref_size:
            n, = struct.unpack_from("<I", data, off)
            n &= 0x3FFFFFFF
            flags = data[off + 4:off + 4 + n]
            hashes = struct.unpack_from(f"<{n}Q", data, off + 4 + n)
            refs = list(zip(hashes, flags))
            off += ref_size
        e.update(type=rtype, size=size, refs=refs, states=states_size, sysmem=sysmem, vidmem=vidmem)
    return {"chunk": chunk, "patch": patch_id, "lang": lang, "entries": entries}


def main(path):
    info = parse(open(path, "rb").read())
    print(f"{path}: chunk {info['chunk']} patch {info['patch']} lang {info['lang']!r}, {len(info['entries'])} resources")
    for e in info["entries"]:
        print(f"  {e['hash']:016X}.{e['type']}  size {e['size']} packed {e['packed']} xor {e['xor']}  "
              f"refs {len(e['refs'])}")
        for h, fl in e["refs"]:
            print(f"      -> {h:016X} flags 0x{fl:02x}")


main(sys.argv[1])
