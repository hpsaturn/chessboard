#ifndef MODAL_INFO_H
#define MODAL_INFO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <functional>
#include "modal_base.h"
#include "chess_pieces.h"
#include "definitions.h"
#define PIECE_SCALED 2


class GameInfoModal : public ModalBase {
public:
    GameInfoModal(SDL_Renderer* renderer, int screenWidth, int screenHeight);
    
    bool handleEvent(const SDL_Event& e) override;
    void render() override;
    
    void updateCapturedPieces(const std::vector<ChessPiece>& whiteCaptured, 
                             const std::vector<ChessPiece>& blackCaptured);
    
    void setPoints(const std::string& points);
    void setWhiteTimer(const std::string& time);
    void setBlackTimer(const std::string& time);

private:
    std::vector<ChessPiece> whiteCapturedPieces;
    std::vector<ChessPiece> blackCapturedPieces;
    
    std::string currentPoints;
    std::string whiteTimer;
    std::string blackTimer;
    
    // Piece display settings
    int pieceSize;
    int pieceSpacing;
    int sectionPadding;
    
    // Helper functions
    void renderCapturedPiecesSection(int startY, const std::string& title, 
                                    const std::vector<ChessPiece>& wpieces,
                                    const std::vector<ChessPiece>& bpieces);
    void renderPointsAndTimersSection(int startY);
};

#endif
