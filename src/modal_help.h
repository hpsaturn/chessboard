// Help Modal Window for Chess Game
#ifndef HELP_MODAL_H
#define HELP_MODAL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include "modal_base.h"

class HelpModal : public ModalBase {
public:
    HelpModal(SDL_Renderer* renderer, int screenWidth, int screenHeight);
    ~HelpModal();
    
    // Override handleEvent for scroll functionality
    bool handleEvent(const SDL_Event& e) override;
    
    // Override render for custom help content
    void render() override;

private:
    // Text content
    std::vector<std::string> helpLines;
    
    // Scroll position
    int scrollOffset;
    
    // Helper functions
    void initializeHelpContent();
};

#endif // HELP_MODAL_H
