#include "gameinfo_modal.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <algorithm>
#include "chess_pieces_sdl.h"

GameInfoModal::GameInfoModal(SDL_Renderer* renderer, int screenWidth, int screenHeight)
    : renderer(renderer), screenWidth(screenWidth), screenHeight(screenHeight), visible(false), font(nullptr) {
    
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
    } else {
        // Load DejaVu Sans font (commonly available on Linux)
        font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 11);
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        }
    }
    
    // Calculate modal position and size
    modalWidth = 300;
    modalHeight = 300;
    modalX = (screenWidth - modalWidth) / 2;
    modalY = (screenHeight - modalHeight) / 2;
    
    // Piece display settings
    pieceSize = SQUARE_SIZE / PIECE_SCALED;
    pieceSpacing = 1;
    sectionPadding = 5;
}

GameInfoModal::~GameInfoModal() {
    if (font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
}

void GameInfoModal::show() {
    visible = true;
}

void GameInfoModal::hide() {
    visible = false;
}

bool GameInfoModal::handleEvent(const SDL_Event& e) {
  if (!visible) return false;

  if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
    if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_i) {
      hide();
      return true;  // Modal consumed the ESC event
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
    if (!visible) return;
    
    // Draw semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128); // Semi-transparent black
    SDL_Rect overlay = {0, 0, screenWidth, screenHeight};
    SDL_RenderFillRect(renderer, &overlay);
    
    // Draw modal background
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 200); // Dark gray
    SDL_Rect modalRect = {modalX, modalY, modalWidth, modalHeight};
    SDL_RenderFillRect(renderer, &modalRect);
    
    // Draw modal border
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255); // Light gray border
    SDL_Rect modalBorder = {modalX - 1, modalY - 1, modalWidth + 2, modalHeight + 2};
    SDL_RenderDrawRect(renderer, &modalBorder);
    
    // Draw captured pieces sections
    int currentY = modalY + 10;
    
    // White captured pieces
    renderCapturedPiecesSection(currentY, "Captured Pieces:", whiteCapturedPieces, blackCapturedPieces);
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    
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

void GameInfoModal::drawText(const std::string& text, int x, int y, SDL_Color color) {
    if (!font) {
        // Fallback: draw placeholder rectangle
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_Rect textRect = {x, y, static_cast<int>(text.length() * 6), 14};
        SDL_RenderFillRect(renderer, &textRect);
        return;
    }
    
    SDL_Texture* texture = createTextTexture(text, color);
    if (texture) {
        int texW, texH;
        SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
        SDL_Rect dstRect = {x, y, texW, texH};
        SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
        SDL_DestroyTexture(texture);
    }
}

SDL_Texture* GameInfoModal::createTextTexture(const std::string& text, SDL_Color color) {
    if (!font) return nullptr;
    
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        return nullptr;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}
