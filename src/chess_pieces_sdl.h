// Chess Pieces SDL Header
#ifndef CHESS_PIECES_SDL_H
#define CHESS_PIECES_SDL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <map>
#include "chess_pieces.h"

// Structure to hold loaded textures
struct PieceTextures {
    SDL_Texture* white;
    SDL_Texture* black;
};

// Function to initialize and load all chess piece textures
bool initChessPieceTextures(SDL_Renderer* renderer);

// Function to cleanup loaded textures
void cleanupChessPieceTextures();

// Function to render a chess piece using loaded textures
void renderChessPiece(SDL_Renderer* renderer, int x, int y, const ChessPiece& piece);

// Function to render text using SDL_ttf
void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y, int size = 1);

#endif // CHESS_PIECES_SDL_H
