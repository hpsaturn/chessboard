#include "chess_game_logic.h"
#include <iostream>
#include <algorithm>

ChessGame::ChessGame() : whiteTurn(true), fenMode(false) {
    initializeBoard();
}

void ChessGame::clearBoard() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = ChessPiece();
        }
    }
}

void ChessGame::loadCapturedPieces() {
    // Implementation for loading captured pieces
    // This would typically load from a file or database
}

const ChessPiece& ChessGame::getPiece(int row, int col) const {
    return board[row][col];
}

bool ChessGame::isWhiteTurn() const {
    return whiteTurn;
}

bool ChessGame::isFenMode() const {
    return fenMode;
}

const std::vector<std::string>& ChessGame::getMoveHistory() const {
    return moveHistory;
}

std::string ChessGame::toChessNotation(int row, int col) const {
    char file = 'a' + col;
    char rank = '8' - row;
    return std::string(1, file) + std::string(1, rank);
}

bool ChessGame::fromChessNotation(const std::string& notation, int& row, int& col) const {
    if (notation.length() != 2) return false;
    
    char file = notation[0];
    char rank = notation[1];
    
    if (file < 'a' || file > 'h' || rank < '1' || rank > '8') return false;
    
    col = file - 'a';
    row = '8' - rank;
    
    return true;
}

bool ChessGame::fromChessMoveNotation(const std::string& moveNotation, int& fromRow, int& fromCol, int& toRow, int& toCol) const {
    if (moveNotation.length() != 4) return false;
    
    std::string from = moveNotation.substr(0, 2);
    std::string to = moveNotation.substr(2, 2);
    
    return fromChessNotation(from, fromRow, fromCol) && fromChessNotation(to, toRow, toCol);
}

void ChessGame::initializeBoard(const std::string& fen) {
    // Implementation for initializing the board
    // This would set up the pieces in their starting positions
    clearBoard();
    
    // Set up pawns
    for (int i = 0; i < 8; i++) {
        board[1][i] = ChessPiece(PieceType::PAWN, PieceColor::BLACK); // Black pawns
        board[6][i] = ChessPiece(PieceType::PAWN, PieceColor::WHITE); // White pawns
    }
    
    // Set up other pieces
    // Rooks
    board[0][0] = ChessPiece(PieceType::ROOK, PieceColor::BLACK); 
    board[0][7] = ChessPiece(PieceType::ROOK, PieceColor::BLACK);
    board[7][0] = ChessPiece(PieceType::ROOK, PieceColor::WHITE);  
    board[7][7] = ChessPiece(PieceType::ROOK, PieceColor::WHITE);
    
    // Knights
    board[0][1] = ChessPiece(PieceType::KNIGHT, PieceColor::BLACK); 
    board[0][6] = ChessPiece(PieceType::KNIGHT, PieceColor::BLACK);
    board[7][1] = ChessPiece(PieceType::KNIGHT, PieceColor::WHITE);  
    board[7][6] = ChessPiece(PieceType::KNIGHT, PieceColor::WHITE);
    
    // Bishops
    board[0][2] = ChessPiece(PieceType::BISHOP, PieceColor::BLACK); 
    board[0][5] = ChessPiece(PieceType::BISHOP, PieceColor::BLACK);
    board[7][2] = ChessPiece(PieceType::BISHOP, PieceColor::WHITE);  
    board[7][5] = ChessPiece(PieceType::BISHOP, PieceColor::WHITE);
    
    // Queens
    board[0][3] = ChessPiece(PieceType::QUEEN, PieceColor::BLACK);
    board[7][3] = ChessPiece(PieceType::QUEEN, PieceColor::WHITE);
    
    // Kings
    board[0][4] = ChessPiece(PieceType::KING, PieceColor::BLACK);
    board[7][4] = ChessPiece(PieceType::KING, PieceColor::WHITE);
}

bool ChessGame::isValidMove(bool& isCastling, int fromRow, int fromCol, int toRow, int toCol) const {
    // Implementation for validating moves
    // This would check if the move is legal according to chess rules
    
    // Basic validation
    if (fromRow < 0 || fromRow > 7 || fromCol < 0 || fromCol > 7 ||
        toRow < 0 || toRow > 7 || toCol < 0 || toCol > 7) {
        return false;
    }
    
    // Check if there's a piece at the source
    if (board[fromRow][fromCol].isEmpty()) {
        return false;
    }
    
    // Check if it's the correct player's turn
    if ((board[fromRow][fromCol].color == PieceColor::WHITE) != whiteTurn) {
        return false;
    }
    
    // For now, allow any move to an empty square or capture
    // In a real implementation, this would check piece-specific movement rules
    return true;
}

bool ChessGame::isInCheck(const ChessPiece& king) const {
    // Implementation for checking if the king is in check
    // This would check if any opponent piece can capture the king
    return false; // Simplified for now
}

bool ChessGame::wouldBeInCheck(int fromRow, int toRow, int toCol, const ChessPiece& king) const {
    // Implementation for checking if a move would put the king in check
    return false; // Simplified for now
}

void ChessGame::handleCastling(int fromRow, int fromCol, int toRow, int toCol) {
    // Implementation for handling castling
}

bool ChessGame::movePiece(int fromRow, int fromCol, int toRow, int toCol) {
    bool isCastling = false;
    
    if (!isValidMove(isCastling, fromRow, fromCol, toRow, toCol)) {
        return false;
    }
    
    // Handle captures
    if (!board[toRow][toCol].isEmpty()) {
        // Add to captured pieces
        if (board[toRow][toCol].color == PieceColor::WHITE) {
            whiteCapturedPieces.push_back(board[toRow][toCol]);
        } else {
            blackCapturedPieces.push_back(board[toRow][toCol]);
        }
    }
    
    // Move the piece
    board[toRow][toCol] = board[fromRow][fromCol];
    board[fromRow][fromCol] = ChessPiece();
    
    // Handle castling if applicable
    if (isCastling) {
        handleCastling(fromRow, fromCol, toRow, toCol);
    }
    
    // Add to move history
    std::string move = toChessNotation(fromRow, fromCol) + toChessNotation(toRow, toCol);
    moveHistory.push_back(move);
    
    // Switch turns
    whiteTurn = !whiteTurn;
    
    return true;
}

void ChessGame::resetGame() {
    initializeBoard();
    moveHistory.clear();
    whiteCapturedPieces.clear();
    blackCapturedPieces.clear();
    whiteTurn = true;
    selectedRow = -1;
    selectedCol = -1;
}

std::string ChessGame::boardToFEN() const {
    // Implementation for converting board to FEN notation
    return ""; // Simplified for now
}

// Selection methods implementation
void ChessGame::selectPiece(int row, int col) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8 && !board[row][col].isEmpty()) {
        selectedRow = row;
        selectedCol = col;
    }
}

void ChessGame::deselectPiece() {
    selectedRow = -1;
    selectedCol = -1;
}

bool ChessGame::isPieceSelected() const {
    return selectedRow != -1 && selectedCol != -1;
}

int ChessGame::getSelectedRow() const {
    return selectedRow;
}

int ChessGame::getSelectedCol() const {
    return selectedCol;
}

std::vector<std::pair<int, int>> ChessGame::getValidMoves(int row, int col) const {
    std::vector<std::pair<int, int>> validMoves;
    
    // Simplified implementation - in a real chess game, this would check
    // piece-specific movement rules and check for checks
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            bool isCastling = false;
            if (isValidMove(isCastling, row, col, r, c)) {
                validMoves.push_back({r, c});
            }
        }
    }
    
    return validMoves;
}

char ChessGame::getPieceAt(int row, int col) const {
    if (row >= 0 && row < 8 && col >= 0 && col < 8 && !board[row][col].isEmpty()) {
        return getPieceChar(board[row][col]);
    }
    return ' ';
}
