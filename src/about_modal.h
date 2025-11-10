// About Modal Window for Chess Game
#ifndef ABOUT_MODAL_H
#define ABOUT_MODAL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

class AboutModal {
public:
    AboutModal(SDL_Renderer* renderer, int screenWidth, int screenHeight);
    ~AboutModal();
    
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
    
    // Helper functions
    void drawText(const std::string& text, int x, int y, SDL_Color color);
    SDL_Texture* createTextTexture(const std::string& text, SDL_Color color);

};

#endif // ABOUT_MODAL_H
