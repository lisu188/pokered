# Asset Pipeline

## Original Build Pipeline
- `rgbgfx` converts source PNG assets into Game Boy tile formats:
  - `*.2bpp`
  - `*.1bpp`
- `tools/gfx` post-processes generated graphics:
  - trim whitespace
  - remove duplicates
  - preserve tile ranges
  - interleave for specific assets
- `tools/pkmncompress` compresses selected graphics into `*.pic`.
- `tools/scan_includes` discovers transitive asm includes during the RGBDS build.
- `tools/make_patch` is used for VC patch output.

## Build Files
- Root build logic: `Makefile`
- Helper tool build logic: `tools/Makefile`

## Native-Port Direction
- Keep the original PNG sources as the human-editable asset base.
- Build/import tooling should reconstruct native-friendly runtime assets from repository sources rather than copying ROM bytes without provenance.
- Asset-generation steps should preserve source-file provenance for debugging and parity checks.
- Checked-in binary assets such as `.blk`, `.bst`, `.tilemap`, and `.rle` are also source inputs and should be treated as first-class canonical data, not as disposable build byproducts.

## Confirmed Helper Tool Boundaries
- `tools/gfx.c`
  - post-processes generated tile data
  - supports trimming whitespace, removing whitespace, deduplicating tiles, x/y-flip culling, preserve lists, and interleaving
- `tools/pkmncompress.c`
  - compresses square `2bpp` image payloads up to `15x15` tiles into `.pic`
  - also contains decompression logic, which is useful for importer verification
- `tools/scan_includes.c`
  - tokenizes asm enough to find `INCLUDE` and `INCBIN` recursively
  - can act as a lightweight source-dependency oracle for future generators

## Representative Asset Rules
- `%.2bpp: %.png`
  - `rgbgfx --colors dmg`
  - optional `tools/gfx` post-pass
- `%.1bpp: %.png`
  - `rgbgfx --colors dmg --depth 1`
  - optional `tools/gfx` post-pass
- `%.pic: %.2bpp`
  - `tools/pkmncompress`

## Native Import Recommendation
- Split native asset handling into two layers:
  - import/generation tools that read repo sources and emit native-friendly assets
  - runtime asset loaders that consume those generated outputs
- Reuse `tools/pkmncompress` logic or port it into a standalone native-tooling library before touching runtime battle/portrait rendering.

## Current Native Usage
- The running first slice already imports `maps/RedsHouse1F.blk` through the build system:
  - `CMakeLists.txt` generates `${build}/generated/reds_house_1f_blk.hpp`
  - `native/src/world/map_data.cpp` consumes `generated::kRedsHouse1FBlocks`
- The running first slice also imports `gfx/blocksets/reds_house.bst`:
  - `CMakeLists.txt` generates `${build}/generated/reds_house_blockset.hpp`
  - `native/src/world/map_data.cpp` derives per-cell behavior tiles from the lower-left representative tile of each 16x16 movement cell
- The running first slice now also imports narrow asm metadata for `RedsHouse1F`:
  - `CMakeLists.txt` generates `${build}/generated/reds_house_1f_metadata.hpp`
  - `cmake/GenerateRedsHouse1FMetadata.cmake` parses `data/maps/objects/RedsHouse1F.asm` and `text/RedsHouse1F.asm`
- Current source-driven room semantics are built from:
  - map layout bytes from `maps/RedsHouse1F.blk`
  - block bytes from `gfx/blocksets/reds_house.bst`
  - passable tile IDs from `data/tilesets/collision_tile_ids.asm`
  - room-specific warp/object/text metadata from `data/maps/objects/RedsHouse1F.asm` and `text/RedsHouse1F.asm`
- `gfx/blocksets/reds_house.bst` is `304` bytes, which implies `19` blocks at `16` bytes per block.
- `maps/RedsHouse1F.blk` is `16` bytes, matching the `4 x 4` block layout declared for `REDS_HOUSE_1F`.

## Immediate Asset Work
- Keep the importer/runtime boundary narrow:
  - expand only the minimum block/tile/collision representation needed for the first slice
  - defer full graphics decoding of `gfx/tilesets/reds_house.2bpp` until after text/provenance work and broader map coverage
  - generalize the first-slice metadata importer beyond `RedsHouse1F` before widening to broader graphics decoding
