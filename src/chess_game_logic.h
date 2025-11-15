// Chess Game Logic - Separated from SDL rendering
#ifndef CHESS_GAME_LOGIC_H
#define CHESS_GAME_LOGIC_H

#include "engine/bitboard.h"
#include "chess_pieces.h"
#include <string>
#include <vector>
#include <ctime>

class ChessGame {
private:
    ChessPiece board[8][8];
    std::vector<std::string> moveHistory;
    bool whiteTurn, fenMode;
    int pointsWhite = 0;
    int pointsBlack = 0;

    // Game timestamp init
    time_t gameStartTime;
    
    // Captured pieces tracking
    std::vector<ChessPiece> whiteCapturedPieces;
    std::vector<ChessPiece> blackCapturedPieces;
 
    void clearBoard();
    void loadCapturedPieces();
    void calculatePoints();
    void startTimers();
    void stopTimers();
    void updateTimers();
    void resetTimers();

public:
    ChessGame();
    
    // Game state access
    const ChessPiece& getPiece(int row, int col) const;
    bool isWhiteTurn() const;
    bool isFenMode() const;
    const std::vector<std::string>& getMoveHistory() const;
    std::string pending_move = "";
    
    // Get captured pieces
    const std::vector<ChessPiece>& getWhiteCapturedPieces() const { return whiteCapturedPieces; }
    const std::vector<ChessPiece>& getBlackCapturedPieces() const { return blackCapturedPieces; }
    int getPointsWhite() const { return pointsWhite; }
    int getPointsBlack() const { return pointsBlack; }

    // Timers methods
    std::string getWhiteTimer() const;
    std::string getBlackTimer() const;
    
    // Coordinate conversion
    std::string toChessNotation(int row, int col) const;
    bool fromChessNotation(const std::string& notation, int& row, int& col) const;
    bool fromChessMoveNotation(const std::string& moveNotation, int& fromRow, int& fromCol, int& toRow, int& toCol) const;
    
    // Game logic
    void initializeBoard(const std::string& fen = "");
    bool isValidMove(bool& isCastling, int fromRow, int fromCol, int toRow, int toCol) const;
    bool isInCheck(int& kingRow, int& kingCol) const;
    bool would_move_leave_king_in_check(int fromRow, int fromCol, int toRow, int toCol) const;
    void handleCastling(int fromRow, int fromCol, int toRow, int toCol);
    bool movePiece(int fromRow, int fromCol, int toRow, int toCol);
    void resetGame();
    std::string boardToFEN() const;
};

#endif // CHESS_GAME_LOGIC_H
