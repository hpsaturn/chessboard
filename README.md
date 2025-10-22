# SDL2 Chessboard Project

A simple SDL2 application that renders a chessboard using framebuffer techniques.

## Features
- Uses SDL2 for graphics rendering
- Implements framebuffer for pixel-level control
- Renders an 8x8 chessboard pattern
- Clean CMake build system

## Dependencies
- SDL2 development libraries
- CMake (>= 3.10)
- C++11 compatible compiler

## Installation (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install libsdl2-dev cmake build-essential
```

## Building
```bash
./build.sh
```

## Running
```bash
./build/sdl_chessboard
```

## Controls
- ESC or close window to exit

## Project Structure
- `CMakeLists.txt` - CMake build configuration
- `main.cpp` - Main application source code
- `build.sh` - Build script
