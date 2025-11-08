#include "help_modal.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

HelpModal::HelpModal(SDL_Renderer* renderer, int screenWidth, int screenHeight)
    : renderer(renderer), screenWidth(screenWidth), screenHeight(screenHeight), 
      visible(false), font(nullptr), scrollOffset(0) {
    
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
    
    // Initialize help content
    initializeHelpContent();
}

HelpModal::~HelpModal() {
    if (font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
}

void HelpModal::show() {
    visible = true;
    scrollOffset = 0; // Reset scroll position when showing
}

void HelpModal::hide() {
    visible = false;
}

bool HelpModal::handleEvent(const SDL_Event& e) {
    if (!visible) return false;
    
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
                hide();
                return true;
            case SDLK_UP:
                if (scrollOffset > 0) {
                    scrollOffset--;
                }
                return true;
            case SDLK_DOWN:
                if (scrollOffset < (int)helpLines.size() - 15) {
                    scrollOffset++;
                }
                return true;
            case SDLK_PAGEUP:
                scrollOffset = std::max(0, scrollOffset - 10);
                return true;
            case SDLK_PAGEDOWN:
                scrollOffset = std::min((int)helpLines.size() - 15, scrollOffset + 10);
                return true;
            case SDLK_HOME:
                scrollOffset = 0;
                return true;
            case SDLK_END:
                scrollOffset = std::max(0, (int)helpLines.size() - 15);
                return true;
        }
    }
    
    return false;
}

void HelpModal::render() {
    if (!visible) return;
    
    // Draw semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128); // Semi-transparent black
    SDL_Rect overlay = {0, 0, screenWidth, screenHeight};
    SDL_RenderFillRect(renderer, &overlay);
    
    // Draw modal background
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255); // Dark gray
    SDL_Rect modalRect = {modalX, modalY, modalWidth, modalHeight};
    SDL_RenderFillRect(renderer, &modalRect);
    
    // Draw modal border
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255); // Light gray border
    SDL_Rect modalBorder = {modalX - 1, modalY - 1, modalWidth + 2, modalHeight + 2};
    SDL_RenderDrawRect(renderer, &modalBorder);
    
    // Draw title
    drawText("Key Bindings", modalX + 10, modalY + 10, {255, 255, 255, 255});
    
    // Draw help content with scroll
    int lineHeight = 15;
    int startY = modalY + 35;
    int maxVisibleLines = (modalHeight - 50) / lineHeight;
    
    for (int i = 0; i < maxVisibleLines; i++) {
        int lineIndex = scrollOffset + i;
        if (lineIndex < (int)helpLines.size()) {
            SDL_Color color = {200, 200, 200, 255}; // Light gray for regular text
            drawText(helpLines[lineIndex], modalX + 10, startY + (i * lineHeight), color);
        }
    }
    
    // Draw scroll indicator if needed
    if (helpLines.size() > maxVisibleLines) {
        int scrollBarHeight = (maxVisibleLines * modalHeight) / helpLines.size();
        int scrollBarY = modalY + 35 + (scrollOffset * (modalHeight - 50)) / helpLines.size();
        
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_Rect scrollBar = {modalX + modalWidth - 10, scrollBarY, 5, scrollBarHeight};
        SDL_RenderFillRect(renderer, &scrollBar);
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void HelpModal::drawText(const std::string& text, int x, int y, SDL_Color color) {
    SDL_Texture* texture = createTextTexture(text, color);
    if (texture) {
        int texW, texH;
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        SDL_Rect dstRect = {x, y, texW, texH};
        SDL_RenderCopy(renderer, texture, NULL, &dstRect);
        SDL_DestroyTexture(texture);
    }
}

SDL_Texture* HelpModal::createTextTexture(const std::string& text, SDL_Color color) {
    if (!font) return nullptr;
    
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

void HelpModal::initializeHelpContent() {
    helpLines.clear();
    
    // Add help content with proper tabular formatting
    helpLines.push_back("Arrows     Piece and setting selections");
    helpLines.push_back("SPACE      Select piece or setting");
    helpLines.push_back("ENTER      Move selected piece");
    helpLines.push_back("ESC        Deselect piece or window exit");
    helpLines.push_back("F2         Save game state");
    helpLines.push_back("F3         Load last game state saved");
    helpLines.push_back("F4         Enter to game states window");
    helpLines.push_back("BACKSPACE  Delete game state");
    helpLines.push_back("S          Enter to settings section");
    helpLines.push_back("I          Toggle to show game info");
    helpLines.push_back("Q          Exit the game");
    helpLines.push_back("R          Restart the game");
    helpLines.push_back("H / F1     Show this help window");
    helpLines.push_back("ESC        Close help window");
}
