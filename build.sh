#!/bin/bash
set -e

BUILD_DIR="build"
BUILD_TYPE="Debug"
CLEAN=0

usage() {
    echo "usage: $0 [clean] [debug|release]" >&2
    exit 1
}

for arg in "$@"; do
    case "$arg" in
        clean)           CLEAN=1 ;;
        debug|Debug)     BUILD_TYPE="Debug" ;;
        release|Release) BUILD_TYPE="Release" ;;
        *)               usage ;;
    esac
done

if [ "$CLEAN" -eq 1 ]; then
    echo "Cleaning previous build..."
    rm -rf "$BUILD_DIR"
fi

echo "Configuring CMake project ($BUILD_TYPE)..."
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

echo "Building projects..."
cmake --build "$BUILD_DIR" --parallel

echo "Build completed successfully!"
