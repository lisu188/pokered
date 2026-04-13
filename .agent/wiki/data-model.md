# Data Model

## Source Of Truth
- The asm/data files remain the authoritative source of gameplay data.
- Native runtime data should be generated or imported from repo sources, not hand-maintained in parallel.

## Native Data Principles
- Preserve integer widths and bit-level semantics where correctness depends on them.
- Preserve data layout concepts that affect behavior, save compatibility, or determinism.
- Attach provenance metadata where practical:
  - source file
  - label/symbol
  - original table name

## Early Native Data Categories
- map layouts and collision
- map object placements
- text blocks
- encounter tables
- species/move/trainer tables
- tilesets and sprite metadata
