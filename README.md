# Chessboard Frontend

A C++ chessboard renderer with ASCII, SDL2 graphical, and NCurses terminal output modes.

## Features

- [x] **SDL2 Mode**: Graphical chessboard with smooth rendering (default)
- [x] **GNUChess UCI**: protocol integration
- [x] **Unified Binary**: Single executable with command-line options
- [x] **Cross-platform**: Works on Linux, Windows, and macOS
- [ ] **Picocalc Luckfox Lyra** full support (in progress)
- [ ] **NCurses Mode**: Terminal interface with keyboard navigation (in progress)
- [ ] **ASCII Mode**: Terminal interface with Unicode pieces (inprogress)

## Requirements

### Build Dependencies

- GNUChess
- CMake (>= 3.10)
- C++11 compatible compiler
- SDL2 development libraries (ttf and image dev libraries)
- NCurses development libraries (optional, for interactive terminal mode)

### Runtime Dependencies

- SDL2 runtime libraries (for graphical mode)
- NCurses runtime libraries (for interactive terminal mode)

## Installation

### Linux (Ubuntu/Debian)

```bash
# Install development libraries
sudo apt update
sudo apt install gnuchess cmake build-essential libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libncurses-dev

# Build the project
./build.sh

# Run SLD2 mode:
./chess
```

![Installation Preview](res/screenshots/preview.jpg)

### Ncurses/Chars Board Piece Notation

| Piece | ASCII | NCurses | Description |
|-------|-------|---------|-------------|
| ♔ ♚ | K k | K k | King |
| ♕ ♛ | Q q | Q q | Queen |
| ♗ ♝ | B b | B b | Bishop |
| ♘ ♞ | N n | N n | Knight |
| ♖ ♜ | R r | R r | Rook |
| ♙ ♟ | P p | P p | Pawn |

**Note**: Uppercase letters represent black pieces, lowercase represent white pieces.

## TODO

- [x] Framebuffer support for Luckfox Lyra basic Linux buildroot image
- [x] Intermediate class for join with different engines like GNU-Chess
- [ ] Picocalc keyboard issues (in progress)
- [ ] Buildroot config files for Luckfox Lyra tests
- [ ] Sprite pieces and its implementation

## License

This project is GPLv3. Some assets they are licensed under CC BY-SA 3.0. See assets directory for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test all modes (ASCII, SDL2, NCurses)
5. Submit a pull request

## Credits

- Thanks to [@lunokjod](https://github.com/lunokjod) for his MongoIA assistant
- Thanks to [Chess SDL project](https://gitlab.com/fortysixandtwo/SDL-Chess) for its CC by SA image assets.