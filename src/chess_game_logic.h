// Chess Game Logic - Separated from SDL rendering
#ifndef CHESS_GAME_LOGIC_H
#define CHESS_GAME_LOGIC_H

#include "chess_pieces.h"
#include <string>
#include <vector>

class ChessGame {
private:
    ChessPiece board[8][8];
    std::vector<std::string> moveHistory;
    bool whiteTurn;
    
public:
    ChessGame();
    
    // Game state access
    const ChessPiece& getPiece(int row, int col) const;
    bool isWhiteTurn() const;
    const std::vector<std::string>& getMoveHistory() const;
    std::string pending_move = "";
    
    // Coordinate conversion
    std::string toChessNotation(int row, int col) const;
    bool fromChessNotation(const std::string& notation, int& row, int& col) const;
    bool fromChessMoveNotation(const std::string& moveNotation, int& fromRow, int& fromCol, int& toRow, int& toCol) const;
    
    // Game logic
    void initializeBoard();
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol) const;
    void handleCastling(int fromRow, int fromCol, int toRow, int toCol);
    bool movePiece(int fromRow, int fromCol, int toRow, int toCol);
    void resetGame();
};

#endif // CHESS_GAME_LOGIC_H
