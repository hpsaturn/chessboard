#include "modal_help.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <algorithm>

HelpModal::HelpModal(SDL_Renderer* renderer, int screenWidth, int screenHeight)
    : ModalBase(renderer, screenWidth, screenHeight, 300, 300), scrollOffset(0) {
    
    // Initialize help content
    initializeHelpContent();
}

HelpModal::~HelpModal() {
    // Font cleanup is handled by ModalBase destructor
}

bool HelpModal::handleEvent(const SDL_Event& e) {
    if (!visible) return false;
    
    // First check for base modal events (ESC to close)
    if (ModalBase::handleEvent(e)) {
        return true;
    }
    
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
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
    if (!visible || !font) return;
    
    // Use base class methods for common modal rendering
    renderModalBackground();
    renderModalBorder();
    
    // Draw title
    SDL_Color titleColor = {255, 255, 255, 255};  // White
    SDL_Color textColor = {220, 220, 220, 255};  // Light gray
    drawText("Key Bindings", modalX + 10, modalY + 10, titleColor);
    
    // Draw help content with scroll
    int lineHeight = 15;
    int startY = modalY + 35;
    int maxVisibleLines = (modalHeight - 50) / lineHeight;
    
    for (int i = 0; i < maxVisibleLines; i++) {
        int lineIndex = scrollOffset + i;
        if (lineIndex < (int)helpLines.size()) {
            drawText(helpLines[lineIndex], modalX + 10, startY + (i * lineHeight), textColor);
        }
    }
    
    // Draw scroll indicator if needed
    // if (helpLines.size() > maxVisibleLines) {
    //     int scrollBarHeight = (maxVisibleLines * modalHeight) / helpLines.size();
    //     int scrollBarY = modalY + 35 + (scrollOffset * (modalHeight - 50)) / helpLines.size();
        
    //     SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    //     SDL_Rect scrollBar = {modalX + modalWidth - 10, scrollBarY, 5, scrollBarHeight};
    //     SDL_RenderFillRect(renderer, &scrollBar);
    // }
    
    // Add close instruction
    renderBottomLine();
}

void HelpModal::initializeHelpContent() {
    helpLines.clear();
    
    // Add help content with proper tabular formatting
    helpLines.push_back("ARROWS      Piece and setting selections");
    helpLines.push_back("SPACE          Select piece or setting");
    helpLines.push_back("ENTER          Move selected piece");
    helpLines.push_back("ESC             Deselect piece or window exit");
    helpLines.push_back("F1               Show this help window");
    helpLines.push_back("F2               Save game state");
    helpLines.push_back("F3               Load last game state saved");
    helpLines.push_back("F4               Enter to game states window");
    helpLines.push_back("BACKSPACE  Delete game state");
    helpLines.push_back("S                 Enter to settings section");
    helpLines.push_back("I                  Toggle to show game info");
    helpLines.push_back("Q                 Exit the game");
    helpLines.push_back("R                 Restart the game");
    helpLines.push_back("H                 Show this help window");
    helpLines.push_back("ESC             Close current window");
}
