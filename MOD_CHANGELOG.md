# Mod Changelog

Tracks changes made on top of pokeemerald-expansion as part of the "Gen 1–3 non-legendaries obtainable in normal gameplay" project.

Baseline: pokeemerald-expansion default branch (Gen 9 mechanics, all species enabled, all four battle mechanics implemented).

Format: entries are grouped by subsystem — engine changes first, then scripts/sidequests (Hoenn → Sevii → Kanto), then map connections, then wild encounters, then species data, and finally the running obtainability list.

---

## Contents

1. [Engine & Framework](#engine--framework)
2. [Scripts & Sidequests](#scripts--sidequests)
3. [Map Connections & Warps](#map-connections--warps)
4. [Wild Encounters](#wild-encounters)
5. [Species Data](#species-data)
6. [Obtainability Status (Gen 1–3 non-legendaries)](#obtainability-status-gen-13-non-legendaries)

---

## Engine & Framework

### Custom-priced Pokémarts
Vanilla marts always use the item's global `GetItemPrice`, which means overriding a berry's price via `.price` would also inflate sell prices (berry-tree exploit). Instead, added a parallel per-mart price list so custom marts (e.g. the Lilycove Berry Vendor) can set arbitrary prices without touching item globals.

- [src/shop.c](src/shop.c) — added `const u32 *priceList` to `struct MartInfo`, reset in `SetShopItemsForSale`. New helper `GetMartItemPrice(itemId)` returns `priceList[i]` for a matching item or falls back to `GetItemPrice(itemId) >> IsPokeNewsActive(POKENEWS_SLATEPORT)`. All 4 `GetItemPrice(...) >> IsPokeNewsActive(POKENEWS_SLATEPORT)` call sites (list display, initial totalCost, importance-item branch, buy-quantity handler) now call `GetMartItemPrice`. Added `CreatePokemartMenuWithPrices(items, prices)`.
- [include/shop.h](include/shop.h) — exported `CreatePokemartMenuWithPrices`.
- [src/scrcmd.c](src/scrcmd.c) — added `ScrCmd_pokemartpriced` (reads two `.4byte` pointers).
- [include/constants/script_commands.h](include/constants/script_commands.h) — added `SCR_OP_POKEMART_PRICED = 0xE7`.
- [data/script_cmd_table.inc](data/script_cmd_table.inc) — registered opcode `0xe7 → ScrCmd_pokemartpriced`.
- [asm/macros/event.inc](asm/macros/event.inc) — new `pokemartpriced products, prices` macro.

### `ScriptGiveMonToPC` special
Delivers a wild-caught Pokémon directly to PC storage, bypassing the party. Used by the Devon Corp Porygon vendor.

- [src/script_pokemon_util.c](src/script_pokemon_util.c) — new `ScriptGiveMonToPC` special. Reads species from `gSpecialVar_0x8004` and level from `gSpecialVar_0x8005`, creates the mon with player OT, and calls `CopyMonToPC` directly (bypassing party). Sets `gSpecialVar_Result` and the Seen/Caught Pokédex flags.
- [include/script_pokemon_util.h](include/script_pokemon_util.h) — declared `ScriptGiveMonToPC`.
- [data/specials.inc](data/specials.inc) — registered `ScriptGiveMonToPC`.

### Roamer-catch scriptable flag
Adds a save-persistent signal that scripts can query when the roaming Eon-duo member is captured (used by Norman's Eon Ticket handoff).

- [include/constants/flags.h](include/constants/flags.h) — `FLAG_CAUGHT_ROAMING_EON_MON` at `SYSTEM_FLAGS + 0x23` (repurposed `FLAG_UNUSED_0x023`).
- [src/battle_main.c](src/battle_main.c) — the `BATTLE_TYPE_ROAMER` cleanup path around `SetRoamerInactive` now also `FlagSet(FLAG_CAUGHT_ROAMING_EON_MON)` when `gBattleOutcome == B_OUTCOME_CAUGHT`.

### Safari Zone — independent Hoenn/Kanto step counters
The Safari Zone game normally has a single step budget (500 in Emerald, 600 in FRLG). Since the player can now travel between the Emerald and FRLG safari maps mid-game via the warp pad, a single counter would let a walk in one region drain the other's budget.

Two counters now run in parallel:
- `gSafariZoneStepCounter` (existing, `u16`): decrements while the player is on any non-Kanto map. Starts at **500**.
- `gSafariZoneStepCounterKanto` (new, `u16`): decrements while the player is on a map tagged `MAPSEC_KANTO_SAFARI_ZONE`. Starts at **600**.

Either counter hitting zero fires the existing `SafariZone_EventScript_TimesUp`, which warps the player back to the Route 121 entrance building and ends the Safari game. All 9 FRLG safari maps (Center + Center Rest House, East + East Rest House, North + North Rest House, West + West Rest House, Secret House) are tagged `MAPSEC_KANTO_SAFARI_ZONE`, so region discrimination is a simple `gMapHeader.regionMapSectionId` check.

- [include/safari_zone.h](include/safari_zone.h) — exported `gSafariZoneStepCounterKanto`.
- [src/safari_zone.c](src/safari_zone.c) — added the new counter; `EnterSafariMode` initializes both unconditionally; `SafariZoneTakeStep` routes the tick by region; `ExitSafariMode` zeros both. Added `#include "constants/region_map_sections.h"`.

### FRLG layouts & tilesets compiled into the Emerald build
Vanilla `pokeemerald-expansion` conditionally compiles FRLG map layouts and tilesets out of the Emerald build via a `layout_version: "frlg"` filter in the mapjson tool and `#if IS_FRLG` guards over every FRLG tileset struct/tile/palette/metatile blob. In this mod, many overworld connections and warps (Two Island, Victory Road, Cerulean Cave, Rock Tunnel, Diglett's Cave, Cerulean Cave, Mt. Moon, and every FRLG Safari sub-map) send the player into FRLG-only maps. Under the vanilla setup, every one of those destinations resolves to `NULL` in `gMapLayouts`, `GetMapLayout()` returns `NULL`, and `LoadCurrentMapData` deref-crashes to open-bus (typical symptom: "Jumped to invalid address: BAFFFFA"). The Safari Zone Northeast attendant warp to `MAP_SAFARI_ZONE_CENTER` triggered this on the first outbound trip.

Fix: FRLG layouts, tileset structs, tile graphics, palettes, and metatile blobs now compile alongside the Emerald ones in every build, and the mapjson generator always emits the real layout symbol into `gMapLayouts` (never `NULL`). ROM footprint grows from ~79% to ~82% of 32 MB; still comfortably under the cart limit.

- [tools/mapjson/mapjson.cpp](tools/mapjson/mapjson.cpp) — in `generate_layouts_text`, removed the `if (version-mismatch) continue;` filter so FRLG layouts always emit their `<name>_Border`, `<name>_Blockdata`, and `<name>_Layout` symbols in the Emerald build. In `generate_layouts_table_text`, removed the `NULL`-emit branch so `layouts_table.inc` always references the real layout symbol.
- [src/data/tilesets/headers.h](src/data/tilesets/headers.h) — removed the `#if !IS_FRLG` / `#else` / `#endif // IS_FRLG` fence around the Emerald and FRLG `struct Tileset` blocks. Both sets now define side-by-side (no symbol collisions; verified). Includes `gTileset_General_Frlg` (with `.callback = InitTilesetAnim_General_Frlg`), `gTileset_BuildingFrlg`, `gTileset_FuchsiaCity`, `gTileset_SafariZoneBuilding`, all Kanto route/city/interior tilesets, `gTileset_PokemonLeague` (FRLG variant), `gTileset_HallOfFame` (FRLG variant), etc.
- [src/data/tilesets/metatiles.h](src/data/tilesets/metatiles.h) — same fence removal for the `INCBIN_U16` metatile / metatile-attribute arrays (`gMetatiles_Building_Frlg`, `gMetatiles_General_Frlg`, `gMetatiles_SafariZoneBuilding`, and all Kanto secondaries).
- [src/data/tilesets/graphics.h](src/data/tilesets/graphics.h) — removed the `#if IS_FRLG` / `#endif` guard so `gTilesetTiles_General_Frlg`, `gTilesetPalettes_General_Frlg`, `gTilesetTiles_Building_Frlg`, `gTilesetTiles_SafariZoneBuilding`, and every FRLG palette/tile blob link into the Emerald ROM.
- [src/tileset_anims.c](src/tileset_anims.c) — no changes needed; FRLG anim callbacks (e.g. `InitTilesetAnim_General_Frlg`) are already defined unconditionally.

Verified with a full rebuild: no undefined references, no duplicate symbols, ROM at 81.91% of 32 MB (EWRAM 86.43%, IWRAM 86.60%). All 344 FRLG-only layouts now resolve to real pointers in `gMapLayouts`, unblocking every FRLG destination the mod wires up.

---

## Scripts & Sidequests

### Hoenn

#### Lilycove City — post-Champion Berry Vendor
Adds a female NPC (`OBJ_EVENT_GFX_WOMAN_2`) outside the Lilycove City Department Store at **(29, 7)**, one tile SE of the store entrance. She is hidden by `FLAG_HIDE_LILYCOVE_BERRY_VENDOR` until the player becomes Champion (`FLAG_IS_CHAMPION`), at which point `LilycoveCity_OnTransition` clears the flag.

She opens a mart-style buy menu split across 5 price tiers (multichoice → dedicated Pokémart per tier):
- **$200** — Cheri, Chesto, Pecha, Rawst, Aspear, Leppa, Oran, Persim, Lum, Sitrus
- **$500** — Figy, Wiki, Mago, Aguav, Iapapa, Razz, Bluk, Nanab, Wepear, Pinap
- **$1000** — Pomeg, Kelpsy, Qualot, Hondew, Grepa, Tamato, Cornn, Magost, Rabuta, Nomel, Spelon, Pamtre, Watmel, Durin, Belue
- **$10000** — Chilan
- **$20000** — Enigma

Uses the `pokemartpriced` opcode added under Engine & Framework so the per-mart prices don't inflate the item sell price globally.

- [include/constants/flags.h](include/constants/flags.h) — renamed `FLAG_UNUSED_0x021` → `FLAG_HIDE_LILYCOVE_BERRY_VENDOR`.
- [data/maps/LilycoveCity/map.json](data/maps/LilycoveCity/map.json) — new object_event at (29, 7).
- [data/maps/LilycoveCity/scripts.inc](data/maps/LilycoveCity/scripts.inc) — `LilycoveCity_OnTransition` now clears the hide flag on champions (else sets it). Appended `LilycoveCity_EventScript_BerryVendor` with a `dynmultistack` tier picker that loops back after each mart, five `pokemartpriced` handlers, five item lists (`.2byte` + `ITEM_NONE`), five parallel price lists (`.4byte`), and vendor greeting / farewell / tier label text strings.

Result: post-game berry economy sink; the special / EV / flavor berries plus Chilan and Enigma become reliably purchasable without patching global item prices.

#### Devon Corp 1F — Porygon vendor
Adds a new NPC on the ground floor of Devon Corporation (Rustboro) who sells the player a Porygon for ¥1000. The Porygon is created at level 15 and delivered **directly to the PC** — even if the party has open slots — matching the flavor of buying "virus protection software" for the PC.

Dialogue:
- Offer: *"I'll sell you virus protection software for your PC for only ¥1000! What do you say?"* (Yes/No)
- Yes with money: pays ¥1000, plays item fanfare, Porygon added to PC, *"Congratulations! Check your PC!"*
- Yes without money: *"You'll need at least ¥1000 to purchase a license."*
- No: *"Never say I didn't warn you!"*
- After purchase: *"Your PC is already fully protected. Enjoy your new digital friend!"*

Uses the `ScriptGiveMonToPC` special added under Engine & Framework so the Porygon lands in storage regardless of party state.

- [data/maps/RustboroCity_DevonCorp_1F/map.json](data/maps/RustboroCity_DevonCorp_1F/map.json) — new `OBJ_EVENT_GFX_DEVON_EMPLOYEE` at (10, 6) linked to `RustboroCity_DevonCorp_1F_EventScript_PorygonVendor`.
- [data/maps/RustboroCity_DevonCorp_1F/scripts.inc](data/maps/RustboroCity_DevonCorp_1F/scripts.inc) — added vendor script + Yes/No/insufficient-funds/already-purchased branches and text strings.
- [include/constants/flags.h](include/constants/flags.h) — renamed `FLAG_UNUSED_0x020` (0x20) to `FLAG_PURCHASED_DEVON_PORYGON` to gate the one-time purchase.

Rationale: makes Porygon obtainable in normal gameplay for a modest fee, thematically consistent (Devon = tech company, Porygon = digital Pokémon, "virus protection" = wink at Porygon's origin). The always-to-PC behavior lets the player buy it without disrupting their active party.

#### Meteor Falls — Steven's Fruit Salad quest (Tri-Pass reward)
After defeating Steven in his cave (`FLAG_DEFEATED_METEOR_FALLS_STEVEN` already set by the existing battle), talking to him again now triggers a one-time trade: Steven is starving and wants "an enormous fruit salad" made from every berry the Lilycove post-Champion vendor sells. If the player has 1 of all 37 berries in their bag (`ITEM_CHERI_BERRY` through `ITEM_BELUE_BERRY`, plus `ITEM_CHILAN_BERRY` and `ITEM_ENIGMA_BERRY`), a YES/NO confirmation appears; on YES all 37 berries are removed and Steven hands over `ITEM_TRI_PASS`. The grant also `setflag FLAG_SYS_SEVII_MAP_123` so the Town Map pages for Sevii isles 1-3 appear immediately.

Flow:
- Any berry missing → *"A single BERRY missing would ruin the whole salad."* — return without penalty; player can try again.
- All 37 present → YES/NO. NO → *"A trainer's BERRIES are precious. Don't mind me."* NO decrements nothing and the quest can be re-attempted.
- All 37 present + YES → 37 `removeitem` calls, "feast" flavor msgbox, `giveitem ITEM_TRI_PASS`, `setflag FLAG_RECEIVED_STEVEN_TRI_PASS`, `setflag FLAG_SYS_SEVII_MAP_123`, closing dialogue.
- Post-trade → falls through to the original `MyPredictionCameTrue` dialogue so re-visits keep the vanilla flavor beat.

- [include/constants/flags.h](include/constants/flags.h) — renamed `FLAG_UNUSED_0x022` (0x22) → `FLAG_RECEIVED_STEVEN_TRI_PASS` to gate the one-time trade.
- [data/maps/MeteorFalls_StevensCave/scripts.inc](data/maps/MeteorFalls_StevensCave/scripts.inc) — rewrote `MeteorFalls_StevensCave_EventScript_Defeated` to branch on the new flag, then run 37 sequential `checkitem` calls (each `goto_if_eq VAR_RESULT, FALSE, ...MissingBerries`), a YES/NO confirmation, 37 `removeitem` calls, and `giveitem ITEM_TRI_PASS`. Added `MeteorFalls_StevensCave_EventScript_MissingBerries`, `_DeclinedTrade`, and `_StevenPostTrade` subscripts plus 6 new text strings (`Text_StevenStarving`, `Text_MissingBerries`, `Text_YouHaveThemAll`, `Text_DeclinedTrade`, `Text_StevenThanksFeast`, `Text_StevenGivesTriPass`). The original `Text_MyPredictionCameTrue` message is preserved and reused after the trade.

Rationale: Tri-Pass is normally a JP-only demo/e-Reader item and never obtainable in vanilla Emerald; this hooks it to an endgame post-Champion economy sink that consumes the entire output of the Lilycove Berry Vendor in one purchase.

#### Eon Duo — deferred to postgame with an in-story Eon Ticket handoff
The Latias/Latios TV broadcast and the Eon Ticket are no longer tied to mid-game progression or Mystery Gift. Both are now postgame content driven by real story beats.

- **SS Ticket script split** in [data/scripts/players_house.inc](data/scripts/players_house.inc): the vanilla `PlayersHouse_1F_EventScript_GetSSTicketAndSeeLatiTV` no longer plays the Latias/Latios TV segment. Dad still delivers the SS Ticket exactly as before, and the script ends after `Text_DadShouldStayLonger`. The TV+color-choice+`InitRoamer` block moves verbatim into a new script `PlayersHouse_1F_EventScript_ChampionLatiTV` that reuses the same text (`Text_IsThatABreakingStory`, `Text_LatiEmergencyNewsFlash`, `Text_WhatColorDidTheySay`, `Text_StillUnknownPokemon`).
- **Postgame trigger** in [data/maps/LittlerootTown_MaysHouse_1F/scripts.inc](data/maps/LittlerootTown_MaysHouse_1F/scripts.inc) and [data/maps/LittlerootTown_BrendansHouse_1F/scripts.inc](data/maps/LittlerootTown_BrendansHouse_1F/scripts.inc): the `OnTransition` handler now `call_if_set FLAG_IS_CHAMPION, ...CheckPostChampionLatiTV`, which sets `VAR_TEMP_1 = 1` when the player is Champion and `FLAG_LATIOS_OR_LATIAS_ROAMING` is still unset. The `OnFrame` table gets a matching `map_script_2 VAR_TEMP_1, 1, PlayersHouse_1F_EventScript_ChampionLatiTV` entry. Both houses fire the event symmetrically for either player gender, exactly once per save file.
- **Norman hands over the Eon Ticket** in [data/maps/PetalburgCity_Gym/scripts.inc](data/maps/PetalburgCity_Gym/scripts.inc): `EventScript_NormanPostBattle` now `call`s a new `EventScript_ShouldGiveEonTicket` helper that returns `TRUE` when `FLAG_CAUGHT_ROAMING_EON_MON` is set and `FLAG_ENABLE_SHIP_SOUTHERN_ISLAND` is not yet set. On `TRUE`, Norman speaks two new lines (`Text_NormanEonTicketIntro` / `Text_NormanEonTicketTryLilycove`), `giveitem ITEM_EON_TICKET`, and `setflag FLAG_ENABLE_SHIP_SOUTHERN_ISLAND`, which is the exact wiring the vanilla Mystery-Gift path used. The default post-Facade Norman dialog is untouched otherwise.
- **Southern Island / S.S. Tidal** — no code changes needed. `data/maps/LilycoveCity_Harbor/scripts.inc` already surfaces the Southern Island destination when `FLAG_ENABLE_SHIP_SOUTHERN_ISLAND` is set, and `data/maps/SouthernIsland_Interior/scripts.inc` already picks the non-roaming Eon-duo mon via `VAR_ROAMER_POKEMON` for the on-island encounter. Both fall through with the vanilla setup as soon as Norman hands over the ticket.

The catch hook that sets `FLAG_CAUGHT_ROAMING_EON_MON` lives under Engine & Framework.

Net flow: **Champion → home TV news → Mom asks color → roamer starts → catch the roamer → talk to Dad → Eon Ticket → board S.S. Tidal at Lilycove → Southern Island → catch the other Lati.**

### Sevii Islands

#### Celio / Ruby / Sapphire / Rainbow Pass sidequest repaired
The whole One-Island Celio sidequest was present in the tree but silently broken: every flag it depended on was `#define … 0` in the FRLG-alias block, and its completion script mis-set Hoenn's `FLAG_IS_CHAMPION`. The player also had no way to reach One Island's Poké Center without triggering the vanilla Bill-arrival cutscene (which is no longer wired). Now repaired end-to-end:

- **Flags given real slots** in [include/constants/flags.h](include/constants/flags.h). The Emerald-side declarations at `SYSTEM_FLAGS + 0x24..0x2C` are documentation-only; the FRLG-alias stubs (`FLAG_GOT_RUBY`, `FLAG_RECOVERED_SAPPHIRE`, `FLAG_SEVII_DETOUR_FINISHED`, `FLAG_SYS_SEVII_MAP_123`, `FLAG_SYS_SEVII_MAP_4567`, `FLAG_HIDE_ONE_ISLAND_BILL`, `FLAG_HIDE_ONE_ISLAND_POKECENTER_BILL`, `FLAG_HIDE_ONE_ISLAND_POKECENTER_CELIO`, `FLAG_HIDE_CERULEAN_CAVE_GUARD`) now resolve to those slots. That makes `setflag FLAG_GOT_RUBY` in `MtEmber_RubyPath_B5F_EventScript_Ruby` and `setflag FLAG_RECOVERED_SAPPHIRE` in `FiveIsland_RocketWarehouse_EventScript_DefeatedGideon` actually persist, and lets Celio's dispatcher branch on them for real.
- **Steven grants the Town-Map pages too** (see Meteor Falls entry above): the fruit-salad hand-over now also `setflag FLAG_SYS_SEVII_MAP_123` alongside the `ITEM_TRI_PASS` grant, so the Town Map pages for Sevii 1-3 appear immediately.
- **One Island Poké Center rewired** in [data/maps/OneIsland_PokemonCenter_1F_Frlg/scripts.inc](data/maps/OneIsland_PokemonCenter_1F_Frlg/scripts.inc):
  - `OnTransition` unconditionally `setflag FLAG_HIDE_ONE_ISLAND_POKECENTER_BILL` so Bill's object event never spawns in this build. The two Bill-positioning helper calls (`SetBillCelioFirstMeetingPos`, `SetBillCelioReadyToLeavePos`) are dropped from the dispatcher.
  - `OnFrame` no longer runs `MeetCelioScene`. The vanilla FRLG scene has Bill walk in with the player from Vermilion — none of that happens on this build's Tri-Pass ferry route, so the trigger is removed.
  - `EventScript_Celio` drops the `IsNationalPokedexEnabled` gate and the state-7 `CelioGiveBillFact` random-facts branch. On the player's first talk, Celio goes straight into `CelioRequestRuby`; on subsequent visits after the machine is done, state 6 and state 7 both funnel into `CelioJustGivenSapphire`.
  - `EventScript_GiveCelioSapphire` no longer sets `FLAG_IS_CHAMPION` (that's Hoenn's champion flag — the player is already Hoenn Champion at this point), no longer calls `special SetPostgameFlags` (FRLG-only postgame bookkeeping), no longer sets `FLAG_HIDE_LORELEI_IN_HER_HOUSE` (Lorelei isn't in the flow), and no longer calls `special InitRoamer` (would spawn a Kanto legendary bird with an uninitialised species argument, and we already have the Eon-duo roamer wired). It now `setflag FLAG_HIDE_CERULEAN_CAVE_GUARD` (which now actually opens Cerulean Cave) and `setflag FLAG_SEVII_DETOUR_FINISHED`.
- **Outdoor One Island** in [data/maps/OneIsland_Frlg/scripts.inc](data/maps/OneIsland_Frlg/scripts.inc): `OnTransition` also `setflag FLAG_HIDE_ONE_ISLAND_BILL`, keeping the Bill object hidden on the harbor-side overworld. The unreachable `EnterOneIslandFirstTime` OnFrame trigger (tied to the never-set `VAR_MAP_SCENE_ONE_ISLAND_HARBOR == 2` from the Cinnabar cutscene) is left in place as dead code.

Net flow: **Hoenn Champion → give Steven the fruit salad → Tri-Pass + Sevii 1-3 map pages → Seagallop ferry from Two Island Harbor to One Island → talk to Celio → Mt. Ember Ruby quest → Rainbow Pass + Sevii 4-7 map pages → Rocket Warehouse Sapphire quest → return the Sapphire → machine linked, Cerulean Cave guard leaves.**

### Kanto (Indigo Plateau)

#### Kanto Indigo League — repaired as a standalone postgame rung
The FRLG Elite Four + Champion sequence at the top of Victory Road was broken in the Emerald build because every one of the FRLG "defeated" flags collided on flag id 0. Fixed and given its own postgame flow:

- **Flags reassigned to real system slots** in [include/constants/flags.h](include/constants/flags.h). The seven `FLAG_UNUSED_0x881`–`FLAG_UNUSED_0x887` slots at (`SYSTEM_FLAGS + 0x21`)–(`SYSTEM_FLAGS + 0x27`) are repurposed:
  - `FLAG_DEFEATED_LORELEI`, `FLAG_DEFEATED_BRUNO`, `FLAG_DEFEATED_AGATHA`, `FLAG_DEFEATED_LANCE`, `FLAG_DEFEATED_CHAMP`
  - `FLAG_KANTO_LEAGUE_UPGRADE_GIVEN`, `FLAG_KANTO_LEAGUE_DUBIOUS_DISC_GIVEN` (one-shot reward flags)
- The stub `#define FLAG_DEFEATED_* 0` lines in the FRLG-aliases block are removed.
- **Rematch dispatch retargeted** in every E4 room ([data/maps/PokemonLeague_LoreleisRoom_Frlg/scripts.inc](data/maps/PokemonLeague_LoreleisRoom_Frlg/scripts.inc), [PokemonLeague_BrunosRoom_Frlg](data/maps/PokemonLeague_BrunosRoom_Frlg/scripts.inc), [PokemonLeague_AgathasRoom_Frlg](data/maps/PokemonLeague_AgathasRoom_Frlg/scripts.inc), [PokemonLeague_LancesRoom_Frlg](data/maps/PokemonLeague_LancesRoom_Frlg/scripts.inc), [PokemonLeague_ChampionsRoom_Frlg](data/maps/PokemonLeague_ChampionsRoom_Frlg/scripts.inc)): `FLAG_IS_CHAMPION` → `FLAG_DEFEATED_CHAMP`. That flag is Hoenn's champion status; using it to gate the FRLG rematches was accidentally serving all Kanto E4 rematches to any Hoenn champion.
- **Indigo Plateau PC guard unlocked** ([data/maps/IndigoPlateau_PokemonCenter_1F_Frlg/scripts.inc](data/maps/IndigoPlateau_PokemonCenter_1F_Frlg/scripts.inc)): the `OnTransition` no longer runs the National-Dex + FLAG_IS_CHAMPION blocking dance, and the guard just gives the `FaceEliteFourGoodLuck` speech. No more Kanto side-quest gating from Emerald's flag layout.

#### Kanto Champion — Two Island Sabbatical postgame flow
[data/maps/PokemonLeague_ChampionsRoom_Frlg/scripts.inc](data/maps/PokemonLeague_ChampionsRoom_Frlg/scripts.inc): after defeating the FRLG Champion, the vanilla Oak-arrival + Hall of Fame sequence is replaced with a mini postgame:

1. Rival delivers the vanilla post-battle line.
2. `special HealPlayerParty` heals the party (with `MUS_HEAL` cue).
3. `MUS_RG_ENDING` plays under a congratulatory `Text_TwoIslandSabbaticalCongrats` message framing the run as the player's "Two Island Sabbatical" through the Indigo League.
4. **First win**: `giveitem ITEM_UPGRADE`, `setflag FLAG_KANTO_LEAGUE_UPGRADE_GIVEN`, and a message inviting the player to return for a rematch.
5. **Second win**: `giveitem ITEM_DUBIOUS_DISC`, `setflag FLAG_KANTO_LEAGUE_DUBIOUS_DISC_GIVEN`.
6. **Third+ win**: a shorter thank-you message.
7. `setflag FLAG_DEFEATED_CHAMP` (which unlocks the Rematch trainer IDs everywhere), then `warp MAP_INDIGO_PLATEAU_POKEMON_CENTER_1F, 11, 15` — one tile north of the PC's exterior warp, so the player lands next to the Nurse & the front door instead of the Hall of Fame.

Intro/RematchIntro dispatch is also switched from `FLAG_SYS_GAME_CLEAR` to `FLAG_DEFEATED_CHAMP` so the intro dialog reflects Kanto-specific progress, not Hoenn's ending.

The old Oak movement/text scripts and the Hall of Fame warp are left in place but no longer reachable.

---

## Map Connections & Warps

### Route 115 — Mt. Moon entrance
Adds a warp on Route 115 in the northern cliff area at **(6, 3)** that leads into `MAP_MT_MOON_1F` at existing warp id 0 (arrival at 5, 6). The interior floors (1F ↔ B1F ↔ B2F) already have their normal FRLG inter-floor warps wired, so the player can traverse the mountain end-to-end.

The "other end" of Mt. Moon is `MAP_MT_MOON_1F` warp id 3 at (18, 37), which in vanilla FRLG exits to `MAP_ROUTE4`. Since Route 4 isn't reachable in Emerald, that warp is now retargeted to Route 115 warp id 4 (a new landing tile at **(30, 37)**, elevation 3, three tiles east of the Meteor Falls entrance at (27, 37)). The exit therefore drops the player onto the southern shoreline right next to the Meteor Falls entrance — matching the requested "other end near the shoreline / Meteor Falls side".

Note: only the `warp_events` are wired — the Route 115 layout metatiles at (6, 3) and (30, 37) are not repainted to show cave-entrance graphics. The warps work functionally; visual polish (cave-mouth metatiles) can be added in Porymap.

Files changed:
- [data/maps/Route115/map.json](data/maps/Route115/map.json) — appended warp_events #3 (elevation 0 → `MAP_MT_MOON_1F` warp 0) and #4 (elevation 3 landing tile; `dest_map` self-referential as a no-op since it's only a target).
- [data/maps/MtMoon_1F_Frlg/map.json](data/maps/MtMoon_1F_Frlg/map.json) — warp #3 at (18, 37) now targets `MAP_ROUTE115` warp id 4 instead of `MAP_ROUTE4` warp id 0.

### Route 111 / 112 — Diglett's Cave entrances
Diglett's Cave (Kanto FRLG) is now reachable in Hoenn:

- **Route 112 (Mt. Chimney base area)** — new warp at **(5, 15)** enters `MAP_DIGLETTS_CAVE_NORTH_ENTRANCE` (warp id 1). This is the north side of Diglett's Cave in the northwest cliff strip of Route 112, near the base of Mt. Chimney.
- **Route 111 (desert side)** — new warp at **(35, 70)** enters `MAP_DIGLETTS_CAVE_SOUTH_ENTRANCE` (warp id 0). This sits on the east edge of the Route 111 desert, between the Mirage Tower and Desert Ruins latitudes.

Inside the cave, the two vanilla FRLG "exit to overworld" warps that pointed at Kanto Route 2 (north entrance) and Kanto Route 11 (south entrance) are retargeted at the new Route 112 and Route 111 warp ids respectively. Traversing `MAP_DIGLETTS_CAVE_B1F` between the two entrances works exactly as it did in FRLG.

Same caveat as Mt. Moon: only the `warp_events` are wired — the Route 112 and Route 111 layout metatiles at the two new warp coordinates are not repainted to show cave-mouth graphics. Cave entrance art can be added in Porymap.

Files changed:
- [data/maps/Route112/map.json](data/maps/Route112/map.json) — appended warp_events #6 → `MAP_DIGLETTS_CAVE_NORTH_ENTRANCE` warp 1.
- [data/maps/Route111/map.json](data/maps/Route111/map.json) — appended warp_events #5 → `MAP_DIGLETTS_CAVE_SOUTH_ENTRANCE` warp 0.
- [data/maps/DiglettsCave_NorthEntrance_Frlg/map.json](data/maps/DiglettsCave_NorthEntrance_Frlg/map.json) — warp #1 (outside door) retargeted from `MAP_ROUTE2` warp 3 → `MAP_ROUTE112` warp 6.
- [data/maps/DiglettsCave_SouthEntrance_Frlg/map.json](data/maps/DiglettsCave_SouthEntrance_Frlg/map.json) — warp #0 (outside door) retargeted from `MAP_ROUTE11` warp 0 → `MAP_ROUTE111` warp 5.

### Route 120 / 121 — Rock Tunnel entrances
Rock Tunnel (Kanto FRLG) is now reachable in Hoenn:

- **Route 120 (mountain pass near Fortree)** — new warp at **(5, 10)** enters `MAP_ROCK_TUNNEL_1F` (warp id 0, the north entrance landing at 17, 2). Placed in the rocky northern strip of Route 120 just south of Fortree.
- **Route 121 (near Lilycove)** — new warp at **(72, 8)** enters `MAP_ROCK_TUNNEL_1F` (warp id 5, the south entrance landing at 18, 37). Placed on the east end of Route 121, close to the Lilycove approach.

Inside Rock Tunnel, the two vanilla FRLG "exit to overworld" warps that pointed at Kanto Route 10 are retargeted at the new Route 120 and Route 121 warp ids respectively. The four internal 1F ↔ B1F ladder warps are untouched, so the full FRLG traversal (ladder puzzle across both floors, trainers included) works as-is.

Same caveat as Mt. Moon and Diglett's Cave: warp coords are wired but the Route 120 / Route 121 layout metatiles at (5, 10) and (72, 8) are not repainted to show cave-mouth graphics. Add cave entrance art in Porymap if desired.

Files changed:
- [data/maps/Route120/map.json](data/maps/Route120/map.json) — appended warp_events #2 → `MAP_ROCK_TUNNEL_1F` warp 0.
- [data/maps/Route121/map.json](data/maps/Route121/map.json) — appended warp_events #1 → `MAP_ROCK_TUNNEL_1F` warp 5.
- [data/maps/RockTunnel_1F_Frlg/map.json](data/maps/RockTunnel_1F_Frlg/map.json) — warp #0 (north exit) retargeted `MAP_ROUTE10` warp 0 → `MAP_ROUTE120` warp 2; warp #5 (south exit) retargeted `MAP_ROUTE10` warp 1 → `MAP_ROUTE121` warp 1.

### Two Island → Victory Road → Indigo Plateau
Kanto's Victory Road (and Route 23 / Indigo Plateau beyond it) are now reachable from the Sevii Islands side of the map:

- **Two Island (bottom-right)** — new warp at **(44, 18)** enters `MAP_VICTORY_ROAD_1F_FRLG` (warp id 1, the vanilla south entrance landing at 11, 20). Placed in the SE corner of Two Island town; add a cave-entrance metatile in Porymap to make it obvious.
- **Victory Road 1F south exit** at (11, 20) — retargeted `MAP_ROUTE23` warp 0 → `MAP_TWO_ISLAND` warp 4 (the new SE tile). Player enters and exits through the same tile pair.
- **Victory Road 2F east exit** — the three vanilla FRLG "exit to Route 23" warps at (49, 13), (48, 12), (47, 13) are left **untouched**; they still point at `MAP_ROUTE23` warp 1. That drops the player at (18, 28) on Route 23, one tile south of the Indigo Plateau Exterior connection edge, so walking north takes them straight to the Elite Four building.

Internal wiring across Victory Road 1F ↔ 2F ↔ 3F is unchanged; the full FRLG boulder puzzle, trainers, and item balls work as-is. Route 23 becomes indirectly reachable through the 2F east exit, and its up-connection to `MAP_INDIGO_PLATEAU_EXTERIOR` gets the player onto the Plateau. Route 23's own south connections (Route 22, Viridian) remain isolated.

Metatile caveat as usual: the Two Island (44, 18) tile is functional but not repainted to show cave graphics. Add art in Porymap if desired.

Files changed:
- [data/maps/TwoIsland_Frlg/map.json](data/maps/TwoIsland_Frlg/map.json) — appended warp_events #4 → `MAP_VICTORY_ROAD_1F_FRLG` warp 1.
- [data/maps/VictoryRoad_1F_Frlg/map.json](data/maps/VictoryRoad_1F_Frlg/map.json) — warp #1 retargeted `MAP_ROUTE23` warp 0 → `MAP_TWO_ISLAND` warp 4.

### Altering Cave ↔ Cerulean Cave 1F warp
- Files: [data/maps/AlteringCave/map.json](data/maps/AlteringCave/map.json), [data/maps/CeruleanCave_1F_Frlg/map.json](data/maps/CeruleanCave_1F_Frlg/map.json)
- Added a two-way warp pair connecting Altering Cave (top section) to Cerulean Cave 1F, providing a direct Hoenn ↔ Kanto shortcut, and **sealed Cerulean Cave's overworld exit** so the player can never leak into the broken Kanto overworld from that door.
  - New Altering Cave warp id 1 at **(8, 2)** → `MAP_CERULEAN_CAVE_1F` warp id 0 (the vanilla stairs tile at (33, 21)).
  - Cerulean Cave 1F warp id 0 at **(33, 21)** — the vanilla FRLG stairs that used to exit to `MAP_CERULEAN_CITY` warp 7 — **retargeted** to `MAP_ALTERING_CAVE` warp id 1. Cerulean Cave now has no path back to Cerulean City; any exit from the cave (including from 2F/B1F, which only chain up to 1F) drops the player back into Altering Cave.
- Rationale: gives players a way to reach Kanto's Cerulean Cave from Hoenn once they've cleared Route 103's water, and forces the return trip through the same tile. This keeps the FRLG Cerulean City overworld (and everything it connects to) firmly out of reach.
- **Note:** The Altering Cave (8, 2) metatile must be set to a warp behavior (stair/hole/door) via Porymap for that end to trigger on step; the Cerulean Cave (33, 21) end already has a stair-tile behavior from vanilla FRLG.

### Safari Zone Northeast ↔ FRLG Safari Center — warp attendant
Adds an NPC warp attendant (`OBJ_EVENT_GFX_MAN_5`) in the Emerald Safari Zone Northeast (Area 6) at tile **(32, 11)**. Interacting with him prompts a YES/NO confirmation; on YES, `SE_WARP_IN` plays and the player is warped to **Center Area (hub)** of the FRLG Safari Zone at (26, 15).

A matching return pad is placed on the FRLG Safari Zone Center hub at tile **(26, 17)** — just north of the Fuchsia City exit warps — which warps the player back to Safari Zone Northeast at (32, 24). Player arrival tiles are offset from the outbound pads so the trip is not accidentally re-triggered.

Files changed:
- [data/maps/SafariZone_Northeast/map.json](data/maps/SafariZone_Northeast/map.json) — added `object_event` at (32, 11) invoking `SafariZone_Northeast_EventScript_WarpAttendantToKanto`. Removed the previous step-triggered `coord_events` pad at (32, 22).
- [data/maps/SafariZone_South/scripts.inc](data/maps/SafariZone_South/scripts.inc) — added `SafariZone_Northeast_EventScript_WarpAttendantToKanto` with YES/NO confirmation, decline branch, and two new text strings (Northeast's scripts live in South's `.inc` file).
- [data/maps/SafariZone_Center_Frlg/map.json](data/maps/SafariZone_Center_Frlg/map.json) — added `coord_events` trigger at (26, 17) invoking `SafariZone_Center_EventScript_WarpPadToHoenn`.
- [data/maps/SafariZone_Center_Frlg/scripts.inc](data/maps/SafariZone_Center_Frlg/scripts.inc) — added `SafariZone_Center_EventScript_WarpPadToHoenn`.

The outbound direction uses interact-to-warp (`lock`/`faceplayer`/YES-NO/`playse SE_WARP_IN`/`warp`). The return direction still uses the standard `lockall` / `playse SE_WARP_IN` / `waitse` / `warp` / `waitstate` / `releaseall` step-triggered idiom.

### FRLG Safari Zone Center — Rest House exit redirected
Entering the Rest House door on FRLG Safari Zone Center previously exited the player back into the FRLG hub, trapping them in Kanto maps. All three Rest House exit warps now redirect to `MAP_ROUTE121_SAFARI_ZONE_ENTRANCE` (the Emerald Safari Zone entrance building), giving the player a controlled route back to Hoenn. A new interior warp landing tile was added to the entrance building at (14, 12), just above the Route 121 exit doormat.

Files changed:
- [data/maps/SafariZone_Center_RestHouse_Frlg/map.json](data/maps/SafariZone_Center_RestHouse_Frlg/map.json) — all three exit warps at (3,9)/(4,9)/(5,9) now target `MAP_ROUTE121_SAFARI_ZONE_ENTRANCE` warp id 4.
- [data/maps/Route121_SafariZoneEntrance/map.json](data/maps/Route121_SafariZoneEntrance/map.json) — added a fifth `warp_event` at (14, 12) to serve as a safe interior landing tile (defensive `dest_map` = `MAP_ROUTE121` in the unlikely case its metatile ever gets a warp behavior).

Note: the FRLG Safari Zone Center's encounter tables and Safari game state may not activate correctly from an Emerald entry; wire this up as part of the larger Safari Zone expansion pass. The current implementation does not remove any active Safari game state when using the Rest House exit — the game will proceed as if the player simply walked out via Route 121.

### FRLG Safari Zone Center — Fuchsia City exit sealed
`MAP_SAFARI_ZONE_CENTER` had three south-edge doormat warps at (25, 30) / (26, 30) / (27, 30) leading to `MAP_FUCHSIA_CITY_SAFARI_ZONE_ENTRANCE` — the FRLG Safari Zone entrance building, which in turn opens onto Fuchsia City proper. Left as-is, the player could walk south from the FRLG hub and escape into a broken Kanto overworld.

All three exits now redirect to `MAP_ROUTE121_SAFARI_ZONE_ENTRANCE` warp id 4 — the same safe landing tile used by the Rest House redirect. So both the "front door" (walk south) and the "back door" (Rest House) return the player to the Emerald Safari Zone entrance building.

The outer FRLG safari sub-maps (`SafariZone_East_RestHouse_Frlg`, `SafariZone_North_RestHouse_Frlg`, `SafariZone_West_RestHouse_Frlg`, `SafariZone_SecretHouse_Frlg`) all exit back to their local safari section only, so they don't leak into Kanto and were left unchanged.

Files changed:
- [data/maps/SafariZone_Center_Frlg/map.json](data/maps/SafariZone_Center_Frlg/map.json) — three south-edge warps redirected.

### FRLG Safari Zone NPC audit — no changes needed
Audited every object_event script referenced by the 9 FRLG safari maps for FRLG-only flags/vars/specials that could misbehave in Emerald. Findings:
- All rest house NPCs (Sara, Scientist, Fisher, Gentleman, Rocker, BaldingMan, CooltrainerF, Man) are plain `msgbox` flavor dialogue.
- All area/rest-house signs and Trainer Tips are plain `MSGBOX_SIGN` text.
- All item balls are simple `finditem` calls gated by their own `FLAG_HIDE_SAFARI_ZONE_*_*` object-visibility flags (unused elsewhere in Emerald → items visible and pickable).
- `SafariZone_Center_OnTransition` sets `FLAG_WORLD_MAP_SAFARI_ZONE_CENTER` — no-op on the Hoenn world map.
- `SafariZone_SecretHouse_EventScript_Attendant` gives HM03 Surf if `FLAG_GOT_HM03` isn't set; will simply repeat the "explain surf" branch for any Hoenn player who already has Surf.

No FRLG-specific flags, `IS_FRLG`/`GetGameVersion` checks, warden-quest gating, or Kanto Pokédex assumptions were found. The FRLG safari scripts are fully compatible with Emerald as-is; no patching required.

---

## Wild Encounters

### Encounter tables merged for EMERALD build

Previously, FRLG-imported maps had two encounter tables — one gated by `#ifdef FIRERED` and one by `#ifdef LEAFGREEN` — meaning no encounters would fire in the Emerald build. Each pair is now merged into a single `#ifdef EMERALD`-gated table by giving the entry a `base_label` with no version substring (which the generator wraps in `#ifdef EMERALD`). Where the FR and LG tables were byte-identical, the merge reduces to a rename + duplicate deletion. Where they differed, species were interleaved and deduplicated to fit the fixed slot count.

All encounter-JSON changes are in [src/data/wild_encounters.json](src/data/wild_encounters.json) with the C table generated to [src/data/wild_encounters.h](src/data/wild_encounters.h).

#### Mt. Moon 1F / B1F / B2F
FR and LG land tables were identical in the JSON; merge is a rename + duplicate deletion.

- `sMtMoon1F` (encounter_rate 7) — Zubat 7–10, Geodude 7–9, Paras 8, Clefairy 8.
- `sMtMoonB1F` (encounter_rate 5) — Paras 5–10 across all 12 slots.
- `sMtMoonB2F` (encounter_rate 7) — Zubat 8–11, Geodude 9–10, Paras 10 & 12, Clefairy 10 & 12.

#### Diglett's Cave B1F
`sDiglettsCaveB1F_FireRed` and `sDiglettsCaveB1F_LeafGreen` were byte-identical in the JSON, so the merge is a rename plus deletion of the `_LeafGreen` duplicate.

- `sDiglettsCaveB1F` (encounter_rate 5): Diglett Lv 15–22 filling most slots, Dugtrio Lv 29 and Lv 31 in the two rare slots.

#### Rock Tunnel 1F / B1F
FR and LG tables were byte-identical for both floors; merge is a rename + duplicate deletion.

- `sRockTunnel1F` (encounter_rate 7) — Zubat 15–16, Geodude 15–17, Mankey 16–17, Machop 16–17, Onix 13 & 15.
- `sRockTunnelB1F` (encounter_rate 7) — Zubat 15–16, Geodude 15–17, Mankey 16–17, Machop 17, Onix 13, 15, 17. Rock-smash: Geodude Lv 5–30, Graveler Lv 25–40.

#### Victory Road 1F / 2F / 3F
FR has **Arbok** in slot 6, LG has **Sandslash**. To keep both LG-exclusive and FR-exclusive mons in a single unversioned table, the FR entry is renamed to unversioned and its duplicated final Marowak slot is swapped to Sandslash.

- `sVictoryRoad1F` (encounter_rate 7) — Machop 32, Geodude 32, Onix 40 / 43 / 46, Zubat 32, Arbok 44, Golbat 44, Marowak 44, Machoke 44 / 46, Sandslash 46.
- `sVictoryRoad2F` (encounter_rate 7) — Machop 34, Geodude 34, Primeape 42, Onix 45 / 48, Zubat 34, Arbok 46, Golbat 46, Marowak 46, Machoke 46 / 48, Sandslash 48.
- `sVictoryRoad3F` (encounter_rate 7) — same as 1F.

#### FRLG Safari Zone (Center / East / North / West)
Each pair (`_FireRed` + `_LeafGreen`) is merged with interleave + dedupe. Slot rules: FR slot 0, LG slot 0, FR slot 1, LG slot 1, …; deduplicate by `(species, min_level, max_level)`; truncate to slot count (land 12, water 5, fishing 10); pad short entries by repeating the last; `encounter_rate` = max of the two sources.

Result: every FRLG Safari species from either version is now catchable on the corresponding map. Highlights:
- Center — both `NIDORAN_M`/`NIDORAN_F`, both `NIDORINO`/`NIDORINA`, `SCYTHER` + `PINSIR`, `CHANSEY`, `PSYDUCK` + `SLOWPOKE`, `DRATINI` + `DRAGONAIR`.
- East, North_Frlg, West — union of both games' species mixes (see `sSafariZoneEast_LandMons` etc. in the generated header).

Maps affected: `MAP_SAFARI_ZONE_CENTER`, `MAP_SAFARI_ZONE_EAST`, `MAP_SAFARI_ZONE_NORTH_FRLG`, `MAP_SAFARI_ZONE_WEST`.

#### Cerulean Cave 1F / B1F
FRLG had version-exclusive water and fishing slots (Psyduck line in FR, Slowpoke line in LG). Merged both variants so every player can catch species from both games.

- **1F water** — Psyduck 30–40, Slowpoke 30–40, Golduck 40–50, Slowbro 40–50, Golduck 45–55.
- **B1F water** — Psyduck 40–50, Slowpoke 40–50, Golduck 50–60, Slowbro 50–60, Golduck 55–65.
- **1F fishing** — slot 9 changed to **Slowpoke Lv25–35** so both Psyduck (slot 8) and Slowpoke are catchable via Old/Good/Super Rod.
- **B1F fishing** — slot 8 unified to **Psyduck Lv15–25**; slot 9 kept as Gyarados Lv25–35 (Slowpoke still available via B1F water).
- 2F tables were already identical between FR and LG; no changes needed there.

Rationale: single-cartridge parity — no reason to force a player to pick between the two Kanto starters for these caves when both are lore-consistent Cerulean Cave inhabitants.

### Species additions & swaps

Each of these adjusts existing Emerald wild tables to expose missing families without dropping any species from the affected maps. All changes are in [src/data/wild_encounters.json](src/data/wild_encounters.json) unless noted.

#### Route 117 — Budew & Roselia
Land slots 6 and 7 (both previously duplicate Illumise Lv13, 5% each) were overwritten in Porymap:
- Slot 6 → **Budew Lv13** (5%)
- Slot 7 → **Roselia Lv13** (5%)

Illumise remains in slot 8 (Lv14, 4%), so no species is removed from the route. Adds the Gen 4 Roselia pre-evolution and the Roselia line to natural gameplay early in Hoenn.

#### Route 114 — Zangoose
Slot originally holding a duplicate Seviper (Lv17) → **Zangoose (Lv17)**.

Rationale: Zangoose is the Ruby version-exclusive counterpart to Seviper. Adds Zangoose obtainability without removing Seviper from the map.

#### Surskit — restored on R/S native routes
Surskit appears on Routes 102, 111, 114, 117, and 120 in vanilla R/S but is unobtainable in vanilla Emerald. Added it back on all five routes in both grass and (except R111) surf, without dropping any species from the affected tables.

Slot-count constraint: Emerald wild tables are fixed at 12 land / 5 water slots, so each addition overwrote an existing slot whose species was still preserved in the same table (or in the case of surf Goldeen, still catchable on the same route via fishing).

Grass:
- **Route 102** — land slot 10 (was duplicate Zigzagoon Lv4) → **Surskit Lv4** at 1%.
- **Route 111** — land slot 10 (was duplicate Cacnea Lv22) → **Surskit Lv22** at 1%.
- **Route 114** — land slot 10 (was Seviper Lv17; Seviper still in slot 9 Lv15) → **Surskit Lv17** at 1%.
- **Route 117** — land slot 9 (was one of four Illumise slots) → **Surskit Lv14** at 4%. Both 1% slots on R117 hold unique species (Volbeat, Seedot) that would be lost otherwise, so we used the nearest duplicate slot instead.
- **Route 120** — land slot 9 (was one of two Absol slots; Absol still in slot 8) → **Surskit Lv27** at 4%. Both 1% slots on R120 hold unique species (Kecleon, Seedot); same rationale as R117.

Surf:
- **Routes 102, 114, 117, 120** — water slot 4 (was Goldeen Lv20–30) → **Surskit Lv20–30** at 1%. Goldeen remains catchable on each route via the fishing table.
- **Route 111** — no surf addition (Surskit doesn't appear in R/S surf on 111).

Rationale: Surskit → Masquerain is a Gen 3 native line whose native encounter routes were dropped from Emerald. This restores authentic R/S availability while preserving Emerald's own additions to each route.

#### Mt. Pyre & Victory Road — Meditite / Medicham
The Meditite line was previously only reachable via the Emerald-exclusive tables in a way that's easy to miss. Added encounters on their canonical R/S-style locations without dropping any existing species.

- **Mt. Pyre 1F–6F land** — slot 2 (10%) on each floor (was duplicate Shuppet) → **Meditite Lv38**. Shuppet remains in the other 11 slots of every floor. Exterior and Summit unchanged.
- **Victory Road B1F land**:
  - Slot 3 (was Lairon Lv40, duplicate of slot 2) → **Medicham Lv40** (10%). Lairon still in slots 2, 8, 10.
  - Slot 6 (was Golbat Lv42, duplicate of slots 0/4) → **Meditite Lv38** (5%). Golbat still in slots 0, 4.
- **Victory Road B2F land** — 15% Medicham split across two duplicate-slot overwrites since Emerald has no 15% slot rate:
  - Slot 3 (was Lairon Lv40, duplicate of slot 2) → **Medicham Lv40** (10%). Lairon still in slots 2, 8, 10.
  - Slot 7 (was Sableye Lv44, duplicate of slots 1/5) → **Medicham Lv44** (5%). Sableye still in slots 1, 5.
  - Combined 15% of encounters land as Medicham Lv40–44.

#### Mt. Pyre Summit — Chingling
`MAP_MT_PYRE_SUMMIT` land slot originally holding a duplicate Chimecho (Lv28) → **Chingling (Lv24)**.

Rationale: Chingling is the Gen 4 baby pre-evolution of Chimecho. Placing it alongside the existing Chimecho slot lets players catch the pre-evo in its natural habitat while preserving Chimecho as an encounter.

#### Meteor Falls (B1F 2R) — Lunatone
Slots originally holding duplicate Solrock → **Lunatone**.
- Land slot Lv37 Solrock → Lunatone
- Water slot Lv5–15 Solrock → Lunatone

Rationale: Lunatone is the Sapphire version-exclusive counterpart to Solrock. Preserves the fossil-moon pair in the same map region as vanilla Sapphire.

#### Desert Underpass — Kanto fossils
Replaced 3 of the 6 redundant Ditto slots with fossil Pokémon at their existing levels.
- Slot 0: Ditto Lv38 → **Omanyte Lv38**
- Slot 2: Ditto Lv40 → **Kabuto Lv40**
- Slot 11: Ditto Lv45 → **Aerodactyl Lv45**

Rationale: Desert Underpass fits fossil lore (ancient/buried) and the level band (35–45) matches Aerodactyl's late-game vibe. Ditto is still available at 4 remaining slots. Omastar/Kabutops obtainable via evolution.

#### Altering Cave — all 9 species accessible
Altering Cave normally has 9 alternate encounter tables (`gAlteringCave1`–`gAlteringCave9`) selected by a Mystery Gift event variable; without the event, only table 1 (Zubat only, 12 slots) is ever loaded, making the other 8 species unobtainable.

Rewrote `gAlteringCave1` so its 12 land slots cover every intended species:
- Slots 1–4: Zubat at Lv **10 / 12 / 14 / 16** (unchanged main occupant, 60% combined encounter rate)
- Slot 5: Mareep Lv **13**
- Slot 6: Pineco Lv **29**
- Slot 7: Houndour Lv **22**
- Slot 8: Teddiursa Lv **16**
- Slot 9: Aipom Lv **28**
- Slot 10: Shuckle Lv **28**
- Slot 11: Stantler Lv **28**
- Slot 12: Smeargle Lv **28**

Levels for the added species use the highest level listed in their original table. Tables 2–9 are left in place (dead-code fallback if the Mystery Gift path is ever wired up).

---

## Species Data

### Aipom — always holds Berry Juice
- File: [src/data/pokemon/species_info/gen_2_families.h](src/data/pokemon/species_info/gen_2_families.h)
- Change: Added `.itemCommon = ITEM_BERRY_JUICE` and `.itemRare = ITEM_BERRY_JUICE` to `SPECIES_AIPOM`.
- Rationale: matches Shuckle (which already always holds Berry Juice) so both species — main sources of Berry Juice in the Gen 2 tradition — carry it 100% of the time on any wild catch. Ensures Berry Juice is reliably obtainable without depending on RNG. Matches intent from development for Altering Cave.

---

## Obtainability Status (Gen 1–3 non-legendaries)

A Safari Zone expansion pass is planned separately; encounter placements below intentionally exclude the Safari Zone for now.

### Families now obtainable via this mod

- Zangoose (Route 114)
- Lunatone (Meteor Falls B1F 2R)
- Chingling → Chimecho (Mt. Pyre Summit; Chimecho already present)
- Omanyte → Omastar (Desert Underpass)
- Kabuto → Kabutops (Desert Underpass)
- Aerodactyl (Desert Underpass)
- Porygon (Devon Corp 1F vendor — ¥1000, sent straight to PC)

### Families still requiring placement

Gen 1 (37 families):
Bulbasaur line · Charmander line · Squirtle line · Caterpie line · Weedle line · Pidgey line · Rattata line · Spearow line · Ekans line · Nidoran♀ line · Nidoran♂ line · Paras line · Venonat line · Diglett line · Meowth line · Mankey line · Growlithe line · Poliwag line · Bellsprout line · Ponyta line · Slowpoke line · Seel line · Shellder line · Gastly line · Onix (→ Steelix) · Drowzee line · Krabby line · Exeggcute line · Cubone line · Chansey (→ Blissey) · Tangela · Kangaskhan · Scyther (→ Scizor) · Tauros · Lapras · Eevee + all eeveelutions · Porygon2 (Porygon obtainable; needs evo path or wild)

Gen 2 (16 families):
Cleffa/Clefairy line · Elekid/Electabuzz line · Magby/Magmar line · Hoppip line · Yanma · Murkrow · Misdreavus · Unown · Dunsparce · Qwilfish · Sneasel · Swinub line · Delibird · Mantine · Larvitar line · Sentret line

Gen 3 (5 — all event/gift in vanilla Emerald; verify still functional in this expansion):
Lileep line (Root Fossil revival) · Anorith line (Claw Fossil revival) · Feebas line (Route 119 tile) · Castform (Weather Institute gift) · Beldum line (Steven's house gift)

Species reachable via evolution from already-catchable Emerald mons (no placement needed): Kadabra/Alakazam (from Abra), Magcargo (from Slugma), and any other line whose base form is already in an Emerald-accessible map.
