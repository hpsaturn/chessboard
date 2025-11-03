// Game Info Modal Window for Chess Game
#ifndef GAMEINFO_MODAL_H
#define GAMEINFO_MODAL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <functional>
#include "chess_pieces.h"
#include "definitions.h"

#define PIECE_SCALED 2

class GameInfoModal {
public:
    GameInfoModal(SDL_Renderer* renderer, int screenWidth, int screenHeight);
    ~GameInfoModal();
    
    // Show the modal window
    void show();
    
    // Hide the modal window
    void hide();
    
    // Handle events for the modal
    bool handleEvent(const SDL_Event& e);
    
    // Render the modal
    void render();
    
    // Check if modal is visible
    bool isVisible() const { return visible; }
    
    // Update captured pieces
    void updateCapturedPieces(const std::vector<ChessPiece>& whiteCaptured, 
                             const std::vector<ChessPiece>& blackCaptured);

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    int screenWidth;
    int screenHeight;
    bool visible;
    
    // Modal dimensions
    int modalWidth;
    int modalHeight;
    int modalX;
    int modalY;
    
    // Captured pieces
    std::vector<ChessPiece> whiteCapturedPieces;
    std::vector<ChessPiece> blackCapturedPieces;
    
    // Piece display settings
    int pieceSize;
    int pieceSpacing;
    int sectionPadding;
    
    // Helper functions
    void drawText(const std::string& text, int x, int y, SDL_Color color);
    SDL_Texture* createTextTexture(const std::string& text, SDL_Color color);
    void renderCapturedPiecesSection(int startY, const std::string& title, 
                                    const std::vector<ChessPiece>& wpieces,
                                    const std::vector<ChessPiece>& bpieces);

};

#endif // GAMEINFO_MODAL_H
