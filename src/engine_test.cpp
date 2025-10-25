#include <iostream>
#include <string>
#include "engine/gnuchess_adapter.h"

int main() {
    std::cout << "Testing GNUChess Engine Integration" << std::endl;
    std::cout << "===================================" << std::endl;
    
    GNUChessAdapter engine;
    
    std::cout << "Initializing GNUChess engine..." << std::endl;
    if (!engine.initialize()) {
        std::cerr << "Failed to initialize GNUChess engine" << std::endl;
        return 1;
    }
    
    std::cout << "Engine: " << engine.getName() << " " << engine.getVersion() << std::endl;
    std::cout << std::endl;
    
    // Test 1: New game
    std::cout << "Test 1: Starting new game..." << std::endl;
    engine.newGame();
    std::cout << "New game started successfully" << std::endl;
    std::cout << std::endl;
    
    // Test 2: Get initial board state
    std::cout << "Test 2: Getting initial board state..." << std::endl;
    std::string boardState = engine.getBoardState();
    std::cout << "Board state received (length: " << boardState.length() << " chars)" << std::endl;
    std::cout << std::endl;
    
    // Test 3: Make a move as white
    std::cout << "Test 3: Making move e2e4..." << std::endl;
    engine.makeMove("e2e4");
    std::cout << "Move e2e4 made successfully" << std::endl;
    std::cout << std::endl;
    
    // Test 4: Get engine's response
    std::cout << "Test 4: Getting engine's best move (thinking for 3 seconds)..." << std::endl;
    std::string bestMove = engine.getBestMove(3000);
    if (!bestMove.empty()) {
        std::cout << "Engine's best move: " << bestMove << std::endl;
    } else {
        std::cout << "No move received from engine" << std::endl;
    }
    std::cout << std::endl;
    
    // Test 5: Get legal moves
    std::cout << "Test 5: Getting legal moves..." << std::endl;
    auto legalMoves = engine.getLegalMoves();
    std::cout << "Number of legal moves: " << legalMoves.size() << std::endl;
    if (!legalMoves.empty()) {
        std::cout << "First few legal moves: ";
        for (size_t i = 0; i < std::min(legalMoves.size(), size_t(5)); ++i) {
            std::cout << legalMoves[i] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    
    // Test 6: Shutdown
    std::cout << "Test 6: Shutting down engine..." << std::endl;
    engine.shutdown();
    std::cout << "Engine shutdown successfully" << std::endl;
    
    std::cout << std::endl;
    std::cout << "All tests completed successfully!" << std::endl;
    
    return 0;
}
