# c-programming

C source focusing on memory-level behavior, cross-platform robustness, performance optimization tactics, and common pitfalls.

## Notice

- **Cross-Platform Build System**: CMake-based configuration ensuring consistent artifact generation across Linux (GCC/Clang) and Windows (MSVC).
- **1:1 Source-to-Binary Mirroring**: Executables are automatically named after their source files and strictly organized in `bin/<module>/`, mirroring the source directory structure.
- **Strict Coding Standards**:
    - **Encoding**: UTF-8 without BOM.
    - **Line Endings**: LF (Line Feed) enforced via `.gitattributes`.
    - **Indentation**: 4-space indentation for source code, 4-width hard tabs for Makefiles via `.editorconfig`.
- **Environment Isolation**: Build artifacts (`.o`, `.obj`, `.pdb`) and binaries (`.exe`, ELF) are strictly contained within `build/` and `bin/` directories.
- **IDE Integration**: Native support for Visual Studio 2022+ "Open Folder" workflow without requiring `.sln` or `.vcxproj` files.

## Project Structure

The repository ignores system-specific configuration files. All compiled binaries are output to the root-level `bin/` directory, organized by module.

### 1. Universal Modules
Standard C code compatible with all platforms. Platform-specific logic is handled via `#ifdef` preprocessor directives.

- `math/`: Mathematical algorithms and numerical logic.
- `pointer/`: Memory management, pointer arithmetic, and raw memory manipulation.
- `std/`: Experiments with the C Standard Library (libc).
- `type/`: Data type sizes, alignment, and structure memory layout.

### 2. Platform-Specific Modules
Isolated at the build system level. These directories are excluded from the build target list on incompatible OSs.

- `gcc/`: Linux-exclusive implementations (POSIX API, System Calls, Inline Assembly for GCC). **[Ignored on Windows]**
- `msvc/`: Windows-exclusive implementations (Win32 API, MSVC Intrinsics). **[Ignored on Linux]**

## Build Instructions

### Option A: Command Line Scripts (Automated)

Use the provided scripts to initialize the `build/` directory and compile all targets using the unified build script.

#### Linux (GCC / Clang)
```bash
chmod +x setup_linux.sh
./setup_linux.sh
```

#### Windows (PowerShell / MSVC)
```powershell
./setup_windows.ps1
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

## Execution

Compiled binaries are located in the `bin/` directory, structured by module name.

```bash
# Linux
./bin/somefolder/filename

# Windows
.\bin\somefolder\filename.exe
```

