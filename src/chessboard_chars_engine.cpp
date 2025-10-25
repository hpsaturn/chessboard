#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include "chess_pieces.h"
#include "engine/gnuchess_adapter.h"

class CharEngineInterface {
private:
    GNUChessAdapter engine;
    std::vector<std::string> moveHistory;
    std::string lastMove;
    std::string engineFeedback;
    ChessPiece currentBoard[8][8];
    
public:
    CharEngineInterface() {
        // Initialize current board with standard position
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                currentBoard[row][col] = STANDARD_BOARD[row][col];
            }
        }
        
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
                char pieceSymbol = getPieceSymbol(currentBoard[row][col]);
                
                // Add color formatting for better readability
                if (currentBoard[row][col].color == PieceColor::WHITE && !currentBoard[row][col].isEmpty()) {
                    std::cout << "\033[1;37m" << pieceSymbol << "\033[0m"; // Bold white
                } else if (currentBoard[row][col].color == PieceColor::BLACK && !currentBoard[row][col].isEmpty()) {
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
    }
    
    void renderLastMove() {
        if (!lastMove.empty()) {
            std::cout << "\nLast Move: " << lastMove << "\n";
        }
    }
    
    void renderEngineFeedback() {
        std::cout << "Engine: " << engineFeedback << "\n";
    }
    
    void animateMove(const std::string& move) {
        // Extract source and destination squares
        if (move.length() < 4) return;
        
        std::string from = move.substr(0, 2);
        std::string to = move.substr(2, 2);
        
        // Convert algebraic notation to board coordinates
        int fromCol = from[0] - 'a';
        int fromRow = 8 - (from[1] - '0');
        int toCol = to[0] - 'a';
        int toRow = 8 - (to[1] - '0');
        
        // Get the piece being moved
        ChessPiece movingPiece = currentBoard[fromRow][fromCol];
        
        // Clear the source square
        currentBoard[fromRow][fromCol] = ChessPiece();
        
        // Update last move information
        lastMove = (movingPiece.color == PieceColor::WHITE ? "White: " : "Black: ") + move;
        
        // Render intermediate state (empty source square)
        std::cout << "\033[2J\033[H"; // Clear screen and move cursor to top
        renderChessboard();
        renderLastMove();
        renderEngineFeedback();
        std::cout.flush();
        
        // Small delay for animation effect
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        
        // Place piece on destination square
        currentBoard[toRow][toCol] = movingPiece;
        
        // Render final state
        std::cout << "\033[2J\033[H"; // Clear screen and move cursor to top
        renderChessboard();
        renderLastMove();
        renderEngineFeedback();
        std::cout.flush();
    }
    
    bool makeMove(const std::string& move) {
        try {
            // Animate the move
            animateMove(move);
            
            // Send move to engine
            engine.makeMove(move);
            moveHistory.push_back(move);
            engineFeedback = "Move accepted: " + move;
            
            // Get engine's response
            std::string engineMove = engine.getBestMove();
            if (!engineMove.empty() && engineMove != "(none)") {
                // Animate engine's move
                animateMove(engineMove);
                moveHistory.push_back("Engine: " + engineMove);
                engineFeedback = "Engine played: " + engineMove;
                lastMove = "Black: " + engineMove;
            }
            return true;
        } catch (const std::exception& e) {
            engineFeedback = "Invalid move: " + move + " (" + e.what() + ")";
            return false;
        }
    }
    
    void playTestGame() {
        std::cout << "\033[2J\033[H"; // Clear screen and move cursor to top
        std::cout << "=== Testing GNUChess Integration ===\n";
        std::cout << "Playing 5 moves against GNUChess...\n\n";
        
        // Initial render
        renderChessboard();
        renderEngineFeedback();
        
        // Test moves
        std::vector<std::string> testMoves = {
            "e2e4", "e7e5", "g1f3", "b8c6", "f1c4"
        };
        
        for (const auto& move : testMoves) {
            std::cout << "\nMaking move: " << move << "\n";
            if (makeMove(move)) {
                std::cout << "---\n";
            } else {
                std::cout << "Failed to make move: " << move << "\n";
                break;
            }
        }
        
        std::cout << "\nTest game completed!\n";
    }
};

void renderChessboardCharsEngine() {
    CharEngineInterface engineInterface;
    engineInterface.playTestGame();
}
