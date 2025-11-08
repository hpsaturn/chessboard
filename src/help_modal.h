// Help Modal Window for Chess Game
#ifndef HELP_MODAL_H
#define HELP_MODAL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

class HelpModal {
public:
    HelpModal(SDL_Renderer* renderer, int screenWidth, int screenHeight);
    ~HelpModal();
    
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

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    int screenWidth;
    int screenHeight;
    bool visible;
    
    // Modal dimensions
    int modalWidth;
    int modalHeight;
    int modalX;
    int modalY;
    
    // Text content
    std::vector<std::string> helpLines;
    
    // Scroll position
    int scrollOffset;
    
    // Helper functions
    void drawText(const std::string& text, int x, int y, SDL_Color color);
    SDL_Texture* createTextTexture(const std::string& text, SDL_Color color);
    void initializeHelpContent();
};

#endif // HELP_MODAL_H
