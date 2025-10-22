#include <iostream>
#include <string>

void renderChessboardChars() {
    std::cout << "\n";
    std::cout << "  ASCII Chessboard\n";
    std::cout << "  a b c d e f g h\n";
    
    for (int row = 8; row >= 1; --row) {
        std::cout << row << " ";
        for (int col = 1; col <= 8; ++col) {
            // Determine piece based on position
            char piece = ' ';
            if (row == 1 || row == 8) {
                // Back rank pieces
                if (col == 1 || col == 8) piece = 'R';
                else if (col == 2 || col == 7) piece = 'N';
                else if (col == 3 || col == 6) piece = 'B';
                else if (col == 4) piece = 'Q';
                else if (col == 5) piece = 'K';
            } else if (row == 2 || row == 7) {
                // Pawns
                piece = 'P';
            }
            
            // Color pieces based on side
            if (row <= 2) piece = std::tolower(piece); // Black pieces (lowercase)
            
            // Print square with background
            if ((row + col) % 2 == 0) {
                std::cout << "\033[47m" << piece << " \033[0m"; // White background
            } else {
                std::cout << "\033[40m" << piece << " \033[0m"; // Black background
            }
        }
        std::cout << " " << row << "\n";
    }
    
    std::cout << "  a b c d e f g h\n";
    std::cout << "\n";
}
