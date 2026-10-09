if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    [Console]::Error.WriteLine('cmake is not on PATH.')
    exit 1
}
$self = Split-Path -Leaf $PSCommandPath
if ($args -contains '') {
    [Console]::Error.WriteLine("Unknown argument ''; see '$self -h'.")
    exit 1
}
& cmake -D "BUILD_SCRIPT=$self" -P (Join-Path $PSScriptRoot 'build.cmake') '--' @args
exit $LASTEXITCODE