$ErrorActionPreference = "Stop"

$BuildDir = "build"

param (
    [switch]$Clean
)

if ($Clean) {
    Write-Host "Cleaning previous build..." -ForegroundColor Yellow
    if (Test-Path $BuildDir) {
        Remove-Item -Recurse -Force $BuildDir
    }
}

Write-Host "Configuring CMake project..." -ForegroundColor Cyan
cmake -S . -B $BuildDir

Write-Host "Building projects..." -ForegroundColor Cyan
cmake --build $BuildDir --parallel

Write-Host "Build completed successfully!" -ForegroundColor Green