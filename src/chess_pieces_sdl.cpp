// Chess Pieces SDL Implementation
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <limits.h>
#include <vector>
#include "chess_pieces_sdl.h"
#include <map>
#include "chess_pieces.h"
#include "definitions.h"

// Global texture storage
std::map<PieceType, PieceTextures> pieceTextures;
TTF_Font* font = nullptr;

// Function to get the directory where the executable is located
std::string getExecutablePath() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        std::string path(result, count);
        return path.substr(0, path.find_last_of("/"));
    }
    return "";
}

// Function to get resource path with robust detection
std::string getResourcePath(const std::string& relativePath) {
    std::vector<std::string> searchPaths;
    
    // 1. Try relative to executable (development build)
    std::string exeDir = getExecutablePath();
    if (!exeDir.empty()) {
        searchPaths.push_back(exeDir + "/res/" + relativePath);
        searchPaths.push_back(exeDir + "/../res/" + relativePath);
    }
    
    // 2. Try current directory (development)
    searchPaths.push_back("res/" + relativePath);
    searchPaths.push_back("../res/" + relativePath);
    
    // 3. Try installed paths
    searchPaths.push_back(RESOURCE_PATH_INSTALLED "/" + relativePath);
    searchPaths.push_back("/usr/share/chess/" + relativePath);
    searchPaths.push_back("/usr/local/share/chess/" + relativePath);
    
    // 4. Try absolute paths from CMake definitions
    searchPaths.push_back(RESOURCE_PATH_DEVELOPMENT "/" + relativePath);
    
    // Debug: print search paths if needed
    #ifdef DEBUG_RESOURCE_PATHS
    std::cout << "[RESOURCE] Searching for: " << relativePath << std::endl;
    for (const auto& path : searchPaths) {
        std::cout << "[RESOURCE] Checking: " << path << std::endl;
    }
    #endif
    
    // Try each path in order
    for (const auto& path : searchPaths) {
        if (std::filesystem::exists(path)) {
            #ifdef DEBUG_RESOURCE_PATHS
            std::cout << "[RESOURCE] Found: " << path << std::endl;
            #endif
            return path;
        }
    }
    
    // If nothing found, return the most likely development path for error reporting
    std::cerr << "[RESOURCE] ERROR: Could not find resource: " << relativePath << std::endl;
    std::cerr << "[RESOURCE] Searched in:" << std::endl;
    for (const auto& path : searchPaths) {
        std::cerr << "  - " << path << std::endl;
    }
    
    return searchPaths[0]; // Return first path for error context
}

// Function to load a texture from file
SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path) {
    SDL_Texture* texture = nullptr;
    
    // Get the actual resource path
    std::string actualPath = getResourcePath(path);
    SDL_Surface* surface = IMG_Load(actualPath.c_str());
    
    if (surface == nullptr) {
        std::cerr << "[SDLG] Unable to load image " << actualPath << ": " << IMG_GetError() << std::endl;
        return nullptr;
    }
    
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (texture == nullptr) {
        std::cerr << "[SDLG] Unable to create texture from " << actualPath << ": " << SDL_GetError() << std::endl;
    }
    
    return texture;
}

// Function to initialize and load all chess piece textures
bool initChessPieceTextures(SDL_Renderer* renderer) {
    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "[SDLG] SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }
    
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "[SDLG] SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }
    
    // Load font - try multiple locations
    std::vector<std::string> fontPaths = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf"
    };
    
    for (const auto& fontPath : fontPaths) {
        font = TTF_OpenFont(fontPath.c_str(), 10);
        if (font != nullptr) {
            std::cout << "[SDLG] Loaded font from: " << fontPath << std::endl;
            break;
        }
    }
    
    if (font == nullptr) {
        std::cerr << "[SDLG] Failed to load font from any location! SDL_ttf Error: " << TTF_GetError() << std::endl;
        // Continue without font - text rendering will fall back to primitive method
    }
    
    // Load all chess piece textures
    pieceTextures[PieceType::PAWN] = {
        loadTexture(renderer, "default/pawn_white.png"),
        loadTexture(renderer, "default/pawn_black.png")
    };
    
    pieceTextures[PieceType::ROOK] = {
        loadTexture(renderer, "default/rook_white.png"),
        loadTexture(renderer, "default/rook_black.png")
    };
    
    pieceTextures[PieceType::KNIGHT] = {
        loadTexture(renderer, "default/knight_white.png"),
        loadTexture(renderer, "default/knight_black.png")
    };
    
    pieceTextures[PieceType::BISHOP] = {
        loadTexture(renderer, "default/bishop_white.png"),
        loadTexture(renderer, "default/bishop_black.png")
    };
    
    pieceTextures[PieceType::QUEEN] = {
        loadTexture(renderer, "default/queen_white.png"),
        loadTexture(renderer, "default/queen_black.png")
    };
    
    pieceTextures[PieceType::KING] = {
        loadTexture(renderer, "default/king_white.png"),
        loadTexture(renderer, "default/king_black.png")
    };
    
    // Check if all textures loaded successfully
    bool allLoaded = true;
    for (const auto& pair : pieceTextures) {
        if (pair.second.white == nullptr || pair.second.black == nullptr) {
            std::cerr << "[SDLG] Failed to load textures for piece type: " << static_cast<int>(pair.first) << std::endl;
            allLoaded = false;
        }
    }
    
    if (allLoaded) {
        std::cout << "[SDLG] Successfully loaded all chess piece textures" << std::endl;
    } else {
        std::cerr << "[SDLG] Some textures failed to load" << std::endl;
    }
    
    return allLoaded;
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
void renderChessPiece(SDL_Renderer* renderer, int x, int y, const ChessPiece& piece, int scale) {
    if (piece.isEmpty()) return;
    
    auto it = pieceTextures.find(piece.type);
    if (it == pieceTextures.end()) {
        std::cerr << "[SDLG] Texture not found for piece type: " << static_cast<int>(piece.type) << std::endl;
        return;
    }
    
    SDL_Texture* texture = (piece.color == PieceColor::WHITE) ? it->second.white : it->second.black;
    if (texture == nullptr) {
        std::cerr << "[SDLG] Texture is null for piece type: " << static_cast<int>(piece.type) << std::endl;
        return;
    }
    
    // Get texture dimensions
    int textureWidth, textureHeight;
    SDL_QueryTexture(texture, nullptr, nullptr, &textureWidth, &textureHeight);

    textureWidth = textureWidth / scale;
    textureHeight = textureHeight / scale;
    
    // Calculate destination rectangle
    SDL_Rect destRect = {
        x + (SQUARE_SIZE - textureWidth) / 2,
        y + (SQUARE_SIZE - textureHeight) / 2,
        textureWidth - (abs(textureWidth - SQUARE_SIZE)/3),
        textureHeight - (abs(textureHeight - SQUARE_SIZE)/3)
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
