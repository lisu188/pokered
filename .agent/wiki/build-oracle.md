# Build Oracle

## Environment
- Host: Ubuntu 24.04.4 LTS under WSL
- Date validated: 2026-04-09
- Existing host tools: `make`, `gcc`, `git`
- Installed oracle tools:
  - `rgbasm v1.0.1`
  - `rgblink v1.0.1`
  - `rgbfix v1.0.1`
  - `rgbgfx v1.0.1`

## RGBDS Source Of Truth
- Repo docs: `INSTALL.md`
- CI pin: `.github/workflows/main.yml`
- Required version used by CI: `v1.0.1`

## Commands
```bash
make -j"$(nproc)"
sha1sum pokered.gbc pokeblue.gbc pokeblue_debug.gbc
```

## Outputs
- `pokered.gbc` -> `ea9bcae617fdf159b045185467ae58b2e4a48b9a`
- `pokeblue.gbc` -> `d7037c83e1ae5b39bde3c30787637ba1d4c48ce2`
- `pokeblue_debug.gbc` -> `5b1456177671b79b263c614ea0e7cc9ac542e9c4`

These match `README.md`, so the local ROM build is a good oracle baseline.

## Notes
- The Makefile can execute helper-tool builds even during `make -n`, so dry runs are not artifact-free.
- The original build emits many ignored intermediates (`.o`, generated `gfx/*.2bpp`, `.pic`, `.map`, `.sym`, ROMs).
- This oracle should remain buildable while the native port grows alongside it.

## Native Host Build
- Verified local native host toolchain:
  - `cmake`
  - `g++`
  - `pkg-config`
  - `sdl2 2.30.0`
- Verified native commands:
```bash
cmake -S . -B build-native
cmake --build build-native -j"$(nproc)"
ctest --test-dir build-native --output-on-failure
./build-native/pokered_native --smoke-test
```
- Current smoke output:
  - `smoke-ok: world=0 pos=3,2 steps=8`
  - repo-root smoke runs no longer depend on `TMPDIR` being set

## Windows Cross-Build Path From WSL
- Repo support already present:
  - `cmake/toolchains/mingw-w64-x86_64.cmake`
  - top-level `CMakeLists.txt` now links `SDL2::SDL2main` when a MinGW SDL2 package exposes it
- MinGW compiler packages available from Ubuntu:
  - `binutils-mingw-w64-x86-64`
  - `gcc-mingw-w64-x86-64-posix`
  - `g++-mingw-w64-x86-64-posix`
  - `mingw-w64-x86-64-dev`
- Local blocker:
  - no `x86_64-w64-mingw32-g++` installed yet
  - no MinGW SDL2 development package with `SDL2Config.cmake` installed yet
- Practical WSL recipe to verify once packages are available:
```bash
sudo apt install \
  binutils-mingw-w64-x86-64 \
  gcc-mingw-w64-x86-64-posix \
  g++-mingw-w64-x86-64-posix \
  mingw-w64-x86-64-dev

cmake -S . -B build-mingw -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$PWD/cmake/toolchains/mingw-w64-x86_64.cmake" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="/path/to/mingw-sdl2"

cmake --build build-mingw -j"$(nproc)"
```
- Expected output path once SDL2 is available:
  - `build-mingw/pokered_native.exe`
