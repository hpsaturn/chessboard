#include "about_modal.h"
#include "version.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

AboutModal::AboutModal(SDL_Renderer* renderer, int screenWidth, int screenHeight) 
    : renderer(renderer), screenWidth(screenWidth), screenHeight(screenHeight), visible(false) {
    // Modal dimensions (same as gameinfo_modal)
    modalWidth = 300;
    modalHeight = 300;
    modalX = (screenWidth - modalWidth) / 2;
    modalY = (screenHeight - modalHeight) / 2;
    
    // Initialize font (will be set during render)
    font = nullptr;
}

AboutModal::~AboutModal() {
    // Cleanup is handled by the main application
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
        switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
            case SDLK_RETURN:
            case SDLK_SPACE:
                hide();
                return true;
        }
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        
        // Close if clicked outside the modal
        if (mouseX < modalX || mouseX > modalX + modalWidth ||
            mouseY < modalY || mouseY > modalY + modalHeight) {
            hide();
            return true;
        }
    }
    
    return false;
}

void AboutModal::render() {
    if (!visible) return;

    // Save current render color
    SDL_Color oldColor;
    SDL_GetRenderDrawColor(renderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);

    // Create modal rectangle
    SDL_Rect modalRect = {modalX, modalY, modalWidth, modalHeight};

    // Draw modal background (dark gray)
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &modalRect);

    // Draw modal border (light gray)
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &modalRect);

    // Text color (white)
    SDL_Color textColor = {255, 255, 255, 255};

    // About content
    std::vector<std::string> lines = {
        "Chessboard",
        "Version: " + Version::getVersion(),
        "",
        "Credits:",
        "",
        "Developer:",
        "  - Cesar Hernandez (hpsaturn)",
        "",
        "Contributors:",
        "  - @lunokjod (AI assistant)",
        "",
        "Special Thanks:",
        "  - SDL2 Community",
        "  - Buildroot Project"
    };

    // Add Buildroot-specific info if detected
    if (Version::isBuildrootEnvironment()) {
        lines.insert(lines.begin() + 2, "Build: Buildroot");
        lines.insert(lines.begin() + 3, "Commit: " + Version::getGitVersion());
    }

    // Render each line
    int lineHeight = 20; // Default line height if font is not available
    int startY = modalY + 20;

    for (size_t i = 0; i < lines.size(); i++) {
        if (lines[i].empty()) {
            startY += lineHeight / 2;
            continue;
        }

        drawText(lines[i], modalX + (modalWidth) / 2, startY, textColor);
        startY += lineHeight;
    }

    // Restore original render color
    SDL_SetRenderDrawColor(renderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
}

void AboutModal::drawText(const std::string& text, int x, int y, SDL_Color color) {
    if (!font) return;
    
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_Rect textRect = {
                x - surface->w / 2,
                y,
                surface->w,
                surface->h
            };
            SDL_RenderCopy(renderer, texture, NULL, &textRect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

SDL_Texture* AboutModal::createTextTexture(const std::string& text, SDL_Color color) {
    if (!font) return nullptr;
    
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surface) return nullptr;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}
