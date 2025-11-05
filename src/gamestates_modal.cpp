#include "gamestates_modal.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "chess_pieces_sdl.h"
#include "chess_game_logic.h"

GameStatesModal::GameStatesModal(SDL_Renderer* renderer, int screenWidth, int screenHeight, GameStateManager* stateManager)
    : renderer(renderer), screenWidth(screenWidth), screenHeight(screenHeight), 
      visible(false), stateManager(stateManager), font(nullptr), selectedIndex(-1), scrollOffset(0),
      onStateSelected(nullptr) {
    
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
    } else {
        // Load DejaVu Sans font (commonly available on Linux)
        font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 11);
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        }
    }
    
    // Calculate modal position and size (same as gameinfo_modal)
    modalWidth = 300;
    modalHeight = 300;
    modalX = (screenWidth - modalWidth) / 2;
    modalY = (screenHeight - modalHeight) / 2;
    
    // List view settings
    itemsPerPage = 8;
    
    // Board preview settings
    previewBoardSize = 120;
    previewBoardX = modalX + modalWidth - previewBoardSize - 10;
    previewBoardY = modalY + 10;
}

GameStatesModal::~GameStatesModal() {
    if (font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
}

void GameStatesModal::show() {
    visible = true;
    loadStates();
    selectedIndex = -1;
    scrollOffset = 0;
}

void GameStatesModal::hide() {
    visible = false;
}

void GameStatesModal::loadStates() {
    states.clear();
    const auto& allStates = stateManager->getGameStates();
    for (const auto& state : allStates) {
        states.push_back(state);
    }
}

bool GameStatesModal::handleEvent(const SDL_Event& e) {
    if (!visible) return false;
    
    switch (e.type) {
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                hide();
                return true;
            }
            
            // Handle keyboard navigation with ARM CPU fix
            if (e.key.repeat == 0) {
                handleKeyPress(e.key.keysym.sym);
            }
            return true;
            
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT) {
                handleMouseClick(e.button.x, e.button.y);
                return true;
            }
            break;
            
        case SDL_MOUSEWHEEL:
            if (e.wheel.y > 0) {
                // Scroll up
                scrollOffset = std::max(0, scrollOffset - 1);
            } else if (e.wheel.y < 0) {
                // Scroll down
                scrollOffset = std::min((int)states.size() - itemsPerPage, scrollOffset + 1);
            }
            return true;
    }
    
    return false;
}

void GameStatesModal::render() {
    if (!visible) return;
    
    // Draw modal background
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_Rect modalRect = {modalX, modalY, modalWidth, modalHeight};
    SDL_RenderFillRect(renderer, &modalRect);
    
    // Draw modal border
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &modalRect);
    
    // Draw title
    SDL_Color white = {255, 255, 255, 255};
    drawText("Saved Game States", modalX + 10, modalY + 10, white);
    
    // Render list view and board preview
    renderListView();
    renderBoardPreview();
}

void GameStatesModal::renderListView() {
    int listX = modalX + 10;
    int listY = modalY + 40;
    int listWidth = modalWidth - previewBoardSize - 30;
    int itemHeight = 25;
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color selectedColor = {100, 150, 255, 255};
    SDL_Color normalColor = {200, 200, 200, 255};
    
    // Draw list background
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_Rect listRect = {listX, listY, listWidth, itemsPerPage * itemHeight};
    SDL_RenderFillRect(renderer, &listRect);
    
    // Draw list border
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &listRect);
    
    // Draw items
    for (int i = 0; i < itemsPerPage; i++) {
        int stateIndex = scrollOffset + i;
        if (stateIndex >= states.size()) break;
        
        const GameStateManager::GameState& state = states[stateIndex];
        int itemY = listY + i * itemHeight;
        
        // Draw selection background
        if (stateIndex == selectedIndex) {
            SDL_SetRenderDrawColor(renderer, selectedColor.r, selectedColor.g, selectedColor.b, 100);
            SDL_Rect selectedRect = {listX, itemY, listWidth, itemHeight};
            SDL_RenderFillRect(renderer, &selectedRect);
        }
        
        // Draw item text
        std::string displayText = state.date + ": " + state.title;
        if (displayText.length() > 25) {
            displayText = displayText.substr(0, 25) + "...";
        }
        
        SDL_Color textColor = (stateIndex == selectedIndex) ? white : normalColor;
        drawText(displayText, listX + 5, itemY + 5, textColor);
    }
    
    // Draw scroll indicator if needed
    if (states.size() > itemsPerPage) {
        std::string scrollText = "[" + std::to_string(scrollOffset + 1) + "-" + 
                               std::to_string(std::min(scrollOffset + itemsPerPage, (int)states.size())) + 
                               "/" + std::to_string(states.size()) + "]";
        drawText(scrollText, listX, listY + itemsPerPage * itemHeight + 5, white);
    }
}

void GameStatesModal::renderBoardPreview() {
    if (selectedIndex < 0 || selectedIndex >= states.size()) {
        // Draw placeholder when no state is selected
        SDL_Color gray = {100, 100, 100, 255};
        SDL_SetRenderDrawColor(renderer, gray.r, gray.g, gray.b, 255);
        SDL_Rect previewRect = {previewBoardX, previewBoardY, previewBoardSize, previewBoardSize};
        SDL_RenderFillRect(renderer, &previewRect);
        
        SDL_Color white = {255, 255, 255, 255};
        drawText("Select a state", previewBoardX + 10, previewBoardY + previewBoardSize/2 - 10, white);
        return;
    }
    
    // Render the board from FEN
    renderBoardFromFEN(states[selectedIndex].fen);
}

void GameStatesModal::renderBoardFromFEN(const std::string& fen) {
    // Create a temporary chess game to render the board
    ChessGame tempGame;
    tempGame.initializeBoard(fen);
    
    int squareSize = previewBoardSize / 8;
    
    // Draw board squares
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            int x = previewBoardX + col * squareSize;
            int y = previewBoardY + row * squareSize;
            
            // Alternate square colors
            if ((row + col) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255); // Light square
            } else {
                SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255);  // Dark square
            }
            
            SDL_Rect squareRect = {x, y, squareSize, squareSize};
            SDL_RenderFillRect(renderer, &squareRect);
            
            // Draw piece if present
            ChessPiece piece = tempGame.getPiece(row, col);
            if (piece.type != PieceType::NONE) {
                // Use chess_pieces_sdl to render the piece
                renderChessPiece(renderer, x, y, piece, squareSize);
            }
        }
    }
    
    // Draw board border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect boardRect = {previewBoardX, previewBoardY, previewBoardSize, previewBoardSize};
    SDL_RenderDrawRect(renderer, &boardRect);
}

void GameStatesModal::handleMouseClick(int x, int y) {
    int listX = modalX + 10;
    int listY = modalY + 40;
    int listWidth = modalWidth - previewBoardSize - 30;
    int itemHeight = 25;
    
    // Check if click is within list area
    if (x >= listX && x <= listX + listWidth && 
        y >= listY && y <= listY + itemsPerPage * itemHeight) {
        
        int clickedItem = (y - listY) / itemHeight;
        int stateIndex = scrollOffset + clickedItem;
        
        if (stateIndex < states.size()) {
            selectedIndex = stateIndex;
        }
    }
}

void GameStatesModal::handleKeyPress(SDL_Keycode key) {
    switch (key) {
        case SDLK_UP:
            if (selectedIndex > 0) {
                selectedIndex--;
                // Adjust scroll if needed
                if (selectedIndex < scrollOffset) {
                    scrollOffset = selectedIndex;
                }
            } else if (states.size() > 0 && selectedIndex == -1) {
                // If no selection, select the first item
                selectedIndex = 0;
            }
            break;
            
        case SDLK_DOWN:
            if (selectedIndex < (int)states.size() - 1) {
                selectedIndex++;
                // Adjust scroll if needed
                if (selectedIndex >= scrollOffset + itemsPerPage) {
                    scrollOffset = selectedIndex - itemsPerPage + 1;
                }
            } else if (states.size() > 0 && selectedIndex == -1) {
                // If no selection, select the first item
                selectedIndex = 0;
            }
            break;
            
        case SDLK_RETURN:
            loadSelectedState();
            break;
    }
}

void GameStatesModal::loadSelectedState() {
    if (selectedIndex >= 0 && selectedIndex < states.size()) {
        std::cout << "[GameStatesModal] Loading state: " << states[selectedIndex].fen << std::endl;
        
        // Call the callback if set
        if (onStateSelected) {
            onStateSelected(states[selectedIndex].fen);
        }
        
        hide();
    }
}

void GameStatesModal::drawText(const std::string& text, int x, int y, SDL_Color color) {
    if (!font) return;
    
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_Rect destRect = {x, y, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &destRect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

SDL_Texture* GameStatesModal::createTextTexture(const std::string& text, SDL_Color color) {
    if (!font) return nullptr;
    
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) return nullptr;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}
