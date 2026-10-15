#!/usr/bin/env python3
"""Turns ServiceMap.java output into service/method/request-key rows.

usage: python3 -I tools/servicemap_extract.py docs/ghidra/servicemap > docs/ghidra/service_calls.tsv
Each request builder stores its JSON body keys as ZStrings, then formats "{0}/{1}/{2}" with the
service and method literals (see docs/PS5_GHIDRA_FINDINGS.md). Keys are the identifier literals
referenced in the 0x180 bytes of code before the "{0}/{1}/{2}" reference. Literals that are only
symbol-table noise (small immediates landing in .dynstr/.shstrtab) are dropped."""
import os, re, sys

JUNK = re.compile(r"@plt|PATH@|^\.|^(rodata|data|s\.1|ta\.1|lt|plt|odata)$|^E:/p4/")
IDENT = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
WINDOW = 0x180

def main(d):
    print("service\tmethod\trequest_keys\tghidra_fn\tsite\tcallers")
    for fn in sorted(os.listdir(os.path.join(d, "functions"))):
        with open(os.path.join(d, "functions", fn), encoding="latin-1") as f:
            head = [l.rstrip("\n") for l in f if l.startswith("//")]
        lits = []
        for l in head:
            if l.startswith("// lit ") and "\t" in l:
                addr, s = l[len("// lit "):].split("\t", 1)
                if not JUNK.search(s):
                    lits.append((int(addr, 16), s))
        callers = next((l[len("// callers: "):] for l in head if l.startswith("// callers: ")), "")
        done = set()
        for i, (a, s) in enumerate(lits):
            if s != "{0}/{1}/{2}" or a in done:
                continue
            nxt = [x for (b, x) in lits[i + 1:i + 8] if x != "{0}/{1}/{2}" and b - a < 0x80]
            if len(nxt) < 1 or not nxt[0].endswith("Service"):
                continue
            service = nxt[0]
            method = next((x for x in nxt[1:] if x != service and IDENT.match(x)), "?")
            keys = []
            for (b, x) in lits[:i]:
                if a - WINDOW <= b < a and IDENT.match(x) and x not in keys:
                    keys.append(x)
            done.add(a)
            print("\t".join((service, method, ",".join(keys), "0x" + fn[:-2], hex(a), callers[:200])))

main(sys.argv[1])
