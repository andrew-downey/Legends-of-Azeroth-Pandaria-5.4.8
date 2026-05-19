# Legends of Azeroth -- Pandaria 5.4.8

WoW Mists of Pandaria 5.4.8 server emulator (TrinityCore/SkyFire fork). C++20, CMake.

## Build & develop

```
./build.sh configure          # cmake - Release, -DTOOLS=0
./build.sh configure --debug  # Debug build
./build.sh build              # cmake --build -> worldserver + authserver
./build.sh install            # installs to $HOME/warcraft-server (backups old as *_old)
./build.sh restore            # restore previous binaries
```

Manual CMake equivalent:
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DTOOLS=0
cmake --build build --target worldserver authserver -j$(nproc)
```

CI reference builds:
- **Linux GCC** `-DTOOLS=1 -DELUNA=0`, `make -j4 -k`
- **macOS arm64** `cmake -GNinja -B build -DWITH_WARNINGS=1`, `ninja`
- **Windows** `cmake .. -DTOOLS=ON -DELUNA=1 -A x64`, `cmake --build . --config RelWithDebInfo`

## Key CMake options (all in `cmake/options.cmake`)

| Option | Default | What |
|---|---|---|
| `TOOLS` | OFF | map/vmap/mmap extraction tools |
| `ELUNA` | ON | Lua scripting engine |
| `PLAYERBOTS` | ON | AI player bots module |
| `SCRIPTS` | ON | Scripted content |
| `USE_COREPCH` | ON | Precompiled headers for servers |
| `WITH_SANITIZER` | OFF | AddressSanitizer (GCC only) |
| `NOJEM` | (implicit OFF) | Disable jemalloc (use for valgrind) |

**Disable ELUNA when PLAYERBOTS is enabled** (they conflict). README warns this explicitly.

## No tests, no lint, no typecheck

Compilation passing CI is the only quality gate. No unit test framework is configured (CTest exists but unused). No `.clang-format` or `.clang-tidy`.

Code standards (`doc/code_standards.md`): Oracle/Sun style, 4-space indent, no tabs, no trailing whitespace, LF line endings, squash PR commits.

## Quirks

- **CMake configure modifies source files.** `ModulesLoader.cpp` is auto-generated during configure and written back to `modules/ModulesLoader.cpp` in the source tree.
- **`revision.h` is auto-generated** at configure time by `cmake/genrev.cmake` (from `revision.h.in.cmake`). Uses `hg` (Mercurial) commands; falls back to defaults if `hg` unavailable.
- **Config files use `.dist` extension** (e.g., `worldserver.conf.dist`, `authserver.conf.dist`). Copy to same name without `.dist` to customize.
- **Playerbots** is early-stage, may crash. Requires `enUS` DBC, a `playerbots.conf`, and specific `worldserver.conf` entries (see README).
- **Install prefix** defaults to `$HOME/warcraft-server` via `build.sh`, or `/server/wow/horizon` when `BUILD_DEPLOY=ON` and not `WITH_COREDEBUG`.

## Database

Three databases: `auth`, `characters`, `world` + optional `playerbots`. Base schemas in `sql/base/`, incremental updates in `sql/updates/`.

## Sources of information

- [GitHub repo](https://github.com/Legends-of-Azeroth/Legends-of-Azeroth-Pandaria-5.4.8) — issues, PRs, CI logs
- [Discord](https://discord.gg/byBCHbwJEg)
- `doc/code_standards.md` — coding conventions
- `src/server/worldserver/worldserver.conf.dist` — all server config options
