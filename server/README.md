# hitman-server

A local online service for **HITMAN 3 on PS5** (`PPSA16387`, 01.017.000, 01.041.000 or the 01.041 backport) that runs on the console itself as a homebrew payload. No PC is needed: the payload serves the game's online API on the console, keeps your profile in a SQLite database under `/data`, and points the running game at itself in memory.

This is clean protocol emulation for personal game preservation. It's written from a network map of the game's own client behaviour, it contains no IOI code or content, and it's meant for people who own the game and want it to keep working without IOI's servers. The PC community already has a server that does the same job; this is a separate, independent implementation for the PS5 client.

## Status

Checked on a PS5 on firmware 13.60 on 2026-10-15:

| what | result |
|---|---|
| payload starts from Payload Manager, opens `/data/hitman-server/hitman.db` (SQLite 3.46.1) | works |
| full protocol run over the LAN (`test/flow_test.py --remote`, 44 checks) | passes |
| loading a new build over a running one (handoff, no process kill) | works |
| in-memory patch of the game (4 sites, verified before and after each write) | works, from process start |
| game reaching the online flow by itself | **not yet**: it sits in state 1 (OFFLINE) at the title screen |

The last row is the open item. With the patch in place the game should come to `127.0.0.1:8000` as soon as its connection flow starts. What I haven't seen yet is the game start that flow: it either waits for a button press on the title screen or it stops at the PSN check (state 5, `PREAUTHENTICATE`, `sceNpCheckNpReachability`) before it ever fetches the config. `/healthz` and `server.log` show the live state, so this is quick to check from the couch: press X on the title screen and watch the state.

## How it works

```
 HITMAN 3 (eboot.bin)                      hitman-server.elf (payload)
 ---------------------                     -----------------------------------------
 config  GET  http://127.0.0.1:8000/config/ps4-prod/8.12.0      --> Versions[] (8.12.0, WebSockets off)
 OAuth   POST /oauth/token  grant_type=external_psn             --> user keyed on psn_userid, opaque tokens
 auth    GET  /authentication/api/configuration/Init            --> profileid, ias, blobconfig, server time
 RPC     POST /authentication/api/userchannel/<Service>/<Method> --> profile, extensions, events in SQLite
```

The game is hard-wired to `https://config.hitman.io`. Once it's running, the payload's patcher changes four things in its memory (eboot base `0x400000`):

| eboot VA (01.017.000) | eboot VA (01.041.000) | eboot VA (01.041 backport) | was | becomes |
|---|---|---|---|---|
| `0x1c88dc5` | `0x20f6945` | `0x21670dc` | `https://{0}` (config URL scheme) | `http://{0}` |
| `0x1c558a0` | `0x20bad89` | `0x212ae9d` | `https://auth.hitman.io` (auth fallback host) | `http://127.0.0.1:8000` |
| `0x1c240d4` | `0x2105374` | `0x2175d38` | `config.hitman.io` (default config domain) | `127.0.0.1:8000` |
| `0x3c6b510` | `0x3bbe9d8` | `0x3c46e58` | `online_VersionConfigDomain`, filled at static init | `127.0.0.1:8000` |

The build is picked by finding those three strings at one build's addresses; any other build is left alone.

Every site is compared with the expected bytes first. On any other game build nothing is written and you get a notification. Nothing on disk is changed, so the game dump stays as it was.

## Install

You need a jailbroken PS5 that loads payloads (tested with Payload Manager 0.6.1 on 13.60) and HITMAN 3 `PPSA16387` 01.017.000, 01.041.000 or the 01.041 backport.

1. Build it, or take `hitman-server.elf` from a release:
   ```sh
   make            # ps5-payload-sdk v0.43+ with the jansson and sqlite3 ports
   ```
2. Load it. With Payload Manager:
   ```sh
   make deploy PS5_HOST=192.168.0.37
   ```
   This uploads it to `/data/pldmgr/payloads/local/hitman-server/` and starts it. Or send it to an elfldr on port 9021 like any other payload.
3. To have it start on every boot, add `local/hitman-server` to Payload Manager's autoload list in the PM app.
4. Start HITMAN 3. You'll get a "HITMAN 3 online -> local server" notification once the game is patched.

Loading a new build while an old one is running is fine: the new one asks the old one to exit (a loopback-only `/admin/shutdown`) and then takes the port.

## Running it on a LAN address

The payload listens on `0.0.0.0:8000`, so the same server is reachable from your network at `http://<console-ip>:8000`. The game itself talks to `127.0.0.1:8000`. Handy checks from another machine:

```sh
curl http://192.168.0.37:8000/healthz          # version and patcher state, including the game's online state
python3 -I test/flow_test.py --remote 192.168.0.37:8000
```

Note that `--remote` creates a test profile (`psn_userid` 1234567890123456789) in the console's database.

The server also builds for macOS and Linux (`make host`) if you'd rather run it on a machine on your network. In that case set `patch = false`, point the game at that machine yourself (DNS for `config.hitman.io` and `auth.hitman.io`, plus the `http://` scheme patch) and set `public_url` to the address the console should use.

## Configuration

`/data/hitman-server/config.ini` is optional. Every key has a default; see [`config.ini.example`](config.ini.example).

| key | default | |
|---|---|---|
| `port`, `bind` | `8000`, `0.0.0.0` | listen address |
| `public_url` | from the request `Host` | base URL put in the config response |
| `data_dir` | `/data/hitman-server` | database, `server.log`, `resources/` |
| `game_ver`, `rpc_version` | `8.12.0` | `GAME_VER` and `Version` in the config; the client only keeps an `8.x.y` entry |
| `token_ttl`, `refresh_ttl` | `3600`, `2592000` | OAuth `expires_in`, and how long a refresh token outlives its access token |
| `ias` | `1` | must be non-zero, 0 sends the client to its signup screen |
| `next_poll` | `10` | `NextPoll` seconds in event responses |
| `seed_extensions` | none | JSON file of `{ extensionName: value }` that new profiles start with |
| `patch`, `title_id`, `patch_domain` | `true`, `PPSA16387`, `127.0.0.1:<port>` | the in-memory patch (domain at most 15 characters) |
| `log_level` | `info` | `debug` also logs OAuth fields (auth code redacted), events and metrics |

## Endpoints

| route | |
|---|---|
| `GET /config/{name}/{version}?issuer=` | one `Versions[]` entry: `Name` echoes `{name}`, `GAME_VER`, `SERVER_VER` hosts pointing back here, `"Online.WebSocketsEnabled": false`, `Version` |
| `POST /oauth/token` | `grant_type=external_psn` (key `psn_userid`, `psn_onlineid` becomes `Gamertag` and `LinkedAccounts.ps4onlineid`) and `grant_type=refresh_token` |
| `GET /authentication/api/configuration/Init` | `profileid`, `ias`, `blobconfig`, `serverversion`, `servertimeutc` |
| `POST .../userchannel/ProfileService/GetProfile` | `{id, extensions[]}` in, `UserProfile` out with `LinkedAccounts.ps4` and `ps4onlineid` |
| `POST .../userchannel/ProfileService/UpdateExtensions` | `{id, extensionsData{}}`, each key stored as a JSON blob, `Version` and `ETag` move |
| `POST .../userchannel/EventsService/SaveAndSynchronizeEvents4` | stores `values[]`, returns `SavedTokens`, `NewEvents`, `NextPoll`, `PushMessages` |
| `POST .../userchannel/ProfileService/SynchronizeOfflineUnlockables` | 204 |
| `POST .../userchannel/AuthenticationService/GetBlobOfflineCacheDatabaseDiff` | `[]` |
| `POST /api/metrics/add` | telemetry, dropped |
| `GET /resources/...` | files from `data_dir/resources/` (dynamic resources aren't bundled) |
| `GET /healthz` | version and patcher state |

Anything else is answered with 404 and logged with a body preview, so `server.log` shows the next call the game needs.

Paths are served in the same form as the client's own built-in fallbacks (`/oauth/token`, `authentication/api/userchannel`, `authentication/api/configuration`, `api/metrics/add`), so a client that ignores the served hosts still lands on the right routes.

## Database

`db/schema.sql` is the schema, table for table: `users`, `auth_tokens`, `profile_extensions`, `profile_meta`, `events`, `contract_sessions`, with their primary keys, `user_id` foreign keys and the `events_session` index. It's compiled into the payload and applied on every start. Extensions are kept as JSON text per key, the way the client treats them.

`clienttype` from `SaveAndSynchronizeEvents4` is parsed and logged with each batch. The schema has no column for it, so it isn't stored.

## Protocol notes

Checked in the 01.017.000 client while building this:

* `ias` is read as an int (`sub_5fa7e0`) and 0 takes the `SignupRequired` branch of `sub_c74414`.
* GetProfile sends `{"id": <profileId>, "extensions": [...]}` (the `id` literal is at `0xc531c8`).
* UpdateExtensions sends `{"id": <profileId>, "extensionsData": {...}}` (`0xc758ef`, `0xc75912`).
* `Online.WebSocketsEnabled` is read as a bool and the other `Online.*` tunables as numbers (`sub_c47d4a`). Where they sit in the response is still inferred. If the game ever tries a WebSocket, the server refuses it and logs a warning that says so.
* `sceKernelGetAppInfo` puts the title id at byte 16 on 13.60.

## Not done

* Menus and game content (`/profiles/page/...`, challenges, contracts, mastery). These are most of what the PC community server serves and need game data this repo doesn't ship.
* Dynamic resources: the game asks for `dynamic_resources_*.rpkg` under `blobconfig.bloburl`; drop your own copies into `data_dir/resources/blob/`.
* PSN: the client gets a PSN auth code before it calls `/oauth/token`. If the console can't reach PSN, the flow may stop there, and that would need a hook in the game, not a server change.

## Testing

```sh
make test       # host build + 56 end-to-end checks: flow, chunked uploads, tokens, schema, restart, handoff
```

The test replays the console's connection sequence, including OAuth sent the way the game's curl sends it (chunked, `Expect: 100-continue`).

## Layout

```
db/schema.sql          database schema
src/main.c             startup, config, handoff from an older instance
src/http.c             HTTP/1.1 server (keep-alive, chunked bodies, 100-continue)
src/handlers.c         routes and payloads
src/db.c               SQLite storage
src/patcher.c          finds the game, patches it in memory, follows its online state
src/config.c, log.c, util.c
test/flow_test.py      end-to-end test
```

## License

AGPL-3.0-or-later. See `LICENSE`.
