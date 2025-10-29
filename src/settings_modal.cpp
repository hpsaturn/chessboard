#include "settings_modal.h"
#include <SDL2/SDL.h>
#include <iostream>

SettingsModal::SettingsModal(SDL_Renderer* renderer, int screenWidth, int screenHeight)
    : renderer(renderer), screenWidth(screenWidth), screenHeight(screenHeight), visible(false) {
    
    // Initialize default settings
    currentSettings.soundEnabled = true;
    currentSettings.showLegalMoves = true;
    currentSettings.highlightLastMove = true;
    currentSettings.difficultyLevel = 3;
    currentSettings.boardTheme = "Classic";
    currentSettings.pieceSet = "Standard";
    
    // Calculate modal position and size
    modalWidth = 300;
    modalHeight = 400;
    modalX = (screenWidth - modalWidth) / 2;
    modalY = (screenHeight - modalHeight) / 2;
    
    // Initialize UI elements
    initializeUI();
}

SettingsModal::~SettingsModal() {
    // Cleanup if needed
}

void SettingsModal::initializeUI() {
    // Clear existing UI elements
    buttons.clear();
    checkboxes.clear();
    sliders.clear();
    
    // Calculate positions for UI elements
    int padding = 20;
    int elementWidth = modalWidth - 2 * padding;
    int elementHeight = 30;
    int currentY = modalY + padding;
    
    // Sound checkbox
    Checkbox soundCheckbox;
    soundCheckbox.rect = {modalX + padding, currentY, elementWidth, elementHeight};
    soundCheckbox.label = "Sound";
    soundCheckbox.value = &currentSettings.soundEnabled;
    soundCheckbox.hovered = false;
    checkboxes.push_back(soundCheckbox);
    currentY += elementHeight + 10;
    
    // Legal moves checkbox
    Checkbox legalMovesCheckbox;
    legalMovesCheckbox.rect = {modalX + padding, currentY, elementWidth, elementHeight};
    legalMovesCheckbox.label = "Show Legal Moves";
    legalMovesCheckbox.value = &currentSettings.showLegalMoves;
    legalMovesCheckbox.hovered = false;
    checkboxes.push_back(legalMovesCheckbox);
    currentY += elementHeight + 10;
    
    // Highlight last move checkbox
    Checkbox highlightLastMoveCheckbox;
    highlightLastMoveCheckbox.rect = {modalX + padding, currentY, elementWidth, elementHeight};
    highlightLastMoveCheckbox.label = "Highlight Last Move";
    highlightLastMoveCheckbox.value = &currentSettings.highlightLastMove;
    highlightLastMoveCheckbox.hovered = false;
    checkboxes.push_back(highlightLastMoveCheckbox);
    currentY += elementHeight + 10;
    
    // Difficulty slider
    Slider difficultySlider;
    difficultySlider.trackRect = {modalX + padding, currentY, elementWidth, elementHeight};
    difficultySlider.thumbRect = {modalX + padding, currentY, 20, elementHeight};
    difficultySlider.label = "Difficulty";
    difficultySlider.value = &currentSettings.difficultyLevel;
    difficultySlider.minValue = 1;
    difficultySlider.maxValue = 5;
    difficultySlider.dragging = false;
    sliders.push_back(difficultySlider);
    currentY += elementHeight + 10;
    
    // Apply button
    Button applyButton;
    applyButton.rect = {modalX + padding, currentY, (elementWidth / 2) - 5, elementHeight};
    applyButton.text = "Apply";
    applyButton.hovered = false;
    applyButton.onClick = [this]() {
        if (onSettingsChanged) {
            onSettingsChanged(currentSettings);
        }
        hide();
    };
    buttons.push_back(applyButton);
    
    // Close button
    Button closeButton;
    closeButton.rect = {modalX + padding + (elementWidth / 2) + 5, currentY, (elementWidth / 2) - 5, elementHeight};
    closeButton.text = "Close";
    closeButton.hovered = false;
    closeButton.onClick = [this]() {
        hide();
    };
    buttons.push_back(closeButton);
}

void SettingsModal::show() {
    visible = true;
}

void SettingsModal::hide() {
    visible = false;
}

bool SettingsModal::handleEvent(const SDL_Event& e) {
    if (!visible) return false;
    
    switch (e.type) {
        case SDL_MOUSEBUTTONDOWN: {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            
            // Check if click is within modal
            if (mouseX >= modalX && mouseX <= modalX + modalWidth &&
                mouseY >= modalY && mouseY <= modalY + modalHeight) {
                
                // Handle checkbox toggles
                for (auto& checkbox : checkboxes) {
                    if (mouseX >= checkbox.rect.x && mouseX <= checkbox.rect.x + checkbox.rect.w &&
                        mouseY >= checkbox.rect.y && mouseY <= checkbox.rect.y + checkbox.rect.h) {
                        *checkbox.value = !(*checkbox.value);
                        return true;
                    }
                }
                
                // Handle slider dragging
                for (auto& slider : sliders) {
                    if (mouseX >= slider.trackRect.x && mouseX <= slider.trackRect.x + slider.trackRect.w &&
                        mouseY >= slider.trackRect.y && mouseY <= slider.trackRect.y + slider.trackRect.h) {
                        slider.dragging = true;
                        updateSliderValue(slider, mouseX);
                        return true;
                    }
                }
                
                // Handle buttons
                for (auto& button : buttons) {
                    if (mouseX >= button.rect.x && mouseX <= button.rect.x + button.rect.w &&
                        mouseY >= button.rect.y && mouseY <= button.rect.y + button.rect.h) {
                        if (button.onClick) {
                            button.onClick();
                        }
                        return true;
                    }
                }
                
                return true; // Modal consumed the event
            }
            break;
        }
        
        case SDL_MOUSEBUTTONUP: {
            // Stop slider dragging
            for (auto& slider : sliders) {
                slider.dragging = false;
            }
            break;
        }
        
        case SDL_MOUSEMOTION: {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            
            // Update slider value if dragging
            for (auto& slider : sliders) {
                if (slider.dragging) {
                    updateSliderValue(slider, mouseX);
                    return true;
                }
            }
            break;
        }
        
        case SDL_KEYDOWN: {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                hide();
                return true; // Modal consumed the ESC event
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
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128); // Semi-transparent black
    SDL_Rect overlay = {0, 0, screenWidth, screenHeight};
    SDL_RenderFillRect(renderer, &overlay);
    
    // Draw modal background
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Dark gray
    SDL_Rect modalRect = {modalX, modalY, modalWidth, modalHeight};
    SDL_RenderFillRect(renderer, &modalRect);
    
    // Draw modal border
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Light gray border
    SDL_Rect modalBorder = {modalX - 2, modalY - 2, modalWidth + 4, modalHeight + 4};
    SDL_RenderDrawRect(renderer, &modalBorder);
    
    // Draw UI elements
    for (const auto& checkbox : checkboxes) {
        drawCheckbox(checkbox);
    }
    
    for (const auto& slider : sliders) {
        drawSlider(slider);
    }
    
    for (const auto& button : buttons) {
        drawButton(button);
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void SettingsModal::drawCheckbox(const Checkbox& checkbox) {
    // Draw checkbox background
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_RenderFillRect(renderer, &checkbox.rect);
    
    // Draw checkbox border
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderDrawRect(renderer, &checkbox.rect);
    
    // Draw checkmark if checked
    if (*checkbox.value) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green checkmark
        SDL_Rect checkRect = {checkbox.rect.x + 5, checkbox.rect.y + 5, checkbox.rect.w - 10, checkbox.rect.h - 10};
        SDL_RenderFillRect(renderer, &checkRect);
    }
    
    // Draw label (placeholder)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect labelRect = {checkbox.rect.x + 35, checkbox.rect.y + 10, checkbox.rect.w - 40, 10};
    SDL_RenderFillRect(renderer, &labelRect);
}

void SettingsModal::drawSlider(const Slider& slider) {
    // Draw slider track
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_RenderFillRect(renderer, &slider.trackRect);
    
    // Draw slider track border
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderDrawRect(renderer, &slider.trackRect);
    
    // Calculate thumb position based on value
    float normalizedValue = static_cast<float>(*slider.value - slider.minValue) / (slider.maxValue - slider.minValue);
    int thumbX = slider.trackRect.x + static_cast<int>(normalizedValue * (slider.trackRect.w - 20));
    SDL_Rect thumbRect = {thumbX, slider.trackRect.y, 20, slider.trackRect.h};
    
    // Draw slider thumb
    SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255); // Blue thumb
    SDL_RenderFillRect(renderer, &thumbRect);
    
    // Draw label (placeholder)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect labelRect = {slider.trackRect.x + 5, slider.trackRect.y + 10, slider.trackRect.w - 10, 10};
    SDL_RenderFillRect(renderer, &labelRect);
}

void SettingsModal::drawButton(const Button& button) {
    // Draw button background
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &button.rect);
    
    // Draw button border
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderDrawRect(renderer, &button.rect);
    
    // Draw label (placeholder)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect labelRect = {button.rect.x + 10, button.rect.y + 10, button.rect.w - 20, 10};
    SDL_RenderFillRect(renderer, &labelRect);
}

void SettingsModal::updateSliderValue(Slider& slider, int mouseX) {
    // Calculate new value based on mouse position
    float normalizedPos = static_cast<float>(mouseX - slider.trackRect.x) / slider.trackRect.w;
    normalizedPos = std::max(0.0f, std::min(1.0f, normalizedPos));
    *slider.value = slider.minValue + static_cast<int>(normalizedPos * (slider.maxValue - slider.minValue));
}

void SettingsModal::drawRoundedRect(const SDL_Rect& rect, int radius, SDL_Color color) {
    // Simple rectangle drawing (rounded corners not implemented)
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

void SettingsModal::drawText(const std::string& text, int x, int y, SDL_Color color) {
    // Placeholder for text rendering
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect textRect = {x, y, 100, 10}; // Fixed size for placeholder
    SDL_RenderFillRect(renderer, &textRect);
}
