#!/bin/bash

echo "Building SDL2 Chessboard Project..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring project with CMake..."
cmake ..

# Build the project
echo "Building executable..."
make

echo "Build complete! Run with: ./sdl_chessboard"
