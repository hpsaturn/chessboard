#include <iostream>
#include <string>

// Function declarations
void renderChessboardChars();
void renderChessboardSDL();

void printHelp() {
    std::cout << "Chessboard Renderer\n";
    std::cout << "===================\n";
    std::cout << "Usage:\n";
    std::cout << "  ./chessboard           - Render ASCII chessboard (default)\n";
    std::cout << "  ./chessboard --chars   - Render ASCII chessboard\n";
    std::cout << "  ./chessboard --sdl     - Render SDL2 chessboard\n";
    std::cout << "  ./chessboard --help    - Show this help\n";
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    std::string mode = "chars";
    
    // Parse command line arguments
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--sdl") {
            mode = "sdl";
        } else if (arg == "--chars") {
            mode = "chars";
        } else if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            printHelp();
            return 1;
        }
    }

    // Execute selected mode
    if (mode == "chars") {
        renderChessboardChars();
    } else if (mode == "sdl") {
        renderChessboardSDL();
    }

    return 0;
}
