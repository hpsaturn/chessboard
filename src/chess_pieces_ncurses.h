#ifndef CHESS_PIECES_NCURSES_H
#define CHESS_PIECES_NCURSES_H

#include <ncurses.h>
#include "chess_pieces.h"

// Color pairs for ncurses
#define WHITE_PIECE 1
#define BLACK_PIECE 2
#define HIGHLIGHT 3
#define BORDER_COLOR 4
#define HISTORY_COLOR 5
#define INPUT_COLOR 6

// Initialize ncurses colors
void initNcursesColors();

// Get piece symbol for ncurses display
char getNcursesPieceSymbol(const ChessPiece& piece);

// Render a chess piece in ncurses
void renderNcursesPiece(int y, int x, const ChessPiece& piece, bool is_selected);

#endif
