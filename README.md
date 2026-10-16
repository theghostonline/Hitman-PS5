# HITMAN PS5 Online

[![Buy Me a Coffee](https://img.shields.io/badge/Buy_Me_a_Coffee-Support_this_project-FFDD00?style=for-the-badge&logo=buymeacoffee&logoColor=black)](https://buymeacoffee.com/theghostonline)

HITMAN World of Assassination (`PPSA16387`) online again on a jailbroken PS5,
with no IOI servers involved. A small payload on the console points the game
at a backend server on your own network, so the online hub, menus, missions,
scoring and progression all work.

## What works

Tested on a PS5 on firmware 13.60:

- Connects to the online hub
- Story, Destinations, Campaigns, Store, Career and Freelancer load
- Plan, play and finish missions
- Score, XP and mastery save and stick across restarts

## How it works

```
HITMAN 3  ->  hitman-server payload (on the PS5)  ->  backend server (on your PC/Mac)
```

The payload patches the running game in memory so it talks to `127.0.0.1`
instead of IOI, keeps the PSN checks happy offline, and forwards every request
to the backend. Nothing on disk is changed.

## Requirements

- A jailbroken PS5 that loads payloads (tested with Payload Manager 0.6.1 on 13.60)
- HITMAN 3 `PPSA16387`, version 01.017.000 or 01.041.000, including the 01.041 low firmware backport (USA PPSA16388 not tested)
- A computer on the same network to run the backend

## Install

Everything is on the [latest release](https://github.com/theghostonline/Hitman-PS5/releases/latest).
You need two things from it: `hitman-server-0.3.6.elf` for the PS5, and the
backend zip for your computer (Windows, macOS Apple Silicon, macOS Intel or Linux).
Node is bundled, so there's nothing else to install.

1. **Start the backend.** Unzip it, then:
   - Windows: double-click `Start Server.cmd`
   - macOS / Linux: open a terminal in the folder and run `sh start.sh`

   On the first run it asks for your PS5's IP. Have the PS5 on with an FTP
   server running (etaHEN or ftpsrv) and ideally HITMAN 3 open. It finds the
   game and builds the PS5 resource package from your own game files, which
   takes about 20 seconds and only happens once.
2. **Point the payload at it.** The backend window shows a line like
   `upstream = http://192.168.1.100:8410`. Put that line in the `config.ini`
   from the zip and copy it to the PS5 as `/data/hitman-server/config.ini`.
3. **Load the payload.** Load `hitman-server-0.3.6.elf` with Payload Manager,
   or send it to an elfldr on port 9021.
4. **Optional:** add `local/hitman-server` to Payload Manager's autoload list so
   it starts on every boot.
5. **Start HITMAN 3.** You'll get a notification once the game is patched,
   then it connects online by itself.

Keep the backend window open whenever you play. Your progress is saved in the
backend folder (`userdata/`), so keep that folder when you update.

## Troubleshooting

- Not connecting? Check the backend window is still open and that the IP in
  `config.ini` matches the one it shows.
- `http://<ps5-ip>:8000/healthz` shows the payload version, whether it can
  reach the backend, and the game's online state.
- First run can't find the game? Open HITMAN 3 on the PS5 and start the
  backend again. If you moved the game files somewhere unusual, run
  `node/bin/node build-ps5-package.mjs <ps5-ip> --chunk0 ftp://<ps5-ip>:2121/<path>/runtime/chunk0.rpkg`
  (Windows: `node\node.exe`).
- Payload log: `/data/hitman-server/server.log` on the PS5.

## Building

```sh
make -C server                        # payload: ps5-payload-sdk v0.43+ with jansson and sqlite3
make -C server test
scripts/package-release.sh <version>  # backend zips, from the backend source on the release page
```

Technical notes on the game's online protocol are in
[docs/network-map.md](docs/network-map.md).

## Support

This was built and tested on a real console, one protocol quirk at a time. If
it brought your HITMAN back online, a coffee helps cover the cost of building
it and keeps the updates coming.

<a href="https://buymeacoffee.com/theghostonline"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Buy Me a Coffee" height="50"></a>

Not able to donate? A star on the repo and a bug report with your firmware and
game version help just as much.

## Credits

The backend that runs on your computer is a modified build of
[The Peacock Project](https://github.com/thepeacockproject/Peacock). That's
where the game modes, contracts, challenges and progression come from. This
project isn't affiliated with Peacock.

Everything that gets it working on PS5 was built here: the payload (patching
the game, the PSN checks, forwarding to the backend), the PS5 changes to the
backend, the PS5 resource package builder, the launcher, the in-game updater,
and the reverse engineering of the PS5 version of the game. Every change made
to the backend is listed in `PS5-CHANGES.md` in the backend zip.

## License

AGPL-3.0, see [LICENSE](LICENSE). Every release has a
`hitman-ps5-backend-src-<version>.zip` with the full source of the backend.

## Disclaimer

A preservation project for people who own the game. Not affiliated with IOI or
Sony. No game files are included.
