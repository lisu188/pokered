# Execution Log

## [2026-04-09 10:03] bootstrap | durable memory bootstrap started
Created the project-local durable memory structure: `AGENTS.md`, `.codex/config.toml`, `.codex/agents/`, `.agent/wiki/`, `.agent/state.json`, `.agent/PLANS.md`, and `.agent/EXECPLAN.md`.

## [2026-04-09 10:03] oracle | ROM build validated under WSL
Confirmed the original disassembly builds successfully under Ubuntu 24.04 WSL with `rgbds v1.0.1`. `make -j$(nproc)` produced `pokered.gbc`, `pokeblue.gbc`, and `pokeblue_debug.gbc` with SHA1 hashes matching `README.md`.

## [2026-04-09 10:04] environment | native host tools detected
Confirmed local availability of `cmake`, `g++`, `pkg-config`, and `sdl2` (`2.30.0`). MinGW cross-tools are not installed yet and are tracked as a non-blocking gap.

## [2026-04-09 10:04] decision | Linux-first native runtime with generated data
Chose a Linux-first native runtime in C++20 + SDL2 + CMake, with generated/imported data artifacts derived from the disassembly and helper tools rather than hand-transcribed gameplay tables or binaries.

## [2026-04-09 10:08] exploration | runtime entrypoints and save flow mapped
Mapped the current native-oracle runtime seams: `_Start` in `home/start.asm`, `Init` in `home/init.asm`, `PrepareTitleScreen` and `DisplayTitleScreen` in `engine/movie/title.asm`, `MainMenu` and `StartNewGame` in `engine/menus/main_menu.asm`, `OakSpeech` in `engine/movie/oak_speech/oak_speech.asm`, `EnterMap` / `OverworldLoop` / `NewBattle` in `home/overworld.asm`, and `TryLoadSaveFile` / `SaveGameData` in `engine/menus/save.asm`.

## [2026-04-09 10:08] exploration | asset pipeline boundaries confirmed
Confirmed that the original asset pipeline is cleanly split between RGBDS tools and small native helper tools. `rgbgfx` produces `1bpp`/`2bpp`; `tools/gfx` trims, deduplicates, flips, and interleaves tiles; `tools/pkmncompress` handles square `2bpp` picture compression; `tools/scan_includes` discovers include dependencies for build-time graph expansion.

## [2026-04-09 10:08] exploration | parallel explorer pass launched
Started bounded read-only explorer agents for repo structure, gameplay flow, asset pipeline, and verification strategy. Their findings are being merged into the wiki as the main thread prepares the native runtime skeleton.

## [2026-04-09 10:10] decision | first playable slice narrowed to RedsHouse1F
Locked the first playable slice to title -> minimal menu -> deterministic new-game shortcut -> `RedsHouse1F` map load -> movement/collision -> one text interaction. Battle remains intentionally stubbed for the first pass.

## [2026-04-09 10:10] verification | deterministic save/load moved into the first slice
Raised deterministic binary save/load, checksum-oriented tests, and early `.sym`/`.map` provenance into the first native milestone so verification grows with the runtime instead of being deferred.

## [2026-04-09 10:39] milestone | native first slice runs locally
The native top-level CMake + SDL2 build now configures, builds, and runs. The current slice reaches title -> menu -> deterministic new game -> `RedsHouse1F`, with movement, collision, interactions, and deterministic save/load through `native/src/app/application.cpp`, `native/src/world/map_data.cpp`, and `native/src/save/save_system.cpp`.

## [2026-04-09 10:39] verification | native tests and smoke checks passed
Verified the native slice with `cmake -S . -B build-native`, `cmake --build build-native -j"$(nproc)"`, `ctest --test-dir build-native --output-on-failure`, and `./build-native/pokered_native --smoke-test`. Current smoke output is `smoke-ok: world=0 pos=3,2 steps=8`.

## [2026-04-09 10:39] architecture | next fidelity step narrowed to source-driven room semantics
The running slice already consumes the real `maps/RedsHouse1F.blk` data through a generated header. The next immediate work item is to replace the prototype semantic room decode with source-driven expansion from `gfx/blocksets/reds_house.bst` and tileset collision/warp metadata.

## [2026-04-09 10:50] verification | source-driven RedsHouse1F import confirmed green
Re-verified that the current native slice already imports `RedsHouse1F` from real `.blk` and `.bst` sources with tileset collision metadata. `cmake -S . -B build-native`, `cmake --build build-native -j"$(nproc)"`, `ctest --test-dir build-native --output-on-failure`, and `./build-native/pokered_native --smoke-test` all still pass.

## [2026-04-09 10:50] save | native save writes switched to temp-file replacement
Updated `native/src/save/save_system.cpp` so the normal `.sav` path is written through a sibling temp file and only replaced after a successful write. This keeps the deterministic save format while avoiding truncate-in-place behavior.

## [2026-04-09 10:50] build | MinGW path documented and CMake hardened for SDL2main
Documented the WSL -> MinGW cross-build path in the wiki and updated the top-level CMake logic to link `SDL2::SDL2main` when a MinGW SDL2 package exposes it. Local end-to-end verification remains blocked by missing MinGW compiler and SDL2 packages.

## [2026-04-09 10:50] implementation | paged first-slice room text added
Added lightweight paging for the Mom and TV room messages so the exact `RedsHouse1F` lines fit the current native text box without clipping. Verification stayed green after a clean rebuild and test run.

## [2026-04-09 10:51] milestone | RedsHouse1F object and text metadata imported from asm
Added a targeted build-time metadata generator for the first slice. `cmake/GenerateRedsHouse1FMetadata.cmake` now parses `data/maps/objects/RedsHouse1F.asm` and `text/RedsHouse1F.asm`, and `native/src/world/map_data.cpp` consumes the generated warps, bg events, NPC placement, and paged Mom/TV text instead of mirroring them in hand-written C++ arrays.

## [2026-04-09 10:51] provenance | existing symbol-file reader confirmed in tests
Confirmed that provenance work had already started in `native/src/oracle/symbol_file.cpp`. The native test binary loads `pokered.sym` and checks oracle anchors such as `EnterMap`, `OverworldLoop`, and `TryLoadSaveFile`; the next provenance gap is `.map` support and runtime integration, not basic symbol parsing.

## [2026-04-09 11:48] provenance | .map sections joined to native oracle tests
Added `native/src/oracle/map_file.cpp` and wired it into the native test binary. The Linux/WSL CMake path now verifies `.sym` symbol addresses against `.map` section ownership for `EnterMap`, `OverworldLoop`, `RedsHouse1F_h`, `RedsHouse2F_h`, and `TryLoadSaveFile`.

## [2026-04-09 11:48] milestone | RedsHouse2F added to the runnable indoor slice
Extended the native runtime beyond the single-room bootstrap. `maps/RedsHouse2F.blk` and `data/maps/objects/RedsHouse2F.asm` now feed a second generated indoor map, and `native/src/core/game_state.cpp` handles stair traversal between `RedsHouse1F` and `RedsHouse2F`.

## [2026-04-09 11:48] verification | two-room Linux SDL slice re-verified
Rebuilt and re-ran the native checks on the Linux SDL2 path under WSL with `cmake --build build-native -j"$(nproc)"`, `ctest --test-dir build-native --output-on-failure`, `./build-native/pokered_native_tests`, and `./build-native/pokered_native --smoke-test`. The current smoke output is `smoke-ok: world=1 pos=7,1 steps=1`.

## [2026-04-09 12:22] implementation | indoor importer generalized to a HOUSE tileset room
Added a second small-indoor importer path beyond the `RedsHouse*` pair. `PewterSpeechHouse` now imports from `maps/PewterSpeechHouse.blk`, `data/maps/objects/PewterSpeechHouse.asm`, `text/PewterSpeechHouse.asm`, and `gfx/blocksets/house.bst`, with `House_Coll` driving passability.

## [2026-04-09 12:22] implementation | NPC interactions now carry source-driven message ids
Extended `Npc` records to include a `MessageId`, which lets the runtime dispatch static indoor NPC text without a Mom-specific switch. `RedsHouse1F` still uses its conditional Mom text path, while `PewterSpeechHouse` uses direct source-backed NPC messages.

## [2026-04-09 12:22] verification | three-room indoor slice re-verified on Linux SDL
Rebuilt and re-ran the Linux/WSL native checks with `cmake --build build-native -j"$(nproc)"`, `ctest --test-dir build-native --output-on-failure`, `./build-native/pokered_native_tests`, and `./build-native/pokered_native --smoke-test`. The current smoke output remains `smoke-ok: world=1 pos=7,1 steps=1`.

## [2026-04-09 12:50] milestone | PalletTown exterior slice is now playable
Added a source-driven `PalletTown` native map using `maps/PalletTown.blk`, `gfx/blocksets/overworld.bst`, `Overworld_Coll`, `data/maps/objects/PalletTown.asm`, and `text/PalletTown.asm`. The current runtime can now leave `RedsHouse1F`, walk around PalletTown, read signs, talk to the Girl/Fisher/Oak, and save/load that outdoor state deterministically.

## [2026-04-09 12:50] architecture | larger maps now render through a scrolling camera
Updated `native/src/app/application.cpp` to use a camera-based SDL viewport instead of drawing the whole map at once. This keeps the Linux/WSL native window readable for `20 x 18` PalletTown movement-cell maps without waiting for a pixel-perfect renderer.

## [2026-04-09 12:50] implementation | LAST_MAP doors now resolve when the target map exists natively
`native/src/core/game_state.cpp` now resolves `LAST_MAP` exits through `world.last_map` when the referenced target has native map data, and records the current warp provenance on successful door exits. This makes the `RedsHouse1F` front-door round-trip into `PalletTown` work for real while leaving unsupported destinations inert.

## [2026-04-09 12:50] verification | PalletTown Linux SDL slice re-verified
Rebuilt and re-ran the Linux/WSL checks with `cmake --build build-native -j"$(nproc)"`, `./build-native/pokered_native_tests`, `ctest --test-dir build-native --output-on-failure`, and `./build-native/pokered_native --smoke-test`. The current smoke output is `smoke-ok: world=3 pos=4,8 steps=5`. A direct `timeout 2s ./build-native/pokered_native` launch also starts cleanly under WSL and exits only because of the timeout.

## [2026-04-09 13:29] milestone | BluesHouse added as the next live PalletTown interior
Added a source-driven `BluesHouse` native map using `maps/BluesHouse.blk`, `gfx/blocksets/house.bst`, `House_Coll`, `data/maps/objects/BluesHouse.asm`, and `text/BluesHouse.asm`. The PalletTown rivals-house door now enters `BluesHouse`, and the house can return to PalletTown through its `LAST_MAP` door warp.

## [2026-04-09 13:29] scripting | BluesHouse Daisy kept on the default dialogue branch
Imported the `BluesHouse` object/text pointers, but intentionally kept Daisy's seated interaction on the default pre-Pokedex `Rival is out at Grandpa's lab` text branch. The Town Map gift/event path remains deferred until native item/event state exists.

## [2026-04-09 13:29] verification | BluesHouse-expanded PalletTown hub re-verified
Rebuilt and re-ran the Linux/WSL checks with `cmake --build build-native -j"$(nproc)"`, `./build-native/pokered_native_tests`, `ctest --output-on-failure`, `./build-native/pokered_native --smoke-test`, and `timeout 2s ./build-native/pokered_native`. Current smoke output is `smoke-ok: world=4 pos=3,4 steps=6`, and the SDL launch still exits only because of the expected timeout.

## [2026-04-09 13:39] milestone | OaksLab added as the third live PalletTown interior
Added a source-driven `OaksLab` native map using `maps/OaksLab.blk`, `data/maps/objects/OaksLab.asm`, `text/OaksLab.asm`, the original `DOJO -> gym.bst` blockset alias, and `Dojo_Coll`. The PalletTown lab door now enters `OaksLab`, and the lab returns to PalletTown through its `LAST_MAP` door warps.

## [2026-04-09 13:39] scripting | OaksLab kept on safe default dialogue branches
Imported `OaksLab` object and text pointers, but intentionally kept only the safe interaction subset live. Rival, Oak, and Pokeball interactions branch on `got_starter`, while Pokedex, Girl, Scientist, and Oak2 use direct source-backed text; starter selection, rival battle, Pokedex handoff, and later Oak progression remain deferred.

## [2026-04-09 13:39] verification | OaksLab-expanded PalletTown hub re-verified
Rebuilt and re-ran the Linux/WSL checks with `cmake -S . -B build-native`, `cmake --build build-native -j"$(nproc)"`, `./build-native/pokered_native_tests`, `ctest --output-on-failure`, `./build-native/pokered_native --smoke-test`, and `timeout 2s ./build-native/pokered_native`. Current smoke output is `smoke-ok: world=5 pos=2,2 steps=6`, and the SDL launch still exits only because of the expected timeout.

## [2026-04-09 18:57] scripting | PalletTown gained the first bounded outdoor movement-trigger seam
Added a narrow movement-trigger result path to the native runtime and used it for the PalletTown north exit. Before `got_starter`, moving up into row `y == 1` now surfaces the short source-backed Oak warning text (`Hey! Wait! Don't go out!`) instead of behaving like a normal step.

## [2026-04-09 18:57] verification | PalletTown Oak warning seam re-verified
Rebuilt and re-ran the Linux/WSL checks with `cmake --build build-native -j"$(nproc)"`, `./build-native/pokered_native_tests`, `ctest --output-on-failure`, `./build-native/pokered_native --smoke-test`, and `timeout 2s ./build-native/pokered_native`. Current smoke output remains `smoke-ok: world=5 pos=2,2 steps=6`, and the new tests cover both the pre-starter warning trigger and the post-starter north-exit reopen behavior.

## [2026-04-09 19:50] provenance | current-map oracle overlay added to the SDL runtime
Added `native/src/oracle/provenance.cpp` as a small bridge from `WorldId` to source labels, then loaded `pokered.sym` and `pokered.map` in the app binary so `F7` can replace the help box with the current map's header/object labels plus bank/section ownership.

## [2026-04-09 19:50] verification | runtime provenance overlay plumbing re-verified
Rebuilt and re-ran the Linux/WSL checks with `cmake -S . -B build-native`, `cmake --build build-native -j"$(nproc)"`, `./build-native/pokered_native_tests`, `ctest --output-on-failure`, `./build-native/pokered_native --smoke-test`, and `timeout 2s ./build-native/pokered_native`. Current smoke output remains `smoke-ok: world=5 pos=2,2 steps=6`, and the test binary now also verifies shared `PalletTown` / `OaksLab` provenance lookups used by the runtime overlay.

## [2026-04-09 20:10] provenance | last-warp trace page added to the SDL overlay
Extended the `F7` runtime provenance overlay so it now cycles through normal controls, current-map provenance, and the last successful warp's source/target object provenance. `TryMoveWithResult` now carries warp metadata, and `native/src/oracle/provenance.cpp` exposes a small `LookupWarpProvenance` helper for the SDL app and tests.

## [2026-04-09 20:10] verification | last-warp provenance tracing re-verified
Rebuilt and re-ran the Linux/WSL checks with `cmake --build build-native -j"$(nproc)"`, `./build-native/pokered_native_tests`, `ctest --output-on-failure`, `./build-native/pokered_native --smoke-test`, and `timeout 2s ./build-native/pokered_native`. Current smoke output remains `smoke-ok: world=5 pos=2,2 steps=6`, and the test binary now verifies both shared warp provenance lookups and `TryMoveWithResult` warp metadata for real PalletTown/RedsHouse/OaksLab transitions.
