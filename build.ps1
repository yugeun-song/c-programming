param (
    [switch]$Clean,

    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Debug"
)

$ErrorActionPreference = "Stop"

$BuildDir = "build"

if ($Clean) {
    Write-Host "Cleaning previous build..." -ForegroundColor Yellow
    if (Test-Path $BuildDir) {
        Remove-Item -Recurse -Force $BuildDir
    }
}

Write-Host "Configuring CMake project ($Configuration)..." -ForegroundColor Cyan
cmake -S . -B $BuildDir -DCMAKE_BUILD_TYPE=$Configuration
if ($LASTEXITCODE -ne 0) { throw "CMake configuration failed with exit code $LASTEXITCODE." }

Write-Host "Building projects..." -ForegroundColor Cyan
cmake --build $BuildDir --parallel --config $Configuration
if ($LASTEXITCODE -ne 0) { throw "Build failed with exit code $LASTEXITCODE." }

Write-Host "Build completed successfully!" -ForegroundColor Green
