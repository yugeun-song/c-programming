$ErrorActionPreference = 'Stop'
& cmake -D "BUILD_SCRIPT=$(Split-Path -Leaf $PSCommandPath)" -P (Join-Path $PSScriptRoot 'build.cmake') '--' @args
exit $LASTEXITCODE