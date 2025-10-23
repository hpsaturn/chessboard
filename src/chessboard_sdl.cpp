#ifdef HAS_SDL2
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include "chess_pieces.h"

#define BOARD_SIZE 640
#define SQUARE_SIZE (BOARD_SIZE/8)

// Function to render a chess piece using SDL primitives
void renderChessPiece(SDL_Renderer* renderer, int x, int y, const ChessPiece& piece) {
    if (piece.isEmpty()) return;
    
    int centerX = x + SQUARE_SIZE/2;
    int centerY = y + SQUARE_SIZE/2;
    int pieceSize = SQUARE_SIZE * 0.7;
    int radius = pieceSize / 2;
    
    // Set piece color
    if (piece.color == PieceColor::WHITE) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
    }
    
    // Draw base circle
    for (int w = 0; w < pieceSize; w++) {
        for (int h = 0; h < pieceSize; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centerX - radius + w, centerY - radius + h);
            }
        }
    }
    
    // Draw piece-specific details
    SDL_SetRenderDrawColor(renderer, 
        (piece.color == PieceColor::WHITE) ? 200 : 255, 
        (piece.color == PieceColor::WHITE) ? 200 : 255, 
        (piece.color == PieceColor::WHITE) ? 200 : 255, 
        255);
    
    switch (piece.type) {
        case PieceType::PAWN: {
            // Simple pawn shape - small circle on top
            for (int w = 0; w < radius; w++) {
                for (int h = 0; h < radius; h++) {
                    int dx = radius/2 - w;
                    int dy = radius/2 - h;
                    if ((dx*dx + dy*dy) <= (radius/2 * radius/2)) {
                        SDL_RenderDrawPoint(renderer, centerX - radius/2 + w, centerY - radius + h);
                    }
                }
            }
            break;
        }
            
        case PieceType::ROOK: {
            // Rook shape - castle-like
            SDL_Rect top = {centerX - radius/2, centerY - radius, radius, radius/3};
            SDL_RenderFillRect(renderer, &top);
            SDL_Rect base = {centerX - radius/2, centerY - radius/3, radius, radius/2};
            SDL_RenderFillRect(renderer, &base);
            break;
        }
            
        case PieceType::KNIGHT: {
            // Knight shape - horse head approximation
            for (int w = 0; w < radius; w++) {
                for (int h = 0; h < radius; h++) {
                    if (w + h > radius/2 && w + h < radius*1.5) {
                        SDL_RenderDrawPoint(renderer, centerX - radius/2 + w, centerY - radius/2 + h);
                    }
                }
            }
            break;
        }
            
        case PieceType::BISHOP: {
            // Bishop shape - pointed top
            for (int i = 0; i < radius; i++) {
                SDL_RenderDrawLine(renderer, 
                    centerX - i/2, centerY - radius + i,
                    centerX + i/2, centerY - radius + i);
            }
            break;
        }
            
        case PieceType::QUEEN: {
            // Queen shape - crown
            for (int i = 0; i < 3; i++) {
                SDL_RenderDrawLine(renderer, 
                    centerX - radius/2 + i*radius/2, centerY - radius,
                    centerX - radius/2 + i*radius/2, centerY - radius/2);
            }
            SDL_Rect base = {centerX - radius/2, centerY - radius/2, radius, radius/2};
            SDL_RenderFillRect(renderer, &base);
            break;
        }
            
        case PieceType::KING: {
            // King shape - cross on top
            SDL_RenderDrawLine(renderer, centerX, centerY - radius, centerX, centerY - radius/2);
            SDL_RenderDrawLine(renderer, centerX - radius/3, centerY - radius*2/3, centerX + radius/3, centerY - radius*2/3);
            SDL_Rect base = {centerX - radius/2, centerY - radius/2, radius, radius/2};
            SDL_RenderFillRect(renderer, &base);
            break;
        }
            
        default:
            break;
    }
}

void renderChessboardSDL() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "SDL Chessboard with Pieces",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        BOARD_SIZE,
        BOARD_SIZE,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // Main loop flag
    bool quit = false;
    SDL_Event e;

    // Main loop
    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255); // Light square color
        SDL_RenderClear(renderer);

        // Draw chessboard
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                // Set square color
                if ((row + col) % 2 == 0) {
                    SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255); // Light squares
                } else {
                    SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255); // Dark squares
                }
                
                // Draw square
                SDL_Rect square = {col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
                SDL_RenderFillRect(renderer, &square);

                // Draw piece if present
                const ChessPiece& piece = STANDARD_BOARD[row][col];
                if (!piece.isEmpty()) {
                    renderChessPiece(renderer, col * SQUARE_SIZE, row * SQUARE_SIZE, piece);
                }
            }
        }

        // Update screen
        SDL_RenderPresent(renderer);

        // Small delay to prevent high CPU usage
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

#else
void renderChessboardSDL() {
    std::cout << "SDL2 support not available in this build.\n";
    std::cout << "Please install SDL2 development libraries and rebuild.\n";
}
#endif
