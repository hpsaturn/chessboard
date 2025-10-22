#include <iostream>
#include <vector>
#include <cstdint>

const int BOARD_SIZE = 8;

class ChessboardRenderer {
private:
    std::vector<std::vector<char>> board;
    
public:
    ChessboardRenderer() {
        board.resize(BOARD_SIZE, std::vector<char>(BOARD_SIZE, ' '));
    }
    
    void renderChessboard() {
        // Render chessboard pattern
        for (int row = 0; row < BOARD_SIZE; ++row) {
            for (int col = 0; col < BOARD_SIZE; ++col) {
                // Alternate colors for chessboard pattern
                if ((row + col) % 2 == 0) {
                    board[row][col] = '#'; // White square
                } else {
                    board[row][col] = '.'; // Dark square
                }
            }
        }
    }
    
    void display() {
        std::cout << "\nChessboard ASCII Representation:\n";
        std::cout << "  a b c d e f g h\n";
        
        for (int row = 0; row < BOARD_SIZE; ++row) {
            std::cout << 8 - row << " ";
            for (int col = 0; col < BOARD_SIZE; ++col) {
                std::cout << board[row][col] << " ";
            }
            std::cout << 8 - row << "\n";
        }
        
        std::cout << "  a b c d e f g h\n";
    }
    
    void run() {
        renderChessboard();
        display();
        
        std::cout << "\nChessboard rendered successfully!\n";
        std::cout << "Legend: # = White square, . = Dark square\n";
    }
};

int main(int argc, char* argv[]) {
    ChessboardRenderer chessboard;
    chessboard.run();
    return 0;
}
