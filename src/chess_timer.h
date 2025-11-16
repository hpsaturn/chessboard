#ifndef CHESSTIMER_H
#define CHESSTIMER_H

#include <chrono>
#include <string>

class ChessTimer {
private:
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Seconds = std::chrono::seconds;
    using Milliseconds = std::chrono::milliseconds;

    // Player time remaining in milliseconds
    int64_t whiteTimeRemaining;
    int64_t blackTimeRemaining;
    // Game user time
    int minutesPerPlayer;
    
    // Game state
    TimePoint gameStartTime;
    TimePoint lastUpdateTime;
    bool isWhiteTurn;
    bool gameActive;

    // Private helper methods
    void updateTimers();
    std::string formatTime(int64_t milliseconds);

public:
    // Constructor - takes time in minutes
    ChessTimer(int minutesPerPlayer = 10);

    // Public interface methods
    std::string getWhiteTimer();
    std::string getBlackTimer();
    void startGame();
    void resetGame();
    void switchTurn();
    bool isTimeUp();
    std::string getCurrentPlayer();
    void pauseGame();
    void resumeGame();
};

#endif // CHESSTIMER_H
