#include <iostream>
#include <string>
#include "chess_pieces.h"

void renderChessboardChars() {
    std::cout << "\n";
    std::cout << "  +---+---+---+---+---+---+---+---+\n";
    
    for (int row = 0; row < 8; ++row) {
        std::cout << 8 - row << " | ";
        
        for (int col = 0; col < 8; ++col) {
            char pieceSymbol = getPieceSymbol(STANDARD_BOARD[row][col]);
            
            // Add color formatting for better readability
            if (STANDARD_BOARD[row][col].color == PieceColor::WHITE && !STANDARD_BOARD[row][col].isEmpty()) {
                std::cout << "\033[1;37m" << pieceSymbol << "\033[0m"; // Bold white
            } else if (STANDARD_BOARD[row][col].color == PieceColor::BLACK && !STANDARD_BOARD[row][col].isEmpty()) {
                std::cout << "\033[1;36m" << pieceSymbol << "\033[0m"; // Bold cyan for black pieces
            } else {
                std::cout << pieceSymbol;
            }
            
            std::cout << " | ";
        }
        
        std::cout << "\n";
        std::cout << "  +---+---+---+---+---+---+---+---+\n";
    }
    
    std::cout << "    a   b   c   d   e   f   g   h\n";
    std::cout << "\n";
    
    // Display simplified piece legend without borders
    std::cout << "Piece Legend: ";
    std::cout << "White: P=Pawn, R=Rook, N=Knight, B=Bishop, Q=Queen, K=King | ";
    std::cout << "Black: p=Pawn, r=Rook, n=Knight, b=Bishop, q=Queen, k=King\n";
    std::cout << "\n";
}
