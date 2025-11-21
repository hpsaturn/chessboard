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
#include "modal_base.h"

class GameStatesModal : public ModalBase {
public:
    GameStatesModal(SDL_Renderer* renderer, int screenWidth, int screenHeight, GameStateManager* stateManager);
    
    // Handle events for the modal
    bool handleEvent(const SDL_Event& e) override;
    
    // Render the modal
    void render() override;

    void show();
    
    // Load states from manager
    void loadStates();
    
    // Set callback for when a state is selected
    void setOnStateSelected(std::function<void(const std::string& fen)> callback) {
        onStateSelected = callback;
    }

private:
    GameStateManager* stateManager;
    
    // List view state
    std::vector<GameStateManager::GameState> states;
    int selectedIndex;
    int indexShowed;
    int scrollOffset;
    int itemsPerPage;
    
    // Board preview
    int previewBoardSize;
    int previewBoardX;
    int previewBoardY;
    
    // Callback for state selection
    std::function<void(const std::string& fen)> onStateSelected;
    
    // Helper functions
    void renderListView();
    void renderBoardPreview();
    void renderBoardFromFEN(const std::string& fen);
    void handleMouseClick(int x, int y);
    void handleKeyPress(SDL_Keycode key);
    void loadSelectedState();
    void removeSelectedState();

};

#endif // GAMESTATES_MODAL_H
