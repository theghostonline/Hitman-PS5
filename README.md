# HITMAN 3 (PS5) network map, for a Peacock port

Target: `PPSA16387` HITMAN 3, content version 01.017.000, engine 3.155.0.0, build 6837802.
Dump: `/data/homebrew/PPSA16387-app0_HITMAN_3` on the console (ps5-app-dumper 2.10, decrypted).
eboot.bin sha256 `4a7aad3214f193bba0fcbf2b528eab779960acd16ab5e188665ae01454f35bed`.

Addresses are eboot virtual addresses (code VA == file offset - 0x4000, rodata VA == file offset - 0x4000).
"Verified" means I read it in the disassembly. "Inferred" means it is my reading and still needs a hardware check.

---

## 1. Short version

* The PS5 build talks to IOI with the **PS4 backend audience `ps4-prod`** and asks for **server version `8.12.0`**.
* Every IOI web call goes through **libcurl statically linked into the eboot** (class `ZHttpManagerOrbisCurl`), TLS by mbedTLS, trust store built from the console's system CAs (`sceSslGetCaList` / `sceSslGetCaCerts`). Sony's `sceHttp2` / `sceNpWebApi2` are only used for PSN calls.
* The request/response JSON matches what Peacock already serves on PC (config `Versions[]`, `/oauth/token`, `configuration/Init`, `userchannel/<Service>/<Method>`, `SaveAndSynchronizeEvents4`). The PS5-specific parts are:
  1. OAuth grant is **`external_psn`** (Peacock only knows steam/epic).
  2. Platform account key in `LinkedAccounts` is **`ps4`** plus **`ps4onlineid`**.
  3. Config audience/name is `ps4-prod`, `GAME_VER` major must be `8`.
  4. There is an optional **WebSocket** channel (`wss://`) that should be switched off in the config you serve.
* The config host is a named engine variable, `online_VersionConfigDomain` (default `config.hitman.io`), and `thumbs.dat` (XTEA, same key as PC) already runs `ConsoleCmd` lines. The scheme is hard-coded `https://`.

---

## 2. Connection flow

State labels come from the UI table that `ZOnlineManager` uses (`UI_CONNECTION_STATUS_*`, pointer array at `0x241b830`, index = state). The order is verified. The wire column is matched by name and by the code in section 3, not traced per state (indices 37-38 have no label).

| idx | state | what happens on the wire |
|---|---|---|
| 0-1 | OFFLINE | |
| 2,4 | BEGIN_CONNECTION_FLOW | |
| 3 | DISCONNECTED | |
| 5 | PREAUTHENTICATE | PSN sign-in check, `sceNpCheckNpReachability` |
| 6-8 | FETCH_API_VERSION / API_VERSION_RECEIVED | |
| 9-10 | FETCHING_CONFIGURATION / CONFIGURATION_RECEIVED | `GET https://{domain}/config/ps4-prod/8.12.0?issuer={psnIssuerId}` |
| 11 | ONLINE_CONSENT_CONFIRMATION | |
| 14-15 | AUTHENTICATING / AUTHENTICATION_RECEIVED | PSN auth code, then `POST {GlobalAuth host}{OAuthPath}` (`external_psn`) |
| 16-17 | ENTITLEMENTS_SYNCHRONIZE_* | `sceNpEntitlementAccess*` (local, PSN side) |
| 18-19 | AUTHENTICATING_GAMESERVICE / ..._RECEIVED | `GET {Auth host}/{AuthenticationServicePathConfiguration}/Init?...` |
| 20 | FSP_IOI_ACCOUNT_SIGNUP | free starter pack signup branch (`SignupRequired`) |
| 21-22 | FETCHING_OFFLINE_CACHE_DB_DIFF / RECEIVED | `AuthenticationService/GetBlobOfflineCacheDatabaseDiff` |
| 23-24 | FETCHING_DYNAMIC_RESOURCES / MOUNTED | `dynamic_resources_*` rpkg from `blobconfig.bloburl` |
| 26-27 | FETCHING_PLATFORM_USERINFO / RECEIVED | `sceNpGetOnlineId`, `sceNpGetAccountCountryA`, `sceUserServiceGetUserName` |
| 30-31 | FETCHING_PROFILE / PROFILE_RECEIVED | `ProfileService/GetProfile` |
| 32-33 | SYNCHRONIZING_PROFILE / PROFILE_SYNCHRONIZED | `ProfileService/SynchronizeOfflineUnlockables`, `UpdateExtensions` |
| 34-36 | RETRIEVING_EVENTS / EVENTS_RETRIEVED / WAITING_FOR_SYNCHRONIZING_EVENTS | `EventsService/SaveAndSynchronizeEvents4` |
| 39 | CONNECTED | |

### Code map

| function | role |
|---|---|
| `sub_c2d9ee` | static init: registers `OnlineResources_Disable`, `OnlineResources_ForceOfflineOnFailure`, `online_VersionConfigDomain` = `"config.hitman.io"` (char[256] at `0x3c6b510`), `online_VersionConfigName` = `"ps4-prod"` (char[256] at `0x3c6b628`) |
| `sub_c44010` | `Format("https://{0}", online_VersionConfigDomain)` |
| `sub_c431be` | `Format("{0}/config/{1}/{2}", sub_c44010(), online_VersionConfigName, "8.12.0")` |
| `sub_c7b0b8` | appends `"?issuer={0}"`, GETs it with `Accept: application/json` |
| `sub_c56280` | parses config response: walks `Versions[]`, reads `Name`, splits `GAME_VER` on `.`, keeps the entry whose major == client major (8) with the best minor/build, stores it as the live server config (`0x3c6bce0`), sets `OnlineVersionConfigName`; failure logs `Failed to get API version!` |
| `sub_15ecca` | lookup of a dotted path (`"SERVER_VER.X.Y"`) in the live server config |
| `sub_15eb9a` / `sub_15f4f0` | JSON object key lookup / value as string (used by every response parser) |
| `sub_c50246` | global auth host: config `SERVER_VER.GlobalAuthentication.AuthenticationHost`, fallback `"https://auth.hitman.io"` |
| `sub_c502da` | OAuth path: `SERVER_VER.GlobalAuthentication.OAuthPath`, fallback `"/oauth/token"` |
| `sub_c5aa84` | userchannel path: `SERVER_VER.Authentication.UserChannelServicePath`, fallback `"authentication/api/userchannel"` |
| `sub_c5ab08` | configuration path: `SERVER_VER.Authentication.AuthenticationServicePathConfiguration`, fallback `"authentication/api/configuration"` |
| `sub_c83194` | `sceNpAuthCreateAsyncRequest` + `sceNpAuthGetAuthorizationCodeV3` (PSN auth code) |
| `sub_c5acb6` | builds the `external_psn` OAuth body and sends it (calls `sub_c50246`, `sub_c502da`, `sub_f259c8`) |
| `sub_c508c6` | OAuth response parser |
| `sub_c5c22c` | `configuration/Init` request builder |
| `sub_c74414` | `ProcessGameAuthenticationResponse` (Init response) |
| `sub_c43302` | common userchannel request: sets `accept`, `content-type`, `authorization: bearer <token>`, `version` |
| `sub_c52c76` | `ProfileService/GetProfile` request |
| `sub_c53b92` | GetProfile response handling |
| `sub_c4b834` | profile identity parse (`LinkedAccounts`, `Gamertag`) |
| `sub_c6e5ee` | `EventsService/SaveAndSynchronizeEvents4` request |
| `sub_c47d4a` | config tunables dispatcher (`Online.*` keys, see 5.4) |
| `sub_f2a4xx` | the only place game code calls `curl_easy_setopt` |

**Unresolved gate.** `sub_c50246`, `sub_c502da`, `sub_c5aa84` and `sub_c5ab08` only use the config value when `[0x3c6b7d8] >= 9` or (`[0x3c6b7d8] == 4` and `[0x3c6b7dc] >= 9`), otherwise they return the hard-coded fallback.

What Ghidra shows about it:
* `0x3c6b7c0` is a global object with three vtable pointers (`+0`, `+8`, `+0x10`, multiple inheritance), then three ints at `+0x18`, `+0x1c`, `+0x20`, all initialised to `-5` in `sub_c309fa`. That layout fits current / previous / target state of `ZOnlineManager`, which would make the gate read "state >= 9, or back at state 4 after having reached >= 9".
* Values the code tests on `+0x18`: `2`, `4`, `>= 9`, and the set `{38, 41, 43}` (`sub_c42162`, `sub_c9309a`, `sub_ab0e92`). 38+ is past the 40 labelled UI states, so either the enum has unlabelled error states or this is not the UI state index.
* The only direct write is the `-5` init; the real writer goes through a base pointer and I did not find it.
* When `+0x18 == 2`, RPCs skip HTTP and call a per-RPC `std::function` slot instead (`0x3c6d020` .. about `0x3c6d6xx`, one per RPC; `GetProfile` = `0x3c6d0e0`, `SynchronizeOfflineUnlockables` = `0x3c6d410`). The slots are only zeroed in static init (`sub_c714af`, `sub_c5b670`); nothing visible assigns them. Together with the `SetUpTestCase` string this looks like an internal mock/test backend. It is also an in-process hook point: a payload that fills those slots and sets the mode to 2 could serve every RPC without network or TLS. Not tried.

Practical upshot: **redirect `auth.hitman.io` as well as `config.hitman.io`**, so the fallback path lands on your server too.

---

## 3. Endpoints

| step | method + URL | body |
|---|---|---|
| config | `GET https://{online_VersionConfigDomain}/config/{online_VersionConfigName}/8.12.0?issuer={psnIssuerId}` | none |
| OAuth | `POST {GlobalAuthentication.AuthenticationHost}{GlobalAuthentication.OAuthPath}` | form, see 4.2 |
| refresh | same URL | `grant_type=refresh_token&refresh_token={0}` |
| game auth | `GET {Authentication.AuthenticationHost}/{AuthenticationServicePathConfiguration}/Init?configName=..&lockedContentDisabled=..&isFreePrologueUser=..&isIntroPackUser=..&isFullExperienceUser=..` | none |
| RPC | `POST {Authentication.AuthenticationHost}/{UserChannelServicePath}/{Service}/{Method}` (`"{0}/{1}/{2}"`) | JSON |
| pages | `/profiles/page/...` (`{0}&locale={1}&rgn={2}` query) | |
| dynamic resources | `{0}dynamic_resources_c_{1}_{2}_{3}_s_{4}_{5}_{6}.rpkg`, `dynamic_resources_{0}_{1}_rpkg` | |
| websocket | `wss://{Authentication.AuthenticationHost}/{Online.WebSocketsRoute}` | |
| metrics | `SERVER_VER.Metrics.MetricsServerHost` + `MetricsServerServicePath` (default host `metrics.hitman.io`) | |

Hosts seen in the binary: `config.hitman.io`, `auth.hitman.io`, `metrics.hitman.io`, `dev-auth.hitman.io` (dev-host check in `sub_c4313a`), `account.ioi.dk`.

RPC services and methods present as strings (same names as Peacock's PC routes):

* `ProfileService`: `GetProfile`, `UpdateExtensions`, `SynchronizeOfflineUnlockables`, `GetFriendsCount`, `GetUserConfig`, `ResolveProfiles`, `ResolveGamerTags`
* `EventsService`: `SaveAndSynchronizeEvents4`, `SaveEvents2`
* `AuthenticationService`: `GetBlobOfflineCacheDatabaseDiff`
* `ChallengesService`: `GetActiveChallengesAndProgression`, `GetChallengeTreeFor`, `GetProgression`, `GetPlayerProfileXpData`
* `ContractSessionsService`, `ContractsService` (`GetForPlay2`, `GetContractOpportunities`), `HubPagesService`, `ReportingService` (`ReportContract`)
* mastery: `/GetMasteryCompletionDataForLocation`, `/GetMasteryCompletionDataForUnlockable`, `/ChallengeLocation`

---

## 4. Payloads (keys verified in the parsers/builders)

### 4.1 Config response

```ts
interface ConfigResponse {
  Versions: Array<{
    Name: string                 // compared with strcmp
    GAME_VER: string             // "8.x.y", major must be 8
    SERVER_VER: {                // looked up as flattened "SERVER_VER.A.B"
      Metrics:              { MetricsServerHost: string; MetricsServerServicePath: string }
      Authentication:       { AuthenticationHost: string; UserChannelServicePath: string; AuthenticationServicePathConfiguration: string }
      GlobalAuthentication: { AuthenticationHost: string; OAuthPath: string; RequestedAudience: string }
      Configuration:        { Url: string; AgreementUrl: string }
      Resources:            { ResourcesServicePath: string }
    }
    // tunables read by sub_c47d4a as (key, value) pairs, keep WebSockets off.
    // Where they sit in the response is inferred, not traced.
    // "Online.WebSocketsEnabled": false, "Online.DefaultHttpTimeout", "Online.DefaultWebSocketTimeout",
    // "Online.WebSocketsRoute", "Online.WebSocketsPingInterval", "Online.DefaultAdaptiveHttpTimeout.{enabled,speed_kbs,timeout_s}"
  }>
}
```

There is no `ISSUER_ID` literal in this build; the issuer goes to the server as `?issuer=`. `RequestedAudience` falls back to `"{0}_{1}"` built from the config name when absent.

### 4.2 OAuth request (`application/x-www-form-urlencoded`)

```
grant_type=external_psn
&psn_userid={0}&pId={1}&authcode={2}&psn_issuerid={3}&psn_onlineid={4}&fsp={5}&product_id={6}&psn_cid={7}
&device_type={0}&device_id={1}
&{audience part}
&locale={1}&rgn={2}
&gs={1}
```

`authcode` is from `sceNpAuthGetAuthorizationCodeV3`, `psn_userid` from `sceNpGetAccountIdA`, `psn_onlineid` from `sceNpGetOnlineId`. My guesses for the rest, not verified: `psn_issuerid` = NP environment issuer, `fsp` = free starter pack flag, `product_id` = content id, `psn_cid` = client id. `rgn` comes from the account country (`"asia"` is one of the literals). A local server can ignore all of it and key the user on `psn_userid`.

### 4.3 OAuth response (`sub_c508c6`)

```ts
interface OAuthResponse {
  access_token: string     // treated as opaque, no JWT claim names exist in the binary
  token_type: "bearer"     // strcmp with lowercase "bearer"
  expires_in: number       // turned into an absolute time with sceRtc
  refresh_token: string
  in_queue_message?: unknown   // login queue
  in_queue_data?: unknown
  availability_msg?: unknown
  availability_data?: unknown
}
```

### 4.4 Game auth Init response (`sub_c74414`)

```ts
interface InitResponse {
  profileid: string                 // also stored as OnlineProfileID
  ias: number
  blobconfig: { bloburl: string; blobsig: string; blobsigduration: number }
  serverversion: string             // stored as OnlineServerVersion, default "unknown"
  servertimeutc: string
  // SignupRequired branch exists (ProcessGameAuthenticationResponse::SignupRequired)
}
```

### 4.5 Every RPC

Headers set by `sub_c43302`:

```
Accept: application/json
Content-Type: application/json
Authorization: bearer <access_token>
Version: <looked up with key "Version" from the live server config object (sub_c43e2a), inferred>
```

### 4.6 GetProfile request / response

Request (`sub_c52c76`, Ghidra-verified):

```json
{ "id": "<profile id from Init.profileid, global 0x3c6bc58>",
  "extensions": ["inventory", "progression", "transactions", "contracts", "gameclient",
                 "gamepersistentdata", "opportunityprogression", "achievements"] }
```

The client itself only reads `Extensions.gamepersistentdata.prologue["proceed-intro"]` and `VideoShown` during the profile step; the rest of `Extensions` is handed to subsystems as a blob. Identity is read in `sub_c4b834`:

```ts
interface UserProfile {
  Id: string
  LinkedAccounts: {
    ps4?: string           // PSN account id, PS5 uses the "ps4" key
    ps4onlineid?: string   // PSN online id
    dev?: string
    nintendo?: string
  }
  Gamertag?: string        // missing -> "UI_USER_ANONYMOUS"
  Extensions: {
    inventory?: unknown
    progression?: unknown
    transactions?: unknown
    contracts?: unknown
    gameclient?: unknown
    gamepersistentdata?: { prologue?: { "proceed-intro"?: boolean }; [k: string]: unknown }
    opportunityprogression?: Record<string, boolean>
    achievements?: unknown
    entitlements?: unknown
    [k: string]: unknown   // Peacock* keys are harmless
  }
  ETag?: string | null
  Version?: number
}
```

Peacock's PC profile fields `SteamId`, `EpicId`, `DevId`, `PSNAccountId`, `PSNOnlineId` etc. do not appear as strings in this build, so the client never reads them. Keep them only if Peacock code needs them.

### 4.7 SaveAndSynchronizeEvents4

Request (`sub_c6e5ee`):

```ts
interface SaveAndSyncEvents4Request {
  userId: string
  values: OnlineEvent[]
  lastEventTicks: number
  clienttype: string        // new compared with Peacock's PC handler
  lastPushDt: number
}
// SOnlineEvent is a reflected type. ContractSessionId, ContractId, Timestamp, Origin, UserId,
// Version and XPGain exist as strings; the exact field set is taken from Peacock's PC shape.
interface OnlineEvent {
  Name: string
  Value: unknown
  ContractSessionId: string
  ContractId: string
  Timestamp: number
  Origin?: string
  UserId: string
  Version: unknown
  XPGain?: number
  Id: string
}
```

Response keys present: `SavedTokens`, `NewEvents`, `NextPoll`, `PushMessages`.

### 4.8 PlayerConnected telemetry event (built right after auth)

`Name: "PlayerConnected"`, `Properties: { ProfileId, ServerConfig: { ServerVersion }, Entitlements, AvailableLocalEntitlements, SystemConfiguration: { Platform, SubPlatform, IsHDR, Resolution, Is4k, HasHDRCapability, Locale, TextLocale, Region, IsDisc, IsWoaDisc }, IsGameOfTheYearUser, IsFullExperienceUser, IsIntroPackUser, IsFreePrologueUser, IsFreePrologueConvertedToPaidUser, NumFriendsOwnHitman, IsPlatformPremiumSubscriptionUser, Issuer }`. Safe to accept and drop.

---

## 5. Transport

### 5.1 HTTP client

`ZHttpManagerOrbisCurl` / `ZHttpManagerOrbisCurlPoller`, libcurl multi interface (`curl_multi_add_handle` at `0xf2aed9`, `curl_multi_perform` at `0xf27f81`). curl is exported by the eboot itself (`curl_easy_setopt` at `0x1bf1d46`).

Options set by game code (`0xf2a4a5`..`0xf2aec0`): `HTTP_VERSION`, `URL`, `WRITEFUNCTION/WRITEDATA`, `READFUNCTION/READDATA`, `HEADERFUNCTION/HEADERDATA`, `NOSIGNAL`, `VERBOSE`, `TCP_KEEPALIVE/KEEPIDLE/KEEPINTVL`, `USERAGENT`, `LOW_SPEED_LIMIT/TIME`, `TIMEOUT_MS`, `PRIVATE`, `PROXY`, `PROXYPORT`, `PROXYTYPE`, `PROXY_SSL_VERIFYPEER`, `POST`, `NOBODY`, `ACCEPT_ENCODING`, `HTTPHEADER`.

**Not set:** `SSL_VERIFYPEER`, `SSL_VERIFYHOST`, `CAINFO`, `SSL_CTX_FUNCTION`, `PINNEDPUBLICKEY`. So curl's default applies (verify on) and there is no public key pinning in game code.

### 5.2 TLS trust

Engine start-up `sub_1481a8` (`0x14aa05`) calls `sceSslGetCaList` + `sceSslGetCaCerts`, walks the certs with `sceSslGetIssuerName` / `sceSslGetNameEntryCount` / `sceSslGetNameEntryInfo` into its own store, and sets two hooks into the curl/TLS region (`0x263c250 = sub_1ba5be0`, `0x263c258 = sub_1ba5c2e`). A self-signed local cert will fail unless one of the options in 6 is used.

### 5.3 Sockets

| where | API | purpose |
|---|---|---|
| curl, `0x1bd4bdd` / `0x1bd4ebd` | libc `socket` / `connect` (+ `sceNetResolverStartNtoa` at `0x1bd772b`) | all IOI HTTPS traffic |
| `sub_f2266c` | `sceNetSocket("hmws", AF_INET, SOCK_STREAM, TCP)`, `SO 0x1109` = 30 s, `sceNetConnect`, `sceNetSend`, `sceNetRecv`, `"HTTP/1.1 200"` check | WebSocket client (`ZRuntimeOnlineModule::WebsocketsThreadProc`), handshake template `GET {0}{1} HTTP/1.1 / Host: {2}:{3} / Upgrade: websocket / Sec-WebSocket-Key: {4} / {5} / Sec-WebSocket-Version: 13`; frames in `sub_f217c4` (control), `sub_f21a28` (write), `sub_f21b74` (read), mask key from `sceRandomGetRandomNumber` (`sub_f2c858`), TLS via mbedTLS (`ControlFrameFailedWebSocketMbedTls`) |
| `sub_f416b6` | `sceNetSocket("udpsocket")` | UDP, not part of the profile flow |
| `0x17c575c` / `0x17c5798` | `"StreamSocket"` / `"Datagram"` + `sceNetBind` / `sceNetListen` / `sceNetAccept` | listening socket, looks like the Wwise/dev tool link |
| `0x1056682` | libc `socket` / `sendto` | debug output |

### 5.4 Tunables from the server config (`sub_c47d4a`)

`Online.DefaultHttpTimeout`, `Online.WebSocketsRoute`, `Online.WebSocketsPingInterval`, `Online.WebSocketsEnabled` (stored at `this+0x6e4`), `Online.DefaultWebSocketTimeout`, `Online.DefaultAdaptiveHttpTimeout.speed_kbs`, `.timeout_s`, `.enabled`. A `WebSocketFallback` path exists, so leaving WebSockets disabled keeps everything on plain HTTP polling.

---

## 6. Getting the game onto a local server

1. **Host.** Either DNS (nanodns is already on the console, `/data/nanodns`) for `config.hitman.io` and `auth.hitman.io`, or set `online_VersionConfigDomain` from `thumbs.dat`:
   `ConsoleCmd online_VersionConfigDomain 192.168.0.62:8000` (buffer is 256 bytes, `host:port` is fine for curl). Whether ConsoleCmd is allowed to set this variable in a retail build is **not verified**; DNS works regardless.
2. **TLS.** Pick one:
   * Patch the scheme, like Peacock's PC `protocol` patch: file `0x1c8cdc5` `https://{0}` -> `http://{0}\0`, and file `0x1c598a0` `https://auth.hitman.io` -> `http://auth.hitman.io\0`. Then serve `http://` hosts in the config. Both strings are unique in the file.
   * Or keep HTTPS and make the cert trusted: hook `sceSslGetCaCerts` (or the store built in `sub_1481a8`) to add your own root, from a payload.
   * Or flip curl's default `verifypeer` in its init (needs locating `Curl_init_userdefined` in `0x1bd0000..0x1c00000`).
   The dump is decrypted, so a file patch is possible if your loader runs plain ELFs; otherwise do the same edits in memory from a payload.
3. **Config you serve.** One `Versions[]` entry with `GAME_VER` `8.12.0`, `SERVER_VER` hosts pointing at the local server, `Online.WebSocketsEnabled: false`, and a `Version` value for the RPC header.
4. **Peacock changes.** Accept `grant_type=external_psn` (key user on `psn_userid`, store `psn_onlineid` as `Gamertag` and `LinkedAccounts.ps4onlineid`, account id in `LinkedAccounts.ps4`); add the `ps4-prod` / `8.12.0` version mapping; accept `clienttype` in `SaveAndSynchronizeEvents4`.

Other strings for reference: `thumbs.dat` (decrypted in `data/`) already has `SkipSteamAuthentication=true`, `OnlineDisableAutoReconnect=1`, `ps5RegionId=PPSA16387`. `OnlineResources_Disable` / `OnlineResources_ForceOfflineOnFailure` control the dynamic-resources step.

---

## 7. Local database schema

Shaped by what the client sends and reads above. Extensions stay as JSON blobs because the client treats them that way and `UpdateExtensions` writes them per key.

```sql
CREATE TABLE users (
  id               TEXT PRIMARY KEY,        -- profile Id / Init.profileid (uuid)
  psn_account_id   TEXT UNIQUE NOT NULL,    -- external_psn psn_userid, LinkedAccounts.ps4
  psn_online_id    TEXT,                    -- psn_onlineid, LinkedAccounts.ps4onlineid, Gamertag
  issuer_id        TEXT,                    -- psn_issuerid / ?issuer=
  region           TEXT,                    -- rgn
  locale           TEXT,
  created_at       INTEGER NOT NULL,
  updated_at       INTEGER NOT NULL
);

CREATE TABLE auth_tokens (
  access_token     TEXT PRIMARY KEY,        -- opaque to the client
  refresh_token    TEXT UNIQUE NOT NULL,
  user_id          TEXT NOT NULL REFERENCES users(id),
  expires_at       INTEGER NOT NULL         -- now + expires_in
);

CREATE TABLE profile_extensions (
  user_id          TEXT NOT NULL REFERENCES users(id),
  name             TEXT NOT NULL,           -- inventory, progression, transactions, contracts, gameclient,
                                            -- gamepersistentdata, opportunityprogression, achievements, entitlements, Peacock*
  data             TEXT NOT NULL,           -- JSON
  updated_at       INTEGER NOT NULL,
  PRIMARY KEY (user_id, name)
);

CREATE TABLE profile_meta (
  user_id          TEXT PRIMARY KEY REFERENCES users(id),
  etag             TEXT,
  version          INTEGER NOT NULL DEFAULT 1
);

CREATE TABLE events (
  user_id              TEXT NOT NULL REFERENCES users(id),
  ticks                INTEGER NOT NULL,    -- server ticks, answers lastEventTicks
  id                   TEXT,
  name                 TEXT NOT NULL,
  contract_session_id  TEXT,
  contract_id          TEXT,
  timestamp            REAL,
  origin               TEXT,
  value                TEXT,                -- JSON
  xp_gain              INTEGER,
  PRIMARY KEY (user_id, ticks)
);
CREATE INDEX events_session ON events(contract_session_id);

CREATE TABLE contract_sessions (
  id               TEXT PRIMARY KEY,        -- ContractSessionId
  user_id          TEXT NOT NULL REFERENCES users(id),
  contract_id      TEXT NOT NULL,
  difficulty_level INTEGER,
  started_at       INTEGER,
  state            TEXT                     -- JSON scoring state
);
```

---

## 8. Still open

* The writer of `ZOnlineManager+0x18/+0x1c` (the host gate in section 2).
* Meaning of `pId`, `fsp`, `product_id`, `psn_cid`, `gs`, `ias`.
* Whether `ConsoleCmd online_VersionConfigDomain` works in retail.
* Who assigns the per-RPC handler slots used in mode 2.

## 9. Reproduce

* `tools/xtea_dec.py thumbs.dat` decrypts Glacier `thumbs.dat` / `packagedefinition.txt` (CRC32 checked).
* `tools/ripscan.py eboot.bin "<string>" ...` lists code sites that `lea` each string.
* `tools/callscan.py eboot.bin <target_va>` lists direct calls to a target, plus the `esi` constant before each call (used for curl option ids).
* `tools/dis.sh <start> <stop>` annotated llvm-objdump of a range (needs `eboot.sym.elf`).
* `ps5vr/tools/ps5_dynlib.py --names <shadps4 src> --elf-dir out eboot.bin` resolves 836/849 imports and writes `eboot.sym.elf` (import list in `data/eboot_imports_exports.txt`).
* `tools/DumpNet.java` + `tools/targets1.txt` with Ghidra headless decompiles every function that references the strings / calls the APIs in the target list. Output is in `decomp/` (148 functions, `decomp/index.txt` says why each was picked).
* `tools/XrefDump.java <outDir> <addr...>` lists read/write refs to addresses and decompiles the writers (`decomp/xrefs_*.txt`).
* `ghidra_proj/hitman.gpr` is the analysed Ghidra 12.1.3 project. **Ghidra addresses are these VAs + 0x100000** (it rebased the PIE), so `sub_c52c76` here is `00d52c76` in Ghidra and in the `decomp/` file names. Re-run scripts without re-analysis: `analyzeHeadless ghidra_proj hitman -process eboot.sym.elf -noanalysis -readOnly -scriptPath tools -postScript XrefDump.java out 0x03d6b7d8`.
