#include "modal_info.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <algorithm>
#include "chess_pieces_sdl.h"

GameInfoModal::GameInfoModal(SDL_Renderer* renderer, int screenWidth, int screenHeight)
    : ModalBase(renderer, screenWidth, screenHeight, 300, 300){

    // Piece display settings
    pieceSize = SQUARE_SIZE / PIECE_SCALED;
    pieceSpacing = 0;
    sectionPadding = 5;
}

bool GameInfoModal::handleEvent(const SDL_Event& e) {
    // First let base class handle ESC key
    if (ModalBase::handleEvent(e)) {
        return true;
    }

    if (!isVisible()) return false;

    // Handle 'i' key to close modal
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        if (e.key.keysym.sym == SDLK_i) {
            hide();
            return true;  // Modal consumed the 'i' event
        }
    }
    // Handle mouse click
    else if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Check if click is within modal
        if (mouseX >= modalX && mouseX <= modalX + modalWidth && mouseY >= modalY &&
            mouseY <= modalY + modalHeight) {
            return true;  // Modal consumed the event
        }
    }
    return false;
}

void GameInfoModal::render() {
    if (!isVisible()) return;
    
    // Render modal background and border
    renderModalBackground();
    renderModalBorder();
    
    // Draw captured pieces sections
    int currentY = modalY + 10;
    
    // White captured pieces
    renderCapturedPiecesSection(currentY, "Captured Pieces:", whiteCapturedPieces, blackCapturedPieces);
    
    // Render points and timers in horizontal alignment
    renderPointsAndTimersSection(currentY + 85);
    
    // Render close instruction
    renderBottomLine("Press I to toggle");
}

void GameInfoModal::updateCapturedPieces(const std::vector<ChessPiece>& whiteCaptured, 
                                        const std::vector<ChessPiece>& blackCaptured) {
    whiteCapturedPieces = whiteCaptured;
    blackCapturedPieces = blackCaptured;
}

void GameInfoModal::setPoints(const std::string& points, bool isNegative) {
    currentPoints = points;
    isNegativePoints = isNegative;
}


void GameInfoModal::setWhiteTimer(const std::string& time) { whiteTimer = time; }

void GameInfoModal::setBlackTimer(const std::string& time) { blackTimer = time; }

void GameInfoModal::renderCapturedPiecesSection(int startY, const std::string& title, 
                                               const std::vector<ChessPiece>& wpieces, const std::vector<ChessPiece>& bpieces) {
    // Draw section title
    drawText(title, modalX + sectionPadding, startY, {255, 255, 255, 255});

    int box_height = 70;
    if(wpieces.size() > 13 && bpieces.size() > 13) box_height = 130;
    else if(wpieces.size() > 13 || bpieces.size() > 13) box_height = 100;
    
    // Draw section background
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_Rect sectionRect = {modalX + sectionPadding, startY + 20, modalWidth - 2 * sectionPadding, box_height - 10};
    SDL_RenderFillRect(renderer, &sectionRect);
    
    // Render captured pieces
    int currentX = modalX + sectionPadding;
    int pieceY = startY + 20;
   

    if (wpieces.empty() && bpieces.empty()) {
        drawText("None", currentX, pieceY, {150, 150, 150, 255});
    } else {
        // White captured pieces (top row)
        for (const auto& piece : wpieces) {
            renderChessPiece(renderer, currentX, pieceY, piece, PIECE_SCALED);
            currentX += pieceSize + pieceSpacing;
            
            // Wrap to next line if needed
            if (currentX + pieceSize > modalX + modalWidth - sectionPadding) {
                currentX = modalX + sectionPadding;
                pieceY += pieceSize + pieceSpacing;
            }
        }

        currentX = modalX + sectionPadding;
        pieceY = startY + box_height / 2 + 5;

        for (const auto& piece : bpieces) {
            renderChessPiece(renderer, currentX, pieceY, piece, PIECE_SCALED);
            currentX += pieceSize + pieceSpacing;
            
            // Wrap to next line if needed
            if (currentX + pieceSize > modalX + modalWidth - sectionPadding) {
                currentX = modalX + sectionPadding;
                pieceY += pieceSize + pieceSpacing;
            }
        }
    }
}

void GameInfoModal::renderPointsAndTimersSection(int startY) {
    int sectionHeight = 30;
    int sectionWidth = (modalWidth - 4 * sectionPadding) / 3;
    
    // Calculate positions for horizontal alignment
    int whiteTimerX = modalX + sectionPadding;
    int pointsX = modalX + sectionPadding + sectionWidth + sectionPadding;
    int blackTimerX = modalX + sectionPadding + 2 * sectionWidth + 2 * sectionPadding;
    
    // Render white timer (left)
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_Rect whiteRect = {whiteTimerX, startY, sectionWidth, sectionHeight};
    SDL_RenderFillRect(renderer, &whiteRect);
    
    // Render points (center)
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_Rect pointsRect = {pointsX, startY, sectionWidth, sectionHeight};
    SDL_RenderFillRect(renderer, &pointsRect);
    
    // Render black timer (right)
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_Rect blackRect = {blackTimerX, startY, sectionWidth, sectionHeight};
    SDL_RenderFillRect(renderer, &blackRect);
    
    // Render white timer value (centered in its section)
    int whiteTextX = whiteTimerX + sectionWidth / 2 - (whiteTimer.length() * 14) / 2;
    drawText(whiteTimer, whiteTextX, startY + sectionHeight / 2 - 15, {255, 255, 255, 255}, 24);

    // Render current points value (centered in its section)
    SDL_Color color = isNegativePoints ? SDL_Color{255, 100, 100, 255} : SDL_Color{100, 255, 100, 255};
    drawText(currentPoints, pointsX + sectionWidth / 2 - (currentPoints.length() * 14) / 2, 
             startY + sectionHeight / 2 - 15, color, 24);
    
    // Render black timer value (centered in its section)
    int blackTextX = blackTimerX + sectionWidth / 2 - (blackTimer.length() * 14) / 2;
    drawText(blackTimer, blackTextX, startY + sectionHeight / 2 - 15, {255, 255, 255, 255}, 24);
}
