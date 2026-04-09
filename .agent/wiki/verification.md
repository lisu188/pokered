# Verification

## Oracle Strategy
- Keep the original RGBDS ROM build working locally.
- Use ROM outputs and symbols as the behavior oracle.

## Early Verification Layers
- build parity:
  - ROM build still succeeds
  - known SHA1 hashes remain stable
- native determinism:
  - deterministic save/load round-trips
  - reproducible RNG sequences from known seeds
- asset parity:
  - importer output traceability to source assets
  - screenshot comparisons for simple scenes
- behavior parity:
  - scripted scenario traces
  - state diffs for focused subsystems

## Planned Native Checks
- unit tests for pure data importers and deterministic systems
- smoke test for native app startup
- scenario fixtures for map load and movement
- later, emulator-backed differential traces where practical

## Current Native Verification
- Native configure/build:
  - `cmake -S . -B build-native`
  - `cmake --build build-native -j"$(nproc)"`
- Native tests:
  - `ctest --test-dir build-native --output-on-failure`
  - `./build-native/pokered_native_tests`
  - current result: `100% tests passed, 0 tests failed out of 2`
- Native smoke run:
  - `./build-native/pokered_native --smoke-test`
  - current result: `smoke-ok: world=5 pos=2,2 steps=6`
  - note: the smoke temp path falls back to `/tmp` when `std::filesystem::temp_directory_path` is unavailable
  - native Linux SDL launch also still starts under WSL via `timeout 2s ./build-native/pokered_native`, with the expected timeout exit and no startup error
- Save determinism coverage currently verifies:
  - identical byte output for equivalent saves
  - round-trip restoration of the first-slice world state
  - temp-file save replacement behavior through the normal native save path
  - payload-size validation on load
  - corrupted-save rejection after a serialized byte flip
- Map import coverage currently verifies:
  - imported behavior tile values for selected `RedsHouse1F` cells, including the TV cell, upstairs warp, front-door cells, and table cells
  - imported `RedsHouse2F` block data, stair tile semantics, and stair warp target metadata
  - imported `PewterSpeechHouse` block data, `House_Coll` passability, deferred door-warp metadata, and NPC counts
  - imported `BluesHouse` block data, `House_Coll` passability, live PalletTown door-warp metadata, and NPC/object counts
  - imported `OaksLab` block data, `Dojo_Coll` passability, live PalletTown door-warp metadata, and NPC/object counts
  - imported `PalletTown` block data, `Overworld_Coll` passability, door-warp metadata, sign/NPC counts, and outdoor message coverage
  - source-driven passability through `native/tests/save_system_tests.cpp`
  - paged Mom/TV message lookup behavior through `native/tests/save_system_tests.cpp`
  - imported warp/bg-event/NPC counts for `RedsHouse1F`, `RedsHouse2F`, `PewterSpeechHouse`, `BluesHouse`, `OaksLab`, and `PalletTown`
  - render overlays for TV, stairs, door, and table cells after blockset expansion
  - stair traversal from `RedsHouse1F` -> `RedsHouse2F` -> `RedsHouse1F`
  - real `RedsHouse1F` front-door traversal into `PalletTown` and re-entry from `PalletTown`
  - static NPC interaction dispatch for `PewterSpeechHouse`
  - static interaction dispatch for the current `BluesHouse` Daisy/Town Map objects
  - safe default interaction dispatch for the current `OaksLab` Rival / Oak / Pokeball branches plus Pokedex / Girl / Scientist text
  - static NPC/sign interaction dispatch for `PalletTown`
  - movement-triggered `PalletTown` Oak warning coverage before `got_starter`, plus reopening of the north exit once the starter flag is set
  - PalletTown <-> BluesHouse door traversal
  - PalletTown <-> OaksLab door traversal
- Oracle provenance coverage currently verifies:
  - `native/src/oracle/symbol_file.cpp` can load `pokered.sym`
  - `native/src/oracle/map_file.cpp` can load `pokered.map`
  - `native/src/oracle/provenance.cpp` can join current-map header/object labels to symbol addresses and `.map` sections for runtime-facing use
  - `native/src/oracle/provenance.cpp` can also join source/target map object provenance for last-warp runtime traces
  - symbol lookup for `EnterMap`, `OverworldLoop`, `PalletTown_h`, `BluesHouse_h`, `OaksLab_h`, `RedsHouse1F_h`, `RedsHouse2F_h`, `PewterSpeechHouse_h`, and `TryLoadSaveFile`
  - `.sym` + `.map` section containment for `Home`, `Maps 1`, `Maps 2`, `Maps 8`, `Maps 15`, `Maps 4`, and `bank1C`
  - `TryMoveWithResult` warp metadata for PalletTown doors, OaksLab entry, and RedsHouse1F exit versus non-warp movement/script seams

## Confirmed Oracle Assets
- `pokered.gbc`, `pokeblue.gbc`, `pokeblue_debug.gbc`
- `pokered.sym`, `pokeblue.sym`, `pokeblue_debug.sym`
- `pokered.map`, `pokeblue.map`, `pokeblue_debug.map`
- `roms.sha1`

## Immediate Verification Hooks
- use `.sym` and `.map` to anchor specific oracle labels and bank ownership
- treat `engine/menus/save.asm` plus `ram/sram.asm` as the source of truth for save layout and checksum behavior
- keep deterministic save/load as a native milestone requirement, not a later cleanup item

## Next Verification Work
- extend the current runtime-facing provenance hooks past the last-warp page into broader movement/script tracing
- add the next outdoor-adjacent scenario beyond the now-live `OaksLab` seam
- add checks for deferred `LAST_MAP` door behavior on maps beyond the PalletTown seam
- add importer regression checks for the lower-left representative tile rule used by overworld collision/warp semantics
- later, add screenshot and structured trace comparisons for a multi-room indoor scenario
