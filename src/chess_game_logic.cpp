// Chess Game Logic Implementation
#include "chess_game_logic.h"
#include <iostream>

ChessGame::ChessGame() : whiteTurn(true) {
    initializeBoard();
}

const ChessPiece& ChessGame::getPiece(int row, int col) const {
    return board[row][col];
}

bool ChessGame::isWhiteTurn() const {
    return whiteTurn;
}

const std::vector<std::string>& ChessGame::getMoveHistory() const {
    return moveHistory;
}

std::string ChessGame::toChessNotation(int row, int col) const {
    std::string notation;
    notation += 'a' + col;
    notation += '8' - row;
    return notation;
}

bool ChessGame::fromChessNotation(const std::string& notation, int& row, int& col) const {
    if (notation.length() != 2) return false;
    
    col = notation[0] - 'a';
    row = '8' - notation[1];
    
    return (col >= 0 && col < 8 && row >= 0 && row < 8);
}

bool ChessGame::fromChessMoveNotation(const std::string& moveNotation, int& fromRow, int& fromCol, int& toRow, int& toCol) const {
    if (moveNotation.length() != 4) return false;
    
    // Extract source and destination notations
    std::string fromNotation = moveNotation.substr(0, 2);
    std::string toNotation = moveNotation.substr(2, 2);
    
    // Convert using existing fromChessNotation function
    if (!fromChessNotation(fromNotation, fromRow, fromCol)) return false;
    if (!fromChessNotation(toNotation, toRow, toCol)) return false;
    
    return true;
}

void ChessGame::initializeBoard() {
    // Copy standard board
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            board[row][col] = STANDARD_BOARD[row][col];
        }
    }
}

bool ChessGame::isValidMove(bool& isCastling, int fromRow, int fromCol, int toRow, int toCol) const {
    // Can't move to the same square
    if (fromRow == toRow && fromCol == toCol) return false;
    
    const ChessPiece& fromPiece = board[fromRow][fromCol];
    const ChessPiece& toPiece = board[toRow][toCol];
    
    // Can't move empty squares
    if (fromPiece.isEmpty()) return false;
    
    // Can't capture own pieces
    if (!toPiece.isEmpty() && fromPiece.color == toPiece.color) return false;
    
    // Pawn move validation
    if (fromPiece.type == PieceType::PAWN) {
        int direction = (fromPiece.color == PieceColor::WHITE) ? -1 : 1;
        int startRow = (fromPiece.color == PieceColor::WHITE) ? 6 : 1;
        
        // Forward move (1 square)
        if (fromCol == toCol && toRow == fromRow + direction && toPiece.isEmpty()) {
            return true;
        }
        
        // Initial double move (2 squares)
        if (fromCol == toCol && fromRow == startRow && 
            toRow == fromRow + 2 * direction && toPiece.isEmpty() && 
            board[fromRow + direction][fromCol].isEmpty()) {
            return true;
        }
        
        // Capture (diagonal)
        if (abs(fromCol - toCol) == 1 && toRow == fromRow + direction && 
            !toPiece.isEmpty() && toPiece.color != fromPiece.color) {
            return true;
        }
        
        // TODO: Add en passant and promotion logic
        return false;
    }

    if (fromPiece.type == PieceType::KING) {
        std::cout << "[KEYH] King move.." << std::endl;
        // Check for castling - king moves two squares horizontally
        if (fromRow == toRow && abs(fromCol - toCol) == 2) {
            // Castling conditions:
            // 1. King must not have moved before
            // 2. Rook must not have moved before
            // 3. No pieces between king and rook
            // 4. King must not be in check
            // 5. King must not pass through check
            // 6. King must not end up in check
            
            // Determine if kingside or queenside castling
            bool isKingside = (toCol > fromCol);
            int rookCol = isKingside ? 7 : 0;
            int direction = isKingside ? 1 : -1;
            
            // Check if king and rook are in starting positions
            if (fromPiece.hasMoved) return false;
            std::cout << "[KEYH] King never moved.." << std::endl;
            
            const ChessPiece& rook = board[fromRow][rookCol];
            if (rook.isEmpty() || rook.type != PieceType::ROOK || rook.hasMoved || rook.color != fromPiece.color) {
                return false;
            }
            std::cout << "[KEYH] Rook castelling never moved.." << std::endl;
            
            // Check that squares between king and rook are empty
            for (int col = fromCol + direction; col != rookCol; col += direction) {
                if (!board[fromRow][col].isEmpty()) {
                    return false;
                }
            }
            std::cout << "[KEYH] Castelling line is free.." << std::endl;
            std::cout << "[KEYH] Castelling Approved" << std::endl;
            
            // Check that king is not currently in check
            // if (isInCheck(fromPiece.color)) {
                // return false;
            // }
            
            // Check that king doesn't pass through check (square king moves over)
            // int intermediateCol = fromCol + direction;
            // if (wouldBeInCheck(fromRow, intermediateCol, fromPiece.color)) {
                // return false;
            // }
            
            // Check that king doesn't end up in check
            // if (wouldBeInCheck(toRow, toCol, fromPiece.color)) {
                // return false;
            // }

            // Mark castling in the piece (this will be handled in movePiece)
            // Note: We need to modify the piece, but this is a const method
            // We'll handle this in movePiece instead
            isCastling = true;
            return true;
        }
        
        // Regular king move (one square in any direction)
        if (abs(fromRow - toRow) <= 1 && abs(fromCol - toCol) <= 1) {
            // Mark king as moved (handled in movePiece)
            return true;
        }
        
        return false;
    }
    
    // Knight move validation
    if (fromPiece.type == PieceType::KNIGHT) {
        // Knight moves in L-shape: 2 squares in one direction, 1 square perpendicular
        int rowDiff = abs(fromRow - toRow);
        int colDiff = abs(fromCol - toCol);
        
        // Valid knight moves: (2,1) or (1,2)
        if ((rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2)) {
            return true;
        }
        
        return false;
    }
    
    // Bishop move validation
    if (fromPiece.type == PieceType::BISHOP) {
        // Bishop moves diagonally - row and column differences must be equal
        int rowDiff = abs(fromRow - toRow);
        int colDiff = abs(fromCol - toCol);
        
        // Valid bishop moves: same absolute difference in rows and columns
        if (rowDiff == colDiff && rowDiff > 0) {
            // Check for pieces blocking the diagonal path
            int rowStep = (toRow > fromRow) ? 1 : -1;
            int colStep = (toCol > fromCol) ? 1 : -1;
            
            int currentRow = fromRow + rowStep;
            int currentCol = fromCol + colStep;
            
            // Check all squares along the diagonal path
            while (currentRow != toRow && currentCol != toCol) {
                if (board[currentRow][currentCol].type != PieceType::NONE) {
                    return false; // Path is blocked
                }
                currentRow += rowStep;
                currentCol += colStep;
            }
            
            return true;
        }
        
        return false;
    }
    // Rook move validation
    if (fromPiece.type == PieceType::ROOK) {
        // Rook moves in straight lines only
        bool isHorizontalMove = (fromRow == toRow && fromCol != toCol);
        bool isVerticalMove = (fromCol == toCol && fromRow != toRow);
        
        if (!isHorizontalMove && !isVerticalMove) {
            return false;
        }
        
        // Check for pieces blocking the path
        int rowStep = 0;
        int colStep = 0;
        
        if (fromRow != toRow) {
            rowStep = (toRow > fromRow) ? 1 : -1;
        }
        if (fromCol != toCol) {
            colStep = (toCol > fromCol) ? 1 : -1;
        }
        
        int currentRow = fromRow + rowStep;
        int currentCol = fromCol + colStep;
        
        // Check all squares along the path
        while (currentRow != toRow || currentCol != toCol) {
            if (board[currentRow][currentCol].type != PieceType::NONE) {
                return false; // Path is blocked
            }
            currentRow += rowStep;
            currentCol += colStep;
        }
        
        return true;
    }
    if (fromPiece.type == PieceType::QUEEN) {
        int rowDiff = abs(fromRow - toRow);
        int colDiff = abs(fromCol - toCol);
        
        // Queen moves like a rook (straight lines) or bishop (diagonals)
        bool isStraightMove = (fromRow == toRow || fromCol == toCol);
        bool isDiagonalMove = (rowDiff == colDiff && rowDiff > 0);
        
        if (!isStraightMove && !isDiagonalMove) {
            return false;
        }
        
        // Check for pieces blocking the path
        int rowStep = 0;
        int colStep = 0;
        
        if (fromRow != toRow) {
            rowStep = (toRow > fromRow) ? 1 : -1;
        }
        if (fromCol != toCol) {
            colStep = (toCol > fromCol) ? 1 : -1;
        }
        
        int currentRow = fromRow + rowStep;
        int currentCol = fromCol + colStep;
        
        // Check all squares along the path
        while (currentRow != toRow || currentCol != toCol) {
            if (board[currentRow][currentCol].type != PieceType::NONE) {
                return false; // Path is blocked
            }
            currentRow += rowStep;
            currentCol += colStep;
        }
        
        return true;
    }
    // For other pieces, allow any move (basic implementation)
    return true;
}

void ChessGame::handleCastling(int fromRow, int fromCol, int toRow, int toCol) {
  ChessPiece& fromPiece = board[fromRow][fromCol];
  // Determine castling direction
  bool isKingside = (toCol > fromCol);
  int rookCol = isKingside ? 7 : 0;
  int direction = isKingside ? 1 : -1;

  // Move the rook
  if (isKingside) {
    board[toRow][toCol - 1] = board[fromRow][toCol+1];  // move rook king side
    board[fromRow][toCol + 1] = ChessPiece();
  } else {
    board[toRow][toCol + 1] = board[fromRow][toCol - 2];  // move rook queen side
    board[fromRow][toCol - 2] = ChessPiece();
  }
}

bool ChessGame::movePiece(int fromRow, int fromCol, int toRow, int toCol) {
    bool isCastling = false;
    if (!isValidMove(isCastling, fromRow, fromCol, toRow, toCol)) {
        return false;
    }
    
    ChessPiece& fromPiece = board[fromRow][fromCol];
    ChessPiece toPiece   = board[toRow][toCol];
    
    // Handle castling
    if (fromPiece.type == PieceType::KING && isCastling) {
        handleCastling(fromRow, fromCol, toRow, toCol);
    }
    
    // Mark king as moved if it moves
    if (fromPiece.type == PieceType::KING) {
        fromPiece.hasMoved = true;
    }
    
    // Record the move in chess notation
    std::string move = toChessNotation(fromRow, fromCol) + toChessNotation(toRow, toCol);
    
    // Perform the move
    if (!toPiece.isEmpty() && whiteTurn) whiteCapturedPieces.push_back(toPiece);
    else if (!toPiece.isEmpty() && !whiteTurn) blackCapturedPieces.push_back(toPiece);
    
    board[toRow][toCol] = fromPiece;
    board[fromRow][fromCol] = ChessPiece(); // Empty square
   
    moveHistory.push_back(move);
    if (whiteTurn) {
      pending_move = move;
    }
    
    // Switch turns
    whiteTurn = !whiteTurn;
    
    return true;
}

void ChessGame::resetGame() {
    whiteCapturedPieces.clear();
    blackCapturedPieces.clear();
    initializeBoard();
    moveHistory.clear();
    whiteTurn = true;
}

