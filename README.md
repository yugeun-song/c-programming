# c-programming

C source focusing on memory-level behavior, cross-platform robustness, performance optimization tactics, and common pitfalls.

## Notice

- **Cross-Platform Build System**: CMake-based configuration ensuring consistent artifact generation across Linux (GCC/Clang) and Windows (MSVC).
- **1:1 Source-to-Binary Mirroring**: Executables are automatically named after their source files and strictly organized in `bin/<module>/`, mirroring the source directory structure.
- **Strict Coding Standards**:
    - **Formatting**: LLVM-based `.clang-format` — right-aligned pointers (`char *p`), Allman function braces, K&R control flow, 100-column soft limit.
    - **Encoding**: UTF-8 without BOM.
    - **Line Endings**: LF (Line Feed) enforced via `.gitattributes`.
    - **Indentation**: 4-space indentation for source code, 4-width hard tabs for Makefiles via `.editorconfig`.
- **Dual Binary Builds** (GCC/Clang): Each source produces two executables in `bin/<module>/`: `<name>` (trace build) and `<name>_opt` (performance build, `-O2`). MSVC builds remain single-variant.
- **Debug-Oriented Compilation** (GCC/Clang, trace build): `-ggdb3 -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fasynchronous-unwind-tables -pg` for full GDB macro support, accurate backtraces, gprof/uftrace function tracing, and reliable perf profiling.
- **Assembly Listing Generation** (GCC/Clang): Every build also lowers each source to assembly listings (`-S`) for x86_64, AArch64, and RISC-V across seven optimization levels (`-O0` to `-Og`), stored under `asm/`. Missing cross compilers are logged at configure time and skipped without failing the build.
- **Environment Isolation**: Build artifacts (`.o`, `.obj`, `.pdb`), binaries (`.exe`, ELF), and generated assembly listings (`.s`) are strictly contained within `build/`, `bin/`, and `asm/`. Debugging tool outputs (GDB, Valgrind, strace, ltrace, uftrace) are excluded via `.gitignore`.
- **IDE Integration**: Native support for Visual Studio 2022+ "Open Folder" workflow without requiring `.sln` or `.vcxproj` files.

## Project Structure

The repository ignores system-specific configuration files. All compiled binaries are output to the root-level `bin/` directory, organized by module. Generated assembly listings are output to the root-level `asm/` directory, mirroring the same module structure.

### 1. Universal Modules
Standard C code compatible with all platforms. Platform-specific logic is handled via `#ifdef` preprocessor directives.

- `math/`: Mathematical algorithms and numerical logic.
- `memory/`: Structure padding, alignment rules, and byte-level memory layout visualization.
- `pointer/`: Memory management, pointer arithmetic, and raw memory manipulation.
- `std/`: Experiments with the C Standard Library (libc).
- `type/`: Data type sizes, alignment, and structure memory layout.

### 2. Platform-Specific Modules
Isolated at the build system level. These directories are excluded from the build target list on incompatible OSs.

- `gcc/`: Linux-exclusive implementations (POSIX API, System Calls, Inline Assembly for GCC). **[Ignored on Windows]**
- `msvc/`: Windows-exclusive implementations (Win32 API, MSVC Intrinsics). **[Ignored on Linux]**

### Adding a New Program

Source files are listed explicitly in `CMakeLists.txt` (`UNIVERSAL_SOURCES`, `GCC_SOURCES`, `MSVC_SOURCES`). To add a program, create the `.c` file under its module directory and append its path to the matching list.

## Build Instructions

### Option A: Command Line Scripts (Automated)

Use the provided scripts to initialize the `build/` directory and compile all targets using the unified build script.

#### Linux (GCC / Clang)
```bash
chmod +x build.sh
./build.sh
```

#### Windows (PowerShell / MSVC)
```powershell
./build.ps1
```

---

### Option B: Visual Studio 2022+ (IDE Workflow)

This project utilizes the **"Open Folder"** capability of Visual Studio 2022+. Solution files (`.sln`) are not required.

1.  **Open Project**:
    - Launch Visual Studio 2022+.
    - Select **"Open a local folder"**.
    - Navigate to and select the root `c-programming` directory.

2.  **Configuration**:
    - Wait for the **"CMake generation finished"** status in the Output window.
    - Visual Studio automatically parses `CMakeLists.txt` and configures the environment.

3.  **Build & Run**:
    - The project contains multiple entry points (`main` functions).
    - Locate the **"Select Startup Item"** dropdown in the top toolbar (Play button area).
    - Select the specific target to execute (e.g., `math_fibonacci.exe`, `type_basic_types.exe`).
    - Press **F5** (Start Debugging) or **Ctrl+F5** (Start Without Debugging).
    - *Note: Switching the source file in the editor does not automatically switch the build target. The Startup Item must be selected explicitly.*

4.  **Debugging**:
    - Debug symbols (`.pdb`) are automatically generated in the `bin/<module>/` directory alongside the executable.
    - Breakpoints and Memory View function natively.

## Manual Build (Unified Script)

For manual execution without helper scripts, use the CMake script mode. This ensures correct configuration (e.g., Debug mode on MSVC) across all platforms.

```bash
cmake -P build.cmake
```

## Assembly Listings

Each build (GCC/Clang environments only; skipped under MSVC) additionally generates assembly listings for every target source via `-S`, organized as `asm/<module>/<source>/<compiler>/<arch>/<level>.s`.

- **Compilers**: `gcc` only for now. The `<compiler>` path segment reserves room for additional families (e.g. `clang`) without relocating existing output. The exact compiler version that produced a listing is embedded in its `.ident` directive.
- **Architectures**: `x86_64` (native), `aarch64`, `riscv64`. Cross targets require the corresponding cross compiler (`aarch64-linux-gnu-gcc`, `riscv64-linux-gnu-gcc`). If a compiler is missing, that architecture is skipped and reported at configure time; the rest of the build proceeds.
- **Optimization Levels**: `-O0`, `-O1`, `-O2`, `-O3`, `-Os`, `-Ofast`, `-Og` — one listing per level.
- **Flag Isolation**: Listings are compiled with the optimization flag only, isolated from the debug-oriented flags used for binaries, so the output reflects the optimizer's default behavior at each level.
- **Incompatible Sources**: If a source cannot be compiled for an architecture (e.g. `gcc/likely.c` requires x86_64/AArch64), a `<level>.log` containing the compiler error is written instead of the listing and the build continues.
- **Version Control**: The `asm/` directory is generated output and is excluded via `.gitignore`.

```bash
# Compare optimization levels for the same source and architecture
diff asm/gcc/likely/gcc/x86_64/O0.s asm/gcc/likely/gcc/x86_64/O2.s

# Compare architectures at the same optimization level
diff asm/std/hello_world/gcc/x86_64/O2.s asm/std/hello_world/gcc/aarch64/O2.s
```

## Execution

Compiled binaries are located in the `bin/` directory, structured by module name.

```bash
# Linux (trace build / optimized build)
./bin/somefolder/filename
./bin/somefolder/filename_opt

# Windows
.\bin\somefolder\filename.exe
```

