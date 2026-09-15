$ErrorActionPreference = "Stop"

$BuildDir = Join-Path $PSScriptRoot "build"
$BuildType = "Debug"
$Clean = $false

function Get-Usage {
    @"
usage: $(Split-Path -Leaf $PSCommandPath) [clean] [debug|release]

Configure build/ and build every target into bin/.

  clean       delete build/ first
  debug       Debug configuration (default)
  release     Release configuration
  -h, --help  print this help

Arguments are case-insensitive and may appear in any order.
"@
}

foreach ($arg in $args) {
    switch ("$arg") {
        "clean"   { $Clean = $true }
        "debug"   { $BuildType = "Debug" }
        "release" { $BuildType = "Release" }
        { $_ -in "-h", "--help" } {
            Get-Usage
            exit 0
        }
        default {
            [Console]::Error.WriteLine((Get-Usage))
            [Console]::Error.WriteLine("unknown argument: $arg")
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
