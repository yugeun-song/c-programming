# c-programming

C study sources on memory layout, type behavior, portability, and optimization tactics.

## Layout

Directories name the topic, not the platform; one appears with its first file.

| directory | holds |
|---|---|
| `math/` | numerical algorithms |
| `memory/` | padding, alignment, object layout |
| `pointer/` | pointer arithmetic and raw memory |
| `std/` | C standard library experiments |
| `tricks/` | language corners that read as puzzles |
| `type/` | type widths, ranges, format specifiers |
| `gcc/` | extensions GCC introduced; clang builds most of them |
| `glibc/` | glibc implementation behavior |
| `msvc/` | MSVC extensions and Win32 |

| build file | role |
|---|---|
| `CMakeLists.txt` | configs and the source table |
| `cmake/platform.cmake` | compiler, OS, architecture and libc checks |
| `cmake/flags.cmake` | flag tables, probing, options |
| `CMakePresets.json` | one configure preset per target |
| `build.cmake` | build command; `build.sh` and `build.ps1` forward to it |
| `cmake/targets.cmake` | preset lookup and availability for `build.cmake` |

Programs go to `bin/<arch>-<os>-<compiler>-<config>/<dir>/`, with `.exe` and `.pdb` on Windows.

## Targets

| preset | host | compiler | runtime | generator |
|---|---|---|---|---|
| `x86_64-linux-gcc` | Linux | `gcc` | glibc | Ninja Multi-Config |
| `x86_64-linux-clang` | Linux | `clang` | glibc | Ninja Multi-Config |
| `aarch64-linux-gcc` | Linux | `aarch64-linux-gnu-gcc` | glibc | Ninja Multi-Config |
| `aarch64-linux-clang` | Linux | `clang --target=aarch64-linux-gnu`, `lld` | glibc | Ninja Multi-Config |
| `riscv64-linux-gcc` | Linux | `riscv64-linux-gnu-gcc` | glibc | Ninja Multi-Config |
| `riscv64-linux-clang` | Linux | `clang --target=riscv64-linux-gnu`, `lld` | glibc | Ninja Multi-Config |
| `x86_64-windows-gcc` | Linux, Windows | `x86_64-w64-mingw32-gcc` | MinGW-w64 | Ninja Multi-Config |
| `x86_64-windows-msvc` | Windows | `cl` | UCRT | newest Visual Studio |
| `x86_64-windows-clang` | Windows | `clang-cl`, toolset `ClangCL` | UCRT | newest Visual Studio |

`-h` lists which presets the host can build and what the others miss. Clang cross presets reuse the gcc cross sysroot under `/usr/<triple>`. Presets blank `CFLAGS`, `LDFLAGS`, `CMAKE_TOOLCHAIN_FILE`, `CL`, `_CL_`, `LINK` and `_LINK_` and neutralize `CCC_OVERRIDE_OPTIONS`; `build.cmake` applies that environment to the build step too.

Configure stops unless the compiler is GCC, Clang or MSVC, and a compiler CMake calls GNU must report through `-dumpfullversion` the version its macros carry. It also stops when the detected `<arch>-<os>-<compiler>` differs from the preset name.

## Build types

`cmake/flags.cmake` has a candidate table per config and command-line syntax, `gnu` or `msvc`. Each flag is probed in its own config under `-Werror` (`/WX`), GNU link flags under `-Wl,--fatal-warnings`, after the flags accepted before it; configure lists what it drops. Probes rerun when the compiler file, target, sysroot or base flags change. Every config gets `-std=c99 -Wall -Wextra -pedantic` and PIE, or `/utf-8 /W4 /D_CRT_SECURE_NO_WARNINGS`. A cached `CMAKE_C_FLAGS_<CONFIG>` is ignored with a warning; extra flags go in `CMAKE_C_FLAGS`.

Debug keeps the machine code a literal translation of the C: `-O0 -ggdb3 -fno-inline -fno-omit-frame-pointer -fno-optimize-sibling-calls -fasynchronous-unwind-tables -fno-eliminate-unused-debug-types -grecord-gcc-switches`, `-fdebug-macro` on clang for `info macro`, and:

| flag | removes |
|---|---|
| `-fno-builtin` | `printf("...\n")` turned into `puts`, `memset` expanded inline |
| `-ffp-contract=off` | `a * b + c` fused into `fmadd`, as clang does at `-O0` on aarch64 and riscv64 |
| `-fno-stack-protector -fno-stack-clash-protection -fcf-protection=none -mbranch-protection=none -U_FORTIFY_SOURCE` | canaries, stack probes, `endbr64`, PAC/BTI and `__*_chk` calls that toolchains add by default: the canary on Arch gcc, all but PAC/BTI on Ubuntu gcc |
| `-mno-relax` | riscv64 linker relaxation of `auipc`+`jalr` into `jal` |
| `-mno-omit-leaf-frame-pointer` | frameless leaf functions, which aarch64 emits at `-O0` |
| `-fkeep-inline-functions -fkeep-static-functions` (gcc), `-fkeep-persistent-storage-variables` (clang) | unused static functions and variables dropped from the binary |

MSVC syntax gets `/Od /Ob0 /Zi /GS-`, linked `/DEBUG /INCREMENTAL:NO`: no cookies, `/RTC1` checks, `/JMC` calls or incremental-link thunks. No flag stops gcc from strength-reducing `x / 3` and `x * 5` at `-O0`, or either compiler from folding constants and dropping `if (0)` bodies.

Release builds what distributions ship, at `-O2` with LTO: `-O2 -g -fasynchronous-unwind-tables -fstack-protector-strong -fstack-clash-protection -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -Wformat -Werror=format-security -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -DNDEBUG`, `-fcf-protection=full` on x86_64, `-mbranch-protection=standard` on aarch64, linked `-Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -Wl,-z,pack-relative-relocs -Wl,--as-needed`; riscv64 ld.bfd rejects `pack-relative-relocs`. LTO is `-flto=auto -fno-fat-lto-objects` on gcc and `-flto=thin` on clang; a toolchain failing `check_ipo_supported` warns and links without it. MSVC syntax gets `/O2 /Zi /sdl /GS /guard:cf /DNDEBUG` with CMake's `/GL`, linked `/DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF /guard:cf /CETCOMPAT /LTCG`; clang-cl accepts `/sdl` but ignores it.

`OPTIMIZED` in the source table adds `-O2` (`/O2`) to one source in every config; `memory/strict_pointer.c` needs it, since its `restrict` violation is invisible at `-O0`.

`-pg` stays out of Debug: it skews perf profiles, lands mid-prologue on aarch64 and riscv64, and writes `gmon.out` into the working directory. uftrace needs no flag: `uftrace record -P . -a <binary>` patches a Debug build at load time and reads arguments from DWARF.

| option | effect |
|---|---|
| `-DENABLE_GPROF=ON` | `-pg` on compile and link, every config |
| `-DSANITIZE=address,undefined` | those sanitizers, every config |
| `-DENABLE_LTO=OFF` | Release without LTO |

Configure stops if the compiler rejects `ENABLE_GPROF` or `SANITIZE` in any config.

## Portability

Targets are Linux on x86_64, aarch64 and riscv64, and Windows on x86_64. CMake checks the OS, asks the compiler its architecture, and fails on anything else, macOS included.

Baseline is C99, extensions off: `-std=c99` on gcc and clang; MSVC has no `/std:c99`, and its default covers C99 from 19.27 (VS 2019 16.7), the first to accept `restrict`. Past C99 is opt-in: `__extension__` and `__builtin_*` for GNU, feature-test macros for POSIX and glibc.

| axis | build system | source |
|---|---|---|
| dialect | `DIALECT` tag against the compiler's dialects | none |
| libc | `check_symbol_exists(__GLIBC__ features.h)` | none |
| OS | `CMAKE_SYSTEM_NAME` and feature probes | `_POSIX_C_SOURCE`, `_DEFAULT_SOURCE` on line 1 |
| architecture | one `check_c_source_compiles` each, fatal unless exactly one matches | `__x86_64__` / `__aarch64__` / `__riscv` dispatch |

| dialect | compilers |
|---|---|
| `gnu` | gcc, clang |
| `gcc` | gcc |
| `clang` | clang, clang-cl |
| `msvc` | MSVC, clang-cl |

A compiler's dialects are its name and its command-line syntax; a source tagged with another dialect is skipped, not failed.

- The build system decides which files compile. The source decides how it adapts.
- A source guards only where it already branches and one branch has no code, as in `gcc/likely.c` picking an asm body. A guard restating a build-system gate is not written.
- `#error` means "cannot be correct here", never "unrecognized platform". A branch that only picks a printed label falls back to `unknown`, which is why `type/format_string.c` names macOS and the BSDs.
- Compiling one file by hand is the caller's problem.
- Probe the compiler, never match on `CMAKE_SYSTEM_PROCESSOR`.
- Feature-test macros go on line 1.

## Adding a program

Drop the `.c` under its topic directory and add a line to the source table in `CMakeLists.txt`; nothing is globbed.

```cmake
study_source(memory/foo.c)                                   # portable
study_source(glibc/bar.c WHEN HAVE_GLIBC)                    # environment
study_source(gcc/baz.c DIALECT gnu WHEN HAVE_CLOCK_GETTIME)  # dialect and feature
study_source(memory/qux.c OPTIMIZED)                         # -O2 in every config
```

Every argument must parse: a path other than `<topic>/<name>.c`, an unknown, repeated or empty keyword or value, or a condition that is not a cached probe stops configure. Use one build directory per preset.

## Build and run

```bash
./build.sh [clean] [debug|release] [target]               # Linux
./build.ps1 [clean] [debug|release] [target]              # Windows
cmake -P build.cmake -- [clean] [debug|release] [target]  # either

cmake --preset aarch64-linux-gcc && cmake --build build/aarch64-linux-gcc --config Release
qemu-aarch64 -L /usr/aarch64-linux-gnu ./bin/aarch64-linux-gcc-release/<dir>/<name>
```

All three entry points parse the same words, from any working directory: any order and case, each kind once. Anything else, or a target the host cannot build, stops before any work. No word means `debug` and the host default, `<arch>-linux-gcc` on Linux or `x86_64-windows-msvc` on Windows. A target configures into `build/<target>/` and builds either config there; `clean` first deletes that tree, refusing one without `CMakeCache.txt`, and the target's `bin/` directories.

Visual Studio 2022+: "Open a local folder" on the root, choose `x86_64-windows-msvc` or `x86_64-windows-clang` from the preset list, pick a program from **Select Startup Item**, F5. Breakpoints, Memory View and `.pdb` work without a `.sln`.

## Conventions

UTF-8 without BOM and LF via `.gitattributes`; 4-space indent, hard tabs in Makefiles and no final newline via `.editorconfig`. `.clang-format` is LLVM-based: right-aligned pointers, Allman function braces, K&R control flow, `{ a, b }` braced initializers, 100 soft columns (`PenaltyExcessCharacter: 1`).

Include guards use the kernel form; `#pragma once` only in Windows-specific code.

```c
#ifndef _MODULE_NAME_H
#define _MODULE_NAME_H

#endif /* _MODULE_NAME_H */
```