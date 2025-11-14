#include "modal_base.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <algorithm>

ModalBase::ModalBase(SDL_Renderer* renderer, int screenWidth, int screenHeight, 
                     int modalWidth, int modalHeight)
    : renderer(renderer), screenWidth(screenWidth), screenHeight(screenHeight), 
      visible(false), modalWidth(modalWidth), modalHeight(modalHeight), font(nullptr) {
    
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
    
    // Calculate modal position (centered)
    modalX = (screenWidth - modalWidth) / 2;
    modalY = (screenHeight - modalHeight) / 2;
}

ModalBase::~ModalBase() {
    if (font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
}

void ModalBase::show() {
    visible = true;
}

void ModalBase::hide() {
    visible = false;
}

bool ModalBase::handleEvent(const SDL_Event& e) {
    if (!visible) return false;
    
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_RETURN) {
            hide();
            return true;
        }
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        
        // Check if click is outside modal
        if (mouseX < modalX || mouseX > modalX + modalWidth || 
            mouseY < modalY || mouseY > modalY + modalHeight) {
            hide();
            return true;
        }
    }
    
    return false;
}

void ModalBase::renderModalBackground() {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 220);  // Dark gray background
    SDL_Rect overlay = {modalX, modalY, modalWidth, modalHeight};
    SDL_RenderFillRect(renderer, &overlay);
}

void ModalBase::renderModalBorder() {
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255); // Light gray border
    SDL_Rect modalBorder = {modalX - 1, modalY - 1, modalWidth + 2, modalHeight + 2};
    SDL_RenderDrawRect(renderer, &modalBorder);
}

void ModalBase::renderBottomLine(const std::string& instruction) {
    SDL_Color instructionColor = {150, 255, 150, 255};  // Light green
    drawText(instruction.c_str(), modalX + 15, modalY + modalHeight - 25, instructionColor);
}

void ModalBase::drawText(const std::string& text, int x, int y, SDL_Color color) {
    SDL_Texture* textTexture = createTextTexture(text, color);
    if (!textTexture) return;
    
    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
    
    SDL_Rect renderQuad = {x, y, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
    SDL_DestroyTexture(textTexture);
}

SDL_Texture* ModalBase::createTextTexture(const std::string& text, SDL_Color color) {
    if (!font) return nullptr;
    
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!textSurface) {
        std::cerr << "Unable to render text surface: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    
    return textTexture;
}
