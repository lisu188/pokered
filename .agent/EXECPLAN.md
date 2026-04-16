# Active Execution Plan

## Current Milestone
- Keep the playable Pallet Town slice green while extending adjacent interiors, outdoor script seams, runtime-facing provenance hooks, and a verified Windows cross-build path.

## Immediate Tasks
- [x] Create work branch
- [x] Validate original ROM build
- [x] Create durable memory files
- [x] Run four bounded exploration scopes in parallel
- [x] Merge their findings into the wiki
- [x] Add CMake project skeleton
- [x] Add SDL app bootstrap
- [x] Define first vertical slice boundaries in code
- [x] Implement the first native executable path
- [x] Add deterministic native save/load coverage and smoke tests
- [x] Replace prototype RedsHouse1F tile semantics with source-driven blockset expansion
- [x] Harden native save writes with temp-file replacement and explicit payload sizing
- [x] Add paged first-slice text so the exact Mom/TV dialogue fits the native UI
- [x] Import exact source-backed first-slice object/text metadata and dispatch data
- [x] Add `.sym` / `.map` provenance plumbing for native assets and traces
- [x] Add `RedsHouse2F` as a second source-driven indoor map
- [x] Add stair warp traversal between `RedsHouse1F` and `RedsHouse2F`
- [x] Document MinGW install and cross-build invocation from WSL
- [x] Generalize the indoor metadata importer beyond the `RedsHouse*` pair
- [x] Add a minimal exterior slice so `LAST_MAP` door warps can transition for real
- [x] Add camera-based SDL rendering so larger native maps remain navigable
- [x] Import `BluesHouse` as the next PalletTown-adjacent live interior
- [x] Import `OaksLab` as the next PalletTown-adjacent live interior
- [x] Add the first bounded `PalletTown` Oak north-exit warning seam
- [x] Lift current `.sym` / `.map` provenance into a runtime-facing current-map overlay
- [x] Extend the runtime-facing provenance overlay with a last-warp trace page
- [x] Extend the runtime-facing provenance overlay with a last-message text trace page
- [x] Extend the runtime-facing provenance overlay with a last-message source-label trace page
- [x] Extend the runtime-facing provenance overlay with a last-move trace page
- [x] Extend the runtime-facing provenance overlay with a last-interaction trace page
- [x] Extend the runtime-facing provenance overlay with a last-interaction-branch trace page
- [x] Extend the runtime-facing provenance overlay with a last-state-gate trace page
- [x] Extend the runtime-facing provenance overlay with a last-map-state trace page
- [x] Extend the runtime-facing provenance overlay with a gate-source trace page
- [x] Extend the runtime-facing provenance overlay with a current map-script state trace page
- [x] Extend the runtime-facing provenance overlay with a live facing-target trace page
- [x] Extend the runtime-facing provenance overlay with a live facing text trace page
- [x] Extend the runtime-facing provenance overlay with a live facing-branch trace page
- [x] Extend the runtime-facing provenance overlay with a live facing gate-source trace page
- [x] Align PalletTown exterior door exits with the original auto step-out flow
- [x] Preserve final landing coordinates in PalletTown door warp move traces
- [x] Restore collision-triggered immediate PalletTown door re-exits from indoor warp tiles
- [x] Constrain blocked PalletTown re-warps to door tiles and cover the remaining BluesHouse doorway tile
- [x] Import `Route1` as the first PalletTown-adjacent outdoor map and connect PalletTown's north edge after the starter gate

## Current Working Assumptions
- The disassembly remains the authoritative behavior specification.
- Native runtime code and import/tooling code should be kept separate.
- First playable slice should prioritize visible motion and text over battle.
- Checked-in binary assets such as `.blk` and `.bst` are valid source inputs and should be consumed directly before building broader asm parsers.

## Known Gaps
- No MinGW cross-toolchain installed locally yet
- No MinGW SDL2 package installed locally yet
- No emulator-backed trace harness yet
- Runtime-facing provenance now covers the current map, the current map-script state, the current `LAST_MAP` return anchor, the current facing source-backed object/interaction target, the current facing source-backed text label, the current facing branch/handler preview, the current facing gate source/backing state, the last successful warp, the last move attempt, the last confirm-based interaction, the last interaction branch/handler state, the last evaluated native state gate and its current source backing state, the last displayed source-backed message, and the last displayed message's source label, but broader object-state/script-state trace hooks are still missing
- Map import is currently proven for the `RedsHouse*`, `HOUSE`, and one `OVERWORLD` slice, but not yet broader than that
- `Route21` is still not imported, and `Route1` is live only through the PalletTown-side connection; its ViridianCity edge is still bounded
- Oak's PalletTown follow-to-lab chain and the `OaksLab` starter / rival sequence remain deferred
- `BluesHouse` currently uses the source-backed default Daisy text branch only; the Town Map gift/event path is still deferred

## First Slice Decision
- First native slice target:
  - SDL window
  - deterministic core state bootstrap
  - title/new-game handoff placeholder
  - starter map data path
  - player movement/collision/text scaffolding

## Current Verification Status
- `cmake -S . -B build-native`
- `cmake --build build-native -j"$(nproc)"`
- `ctest --test-dir build-native --output-on-failure`
- `./build-native/pokered_native_tests`
- `./build-native/pokered_native --smoke-test`
- Current result: `100% tests passed, 0 tests failed out of 2`
- Current smoke output: `smoke-ok: world=5 pos=2,2 steps=9`
- Native Linux SDL launch still starts under WSL via `timeout 2s ./build-native/pokered_native`; on the latest run it timed out normally with no startup error.

## Next Focus
- Extend the new `Route1` seam toward either ViridianCity boundary handling, Route1 encounter/ledge semantics, or a small source-backed Route1 item/event simplification.
- Decide whether to keep `OaksLab` on safe default dialogue branches or implement the first starter / rival script seam.
- Decide whether to extend the bounded `PalletTown` Oak warning into the full follow-to-lab cutscene.
- Decide whether `BluesHouse` Daisy should remain a static-text simplification or become the first small indoor gift/event seam.
- Extend the current `.sym` / `.map` overlay past the current-map, current-map-script, last-map-state, live-facing-target, live-facing-text, live-facing-branch, live-facing-gate-source, last-state-gate, last-state-gate-source, last-interaction-branch, last-interaction, last-move, and last-message-source pages into broader object-state and script-state trace hooks.
- Broaden the importer to another tileset family after the exterior seam is stable.
- Validate the documented WSL -> MinGW -> `.exe` flow once the missing compiler and SDL2 packages are available.
