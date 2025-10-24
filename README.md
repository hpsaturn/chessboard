# Chessboard Frontend

A C++ chessboard renderer with ASCII, SDL2 graphical, and NCurses terminal output modes.

## Features

- **ASCII Mode**: Terminal-based chessboard display with Unicode pieces
- **SDL2 Mode**: Graphical chessboard with smooth rendering
- **NCurses Mode**: Interactive terminal interface with keyboard navigation
- **Unified Binary**: Single executable with command-line options
- **Cross-platform**: Works on Linux, Windows, and macOS

## Requirements

### Build Dependencies
- CMake (>= 3.10)
- C++11 compatible compiler
- SDL2 development libraries (optional, for graphical mode)
- NCurses development libraries (optional, for interactive terminal mode)

### Runtime Dependencies
- SDL2 runtime libraries (for graphical mode)
- NCurses runtime libraries (for interactive terminal mode)

## Installation

### Linux (Ubuntu/Debian)
```bash
# Install development libraries
sudo apt update
sudo apt install cmake build-essential libsdl2-dev libncurses-dev

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

# Interactive NCurses mode
./chessboard --ncurses

# Explicit ASCII mode
./chessboard --chars

# Show help
./chessboard --help
```

### Command Line Options

| Option | Description |
|--------|-------------|
| `--sdl` | Display chessboard using SDL2 (graphical) |
| `--ncurses` | Display chessboard using NCurses (interactive terminal) |
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

- Opens a graphical window with three zones: chessboard, history panel, and input zone
- Displays a colored chessboard with piece sprites
- Press 'q' key or close window to exit

#### NCurses Mode

- Interactive terminal interface with keyboard navigation
- Three-zone layout: chessboard, history panel, and input zone
- Arrow key navigation for piece selection
- Transparent background respecting user terminal settings
- Complete grid lines with proper ACS characters
- Press 'q' key to exit

## Piece Notation

| Piece | ASCII | NCurses | Description |
|-------|-------|---------|-------------|
| ♔ ♚ | K k | K k | King |
| ♕ ♛ | Q q | Q q | Queen |
| ♗ ♝ | B b | B b | Bishop |
| ♘ ♞ | N n | N n | Knight |
| ♖ ♜ | R r | R r | Rook |
| ♙ ♟ | P p | P p | Pawn |

**Note**: Uppercase letters represent black pieces, lowercase represent white pieces.

## Build Scripts

- `build.sh` - Main build script
- `test_project.sh` - Test script to verify functionality

## Project Structure

```
├── src/
│   ├── main.cpp                 # Unified chessboard implementation
│   ├── chessboard_sdl.cpp       # SDL2 graphical implementation
│   ├── chessboard_sdl.h         # SDL2 header
│   ├── chessboard_ncurses.cpp   # NCurses terminal implementation
│   ├── chessboard_ncurses.h     # NCurses header
│   ├── chess_pieces_sdl.cpp     # SDL2 chess piece rendering
│   └── chess_pieces_sdl.h       # SDL2 chess piece header
├── CMakeLists.txt               # CMake build configuration
├── build.sh                     # Build automation script
├── test_project.sh              # Test verification script
└── README.md                    # This documentation
```

## Troubleshooting

### SDL2 Not Found

If SDL2 is not available, the program will automatically fall back to ASCII mode.

### NCurses Not Found

If NCurses is not available, the program will automatically fall back to ASCII mode.

### Build Issues

1. Ensure CMake and build tools are installed
2. Check SDL2 development packages are installed for graphical mode
3. Check NCurses development packages are installed for interactive terminal mode
4. Verify C++11 compiler support

### Runtime Issues

- ASCII mode should work on any terminal
- SDL2 mode requires X11/Wayland display server on Linux
- NCurses mode requires a terminal that supports NCurses

## TODO

- [ ] Framebuffer support for Luckfox Lyra basic Linux buildroot image
- [ ] Buildroot config files for Luckfox Lyra tests
- [ ] Sprite pieces and its implementation
- [ ] Intermediate class for join with different engines like GNU-Chess

## License

This project is open source. Feel free to modify and distribute.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test all modes (ASCII, SDL2, NCurses)
5. Submit a pull request
