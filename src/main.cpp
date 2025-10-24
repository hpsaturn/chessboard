#include <iostream>
#include <string>
#include "chess_pieces.h"

// Function declarations
void renderChessboardChars();
void renderChessboardSDL();
void renderChessboardNcurses();

void printHelp() {
    std::cout << "Chessboard Renderer\n";
    std::cout << "===================\n";
    std::cout << "Usage:\n";
    std::cout << "  chessboard [options]\n";
    std::cout << "\n";
    std::cout << "Options:\n";
    std::cout << "  --chars     Render chessboard using ASCII characters (default)\n";
    std::cout << "  --sdl       Render chessboard using SDL2 graphics\n";
    std::cout << "  --ncurses   Render chessboard using interactive ncurses interface\n";
    std::cout << "  --help      Show this help message\n";
    std::cout << "\n";
    std::cout << "Controls (SDL mode):\n";
    std::cout << "  Q or ESC   Quit the application\n";
    std::cout << "\n";
    std::cout << "Controls (NCurses mode):\n";
    std::cout << "  Arrow Keys  Move selection\n";
    std::cout << "  Q           Quit\n";
    std::cout << "  Enter       Select piece\n";
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    std::string mode = "chars";
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--sdl") {
            mode = "sdl";
        } else if (arg == "--chars") {
            mode = "chars";
        } else if (arg == "--ncurses") {
            mode = "ncurses";
        } else if (arg == "--help") {
            printHelp();
            return 0;
        } else {
            std::cout << "Unknown option: " << arg << "\n";
            std::cout << "Use --help for usage information.\n";
            return 1;
        }
    }
    
    std::cout << "Chessboard with Pieces - " << mode << " mode\n";
    std::cout << "========================\n";
    
    if (mode == "sdl") {
        renderChessboardSDL();
    } else if (mode == "ncurses") {
        renderChessboardNcurses();
    } else {
        renderChessboardChars();
    }
    
    return 0;
}
