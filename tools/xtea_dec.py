import struct, sys, zlib
KEY = (0x30F95282, 0x1F48C419, 0x295F8548, 0x2A78366D)
DELTA = 0x9E3779B9
def dec(v0, v1):
    s = (DELTA * 32) & 0xFFFFFFFF
    for _ in range(32):
        v1 = (v1 - ((((v0 << 4) ^ (v0 >> 5)) + v0) ^ (s + KEY[(s >> 11) & 3]))) & 0xFFFFFFFF
        s = (s - DELTA) & 0xFFFFFFFF
        v0 = (v0 - ((((v1 << 4) ^ (v1 >> 5)) + v1) ^ (s + KEY[s & 3]))) & 0xFFFFFFFF
    return v0, v1
b = open(sys.argv[1], 'rb').read()
crc = struct.unpack_from('<I', b, 16)[0]
body = b[20:]
out = bytearray()
for i in range(0, len(body) - len(body) % 8, 8):
    out += struct.pack('<2I', *dec(*struct.unpack_from('<2I', body, i)))
print('header', b[:16].hex(), 'crc_field', hex(crc), 'crc_calc', hex(zlib.crc32(out.rstrip(b'\0')) & 0xFFFFFFFF), 'crc_full', hex(zlib.crc32(out) & 0xFFFFFFFF), file=sys.stderr)
sys.stdout.write(out.decode('utf-8', 'replace'))
