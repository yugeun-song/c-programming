# Stop on error
$ErrorActionPreference = "Stop"

$BuildDir = "build"
$BinDir = "bin"

Write-Host "Initializing build environment for Windows (MSVC)..." -ForegroundColor Cyan

# 1. Create build directory if it doesn't exist
if (-not (Test-Path $BuildDir)) {
    New-Item -Path $BuildDir -ItemType Directory | Out-Null
    Write-Host "Created $BuildDir directory."
}

# 2. Run CMake configuration
# MSVC specific flags are handled by CMakeLists.txt automatically
Write-Host "Running CMake configuration..." -ForegroundColor Yellow
cmake -S . -B $BuildDir

# 3. Compile the project
Write-Host "Compiling targets..." -ForegroundColor Yellow
cmake --build $BuildDir

Write-Host "------------------------------------------------" -ForegroundColor Green
Write-Host "Setup complete. Binaries are located in the '$BinDir' directory."
Write-Host "Note: No registry keys or system environment variables were modified."
