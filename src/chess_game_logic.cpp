// Chess Game Logic Implementation
#include "chess_game_logic.h"
#include <iostream>
#include <sstream>  
#include <cctype>   
#include <string>   
#include <iomanip>

ChessGame::ChessGame() : whiteTurn(true) {
    initializeBoard();
    resetTimers();
}

const ChessPiece& ChessGame::getPiece(int row, int col) const {
    return board[row][col];
}

bool ChessGame::isWhiteTurn() const {
    return whiteTurn;
}

bool ChessGame::isFenMode() const { 
  return fenMode; }

const std::vector<std::string>& ChessGame::getMoveHistory() const {
    return moveHistory;
}

void ChessGame::setTimeMatch(int seconds) {
  matchTime = seconds;
  resetTimers();
}

std::string ChessGame::getWhiteTimer() const {
  int currentTime = whiteTimeRemaining;
  if (timersRunning && whiteTurn) {
    time_t currentTimeValue = time(nullptr);
    currentTime = whiteTimeRemaining - (currentTimeValue - whiteTimerStart);
    if (currentTime < 0) currentTime = 0;
  }

  int minutes = currentTime / 60;
  int seconds = currentTime % 60;

  std::stringstream ss;
  ss << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0')
     << seconds;
  return ss.str();
}

std::string ChessGame::getBlackTimer() const {
    int currentTime = blackTimeRemaining;
    if (timersRunning && !whiteTurn) {
        time_t currentTimeValue = time(nullptr);
        currentTime = blackTimeRemaining - (currentTimeValue - blackTimerStart);
        if (currentTime < 0) currentTime = 0;
    }
    
    int minutes = currentTime / 60;
    int seconds = currentTime % 60;
    
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << minutes << ":" 
       << std::setw(2) << std::setfill('0') << seconds;
    return ss.str();
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

std::string ChessGame::boardToFEN() const {
  std::stringstream fen;

  // Piece placement
  for (int row = 0; row < 8; row++) {
    int emptyCount = 0;

    for (int col = 0; col < 8; col++) {
      const ChessPiece& piece = board[row][col];

      if (piece.type == PieceType::NONE) {
        emptyCount++;
      } else {
        if (emptyCount > 0) {
          fen << emptyCount;
          emptyCount = 0;
        }

        char pieceChar = getPieceChar(piece);
        fen << pieceChar;
      }
    }

    if (emptyCount > 0) {
      fen << emptyCount;
    }

    if (row < 7) {
      fen << '/';
    }
  }

  // Add other FEN components (you'll need to track these in your game state)
  fen << " b KQkq - 0 1";  // Default values for new game

  return fen.str();
}

void ChessGame::initializeBoard(const std::string& fen) {
  // Copy standard board
  if (fen.empty()) {
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        board[row][col] = STANDARD_BOARD[row][col];
      }
    }
  }
  else {
    clearBoard();
    std::istringstream fenStream(fen);
    std::string piecePlacement;
    fenStream >> piecePlacement;

    int row = 0;
    int col = 0;

    for (char c : piecePlacement) {
      if (c == '/') {
        // Move to next row
        row++;
        col = 0;
      } else if (std::isdigit(c)) {
        // Skip empty squares
        int emptySquares = c - '0';
        col += emptySquares;
      } else {
        // Place a piece
        ChessPiece piece = charToPiece(c);
        if (row < 8 && col < 8) {
          board[row][col] = piece;
        }
        col++;
      }
      if (row >= 8) break;
    }
    fenMode = true;
    loadCapturedPieces();
  }
}

void ChessGame::clearBoard() {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            board[row][col] = ChessPiece{};
        }
    }
}

void ChessGame::loadCapturedPieces() {
  whiteCapturedPieces.clear();
  blackCapturedPieces.clear();

  // Standard piece counts: [PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING]
  const int standardCounts[6] = {8, 2, 2, 2, 1, 1};

  int currentWhite[6] = {0};
  int currentBlack[6] = {0};

  // Count current pieces
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      ChessPiece piece = board[row][col];
      if (!piece.isEmpty()) {
        int typeIndex = static_cast<int>(piece.type) - 1;  // Convert to 0-based
        if (piece.color == PieceColor::WHITE) {
          currentWhite[typeIndex]++;
        } else {
          currentBlack[typeIndex]++;
        }
      }
    }
  }

  // Find captured pieces
  for (int i = 0; i < 6; i++) {
    PieceType pieceType = static_cast<PieceType>(i + 1);

    // White captured black pieces
    int missingBlack = standardCounts[i] - currentBlack[i];
    for (int j = 0; j < missingBlack; j++) {
      whiteCapturedPieces.push_back(ChessPiece(pieceType, PieceColor::BLACK));
    }

    // Black captured white pieces
    int missingWhite = standardCounts[i] - currentWhite[i];
    for (int j = 0; j < missingWhite; j++) {
      blackCapturedPieces.push_back(ChessPiece(pieceType, PieceColor::WHITE));
    }
  }
  // refresh points
  calculatePoints();
}

void ChessGame::calculatePoints() {
    pointsWhite = 0;
    pointsBlack = 0;

    auto pieceValue = [](PieceType type) {
        switch (type) {
            case PieceType::PAWN: return 1;
            case PieceType::KNIGHT: return 3;
            case PieceType::BISHOP: return 3;
            case PieceType::ROOK: return 5;
            case PieceType::QUEEN: return 9;
            default: return 0;
        }
    };

    for (const auto& piece : whiteCapturedPieces) {
        pointsWhite += pieceValue(piece.type);
    }

    for (const auto& piece : blackCapturedPieces) {
        pointsBlack += pieceValue(piece.type);
    }
}

void ChessGame::startTimers() {
  if (!timersRunning) {
    // Only start the timer for the current player
    if (whiteTurn) {
      whiteTimerStart = time(nullptr);
    } else {
      blackTimerStart = time(nullptr);
    }
    timersRunning = true;
  }
}

void ChessGame::stopTimers() {
    if (timersRunning) {
        // Update remaining time for the current player
        time_t currentTime = time(nullptr);
        if (whiteTurn) {
            whiteTimeRemaining -= (currentTime - whiteTimerStart);
            if (whiteTimeRemaining < 0) whiteTimeRemaining = 0;
        } else {
            blackTimeRemaining -= (currentTime - blackTimerStart);
            if (blackTimeRemaining < 0) blackTimeRemaining = 0;
        }
        timersRunning = false;
    }
}

void ChessGame::updateTimers() {
    if (timersRunning) {
        // This method can be called periodically to update timer displays
        // The actual time calculation is done in getWhiteTimer() and getBlackTimer()
        // This method could be used for additional timer-related logic if needed
    }
}

void ChessGame::resetTimers() {
    stopTimers();
    whiteTimeRemaining = matchTime; // 10 minutes in seconds
    blackTimeRemaining = matchTime; // 10 minutes in seconds
    timersRunning = false;
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
        std::cout << "[GAME] King move.." << std::endl;
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
            std::cout << "[GAME] King never moved.." << std::endl;
            
            const ChessPiece& rook = board[fromRow][rookCol];
            if (rook.isEmpty() || rook.type != PieceType::ROOK || rook.hasMoved || rook.color != fromPiece.color) {
                return false;
            }
            std::cout << "[GAME] Rook castelling never moved.." << std::endl;
            
            // Check that squares between king and rook are empty
            for (int col = fromCol + direction; col != rookCol; col += direction) {
                if (!board[fromRow][col].isEmpty()) {
                    return false;
                }
            }
            std::cout << "[GAME] Castelling line is free.." << std::endl;
            std::cout << "[GAME] Castelling Approved" << std::endl;
            std::cout << "[GAME] King valid move" << std::endl;

            // return -1 on invalid input
            
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

bool ChessGame::isInCheck(int& kingRow, int& kingCol) const { 
  ChessBoard bitboard;
  ChessBoard::Square king_pos = bitboard.set_custom_position(boardToFEN());
  std::pair<int, int> kingPos = bitboard.to_row_col(king_pos);
  kingRow = kingPos.first;
  kingCol = kingPos.second;
  bool check = bitboard.is_king_in_check(ChessBoard::Color::WHITE);
  std::cout << "[GAME] isInCheck: " << (check ? "yes" : "no") << "\n";
  return check;
}

bool ChessGame::would_move_leave_king_in_check(int fromRow, int fromCol, int toRow, int toCol) const {
  ChessBoard bitboard;
  ChessBoard::Square king_pos = bitboard.set_custom_position(boardToFEN());
  bool wcheck = bitboard.would_move_leave_king_in_check(
      bitboard.from_row_col(fromRow, fromCol),
      bitboard.from_row_col(toRow, toCol)
    );
  std::cout << "[GAME] would move leave king in check: " << (wcheck ? "yes" : "no") << "\n";
  return wcheck;
}

bool ChessGame::movePiece(int fromRow, int fromCol, int toRow, int toCol) {
    bool isCastling = false;
    if (!isValidMove(isCastling, fromRow, fromCol, toRow, toCol)) {
        return false;
    }

    if (whiteTurn &&  would_move_leave_king_in_check(fromRow, fromCol, toRow, toCol)) {
        std::cout << "[GAME] Move would leave king in check. Move invalid!" << std::endl;
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
    
    // Save captured pieces 
    if (!toPiece.isEmpty() && whiteTurn) whiteCapturedPieces.push_back(toPiece);
    else if (!toPiece.isEmpty() && !whiteTurn) blackCapturedPieces.push_back(toPiece);

    // Pawn coronation 
    if (fromPiece.type == PieceType::PAWN && (toRow == 0 || toRow == 7)) 
      std::cout << "[GAME] PAWN promoted!" << std::endl;
    if (fromPiece.type == PieceType::PAWN && whiteTurn && toRow == 0) fromPiece.type = PieceType::QUEEN;
    if (fromPiece.type == PieceType::PAWN && !whiteTurn && toRow == 7) fromPiece.type = PieceType::QUEEN;

    // Perform the move
    board[toRow][toCol] = fromPiece;
    board[fromRow][fromCol] = ChessPiece(); // Empty square
    // isInCheck();
 
    moveHistory.push_back(move);
    if (whiteTurn) {
      pending_move = move;
    }
    
    // Stop timer for current player before switching turns
    stopTimers();
    
    // Stop timer for current player before switching turns
    stopTimers();

    // Switch turns
    whiteTurn = !whiteTurn;
    calculatePoints();
    
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

void ChessGame::resetGame() {
    whiteCapturedPieces.clear();
    blackCapturedPieces.clear();
    initializeBoard();
    moveHistory.clear();
    whiteTurn = true;
    resetTimers();
    calculatePoints();
}
