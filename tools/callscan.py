import struct, sys, re
b = open(sys.argv[1],'rb').read(); target = int(sys.argv[2],16); lo = int(sys.argv[3],16) if len(sys.argv)>3 else 0; hi = int(sys.argv[4],16) if len(sys.argv)>4 else 1<<62
phoff = struct.unpack_from('<Q', b, 0x20)[0]; phnum = struct.unpack_from('<H', b, 0x38)[0]
segs = [struct.unpack_from('<IIQQQQQQ', b, phoff+i*56) for i in range(phnum)]
pt,fl,off,va,pa,fs,ms,al = [s for s in segs if s[0]==1 and s[1]&1][0]
code = b[off:off+fs]
OPT = {64:'SSL_VERIFYPEER',81:'SSL_VERIFYHOST',10002:'URL',10023:'HTTPHEADER',10015:'POSTFIELDS',60:'POSTFIELDSIZE',47:'POST',10036:'CUSTOMREQUEST',10065:'CAINFO',10097:'CAPATH',20108:'SSL_CTX_FUNCTION',10109:'SSL_CTX_DATA',10230:'PINNEDPUBLICKEY',20011:'WRITEFUNCTION',10001:'WRITEDATA',20012:'READFUNCTION',10018:'USERAGENT',13:'TIMEOUT',78:'CONNECTTIMEOUT',155:'TIMEOUT_MS',156:'CONNECTTIMEOUT_MS',52:'FOLLOWLOCATION',99:'NOSIGNAL',41:'VERBOSE',20079:'HEADERFUNCTION',10029:'HEADERDATA',181:'PROTOCOLS',182:'REDIR_PROTOCOLS',32:'SSLVERSION',10082:'SSL_CIPHER_LIST',10004:'PROXY',10102:'ACCEPT_ENCODING',84:'HTTP_VERSION',20056:'PROGRESSFUNCTION',20219:'XFERINFOFUNCTION',43:'NOPROGRESS',10057:'PRIVATE',10086:'SSLCERTTYPE',10025:'SSLCERT',10087:'SSLKEY',10203:'RESOLVE',10100:'SHARE',80:'HTTPGET',44:'NOBODY',54:'PUT',10016:'REFERER',10022:'COOKIE',20014:'IOCTLFUNCTION',121:'TCP_NODELAY',213:'TCP_KEEPALIVE',30145:'POSTFIELDSIZE_LARGE',10165:'COPYPOSTFIELDS',216:'SSL_OPTIONS',230:'SSL_ENABLE_ALPN',103:'SSL_SESSIONID_CACHE',10063:'RANDOM_FILE'}
for m in re.finditer(rb'\xe8', code):
    i = m.start(); a = va+i
    if not (lo <= a < hi): continue
    if a + 5 + struct.unpack_from('<i', code, i+1)[0] != target: continue
    # look back up to 40 bytes for mov esi, imm32 (be xx xx xx xx)
    opt = None
    for j in range(i-5, max(i-48,0), -1):
        if code[j] == 0xbe:
            opt = struct.unpack_from('<I', code, j+1)[0]; break
    print(hex(a), opt, OPT.get(opt, '?'))
