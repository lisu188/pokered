# Scripting

## Scope
- map scripts
- event flags
- map object interactions
- text dispatch
- cutscene/event progression

## Current Status
- Script system is still not mapped in detail overall.
- The first slice now imports `RedsHouse1F` object/text metadata directly from source asm, but the control-flow decisions around Mom/TV interaction are still expressed as a small native switch rather than a reusable script runner.
- `PewterSpeechHouse` confirms a simpler script pattern: `scripts/PewterSpeechHouse.asm` only enables auto text-box drawing and routes static NPC text pointers, which the native runtime now reproduces through source-driven NPC message IDs rather than a per-map script interpreter.
- `BluesHouse` adds the first small indoor script seam beyond static NPC text: `scripts/BluesHouse.asm` conditionally offers the Town Map through Daisy, but the current native runtime intentionally stays on the default pre-Pokedex dialogue branch until item/event state exists.
- Expected source hotspots:
  - `scripts/`
  - `data/maps/objects/`
  - event constant tables

## Native Direction
- Prefer a small explicit native interpreter/runtime for generated script data over ad hoc per-map handwritten logic.

## Confirmed Script Pattern
- Map scripts typically follow a stable structure:
  - `<MapName>_Script`
  - `<MapName>_ScriptPointers`
  - `w<MapName>CurScript`
  - per-state handlers
- `home/trainers.asm` provides shared dispatch helpers:
  - `ExecuteCurMapScriptInTable`
  - `CheckFightingMapTrainers`
  - `DisplayEnemyTrainerTextAndStartBattle`
  - `EndTrainerBattle`
- `PewterSpeechHouse_Script` is a minimal example of the “static indoor text only” path:
  - script body: `jp EnableAutoTextBoxDrawing`
  - text table: two fixed `dw_const` entries
  - text bodies: `text_far` -> `_PewterSpeechHouseGamblerText` / `_PewterSpeechHouseYoungsterText`
- `BluesHouse_Script` is the first imported interior whose original asm logic is richer than the current runtime:
  - script body sets `EVENT_ENTERED_BLUES_HOUSE` and then falls through to a noop script
  - Daisy's seated text branches across `EVENT_GOT_POKEDEX`, `EVENT_GOT_TOWN_MAP`, item-give success, and object hiding
  - the native runtime currently imports the text/object pointers but keeps only the default seated-text branch live

## Practical Native Direction
- Generate script tables and identifiers from asm/map-object data.
- Recreate a small dispatcher that mirrors:
  - current-script index
  - event-flag lookups
  - trainer battle handoffs
  - text callback transitions
