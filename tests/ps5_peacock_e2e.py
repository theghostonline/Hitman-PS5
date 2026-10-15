#!/usr/bin/env python3
"""End-to-end test of the PS5 port on one machine: a throwaway full Peacock (upstream/Peacock,
branch ps5-port, production build) behind the hitman-server proxy (host build), driven with the
PS5 client's own request sequence and headers as seen in the console's server.log and the Ghidra
service map (docs/PS5_API_MATRIX.csv). The game sends no Authorization header over http; the
proxy supplies the game's own token, as on the console.

usage: python3 -I tests/ps5_peacock_e2e.py [--keep]
Needs: server/build/hitman-server-host (make -C server host), upstream/Peacock/chunk0.js
(scripts/backend.sh build) and .tools/bin/node.
"""
import hashlib
import http.client
import json
import os
import shutil
import socket
import subprocess
import sys
import tempfile
import time
import urllib.parse
import uuid

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PEACOCK = os.path.join(ROOT, "upstream", "Peacock")
NODE = os.path.join(ROOT, ".tools", "bin", "node")
PROXY = os.path.join(ROOT, "server", "build", "hitman-server-host")

# From the console log of 2026-10-15 08:18 (identity changed to a test account).
ACCOUNT_ID = "1765932407398679000"
ONLINE_ID = "TestAgent47"
GAME_HEADERS = {"Accept": "application/json", "User-Agent": "hitman6/6.0/sceCURL"}
RPKG_SHA256 = "ca44edc8e4d4e2805796a2574f0c91168e3312e39bd5d9373490af1e1594d09a"
SHOWSTOPPER = "00000000-0000-0000-0000-000000000200"
NOVIKOV = "052434e7-f451-462f-a9d7-13657cb047c0"
MARGOLIS = "edad702b-5b37-4dc1-a47c-36a1588f1d3f"

passed = 0


def check(cond, what, detail=None):
    global passed
    if not cond:
        raise AssertionError(f"{what}" + (f": {detail}" if detail is not None else ""))
    passed += 1
    print(f"  ok  {what}")


def free_port():
    with socket.socket() as s:
        s.bind(("127.0.0.1", 0))
        return s.getsockname()[1]


def wait_http(port, path, tries=150):
    for _ in range(tries):
        try:
            c = http.client.HTTPConnection("127.0.0.1", port, timeout=2)
            c.request("GET", path)
            r = c.getresponse()
            r.read()
            c.close()
            return r.status
        except OSError:
            time.sleep(0.1)
    raise RuntimeError(f"nothing answers on :{port}{path}")


class Game:
    """Talks to the proxy the way the PS5 client does."""

    def __init__(self, port):
        self.port = port
        self.host = f"127.0.0.1:{port}"

    def req(self, method, path, body=None, headers=None):
        c = http.client.HTTPConnection("127.0.0.1", self.port, timeout=30)
        h = dict(GAME_HEADERS)
        h.update(headers or {})
        c.request(method, path, body=body, headers=h)
        r = c.getresponse()
        raw = r.read()
        hd = {k.lower(): v for k, v in r.getheaders()}
        c.close()
        data = raw
        if raw and ("json" in hd.get("content-type", "") or raw[:1] in (b"{", b"[", b'"')):
            try:
                data = json.loads(raw)
            except ValueError:
                pass
        return r.status, hd, data

    def rpc(self, service, method, payload):
        # sub_c43302: accept + content-type application/json, Version = the served GAME_VER.
        return self.req("POST", f"/authentication/api/userchannel/{service}/{method}", json.dumps(payload),
                        {"Content-Type": "application/json", "Version": "8.12.0"})

    def page(self, name, **query):
        q = ("?" + urllib.parse.urlencode(query)) if query else ""
        return self.req("GET", f"/profiles/page/{name}{q}")


def main(keep):
    work = tempfile.mkdtemp(prefix="ps5-e2e-")
    pk_dir, px_dir = os.path.join(work, "peacock"), os.path.join(work, "proxy")
    os.makedirs(pk_dir)
    os.makedirs(px_dir)
    with open(os.path.join(pk_dir, "options.ini"), "w") as f:
        f.write("[peacock]\nupdateChecking=false\nleaderboards=false\nps5Entitlements=ALL\n")
    pk_port, px_port = free_port(), free_port()
    pk_log = open(os.path.join(pk_dir, "peacock.log"), "w")
    pk = subprocess.Popen([NODE, os.path.join(PEACOCK, "chunk0.js")], cwd=pk_dir, stdout=pk_log,
                          stderr=subprocess.STDOUT, env=dict(os.environ, PORT=str(pk_port), HOST="127.0.0.1"))
    with open(os.path.join(px_dir, "config.ini"), "w") as f:
        f.write(f"port = {px_port}\nbind = 127.0.0.1\ndata_dir = {px_dir}\npatch = false\nlog_level = debug\n"
                f"upstream = http://127.0.0.1:{pk_port}\n")
    px = subprocess.Popen([PROXY, os.path.join(px_dir, "config.ini")], stdout=subprocess.DEVNULL,
                          stderr=subprocess.DEVNULL)
    try:
        wait_http(pk_port, "/files/onlineconfig.json")
        wait_http(px_port, "/healthz")
        g = Game(px_port)
        base = f"http://{g.host}"

        print("connection flow (states 4-24)")
        st, h, cfg = g.req("GET", "/")
        v = cfg["Versions"][0] if isinstance(cfg, dict) else {}
        check(st == 200 and v.get("Name") == "ps4-prod" and v.get("GAME_VER") == "8.12.0",
              "GET / from the game is the ps4-prod config, GAME_VER 8.12.0", cfg)
        sv = v["SERVER_VER"]
        check(sv["GlobalAuthentication"]["RequestedAudience"] == "ps4-prod_8"
              and sv["GlobalAuthentication"]["AuthenticationHost"] == base
              and sv["Authentication"]["AuthenticationHost"] == base
              and sv["Configuration"]["Url"] == f"{base}/files/onlineconfig.json",
              "every host in the config points back at the console proxy")
        c = http.client.HTTPConnection("127.0.0.1", pk_port, timeout=10)
        c.request("GET", "/", headers={"User-Agent": "Mozilla/5.0"})
        r = c.getresponse()
        web = r.read()
        c.close()
        check(r.status == 200 and b"<html" in web.lower(), "a browser on / still gets the Peacock web UI")
        st2, _, cfg2 = g.req("GET", "/config/ps4-prod/8_12_0?issuer=256")
        check(st2 == 200 and cfg2["Versions"][0]["Name"] == "ps4-prod" and cfg2["Versions"][0]["ISSUER_ID"] == "256",
              "the /config/ps4-prod/8_12_0 form gives the same config")
        st, _, oc = g.req("GET", "/files/onlineconfig.json")
        oc = json.loads(oc) if isinstance(oc, bytes) else oc
        check(st == 200 and oc["Online"]["WebSocketsEnabled"] is False, "onlineconfig (WebSockets off)")
        st, h, _ = g.req("HEAD", "/files/privacypolicy/hm3/privacypolicy_en.json")
        check(st == 200 and h.get("x-ms-meta-version") == "20181001", "privacy policy HEAD with x-ms-meta-version")
        st, h, pp = g.req("GET", "/files/privacypolicy/hm3/privacypolicy_en.json")
        check(st == 200 and b"gdprconsent" in (pp if isinstance(pp, bytes) else json.dumps(pp).encode()),
              "privacy policy GET (GDPR consent modal)")

        # The client's form, field order from the format string at 0x1c97fd9 (sub_c5acb6), sent
        # with the Content-Type: application/json it always claims.
        pid = str(uuid.uuid4())
        form = "&".join([
            "grant_type=external_psn", f"psn_userid={ACCOUNT_ID}", f"pId={pid}", "authcode=hitman-ps5-local",
            "psn_issuerid=256", f"psn_onlineid={ONLINE_ID}", "fsp=false", "product_id=PPSA16387",
            "psn_cid=58a9d334-a261-46a3-a6ee-c38036105e2f", "device_type=ps5", "device_id=2c:9e:00:00:00:00",
            "locale=en-GB", "rgn=en", "gs=ps4-prod_8"])
        st, _, tok = g.req("POST", "/oauth/token", form, {"Content-Type": "application/json"})
        check(st == 200 and tok.get("token_type") == "bearer" and tok.get("access_token"),
              "external_psn login (form body under an application/json header)", tok)
        st, _, ent = g.rpc("ProfileService", "SetClientEntitlements", {"entitlements": ["EP0000-PPSA16387_00-TEST"]})
        check(st == 200 and ent == "null", "SetClientEntitlements without Authorization (proxy adds the token)")
        st, _, init = g.req("GET", "/authentication/api/configuration/Init?configName=ps4-prod&lockedContentDisabled="
                                   "false&isFreePrologueUser=false&isIntroPackUser=false&isFullExperienceUser=true")
        check(st == 200 and init.get("profileid") == pid, "Init resolves to the profile the game logged in as", init)
        check(init["blobconfig"]["bloburl"] == f"{base}/resources-8-25/" and init["blobconfig"]["blobsig"]
              and init["serverversion"] == "8.12.0.0" and init["ias"] == 2, "Init blobconfig, serverversion, ias")
        st, _, diff = g.rpc("AuthenticationService", "GetBlobOfflineCacheDatabaseDiff", {"productVersion": "8.12.0"})
        check(st == 200 and isinstance(diff, list), "GetBlobOfflineCacheDatabaseDiff", diff)
        for path in diff:
            st, _, _ = g.req("GET", f"/resources-8-25/{path}?sv=2018-03-28&ver=h3")
            check(st == 200, f"menu override {path} is served")
        st, h, _ = g.req("HEAD", "/resources-8-25/dynamic_resources_ps5_release_rpkg?sv=2018-03-28&ver=h3")
        check(st == 200 and int(h["content-length"]) == 88481, "dynamic resources HEAD", h)
        st, h, rp = g.req("GET", "/resources-8-25/dynamic_resources_ps5_release_rpkg?sv=2018-03-28&ver=h3")
        check(st == 200 and hashlib.sha256(rp).hexdigest() == RPKG_SHA256,
              "dynamic_resources_ps5_release_rpkg is the H3 package the console mounted, byte for byte")

        print("profile bootstrap (states 24-39)")
        st, _, _ = g.rpc("ProfileService", "UpdateUserSaveFileTable", {"clientSaveFileList": [], "deletedSaveFileList": []})
        check(st == 204, "UpdateUserSaveFileTable (the request that followed state 24 on the console)")
        exts = ["inventory", "progression", "transactions", "contracts", "gameclient", "gamepersistentdata",
                "opportunityprogression", "achievements"]
        st, _, prof = g.rpc("ProfileService", "GetProfile", {"id": pid, "extensions": exts})
        check(st == 200 and prof["Id"] == pid and prof["LinkedAccounts"].get("ps4") == ACCOUNT_ID
              and prof["Gamertag"] == ONLINE_ID and prof["PSNAccountId"] == ACCOUNT_ID, "GetProfile: PSN-linked profile", prof)
        check("progression" in prof["Extensions"] and "gamepersistentdata" in prof["Extensions"],
              "GetProfile returns the requested extensions Peacock keeps")
        st, _, _ = g.rpc("ProfileService", "GetProfile", {"id": str(uuid.uuid4()), "extensions": exts})
        check(st == 403, "GetProfile for another profile id is refused")
        st, _, _ = g.rpc("ProfileService", "SynchronizeOfflineUnlockables", {"unlockableIds": []})
        check(st == 204, "SynchronizeOfflineUnlockables")
        st, _, stats = g.rpc("ProfileService", "SynchroniseGameStats", {"profileId": pid, "localStats": {}})
        check(st == 200 and isinstance(stats.get("Inventory"), list) and len(stats["Inventory"]) > 50,
              f"SynchroniseGameStats returns the inventory ({len(stats.get('Inventory') or [])} items)")
        st, _, uc = g.rpc("ProfileService", "GetUserConfig", {"id": pid})
        check(st == 200 and uc == {}, "GetUserConfig")
        st, _, fc = g.rpc("ProfileService", "GetFriendsCount", {})
        check(st == 200 and fc in (0, b"0"), "GetFriendsCount (upstream answers the text 0)", fc)
        st, _, rp_ = g.rpc("ProfileService", "ResolveProfiles", {"profileIDs": [pid]})
        check(st == 200 and rp_[0]["Id"] == pid, "ResolveProfiles")
        st, _, gt = g.rpc("ProfileService", "ResolveGamerTags", {"profileIds": [pid]})
        check(st == 200 and gt.get("ps4", {}).get(pid) == ONLINE_ID, "ResolveGamerTags groups PSN players under ps4", gt)
        st, _, ux = g.rpc("ProfileService", "UpdateExtensions",
                          {"id": pid, "extensionsData": {"gamepersistentdata": {"prologue": {"proceed-intro": True}}}})
        check(st == 200, "UpdateExtensions")
        st, _, sl = g.rpc("ProfileService", "GetSemLinkStatus",
                          {"forceUpdate": False, "isFreePrologueUser": False, "isIntroPackUser": False,
                           "isFullExperienceUser": True})
        check(st == 200, "GetSemLinkStatus")
        st, _, ev = g.rpc("EventsService", "SaveAndSynchronizeEvents4",
                          {"userId": pid, "values": [], "lastEventTicks": 0, "clienttype": "ps5", "lastPushDt": 0})
        check(st == 200 and "NextPoll" in ev, "first event poll")

        print("menus")
        for name, query in [("Hub", {}), ("Destination", {"locationId": "LOCATION_PARENT_PARIS"}),
                            ("Planning", {"contractid": SHOWSTOPPER, "resetescalation": "false",
                                          "forcecurrentcontract": "false", "errorhandling": "false"}),
                            ("PlayerProfile", {}), ("Safehouse", {}),
                            ("ChallengeLocation", {"locationId": "LOCATION_PARIS"}),
                            ("MasteryDataForLocation", {"locationId": "LOCATION_PARENT_PARIS"}),
                            ("Contracts", {})]:
            st, _, pg = g.page(name, **query)
            ok = st == 200 and isinstance(pg, dict) and ("template" in pg or "data" in pg)
            check(ok, f"/profiles/page/{name}", (st, str(pg)[:300]))
        st, _, ch = g.rpc("ChallengesService", "GetActiveChallengesAndProgression",
                          {"contractId": SHOWSTOPPER, "difficultyLevel": 2})
        check(st == 200 and isinstance(ch, list) and len(ch) > 0, f"GetActiveChallengesAndProgression ({len(ch)})")
        st, _, mc = g.rpc("HubPagesService", "GetMasteryCompletionDataForLocation", {"locationId": "LOCATION_PARIS"})
        check(st == 200 and isinstance(mc, dict), "GetMasteryCompletionDataForLocation")
        st, _, tree = g.rpc("HubPagesService", "GetChallengeTreeFor", {"contractId": SHOWSTOPPER, "difficultyLevel": 2})
        check(st == 200, "GetChallengeTreeFor")

        print("mission: The Showstopper")
        st, _, play = g.rpc("ContractsService", "GetForPlay2", {"id": SHOWSTOPPER, "locationId": "LOCATION_PARIS",
                                                               "extraGameChangerIds": [], "difficultyLevel": 2})
        check(st == 200 and play["Contract"]["Metadata"]["Id"] == SHOWSTOPPER and play["ContractSessionId"],
              "GetForPlay2 resolves the contract")
        st, _, sid = g.rpc("ContractSessionsService", "Start",
                           {"profileId": pid, "contractId": SHOWSTOPPER, "difficultyLevel": 2})
        check(st == 200 and isinstance(sid, str) and sid.count("-") >= 5, "ContractSessionsService/Start (PS5 path)", sid)
        st, _, _ = g.rpc("ContractSessionsService", "Start", {"profileId": str(uuid.uuid4()), "contractId": SHOWSTOPPER})
        check(st == 400, "Start for another profile is refused")

        def ev(name, value, t):
            return {"Name": name, "Value": value, "ContractSessionId": sid, "ContractId": SHOWSTOPPER,
                    "Timestamp": t, "Id": str(uuid.uuid4()), "UserId": pid, "Origin": "gameclient",
                    "Version": {"_Major": 8, "_Minor": 12, "_Build": 0, "_Revision": 0}}

        kill = lambda rid, t: ev("Kill", {"RepositoryId": rid, "IsTarget": True, "KillType": "EKillType_Pistol",
                                          "KillMethodBroad": "pistol", "KillMethodStrict": "", "Accident": False,
                                          "WeaponSilenced": True, "Explosive": False, "Projectile": False,
                                          "Sniper": False, "IsHeadshot": True, "OutfitIsHitmanSuit": True,
                                          "KillClass": "ballistic", "KillItemCategory": "pistol",
                                          "ActorType": 0, "ActorId": 1}, t)
        batch1 = [ev("ContractStart", {"Loadout": [], "Disguise": "4a8fa1ca-6bd4-4ff2-9b36-2c0f0c3c2a3b",
                                       "LocationId": "", "GameChangers": [], "ContractType": "Mission",
                                       "DifficultyLevel": 2, "IsHitmanSuit": True, "SelectedCharacterId": ""}, 1.0),
                  ev("IntroCutEnd", {}, 2.0), kill(NOVIKOV, 120.0)]
        body = {"userId": pid, "values": batch1, "lastEventTicks": 0, "clienttype": "ps5", "lastPushDt": 0}
        st, _, r1 = g.rpc("EventsService", "SaveAndSynchronizeEvents4", body)
        check(st == 200 and len(r1["SavedTokens"]) == 3, "events: start, intro, first target")
        st, _, r1b = g.rpc("EventsService", "SaveAndSynchronizeEvents4", body)
        check(st == 200 and len(r1b["SavedTokens"]) == 3, "the same batch resent (lost response) is acknowledged")
        batch2 = [kill(MARGOLIS, 300.0), ev("exit_gate", {}, 400.0), ev("ContractEnd", {}, 401.0)]
        st, _, r2 = g.rpc("EventsService", "SaveAndSynchronizeEvents4", dict(body, values=batch2))
        check(st == 200 and len(r2["SavedTokens"]) == 3, "events: second target, exit, end")
        st, _, me = g.page("missionend", contractSessionId=sid)
        data = me.get("data", {}) if isinstance(me, dict) else {}
        so = data.get("ScoreOverview", {})
        check(st == 200 and so, "mission end screen computed", (st, str(me)[:400]))
        xp = data.get("XpGained", data.get("Xp", None))
        check(so.get("ContractScore", {}).get("Total", 0) > 0 or so.get("Score", 0) > 0,
              f"score is positive ({so.get('ContractScore', {}).get('Total', so.get('Score'))})")
        st, _, mr = g.page("missionrewards", contractSessionId=sid)
        check(st == 200 and isinstance(mr, dict), "mission rewards page", (st, str(mr)[:300]))
        st, _, prof2 = g.rpc("ProfileService", "GetProfile", {"id": pid, "extensions": ["progression"]})
        prog = prof2["Extensions"]["progression"]
        paris = prog.get("Locations", {}).get("LOCATION_PARENT_PARIS", {})
        check(prog.get("PlayerProfileXP", {}).get("Total", 0) > 0 and paris.get("Xp", 0) > 0,
              f"player XP {prog.get('PlayerProfileXP', {}).get('Total')} and Paris mastery XP {paris.get('Xp')} granted",
              json.dumps(prog)[:500])
        pk_log.flush()
        with open(os.path.join(pk_dir, "peacock.log")) as f:
            replays = f.read().count("already processed for session")
        check(replays == 3, f"the engine skipped all 3 replayed events ({replays})")

        print("restart: durability")
        xp_before = prog["PlayerProfileXP"]["Total"]
        pk.terminate()
        pk.wait(10)
        saved = os.path.join(pk_dir, "userdata", "users", f"{pid}.json")
        check(os.path.exists(saved), "the profile was written to disk on shutdown")
        shutil.copy(saved, os.path.join(work, "profile_at_shutdown.json"))
        pk = subprocess.Popen([NODE, os.path.join(PEACOCK, "chunk0.js")], cwd=pk_dir, stdout=pk_log,
                              stderr=subprocess.STDOUT, env=dict(os.environ, PORT=str(pk_port), HOST="127.0.0.1"))
        wait_http(pk_port, "/files/onlineconfig.json")
        # A fresh game launch: the client has its cached profile id and logs in again.
        st, _, tok2 = g.req("POST", "/oauth/token", form, {"Content-Type": "application/json"})
        check(st == 200, "login after the backend restarted")
        st, _, prof3 = g.rpc("ProfileService", "GetProfile", {"id": pid, "extensions": ["progression"]})
        check(st == 200 and prof3["Extensions"]["progression"]["PlayerProfileXP"]["Total"] == xp_before,
              "XP and mastery survive a backend restart")
        form_other = form.replace(f"pId={pid}", f"pId={uuid.uuid4()}")
        st, _, _ = g.req("POST", "/oauth/token", form_other, {"Content-Type": "application/json"})
        check(st == 410, "the PSN account cannot be re-linked to another profile id (410)")
        # A profile that exists but cannot be read must never be replaced by a fresh one.
        bad_pid, bad_acct = str(uuid.uuid4()), "1000000000000000777"
        bad_path = os.path.join(pk_dir, "userdata", "users", f"{bad_pid}.json")
        with open(bad_path, "w") as f:
            f.write('{"Id": "' + bad_pid + '", "Extensions": {"progression": ')
        with open(os.path.join(pk_dir, "userdata", "psnids", f"{bad_acct}.json"), "w") as f:
            f.write(bad_pid)
        bad_form = form.replace(f"pId={pid}", f"pId={bad_pid}").replace(ACCOUNT_ID, bad_acct)
        st, _, _ = g.req("POST", "/oauth/token", bad_form, {"Content-Type": "application/json"})
        with open(bad_path) as f:
            untouched = f.read().endswith('"progression": ')
        check(st == 500 and untouched, "an unreadable profile refuses the login and is left as it was")
        st, _, hz = g.req("GET", "/healthz")
        check(hz["mode"] == "proxy" and "game token held" in hz["upstream"], "proxy health")
    finally:
        px.terminate()
        pk.terminate()
        for p in (px, pk):
            try:
                p.wait(10)
            except subprocess.TimeoutExpired:
                p.kill()
        pk_log.close()
    with open(os.path.join(px_dir, "server.log")) as f:
        plog = f.read()
    check("hitman-ps5-local" not in plog and tok["access_token"] not in plog, "no auth code or token in the proxy log")
    print(f"\n{passed} checks passed ({work})")
    if not keep:
        shutil.rmtree(work, ignore_errors=True)


if __name__ == "__main__":
    main("--keep" in sys.argv)
