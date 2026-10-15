#!/usr/bin/env python3
"""End-to-end test: replays the PS5 connection flow (network map section 2) against a host
build of hitman-server and checks responses and stored rows against sections 4 and 7.

usage: python3 -I test/flow_test.py build/hitman-server-host
       python3 -I test/flow_test.py --remote 192.168.0.37:8000   (protocol checks against a running server)
"""
import http.client
import json
import os
import re
import socket
import sqlite3
import subprocess
import sys
import tempfile
import time

ACCOUNT_ID = "1234567890123456789"
EXTENSIONS = ["inventory", "progression", "transactions", "contracts", "gameclient",
              "gamepersistentdata", "opportunityprogression", "achievements"]

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


class Server:
    def __init__(self, binary, data_dir, port, host="127.0.0.1"):
        self.binary, self.data_dir, self.port, self.host = binary, data_dir, port, host
        self.proc = None
        self.config = os.path.join(data_dir, "config.ini") if data_dir else None
        if not self.config:
            return
        with open(self.config, "w") as f:
            f.write(f"port = {port}\nbind = 127.0.0.1\ndata_dir = {data_dir}\npatch = false\nlog_level = debug\n")

    def start(self):
        self.proc = subprocess.Popen([self.binary, self.config], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        for _ in range(100):
            try:
                if self.request("GET", "/healthz")[0] == 200:
                    return
            except OSError:
                time.sleep(0.05)
        raise RuntimeError("server did not start")

    def stop(self):
        if self.proc and self.proc.poll() is None:
            self.proc.terminate()
            self.proc.wait(5)

    def request(self, method, path, body=None, headers=None, chunked=False):
        conn = http.client.HTTPConnection(self.host, self.port, timeout=10)
        hdrs = {"Accept": "application/json"}
        hdrs.update(headers or {})
        if chunked:
            # Like the game's curl: a READFUNCTION body of unknown size goes out chunked with
            # Expect: 100-continue.
            hdrs["Transfer-Encoding"] = "chunked"
            hdrs["Expect"] = "100-continue"
            data = body.encode() if isinstance(body, str) else body
            conn.request(method, path, body=iter([data[:7], data[7:]]), headers=hdrs, encode_chunked=True)
        else:
            conn.request(method, path, body=body, headers=hdrs)
        resp = conn.getresponse()
        raw = resp.read()
        conn.close()
        ctype = resp.getheader("Content-Type") or ""
        return resp.status, (json.loads(raw) if raw and "json" in ctype else raw)

    def rpc(self, token, service, method, payload):
        return self.request("POST", f"/authentication/api/userchannel/{service}/{method}", json.dumps(payload),
                            {"Content-Type": "application/json", "Authorization": f"bearer {token}",
                             "Version": "8.12.0"})


def oauth_form(online_id):
    # Field order and names from network map 4.2.
    return "&".join([
        "grant_type=external_psn", f"psn_userid={ACCOUNT_ID}", "pId=1", "authcode=AbC123",
        "psn_issuerid=256", f"psn_onlineid={online_id}", "fsp=0", "product_id=EP0001-PPSA16387_00",
        "psn_cid=cid", "device_type=ps5", "device_id=dev0", "audience=ps4_prod", "locale=en", "rgn=eu", "gs=1",
    ])


def main(binary, remote=None):
    if remote:
        host, port = remote.rsplit(":", 1)
        data_dir, srv = None, Server(None, None, int(port), host)
        base = f"http://{remote}"
    else:
        data_dir = tempfile.mkdtemp(prefix="hitman-server-test-")
        port = free_port()
        srv = Server(os.path.abspath(binary), data_dir, port)
        srv.start()
        base = f"http://127.0.0.1:{port}"
    try:
        print("config (4.1)")
        st, cfg = srv.request("GET", "/config/ps4-prod/8.12.0?issuer=256")
        check(st == 200 and len(cfg["Versions"]) == 1, "one Versions[] entry")
        v = cfg["Versions"][0]
        check(v["Name"] == "ps4-prod", "Name echoes online_VersionConfigName")
        check(v["GAME_VER"] == "8.12.0", "GAME_VER 8.12.0")
        # Matches Peacock's ServerVersionConfig: ISSUER_ID present, no Online.* here (those live in
        # onlineconfig.json), OAuthPath has no leading slash.
        check(v["ISSUER_ID"] == "256", "ISSUER_ID echoes the issuer query")
        sv = v["SERVER_VER"]
        check(sv["GlobalAuthentication"] == {"AuthenticationHost": base, "OAuthPath": "oauth/token",
                                             "RequestedAudience": "ps4-prod_8"}, "GlobalAuthentication host/path/audience")
        check(sv["Authentication"] == {"AuthenticationHost": base,
                                       "UserChannelServicePath": "authentication/api/userchannel",
                                       "AuthenticationServicePathConfiguration": "authentication/api/configuration"},
              "Authentication host/paths")
        check(sv["Configuration"]["Url"] == f"{base}/files/onlineconfig.json", "Configuration.Url is onlineconfig.json")
        check(sv["Metrics"]["MetricsServerHost"] == base and set(sv) == {
            "Metrics", "Authentication", "GlobalAuthentication", "Configuration", "Resources"}, "SERVER_VER sections")

        print("onlineconfig.json")
        st, oc = srv.request("GET", "/files/onlineconfig.json")
        check(st == 200 and oc["Online"]["WebSocketsEnabled"] is False, "onlineconfig has Online.WebSocketsEnabled false")
        check(oc["Online"]["WebSocketsRoute"] == "authentication/ws" and "Contracts" in oc, "onlineconfig shape")

        print("oauth (4.2/4.3)")
        st, tok = srv.request("POST", "/oauth/token", oauth_form("Agent47"),
                              {"Content-Type": "application/x-www-form-urlencoded"}, chunked=True)
        check(st == 200 and tok["token_type"] == "bearer", "external_psn login over chunked + 100-continue")
        check(isinstance(tok["expires_in"], int) and tok["access_token"] and tok["refresh_token"], "token fields")
        access, refresh = tok["access_token"], tok["refresh_token"]
        st, err = srv.request("POST", "/oauth/token", "grant_type=steam&x=1",
                              {"Content-Type": "application/x-www-form-urlencoded"})
        check(st == 400 and err["error"] == "unsupported_grant_type", "other grant types refused")
        st, err = srv.request("POST", "/oauth/token", "grant_type=external_psn",
                              {"Content-Type": "application/x-www-form-urlencoded"})
        check(st == 400 and err["error"] == "invalid_request", "psn_userid required")

        print("game auth Init (4.4)")
        st, init = srv.request("GET", "/authentication/api/configuration/Init?configName=ps4-prod&lockedContentDisabled=false"
                               "&isFreePrologueUser=false&isIntroPackUser=false&isFullExperienceUser=false",
                               headers={"Authorization": f"bearer {access}"})
        check(st == 200 and re.fullmatch(r"[0-9a-f-]{36}", init["profileid"]), "profileid is a uuid")
        check(init["ias"] != 0, "ias non-zero (0 means SignupRequired)")
        check(set(init["blobconfig"]) == {"bloburl", "blobsig", "blobsigduration"}
              and init["blobconfig"]["bloburl"].endswith("/"), "blobconfig")
        # Peacock sends a non-empty blobsig and a token field; an empty blobsig made HITMAN reject.
        check(init["blobconfig"]["blobsig"].startswith("?sv="), "blobsig non-empty")
        check(init["token"] and init["profileid"] in init["token"], "Init token field present")
        check(init["serverversion"] == "8.12.0" and init["servertimeutc"].endswith("Z"), "serverversion, servertimeutc")
        profile_id = init["profileid"]
        # Over http the client sends no Authorization header at all (sub_c43dee only adds it for
        # https/wss). Requests from the console itself resolve to the token the game got from
        # its own /oauth/token: the same profile, never a separate fallback one.
        st, init_noauth = srv.request("GET", "/authentication/api/configuration/Init?configName=ps4-prod")
        check(st == 200 and init_noauth["profileid"] == profile_id, "Init without Authorization is the game's own login")
        st, init2 = srv.request("GET", "/authentication/api/configuration/Init", headers={"Authorization": "bearer nope"})
        check(st == 200 and init2["profileid"] == profile_id, "Init with a stale token from the console: same profile")

        print("offline cache diff, profile (4.6)")
        st, diff = srv.rpc(access, "AuthenticationService", "GetBlobOfflineCacheDatabaseDiff", {})
        check(st == 200 and diff == [], "GetBlobOfflineCacheDatabaseDiff lists no menu files standalone mode cannot serve")
        st, _ = srv.rpc(access, "ProfileService", "UpdateUserSaveFileTable",
                        {"clientSaveFileList": [], "deletedSaveFileList": []})
        check(st == 204, "UpdateUserSaveFileTable with the captured empty body is 204")
        st, _ = srv.rpc(access, "ProfileService", "UpdateUserSaveFileTable", {"clientSaveFileList": {}})
        check(st == 400, "UpdateUserSaveFileTable with a non-array list is 400")
        st, prof = srv.rpc(access, "ProfileService", "GetProfile", {"id": profile_id, "extensions": EXTENSIONS})
        check(st == 200 and prof["Id"] == profile_id, "GetProfile Id")
        check(prof["LinkedAccounts"] == {"ps4": ACCOUNT_ID, "ps4onlineid": "Agent47"}, "LinkedAccounts ps4/ps4onlineid")
        check(prof["Gamertag"] == "Agent47" and prof["Extensions"] == {} and prof["Version"] == 1, "fresh profile")
        etag1 = prof["ETag"]
        st, _ = srv.rpc(access, "ProfileService", "SynchronizeOfflineUnlockables", {})
        check(st == 204, "SynchronizeOfflineUnlockables")
        data = {"gamepersistentdata": {"prologue": {"proceed-intro": True}, "VideoShown": True},
                "opportunityprogression": {"b3b4e8c1": True}, "inventory": []}
        st, echo = srv.rpc(access, "ProfileService", "UpdateExtensions", {"id": profile_id, "extensionsData": data})
        check(st == 200 and echo == data, "UpdateExtensions echoes extensionsData")
        st, prof = srv.rpc(access, "ProfileService", "GetProfile", {"id": profile_id, "extensions": ["gamepersistentdata"]})
        check(prof["Extensions"] == {"gamepersistentdata": data["gamepersistentdata"]}, "extensions filtered and stored")
        check(prof["Version"] == 2 and prof["ETag"] != etag1, "Version and ETag move on update")
        st, _ = srv.rpc(access, "ProfileService", "UpdateExtensions", {"id": profile_id})
        check(st == 400, "UpdateExtensions without extensionsData is 400")

        print("events (4.7)")
        events = [
            {"Name": "ContractStart", "Value": {"Loadout": []}, "ContractSessionId": "sess-1", "ContractId": "c-1",
             "Timestamp": 1.5, "Origin": "gameclient", "UserId": profile_id, "Version": {"_Major": 8}, "Id": "e1"},
            {"Name": "Kill", "Value": {"RepositoryId": "x"}, "ContractSessionId": "sess-1", "ContractId": "c-1",
             "Timestamp": 2.25, "UserId": profile_id, "Version": {"_Major": 8}, "XPGain": 150, "Id": "e2"},
        ]
        body = {"userId": profile_id, "values": events, "lastEventTicks": 0, "clienttype": "ps5", "lastPushDt": 0}
        st, ev = srv.rpc(access, "EventsService", "SaveAndSynchronizeEvents4", body)
        check(st == 200 and set(ev) == {"SavedTokens", "NewEvents", "NextPoll", "PushMessages"}, "response keys")
        check(len(ev["SavedTokens"]) == 2 and int(ev["SavedTokens"][1]) > int(ev["SavedTokens"][0]), "one token per event")
        check(ev["NewEvents"] is None and ev["PushMessages"] is None and ev["NextPoll"] == 10, "no server events")
        body["values"] = events  # retransmission of the same batch
        st, ev_again = srv.rpc(access, "EventsService", "SaveAndSynchronizeEvents4", body)
        check(st == 200 and ev_again["SavedTokens"] == ev["SavedTokens"], "retransmitted events get their first ticks back")
        body["values"] = [dict(events[1], Id="e3")]
        st, ev2 = srv.rpc(access, "EventsService", "SaveAndSynchronizeEvents4", body)
        check(int(ev2["SavedTokens"][0]) > int(ev["SavedTokens"][1]), "ticks keep increasing")
        st, _ = srv.rpc(access, "EventsService", "SaveAndSynchronizeEvents4", dict(body, userId=str(profile_id)[::-1]))
        check(st == 403, "events for another userId are refused")
        body["values"] = []
        st, ev3 = srv.rpc(access, "EventsService", "SaveAndSynchronizeEvents4", body)
        check(st == 200 and ev3["SavedTokens"] is None, "empty poll")

        print("errors and side routes")
        st, prof_fb = srv.rpc("bad", "ProfileService", "GetProfile", {})
        check(st == 200 and prof_fb["Id"] == profile_id, "RPC from the console with an unknown token: the console's login")
        st, _ = srv.rpc(access, "ChallengesService", "GetProgression", {})
        check(st == 404, "unimplemented RPC is 404 (and logged)")
        st, _ = srv.request("GET", "/authentication/api/userchannel/ProfileService/GetProfile")
        check(st == 405, "wrong method is 405")
        st, _ = srv.request("POST", "/api/metrics/add", "{}", {"Content-Type": "application/json"})
        check(st == 204, "metrics sink")
        st, _ = srv.request("GET", "/resources/blob/dynamic_resources_c_x.rpkg")
        check(st == 404, "missing dynamic resource is 404")
        if not remote:
            os.makedirs(os.path.join(data_dir, "resources", "blob"))
            with open(os.path.join(data_dir, "resources", "blob", "r.rpkg"), "wb") as f:
                f.write(b"\x00RPKG" * 1000)
            st, raw = srv.request("GET", "/resources/blob/r.rpkg")
            check(st == 200 and raw == b"\x00RPKG" * 1000, "resource file served")
            with open(os.path.join(data_dir, "resources", "dynamic_resources_h3.rpkg"), "wb") as f:
                f.write(b"RPKG2" + bytes(range(256)) * 4)
            st, raw = srv.request("GET", "/resources-8-25/dynamic_resources_ps5_release_rpkg?sv=2018-03-28&ver=h3")
            check(st == 200 and raw == b"RPKG2" + bytes(range(256)) * 4, "PS5 release rpkg name maps to the H3 package")
            st, _ = srv.request("GET", "/resources-8-25/dynamic_resources_pc_release_rpkg")
            check(st == 404, "other platform rpkg names are not aliased")
        st, _ = srv.request("GET", "/resources/../hitman.db")
        check(st in (400, 404), "no path traversal")
        st, _ = srv.request("GET", "/ws", headers={"Upgrade": "websocket", "Connection": "Upgrade"})
        check(st == 501, "WebSocket upgrade refused")

        print("refresh and re-login")
        st, tok2 = srv.request("POST", "/oauth/token", f"grant_type=refresh_token&refresh_token={refresh}",
                               {"Content-Type": "application/x-www-form-urlencoded"})
        check(st == 200 and tok2["access_token"] != access, "refresh_token grant rotates tokens")
        old_access = access
        st, err = srv.request("POST", "/oauth/token", f"grant_type=refresh_token&refresh_token={refresh}",
                              {"Content-Type": "application/x-www-form-urlencoded"})
        check(st == 400 and err["error"] == "invalid_grant", "old refresh token revoked")
        st, tok3 = srv.request("POST", "/oauth/token", oauth_form("Agent47_2"),
                               {"Content-Type": "application/x-www-form-urlencoded"})
        st, prof = srv.rpc(tok3["access_token"], "ProfileService", "GetProfile", {"extensions": EXTENSIONS})
        check(prof["Id"] == profile_id and prof["Gamertag"] == "Agent47_2", "same psn_userid, same profile, new online id")
        check(prof["Extensions"]["opportunityprogression"] == {"b3b4e8c1": True}, "extensions kept across logins")

        if remote:
            st, _ = srv.request("POST", "/admin/shutdown")
            check(st == 403, "shutdown refused from the network")
            print(f"\n{passed} checks passed against {remote}")
            return

        print("persistence")
        srv.stop()
        srv.start()
        st, prof = srv.rpc(tok3["access_token"], "ProfileService", "GetProfile", {"extensions": ["inventory"]})
        check(st == 200 and prof["Extensions"] == {"inventory": []}, "tokens and profile survive a restart")

        print("schema (section 7)")
        db = sqlite3.connect(os.path.join(data_dir, "hitman.db"))
        tables = {r[0] for r in db.execute("SELECT name FROM sqlite_master WHERE type='table'")}
        check(tables == {"users", "auth_tokens", "profile_extensions", "profile_meta", "events", "contract_sessions"},
              "exactly the six tables")
        pk = lambda t: [r[1] for r in sorted(db.execute(f"PRAGMA table_info({t})"), key=lambda r: r[5]) if r[5]]
        check(pk("profile_extensions") == ["user_id", "name"] and pk("events") == ["user_id", "ticks"], "composite keys")
        fks = {t: {(r[3], r[2], r[4]) for r in db.execute(f"PRAGMA foreign_key_list({t})")} for t in tables}
        check(all(fks[t] == {("user_id", "users", "id")} for t in tables - {"users"}), "user_id foreign keys")
        check(db.execute("SELECT name FROM sqlite_master WHERE type='index' AND name='events_session'").fetchone(),
              "events_session index")
        rows = db.execute("SELECT name, contract_session_id, contract_id, timestamp, origin, value, xp_gain FROM events "
                          "ORDER BY ticks").fetchall()
        check(len(rows) == 3 and rows[0][:5] == ("ContractStart", "sess-1", "c-1", 1.5, "gameclient")
              and json.loads(rows[0][5]) == {"Loadout": []} and rows[1][6] == 150, "event columns")
        check(db.execute("SELECT id, user_id, contract_id FROM contract_sessions").fetchall()
              == [("sess-1", profile_id, "c-1")], "contract session recorded")
        users = db.execute("SELECT psn_account_id, psn_online_id, issuer_id, region, locale FROM users").fetchall()
        check((ACCOUNT_ID, "Agent47_2", "256", "eu", "en") in users, "user row keyed on psn_userid")
        check(len(users) == 1, "one console login, one profile (no fallback profile created)")
        check(not db.execute("SELECT 1 FROM auth_tokens WHERE access_token = ?", (old_access,)).fetchone(),
              "the rotated-out access token is gone")
        check(db.execute("SELECT name FROM sqlite_master WHERE type='index' AND name='events_user_event_id'").fetchone(),
              "event Id uniqueness index")
        db.close()

        print("takeover")
        st, _ = srv.request("POST", "/admin/shutdown")
        check(st == 200, "loopback shutdown accepted")
        srv.proc.wait(5)
        check(srv.proc.returncode == 0, "server exited cleanly")
    finally:
        srv.stop()
    with open(os.path.join(data_dir, "server.log")) as f:
        log = f.read()
    check("clienttype=ps5" in log, "clienttype logged with the event batch")
    check("authcode=<redacted>" in log and "AbC123" not in log, "auth code never logged")
    check("psn_cid=<redacted>" in log and "device_id=<redacted>" in log, "device identifiers redacted")
    print(f"\n{passed} checks passed (data in {data_dir})")


if __name__ == "__main__":
    if sys.argv[1] == "--remote":
        main(None, sys.argv[2])
    else:
        main(sys.argv[1])
