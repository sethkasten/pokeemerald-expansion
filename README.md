# About `pokeemerald-expansion`

![Gif that shows debugging functionality that is unique to pokeemerald-expansion such as rerolling Trainer ID, Cheat Start, PC from Debug Menu, Debug PC Fill, Pokémon Sprite Visualizer, Debug Warp to Map, and Battle Debug Menu](https://github.com/user-attachments/assets/cf9dfbee-4c6b-4bca-8e0a-07f116ef891c) ![Gif that shows overworld functionality that is unique to pokeemerald-expansion such as indoor running, BW2 style map popups, overworld followers, DNA Splicers, Gen 1 style fishing, OW Item descriptions, Quick Run from Battle, Use Last Ball, Wild Double Battles, and Catch from EXP](https://github.com/user-attachments/assets/383af243-0904-4d41-bced-721492fbc48e) ![Gif that shows off a number of modern Pokémon battle mechanics happening in the pokeemerald-expansion engine: 2 vs 1 battles, modern Pokémon, items, moves, abilities, fully customizable opponents and partners, Trainer Slides, and generational gimmicks](https://github.com/user-attachments/assets/50c576bc-415e-4d66-a38f-ad712f3316be)

<!-- If you want to re-record or change these gifs, here are some notes that I used: https://files.catbox.moe/05001g.md -->

**`pokeemerald-expansion`** is a GBA ROM hack base that equips developers with a comprehensive toolkit for creating Pokémon ROM hacks. **`pokeemerald-expansion`** is built on top of [pret's `pokeemerald`](https://github.com/pret/pokeemerald) decompilation project. **It is not a playable Pokémon game on its own.**

# Mod Features (Gen 1–3 Obtainability Project)

This fork extends `pokeemerald-expansion` with an ongoing project to make every Gen 1–3 non-legendary Pokémon reachable through normal gameplay. Highlights below; see [`MOD_CHANGELOG.md`](MOD_CHANGELOG.md) for per-file diffs, flag layouts, and script details.

## New Sidequests
- **Devon Corp Porygon Vendor** — buy Porygon for ¥1000 on Devon Corp 1F; delivered straight to the PC.
- **Steven's Fruit Salad Quest** — bring Steven every berry sold by the Lilycove vendor for the **Tri-Pass** (unlocks Sevii Isles 1–3).
- **Lilycove Berry Vendor** — post-Champion NPC selling all 37 berries at custom prices (including Chilan and Enigma).
- **Eon Duo, in-story** — the Latias/Latios TV broadcast now triggers on becoming Champion; catching the roamer prompts Norman to hand over the **Eon Ticket** for Southern Island.
- **Sevii Islands / Celio quest repaired** — the full One Island → Mt. Ember Ruby → Rocket Warehouse Sapphire → machine-linked flow works end to end, unlocking Cerulean Cave as its reward.
- **Kanto Indigo League postgame** — the FRLG Elite Four + Champion are now a standalone rematchable "Two Island Sabbatical" rung with first- and second-clear rewards (Up-Grade, Dubious Disc).

## New Kanto ↔ Hoenn Connections
- **Route 115 → Mt. Moon** (walkable end-to-end).
- **Route 111 & Route 112 → Diglett's Cave** (both entrances).
- **Route 120 & Route 121 → Rock Tunnel** (both entrances).
- **Two Island → Victory Road → Route 23 → Indigo Plateau**.
- **Altering Cave ↔ Cerulean Cave** shortcut. Cerulean Cave's overworld exit is sealed so the broken Kanto overworld stays out of reach.
- **Safari Zone warp pad** connecting the Emerald Northeast area to the FRLG Safari Center, with Rest House and Fuchsia exits redirected back to Hoenn.
- **Independent Hoenn / Kanto Safari Zone step counters** so travel between regions doesn't drain the other's budget.

## Expanded Wild Encounters
- FRLG-imported caves (Mt. Moon, Diglett's Cave, Rock Tunnel, Victory Road, Cerulean Cave, FRLG Safari Zone) now have unified encounter tables so wild battles actually fire in the Emerald build.
- **Version exclusives added** on canonical routes without removing any species: Zangoose (Route 114), Lunatone (Meteor Falls), and merged FR/LG Cerulean Cave water/fish slots.
- **Native R/S encounters restored**: Surskit is back on Routes 102, 111, 114, 117, and 120.
- **New family placements**: Budew & Roselia (Route 117), Meditite/Medicham (Mt. Pyre, Victory Road), Chingling (Mt. Pyre Summit), and Kanto fossils Omanyte/Kabuto/Aerodactyl (Desert Underpass).
- **Altering Cave** now hosts all 9 of its intended species (Zubat, Mareep, Pineco, Houndour, Teddiursa, Aipom, Shuckle, Stantler, Smeargle) without needing the Mystery Gift event.

## Species Data
- **Aipom** now always holds Berry Juice (matching Shuckle), keeping the Gen 2 Berry Juice supply reliable.

## Engine Additions
- **Custom-priced Pokémarts** — new `pokemartpriced` script command lets marts set arbitrary prices without inflating item sell prices globally (used by the Lilycove Berry Vendor).
- **PC-direct Pokémon delivery** — new `ScriptGiveMonToPC` special sends caught mons straight to storage regardless of party state (used by the Devon Porygon vendor).
- **Roamer-catch script flag** — `FLAG_CAUGHT_ROAMING_EON_MON` lets scripts react to the roamer being captured (used to gate Norman's Eon Ticket handoff).

---

# [Upstream Features](FEATURES.md)

**`pokeemerald-expansion`** offers hundreds of features from various [core series Pokémon games](https://bulbapedia.bulbagarden.net/wiki/Core_series), along with popular quality-of-life enhancements designed to streamline development and improve the player experience. A full list of those features can be found in [`FEATURES.md`](FEATURES.md).

# [Credits](CREDITS.md)

 [![](https://img.shields.io/github/all-contributors/rh-hideout/pokeemerald-expansion/upcoming)](CREDITS.md)

If you use **`pokeemerald-expansion`**, please credit **RHH (Rom Hacking Hideout)**. Optionally, include the version number for clarity.

```
Based off RHH's pokeemerald-expansion 1.16.3 https://github.com/rh-hideout/pokeemerald-expansion/
```

Please consider [crediting all contributors](CREDITS.md) involved in the project!

# Choosing `pokeemerald` or **`pokeemerald-expansion`**

- **`pokeemerald-expansion`** supports multiplayer functionality with other games built on **`pokeemerald-expansion`**. It is not compatible with official Pokémon games.
- If compatibility with official games is important, use [`pokeemerald`](https://github.com/pret/pokeemerald). Otherwise, we recommend using **`pokeemerald-expansion`**.
- **`pokeemerald-expansion`** incorporates regular updates from `pokeemerald`, including bug fixes and documentation improvements.

# [Getting Started](INSTALL.md)

❗❗ **Important**: Do not use GitHub's "Download Zip" option as it will not include commit history. This is necessary if you want to update or merge other feature branches.

If you're new to git and GitHub, [Team Aqua's Asset Repo](https://github.com/Pawkkie/Team-Aquas-Asset-Repo/) has a [guide to forking and cloning the repository](https://github.com/Pawkkie/Team-Aquas-Asset-Repo/wiki/The-Basics-of-GitHub). Then you can follow one of the following guides:

## 📥 [Installing **`pokeemerald-expansion`**](INSTALL.md)
## 🏗️ [Building **`pokeemerald-expansion`**](INSTALL.md#Building-pokeemerald-expansion)
## 🚚 [Migrating from **`pokeemerald`**](INSTALL.md#Migrating-from-pokeemerald)
## 🚀 [Updating **`pokeemerald-expansion`**](INSTALL.md#Updating-pokeemerald-expansion)

# [Documentation](https://rh-hideout.github.io/pokeemerald-expansion/)

For detailed documentation, visit the [pokeemerald-expansion documentation page](https://rh-hideout.github.io/pokeemerald-expansion/).

# [Contributions](CONTRIBUTING.md)
If you are looking to [report a bug](CONTRIBUTING.md#Bug-Report), [open a pull request](CONTRIBUTING.md#Pull-Requests), or [request a feature](CONTRIBUTING.md#Feature-Request), our [`CONTRIBUTING.md`](CONTRIBUTING.md) has guides for each.

# [Community](https://discord.gg/6CzjAG6GZk)

[![](https://dcbadge.limes.pink/api/server/6CzjAG6GZk)](https://discord.gg/6CzjAG6GZk)

Our community uses the [ROM Hacking Hideout (RHH) Discord server](https://discord.gg/6CzjAG6GZk) to communicate and organize. Most of our discussions take place there, and we welcome anybody to join us!
