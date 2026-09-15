$ErrorActionPreference = "Stop"

$BuildDir = Join-Path $PSScriptRoot "build"
$BuildType = "Debug"
$Clean = $false

foreach ($arg in $args) {
    switch ("$arg") {
        "clean"   { $Clean = $true }
        "debug"   { $BuildType = "Debug" }
        "release" { $BuildType = "Release" }
        default {
            [Console]::Error.WriteLine("usage: $(Split-Path -Leaf $PSCommandPath) [clean] [debug|release]")
            exit 1
        }
    }
}

if ($Clean) {
    Write-Host "Cleaning previous build..." -ForegroundColor Yellow
    if (Test-Path $BuildDir) {
        Remove-Item -Recurse -Force $BuildDir
    }
}

Write-Host "Configuring CMake project ($BuildType)..." -ForegroundColor Cyan
cmake -S $PSScriptRoot -B $BuildDir "-DCMAKE_BUILD_TYPE=$BuildType"
if ($LASTEXITCODE -ne 0) { throw "CMake configuration failed with exit code $LASTEXITCODE." }

Write-Host "Building projects..." -ForegroundColor Cyan
cmake --build $BuildDir --parallel --config $BuildType
if ($LASTEXITCODE -ne 0) { throw "Build failed with exit code $LASTEXITCODE." }

Write-Host "Build completed successfully!" -ForegroundColor Green
