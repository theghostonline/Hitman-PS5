#!/usr/bin/env python3
"""Extracts one resource from the PS5 game packages by its path (".pc_" is turned into ".ps5_")
or by its runtime id, searching every chunk's index over FTP byte ranges.

usage: python3 -I tools/ps5_resource.py ftp://host:port/.../runtime/ "<path or 16-hex id>" out_file
Game data: keep extracted files out of the repository.
"""
import os
import struct
import sys

_here = os.path.dirname(os.path.abspath(__file__))
_ns = {"__name__": "ps5dr"}
exec(compile(open(os.path.join(_here, "ps5_dynamic_resources.py")).read().replace("\nmain()\n", "\n"),
             "ps5_dynamic_resources.py", "exec"), _ns)


def main(base, what, out):
    if len(what) == 16 and all(c in "0123456789abcdefABCDEF" for c in what):
        rid, path = int(what, 16), what
    else:
        path = what.replace("].pc_", "].ps5_")
        rid = _ns["runtime_id"](path)
    for i in range(30):
        url = f"{base}chunk{i}.rpkg"
        if _ns["contains"](url, rid):
            entry, data = _ns["extract"](url, rid)
            with open(out, "wb") as f:
                f.write(data)
            print(f"{rid:016X} {entry['type']} {len(data)} bytes from chunk{i}: {path}")
            return
    raise SystemExit(f"{rid:016X} not found: {path}")


main(*sys.argv[1:4])
