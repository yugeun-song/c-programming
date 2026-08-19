# c-programming

C study sources on memory layout, type behavior, portability, and optimization tactics.

## Layout

Directories name the topic, not the platform.

| directory | holds |
|---|---|
| `math/` | numerical algorithms |
| `memory/` | padding, alignment, and object layout |
| `pointer/` | pointer arithmetic and raw memory |
| `std/` | C standard library experiments |
| `type/` | type widths, ranges, format specifiers |
| `gcc/` | extensions GCC introduced |
| `glibc/` | glibc implementation behavior |
| `msvc/` | MSVC extensions and Win32 |

A toolchain directory names whose extension it is, not who can compile it. Clang implements many GCC extensions, so most of `gcc/` builds under clang too; what a source actually requires is recorded by its `DIALECT` tag. Directories with no source yet are created when their first file lands.

Binaries mirror the tree into `bin/<arch>-<os>-<compiler>-<config>/<dir>/`, one per source, so every combination coexists instead of overwriting the last one. MSVC puts the `.pdb` beside it. Configuring without a build type gives Debug.

## Build types

Every flag is probed with `check_c_compiler_flag` and dropped when the compiler rejects it, so gcc, clang and the cross toolchains each get what they actually accept.

Debug keeps the machine code readable as C and every tool fully informed: `-O0 -ggdb3 -fno-builtin -fno-inline -fno-omit-frame-pointer -fno-optimize-sibling-calls -fasynchronous-unwind-tables -fno-stack-protector -fno-eliminate-unused-debug-types`, plus `-fkeep-inline-functions -fkeep-static-functions -grecord-gcc-switches` on gcc and `-fdebug-macro` on clang. `-fno-builtin` is what stops gcc folding `printf("...\n")` into `puts` at `-O0`; `-ggdb3` and `-fdebug-macro` are what make `info macro` work.

Release ships what distributions ship: `-O2 -g -D_FORTIFY_SOURCE=3 -fstack-protector-strong -fstack-clash-protection -fno-omit-frame-pointer -Wformat -Werror=format-security`, with `-fcf-protection=full` on x86_64 and `-mbranch-protection=standard` on aarch64, linked `-Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -Wl,--as-needed`.

`-pg` is not in Debug. It contaminates perf profiles, injects instructions mid-prologue on aarch64 and riscv64, and writes `gmon.out` into the working directory. uftrace needs no instrumentation flag: `uftrace record -P . <binary>` traces a plain Debug build. gprof does need it, so it lives behind an option.

| option | effect |
|---|---|
| `-DENABLE_GPROF=ON` | adds `-pg` to compile and link |
| `-DSANITIZE=address,undefined` | enables those sanitizers |
| `-DENABLE_LTO=ON` | link-time optimization in Release |

## Portability

Supported targets are Linux on x86_64, aarch64 and riscv64, and Windows on x86_64. CMake checks the target OS, asks the compiler which architecture it is building for, and fails configuration on any other combination, so no per-file platform checks are needed.

Baseline is C99 with compiler extensions off. gcc and clang get `-std=c99`; MSVC has no `/std:c99` switch, so CMake emits no flag and its default mode covers it. Anything past C99 is opted into explicitly: GNU extensions through `__extension__` and `__builtin_*`, POSIX and glibc through feature-test macros.

Three classes of dependency, each gated where it can actually be decided.

| axis | build system | source |
|---|---|---|
| dialect | `DIALECT` tag matched against the probed dialect set | none |
| libc | `check_symbol_exists(__GLIBC__ features.h)` | none |
| OS | `CMAKE_SYSTEM_NAME` and feature probes | `_POSIX_C_SOURCE`, `_DEFAULT_SOURCE` on line 1 |
| architecture | `check_c_source_compiles` per architecture, fatal if none match | `__x86_64__` / `__aarch64__` / `__riscv` dispatch, `#else` `#error` only where no code exists |

A compiler belongs to every dialect it satisfies, so clang answers to both `gnu` and `clang`, and clang-cl to both `clang` and `msvc`.

| dialect | predicate |
|---|---|
| `gnu` | `__GNUC__` |
| `gcc` | `__GNUC__` and not `__clang__` |
| `clang` | `__clang__` |
| `msvc` | `_MSC_VER` |

- The build system decides which files compile. The source decides how it adapts.
- A source carries a preprocessor guard only where it already branches on its own and one branch has no code to run, as in `gcc/likely.c` selecting an inline asm body. A guard that only restates a build-system gate is not written.
- `#error` means "cannot be correct here", never "unrecognized platform". A branch that only picks a printed label falls back to `unknown` instead.
- Compiling one file by hand outside CMake is the caller's responsibility. The sources do not defend against it.
- Probe the compiler, never match on `CMAKE_SYSTEM_PROCESSOR`.
- Feature-test macros go on line 1, ahead of every `#include`.

## Adding a program

Drop the `.c` under its topic directory and add one line to the table in `CMakeLists.txt`:

```cmake
study_source(memory/foo.c)                                   # portable
study_source(glibc/bar.c WHEN HAVE_GLIBC_LINUX)              # environment
study_source(gcc/baz.c DIALECT gnu WHEN HAVE_CLOCK_GETTIME)  # dialect and feature
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
