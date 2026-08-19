# c-programming

C study sources on memory layout, type behavior, portability, and optimization tactics.

## Layout

Directories name the topic, not the platform.

| directory | holds |
|---|---|
| `math/` | numerical algorithms |
| `memory/` | padding, alignment, heap behavior |
| `pointer/` | pointer arithmetic and raw memory |
| `std/` | C standard library experiments |
| `type/` | type widths, ranges, format specifiers |
| `gnu/` | GNU C dialect, compiled by both gcc and clang |
| `msvc/` | MSVC dialect |

Directories with no source yet are created when their first file lands.

Binaries mirror the tree into `bin/<arch>-<os>-<compiler>-<config>/<dir>/`, so every combination coexists instead of overwriting the last one. Configuring without a build type gives Debug.

GCC and Clang emit two binaries per source: `<name>`, built with `-ggdb3 -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fasynchronous-unwind-tables -pg` for GDB, gprof, uftrace and perf, and `<name>_opt` at `-O2`. MSVC emits one, with its `.pdb` beside it.

## Portability

Supported platforms are Linux and Windows on x86_64, aarch64 and riscv64. CMake checks the target OS and asks the compiler which architecture it is building for, failing configuration on anything else, so no per-file platform checks are needed.

Baseline is C99 with compiler extensions off. gcc and clang get `-std=c99`; MSVC has no `/std:c99` switch, so CMake emits no flag and its default mode covers it. Anything past C99 is opted into explicitly: GNU extensions through `__extension__` and `__builtin_*`, POSIX and glibc through feature-test macros.

Three classes of dependency, each gated where it can actually be decided.

| axis | build system | source |
|---|---|---|
| dialect | `DIALECT gnu` / `msvc` selects the file | none |
| libc | `check_symbol_exists(__GLIBC__ features.h)` | none |
| OS | `CMAKE_SYSTEM_NAME` and feature probes | `_POSIX_C_SOURCE`, `_DEFAULT_SOURCE` on line 1 |
| architecture | `check_c_source_compiles` per architecture, fatal if none match | `__x86_64__` / `__aarch64__` / `__riscv` dispatch, `#else` `#error` only where no code exists |

- The build system decides which files compile. The source decides how it adapts.
- A source carries a preprocessor guard only where it already branches on its own and one branch has no code to run, as in `gnu/likely.c` selecting an inline asm body. A guard that only restates a build-system gate is not written.
- `#error` means "cannot be correct here", never "unrecognized platform". A branch that only picks a printed label falls back to `unknown` instead.
- Compiling one file by hand outside CMake is the caller's responsibility. The sources do not defend against it.
- Probe the compiler, never match on `CMAKE_SYSTEM_PROCESSOR`.
- Feature-test macros go on line 1, ahead of every `#include`.

## Adding a program

Drop the `.c` under its topic directory and add one line to the table in `CMakeLists.txt`:

```cmake
study_source(memory/foo.c)                                   # portable
study_source(memory/bar.c WHEN HAVE_GLIBC_LINUX)             # environment
study_source(gnu/baz.c DIALECT gnu WHEN HAVE_CLOCK_GETTIME)  # dialect and feature
```

Probe results are cached, so use a separate build directory per toolchain.

## Build and run

```bash
./build.sh [clean] [debug|release]        # Linux, gcc or clang
./build.ps1 [-Clean] [-Configuration ..]  # Windows, MSVC
cmake [-DCONFIG=Release] -P build.cmake   # either, picks the generator itself

./bin/<arch>-<os>-<compiler>-<config>/<dir>/<name>
```

Visual Studio 2022+: "Open a local folder" on the repository root, pick a target from **Select Startup Item**, then F5. Breakpoints, Memory View and `.pdb` symbols work without a `.sln`.

## Conventions

UTF-8 without BOM, LF endings via `.gitattributes`, 4-space indentation with 4-width hard tabs for Makefiles via `.editorconfig`, and an LLVM-based `.clang-format` with right-aligned pointers, Allman function braces, K&R control flow, and a 100-column limit.

Include guards use the kernel form. `#pragma once` is allowed only in Windows-specific code, which otherwise follows Windows conventions.

```c
#ifndef _MODULE_NAME_H
#define _MODULE_NAME_H

#endif /* _MODULE_NAME_H */
```

`build/`, `bin/` and debugger output are gitignored.
