#include "gameinfo_modal.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <algorithm>
#include "chess_pieces_sdl.h"

GameInfoModal::GameInfoModal(SDL_Renderer* renderer, int screenWidth, int screenHeight)
    : ModalBase(renderer, screenWidth, screenHeight, 300, 300) {
    
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
    
    // Use base class to render modal background and border
    renderModalBackground();
    renderModalBorder();
    
    // Draw captured pieces sections
    int currentY = modalY + 10;
    
    // White captured pieces
    renderCapturedPiecesSection(currentY, "Captured Pieces:", whiteCapturedPieces, blackCapturedPieces);
    
    // Render close instruction
    renderBottomLine("Press I to toggle");
}

void GameInfoModal::updateCapturedPieces(const std::vector<ChessPiece>& whiteCaptured, 
                                        const std::vector<ChessPiece>& blackCaptured) {
    whiteCapturedPieces = whiteCaptured;
    blackCapturedPieces = blackCaptured;
}

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
