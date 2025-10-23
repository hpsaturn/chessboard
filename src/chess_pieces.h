// Chess Pieces Sprite Assets
// This file contains chess piece definitions and rendering utilities

#ifndef CHESS_PIECES_H
#define CHESS_PIECES_H

#include <string>

// Chess piece types
enum class PieceType {
    NONE = 0,
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING
};

// Chess piece colors
enum class PieceColor {
    WHITE,
    BLACK
};

// Chess piece structure
struct ChessPiece {
    PieceType type;
    PieceColor color;
    
    ChessPiece(PieceType t = PieceType::NONE, PieceColor c = PieceColor::WHITE) 
        : type(t), color(c) {}
    
    bool isEmpty() const { return type == PieceType::NONE; }
};

// Standard chess starting position
const ChessPiece STANDARD_BOARD[8][8] = {
    // Rank 8 (Black pieces)
    { {PieceType::ROOK, PieceColor::BLACK}, {PieceType::KNIGHT, PieceColor::BLACK}, 
      {PieceType::BISHOP, PieceColor::BLACK}, {PieceType::QUEEN, PieceColor::BLACK}, 
      {PieceType::KING, PieceColor::BLACK}, {PieceType::BISHOP, PieceColor::BLACK}, 
      {PieceType::KNIGHT, PieceColor::BLACK}, {PieceType::ROOK, PieceColor::BLACK} },
    // Rank 7 (Black pawns)
    { {PieceType::PAWN, PieceColor::BLACK}, {PieceType::PAWN, PieceColor::BLACK}, 
      {PieceType::PAWN, PieceColor::BLACK}, {PieceType::PAWN, PieceColor::BLACK}, 
      {PieceType::PAWN, PieceColor::BLACK}, {PieceType::PAWN, PieceColor::BLACK}, 
      {PieceType::PAWN, PieceColor::BLACK}, {PieceType::PAWN, PieceColor::BLACK} },
    // Rank 6-3 (Empty squares)
    { {}, {}, {}, {}, {}, {}, {}, {} },
    { {}, {}, {}, {}, {}, {}, {}, {} },
    { {}, {}, {}, {}, {}, {}, {}, {} },
    { {}, {}, {}, {}, {}, {}, {}, {} },
    // Rank 2 (White pawns)
    { {PieceType::PAWN, PieceColor::WHITE}, {PieceType::PAWN, PieceColor::WHITE}, 
      {PieceType::PAWN, PieceColor::WHITE}, {PieceType::PAWN, PieceColor::WHITE}, 
      {PieceType::PAWN, PieceColor::WHITE}, {PieceType::PAWN, PieceColor::WHITE}, 
      {PieceType::PAWN, PieceColor::WHITE}, {PieceType::PAWN, PieceColor::WHITE} },
    // Rank 1 (White pieces)
    { {PieceType::ROOK, PieceColor::WHITE}, {PieceType::KNIGHT, PieceColor::WHITE}, 
      {PieceType::BISHOP, PieceColor::WHITE}, {PieceType::QUEEN, PieceColor::WHITE}, 
      {PieceType::KING, PieceColor::WHITE}, {PieceType::BISHOP, PieceColor::WHITE}, 
      {PieceType::KNIGHT, PieceColor::WHITE}, {PieceType::ROOK, PieceColor::WHITE} }
};

// Function to get piece symbol for ASCII representation
inline char getPieceSymbol(const ChessPiece& piece) {
    if (piece.isEmpty()) return ' ';
    
    switch (piece.type) {
        case PieceType::PAWN:   return (piece.color == PieceColor::WHITE) ? 'P' : 'p';
        case PieceType::ROOK:   return (piece.color == PieceColor::WHITE) ? 'R' : 'r';
        case PieceType::KNIGHT: return (piece.color == PieceColor::WHITE) ? 'N' : 'n';
        case PieceType::BISHOP: return (piece.color == PieceColor::WHITE) ? 'B' : 'b';
        case PieceType::QUEEN:  return (piece.color == PieceColor::WHITE) ? 'Q' : 'q';
        case PieceType::KING:   return (piece.color == PieceColor::WHITE) ? 'K' : 'k';
        default: return ' ';
    }
}

// Function to get piece name
inline std::string getPieceName(const ChessPiece& piece) {
    if (piece.isEmpty()) return "Empty";
    
    std::string color = (piece.color == PieceColor::WHITE) ? "White " : "Black ";
    
    switch (piece.type) {
        case PieceType::PAWN:   return color + "Pawn";
        case PieceType::ROOK:   return color + "Rook";
        case PieceType::KNIGHT: return color + "Knight";
        case PieceType::BISHOP: return color + "Bishop";
        case PieceType::QUEEN:  return color + "Queen";
        case PieceType::KING:   return color + "King";
        default: return "Unknown";
    }
}

#endif // CHESS_PIECES_H
