# c-programming

C study sources on memory layout, type behavior, portability, and optimization tactics.

## Layout

Directories name the topic, not the platform.

| directory | holds |
|---|---|
| `math/` | numerical algorithms |
| `memory/` | padding, alignment, object layout |
| `pointer/` | pointer arithmetic and raw memory |
| `std/` | C standard library experiments |
| `tricks/` | language corners that read as puzzles |
| `type/` | type widths, ranges, format specifiers |
| `gcc/` | extensions GCC introduced |
| `glibc/` | glibc implementation behavior |
| `msvc/` | MSVC extensions and Win32 |

A toolchain directory names whose extension it is, not who can compile it — clang builds most of `gcc/` too. Empty directories appear when their first file lands.

Binaries go to `bin/<arch>-<os>-<compiler>-<config>/<dir>/`, so every combination coexists. CMake appends the platform suffix, so Windows targets get `.exe`, and MSVC puts the `.pdb` beside it. No build type means Debug.

## Toolchain selection

`CMakeLists.txt` picks no compiler. `project(c-programming C)` leaves it to CMake:

```
CC  ->  cc  ->  gcc  ->  cl  ->  bcc  ->  xlc  ->  icx  ->  clang
```

One name at a time across all of `PATH`, so name order beats directory order: a `clang` first in `PATH` loses to a `cc` or `gcc` last in it. `CC=clang` overrides.

Nothing here prefers a vendor or errors on one. `CMAKE_C_COMPILER_ID` only names the `bin/` directory and selects the MSVC or GCC-style flag set; the gates that abort are OS and architecture. With no compiler at all, the error is CMake's own, raised inside `project()`.

Three entry points pin a toolchain and stop rather than fall back:

| entry point | pinned |
|---|---|
| presets `linux-debug`, `linux-release` | `CMAKE_C_COMPILER=gcc`, over `CC` |
| preset `windows-debug` | generator `Visual Studio 17 2022` |
| `build.cmake` on Windows | first of Visual Studio 2026, 2022, 2019 that configures |

That loop is the only toolchain search written here, and it ranges over MSVC versions, not vendors. `build.sh` and `build.ps1` pin nothing.

## Build types

Every flag is probed with `check_c_compiler_flag` under `-Werror` and dropped if rejected. Both configs also get `-std=c99 -fPIE -Wall -Wextra -pedantic`.

Debug keeps the machine code readable as C: `-O0 -ggdb3 -fno-builtin -fno-inline -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -fno-optimize-sibling-calls -fasynchronous-unwind-tables -fno-stack-protector -fno-eliminate-unused-debug-types -grecord-gcc-switches`, plus `-fkeep-inline-functions -fkeep-static-functions` on gcc and `-fdebug-macro` on clang. `-fno-builtin` stops gcc folding `printf("...\n")` into `puts` at `-O0`; `-ggdb3` and `-fdebug-macro` make `info macro` work; `-mno-omit-leaf-frame-pointer` keeps frame records in leaf functions, which aarch64 omits even at `-O0`.

Release ships what distributions ship: `-O2 -g -fstack-protector-strong -fstack-clash-protection -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -Wformat -Werror=format-security -DNDEBUG`, `-fcf-protection=full` on x86_64, `-mbranch-protection=standard` on aarch64, linked `-Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -Wl,--as-needed`. Fortification is probed separately: `-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3`, else `=2`, else nothing.

`-pg` is not in Debug: it contaminates perf profiles, injects instructions mid-prologue on aarch64 and riscv64, and writes `gmon.out` into the working directory. uftrace needs no flag — `uftrace record -P . <binary>` traces a plain Debug build. gprof does, so it is an option.

| option | effect |
|---|---|
| `-DENABLE_GPROF=ON` | `-pg` on compile and link, every config |
| `-DSANITIZE=address,undefined` | enables those sanitizers |
| `-DENABLE_LTO=ON` | LTO in Release, if `check_ipo_supported` agrees |

`ENABLE_GPROF` and `SANITIZE` emit GCC spelling unprobed, so both are gcc and clang only.

## Portability

Targets are Linux on x86_64, aarch64 and riscv64, and Windows on x86_64. CMake checks the OS, asks the compiler its architecture, and fails on anything else — macOS included — so no per-file platform checks are needed.

Baseline is C99, extensions off. gcc and clang get `-std=c99`; MSVC has no `/std:c99`; its default covers it from 19.27 (VS 2019 16.7), the first to accept `restrict`. Past C99 is opt-in: `__extension__` and `__builtin_*` for GNU, feature-test macros for POSIX and glibc.

| axis | build system | source |
|---|---|---|
| dialect | `DIALECT` tag against the probed set | none |
| libc | `check_symbol_exists(__GLIBC__ features.h)` | none |
| OS | `CMAKE_SYSTEM_NAME` and feature probes | `_POSIX_C_SOURCE`, `_DEFAULT_SOURCE` on line 1 |
| architecture | `check_c_source_compiles` each, fatal if none match | `__x86_64__` / `__aarch64__` / `__riscv` dispatch |

| dialect | predicate |
|---|---|
| `gnu` | `__GNUC__` |
| `gcc` | `__GNUC__` and not `__clang__` |
| `clang` | `__clang__` |
| `msvc` | `_MSC_VER` |

A compiler belongs to every dialect it satisfies: clang to `gnu` and `clang`, clang-cl to `clang` and `msvc`. Satisfying none skips the tagged sources — a smaller build, not a failure.

- The build system decides which files compile. The source decides how it adapts.
- A source guards only where it already branches and one branch has no code, as in `gcc/likely.c` picking an asm body. A guard restating a build-system gate is not written.
- `#error` means "cannot be correct here", never "unrecognized platform". A branch that only picks a printed label falls back to `unknown`, which is why `type/format_string.c` names macOS and the BSDs.
- Compiling one file by hand is the caller's problem.
- Probe the compiler, never match on `CMAKE_SYSTEM_PROCESSOR`.
- Feature-test macros go on line 1.

## Adding a program

Drop the `.c` under its topic directory and add a line to the table in `CMakeLists.txt`. A source not in the table is not built.

```cmake
study_source(memory/foo.c)                                   # portable
study_source(glibc/bar.c WHEN HAVE_GLIBC_LINUX)              # environment
study_source(gcc/baz.c DIALECT gnu WHEN HAVE_CLOCK_GETTIME)  # dialect and feature
```

Probes are cached, so use one build directory per toolchain.

## Build and run

```bash
./build.sh [clean] [debug|release]            # Linux
./build.ps1 [clean] [debug|release]           # Windows
cmake -P build.cmake [clean] [debug|release]  # either, picks the generator

cmake --preset linux-debug && cmake --build out/build/linux-debug

./bin/<arch>-<os>-<compiler>-<config>/<dir>/<name>
```

The three scripts take the same words in any order and case, from any working directory; no word means `debug`. `-h` or `--help` prints the usage; `build.cmake` needs `--` before them, or CMake intercepts them. They configure into `build/`, which `build.cmake` wipes when a cache is present. Presets use `out/build/<preset>/` and define no build presets, hence the two-step line.

Visual Studio 2022+: "Open a local folder" on the root, pick a target from **Select Startup Item**, F5. Breakpoints, Memory View and `.pdb` work without a `.sln`.

## Conventions

UTF-8 without BOM and LF via `.gitattributes`; 4-space indent, hard tabs in Makefiles, `insert_final_newline = false` via `.editorconfig`. `.clang-format` is LLVM-based: right-aligned pointers, Allman function braces, K&R control flow, `{ a, b }` braced initializers, and 100 columns — soft, since `PenaltyExcessCharacter: 1`.

Include guards use the kernel form; `#pragma once` only in Windows-specific code.

```c
#ifndef _MODULE_NAME_H
#define _MODULE_NAME_H

#endif /* _MODULE_NAME_H */
```

`build/`, `out/`, `bin/` and debugger output are gitignored.
