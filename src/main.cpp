#include <iostream>
#include <string>

void renderChessboardChars();
void renderChessboardSDL();
void renderChessboardNcurses();
void renderChessboardCharsEngine();

void printHelp() {
    std::cout << "Chessboard Renderer\n";
    std::cout << "===================\n";
    std::cout << "Usage:\n";
    std::cout << "  chessboard [options]\n";
    std::cout << "\n";
    std::cout << "Options:\n";
    std::cout << "              Render chessboard using SDL2 graphics (default)\n";
    std::cout << "  --chars     Render chessboard using ASCII characters\n";
    std::cout << "  --ncurses   Render chessboard using interactive ncurses interface\n";
    std::cout << "  --engine    Test GNUChess engine integration with character mode\n";
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
    std::string mode = "";
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--chars") {
            mode = "chars";
        } else if (arg == "--ncurses") {
            mode = "ncurses";
        } else if (arg == "--engine") {
            mode = "engine";
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
    
    if (mode == "ncurses") {
        renderChessboardNcurses();
    } else if (mode == "chars") {
        renderChessboardChars();
    } else if (mode == "engine") {
        renderChessboardCharsEngine();
    } else {
        renderChessboardSDL();
    }
    
    return 0;
}
