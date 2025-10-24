// Chess Pieces SDL Implementation
#ifdef HAS_SDL2
#include <SDL2/SDL.h>
#include "chess_pieces.h"

#define SQUARE_SIZE 27.5 // BOARD_SIZE/8 = 220/8 = 27.5

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

// Function to render text using SDL primitives (simple ASCII art style)
void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y, int size) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White text
    
    // Simple character rendering using points
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        int charX = x + i * (size * 6); // Character width
        
        // Basic character rendering (very simplified)
        for (int py = 0; py < size * 8; py++) {
            for (int px = 0; px < size * 6; px++) {
                // Simple pattern for visibility
                if ((px + py) % 3 == 0) {
                    SDL_RenderDrawPoint(renderer, charX + px, y + py);
                }
            }
        }
    }
}

#else
// Stub implementations when SDL2 is not available
#include "chess_pieces.h"
#include <string>

// Forward declarations for SDL types when not available
typedef void* SDL_Renderer;

void renderChessPiece(SDL_Renderer* renderer, int x, int y, const ChessPiece& piece) {
    // No-op when SDL2 is not available
}

void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y, int size) {
    // No-op when SDL2 is not available
}
#endif
