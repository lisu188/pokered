# Runtime Architecture

## Current High-Level Read
- The disassembly is organized around a small set of top-level assembly entry files and a large include-driven engine/data tree.
- Build variants are controlled by compile-time symbols (`_RED`, `_BLUE`, `_DEBUG`, VC flags).
- Runtime concerns appear separable into:
  - boot / movie / title / intro
  - core engine state and dispatch
  - overworld and map scripts
  - text and menu/UI
  - battle
  - audio
  - save/RAM

## Native-Port Direction
- Build a native runtime rather than transliterating assembly into C++.
- Proposed native modules:
  - `platform`
  - `video`
  - `audio`
  - `input`
  - `assets`
  - `save`
  - `core`
  - `world`
  - `text`
  - `script`
  - `battle`
  - `tooling`

## Current Native Slice
- The repo now has a working top-level native CMake build in `CMakeLists.txt`.
- Current native runtime files:
  - `native/src/main.cpp`
  - `native/src/app/application.cpp`
  - `native/src/core/game_state.cpp`
  - `native/src/oracle/map_file.cpp`
  - `native/src/world/map_data.cpp`
  - `native/src/save/save_system.cpp`
  - `native/src/ui/bitmap_font.cpp`
- Current native headers:
  - `native/include/pokered/app/application.hpp`
  - `native/include/pokered/core/game_state.hpp`
  - `native/include/pokered/world/map_data.hpp`
  - `native/include/pokered/save/save_system.hpp`
  - `native/include/pokered/ui/bitmap_font.hpp`
- Current generated-data bridge:
  - `cmake/GenerateBinaryHeader.cmake`
  - `cmake/GenerateBluesHouseMetadata.cmake`
  - `cmake/GenerateOaksLabMetadata.cmake`
  - `cmake/GeneratePewterSpeechHouseMetadata.cmake`
  - `cmake/GenerateRedsHouse1FMetadata.cmake`
  - `cmake/GenerateRedsHouse2FMetadata.cmake`
  - `cmake/GeneratePalletTownMetadata.cmake`
  - `${build}/generated/blues_house_blk.hpp`
  - `${build}/generated/oaks_lab_blk.hpp`
  - `${build}/generated/pewter_speech_house_blk.hpp`
  - `${build}/generated/reds_house_1f_blk.hpp`
  - `${build}/generated/reds_house_2f_blk.hpp`
  - `${build}/generated/pallet_town_blk.hpp`
  - `${build}/generated/dojo_blockset.hpp`
  - `${build}/generated/house_blockset.hpp`
  - `${build}/generated/reds_house_blockset.hpp`
  - `${build}/generated/blues_house_metadata.hpp`
  - `${build}/generated/oaks_lab_metadata.hpp`
  - `${build}/generated/pewter_speech_house_metadata.hpp`
  - `${build}/generated/reds_house_1f_metadata.hpp`
  - `${build}/generated/reds_house_2f_metadata.hpp`
  - `${build}/generated/overworld_blockset.hpp`
  - `${build}/generated/pallet_town_metadata.hpp`
- Current implemented behavior:
  - Linux/WSL host build through CMake + `pkg-config sdl2`
  - SDL window and render loop
  - title screen and minimal menu
  - deterministic new-game shortcut into `RedsHouse1F`
  - movement, collision, one-NPC and one-background interaction
  - stair traversal between `RedsHouse1F` and `RedsHouse2F`
  - deterministic binary save/load and smoke-test path
  - source-driven `RedsHouse1F` map semantics via `.blk`, `.bst`, and tileset collision metadata
  - source-driven `RedsHouse2F` map semantics via `.blk`, shared `reds_house.bst`, and asm warp metadata
  - source-driven `PewterSpeechHouse` semantics via `.blk`, `house.bst`, `House_Coll`, and asm/text metadata
  - source-driven `BluesHouse` semantics via `.blk`, `house.bst`, `House_Coll`, and asm/text metadata
  - source-driven `OaksLab` semantics via `.blk`, the original `DOJO -> gym.bst` blockset alias, `Dojo_Coll`, and asm/text metadata
  - source-driven `PalletTown` semantics via `.blk`, `overworld.bst`, `Overworld_Coll`, and asm/text metadata
  - NPC-specific static text dispatch through source-driven `MessageId` values on `Npc` records
  - movement-triggered message dispatch for bounded outdoor/script seams
  - paged Mom/TV dialogue so the exact first-slice lines fit the current message box
  - source-backed `RedsHouse1F` warps/bg events/NPC placement and Mom/TV text imported from asm
  - `.sym` + `.map` oracle parsing in native code for provenance-oriented tests
  - a runtime-facing `F7` overlay that cycles between the current map's header/object provenance, the current `LAST_MAP` return anchor, the current facing source-backed interaction target, the last successful warp's source/target object provenance, the last move attempt, the last confirm-based interaction, the last interaction branch/handler provenance, the last evaluated native state gate, the exact source gate backing that native predicate, the last displayed source-backed text provenance, and that message's source/local script-label provenance from `pokered.sym` and `pokered.map`
  - `LAST_MAP` door traversal for supported native maps
  - camera-based SDL world rendering for larger maps on the Linux/WSL window size

## Current Gaps
- Real `gfx/tilesets/reds_house.2bpp` graphics are not rendered yet; the first slice still uses semantic colors.
- Runtime-facing provenance now includes current-map, last-map-state, live-facing-target, last-warp, last-move, last-interaction, last-interaction-branch, last-state-gate, last-state-gate-source, last-message, and last-message-source trace pages, but broader object-state/script-state trace hooks are still missing.
- The current control flow is still a native simplification of the original map script/text dispatch rather than a reusable generated script runtime.
- `Route1` and `Route21` are still not imported, so PalletTown is currently a broader bounded hub rather than a full outdoor progression map.
- `BluesHouse` Daisy's Town Map gift path is still simplified to the default static-text branch; native item/event state does not exist yet.
- Oak's PalletTown follow-to-lab cutscene chain and the `OaksLab` starter / rival sequence are still intentionally deferred; only the first north-exit warning seam is live.
- Battle, broader overworld support, and audio are still intentionally deferred.

## Immediate Unknowns
- Minimum generated/imported data needed to reach walking and text
- The exact minimum map/header/script subset needed for a `RedsHouse1F` native slice
- How far title/menu flow should mimic Oak Speech before the native path shortcuts into `EnterMap`

## Confirmed Boot / Flow Path
- `_Start` in `home/start.asm` only distinguishes CGB vs non-CGB and jumps to `Init`.
- `Init` in `home/init.asm`:
  - clears WRAM/HRAM/VRAM
  - initializes DMA and LCD state
  - loads SGB support
  - plays the intro via `predef PlayIntro`
  - then jumps to `PrepareTitleScreen`
- `DisplayTitleScreen` in `engine/movie/title.asm`:
  - prepares title graphics and version branding
  - animates title mons until user interruption
  - can enter clear-save dialogue
- `MainMenu` in `engine/menus/main_menu.asm`:
  - checks SRAM for a save file
  - can continue, start a new game, or open options
- `StartNewGame -> OakSpeech -> SpecialEnterMap -> EnterMap`
- Continue path also eventually lands in `SpecialEnterMap -> EnterMap`

## Confirmed Playable Seams
- `EnterMap`, `LoadMapData`, and `OverworldLoop` in `home/overworld.asm` form the cleanest early playable slice seam.
- `MainMenu`, `DisplayTitleScreen`, and `OakSpeech` are clean upstream entrypoints for boot/title/new-game flow.
- `ExecuteCurMapScriptInTable` in `home/trainers.asm` is the obvious anchor for map-script dispatch.

## First Slice Boundary
- Title screen
- Minimal main menu
- Deterministic new-game shortcut
- Native `RedsHouse1F` load path
- Player movement and collision
- One text interaction through a simplified `DisplayTextID`-style path
- Battle remains stubbed behind the `NewBattle` seam

## Current Two-Room Slice
- `RedsHouse1F` and `RedsHouse2F` now form a connected indoor slice.
- Non-`LAST_MAP` stair warps transition between the two maps and preserve deterministic save/load.
- The current smoke run starts in `RedsHouse1F`, traverses the stairs to `RedsHouse2F`, then exercises save/load from the upstairs room.

## Current Three-Room Coverage
- `PewterSpeechHouse` now loads through the same native data path as the `RedsHouse*` rooms, but uses the `HOUSE` blockset/collision set instead of `reds_house`.
- Static NPC text for `PewterSpeechHouse` comes from source-parsed text labels rather than handwritten C++ message tables.
- The room is currently validation-only because its `LAST_MAP` exits still expect an unimplemented exterior (`Pewter City`) rather than the now-live `PalletTown` seam.

## Current Four-Room Coverage
- `BluesHouse` now loads through the same native `HOUSE` importer path as `PewterSpeechHouse`, but connects to the live `PalletTown` seam through `LAST_MAP` door warps.
- The seated Daisy interaction currently resolves to the default pre-Pokedex branch (`Rival is out at Grandpa's lab`) rather than the later Town Map gift flow.
- The walking Daisy and Town Map object both use source-backed text pointers imported from `scripts/BluesHouse.asm` and `text/BluesHouse.asm`.

## Current Five-Room Coverage
- `OaksLab` now loads through a source-driven `DOJO` importer path using the same blockset alias as the ROM (`Dojo_Block:: Gym_Block:: INCBIN "gfx/blocksets/gym.bst"`).
- The PalletTown lab door is live in both directions through `LAST_MAP` door warps.
- Rival, Oak, Pokeball, Pokedex, Girl, and Scientist interactions now use source-backed text, while only the safe default branches keyed off `world.got_starter` are live.

## Current PalletTown Slice
- `PalletTown` is now playable as a native exterior map with `20 x 18` movement cells expanded from the original `10 x 9` block map.
- `RedsHouse1F` front-door warps now exit into `PalletTown`, and the PalletTown house door re-enters `RedsHouse1F`.
- The `BluesHouse` and `OaksLab` doors are now live in both directions.
- A bounded Oak warning seam now intercepts north-exit movement before `got_starter` and surfaces the short source-backed “Hey! Wait! Don’t go out!” text.
- Girl, Fisher, Oak, and the PalletTown signs use source-backed message text without pulling in the map-script engine.
- Pressing `F7` now cycles the world help box between controls, current-map provenance lines, current `LAST_MAP` state lines, live facing-target lines, last-warp provenance lines, last-move trace lines, last-interaction trace lines, last-interaction-branch trace lines, last-state-gate trace lines, last-state-gate-source lines, the most recent displayed source-backed text label, and the source/local script label that emitted it.
- The current `LAST_MAP` page reports the live return anchor held in `GameState::world.last_map` / `last_warp` and can resolve it back to the owning map object symbol such as `PalletTown_Object #1` or `RedsHouse1F_Object #2`.
- The current facing-target page reports the live NPC/bg-event the player is pointing at from the current `WorldState`, and can resolve branched current results such as `RedsHouse1FTVText.WrongSideText` or `OaksLabRivalText.MyPokemonLooksStrongerText` without requiring a confirm press first.
- The current gate-source page complements the native `GOT_STARTER=0/1` and `FACING_UP=0/1` values by resolving the underlying source gate they approximate, including `EVENT_FOLLOWED_OAK_INTO_LAB`, `wStatusFlags4`, and `wSpritePlayerStateData1FacingDirection`.
- The last-move page preserves blocker-aware `MoveResult` details for steps, walls, NPC collisions, warps, and scripted seams, and can resolve the current PalletTown north-exit seam back to `PalletTownDefaultScript`.
- The last-interaction page preserves NPC/bg-event versus miss context plus target coordinates, and for source-backed interactions it can join the current map object label to the local text source label through shared oracle helpers.
- The last-interaction-branch page preserves the pre-branch interaction `origin_message`, which lets the overlay resolve conditional handlers back to asm-local labels such as `RedsHouse1FMomText.heal`, `OaksLabRivalText.afterChooseMon`, and `OaksLabOak1Text.check_for_poke_balls`.
- The last-state-gate page preserves the evaluated native predicate behind the latest scripted move/interaction branch, which makes current simplifications like `GOT_STARTER=0/1` and `FACING_UP=0/1` visible beside the corresponding handler label.
- The SDL renderer now scrolls a centered camera viewport instead of trying to draw the full world unscaled.

## Current Data Model Notes
- The importer now supports arbitrary compile-time block dimensions, not just `4 x 4` interiors.
- `RedsHouse1F`, `RedsHouse2F`, and `PewterSpeechHouse` each use `8 x 8` movement-cell space expanded from `4 x 4` block maps.
- `OaksLab` uses `10 x 12` movement-cell space expanded from its original `5 x 6` block map.
- `PalletTown` uses `20 x 18` movement-cell space expanded from its original `10 x 9` block map.
- Each movement cell is derived from the lower-left representative tile of the relevant `2 x 2` quadrant inside a `16`-byte `.bst` metatile block, matching the original collision sampling convention.
- Passability for the `RedsHouse*` rooms comes from the shared `RedsHouse1_Coll` / `RedsHouse2_Coll` entries in `data/tilesets/collision_tile_ids.asm`.
- `HOUSE` tileset passability now comes from `House_Coll` in `data/tilesets/collision_tile_ids.asm`.
- `OaksLab` passability now comes from `Dojo_Coll` in `data/tilesets/collision_tile_ids.asm`, matching the original `DOJO` tileset alias.
- `PalletTown` passability now comes from `Overworld_Coll` in `data/tilesets/collision_tile_ids.asm`.
- TV and warp presentation remain layered from map-event data rather than a special-case room mask.
