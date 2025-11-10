[![CMake](https://github.com/hpsaturn/chessboard/actions/workflows/build.yml/badge.svg)](https://github.com/hpsaturn/chessboard/actions/workflows/build.yml)

# GNUChess Frontend

C++ chessboard renderer with SDL2, ASCII, and NCurses on the top of GNUChess UCI server protocol.

![Picocalc Preview](res/screenshots/preview.jpg)

## Features

- [x] **SDL2 Mode**: Graphical chessboard with smooth rendering (default)
- [x] **GNUChess UCI**: protocol integration
- [x] **Bitboard validations**: improvement using bit arrays of 64 bits
- [x] **Picocalc Luckfox Lyra**: Optimized for this mod and board
- [x] **FEN notation support**: Basic game state loader via CLI argument
- [x] **Fast game states**: Save and load bookmarks with small preview
- [ ] **NCurses Mode**: Terminal UI with keyboard navigation (in progress)
- [ ] **Cross-platform**: Works on Linux, Windows, and macOS
- [ ] **ASCII Mode**: Terminal interface with Unicode pieces (inprogress)

## Requirements

### Build Dependencies

- GNUChess
- CMake (>= 3.10)
- C++11 compatible compiler
- SDL2 development libraries (ttf and image dev libraries)
- Yaml cpp library
- NCurses development libraries (optional, for interactive terminal mode)

## Installation

### Linux (Ubuntu/Debian)

```bash
# Install development libraries
sudo apt update
sudo apt install gnuchess cmake build-essential libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libncurses-dev libyaml-cpp-dev

# Build the project
./build.sh

# Run SLD2 mode:
./chess

# Load previous state (FEN standard):
./chess --fen "r3kb1r/ppp1pppp/5n2/8/1q4P1/3b1P2/PP1N3P/R1BQK2R b - - 0 1"
```

For more about FEN notation and details, please enter [here](https://www.redhotpawn.com/chess/chess-fen-viewer.php)

## Usage

This version is optimized for Picocalc board using Luckfox Lyra. For that it has the next key bindings:

| Key | Action |
|:-------:|-------|
| **Arrows** | Piece and setting selections |
| **SPACE** | Select piece or setting |
| **ENTER** | Move selected piece |
| **ESC** | Deselect piece or window exit |
| **F2**| Save game state |
| **F3**| Load last game state saved |
| **F4**| Enter to game states window |
| **BACKSPACE** | Delete game state (states window) |
| **S**| Enter to settings section |
| **I**| Toggle to show game info |
| **Q**| Exit the game. (without state save for now) |
| **R**| Restart the game. (without ask before for now) |
| **H**| Help (in progress) |

![screenshots](res/screenshots/screenshots.png)

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
- [x] Adapter class for join with GNU-Chess using UCI protocol
- [x] Picocalc keyboard issues (fixed)
- [x] Dead pieces count in each side (Game info window)
- [x] Basic settings modal window
- [x] Castelling full validations
- [x] Ilegal check moves validations (engine crashes)
- [x] isInCheck() validations
- [x] Help info modal window
- [ ] FEN casteling support
- [ ] Game info modal window (in porgress)
- [ ] Points from the user (positive/negative)
- [ ] Chronometers
- [ ] Sound support
- [ ] Buildroot config files for Luckfox Lyra tests
- [ ] Sprite pieces improvement

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## Credits

- Special thanks to my son, M.V for its continuous critics and beta tests
- Thanks to [@lunokjod](https://github.com/lunokjod) for his MongoIA assistant
- Thanks to [Chess SDL project](https://gitlab.com/fortysixandtwo/SDL-Chess) for its CC by SA image assets.

## License

This project is GPLv3. Some assets they are licensed under CC BY-SA 3.0. See assets directory for details.
