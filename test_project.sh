#!/bin/bash

echo "=== Chessboard Project Test ==="
echo

# Test 1: Build the project
echo "1. Building project..."
if ./build.sh; then
    echo "✓ Build successful"
else
    echo "✗ Build failed"
    exit 1
fi

echo

# Test 2: Run ASCII version
echo "2. Testing ASCII version..."
cd build
if ./chessboard_simple > /dev/null 2>&1; then
    echo "✓ ASCII version works"
    echo "   Sample output:"
    ./chessboard_simple | head -5
else
    echo "✗ ASCII version failed"
    exit 1
fi

echo

# Test 3: Check if SDL2 version exists
echo "3. Checking SDL2 version..."
if [ -f "sdl_chessboard" ]; then
    echo "✓ SDL2 version available"
    echo "   Run with: ./sdl_chessboard"
else
    echo "⚠ SDL2 version not available (SDL2 not installed)"
fi

echo
echo "=== All tests completed successfully! ==="
