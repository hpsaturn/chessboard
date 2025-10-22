#!/bin/bash

echo "=== SDL2 Chessboard Project Structure Verification ==="
echo ""

# Check if all required files exist
echo "Checking required files:"
files=("CMakeLists.txt" "main.cpp" "build.sh" "README.md")
for file in "${files[@]}"; do
    if [ -f "$file" ]; then
        echo "✓ $file"
    else
        echo "✗ $file (MISSING)"
    fi
done

echo ""
echo "=== Code Structure Summary ==="
echo ""

# Show main.cpp structure
echo "Main.cpp contains:"
grep -E "^class|^const|^#include" main.cpp | head -10

echo ""
echo "CMakeLists.txt contains:"
grep -v "^#" CMakeLists.txt | grep -v "^$"

echo ""
echo "Project ready for building once SDL2 dependencies are installed."
