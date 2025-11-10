#include "about_modal.h"
#include "version.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <algorithm>

AboutModal::AboutModal(SDL_Renderer* renderer, int screenWidth, int screenHeight)
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
    
    // Calculate modal position and size (same as gameinfo_modal)
    modalWidth = 300;
    modalHeight = 300;
    modalX = (screenWidth - modalWidth) / 2;
    modalY = (screenHeight - modalHeight) / 2;
}

AboutModal::~AboutModal() {
    if (font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
}

void AboutModal::show() {
    visible = true;
}

void AboutModal::hide() {
    visible = false;
}

bool AboutModal::handleEvent(const SDL_Event& e) {
    if (!visible) return false;
    
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE) {
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

void AboutModal::render() {
    if (!visible || !font) return;
    
    // Draw modal background (same colors as gameinfo_modal)
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 220);  // Dark gray background
    SDL_Rect overlay = {modalX, modalY, modalWidth, modalHeight};
    SDL_RenderFillRect(renderer, &overlay);
    
    // Draw border (same colors as gameinfo_modal)
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 220);  // Light gray border
    SDL_Rect modalRect = {modalX, modalY, modalWidth, modalHeight};
    SDL_RenderDrawRect(renderer, &modalRect);
    
    // Get dynamic version
    std::string versionInfo = Version::getBuildInfo();
    
    // Draw title
    SDL_Color titleColor = {255, 255, 255, 255};  // White
    drawText("About Chess Game", modalX + 10, modalY + 10, titleColor);
    drawText("(Hpsaturn 2025 " + versionInfo + ")", modalX + 10, modalY + 25, titleColor);
    
    // Draw separator line
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);  // Medium gray
    SDL_RenderDrawLine(renderer, modalX + 10, modalY + 45, modalX + modalWidth - 10, modalY + 45);
    
    // Draw credits content
    SDL_Color textColor = {220, 220, 220, 255};  // Light gray
    int currentY = modalY + 55;
    
    drawText("Credits:", modalX + 15, currentY, textColor);
    currentY += 25;
    
    drawText("- Special thanks to my son, M.V", modalX + 20, currentY, textColor);
    currentY += 15;
    drawText("  for continuous critics and beta tests", modalX + 20, currentY, textColor);
    currentY += 25;
    
    drawText("- Thanks to @lunokjod", modalX + 20, currentY, textColor);
    currentY += 15;
    drawText("  for his MongoIA assistant", modalX + 20, currentY, textColor);
    currentY += 25;
    
    drawText("- Thanks to Chess SDL project", modalX + 20, currentY, textColor);
    currentY += 15;
    drawText("  for CC BY-SA image assets", modalX + 20, currentY, textColor);
    currentY += 25;
    
    // Draw license info
    SDL_Color licenseColor = {180, 180, 255, 255};  // Light blue
    drawText("  License: GPLv3", modalX + 15, currentY, licenseColor);
    currentY += 20;
    drawText("  Assets: CC BY-SA 3.0", modalX + 15, currentY, licenseColor);
    
    // Draw close instruction
    SDL_Color instructionColor = {150, 255, 150, 255};  // Light green
    drawText("Press ESC to close", modalX + 15, modalY + modalHeight - 25, instructionColor);
}

void AboutModal::drawText(const std::string& text, int x, int y, SDL_Color color) {
    SDL_Texture* textTexture = createTextTexture(text, color);
    if (!textTexture) return;
    
    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
    
    SDL_Rect renderQuad = {x, y, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
    SDL_DestroyTexture(textTexture);
}

SDL_Texture* AboutModal::createTextTexture(const std::string& text, SDL_Color color) {
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
