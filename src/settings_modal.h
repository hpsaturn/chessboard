// Settings Modal Window for Chess Game
#ifndef SETTINGS_MODAL_H
#define SETTINGS_MODAL_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <functional>

class SettingsModal {
public:
    SettingsModal(SDL_Renderer* renderer, int screenWidth, int screenHeight);
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
        bool soundEnabled;
        bool showLegalMoves;
        bool highlightLastMove;
        int difficultyLevel; // 1-5
        std::string boardTheme;
        std::string pieceSet;
    };
    
    // Get current settings
    Settings getSettings() const { return currentSettings; }
    
    // Set settings change callback
    void setOnSettingsChanged(std::function<void(const Settings&)> callback) {
        onSettingsChanged = callback;
    }

private:
    SDL_Renderer* renderer;
    int screenWidth;
    int screenHeight;
    bool visible;
    
    // Modal dimensions
    int modalWidth;
    int modalHeight;
    int modalX;
    int modalY;
    
    // UI elements
    struct Button {
        SDL_Rect rect;
        std::string text;
        bool hovered;
        std::function<void()> onClick;
    };
    
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
    
    std::vector<Button> buttons;
    std::vector<Checkbox> checkboxes;
    std::vector<Slider> sliders;
    
    // Current settings
    Settings currentSettings;
    
    // Callback for settings changes
    std::function<void(const Settings&)> onSettingsChanged;
    
    // Initialize UI elements
    void initializeUI();
    
    // Helper functions
    void drawCheckbox(const Checkbox& checkbox);
    void drawSlider(const Slider& slider);
    void drawButton(const Button& button);
    void updateSliderValue(Slider& slider, int mouseX);
    void drawRoundedRect(const SDL_Rect& rect, int radius, SDL_Color color);
    void drawText(const std::string& text, int x, int y, SDL_Color color);
};

#endif // SETTINGS_MODAL_H
