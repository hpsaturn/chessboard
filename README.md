# SDL2 Chessboard Project

A chessboard rendering project with both ASCII and SDL2 graphical versions.

## Project Structure

- `main.cpp` - SDL2 graphical chessboard implementation
- `main_simple.cpp` - ASCII chessboard implementation (fallback)
- `CMakeLists.txt` - CMake build configuration
- `build.sh` - Build script

## Building

Run the build script:
```bash
./build.sh
```

This will create:
- `chessboard_simple` - ASCII version (always available)
- `sdl_chessboard` - SDL2 graphical version (if SDL2 is available)

## Running

### ASCII Version (Always Available)
```bash
./chessboard_simple
```

### SDL2 Graphical Version
```bash
./sdl_chessboard
```

## Features

### ASCII Version
- Renders a standard 8x8 chessboard in ASCII
- Shows chess notation (a-h, 1-8)
- Uses '#' for white squares and '.' for dark squares

### SDL2 Version
- Graphical chessboard using SDL2
- Framebuffer-based rendering
- Real-time display with event handling
- Press ESC or close window to exit

## Dependencies

- **Required**: C++11 compiler, CMake 3.10+
- **Optional**: SDL2 development libraries for graphical version

## Development Notes

The project is designed to work even when SDL2 is not available by providing a fallback ASCII version. The build system automatically detects SDL2 and builds the graphical version if possible.
