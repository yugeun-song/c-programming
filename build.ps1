$ErrorActionPreference = "Stop"

$DefaultTarget = if ($IsLinux) { "$(uname -m)-linux-gcc" } else { "x86_64-windows-msvc" }
$Target = $DefaultTarget
$BuildType = "Debug"
$Clean = $false

function Get-Usage {
    $targets = cmake -S $PSScriptRoot --list-presets 2>$null | ForEach-Object { if ($_ -match '^\s*"(.+)"') { "  " + $Matches[1] } }
    @"
usage: $(Split-Path -Leaf $PSCommandPath) [clean] [debug|release] [target]

Configure build/<target> from its CMake preset and build every program into bin/.

  clean       delete build/<target> first
  debug       Debug configuration (default)
  release     Release configuration
  target      configure preset to use (default $DefaultTarget)
  -h, --help  print this help

Arguments are case-insensitive and may appear in any order.

Targets on this host:
$($targets -join "`n")
"@
}

foreach ($arg in $args) {
    switch -Regex ("$arg") {
        "^clean$"       { $Clean = $true }
        "^debug$"       { $BuildType = "Debug" }
        "^release$"     { $BuildType = "Release" }
        "^(-h|--help)$" {
            Get-Usage
            exit 0
        }
        "^[a-z0-9_]+-"  { $Target = "$arg".ToLowerInvariant() }
        default {
            [Console]::Error.WriteLine((Get-Usage))
            [Console]::Error.WriteLine("unknown argument: $arg")
            exit 1
        }
    }
}

$BuildDir = Join-Path $PSScriptRoot "build/$Target"

if ($Clean) {
    Write-Host "Cleaning $BuildDir..." -ForegroundColor Yellow
    if (Test-Path $BuildDir) {
        Remove-Item -Recurse -Force $BuildDir
    }
}

Write-Host "Configuring $Target..." -ForegroundColor Cyan
cmake -S $PSScriptRoot --preset $Target
if ($LASTEXITCODE -ne 0) { throw "CMake configuration failed with exit code $LASTEXITCODE." }

Write-Host "Building $Target ($BuildType)..." -ForegroundColor Cyan
cmake --build $BuildDir --parallel --config $BuildType
if ($LASTEXITCODE -ne 0) { throw "Build failed with exit code $LASTEXITCODE." }

Write-Host "Build completed successfully!" -ForegroundColor Green
