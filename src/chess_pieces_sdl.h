// Chess Pieces SDL Header
#ifndef CHESS_PIECES_SDL_H
#define CHESS_PIECES_SDL_H

#include <SDL2/SDL.h>
#include <string>
#include "chess_pieces.h"

// Function to render a chess piece using SDL primitives
void renderChessPiece(SDL_Renderer* renderer, int x, int y, const ChessPiece& piece);

// Function to render text using SDL primitives (simple ASCII art style)
void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y, int size = 1);

#endif // CHESS_PIECES_SDL_H
