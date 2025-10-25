#include <iostream>
#include <string>
#include <vector>
#include "chess_pieces.h"
#include "engine/gnuchess_adapter.h"

class CharEngineInterface {
private:
    GNUChessAdapter engine;
    std::vector<std::string> moveHistory;
    std::string engineFeedback;
    
public:
    CharEngineInterface() {
        if (!engine.initialize()) {
            engineFeedback = "ERROR: Failed to initialize GNUChess engine";
        } else {
            engineFeedback = "GNUChess engine ready. Enter moves (e.g., e2e4)";
        }
    }
    
    ~CharEngineInterface() {
        engine.shutdown();
    }
    
    void renderChessboard() {
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
    
    void renderMoveHistory() {
        std::cout << "Move History:\n";
        if (moveHistory.empty()) {
            std::cout << "  No moves yet\n";
        } else {
            for (size_t i = 0; i < moveHistory.size(); ++i) {
                std::cout << "  " << (i + 1) << ". " << moveHistory[i] << "\n";
            }
        }
        std::cout << "\n";
    }
    
    void renderEngineFeedback() {
        std::cout << "Engine: " << engineFeedback << "\n";
        std::cout << "\n";
    }
    
    bool makeMove(const std::string& move) {
        try {
            engine.makeMove(move);
            moveHistory.push_back(move);
            engineFeedback = "Move accepted: " + move;
            
            // Get engine's response
            std::string engineMove = engine.getBestMove();
            if (!engineMove.empty() && engineMove != "(none)") {
                moveHistory.push_back("Engine: " + engineMove);
                engineFeedback = "Engine played: " + engineMove;
            }
            return true;
        } catch (const std::exception& e) {
            engineFeedback = "Invalid move: " + move + " (" + e.what() + ")";
            return false;
        }
    }
    
    void playTestGame() {
        std::cout << "\n=== Testing GNUChess Integration ===\n";
        std::cout << "Playing 5 moves against GNUChess...\n\n";
        
        // Test moves
        std::vector<std::string> testMoves = {
            "e2e4", "e7e5", "g1f3", "b8c6", "f1c4"
        };
        
        for (const auto& move : testMoves) {
            std::cout << "Making move: " << move << "\n";
            if (makeMove(move)) {
                renderChessboard();
                renderMoveHistory();
                renderEngineFeedback();
                std::cout << "---\n";
            } else {
                std::cout << "Failed to make move: " << move << "\n";
                break;
            }
        }
        
        std::cout << "Test game completed!\n";
    }
};

void renderChessboardCharsEngine() {
    CharEngineInterface engineInterface;
    engineInterface.playTestGame();
}
