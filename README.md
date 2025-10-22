# Chessboard Project

A C++ chessboard renderer with both ASCII and SDL2 graphical output modes.

## Features

- **ASCII Mode**: Terminal-based chessboard display with Unicode pieces
- **SDL2 Mode**: Graphical chessboard with smooth rendering
- **Unified Binary**: Single executable with command-line options
- **Cross-platform**: Works on Linux, Windows, and macOS

## Requirements

### Build Dependencies
- CMake (>= 3.10)
- C++11 compatible compiler
- SDL2 development libraries (optional, for graphical mode)

### Runtime Dependencies
- SDL2 runtime libraries (for graphical mode)

## Installation

### Linux (Ubuntu/Debian)
```bash
# Install SDL2 development libraries
sudo apt update
sudo apt install cmake build-essential libsdl2-dev

# Build the project
./build.sh
```

### Building from Source
```bash
# Clone and build
mkdir build && cd build
cmake ..
make

# Install (optional)
sudo make install
```

## Usage

### Basic Usage
```bash
# Default ASCII mode
./chessboard

# Graphical SDL2 mode
./chessboard --sdl

# Explicit ASCII mode
./chessboard --chars

# Show help
./chessboard --help
```

### Command Line Options

| Option | Description |
|--------|-------------|
| `--sdl` | Display chessboard using SDL2 (graphical) |
| `--chars` | Display chessboard using ASCII characters (default) |
| `--help` | Show help message |

### Examples

#### ASCII Mode Output
```
  a b c d e f g h
8 R N B Q K B N R 8
7 P P P P P P P P 7
6 . # . # . # . # 6
5 # . # . # . # . 5
4 . # . # . # . # 4
3 # . # . # . # . 3
2 p p p p p p p p 2
1 r n b q k b n r 1
  a b c d e f g h
```

#### SDL2 Mode
- Opens a 320x320 pixel window
- Displays a colored chessboard
- Press any key or close window to exit

## Piece Notation

| Piece | ASCII | Description |
|-------|-------|-------------|
| ♔ ♚ | K k | King |
| ♕ ♛ | Q q | Queen |
| ♗ ♝ | B b | Bishop |
| ♘ ♞ | N n | Knight |
| ♖ ♜ | R r | Rook |
| ♙ ♟ | P p | Pawn |

**Note**: Uppercase letters represent black pieces, lowercase represent white pieces.

## Build Scripts

- `build.sh` - Main build script
- `test_project.sh` - Test script to verify functionality

## Project Structure

```
├── main.cpp          # Unified chessboard implementation
├── CMakeLists.txt    # CMake build configuration
├── build.sh          # Build automation script
├── test_project.sh   # Test verification script
└── README.md         # This documentation
```

## Troubleshooting

### SDL2 Not Found
If SDL2 is not available, the program will automatically fall back to ASCII mode.

### Build Issues
1. Ensure CMake and build tools are installed
2. Check SDL2 development packages are installed for graphical mode
3. Verify C++11 compiler support

### Runtime Issues
- ASCII mode should work on any terminal
- SDL2 mode requires X11/Wayland display server on Linux

## License

This project is open source. Feel free to modify and distribute.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test both ASCII and SDL2 modes
5. Submit a pull request
