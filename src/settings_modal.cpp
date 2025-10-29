#include "settings_modal.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <algorithm>

SettingsModal::SettingsModal(SDL_Renderer* renderer, int screenWidth, int screenHeight)
    : renderer(renderer), screenWidth(screenWidth), screenHeight(screenHeight), visible(false), font(nullptr), focusedElement(-1) {
    
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
    } else {
        // Load DejaVu Sans font (commonly available on Linux)
        font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 13);
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
    modalWidth = 250;
    modalHeight = 220;
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
    int padding = 25;
    int elementWidth = modalWidth - 2 * padding;
    int elementHeight = 20;
    int currentY = modalY + padding + 30;
    
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
    soundCheckbox.rect = {modalX + padding, currentY, elementWidth, elementHeight+10};
    soundCheckbox.label = "Sound: " + std::string(currentSettings.soundEnabled ? "ON" : "OFF");
    soundCheckbox.value = &currentSettings.soundEnabled;
    soundCheckbox.hovered = false;
    checkboxes.push_back(soundCheckbox);
}

void SettingsModal::show() {
    visible = true;
    focusedElement = 0; // Focus first element when modal opens
}

void SettingsModal::hide() {
    visible = false;
    focusedElement = -1; // Reset focus when modal closes
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
                for (size_t i = 0; i < checkboxes.size(); ++i) {
                    auto& checkbox = checkboxes[i];
                    if (mouseX >= checkbox.rect.x && mouseX <= checkbox.rect.x + checkbox.rect.w &&
                        mouseY >= checkbox.rect.y && mouseY <= checkbox.rect.y + checkbox.rect.h) {
                        *checkbox.value = !(*checkbox.value);
                        checkbox.label = "Sound: " + std::string(*checkbox.value ? "ON" : "OFF");
                        focusedElement = sliders.size() + i; // Set focus to this checkbox
                        // Apply changes immediately
                        if (onSettingsChanged) {
                            onSettingsChanged(currentSettings);
                        }
                        return true;
                    }
                }
                
                // Handle slider dragging
                for (size_t i = 0; i < sliders.size(); ++i) {
                    auto& slider = sliders[i];
                    if (mouseX >= slider.trackRect.x && mouseX <= slider.trackRect.x + slider.trackRect.w &&
                        mouseY >= slider.trackRect.y && mouseY <= slider.trackRect.y + slider.trackRect.h) {
                        slider.dragging = true;
                        focusedElement = i; // Set focus to this slider
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
            
            // Keyboard navigation
            if (e.key.keysym.sym == SDLK_TAB || e.key.keysym.sym == SDLK_DOWN) {
                // Move focus to next element
                focusedElement = (focusedElement + 1) % (sliders.size() + checkboxes.size());
                return true;
            }
            
            if (e.key.keysym.sym == SDLK_UP) {
                // Move focus to previous element
                focusedElement = (focusedElement - 1 + (sliders.size() + checkboxes.size())) % (sliders.size() + checkboxes.size());
                return true;
            }
            
            // Handle focused element actions
            if (focusedElement >= 0) {
                // If focused on a slider
                if (focusedElement < static_cast<int>(sliders.size())) {
                    auto& slider = sliders[focusedElement];
                    
                    if (e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_KP_MINUS) {
                        // Decrease slider value
                        *slider.value = std::max(slider.minValue, *slider.value - 1);
                        if (onSettingsChanged) {
                            onSettingsChanged(currentSettings);
                        }
                        return true;
                    }
                    
                    if (e.key.keysym.sym == SDLK_RIGHT || e.key.keysym.sym == SDLK_KP_PLUS) {
                        // Increase slider value
                        *slider.value = std::min(slider.maxValue, *slider.value + 1);
                        if (onSettingsChanged) {
                            onSettingsChanged(currentSettings);
                        }
                        return true;
                    }
                    
                    if (e.key.keysym.sym == SDLK_HOME) {
                        // Set slider to minimum value
                        *slider.value = slider.minValue;
                        if (onSettingsChanged) {
                            onSettingsChanged(currentSettings);
                        }
                        return true;
                    }
                    
                    if (e.key.keysym.sym == SDLK_END) {
                        // Set slider to maximum value
                        *slider.value = slider.maxValue;
                        if (onSettingsChanged) {
                            onSettingsChanged(currentSettings);
                        }
                        return true;
                    }
                }
                // If focused on a checkbox
                else if (focusedElement >= static_cast<int>(sliders.size()) && 
                         focusedElement < static_cast<int>(sliders.size() + checkboxes.size())) {
                    int checkboxIndex = focusedElement - sliders.size();
                    
                    if (e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_RETURN) {
                        // Toggle checkbox
                        auto& checkbox = checkboxes[checkboxIndex];
                        *checkbox.value = !(*checkbox.value);
                        checkbox.label = "Sound: " + std::string(*checkbox.value ? "ON" : "OFF");
                        if (onSettingsChanged) {
                            onSettingsChanged(currentSettings);
                        }
                        return true;
                    }
                }
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
    for (size_t i = 0; i < sliders.size(); ++i) {
        drawSlider(sliders[i], i == focusedElement);
    }
    
    for (size_t i = 0; i < checkboxes.size(); ++i) {
        drawCheckbox(checkboxes[i], (sliders.size() + i) == focusedElement);
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void SettingsModal::drawCheckbox(const Checkbox& checkbox, bool focused) {
    // Draw checkbox background
    SDL_SetRenderDrawColor(renderer, 
                           focused ? 80 : (checkbox.hovered ? 60 : 50), 
                           focused ? 80 : (checkbox.hovered ? 60 : 50), 
                           focused ? 80 : (checkbox.hovered ? 60 : 50), 255);
    SDL_RenderFillRect(renderer, &checkbox.rect);
    
    // Draw checkbox border
    SDL_SetRenderDrawColor(renderer, focused ? 200 : 100, focused ? 200 : 100, focused ? 200 : 100, 255);
    SDL_RenderDrawRect(renderer, &checkbox.rect);
    
    // Draw focus indicator
    if (focused) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow focus indicator
        SDL_Rect focusRect = {checkbox.rect.x - 2, checkbox.rect.y - 2, checkbox.rect.w + 4, checkbox.rect.h + 4};
        SDL_RenderDrawRect(renderer, &focusRect);
    }
    
    // Draw checkmark if checked
    if (*checkbox.value) {
        SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255); // Green checkmark
        SDL_Rect checkRect = {checkbox.rect.x + 5, checkbox.rect.y + 5, checkbox.rect.w - 10, checkbox.rect.h - 10};
        SDL_RenderFillRect(renderer, &checkRect);
    }
    
    // Draw label
    drawText(checkbox.label, checkbox.rect.x + 30, checkbox.rect.y + 7, {255, 255, 255, 255});
}

void SettingsModal::drawSlider(Slider& slider, bool focused) {
    // Draw slider track
    SDL_SetRenderDrawColor(renderer, focused ? 60 : 50, focused ? 60 : 50, focused ? 60 : 50, 255);
    SDL_RenderFillRect(renderer, &slider.trackRect);
    
    // Draw slider track border
    SDL_SetRenderDrawColor(renderer, focused ? 150 : 100, focused ? 150 : 100, focused ? 150 : 100, 255);
    SDL_RenderDrawRect(renderer, &slider.trackRect);
    
    // Calculate thumb position based on value
    float normalizedValue = static_cast<float>(*slider.value - slider.minValue) / (slider.maxValue - slider.minValue);
    int thumbX = slider.trackRect.x + static_cast<int>(normalizedValue * (slider.trackRect.w - 20));
    SDL_Rect thumbRect = {thumbX, slider.trackRect.y, 20, slider.trackRect.h};
    
    // Draw slider thumb
    SDL_SetRenderDrawColor(renderer, focused ? 0 : 0, focused ? 180 : 120, focused ? 255 : 255, 255); // Brighter blue when focused
    SDL_RenderFillRect(renderer, &thumbRect);
    
    // Draw thumb border
    SDL_SetRenderDrawColor(renderer, focused ? 255 : 200, focused ? 255 : 200, focused ? 255 : 200, 255);
    SDL_RenderDrawRect(renderer, &thumbRect);
    
    // Draw focus indicator
    if (focused) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow focus indicator
        SDL_Rect focusRect = {slider.trackRect.x - 2, slider.trackRect.y - 2, slider.trackRect.w + 4, slider.trackRect.h + 4};
        SDL_RenderDrawRect(renderer, &focusRect);
    }
    
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
