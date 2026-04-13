# Repo Map

## Top-Level Layout
- `main.asm`: high-level runtime and large engine includes
- `home.asm`: core home-bank logic and dispatch helpers
- `audio.asm`, `maps.asm`, `text.asm`, `ram.asm`: major content domains
- `engine/`: feature logic such as battle, movie, menus, overworld, and slots
- `data/`: tables, maps, events, wild encounters, palettes, and more
- `scripts/`: map and event scripts
- `gfx/`: PNG sources plus generated/intermediate graphics artifacts
- `tools/`: native helper programs used by the original Makefile
- `constants/`, `macros/`: shared symbols and assembly macros

## Build Shape
- `Makefile` defines versioned builds via `_RED`, `_BLUE`, `_DEBUG`, `_RED_VC`, `_BLUE_VC`.
- `layout.link` and RGBDS link flags define the final ROM layout.
- The build emits helper tools first, then generates graphics/compressed assets, then assembles, links, and fixes ROMs.

## Immediate Native-Port Implications
- The repo is not a codebase to translate line-for-line; it is a data-rich behavior specification.
- `tools/` is a crucial asset oracle and should be reused or mirrored in native tooling.
- The runtime naturally decomposes into boot/movie, overworld, battle, text/menu, audio, and data-loading concerns.

## Runtime Anchors
- Boot:
  - `home/start.asm` -> `_Start`
  - `home/init.asm` -> `Init`
- Title / intro:
  - `engine/movie/intro.asm` -> `PlayIntro`
  - `engine/movie/title.asm` -> `PrepareTitleScreen`, `DisplayTitleScreen`
- Main menu and new game:
  - `engine/menus/main_menu.asm` -> `MainMenu`, `StartNewGame`, `SpecialEnterMap`
- Overworld:
  - `home/overworld.asm` -> `EnterMap`, `OverworldLoop`, `NewBattle`, `LoadMapData`
- Scripts / trainer flow:
  - `home/trainers.asm` -> `ExecuteCurMapScriptInTable`, `CheckFightingMapTrainers`, `DisplayEnemyTrainerTextAndStartBattle`, `EndTrainerBattle`
- Save:
  - `engine/menus/save.asm` -> `TryLoadSaveFile`, `SaveMenu`, `SaveGameData`, `ClearAllSRAMBanks`

## Layout Notes
- `layout.link` declares the hardware vectors, home bank, ROMX bank naming, WRAM, SRAM, and HRAM sections explicitly.
- `pokered.sym` and `pokered.map` are immediately useful as an oracle for:
  - symbol addresses
  - bank ownership
  - free-space and section boundaries
