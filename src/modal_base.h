// Base Modal Window Class for Chess Game
#ifndef MODAL_BASE_H
#define MODAL_BASE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class ModalBase {
public:
    ModalBase(SDL_Renderer* renderer, int screenWidth, int screenHeight, 
              int modalWidth = 300, int modalHeight = 300);
    virtual ~ModalBase();
    
    // Show the modal window
    void show();
    
    // Hide the modal window
    void hide();
    
    // Handle events for the modal (virtual for override)
    virtual bool handleEvent(const SDL_Event& e);
    
    // Render the modal (pure virtual - must be implemented by derived classes)
    virtual void render() = 0;
    
    // Check if modal is visible
    bool isVisible() const { return visible; }

protected:
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
    
    // Common modal rendering utilities
    void renderModalBackground();
    void renderModalBorder();
    void renderCloseInstruction();

};

#endif // MODAL_BASE_H
