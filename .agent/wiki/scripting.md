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
- `PalletTown` now provides the first bounded outdoor movement-trigger seam:
  - `scripts/PalletTown.asm` triggers Oak's intervention when the player reaches `y == 1` before `EVENT_FOLLOWED_OAK_INTO_LAB`.
  - the current native runtime mirrors only the first warning beat by blocking the north-exit step before `got_starter` and surfacing `_PalletTownOakHeyWaitDontGoOutText`.
  - the rest of the original sequence, including Oak appearing, walking to the player, and escorting the player into the lab, remains deferred.
- `OaksLab` is the next imported interior with much richer original script flow than the current runtime:
  - `scripts/OaksLab.asm` covers starter selection, Oak movement, rival battle, Pokedex handoff, parcel progression, and later reward text.
  - the current native runtime imports the object/text pointers and keeps only safe default interaction branches live:
    - Rival: pre-starter vs post-starter ambient line
    - Pokeballs: explanatory line vs last-mon line
    - Oak: choose-your-#MON line vs post-starter battle tutorial line
    - Pokedex, Girl, Scientist, and Oak2: direct source-backed static text
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

## Provenance Note
- Runtime-facing provenance is no longer test-only: the SDL app can now display the current map's source header/object labels and section ownership through the existing `.sym` / `.map` plumbing.
- The current overlay now also records the last successful warp's source/target object provenance, the last move attempt, the last confirm-based interaction, the last interaction branch/handler label, the most recent displayed source-backed text label, and the corresponding source/local script label that emitted it.
- For the current bounded PalletTown seam, the last-move page can resolve the blocked north-exit trigger back to `PalletTownDefaultScript`, which complements the message-source page's `PalletTownOakText.HeyWaitDontGoOutText` lookup.
- The last-interaction page can now distinguish NPC, bg-event, and miss cases and join current-map object provenance to the local text-source label for confirm-based interactions.
- The new last-interaction-branch page uses each interaction's preserved pre-branch `origin_message` to resolve conditional handlers such as `RedsHouse1FMomText.heal` and `OaksLabRivalText.afterChooseMon`, which makes native branch selection visible without introducing a full script runner.
- The overlay is still intentionally observational only and does not yet trace broader script-state transitions, object toggles, or multi-step cutscene progress.
