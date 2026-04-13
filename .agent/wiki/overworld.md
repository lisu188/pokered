# Overworld

## Scope
- map loading
- player movement
- collision
- object interaction
- scripts/events
- text boxes

## Current Status
- Not yet mapped in detail.
- Expected source hotspots:
  - `engine/overworld/`
  - `maps.asm`
  - `scripts/`
  - `data/maps/`

## First Vertical Slice Goal
- Load a starter map natively.
- Render a visible tilemap and player sprite.
- Support deterministic movement and collision.
- Trigger a simple text interaction.

## Confirmed Overworld Anchors
- `home/overworld.asm`
  - `EnterMap`: loads map data, clears enter-map variables, resets special-case flags, and sets current-map-loaded bits
  - `OverworldLoop` / `OverworldLoopLessDelay`: central movement, interaction, warp, and battle-entry loop
  - `NewBattle`: battle handoff from the overworld
  - `LoadMapData`: map-load anchor
- `engine/overworld/clear_variables.asm`
  - `ClearVariablesOnEnterMap`
- `engine/overworld/player_state.asm`
  - `CheckForceBikeOrSurf`

## Observed Behavior Shape
- Overworld input path:
  - read joypad
  - handle scripted movement / warps / safari / trainer state
  - handle `START` and `A` interactions
  - resolve direction input and movement
  - branch to `EnterMap` or `NewBattle` when required
- This is a strong candidate for the first meaningful native gameplay loop after boot/title.

## Current First-Map Decision
- Use `RedsHouse1F` for the initial native playable slice.
- Reason:
  - smaller than `PalletTown`
  - still exercises map load, movement, collision, facing, and at least one text interaction
  - avoids map-connection support in the first pass
