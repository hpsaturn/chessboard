// Chess Pieces SDL Implementation
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include "chess_pieces_sdl.h"
#include <map>
#include "chess_pieces.h"

#define SQUARE_SIZE 32 // BOARD_SIZE/8 = 260/8 = 32.5

// Global texture storage
std::map<PieceType, PieceTextures> pieceTextures;
TTF_Font* font = nullptr;

// Function to load a texture from file
SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path) {
    SDL_Texture* texture = nullptr;
    SDL_Surface* surface = IMG_Load(path.c_str());
    
    if (surface == nullptr) {
        std::cerr << "Unable to load image " << path << ": " << IMG_GetError() << std::endl;
        return nullptr;
    }
    
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (texture == nullptr) {
        std::cerr << "Unable to create texture from " << path << ": " << SDL_GetError() << std::endl;
    }
    
    return texture;
}

// Function to initialize and load all chess piece textures
bool initChessPieceTextures(SDL_Renderer* renderer) {
    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }
    
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }
    
    // Load font
    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 10);
    if (font == nullptr) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        // Continue without font - text rendering will fall back to primitive method
    }
    
    // Load all chess piece textures
    pieceTextures[PieceType::PAWN] = {
        loadTexture(renderer, "res/default/pawn_white.png"),
        loadTexture(renderer, "res/default/pawn_black.png")
    };
    
    pieceTextures[PieceType::ROOK] = {
        loadTexture(renderer, "res/default/rook_white.png"),
        loadTexture(renderer, "res/default/rook_black.png")
    };
    
    pieceTextures[PieceType::KNIGHT] = {
        loadTexture(renderer, "res/default/knight_white.png"),
        loadTexture(renderer, "res/default/knight_black.png")
    };
    
    pieceTextures[PieceType::BISHOP] = {
        loadTexture(renderer, "res/default/bishop_white.png"),
        loadTexture(renderer, "res/default/bishop_black.png")
    };
    
    pieceTextures[PieceType::QUEEN] = {
        loadTexture(renderer, "res/default/queen_white.png"),
        loadTexture(renderer, "res/default/queen_black.png")
    };
    
    pieceTextures[PieceType::KING] = {
        loadTexture(renderer, "res/default/king_white.png"),
        loadTexture(renderer, "res/default/king_black.png")
    };
    
    // Check if all textures loaded successfully
    for (const auto& pair : pieceTextures) {
        if (pair.second.white == nullptr || pair.second.black == nullptr) {
            std::cerr << "Failed to load textures for piece type: " << static_cast<int>(pair.first) << std::endl;
            return false;
        }
    }
    
    std::cout << "Successfully loaded all chess piece textures" << std::endl;
    return true;
}

// Function to cleanup loaded textures
void cleanupChessPieceTextures() {
    for (auto& pair : pieceTextures) {
        if (pair.second.white) SDL_DestroyTexture(pair.second.white);
        if (pair.second.black) SDL_DestroyTexture(pair.second.black);
    }
    pieceTextures.clear();
    
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    
    TTF_Quit();
    IMG_Quit();
}

// Function to render a chess piece using loaded textures
void renderChessPiece(SDL_Renderer* renderer, int x, int y, const ChessPiece& piece) {
    if (piece.isEmpty()) return;
    
    auto it = pieceTextures.find(piece.type);
    if (it == pieceTextures.end()) {
        std::cerr << "Texture not found for piece type: " << static_cast<int>(piece.type) << std::endl;
        return;
    }
    
    SDL_Texture* texture = (piece.color == PieceColor::WHITE) ? it->second.white : it->second.black;
    if (texture == nullptr) {
        std::cerr << "Texture is null for piece type: " << static_cast<int>(piece.type) << std::endl;
        return;
    }
    
    // Get texture dimensions
    int textureWidth, textureHeight;
    SDL_QueryTexture(texture, nullptr, nullptr, &textureWidth, &textureHeight);
    
    // Calculate destination rectangle
    SDL_Rect destRect = {
        x + (SQUARE_SIZE - textureWidth) / 2 + 4,
        y + (SQUARE_SIZE - textureHeight) / 2 + 4,
        textureWidth-14,
        textureHeight-14
    };
    
    // Render the texture
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
}

// Function to render text using SDL_ttf
void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y, int size) {
    if (font) {
        // Use SDL_ttf for text rendering
        SDL_Color color = {255, 255, 255, 255}; // White text
        SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
        
        if (surface != nullptr) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture != nullptr) {
                SDL_Rect destRect = {x, y, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, nullptr, &destRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    } else {
        // Fallback to primitive text rendering
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
}
