# Pokémon Red and Blue [![Upstream ROM build][ci-badge]][ci] [![Fork native CI][native-ci-badge]][native-ci]

This is a disassembly of Pokémon Red and Blue.

It builds the following ROMs:

- Pokemon Red (UE) [S][!].gb `sha1: ea9bcae617fdf159b045185467ae58b2e4a48b9a`
- Pokemon Blue (UE) [S][!].gb `sha1: d7037c83e1ae5b39bde3c30787637ba1d4c48ce2`
- BLUEMONS.GB (debug build) `sha1: 5b1456177671b79b263c614ea0e7cc9ac542e9c4`
- dmgapae0.e69.patch `sha1: 0fb5f743696adfe1dbb2e062111f08f9bc5a293a`
- dmgapee0.e68.patch `sha1: ed4be94dc29c64271942c87f2157bca9ca1019c7`

To set up the repository, see [**INSTALL.md**](INSTALL.md).


## Native C++ port in this fork

This fork also contains a C++20/SDL2 native port with CMake 3.24 or newer.
Its current playable slice connects the Red house floors, Pallet Town,
Blue house, Oak lab and the Pallet-side Route 1 boundary. Battles, the
complete Oak/starter sequence and the full game remain unfinished; see
[the current execution plan](.agent/EXECPLAN.md).

The [fork native workflow][native-ci] runs on each pull request and master
push. It builds the pinned RGBDS v1.0.1 oracle, compares every output against
`roms.sha1`, then supplies that same run's `pokered.sym` and `pokered.map`
to GCC/Clang Debug/Release CMake builds. CTest runs the native regression
binary and scripted smoke route, and fails if no tests are registered.
The workflow retains hash-check output, symbols/map and test reports for
seven days; it does not upload ROM binaries. These checks establish the
bounded source/import/save/route contracts covered by the tests. They do
not establish full-game behavioral parity or interactive visual acceptance.

For a local build, first install RGBDS v1.0.1 and run `make DEBUG=1 compare`
to generate and verify the oracle files. With SDL2 development headers,
pkg-config and a C++20 compiler installed:

```sh
cmake -S . -B build-native -DPOKERED_NATIVE_BUILD_TESTS=ON
cmake --build build-native --parallel 2
ctest --test-dir build-native --no-tests=error --output-on-failure
```

The upstream badge above describes pret/pokered's ROM build. The fork badge
describes this repository's native workflow.

## See also

- [**Wiki**][wiki] (includes [tutorials][tutorials])
- [**Symbols**][symbols]
- [**Tools**][tools]

You can find us on [Discord (pret, #pokered)](https://discord.gg/d5dubZ3).

For other pret projects, see [pret.github.io](https://pret.github.io/).

[wiki]: https://github.com/pret/pokered/wiki
[tutorials]: https://github.com/pret/pokered/wiki/Tutorials
[symbols]: https://github.com/pret/pokered/tree/symbols
[tools]: https://github.com/pret/gb-asm-tools
[ci]: https://github.com/pret/pokered/actions
[ci-badge]: https://github.com/pret/pokered/actions/workflows/main.yml/badge.svg

[native-ci]: https://github.com/lisu188/pokered/actions/workflows/native.yml
[native-ci-badge]: https://github.com/lisu188/pokered/actions/workflows/native.yml/badge.svg
