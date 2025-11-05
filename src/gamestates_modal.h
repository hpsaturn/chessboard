// Game States Modal Window for Chess Game
#ifndef GAMESTATES_MODAL_H
#define GAMESTATES_MODAL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <functional>
#include "chess_pieces.h"
#include "definitions.h"
#include "game_state_manager.h"

class GameStatesModal {
public:
    GameStatesModal(SDL_Renderer* renderer, int screenWidth, int screenHeight, GameStateManager* stateManager);
    ~GameStatesModal();
    
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
    
    // Load states from manager
    void loadStates();

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    int screenWidth;
    int screenHeight;
    bool visible;
    GameStateManager* stateManager;
    
    // Modal dimensions
    int modalWidth;
    int modalHeight;
    int modalX;
    int modalY;
    
    // List view state
    std::vector<GameStateManager::GameState> states;
    int selectedIndex;
    int scrollOffset;
    int itemsPerPage;
    
    // Board preview
    int previewBoardSize;
    int previewBoardX;
    int previewBoardY;
    
    // Helper functions
    void drawText(const std::string& text, int x, int y, SDL_Color color);
    SDL_Texture* createTextTexture(const std::string& text, SDL_Color color);
    void renderListView();
    void renderBoardPreview();
    void renderBoardFromFEN(const std::string& fen);
    void handleMouseClick(int x, int y);
    void handleKeyPress(SDL_Keycode key);
    void loadSelectedState();

};

#endif // GAMESTATES_MODAL_H
