#ifndef GNUCHESS_ADAPTER_H
#define GNUCHESS_ADAPTER_H

#include "chess_engine.h"
#include <string>
#include <vector>

class GNUChessAdapter : public ChessEngine {
private:
    int engineProcess;
    int inputPipe;
    int outputPipe;
    
    void sendCommand(const std::string& command);
    std::string readOutput();
    void clearOutput();
    
public:
    GNUChessAdapter();
    ~GNUChessAdapter();
    
    // Engine control
    bool initialize() override;
    void shutdown() override;
    
    // Game control
    void newGame() override;
    void setPosition(const std::string& fen) override;
    void makeMove(const std::string& move) override;
    
    // Engine interaction
    std::string getBestMove(int thinkTimeMs = 3000) override;
    std::vector<std::string> getLegalMoves() override;
    std::string getBoardState() override;
    
    // Engine info
    std::string getName() const override;
    std::string getVersion() const override;
};

#endif // GNUCHESS_ADAPTER_H
