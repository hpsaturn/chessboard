#!/bin/bash

echo "Building Chessboard Project..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring project with CMake..."
cmake ..

# Build the project
echo "Building executables..."
make

echo "Build complete!"
echo "Available executables:"
if [ -f "chessboard_simple" ]; then
    echo "  ./chessboard_simple - ASCII version (always available)"
fi
if [ -f "sdl_chessboard" ]; then
    echo "  ./sdl_chessboard - SDL2 graphical version"
else
    echo "  SDL2 version not available (SDL2 not found)"
fi
