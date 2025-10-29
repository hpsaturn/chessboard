// Settings Modal Window Implementation
#include "settings_modal.h"
#include <iostream>

SettingsModal::SettingsModal(SDL_Renderer* renderer, int screenWidth, int screenHeight)
    : renderer(renderer), screenWidth(screenWidth), screenHeight(screenHeight), visible(false) {
    
    // Calculate modal dimensions (80% of screen)
    modalWidth = screenWidth * 0.8;
    modalHeight = screenHeight * 0.8;
    modalX = (screenWidth - modalWidth) / 2;
    modalY = (screenHeight - modalHeight) / 2;
    
    // Initialize default settings
    currentSettings = {
        .soundEnabled = true,
        .showLegalMoves = true,
        .highlightLastMove = true,
        .difficultyLevel = 3,
        .boardTheme = "classic",
        .pieceSet = "classic"
    };
    
    initializeUI();
}

SettingsModal::~SettingsModal() {
    // Cleanup if needed
}

void SettingsModal::show() {
    visible = true;
}

void SettingsModal::hide() {
    visible = false;
}

void SettingsModal::initializeUI() {
    // Clear existing UI elements
    buttons.clear();
    checkboxes.clear();
    sliders.clear();
    
    int contentX = modalX + 20;
    int contentY = modalY + 20;
    int elementWidth = modalWidth - 40;
    int elementHeight = 30;
    int spacing = 10;
    
    // Title
    // (We'll draw this separately)
    
    // Checkboxes
    checkboxes.push_back({
        .rect = {contentX, contentY + 40, 20, 20},
        .label = "Enable Sound",
        .value = &currentSettings.soundEnabled,
        .hovered = false
    });
    
    checkboxes.push_back({
        .rect = {contentX, contentY + 70, 20, 20},
        .label = "Show Legal Moves",
        .value = &currentSettings.showLegalMoves,
        .hovered = false
    });
    
    checkboxes.push_back({
        .rect = {contentX, contentY + 100, 20, 20},
        .label = "Highlight Last Move",
        .value = &currentSettings.highlightLastMove,
        .hovered = false
    });
    
    // Difficulty Slider
    int sliderY = contentY + 140;
    sliders.push_back({
        .trackRect = {contentX, sliderY, elementWidth - 100, 20},
        .thumbRect = {contentX, sliderY, 20, 20},
        .label = "Difficulty Level",
        .value = &currentSettings.difficultyLevel,
        .minValue = 1,
        .maxValue = 5,
        .dragging = false
    });
    
    // Update initial slider position
    updateSliderValue(sliders[0], contentX + 
        ((currentSettings.difficultyLevel - 1) * (elementWidth - 100 - 20) / 4));
    
    // Close button
    buttons.push_back({
        .rect = {modalX + modalWidth - 100, modalY + modalHeight - 40, 80, 30},
        .text = "Close",
        .hovered = false,
        .onClick = [this]() { this->hide(); }
    });
    
    // Apply button
    buttons.push_back({
        .rect = {modalX + modalWidth - 200, modalY + modalHeight - 40, 80, 30},
        .text = "Apply",
        .hovered = false,
        .onClick = [this]() {
            if (onSettingsChanged) {
                onSettingsChanged(currentSettings);
            }
            this->hide();
        }
    });
}

bool SettingsModal::handleEvent(const SDL_Event& e) {
    if (!visible) return false;
    
    switch (e.type) {
        case SDL_MOUSEMOTION: {
            int mouseX = e.motion.x;
            int mouseY = e.motion.y;
            
            // Update hover states
            for (auto& button : buttons) {
                button.hovered = (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
                                 mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h);
            }
            
            for (auto& checkbox : checkboxes) {
                checkbox.hovered = (mouseX >= checkbox.rect.x && mouseX <= checkbox.rect.x + checkbox.rect.w &&
                                   mouseY >= checkbox.rect.y && mouseY <= checkbox.rect.y + checkbox.rect.h);
            }
            
            for (auto& slider : sliders) {
                if (slider.dragging) {
                    updateSliderValue(slider, mouseX);
                }
                slider.thumbRect.h = (mouseX >= slider.thumbRect.x && mouseX <= slider.thumbRect.x + slider.thumbRect.w &&
                                     mouseY >= slider.thumbRect.y && mouseY <= slider.thumbRect.y + slider.thumbRect.h) ? 25 : 20;
            }
            break;
        }
        
        case SDL_MOUSEBUTTONDOWN: {
            if (e.button.button == SDL_BUTTON_LEFT) {
                int mouseX = e.button.x;
                int mouseY = e.button.y;
                
                // Check buttons
                for (auto& button : buttons) {
                    if (button.hovered && button.onClick) {
                        button.onClick();
                        return true;
                    }
                }
                
                // Check checkboxes
                for (auto& checkbox : checkboxes) {
                    if (checkbox.hovered) {
                        *checkbox.value = !(*checkbox.value);
                        return true;
                    }
                }
                
                // Check sliders
                for (auto& slider : sliders) {
                    if (mouseX >= slider.thumbRect.x && mouseX <= slider.thumbRect.x + slider.thumbRect.w &&
                        mouseY >= slider.thumbRect.y && mouseY <= slider.thumbRect.y + slider.thumbRect.h) {
                        slider.dragging = true;
                        return true;
                    }
                }
                
                // If click is outside modal, close it
                if (mouseX < modalX || mouseX > modalX + modalWidth ||
                    mouseY < modalY || mouseY > modalY + modalHeight) {
                    hide();
                    return true;
                }
            }
            break;
        }
        
        case SDL_MOUSEBUTTONUP: {
            if (e.button.button == SDL_BUTTON_LEFT) {
                for (auto& slider : sliders) {
                    slider.dragging = false;
                }
            }
            break;
        }
        
        case SDL_KEYDOWN: {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                hide();
                return true;
            }
            break;
        }
    }
    
    return false;
}

void SettingsModal::render() {
    if (!visible) return;
    
    // Draw semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    SDL_RenderFillRect(renderer, nullptr);
    
    // Draw modal background
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_Rect modalRect = {modalX, modalY, modalWidth, modalHeight};
    SDL_RenderFillRect(renderer, &modalRect);
    
    // Draw modal border
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &modalRect);
    
    // Draw title
    SDL_Color white = {255, 255, 255, 255};
    drawText("Game Settings", modalX + 20, modalY + 15, white);
    
    // Draw checkboxes
    for (const auto& checkbox : checkboxes) {
        // Draw checkbox background
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &checkbox.rect);
        
        // Draw checkbox border
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &checkbox.rect);
        
        // Draw checkmark if checked
        if (*checkbox.value) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderDrawLine(renderer, 
                checkbox.rect.x + 4, checkbox.rect.y + checkbox.rect.h / 2,
                checkbox.rect.x + checkbox.rect.w / 2, checkbox.rect.y + checkbox.rect.h - 4);
            SDL_RenderDrawLine(renderer,
                checkbox.rect.x + checkbox.rect.w / 2, checkbox.rect.y + checkbox.rect.h - 4,
                checkbox.rect.x + checkbox.rect.w - 4, checkbox.rect.y + 4);
        }
        
        // Draw label
        drawText(checkbox.label, checkbox.rect.x + checkbox.rect.w + 10, 
                checkbox.rect.y + (checkbox.rect.h - 20) / 2, white);
    }
    
    // Draw sliders
    for (const auto& slider : sliders) {
        // Draw slider track
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &slider.trackRect);
        
        // Draw slider thumb
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(renderer, &slider.thumbRect);
        
        // Draw label and value
        std::string labelText = slider.label + ": " + std::to_string(*slider.value);
        drawText(labelText, slider.trackRect.x, slider.trackRect.y - 25, white);
    }
    
    // Draw buttons
    for (const auto& button : buttons) {
        // Button background
        SDL_SetRenderDrawColor(renderer, button.hovered ? 80 : 60, 60, 60, 255);
        SDL_RenderFillRect(renderer, &button.rect);
        
        // Button border
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &button.rect);
        
        // Button text
        drawText(button.text, 
                button.rect.x + (button.rect.w - static_cast<int>(button.text.length()) * 8) / 2,
                button.rect.y + (button.rect.h - 20) / 2, white);
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void SettingsModal::drawRoundedRect(const SDL_Rect& rect, int radius, SDL_Color color) {
    // Simple rectangle for now (rounded corners would require more complex drawing)
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

void SettingsModal::drawText(const std::string& text, int x, int y, SDL_Color color) {
    // Simple text rendering using SDL2 primitives
    // In a real implementation, you'd use SDL_ttf
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    // This is a very basic text rendering - just for demonstration
    // Each character is approximately 8x16 pixels
    for (size_t i = 0; i < text.length(); i++) {
        // Simple character rendering (just draw some lines)
        char c = text[i];
        int charX = x + i * 8;
        
        // Draw a simple representation of the character
        if (c != ' ') {
            SDL_Rect charRect = {charX, y, 6, 16};
            SDL_RenderDrawRect(renderer, &charRect);
        }
    }
}

void SettingsModal::updateSliderValue(Slider& slider, int mouseX) {
    // Calculate new thumb position
    int newThumbX = mouseX - slider.thumbRect.w / 2;
    
    // Clamp to track boundaries
    newThumbX = std::max(slider.trackRect.x, newThumbX);
    newThumbX = std::min(slider.trackRect.x + slider.trackRect.w - slider.thumbRect.w, newThumbX);
    
    // Update thumb position
    slider.thumbRect.x = newThumbX;
    
    // Calculate value based on position
    float normalized = static_cast<float>(newThumbX - slider.trackRect.x) / 
                      (slider.trackRect.w - slider.thumbRect.w);
    *slider.value = slider.minValue + static_cast<int>(normalized * (slider.maxValue - slider.minValue));
}
