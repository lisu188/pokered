# Battle

## Scope
- encounter transition
- battle state model
- move execution
- UI/text flow
- RNG-sensitive behavior

## Current Status
- Not yet mapped in detail.
- Expected source hotspots:
  - `engine/battle/`
  - `data/moves/`
  - trainer/species data

## Porting Note
- Battle correctness is highly sensitive to integer math, RNG, and state order. Avoid floats and preserve step ordering.
