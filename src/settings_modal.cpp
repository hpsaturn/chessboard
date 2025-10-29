#include "settings_modal.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <algorithm>

SettingsModal::SettingsModal(SDL_Renderer* renderer, int screenWidth, int screenHeight)
    : renderer(renderer), screenWidth(screenWidth), screenHeight(screenHeight), visible(false), font(nullptr) {
    
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
    } else {
        // Load DejaVu Sans font (commonly available on Linux)
        font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14);
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        }
    }
    
    // Initialize default settings
    currentSettings.depthDifficulty = 3;
    currentSettings.maxTimePerMove = 30;
    currentSettings.matchTime = 10;
    currentSettings.soundEnabled = true;
    
    // Calculate modal position and size
    modalWidth = 320;
    modalHeight = 200;
    modalX = (screenWidth - modalWidth) / 2;
    modalY = (screenHeight - modalHeight) / 2;
    
    // Initialize UI elements
    initializeUI();
}

SettingsModal::~SettingsModal() {
    if (font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
}

void SettingsModal::initializeUI() {
    // Clear existing UI elements
    checkboxes.clear();
    sliders.clear();
    
    // Calculate positions for UI elements
    int padding = 20;
    int elementWidth = modalWidth - 2 * padding;
    int elementHeight = 25;
    int currentY = modalY + padding;
    
    // Depth difficulty slider
    Slider depthSlider;
    depthSlider.trackRect = {modalX + padding, currentY, elementWidth, elementHeight};
    depthSlider.thumbRect = {modalX + padding, currentY, 20, elementHeight};
    depthSlider.label = "Depth Difficulty: " + std::to_string(currentSettings.depthDifficulty);
    depthSlider.value = &currentSettings.depthDifficulty;
    depthSlider.minValue = 1;
    depthSlider.maxValue = 10;
    depthSlider.dragging = false;
    sliders.push_back(depthSlider);
    currentY += elementHeight + 15;
    
    // Max time per move slider
    Slider timePerMoveSlider;
    timePerMoveSlider.trackRect = {modalX + padding, currentY, elementWidth, elementHeight};
    timePerMoveSlider.thumbRect = {modalX + padding, currentY, 20, elementHeight};
    timePerMoveSlider.label = "Max Time/Move: " + std::to_string(currentSettings.maxTimePerMove) + "s";
    timePerMoveSlider.value = &currentSettings.maxTimePerMove;
    timePerMoveSlider.minValue = 0;
    timePerMoveSlider.maxValue = 300;
    timePerMoveSlider.dragging = false;
    sliders.push_back(timePerMoveSlider);
    currentY += elementHeight + 15;
    
    // Match time slider
    Slider matchTimeSlider;
    matchTimeSlider.trackRect = {modalX + padding, currentY, elementWidth, elementHeight};
    matchTimeSlider.thumbRect = {modalX + padding, currentY, 20, elementHeight};
    matchTimeSlider.label = "Match Time: " + std::to_string(currentSettings.matchTime) + "m";
    matchTimeSlider.value = &currentSettings.matchTime;
    matchTimeSlider.minValue = 0;
    matchTimeSlider.maxValue = 60;
    matchTimeSlider.dragging = false;
    sliders.push_back(matchTimeSlider);
    currentY += elementHeight + 15;
    
    // Sound checkbox
    Checkbox soundCheckbox;
    soundCheckbox.rect = {modalX + padding, currentY, elementWidth, elementHeight};
    soundCheckbox.label = "Sound: " + std::string(currentSettings.soundEnabled ? "ON" : "OFF");
    soundCheckbox.value = &currentSettings.soundEnabled;
    soundCheckbox.hovered = false;
    checkboxes.push_back(soundCheckbox);
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
                        checkbox.label = "Sound: " + std::string(*checkbox.value ? "ON" : "OFF");
                        // Apply changes immediately
                        if (onSettingsChanged) {
                            onSettingsChanged(currentSettings);
                        }
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
            
            // Update hover states
            for (auto& checkbox : checkboxes) {
                checkbox.hovered = (mouseX >= checkbox.rect.x && mouseX <= checkbox.rect.x + checkbox.rect.w &&
                                   mouseY >= checkbox.rect.y && mouseY <= checkbox.rect.y + checkbox.rect.h);
            }
            break;
        }
        
        case SDL_KEYDOWN: {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                hide();
                return true; // Modal consumed the ESC event
            }
            
            // Keyboard navigation for settings
            if (e.key.keysym.sym == SDLK_TAB || e.key.keysym.sym == SDLK_DOWN || e.key.keysym.sym == SDLK_UP) {
                // Tab navigation could be implemented here
                return true;
            }
            
            // Space/Enter to toggle sound
            if (e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_RETURN) {
                currentSettings.soundEnabled = !currentSettings.soundEnabled;
                checkboxes[0].label = "Sound: " + std::string(currentSettings.soundEnabled ? "ON" : "OFF");
                if (onSettingsChanged) {
                    onSettingsChanged(currentSettings);
                }
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
    drawText("Game Settings", modalX + 10, modalY + 5, {255, 255, 255, 255});
    
    // Draw UI elements
    for (auto& slider : sliders) {
        drawSlider(slider);
    }
    
    for (const auto& checkbox : checkboxes) {
        drawCheckbox(checkbox);
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void SettingsModal::drawCheckbox(const Checkbox& checkbox) {
    // Draw checkbox background
    SDL_SetRenderDrawColor(renderer, checkbox.hovered ? 60 : 50, checkbox.hovered ? 60 : 50, checkbox.hovered ? 60 : 50, 255);
    SDL_RenderFillRect(renderer, &checkbox.rect);
    
    // Draw checkbox border
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &checkbox.rect);
    
    // Draw checkmark if checked
    if (*checkbox.value) {
        SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255); // Green checkmark
        SDL_Rect checkRect = {checkbox.rect.x + 5, checkbox.rect.y + 5, checkbox.rect.w - 10, checkbox.rect.h - 10};
        SDL_RenderFillRect(renderer, &checkRect);
    }
    
    // Draw label
    drawText(checkbox.label, checkbox.rect.x + 30, checkbox.rect.y + 7, {255, 255, 255, 255});
}

void SettingsModal::drawSlider(Slider& slider) {
    // Draw slider track
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &slider.trackRect);
    
    // Draw slider track border
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &slider.trackRect);
    
    // Calculate thumb position based on value
    float normalizedValue = static_cast<float>(*slider.value - slider.minValue) / (slider.maxValue - slider.minValue);
    int thumbX = slider.trackRect.x + static_cast<int>(normalizedValue * (slider.trackRect.w - 20));
    SDL_Rect thumbRect = {thumbX, slider.trackRect.y, 20, slider.trackRect.h};
    
    // Draw slider thumb
    SDL_SetRenderDrawColor(renderer, 0, 120, 255, 255); // Blue thumb
    SDL_RenderFillRect(renderer, &thumbRect);
    
    // Draw thumb border
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &thumbRect);
    
    // Update and draw label with current value
    if (slider.label.find("Depth") != std::string::npos) {
        slider.label = "Depth Difficulty: " + std::to_string(*slider.value);
    } else if (slider.label.find("Time/Move") != std::string::npos) {
        slider.label = "Max Time/Move: " + std::to_string(*slider.value) + "s";
    } else if (slider.label.find("Match Time") != std::string::npos) {
        slider.label = "Match Time: " + std::to_string(*slider.value) + "m";
    }
    
    drawText(slider.label, slider.trackRect.x, slider.trackRect.y - 18, {255, 255, 255, 255});
}

void SettingsModal::updateSliderValue(Slider& slider, int mouseX) {
    // Calculate new value based on mouse position
    float normalizedPos = static_cast<float>(mouseX - slider.trackRect.x) / slider.trackRect.w;
    normalizedPos = std::max(0.0f, std::min(1.0f, normalizedPos));
    int newValue = slider.minValue + static_cast<int>(normalizedPos * (slider.maxValue - slider.minValue));
    
    if (newValue != *slider.value) {
        *slider.value = newValue;
        // Apply changes immediately
        if (onSettingsChanged) {
            onSettingsChanged(currentSettings);
        }
    }
}

void SettingsModal::drawText(const std::string& text, int x, int y, SDL_Color color) {
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

SDL_Texture* SettingsModal::createTextTexture(const std::string& text, SDL_Color color) {
    if (!font) return nullptr;
    
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        return nullptr;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}
