# Pokered Native Port Instructions

## Mission
- Turn this disassembly into a fully runnable native C++20 + SDL2 application with CMake as the primary build system.
- Keep Linux builds first-class and maintain a documented path to Windows `.exe` builds from WSL via MinGW when available.
- Treat the original ROM build as the behavior oracle and preserve gameplay semantics, integer behavior, RNG behavior, timing-sensitive logic, and save determinism.

## Durable Memory
- The source tree and external references are raw inputs.
- `.agent/wiki/` is the compiled knowledge layer. Keep it current and concise.
- `.agent/state.json` is the turn-to-turn execution state.
- `.agent/PLANS.md` is the long-horizon roadmap.
- `.agent/EXECPLAN.md` is the active execution plan and should stay actionable.

## Resume Protocol
- At the start of every turn, first read:
  - `AGENTS.md`
  - `.agent/state.json`
  - `.agent/EXECPLAN.md`
  - `.agent/wiki/index.md`
  - the newest relevant `.agent/wiki/*.md` pages for the active task
- If the user says only `continue`, resume from `.agent/state.json` and `.agent/EXECPLAN.md` without re-asking for context.

## Logging Protocol
- Update `.agent/wiki/index.md` and `.agent/wiki/log.md` whenever:
  - a task is completed
  - a blocker appears or is cleared
  - an architectural decision changes
  - a verification result matters
- Every log entry must start with:
  - `## [YYYY-MM-DD HH:MM] type | title`

## Engineering Rules
- Keep the oracle build working and documented.
- Prefer generated native data artifacts over hand-transcribed assets or tables.
- Separate runtime code from conversion/import tooling.
- Prefer vertical playable slices over large inert dumps of translated code.
- Keep provenance back to original asm labels, files, and symbols whenever practical.
- Prefer integers and fixed-point logic over floats in gameplay code.

## Subagent Rules
- Use bounded parallel subagents for read-heavy exploration, verification design, and subsystem notes.
- Keep write-heavy integration in the main thread unless the ownership boundary is narrow and explicit.
- At phase boundaries, spawn fresh bounded explorers instead of accumulating noisy long-lived threads.

## Initial Native Milestones
1. Preserve and document the ROM build oracle.
2. Stand up a native CMake + SDL2 skeleton.
3. Import just enough data to boot a native app with deterministic core state.
4. Reach the first playable vertical slice: boot, title, input, save init, basic map load, player movement, collision, and text.

## Git Workflow
- Never work directly on `master`.
- Keep work on a feature branch.
- Commit small coherent milestones.
- Before pushing or opening a PR, rebase onto the latest `master`.
