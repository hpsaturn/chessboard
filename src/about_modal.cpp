#include "about_modal.h"
#include "version.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <algorithm>

AboutModal::AboutModal(SDL_Renderer* renderer, int screenWidth, int screenHeight)
    : ModalBase(renderer, screenWidth, screenHeight, 300, 300) {
    // Constructor logic is now handled by ModalBase
}

void AboutModal::render() {
    if (!isVisible() || !font) return;
    
    // Draw modal background and border using base class methods
    renderModalBackground();
    renderModalBorder();
    
    // Get dynamic version
    std::string versionInfo = Version::getBuildInfo();
    
    // Draw title
    SDL_Color titleColor = {255, 255, 255, 255};  // White
    drawText("About Chess Game", modalX + 10, modalY + 10, titleColor);
    drawText("(Hpsaturn 2025 " + versionInfo + ")", modalX + 10, modalY + 25, titleColor);
    
    // Draw separator line
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);  // Medium gray
    SDL_RenderDrawLine(renderer, modalX + 10, modalY + 45, modalX + modalWidth - 10, modalY + 45);
    
    // Draw credits content
    SDL_Color textColor = {220, 220, 220, 255};  // Light gray
    int currentY = modalY + 55;
    
    drawText("Credits:", modalX + 15, currentY, textColor);
    currentY += 25;
    
    drawText("- Special thanks to my son, M.V", modalX + 20, currentY, textColor);
    currentY += 15;
    drawText("  for continuous critics and beta tests", modalX + 20, currentY, textColor);
    currentY += 25;
    
    drawText("- Thanks to @lunokjod", modalX + 20, currentY, textColor);
    currentY += 15;
    drawText("  for his MongoIA assistant", modalX + 20, currentY, textColor);
    currentY += 25;
    
    drawText("- Thanks to Chess SDL project", modalX + 20, currentY, textColor);
    currentY += 15;
    drawText("  for CC BY-SA image assets", modalX + 20, currentY, textColor);
    currentY += 25;
    
    // Draw license info
    SDL_Color licenseColor = {180, 180, 255, 255};  // Light blue
    drawText("  License: GPLv3", modalX + 15, currentY, licenseColor);
    currentY += 20;
    drawText("  Assets: CC BY-SA 3.0", modalX + 15, currentY, licenseColor);
    
    // Draw close instruction using base class method
    renderCloseInstruction();
}
