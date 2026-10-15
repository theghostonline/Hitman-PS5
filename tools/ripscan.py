import struct, sys, re
path = sys.argv[1]; want = sys.argv[2:]
b = open(path,'rb').read()
phoff = struct.unpack_from('<Q', b, 0x20)[0]; phnum = struct.unpack_from('<H', b, 0x38)[0]
segs = []
for i in range(phnum):
    o = phoff + i*56
    pt, fl, off, va, pa, fs, ms, al = struct.unpack_from('<IIQQQQQQ', b, o)
    if pt == 1: segs.append((va, off, fs, ms, fl))
def off2va(x):
    for va, off, fs, ms, fl in segs:
        if off <= x < off+fs: return va + (x-off)
def va2off(v):
    for va, off, fs, ms, fl in segs:
        if va <= v < va+fs: return off + (v-va)
text = [s for s in segs if s[4] & 1][0]
targets = {}
for w in want:
    pat = w.encode() + b'\0'
    for m in re.finditer(re.escape(pat), b):
        if m.start() > 0 and b[m.start()-1] != 0: continue
        targets[off2va(m.start())] = w
tv, to, tfs = text[0], text[1], text[2]
code = b[to:to+tfs]
hits = {}
for m in re.finditer(rb'[\x48\x4c]\x8d[\x05\x0d\x15\x1d\x25\x2d\x35\x3d]', code):
    i = m.start()
    disp = struct.unpack_from('<i', code, i+3)[0]
    tgt = tv + i + 7 + disp
    if tgt in targets: hits.setdefault(targets[tgt], []).append(tv+i)
for w in want:
    print(repr(w), [hex(x) for x in hits.get(w, [])][:12])
