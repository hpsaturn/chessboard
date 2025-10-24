#include <ncurses.h>
#include "chess_pieces_ncurses.h"

void initNcursesColors() {
    if (has_colors()) {
        start_color();
        // Use transparent background (COLOR_BLACK) for all elements
        init_pair(WHITE_PIECE, COLOR_WHITE, COLOR_BLACK);
        init_pair(BLACK_PIECE, COLOR_CYAN, COLOR_BLACK);
        init_pair(HIGHLIGHT, COLOR_BLACK, COLOR_YELLOW);
        init_pair(BORDER_COLOR, COLOR_CYAN, COLOR_BLACK);
        init_pair(HISTORY_COLOR, COLOR_GREEN, COLOR_BLACK);
        init_pair(INPUT_COLOR, COLOR_YELLOW, COLOR_BLACK);
    }
}

// Use letter-based piece representation like character mode
char getNcursesPieceSymbol(const ChessPiece& piece) {
    if (piece.isEmpty()) return ' ';
    
    switch (piece.type) {
        case PieceType::PAWN:   return (piece.color == PieceColor::WHITE) ? 'P' : 'p';
        case PieceType::ROOK:   return (piece.color == PieceColor::WHITE) ? 'R' : 'r';
        case PieceType::KNIGHT: return (piece.color == PieceColor::WHITE) ? 'N' : 'n';
        case PieceType::BISHOP: return (piece.color == PieceColor::WHITE) ? 'B' : 'b';
        case PieceType::QUEEN:  return (piece.color == PieceColor::WHITE) ? 'Q' : 'q';
        case PieceType::KING:   return (piece.color == PieceColor::WHITE) ? 'K' : 'k';
        default: return '?';
    }
}

void renderNcursesPiece(int y, int x, const ChessPiece& piece, bool is_selected) {
    if (is_selected) {
        attron(COLOR_PAIR(HIGHLIGHT));
    } else if (piece.color == PieceColor::WHITE) {
        attron(COLOR_PAIR(WHITE_PIECE));
    } else {
        attron(COLOR_PAIR(BLACK_PIECE));
    }
    
    mvaddch(y, x, getNcursesPieceSymbol(piece));
    
    if (is_selected) {
        attroff(COLOR_PAIR(HIGHLIGHT));
    } else if (piece.color == PieceColor::WHITE) {
        attroff(COLOR_PAIR(WHITE_PIECE));
    } else {
        attroff(COLOR_PAIR(BLACK_PIECE));
    }
}
