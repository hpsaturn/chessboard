// Settings Modal Window for Chess Game
#ifndef SETTINGS_MODAL_H
#define SETTINGS_MODAL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <functional>
#include "config_manager.h"

class SettingsModal {
public:
    SettingsModal(SDL_Renderer* renderer, int screenWidth, int screenHeight, ConfigManager* configManager = nullptr);
    ~SettingsModal();
    
    // Show the modal window
    void show();
    
    // Hide the modal window
    void hide();
    
    // Handle events for the modal
    bool handleEvent(const SDL_Event& e);
    
    // Render the modal
    void render();
    
    // Check if modal is visible
    bool isVisible() const { return visible; }
    
    // Settings structure
    struct Settings {
        int depthDifficulty = 1;      // 1-10
        int maxTimePerMove = 2;       // seconds (0-300)
        int matchTime = 10;            // minutes (0-60)
        bool soundEnabled = false;
    };
    
    // Get current settings
    Settings getSettings() const { return currentSettings; }
    
    // Set settings change callback
    void setOnSettingsChanged(std::function<void(const Settings&)> callback) {
        onSettingsChanged = callback;
    }

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    int screenWidth;
    int screenHeight;
    bool visible;
    int focusedElement; // Index of currently focused element (-1 for none)
    ConfigManager* configManager;
    
    // Modal dimensions
    int modalWidth;
    int modalHeight;
    int modalX;
    int modalY;
    
    // UI elements
    struct Checkbox {
        SDL_Rect rect;
        std::string label;
        bool* value;
        bool hovered;
    };
    
    struct Slider {
        SDL_Rect trackRect;
        SDL_Rect thumbRect;
        std::string label;
        int* value;
        int minValue;
        int maxValue;
        bool dragging;
    };
    
    std::vector<Checkbox> checkboxes;
    std::vector<Slider> sliders;
    
    // Current settings
    Settings currentSettings;
    
    // Callback for settings changes
    std::function<void(const Settings&)> onSettingsChanged;
    
    // Initialize UI elements
    void initializeUI();
    
    // Helper functions
    void drawCheckbox(const Checkbox& checkbox, bool focused);
    void drawSlider(Slider& slider, bool focused);
    void updateSliderValue(Slider& slider, int mouseX);
    void drawText(const std::string& text, int x, int y, SDL_Color color);
    SDL_Texture* createTextTexture(const std::string& text, SDL_Color color);
    
    // Save settings to config file
    void saveSettingsToFile();
};

#endif // SETTINGS_MODAL_H
