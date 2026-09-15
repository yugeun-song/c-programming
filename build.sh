#!/bin/bash
set -e

cd "$(dirname "$0")"

DEFAULT_TARGET="$(uname -m)-linux-gcc"
TARGET="$DEFAULT_TARGET"
BUILD_TYPE="Debug"
CLEAN=0

usage() {
    cat <<EOF
usage: $(basename "$0") [clean] [debug|release] [target]

Configure build/<target> from its CMake preset and build every program into bin/.

  clean       delete build/<target> first
  debug       Debug configuration (default)
  release     Release configuration
  target      configure preset to use (default $DEFAULT_TARGET)
  -h, --help  print this help

Arguments are case-insensitive and may appear in any order.

Targets on this host:
$(cmake --list-presets 2>/dev/null | sed -n 's/^ *"\(.*\)".*$/  \1/p')
EOF
}

for arg in "$@"; do
    case "${arg,,}" in
        clean)        CLEAN=1 ;;
        debug)        BUILD_TYPE="Debug" ;;
        release)      BUILD_TYPE="Release" ;;
        -h|--help)    usage; exit 0 ;;
        [a-z0-9_]*-*) TARGET="${arg,,}" ;;
        *)            usage >&2; echo "unknown argument: $arg" >&2; exit 1 ;;
    esac
done

BUILD_DIR="build/$TARGET"

if [ "$CLEAN" -eq 1 ]; then
    echo "Cleaning $BUILD_DIR..."
    rm -rf "$BUILD_DIR"
fi

echo "Configuring $TARGET..."
cmake --preset "$TARGET"

echo "Building $TARGET ($BUILD_TYPE)..."
cmake --build "$BUILD_DIR" --parallel --config "$BUILD_TYPE"

echo "Build completed successfully!"
