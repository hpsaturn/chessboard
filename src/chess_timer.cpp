#include "chess_timer.h"
#include <iomanip>
#include <sstream>

// Constructor
ChessTimer::ChessTimer(int minutesPerPlayer) 
    : whiteTimeRemaining(minutesPerPlayer * 60 * 1000),
      blackTimeRemaining(minutesPerPlayer * 60 * 1000),
      isWhiteTurn(true),
      gameActive(false)
{   
    this->minutesPerPlayer = minutesPerPlayer;
    gameStartTime = Clock::now();
    lastUpdateTime = gameStartTime;
}

// Get white player's remaining time as string "MM:SS"
std::string ChessTimer::getWhiteTimer() {
    updateTimers();
    return formatTime(whiteTimeRemaining);
}

// Get black player's remaining time as string "MM:SS"
std::string ChessTimer::getBlackTimer() {
    updateTimers();
    return formatTime(blackTimeRemaining);
}

// Start the game
void ChessTimer::startGame() {
    if (!gameActive) {
        gameActive = true;
        gameStartTime = Clock::now();
        lastUpdateTime = gameStartTime;
    }
}

// Reset the game with optional new time limit
void ChessTimer::resetGame() {
    whiteTimeRemaining = minutesPerPlayer * 60 * 1000;
    blackTimeRemaining = minutesPerPlayer * 60 * 1000;
    isWhiteTurn = true;
    gameActive = false;
    gameStartTime = Clock::now();
    lastUpdateTime = gameStartTime;
}

// Switch turn to the other player
void ChessTimer::switchTurn() {
    if (gameActive) {
        updateTimers();
        isWhiteTurn = !isWhiteTurn;
        lastUpdateTime = Clock::now();
    }
}

// Check if time has run out for current player
bool ChessTimer::isTimeUp() {
    updateTimers();
    if (isWhiteTurn) {
        return whiteTimeRemaining <= 0;
    } else {
        return blackTimeRemaining <= 0;
    }
}

// Get current player
std::string ChessTimer::getCurrentPlayer() {
    return isWhiteTurn ? "White" : "Black";
}

// Pause the game
void ChessTimer::pauseGame() {
    if (gameActive) {
        updateTimers();
        gameActive = false;
    }
}

// Resume the game
void ChessTimer::resumeGame() {
    if (!gameActive) {
        gameActive = true;
        lastUpdateTime = Clock::now();
    }
}

// Private: Update timers based on elapsed time
void ChessTimer::updateTimers() {
    if (!gameActive) return;

    auto currentTime = Clock::now();
    auto elapsed = std::chrono::duration_cast<Milliseconds>(currentTime - lastUpdateTime).count();
    
    if (isWhiteTurn) {
        whiteTimeRemaining -= elapsed;
        if (whiteTimeRemaining < 0) whiteTimeRemaining = 0;
    } else {
        blackTimeRemaining -= elapsed;
        if (blackTimeRemaining < 0) blackTimeRemaining = 0;
    }
    
    lastUpdateTime = currentTime;
}

// Private: Format milliseconds to "MM:SS" string
std::string ChessTimer::formatTime(int64_t milliseconds) {
    int totalSeconds = static_cast<int>(milliseconds / 1000);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds;
    return oss.str();
}
