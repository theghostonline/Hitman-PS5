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
- HITMAN 3 `PPSA16387`, version 01.017.000
- A computer on the same network to run the backend

## Install

1. **Start the backend** on your computer:
   ```sh
   scripts/backend.sh build
   scripts/backend.sh ps5pkg    # builds the PS5 resource package from your console's game files
   scripts/backend.sh start     # listens on port 8410
   ```
2. **Point the payload at it.** Create `/data/hitman-server/config.ini` on the PS5:
   ```ini
   upstream = http://<your-computer-ip>:8410
   ```
3. **Load the payload.** Grab `hitman-server-0.3.2.elf` from the
   [latest release](https://github.com/theghostonline/Hitman-PS5/releases/latest)
   and load it with Payload Manager, or send it to an elfldr on port 9021.
4. **Optional:** add `local/hitman-server` to Payload Manager's autoload list so
   it starts on every boot. The backend needs to be running whenever you play.
5. **Start HITMAN 3.** You'll get a notification once the game is patched,
   then it connects online by itself.

## Troubleshooting

- Not connecting? Check the backend is up (`scripts/backend.sh status`) and
  that the IP in `config.ini` is right.
- `http://<ps5-ip>:8000/healthz` shows the payload version and the game's
  online state.
- Logs: `/data/hitman-server/server.log` on the PS5, `scripts/backend.sh logs`
  on the computer.

## Building

```sh
make -C server          # ps5-payload-sdk v0.43+ with the jansson and sqlite3 ports
make -C server test
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

## Disclaimer

A preservation project for people who own the game. Not affiliated with IOI or
Sony. No game files are included.
