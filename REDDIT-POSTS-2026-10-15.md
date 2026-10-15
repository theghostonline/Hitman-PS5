# Reddit posts, ready to paste

Written to match your own comments: contractions, short, warm, no em dashes.
Nothing here names a release or links a download, because the repo is not
public yet. Where something is not done, the post says so.

Two versions below: one for r/PS5_Jailbreak (the homebrew crowd, more
technical), one for r/hitman (players, keep it plain). Pick a title per post.

---

## r/PS5_Jailbreak

**Title options (pick one):**
- HITMAN World of Assassination now connects online on a jailbroken PS5, running against a local server
- Got HITMAN WoA online on PS5 with no official servers, full menus and progression working

**Body:**

> Been chipping away at this for a while and it finally works: HITMAN World of Assassination (PPSA16387) reaching the online hub on a jailbroken PS5, with no connection to IOI at all.
>
> How it fits together:
>
> - A small payload runs on the console. It patches the running game in memory so it talks to 127.0.0.1 instead of IOI, keeps the PSN checks happy offline, and forwards every request the game makes.
> - The forwarding goes to a full copy of the Peacock Project server (the PC HITMAN server replacement) running on my Mac on the same network. The PS5 is an H3 client under the hood, so most of Peacock already fits, it just needed a PS5 adapter for the login, the config and the bits of the protocol that differ.
> - The game needed two things no PC build has. Its dynamic resource package has to carry a "ps5_repo" that only ships inside Sony's own PS5 package, and its menu text and a couple of menu files are keyed differently from PC. I build both from the console's own game files plus Peacock's resources, so nothing copyrighted gets shared.
>
> What works right now, tested on my console: it connects to the hub, loads Story, Destinations, Campaigns, the Store, Career and Freelancer, you can plan and start a mission, finish it, and the score, XP and mastery come through and stick across a restart.
>
> Mapping the PS5's own protocol out of the binary took the most time. The PS5 build asks for its config on a bare URL, calls itself version 8.12.0, logs in with a PSN grant, and uses a few service methods the PC client doesn't. There's no UnlockableService on PS5 for example, the inventory comes back on a different call.
>
> It's a Mac-hosted backend for now, the console is the client. A fully standalone console-only version is a separate job I haven't proven yet. Not released, this is a "it works on my bench" post, not a download. Happy to answer anything technical 🙂

---

## r/hitman

**Title options (pick one):**
- Got HITMAN World of Assassination working online again on PS5 without the official servers
- HITMAN WoA online, menus and progression and all, running on a local server on PS5

**Body:**

> Quick one for anyone who likes the preservation side of things. I've got HITMAN World of Assassination connecting online on PS5 again, running against a local server instead of IOI's, so the menus and your progression work without their servers in the loop.
>
> Right now it connects to the main hub and loads Story, Destinations, the Store, Career and Freelancer. You can plan a mission, play it, and your score, XP and mastery save and carry over when you restart. It's built on the Peacock Project (the server replacement the PC community already uses), with the extra work to make the PS5 version of the game talk to it.
>
> This is on a jailbroken console, and it's not a download yet, just showing it's possible and working on my own setup. Not trying to step on anything official, it's a preservation project for people who own the game and want it to keep working. More when there's something to share 🙂

---

## Notes for you

- Nothing here claims a release or links a repo, since the project isn't
  public. If you do make it public, add the link to the top of each.
- The r/hitman one deliberately avoids the jailbreak-heavy wording and
  anything that reads as "pirate the DLC", to keep it welcome there.
- DLC: the Deadly Sins, Deluxe, Trilogy and a few outfit packs show as not
  owned in the client because the copies on this console are a different
  region/title from the game, so the game doesn't recognise them. Server side
  all content is unlocked, so it doesn't block play. I left that out of the
  posts on purpose, it's a detail and it's not finished.
