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
#include "modal_base.h"

#define PIECE_SCALED 2

class GameInfoModal : public ModalBase {
public:
    GameInfoModal(SDL_Renderer* renderer, int screenWidth, int screenHeight);
    
    // Handle events for the modal
    bool handleEvent(const SDL_Event& e) override;
    
    // Render the modal
    void render() override;
    
    // Update captured pieces
    void updateCapturedPieces(const std::vector<ChessPiece>& whiteCaptured, 
                             const std::vector<ChessPiece>& blackCaptured);

private:
    // Captured pieces
    std::vector<ChessPiece> whiteCapturedPieces;
    std::vector<ChessPiece> blackCapturedPieces;
    
    // Piece display settings
    int pieceSize;
    int pieceSpacing;
    int sectionPadding;
    
    // Helper functions
    void renderCapturedPiecesSection(int startY, const std::string& title, 
                                    const std::vector<ChessPiece>& wpieces,
                                    const std::vector<ChessPiece>& bpieces);

};

#endif // GAMEINFO_MODAL_H
