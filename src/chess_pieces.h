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
    bool hasMoved = false;
    int castelling = 0;  // 0 not catelling, 1 right , -1 left
    
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

inline ChessPiece charToPiece(char c) {
    PieceColor color = (std::isupper(c)) ? PieceColor::WHITE : PieceColor::BLACK;
    PieceType type;
    
    char lowerC = std::tolower(c);
    switch (lowerC) {
        case 'k': type = PieceType::KING; break;
        case 'q': type = PieceType::QUEEN; break;
        case 'r': type = PieceType::ROOK; break;
        case 'b': type = PieceType::BISHOP; break;
        case 'n': type = PieceType::KNIGHT; break;
        case 'p': type = PieceType::PAWN; break;
        default:  type = PieceType::NONE; break;
    }
    
    return ChessPiece{type, color};
}

inline char getPieceChar(const ChessPiece& piece) {
    if (piece.type == PieceType::NONE) return ' ';
    
    char baseChar;
    switch (piece.type) {
        case PieceType::KING:   baseChar = 'k'; break;
        case PieceType::QUEEN:  baseChar = 'q'; break;
        case PieceType::ROOK:   baseChar = 'r'; break;
        case PieceType::BISHOP: baseChar = 'b'; break;
        case PieceType::KNIGHT: baseChar = 'n'; break;
        case PieceType::PAWN:   baseChar = 'p'; break;
        default: return ' ';
    }
    
    // Uppercase for white pieces
    if (piece.color == PieceColor::WHITE) {
        baseChar = std::toupper(baseChar);
    }
    
    return baseChar;
}

#endif // CHESS_PIECES_H
