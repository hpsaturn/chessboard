// Main SDL Chessboard Logic
#ifdef HAS_SDL2
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include "chess_pieces.h"
#include "chess_pieces_sdl.h"

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 320
#define BOARD_SIZE 220
#define SQUARE_SIZE (BOARD_SIZE/8)
#define HISTORY_WIDTH 100
#define INPUT_HEIGHT 100

// Sample move history for demonstration
std::vector<std::string> moveHistory = {
    "1. e4 e5",
    "2. Nf3 Nc6", 
    "3. Bb5 a6",
    "4. Ba4 Nf6",
    "5. O-O Be7",
    "6. Re1 b5",
    "7. Bb3 d6",
    "8. c3 O-O",
    "9. h3 Nb8",
    "10. d4 Nbd7"
};

void renderChessboardSDL() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "SDL Chessboard with UI",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // Main loop flag
    bool quit = false;
    SDL_Event e;

    // Main loop
    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN) {
                // Check if the pressed key was the 'Q' key
                if (e.key.keysym.sym == SDLK_q || e.key.keysym.sym == SDLK_ESCAPE) {
                    std::cout << "Quitting..." << std::endl;
                    quit = true;
                }
            }
        }

        // Clear screen with dark background
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255); // Dark gray background
        SDL_RenderClear(renderer);

        // ===== CHESSBOARD AREA (220x220 - top-left) =====
        SDL_Rect boardArea = {0, 0, BOARD_SIZE, BOARD_SIZE};
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Dark border
        SDL_RenderFillRect(renderer, &boardArea);

        // Draw chessboard squares
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                // Set square color
                if ((row + col) % 2 == 0) {
                    SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255); // Light squares
                } else {
                    SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255); // Dark squares
                }
                
                // Draw square
                SDL_Rect square = {col * SQUARE_SIZE + 2, row * SQUARE_SIZE + 2, SQUARE_SIZE - 4, SQUARE_SIZE - 4};
                SDL_RenderFillRect(renderer, &square);

                // Draw piece if present
                const ChessPiece& piece = STANDARD_BOARD[row][col];
                if (!piece.isEmpty()) {
                    renderChessPiece(renderer, col * SQUARE_SIZE + 2, row * SQUARE_SIZE + 2, piece);
                }
            }
        }

        // ===== HISTORY PANEL (100x320 - right side) =====
        SDL_Rect historyArea = {BOARD_SIZE, 0, HISTORY_WIDTH, SCREEN_HEIGHT};
        SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255); // Dark blue background
        SDL_RenderFillRect(renderer, &historyArea);

        // Draw history border
        SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255); // Light blue border
        SDL_Rect historyBorder = {BOARD_SIZE, 0, 2, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &historyBorder);

        // Draw history title
        renderText(renderer, "MOVE HISTORY", BOARD_SIZE + 10, 15, 1);

        // Draw move history
        int historyY = 40;
        for (size_t i = 0; i < moveHistory.size() && historyY < SCREEN_HEIGHT - 20; i++) {
            renderText(renderer, moveHistory[i], BOARD_SIZE + 10, historyY, 1);
            historyY += 20;
        }

        // ===== INPUT ZONE (220x100 - bottom-left) =====
        SDL_Rect inputArea = {0, BOARD_SIZE, BOARD_SIZE, INPUT_HEIGHT};
        SDL_SetRenderDrawColor(renderer, 50, 30, 30, 255); // Dark red background
        SDL_RenderFillRect(renderer, &inputArea);

        // Draw input border
        SDL_SetRenderDrawColor(renderer, 150, 100, 100, 255); // Light red border
        SDL_Rect inputBorder = {0, BOARD_SIZE, BOARD_SIZE, 2};
        SDL_RenderFillRect(renderer, &inputBorder);

        // Draw input prompt
        renderText(renderer, "Enter move:", 10, BOARD_SIZE + 15, 1);
        
        // Draw input field (simulated)
        SDL_Rect inputField = {10, BOARD_SIZE + 35, BOARD_SIZE - 20, 30};
        SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255); // Input field background
        SDL_RenderFillRect(renderer, &inputField);
        
        // Draw cursor
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White cursor
        SDL_Rect cursor = {15, BOARD_SIZE + 40, 2, 20};
        SDL_RenderFillRect(renderer, &cursor);

        // Draw placeholder text
        renderText(renderer, "e2-e4", 20, BOARD_SIZE + 45, 1);

        // Update screen
        SDL_RenderPresent(renderer);

        // Small delay to prevent high CPU usage
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

#else
// Stub implementation when SDL2 is not available
#include <iostream>
void renderChessboardSDL() {
    std::cout << "SDL2 support not available in this build.\n";
    std::cout << "Please install SDL2 development libraries and rebuild.\n";
}
#endif
