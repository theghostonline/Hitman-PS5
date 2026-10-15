#!/usr/bin/env python3
"""Proxy-mode test: runs the host build with upstream = a scripted fake server and checks that
requests and responses cross it byte for byte, that the game's token is re-attached, and that
framing errors, timeouts, loops and LAN access are handled.

usage: python3 -I test/proxy_test.py build/hitman-server-host
"""
import hashlib
import http.client
import json
import os
import random
import socket
import subprocess
import sys
import tempfile
import threading
import time

passed = 0


def check(cond, what):
    global passed
    if not cond:
        raise AssertionError(what)
    passed += 1
    print(f"  ok  {what}")


def free_port():
    with socket.socket() as s:
        s.bind(("127.0.0.1", 0))
        return s.getsockname()[1]


def lan_ip():
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        try:
            s.connect(("192.0.2.1", 9))  # no packet is sent for a UDP connect
            ip = s.getsockname()[0]
        except OSError:
            return None
    return None if ip.startswith("127.") else ip


BINARY = random.Random(47).randbytes(1 << 20)


class FakeUpstream:
    """One request per connection, like the proxy sends them. Records what it received."""

    def __init__(self):
        self.port = free_port()
        self.seen = []
        self.sock = socket.socket()
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind(("127.0.0.1", self.port))
        self.sock.listen(16)
        self.alive = True
        threading.Thread(target=self.loop, daemon=True).start()

    def close(self):
        self.alive = False
        self.sock.close()

    def loop(self):
        while self.alive:
            try:
                c, _ = self.sock.accept()
            except OSError:
                return
            threading.Thread(target=self.serve, args=(c,), daemon=True).start()

    def serve(self, c):
        data = b""
        while b"\r\n\r\n" not in data:
            chunk = c.recv(65536)
            if not chunk:
                c.close()
                return
            data += chunk
        head, body = data.split(b"\r\n\r\n", 1)
        lines = head.decode("latin-1").split("\r\n")
        method, target, _ = lines[0].split(" ", 2)
        headers = {}
        for l in lines[1:]:
            k, v = l.split(":", 1)
            headers.setdefault(k.strip().lower(), []).append(v.strip())
        n = int(headers.get("content-length", ["0"])[0])
        while len(body) < n:
            body += c.recv(65536)
        req = {"method": method, "target": target, "headers": headers, "body": body}
        self.seen.append(req)
        path = target.split("?")[0]
        try:
            self.respond(c, method, path, req)
        finally:
            c.close()

    @staticmethod
    def send(c, status, headers, body=b"", length=True):
        out = f"HTTP/1.1 {status} X\r\n"
        for k, v in headers:
            out += f"{k}: {v}\r\n"
        if length:
            out += f"Content-Length: {len(body)}\r\n"
        out += "Connection: close\r\n\r\n"
        c.sendall(out.encode() + body)

    def respond(self, c, method, path, req):
        if path == "/echo":
            info = {"method": method, "target": req["target"], "headers": req["headers"],
                    "sha": hashlib.sha256(req["body"]).hexdigest(), "len": len(req["body"])}
            self.send(c, 200, [("Content-Type", "application/json")], json.dumps(info).encode())
        elif path == "/oauth/token":
            tok = {"access_token": "JWT-XYZ.payload.sig", "token_type": "bearer", "expires_in": 5000,
                   "refresh_token": "r-1"}
            self.send(c, 200, [("Content-Type", "application/json")], json.dumps(tok).encode())
        elif path == "/binary":
            self.send(c, 200, [("Content-Type", "application/octet-stream"), ("ETag", '"b1"')], BINARY)
        elif path == "/chunked":
            c.sendall(b"HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nX-Test: 1\r\nTransfer-Encoding: chunked\r\n"
                      b"Connection: close\r\n\r\n5\r\nhello\r\n5;ext=1\r\nworld\r\n0\r\nX-Trailer: t\r\n\r\n")
        elif path == "/eof":
            self.send(c, 200, [("Content-Type", "text/plain")], b"until-close" * 100, length=False)
        elif path == "/head":
            if method == "HEAD":
                c.sendall(b"HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: 12345\r\n"
                          b"x-ms-meta-version: 20181001\r\nConnection: close\r\n\r\n")
            else:
                self.send(c, 200, [("Content-Type", "application/octet-stream")], b"x" * 12345)
        elif path == "/nocontent":
            c.sendall(b"HTTP/1.1 204 No Content\r\nx-ms-meta-version: 20181001\r\nConnection: close\r\n\r\n")
        elif path == "/err":
            self.send(c, 403, [("Content-Type", "text/plain")], b"nope")
        elif path == "/slow":
            time.sleep(4)
            self.send(c, 200, [], b"late")
        elif path == "/bad":
            c.sendall(b"garbage\r\n\r\n")
        elif path == "/truncated":
            c.sendall(b"HTTP/1.1 200 OK\r\nContent-Length: 100\r\n\r\nshort")
        elif path == "/interim":
            c.sendall(b"HTTP/1.1 100 Continue\r\n\r\nHTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nok")
        else:
            self.send(c, 404, [("Content-Type", "text/plain")], b"Not found!")


class Proxy:
    def __init__(self, binary, upstream_port):
        self.dir = tempfile.mkdtemp(prefix="hitman-proxy-test-")
        self.port = free_port()
        self.config = os.path.join(self.dir, "config.ini")
        with open(self.config, "w") as f:
            f.write(f"port = {self.port}\nbind = 0.0.0.0\ndata_dir = {self.dir}\npatch = false\nlog_level = debug\n"
                    f"upstream = http://127.0.0.1:{upstream_port}\nupstream_timeout = 2\n")
        self.proc = subprocess.Popen([binary, self.config], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        for _ in range(100):
            try:
                if self.request("GET", "/healthz")[0] == 200:
                    return
            except OSError:
                time.sleep(0.05)
        raise RuntimeError("proxy did not start")

    def request(self, method, path, body=None, headers=None, host="127.0.0.1", chunked=False):
        conn = http.client.HTTPConnection(host, self.port, timeout=15)
        hdrs = dict(headers or {})
        if chunked:
            hdrs["Expect"] = "100-continue"
            conn.request(method, path, body=iter([body[:1000], body[1000:]]), headers=hdrs, encode_chunked=True)
        else:
            conn.request(method, path, body=body, headers=hdrs)
        r = conn.getresponse()
        data = r.read()
        hd = {k.lower(): v for k, v in r.getheaders()}
        conn.close()
        return r.status, hd, data

    def raw(self, payload):
        with socket.create_connection(("127.0.0.1", self.port), timeout=10) as s:
            s.sendall(payload)
            out = b""
            while True:
                try:
                    chunk = s.recv(65536)
                except socket.timeout:
                    break
                if not chunk:
                    break
                out += chunk
        return int(out.split(b" ", 2)[1]) if out.startswith(b"HTTP/1.1") else 0

    def stop(self):
        if self.proc.poll() is None:
            self.proc.terminate()
            self.proc.wait(5)

    def log(self):
        with open(os.path.join(self.dir, "server.log")) as f:
            return f.read()


def main(binary):
    up = FakeUpstream()
    px = Proxy(os.path.abspath(binary), up.port)
    try:
        print("forwarding")
        st, h, body = px.request("GET", "/echo?a=1&token=secret%2Bvalue", headers={"Accept": "application/json",
                                                                                  "User-Agent": "hitman6/6.0/sceCURL"})
        e = json.loads(body)
        check(st == 200 and e["method"] == "GET" and e["target"] == "/echo?a=1&token=secret%2Bvalue",
              "method and raw target reach the upstream unchanged")
        check(e["headers"]["host"] == [f"127.0.0.1:{px.port}"], "Host is the game's (Peacock builds URLs back to us)")
        check(e["headers"]["user-agent"] == ["hitman6/6.0/sceCURL"] and e["headers"]["accept"] == ["application/json"],
              "end-to-end request headers forwarded")
        check("hitman-server" in e["headers"]["via"][0] and e["headers"]["x-forwarded-for"] == ["127.0.0.1"]
              and e["headers"]["x-hs-peer"] == ["loopback"], "Via, X-Forwarded-For, X-HS-Peer added")
        check("authorization" not in e["headers"], "no token is invented before the game logs in")
        check(e["headers"]["connection"] == ["close"], "one upstream connection per request")

        print("game token")
        st, h, body = px.request("POST", "/oauth/token", "grant_type=external_psn&psn_userid=1&authcode=SECRETCODE",
                                 {"Content-Type": "application/json"})
        check(st == 200 and json.loads(body)["access_token"] == "JWT-XYZ.payload.sig", "OAuth response relayed")
        oauth_seen = up.seen[-1]
        check(oauth_seen["body"] == b"grant_type=external_psn&psn_userid=1&authcode=SECRETCODE"
              and oauth_seen["headers"]["content-type"] == ["application/json"], "OAuth body and headers as sent")
        st, h, body = px.request("GET", "/echo")
        check(json.loads(body)["headers"].get("authorization") == ["bearer JWT-XYZ.payload.sig"],
              "the game's token is attached to its requests without one")
        st, h, body = px.request("GET", "/echo", headers={"Authorization": "bearer other"})
        check(json.loads(body)["headers"].get("authorization") == ["bearer other"], "an existing Authorization is kept")

        print("bodies, both directions")
        payload = random.Random(1).randbytes(1 << 20)
        st, h, body = px.request("POST", "/echo", payload, {"Content-Type": "application/octet-stream"})
        e = json.loads(body)
        check(e["len"] == len(payload) and e["sha"] == hashlib.sha256(payload).hexdigest(), "1 MiB upload byte-exact")
        st, h, body = px.request("POST", "/echo", payload, {"Content-Type": "application/json"}, chunked=True)
        e = json.loads(body)
        check(e["sha"] == hashlib.sha256(payload).hexdigest() and "transfer-encoding" not in e["headers"]
              and e["headers"]["content-length"] == [str(len(payload))], "chunked + 100-continue upload re-framed")
        st, h, body = px.request("POST", "/echo", b"", {"Content-Type": "application/json"})
        check(json.loads(body)["headers"]["content-length"] == ["0"], "empty POST keeps Content-Length: 0")
        st, h, body = px.request("GET", "/binary")
        check(st == 200 and body == BINARY and h["content-type"] == "application/octet-stream" and h["etag"] == '"b1"',
              "1 MiB binary download byte-exact with its headers")
        st, h, body = px.request("GET", "/chunked")
        check(body == b"helloworld" and h["content-length"] == "10" and "transfer-encoding" not in h
              and h["x-test"] == "1", "chunked upstream response re-framed")
        st, h, body = px.request("GET", "/eof")
        check(body == b"until-close" * 100, "close-delimited upstream response")
        st, h, body = px.request("GET", "/interim")
        check(st == 200 and body == b"ok", "interim 1xx from the upstream skipped")

        print("status codes and HEAD")
        st, h, body = px.request("HEAD", "/head")
        check(st == 200 and body == b"" and h["content-length"] == "12345" and h["x-ms-meta-version"] == "20181001",
              "HEAD forwarded as HEAD, upstream Content-Length kept")
        check(up.seen[-1]["method"] == "HEAD", "upstream saw HEAD")
        st, h, body = px.request("GET", "/nocontent")
        check(st == 204 and body == b"" and h["x-ms-meta-version"] == "20181001", "204 with its headers")
        st, h, body = px.request("GET", "/err")
        check(st == 403 and body == b"nope", "upstream errors relayed as they are")
        st, h, body = px.request("GET", "/nowhere")
        check(st == 404 and body == b"Not found!", "upstream 404 relayed")

        print("failures")
        t0 = time.time()
        st, h, body = px.request("GET", "/slow")
        check(st == 504 and json.loads(body)["error"] == "upstream_timeout" and time.time() - t0 < 4,
              "slow upstream is a 504 after upstream_timeout")
        st, h, body = px.request("GET", "/bad")
        check(st == 502 and json.loads(body)["error"] == "upstream_bad_response", "garbage from the upstream is a 502")
        st, h, body = px.request("GET", "/truncated")
        check(st == 502, "truncated upstream body is a 502, never a short 200")
        st, h, body = px.request("GET", "/echo", headers={"Via": "1.1 hitman-server"})
        check(st == 508, "a request that already passed through this server is refused (loop)")

        print("request framing")
        check(px.raw(b"POST /echo HTTP/1.1\r\nHost: x\r\nContent-Length: 5\r\nTransfer-Encoding: chunked\r\n\r\n"
                     b"0\r\n\r\n") == 400, "Transfer-Encoding with Content-Length refused")
        check(px.raw(b"POST /echo HTTP/1.1\r\nHost: x\r\nContent-Length: 5\r\nContent-Length: 6\r\n\r\nhello!")
              == 400, "conflicting Content-Length refused")
        check(px.raw(b"POST /echo HTTP/1.1\r\nHost: x\r\nTransfer-Encoding: gzip\r\n\r\n") == 501,
              "unsupported Transfer-Encoding refused")
        check(px.raw(b"POST /echo HTTP/1.1\r\nHost: x\r\nTransfer-Encoding: chunked\r\n\r\nzz\r\nhi\r\n0\r\n\r\n")
              == 400, "non-hex chunk size refused")
        check(px.raw(b"POST /echo HTTP/1.1\r\nHost: x\r\nTransfer-Encoding: chunked\r\n\r\n"
                     b"fffffffffff\r\n") == 400, "oversized chunk size refused")
        check(px.raw(b"GET /echo HTTP/1.1\r\nHost: x\r\nX-A: 1\r\n folded\r\n\r\n") == 400, "folded header refused")
        check(px.raw(b"GET /echo HTTP/1.1\r\nHost: x\r\nBad Name: 1\r\n\r\n") == 400, "header name with space refused")
        check(px.raw(b"GET /echo HTTP/1.1\r\nHost: x\r\nContent-Length: +5\r\n\r\n") == 400, "signed Content-Length refused")

        print("status and access")
        st, h, body = px.request("GET", "/healthz")
        hz = json.loads(body)
        check(hz["mode"] == "proxy" and "reachable" in hz["upstream"] and "game token held" in hz["upstream"],
              "healthz reports proxy mode, upstream reachability and the session")
        n_before = len(up.seen)
        ip = lan_ip()
        if ip:
            st, h, body = px.request("GET", "/healthz", host=ip)
            check(st == 200, f"LAN peer ({ip}) gets /healthz")
            st, h, body = px.request("GET", "/echo", host=ip)
            check(st == 403 and len(up.seen) == n_before, "LAN peer cannot reach game routes (lan_access = health)")
        else:
            print("  --  no LAN address, LAN checks skipped")

        print("upstream down")
        up.close()
        time.sleep(0.2)
        t0 = time.time()
        st, h, body = px.request("GET", "/echo")
        check(st == 502 and json.loads(body)["error"] == "upstream_unreachable" and time.time() - t0 < 4,
              "unreachable upstream is a fast 502")
        st, h, body = px.request("GET", "/healthz")
        check("UNREACHABLE" in json.loads(body)["upstream"], "healthz shows the upstream down")
        st, h, body = px.request("POST", "/admin/shutdown")
        check(st == 200, "admin shutdown stays local in proxy mode")
        px.proc.wait(5)
    finally:
        px.stop()
        up.close()
    log = px.log()
    # Debug-level response previews show upstream bodies as received (the fake echoes the
    # target back); request lines, forms and tokens are what must never appear.
    req_lines = "\n".join(l for l in log.splitlines() if "proxy: <" not in l)
    check("secret%2Bvalue" not in req_lines and "token=<redacted>" in log, "secret query values redacted in the log")
    check("SECRETCODE" not in req_lines and "JWT-XYZ" not in req_lines, "auth code and token never logged")
    print(f"\n{passed} checks passed (log in {px.dir})")


if __name__ == "__main__":
    main(sys.argv[1])
