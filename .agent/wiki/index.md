# Wiki Index

This directory is the compiled knowledge layer for the native port effort.

## Read First
- `log.md`
- `build-oracle.md`
- `runtime-architecture.md`
- `verification.md`
- `decisions.md`

## Pages
- `repo-map.md`: top-level structure and ownership map
- `build-oracle.md`: exact ROM build commands, tools, outputs, and oracle notes
- `asset-pipeline.md`: graphics and compression pipeline
- `runtime-architecture.md`: current understanding of runtime structure
- `data-model.md`: native data-model direction and provenance strategy
- `overworld.md`: overworld-specific notes
- `battle.md`: battle-specific notes
- `scripting.md`: scripts, events, and map object wiring
- `verification.md`: determinism and oracle strategy
- `decisions.md`: explicit architectural and workflow decisions
- `log.md`: chronological execution log

## Current Focus
- Preserve the ROM build as oracle.
- Keep the running native slice stable while expanding the playable PalletTown hub into adjacent interiors and later outdoor/script seams.
- Expand provenance so native assets, tests, and later runtime debug hooks can point back to original labels, files, and symbols.
- Preserve a concrete WSL -> MinGW path to `pokered_native.exe` even while Linux remains the first-class host build.

## Latest Findings
- Boot path: `_Start -> Init -> PlayIntro -> PrepareTitleScreen -> DisplayTitleScreen -> MainMenu`.
- New-game path: `MainMenu -> StartNewGame -> OakSpeech -> SpecialEnterMap -> EnterMap`.
- Overworld loop anchor: `EnterMap`, `OverworldLoop`, and `NewBattle` in `home/overworld.asm`.
- Save/load anchor: `TryLoadSaveFile`, `SaveMenu`, `SaveGameData`, and SRAM checksum logic in `engine/menus/save.asm`.
- Native runtime now builds and runs a title -> menu -> connected `RedsHouse1F` / `RedsHouse2F` / `PalletTown` / `BluesHouse` / `OaksLab` slice with deterministic save/load.
- Current native verification passes: configure, build, unit tests, and `--smoke-test` (`smoke-ok: world=5 pos=2,2 steps=9`).
- `RedsHouse1F` now consumes the real `maps/RedsHouse1F.blk`, `gfx/blocksets/reds_house.bst`, and tileset collision metadata through the native map importer.
- The Mom and TV room text now pages through the exact first-slice lines instead of truncating them to fit a single text box.
- `RedsHouse1F` warps, bg events, NPC placement, and first-slice Mom/TV text now come from a generated metadata header sourced from `data/maps/objects/RedsHouse1F.asm` and `text/RedsHouse1F.asm`.
- `native/src/oracle/map_file.cpp` now parses `pokered.map`, and native tests join `.sym` symbol addresses to `.map` sections for `EnterMap`, `OverworldLoop`, `PalletTown_h`, `BluesHouse_h`, `RedsHouse1F_h`, `RedsHouse2F_h`, `PewterSpeechHouse_h`, and `TryLoadSaveFile`.
- The native slice now imports `RedsHouse2F` from `maps/RedsHouse2F.blk` and `data/maps/objects/RedsHouse2F.asm`, using the Linux/WSL SDL2 CMake path as the first-class host build.
- The importer now also handles `PewterSpeechHouse` from `maps/PewterSpeechHouse.blk`, `data/maps/objects/PewterSpeechHouse.asm`, `text/PewterSpeechHouse.asm`, and the shared `gfx/blocksets/house.bst`.
- NPC interactions are no longer hardwired to Mom-only logic; `Npc` records now carry source-driven message IDs for static-text interiors.
- The native runtime now imports `PalletTown` from `maps/PalletTown.blk`, `gfx/blocksets/overworld.bst`, `data/maps/objects/PalletTown.asm`, `text/PalletTown.asm`, and `Overworld_Coll`.
- `LAST_MAP` door warps now resolve when the target exterior map exists natively, which makes the `RedsHouse1F` front door round-trip into `PalletTown` work for real.
- The SDL renderer now uses a camera-based viewport so larger native maps remain playable at the Linux/WSL window size.
- `BluesHouse` now imports from `maps/BluesHouse.blk`, `data/maps/objects/BluesHouse.asm`, `text/BluesHouse.asm`, `gfx/blocksets/house.bst`, and `House_Coll`, which makes the second PalletTown door live.
- `OaksLab` now imports from `maps/OaksLab.blk`, `data/maps/objects/OaksLab.asm`, `text/OaksLab.asm`, `gfx/blocksets/gym.bst` via the original `DOJO` alias, and `Dojo_Coll`, which makes the third PalletTown door live.
- `PalletTown` now has a bounded north-exit Oak warning seam before `got_starter`: directional movement into row `y == 1` surfaces the short source-backed “Hey! Wait! Don’t go out!” line instead of behaving like a normal step.
- The SDL app now has a runtime-facing provenance toggle on `F7` that cycles the help box between normal controls, the current map's source header/object labels, the current map-script state, the current `LAST_MAP` return anchor, the current facing source-backed interaction target, the current facing source-backed text label, the current facing branch/handler preview, the current facing gate source/backing state, the last successful warp's source/target object provenance, the last move attempt, the last confirm-based interaction, the last interaction branch/handler label, the last evaluated native state gate, the exact source gate backing that native predicate, the last displayed source-backed text label, and that message's source/local script label loaded from `pokered.sym` and `pokered.map`.
- The new last-map-state page surfaces the current return anchor held in native state as a source-backed object label such as `PalletTown_Object #1` or `RedsHouse1F_Object #2`, which complements the existing last-successful-warp trace.
- The new current map-script page previews the live map's script root and, where available, the script-state WRAM label such as `PalletTown_Script` + `wPalletTownCurScript`, `OaksLab_Script` + `wOaksLabCurScript`, or the script-only `RedsHouse1F_Script`.
- The new live facing-target page surfaces the current NPC/bg-event the player is pointing at, including branched current sources such as `RedsHouse1FTVText.WrongSideText` and `OaksLabRivalText.MyPokemonLooksStrongerText`.
- The new live facing text page previews the exact source-backed text label that would surface for the current facing target, including `_PalletTownGirlText`, `_RedsHouse1FTVWrongSideText`, and `_OaksLabRivalMyPokemonLooksStrongerText`.
- The new live facing-branch page previews the current handler that would run if confirm were pressed now, including gates and handler labels such as `RedsHouse1FMomText.heal`, `RedsHouse1FTVText`, and `OaksLabRivalText.afterChooseMon`.
- The new live facing gate-source page resolves the current facing predicate back to the source gate and backing state it depends on, including `BIT_GOT_STARTER` / `wStatusFlags4` and `SPRITE_FACING_UP` / `wSpritePlayerStateData1FacingDirection`.
- The new gate-source page resolves current simplified predicates back to the original state they stand in for, including `EVENT_FOLLOWED_OAK_INTO_LAB`, `wStatusFlags4` / `BIT_GOT_STARTER`, and `wSpritePlayerStateData1FacingDirection` / `SPRITE_FACING_UP`.
- The new last-move page records move coordinates plus blocker type, and when the move was stopped by a known scripted seam it can point back to the triggering asm handler such as `PalletTownDefaultScript`.
- The new last-interaction page records NPC/bg-event versus miss context plus target coordinates, and for source-backed interactions it can join the map object label to the local source label such as `PalletTownGirlText` or `OaksLabPokedexText`.
- The new last-interaction-branch page preserves each interaction’s origin `MessageId` before native branching so conditional handlers can point back to asm-local branch labels such as `RedsHouse1FMomText.heal` and `OaksLabRivalText.afterChooseMon`.
- The new last-state-gate page records the latest native branch predicate and value, which makes gates like `GOT_STARTER=0/1` and `FACING_UP=0/1` visible next to source-backed handlers such as `PalletTownDefaultScript`, `RedsHouse1FMomText`, and `RedsHouse1FTVText`.
- Exiting `RedsHouse1F`, `BluesHouse`, or `OaksLab` into `PalletTown` now auto-steps the player one tile below the exterior door, which matches the original outside-door flow more closely and removes the extra manual step from the native smoke path.
- Warped `MoveResult` traces now preserve the final landing coordinates instead of the pre-warp doorway tile, which makes PalletTown door round-trips report the real connected arrival tile in tests and the runtime move trace.
- Standing on the current interior PalletTown doorway tiles and pressing into the blocked exit direction now re-exits correctly, which restores the original collision-on-warp door feel for `RedsHouse1F`, `BluesHouse`, and `OaksLab` without needing a sideways step first.
- Blocked collision re-warps are now limited to door tiles, which keeps the restored PalletTown doorway behavior intact without letting unrelated stair warps retrigger on blocked moves.
- PalletTown door coverage now explicitly includes the `BluesHouse` second doorway tile, so both interior exit tiles are verified against the same outdoor return anchor.
- Immediate blocked re-exit coverage now spans every live interior doorway tile across `RedsHouse1F`, `BluesHouse`, and `OaksLab`, which closes the remaining doorway-symmetry gap in the PalletTown-connected interiors.
- Passable lateral step-offs now stay local on every live interior doorway tile in coverage too, which complements the blocked re-exit checks and verifies that door warps do not over-trigger on sideways movement in `RedsHouse1F`, `BluesHouse`, and `OaksLab`.
- PalletTown-side re-entry coverage now explicitly includes `BluesHouse`, so every live PalletTown door pair is exercised from the outdoor auto-step landing tile as well as the interior doorway tiles.
- Passable lateral step-offs now stay local in both directions on the outdoor auto-step landing tile for each live PalletTown door pair, which verifies that exterior door exits settle onto ordinary PalletTown movement cells instead of creating a sideways re-warp seam.
- Forward movement from the PalletTown outdoor landing tiles is now explicit in coverage too: the `RedsHouse1F` and `BluesHouse` landings step normally into town, while the `OaksLab` landing's front edge stays collision-blocked without retriggering a warp.
- The smoke path now also snapshot-checks the passable forward steps below `RedsHouse1F` and `BluesHouse`, which keeps the main `smoke-ok: world=5 pos=2,2 steps=9` route stable while covering the remaining house frontage cases.
- The smoke path now also asserts the `OaksLab` outdoor landing exception directly: after the immediate lab re-exit into `PalletTown`, pressing forward from the landing tile stays collision-blocked and local before the path re-enters the lab.
- The smoke path now also snapshot-checks passable left/right steps from the outdoor landing tiles below `RedsHouse1F`, `BluesHouse`, and `OaksLab`, which closes the remaining runtime-facing sideways-movement gap without perturbing the established route.
- The current smoke path now verifies all three live PalletTown door pairs (`RedsHouse1F`, `BluesHouse`, and `OaksLab`), including a PalletTown-side re-entry check for each plus outdoor landing forward/lateral behavior, alongside the north-exit Oak seam, outdoor/interior interaction readiness, and save/load, producing `smoke-ok: world=5 pos=2,2 steps=9`.
- `Route1` and `Route21` are still missing, so the current PalletTown slice is broader but still a bounded playable hub.
- `BluesHouse` Daisy currently uses the source-backed default Rival-at-lab text branch; the Town Map gift/event path is still intentionally deferred.
- `OaksLab` currently uses source-backed safe default interaction branches keyed off `got_starter`; starter selection, rival battle, and Oak's wider lab sequence remain deferred.
- Oak's full follow-to-lab cutscene is still deferred; the current seam stops at the warning text and does not animate Oak or escort the player.
- Runtime-facing provenance is still intentionally narrow: the overlay now covers the current map, the current map-script state, the current `LAST_MAP` return anchor, the current facing source-backed object/interaction target, the current facing source-backed text label, the current facing branch/handler preview, the current facing gate source/backing state, the last successful warp, the last move attempt, the last confirm-based interaction, the last interaction branch/handler state, the last evaluated native state gate, the exact source gate backing that state, the last displayed source-backed message, and that message’s source/local script label, but not broader object-state or script-state history.
- Windows cross-build support now has a repo-local MinGW toolchain file plus a documented package/config path; local verification is still blocked by missing MinGW + SDL2 packages.
