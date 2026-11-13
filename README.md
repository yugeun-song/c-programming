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
| `CMakeLists.txt` | configs and the program list |
| `cmake/platform.cmake` | compiler, OS, architecture and libc checks |
| `cmake/flags.cmake` | flag lists, probing, options |
| `CMakePresets.json` | one configure preset per target |
| `build.cmake` | build command; `build.sh`, `build.ps1` and the `Makefile` forward to it |
| `cmake/presets.cmake` | preset lookup and availability for `build.cmake` |

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
| `x86_64-windows-msvc` | Windows | `cl` | UCRT | newest Visual Studio |
| `x86_64-windows-clang` | Windows | `clang-cl`, toolset `ClangCL` | UCRT | newest Visual Studio |

`-h` lists which presets the host can build and what the others miss. Clang cross presets reuse the gcc cross sysroot under `/usr/<triple>`. Presets blank `CFLAGS`, `LDFLAGS`, `CMAKE_TOOLCHAIN_FILE`, `CL`, `_CL_`, `LINK` and `_LINK_` and neutralize `CCC_OVERRIDE_OPTIONS`; `build.cmake` applies that environment to the build step too.

Configure stops unless the compiler is GCC, Clang or MSVC, and a compiler CMake calls GNU must report through `-dumpfullversion` the version its macros carry. It also stops when the detected `<arch>-<os>-<compiler>` differs from the preset name, and when a Linux target is built by anything but GCC or Clang or a Windows target by anything but MSVC or clang-cl.

## Build types

`cmake/flags.cmake` has a candidate flag list per config and command-line syntax, `gnu` or `msvc`. Each flag is probed in its own config under `-Werror` (`/WX`), GNU link flags under `-Wl,--fatal-warnings`, after the flags accepted before it; configure lists what it drops. Probes rerun when the compiler file, target, sysroot or base flags change. Every config gets `-std=c99 -Wall -Wextra -pedantic -Wshadow -Wformat -Werror=format-security`, or `/utf-8 /W4 -Wshadow /clang:-pedantic /Zc:preprocessor /D_CRT_SECURE_NO_WARNINGS`: cl finds shadowing at `/W4` (C4456 to C4459) and has no `-pedantic` for C, clang-cl takes `-Wshadow` and `-pedantic`, and `/Zc:preprocessor` gives cl the C99 preprocessor. The configs are Debug, Sanitize and Release. A cached `CMAKE_C_FLAGS_<CONFIG>` is ignored with a warning; extra flags go in `CMAKE_C_FLAGS`.

Debug keeps the machine code a literal translation of the C: `-O0 -ggdb3 -fno-inline -fno-omit-frame-pointer -fno-optimize-sibling-calls -fasynchronous-unwind-tables -fno-eliminate-unused-debug-types -grecord-gcc-switches -fno-pie`, `-fdebug-macro` on clang for `info macro`, linked `-no-pie -rdynamic -Wl,--build-id`, and:

| flag | removes |
|---|---|
| `-fno-builtin` | `printf("...\n")` turned into `puts`, `memset` expanded inline |
| `-ffp-contract=off` | `a * b + c` fused into `fmadd`, as clang does at `-O0` on aarch64 and riscv64 |
| `-fno-stack-protector -fno-stack-clash-protection -fcf-protection=none -mbranch-protection=none -U_FORTIFY_SOURCE` | canaries, stack probes, `endbr64`, PAC/BTI and `__*_chk` calls that toolchains add by default: the canary on Arch gcc, all but PAC/BTI on Ubuntu gcc |
| `-mno-relax` | riscv64 linker relaxation of `auipc`+`jalr` into `jal` |
| `-mno-omit-leaf-frame-pointer` | frameless leaf functions, which aarch64 emits at `-O0` |
| `-fkeep-inline-functions -fkeep-static-functions` (gcc), `-fkeep-persistent-storage-variables` (clang) | unused static functions and variables dropped from the binary |

MSVC syntax gets `/Od /Ob0 /Oi- /Zi /JMC- /GS-`, linked `/DEBUG /INCREMENTAL:NO /DYNAMICBASE:NO`: no cookies, intrinsics, `/RTC1` checks, `/JMC` calls or incremental-link thunks. clang-cl also gets `/clang:-fno-omit-frame-pointer`, since it omits the frame pointer at `/Od` on x86_64, `/clang:-ffp-contract=off`, `-fno-eliminate-unused-debug-types` and `/clang:-fkeep-persistent-storage-variables`; x64 cl has no frame-pointer option. No flag stops gcc from strength-reducing `x / 3` and `x * 5` at `-O0`, or either compiler from folding constants and dropping `if (0)` bodies.

Without PIE the addresses `objdump` and `nm` print are the ones the process runs at; `-rdynamic` puts every function in the dynamic symbol table, where `backtrace()` and `dladdr()` find it; the build ID lets perf and debuginfod match the binary to its debug information. `/DYNAMICBASE:NO` keeps the addresses of `dumpbin` and the PDB, which names every function, and the PDB GUID in the CodeView debug directory plays the build ID's part. Debug and Sanitize also compile and link every program with `-pg` for gprof and uftrace, except a source marked `NO_GPROF`; MSVC has no counterpart. `-pg` adds a call to every function and allocates the profiling buffers on the heap at startup, so `glibc/heap_fragmentation.c`, which measures how much freed heap a later request reuses, is marked `NO_GPROF`: with `-pg` its probe reused 0 bytes instead of 1888. It also skews perf profiles, lands mid-prologue on aarch64 and riscv64, makes gdb stop at `break <function>` before the arguments are stored, and writes `gmon.out` into the working directory.

Sanitize is Debug plus `-fsanitize=<SANITIZE> -fno-sanitize-recover=all`, `address` by default, or `/fsanitize=address -fno-sanitize-recover=all` in MSVC syntax; cl takes only `address` and drops the second flag, because its runtime already stops at the first error. Debug links the debug CRT (`/MDd`), Sanitize and Release `/MD`, which clang-cl's AddressSanitizer requires. A toolchain that cannot link the sanitizer runtime, such as the cross toolchains on a plain Linux host, gets the config skipped with a note.

Release builds what distributions ship, at `-O2` with link-time optimization (LTO): `-O2 -g -fasynchronous-unwind-tables -fstack-protector-strong -fstack-clash-protection -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -DNDEBUG -fPIE`, `-fcf-protection=full` on x86_64, `-mbranch-protection=standard` on aarch64, linked `-pie -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -Wl,-z,pack-relative-relocs -Wl,--as-needed -Wl,--build-id`; riscv64 ld.bfd rejects `pack-relative-relocs`. LTO is `-flto=auto -fno-fat-lto-objects` on gcc and `-flto=thin` on clang; a toolchain failing `check_ipo_supported` warns and links without it. MSVC syntax gets `/O2 /Zi /sdl /GS /guard:cf /DNDEBUG` with CMake's `/GL` (`-flto=thin` on clang-cl), linked `/DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF /guard:cf /CETCOMPAT /DYNAMICBASE /HIGHENTROPYVA /NXCOMPAT /LTCG`, and clang-cl `/clang:-fno-omit-frame-pointer`; clang-cl accepts `/sdl` but ignores it. Fortify, RELRO, `pack-relative-relocs` and `-Werror=format-security` have no MSVC counterpart.

`OPTIMIZED` in the program list adds `-O2` (`/O2`) to one source in every config; `memory/strict_pointer.c` needs it, since its `restrict` violation is invisible at `-O0`. `NO_GPROF` leaves `-pg` out of one source.

| option | effect |
|---|---|
| `-DSANITIZE=address,undefined` | the sanitizers of the Sanitize config, `address` by default |
| `-DENABLE_LTO=OFF` | Release without LTO |

## Portability

Targets are Linux on x86_64, aarch64 and riscv64 with GCC or Clang, and Windows on x86_64 with MSVC or clang-cl. CMake checks the OS, asks the compiler its architecture, and fails on anything else, macOS included.

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

Drop the `.c` under its topic directory and add a line to the program list in `CMakeLists.txt`; nothing is globbed.

```cmake
add_program(memory/foo.c)                                    # portable
add_program(glibc/bar.c WHEN HAVE_GLIBC)                     # environment
add_program(gcc/baz.c DIALECT gnu WHEN HAVE_CLOCK_GETTIME)   # dialect and feature
add_program(memory/qux.c OPTIMIZED)                          # -O2 in every config
```

Every argument must parse: a path other than `<topic>/<name>.c`, an unknown, repeated or empty keyword or value, or a condition that is not a cached probe stops configure. Use one build directory per preset.

## Build and run

```bash
make [command ...] [PRESET=<preset>] [CC=gcc|clang] [PROFILE=debug|release] [SANITIZE=<list>]   # Linux
./build.sh [command ...] [preset] [PROFILE=debug|release] [SANITIZE=<list>]                       # Linux
./build.ps1 [command ...] [preset] [PROFILE=debug|release] [SANITIZE=<list>]                      # Windows
cmake -P build.cmake -- [command ...] [preset] [PROFILE=debug|release] [SANITIZE=<list>]          # either

cmake --preset aarch64-linux-gcc && cmake --build build/aarch64-linux-gcc --config Release
qemu-aarch64 -L /usr/aarch64-linux-gnu ./bin/aarch64-linux-gcc-release/<dir>/<name>
```

| command | effect |
|---|---|
| `all` | `debug` and `sanitize`, or `release` with `PROFILE=release`; the default |
| `debug`, `sanitize`, `release` | configure `build/<preset>/` and build that config into `bin/<preset>-<config>/` |
| `index` | `compile_commands.json` with the entries of the `PROFILE` config, `tags` and `cscope.out`, in the root; later builds rewrite the `compile_commands.json` when its flags change |
| `clean-index` | remove the index files |
| `clean` | remove `bin/` and the build products of every configured tree; the configuration stays |
| `distclean` | `clean`, `clean-index`, `build/`, tool outputs (`gmon.out`, `perf.data`, `uftrace.data`, core files) and `.cache/` |
| `unused` | unused diagnostics of every source (clang) and unused functions (cppcheck), with the `PROFILE` definitions |
| `check` | build every config the preset can link and run its tests |
| `tidy` | clang-tidy over every source with the `PROFILE` flags |
| `clangd-check` | clangd parses every source and header and prints its errors |
| `help`, `-h` | the commands, the variables and the presets the host can build |

All four entry points parse the same arguments, from any working directory: commands in any case run in the order given, and the preset, `PROFILE` and the `SANITIZE` list come once. Anything else, or a preset the host cannot build, stops before any work. No command means `all`, and no preset means the host default, `<arch>-linux-gcc` on Linux or `x86_64-windows-clang` on Windows. The `Makefile` runs its goals one at a time and maps `CC=gcc`, `CC=clang` and the cross gcc names to their presets; `PRESET=` names any preset. Each command ends with a `=>` line that names what it produced, and configure reports the probed flags in the same form whenever they change.

Visual Studio 2022+: "Open a local folder" on the root, choose `x86_64-windows-msvc` or `x86_64-windows-clang` from the preset list, pick a program from **Select Startup Item**, F5. Breakpoints, Memory View and `.pdb` work without a `.sln`.

## Conventions

The sources follow the C coding style in [`docs/coding-style.md`](docs/coding-style.md) (Korean edition: [`docs/coding-style.ko.md`](docs/coding-style.ko.md)). `.clang-format` holds the formatting part of that style, as listed in its clang-format section; naming, declarations, pointer parameters and the other rules are kept by hand. `.editorconfig` and `.gitattributes` keep UTF-8 without BOM, LF, 4-space indent, hard tabs only in Makefiles, and a final newline only in C sources and headers.

Include guards spell the path from the project root in uppercase, without a leading underscore, and repeat the name after `#endif`:

```c
#ifndef MODULE_NAME_H
#define MODULE_NAME_H

#endif /* MODULE_NAME_H */
```

`tricks/comma_operator.c` keeps the comma operator in its loop conditions and `gcc/statement_expression.c` keeps its labels and backward jumps inside the statement expression, because those constructs are what the two programs demonstrate.