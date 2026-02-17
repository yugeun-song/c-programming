# c-programming

This repository documents a comprehensive learning journey of the C programming language. It is designed as a structured archive for code snippets, algorithmic logic, and low-level system experiments, supporting seamless development on both Linux (GCC/Clang) and Windows (Visual Studio 2026/MSVC).

## Features

- **Cross-Platform Build System**: Powered by CMake to ensure consistent builds across different compilers and operating systems.
- **Strict Coding Standards**:
    - **Encoding**: UTF-8 without BOM.
    - **Line Endings**: LF (Line Feed) enforced via `.gitattributes`.
    - **Indentation**: 4-space indentation for source code, 4-width hard tabs for Makefiles via `.editorconfig`.
- **Environment Isolation**: Build artifacts and temporary files are strictly contained within `build/` and `bin/` directories, keeping the system environment clean.

## Project Structure

The repository is organized into universal and platform-specific modules. All compiled binaries are output to the root-level `bin/` directory.

### 1. Universal Modules
Standard C code that is platform-independent. Platform-specific logic within these folders is handled via `#ifdef` preprocessor directives.

- `math/`: Mathematical algorithms and numerical logic.
- `pointer/`: Memory management, pointer arithmetic, and data structure experiments.
- `std/`: Exploration of the C Standard Library (libc).
- `type/`: Data types, alignment, and structure memory layout studies.

### 2. Platform-Specific Modules
Isolated at the build system level. Each folder is only compiled on its respective host OS.

- `gcc/`: Linux-exclusive implementations (POSIX, system calls, etc.). **[Ignored on Windows]**
- `msvc/`: Windows-exclusive implementations (Win32 API, MSVC intrinsics). **[Ignored on Linux]**

### 3. Build Artifacts
- `bin/`: Organized output of executable binaries.
- `build/`: Temporary CMake cache and intermediate object files. **(Git Ignored)**

## Build Instructions

To maintain a clean environment, use the provided setup scripts.

### Linux (General Distributions)
Requires `cmake` and a C compiler (e.g., `gcc` or `clang`).

\`\`\`bash
chmod +x setup_linux.sh
./setup_linux.sh
\`\`\`

### Windows (Visual Studio 2026 / MSVC)
Requires Visual Studio with C++ CMake tools installed.

\`\`\`powershell
# Run in PowerShell
./setup_windows.ps1
\`\`\`

## Branch Strategy

The repository follows a lightweight branch naming convention for organized development:

- `main`: Stable code that compiles on all supported platforms.
- `feat/<topic>`: New learning topics or feature implementations (e.g., `feat/linked-list`).
- `fix/<issue>`: Bug fixes or corrections in logic.
- `docs/<content>`: Documentation updates or comment refinements.
- `refactor/<target>`: Structural improvements without changing behavior.

## Usage

After a successful build, execute the binaries from the `bin/` directory:

\`\`\`bash
# Example execution on Linux
./bin/math/fibonacci

# Example execution on Windows
.\bin\math\fibonacci.exe
\`\`\`
