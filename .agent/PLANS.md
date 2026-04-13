# Long-Horizon Plan

## Phase 0: Bootstrap
- durable repo memory
- oracle build documentation
- initial architecture notes
- native project skeleton

## Phase 1: Exploration And Import Strategy
- runtime control-flow mapping
- asset pipeline reconstruction
- data import/generation design
- verification harness design

## Phase 2: First Playable Vertical Slice
- native boot path
- SDL window + input loop
- deterministic core state bootstrap
- starter map load
- player movement + collision
- simple text box interaction
- status: running locally; next work is importing first-slice asm metadata and provenance on top of the source-driven block/tile path
- update: source-driven room semantics are in place; next work is source-backed text and provenance
- update: first-slice object/text metadata is now imported from asm; next work is reusable provenance and broader map coverage

## Phase 3: Scripts And Wider Overworld
- event flags
- object interactions
- map transitions
- broader asset coverage

## Phase 4: Encounters And Battle
- encounter generation
- battle bootstrap
- turn loop
- menu/text integration

## Phase 5: Save/Load And Determinism
- deterministic save format
- scenario replay support
- regression harnesses
- status: deterministic file save/load is running with corruption rejection; replay/tracing is still pending

## Phase 6: Packaging And Cross-Builds
- Linux release flow
- MinGW Windows cross-build path
- asset packaging and reproducible builds
