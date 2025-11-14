// About Modal Window for Chess Game
#ifndef ABOUT_MODAL_H
#define ABOUT_MODAL_H

#include "modal_base.h"
#include <string>

class AboutModal : public ModalBase {
public:
    AboutModal(SDL_Renderer* renderer, int screenWidth, int screenHeight);
    
    // Override render method
    void render() override;

};

#endif // ABOUT_MODAL_H
