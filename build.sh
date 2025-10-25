#!/bin/bash

# Chessboard Project Build Script

echo "Building Chessboard Project..."

# Clean previous build
if [ -d "build" ]; then
    echo "Cleaning previous build..."
    rm -rf build
fi

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring project with CMake..."
if cmake ..; then
    echo "CMake configuration successful"
else
    echo "CMake configuration failed"
    exit 1
fi

# Build the project
echo "Building executable..."
if make -j$(nproc); then
    echo "Build successful!"
    cp chess ..
else
    echo "Build failed"
    exit 1
fi

# Display build results
echo ""
echo "Build complete!"
echo "Available executable:"
echo "  ./chessboard - Unified chessboard with ASCII and SDL2 modes"

# Copy executable to project root for easy access
cp chessboard ../

cd ..
