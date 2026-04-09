# Decisions

## Active Decisions

### Linux-first native runtime
- Status: active
- Rationale: Linux toolchain and SDL2 are already available locally; this enables rapid iteration while preserving a clear later path to Windows cross-builds.

### Generated/imported gameplay data
- Status: active
- Rationale: the disassembly is a structured specification. Generated native data is more auditable and maintainable than hand-transcribed tables.

### Vertical-slice implementation
- Status: active
- Rationale: a playable boot/title/map slice will expose the right architecture faster than broad dead-code translation.

### Oracle-first verification
- Status: active
- Rationale: the ROM build provides a stable reference for data, behavior, and regression checks.

### First playable map is `RedsHouse1F`
- Status: active
- Rationale: it provides a compact but meaningful overworld slice with movement, collision, and text without requiring broader map-connection support.

### First native slice shortcuts around full Oak Speech
- Status: active
- Rationale: the boot/title/menu path should stay visible, but deep intro scripting is lower value than reaching `EnterMap` and `OverworldLoop` semantics early.

### First slice uses real checked-in binary map assets before broader asm importers
- Status: active
- Rationale: `maps/*.blk` and `gfx/blocksets/*.bst` are canonical repo inputs with stable provenance. Consuming them directly is lower-risk than inventing a wider parser before the room logic is playable.

### Deterministic native saves are file-based, versioned, and stronger than cart SRAM checksums
- Status: active
- Rationale: native saves should preserve deterministic replay and corruption detection without inheriting the cart's weak presence check or multi-step non-atomic SRAM write flow.

### Native overworld import uses the lower-left representative tile of each 16x16 movement cell
- Status: active
- Rationale: the original overworld collision logic samples the lower-left 8x8 tile for movement cells, so the native map importer uses the same representative tile when turning `.bst` data into behavior tiles.

### Exact first-slice room text should fit through lightweight paging before a full text interpreter exists
- Status: active
- Rationale: the current room is more useful with the exact Mom and TV lines visible now than with clipped placeholder text, and small paging logic is much cheaper than pulling in the entire text-command engine early.

### Top-level `CMakeLists.txt` is the authoritative native build graph
- Status: active
- Rationale: a single native entry point keeps generated headers, tests, and future cross-build wiring from drifting into parallel definitions.

### First exterior support stops at a bounded PalletTown hub
- Status: active
- Rationale: PalletTown plus a live `RedsHouse1F` door proves outdoor import, `LAST_MAP` traversal, camera rendering, and save determinism without forcing Route 1, Route 21, or Oak's cutscene chain into the same milestone.

### Larger native maps use a scrolling camera instead of shrinking the world to fit
- Status: active
- Rationale: a camera viewport keeps larger overworld maps readable at the `160 x 144` SDL logical size and avoids coupling gameplay progress to a future pixel-perfect renderer.
