#ifndef CHESS_ENGINE_H
#define CHESS_ENGINE_H

#include <string>
#include <vector>

class ChessEngine {
public:
    virtual ~ChessEngine() = default;
    
    // Engine control
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    // Game control
    virtual void newGame() = 0;
    virtual void setPosition(const std::string& fen) = 0;
    virtual void makeMove(const std::string& move) = 0;
    
    // Engine interaction
    virtual std::string getBestMove(int thinkTimeMs = 3000) = 0;
    virtual std::vector<std::string> getLegalMoves() = 0;
    virtual std::string getBoardState() = 0;
    
    // Engine info
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
};

#endif // CHESS_ENGINE_H
