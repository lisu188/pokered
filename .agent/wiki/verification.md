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
  - current result: `smoke-ok: world=5 pos=2,2 steps=9`
  - note: the smoke temp path falls back to `/tmp` when `std::filesystem::temp_directory_path` is unavailable
  - native Linux SDL launch also still starts under WSL via `timeout 2s ./build-native/pokered_native`; on the latest run it timed out normally with no startup error
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
  - PalletTown <-> BluesHouse door traversal, including the second interior doorway tile
  - PalletTown <-> OaksLab door traversal
  - smoke coverage across all three live PalletTown door pairs, including immediate indoor re-exits from both live doorway tiles in `RedsHouse1F`, `BluesHouse`, and `OaksLab`, snapshot-checked passable lateral steps from the live interior landing tiles after each PalletTown re-entry, snapshot-checked passable lateral step-offs from the paired interior doorway tiles, snapshot-checked doorway-span warps back into `PalletTown`, plus a PalletTown-side re-entry check for each live door pair
  - forward movement from the live interior landing tiles after PalletTown re-entry, including passable forward steps inside `RedsHouse1F` and `BluesHouse` plus the NPC-blocked `OaksLab` forward input that re-exits into `PalletTown`
  - forward movement from the remaining paired interior doorway tiles, including passable forward steps from the non-entry door tile inside `RedsHouse1F`, `BluesHouse`, and `OaksLab`
  - smoke coverage for the outdoor landing forward and lateral cases below `RedsHouse1F`, `BluesHouse`, and `OaksLab`, including passable house frontage checks via snapshot copies, passable left/right landing steps for all three live doors, plus the collision-blocked `OaksLab` frontage, while preserving the current `smoke-ok: world=5 pos=2,2 steps=9` path
  - exterior PalletTown door auto-step behavior after `RedsHouse1F`, `BluesHouse`, and `OaksLab` exits
  - passable lateral step-offs in both directions from the PalletTown auto-step landing tile for `RedsHouse1F`, `BluesHouse`, and `OaksLab`
  - forward movement from the PalletTown auto-step landing tile, including passable forward steps below `RedsHouse1F` and `BluesHouse` plus the collision-blocked forward edge below `OaksLab`
  - final landing-coordinate `MoveResult` metadata for PalletTown door entries and exits, including `RedsHouse1F`, `BluesHouse`, and `OaksLab` round-trips
  - collision-triggered immediate re-exit while standing on every live interior doorway warp tile for `RedsHouse1F`, `BluesHouse`, and `OaksLab`, while keeping passable lateral step-offs on every live interior doorway tile and blocked stair movement local
- Oracle provenance coverage currently verifies:
  - `native/src/oracle/symbol_file.cpp` can load `pokered.sym`
  - `native/src/oracle/map_file.cpp` can load `pokered.map`
  - `native/src/oracle/provenance.cpp` can join current-map header/object labels to symbol addresses and `.map` sections for runtime-facing use
  - `native/src/oracle/provenance.cpp` can also join the current map to its root script label and, where available, script-table/current-script symbols for runtime-facing script-state traces
  - `native/src/oracle/provenance.cpp` can also join the current `LAST_MAP` return anchor to the owning map object label and `.map` section for runtime-facing state traces
  - `native/src/oracle/provenance.cpp` can also join the current facing NPC/bg-event target from `WorldState` to the current map object label plus local source label for live runtime traces
  - `native/src/oracle/provenance.cpp` can also join the current facing interaction result from `WorldState` to the exact source-backed text label and `.map` section for live runtime previews
  - `native/src/oracle/provenance.cpp` can also join the current facing interaction from `WorldState` to the current asm-local branch handler for live runtime previews
  - `native/src/oracle/provenance.cpp` can also join the current facing interaction gate from `WorldState` to the source gate/storage symbols that back the live runtime preview
  - `native/src/oracle/provenance.cpp` can also join source/target map object provenance for last-warp runtime traces
  - `native/src/oracle/provenance.cpp` can also join blocker-tagged movement seams to script labels and `.map` sections for last-move runtime traces when a source-backed seam is known
  - `native/src/oracle/provenance.cpp` can also join confirm-based interactions to the current map object label plus local source label for last-interaction runtime traces
  - `native/src/oracle/provenance.cpp` can also join conditional interaction outcomes back to asm-local branch labels for last-interaction-branch runtime traces
  - the runtime now also preserves evaluated native branch predicates (`got_starter`, `facing == up`) for a last-state-gate overlay page alongside the existing oracle-backed handler lookups
  - `native/src/oracle/provenance.cpp` can also join those native branch predicates back to the source gate they stand in for, including `EVENT_FOLLOWED_OAK_INTO_LAB`, `wStatusFlags4`, and `wSpritePlayerStateData1FacingDirection`
  - `native/src/oracle/provenance.cpp` can also join surfaced `MessageId` values to source text labels and `.map` sections for last-message runtime traces
  - `native/src/oracle/provenance.cpp` can also join surfaced `MessageId` values to script/local source labels and `.map` sections for last-message-source runtime traces
  - symbol lookup for `EnterMap`, `OverworldLoop`, `PalletTown_h`, `BluesHouse_h`, `OaksLab_h`, `RedsHouse1F_h`, `RedsHouse2F_h`, `PewterSpeechHouse_h`, and `TryLoadSaveFile`
  - `.sym` + `.map` section containment for `Home`, `Maps 1`, `Maps 2`, `Maps 8`, `Maps 15`, `Maps 4`, and `bank1C`
  - `TryMoveWithResult` warp metadata for PalletTown doors, OaksLab entry, and RedsHouse1F exit versus non-warp movement/script seams
  - blocker-aware `TryMoveWithResult` metadata for the PalletTown Oak seam, a solid TV tile, an NPC collision tile, and ordinary reopened north-exit movement, including current state-gate metadata for scripted seams
  - `InspectFacingTile` metadata for NPC, bg-event, miss, and conditional-origin interactions in the current PalletTown, OaksLab, and RedsHouse1F slice, including current state-gate metadata for Mom/TV and OaksLab branches
  - message provenance lookup for `MomWakeUp`, the PalletTown Oak warning seam, and `OaksLabPokedex`, plus source-label provenance lookup for `MomWakeUp`, the PalletTown Oak warning seam, `OaksLabRivalText.GrampsIsntAroundText`, and `OaksLabPokedex`, alongside null coverage for native-only or abstract message ids
  - move-script provenance lookup for the PalletTown Oak warning seam back to `PalletTownDefaultScript`, plus null coverage for non-script blockers and unrelated maps/messages
  - interaction provenance lookup joining `PalletTown_Object` + `PalletTownGirlText` and `OaksLab_Object` + `OaksLabPokedexText`, plus branch provenance lookup for `RedsHouse1FMomText`, `RedsHouse1FMomText.heal`, `RedsHouse1FTVText`, `OaksLabRivalText.afterChooseMon`, and `OaksLabOak1Text.check_for_poke_balls`, plus null coverage for native-only, static no-branch, or empty interactions
  - current map-script provenance lookup for the PalletTown and OaksLab script-table maps plus the script-only RedsHouse1F map, including current-script WRAM labels where the source uses them
  - current `LAST_MAP` provenance lookup for `PalletTown_Object #1` and `RedsHouse1F_Object #2`, plus null coverage for unset/invalid warp ids
  - state-gate provenance lookup for the PalletTown Oak seam event gate, `wStatusFlags4` / `BIT_GOT_STARTER`, and `wSpritePlayerStateData1FacingDirection` / `SPRITE_FACING_UP`, plus null coverage for non-script moves and static interactions
  - current facing provenance lookup for the PalletTown girl, the branched RedsHouse1F TV wrong-side text, and the post-starter OaksLab rival line, plus null coverage for empty facing targets
  - current facing text provenance lookup for the PalletTown girl, the branched RedsHouse1F TV wrong-side text, and the post-starter OaksLab rival line, plus null coverage for empty facing targets
  - current facing-branch provenance lookup for the RedsHouse1F Mom default/rest handlers, the RedsHouse1F TV handler, and the post-starter OaksLab rival handler, plus null coverage for static or empty facing targets
  - current facing gate-source provenance lookup for the RedsHouse1F Mom gate, the RedsHouse1F TV facing gate, and the post-starter OaksLab rival gate, plus null coverage for static or empty facing targets

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
- extend the current runtime-facing provenance hooks past the current-map, current-map-script, last-map-state, live-facing-target, live-facing-text, live-facing-branch, live-facing-gate-source, last-state-gate, last-state-gate-source, last-interaction-branch, last-interaction, last-move, and last-message-source pages into broader object-state and script-state tracing
- add the next outdoor-adjacent scenario beyond the now-live `OaksLab` seam
- add checks for deferred `LAST_MAP` door behavior on maps beyond the PalletTown seam
- add importer regression checks for the lower-left representative tile rule used by overworld collision/warp semantics
- later, add screenshot and structured trace comparisons for a multi-room indoor scenario
