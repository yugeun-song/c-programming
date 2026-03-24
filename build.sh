#!/bin/bash
set -e

BUILD_DIR="build"

if [ "$1" == "clean" ]; then
    echo "Cleaning previous build..."
    rm -rf "$BUILD_DIR"
fi

echo "Configuring CMake project..."
cmake -S . -B "$BUILD_DIR"

echo "Building projects..."
cmake --build "$BUILD_DIR" --parallel

echo "Build completed successfully!"