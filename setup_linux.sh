#!/bin/bash

# Exit on error
set -e

# Project-specific local variables
BUILD_DIR="build"
BIN_DIR="bin"

echo "Initializing build environment for Linux..."

# 1. Create necessary directories if they don't exist
if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
    echo "Created $BUILD_DIR directory."
fi

# 2. Run CMake configuration
# This creates build artifacts only inside the /build folder (ignored by git)
echo "Running CMake configuration..."
cmake -S . -B "$BUILD_DIR"

# 3. Compile the project
echo "Compiling targets..."
cmake --build "$BUILD_DIR"

echo "------------------------------------------------"
echo "Setup complete. Binaries are located in the '$BIN_DIR' directory."
echo "Note: No system-wide environment variables were modified."
