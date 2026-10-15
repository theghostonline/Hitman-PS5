#!/bin/bash
# usage: dis.sh start stop
S=${S:-/private/tmp/claude-501/-Users-husam-Developer-NuvioPS5/d7cd98f7-4ad1-4e8a-adc4-bdb0f9f78088/scratchpad}
/opt/homebrew/opt/llvm/bin/llvm-objdump -d --no-show-raw-insn --start-address=$1 --stop-address=$2 -M intel $S/dyn/eboot.sym.elf 2>/dev/null | grep -vE '^\s*$|file format|Disassembly of' > $S/.dis.tmp
python3 -I - $S/hitman_dump/eboot.bin $S/.dis.tmp <<'PY'
import sys,re,struct
b=open(sys.argv[1],'rb').read()
phoff=struct.unpack_from('<Q',b,0x20)[0]; phnum=struct.unpack_from('<H',b,0x38)[0]
segs=[struct.unpack_from('<IIQQQQQQ',b,phoff+i*56) for i in range(phnum)]
def cstr(va):
    for pt,fl,off,v,pa,fs,ms,al in segs:
        if pt==1 and v<=va<v+fs:
            o=off+va-v; e=b.find(b'\0',o,o+160); s=b[o:e]
            if len(s)>2 and all(32<=c<127 or c in (9,10,13) for c in s): return s.decode()
for l in open(sys.argv[2]):
    l=l.rstrip(); m=re.search(r'#\s*0x([0-9a-f]+)',l)
    s=cstr(int(m.group(1),16)) if m else None
    l=re.sub(r'\s*<__imp_[^>]*>','',l)
    print(l + (f'    ; "{s[:90]}"' if s else ''))
PY
