#!/bin/bash

# Build script for CME Mock Server

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"

echo "Building CME Mock Server..."
echo "Project root: $PROJECT_ROOT"

# Create build directory
mkdir -p "$PROJECT_ROOT/build"
cd "$PROJECT_ROOT/build"

# Configure with CMake
echo "Configuring..."
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
echo "Building..."
make -j$(nproc)

echo "Build complete!"
echo "Executable: $PROJECT_ROOT/build/cme_mock_server"