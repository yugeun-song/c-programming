#!/bin/bash
set -e

cd "$(dirname "$0")"

BUILD_DIR="build"
BUILD_TYPE="Debug"
CLEAN=0

usage() {
    cat <<EOF
usage: $(basename "$0") [clean] [debug|release]

Configure build/ and build every target into bin/.

  clean       delete build/ first
  debug       Debug configuration (default)
  release     Release configuration
  -h, --help  print this help

Arguments are case-insensitive and may appear in any order.
EOF
}

for arg in "$@"; do
    case "${arg,,}" in
        clean)     CLEAN=1 ;;
        debug)     BUILD_TYPE="Debug" ;;
        release)   BUILD_TYPE="Release" ;;
        -h|--help) usage; exit 0 ;;
        *)         usage >&2; echo "unknown argument: $arg" >&2; exit 1 ;;
    esac
done

if [ "$CLEAN" -eq 1 ]; then
    echo "Cleaning previous build..."
    rm -rf "$BUILD_DIR"
fi

echo "Configuring CMake project ($BUILD_TYPE)..."
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

echo "Building projects..."
cmake --build "$BUILD_DIR" --parallel --config "$BUILD_TYPE"

echo "Build completed successfully!"
