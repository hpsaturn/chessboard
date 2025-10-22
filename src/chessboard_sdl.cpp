// #ifdef HAS_SDL2
#include <SDL2/SDL.h>
#include <iostream>
#include <string>

#define BOARD_SIZE 320

void renderChessboardSDL() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "SDL Chessboard",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        BOARD_SIZE,
        BOARD_SIZE,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
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
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);

        // Draw chessboard
        int squareSize = BOARD_SIZE/8;
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                SDL_Rect square = {col * squareSize, row * squareSize, squareSize, squareSize};
                
                // Alternate colors
                if ((row + col) % 2 == 0) {
                    SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255); // Light squares
                } else {
                    SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255);  // Dark squares
                }
                
                SDL_RenderFillRect(renderer, &square);
            }
        }

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
// #else
// void renderChessboardSDL() {
    // std::cout << "SDL2 support not available in this build.\n";
    // std::cout << "Please install SDL2 development libraries and rebuild.\n";
// }
// #endif
