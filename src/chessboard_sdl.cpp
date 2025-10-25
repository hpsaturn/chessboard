// Main SDL Chessboard Logic
#ifdef HAVE_SDL2
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include "chess_pieces.h"
#include "chess_pieces_sdl.h"

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 320
#define BOARD_SIZE 260
#define SQUARE_SIZE (BOARD_SIZE/8)
#define HISTORY_WIDTH 60
#define INPUT_HEIGHT 100

// Sample move history for demonstration
std::vector<std::string> moveHistory = {
    "e4 e5",
    "Nf3 Nc6", 
    "Bb5 a6",
    "Ba4 Nf6",
    "O-O Be7",
    "Re1 b5",
    "Bb3 d6",
    "c3 O-O",
    "h3 Nb8",
    "d4 Nbd7"
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

    // Initialize chess piece textures
    if (!initChessPieceTextures(renderer)) {
        std::cerr << "Failed to initialize chess piece textures!" << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // Sample chessboard state
    ChessPiece board[8][8] = {
        // Rank 8 (black pieces)
        {ChessPiece(PieceType::ROOK, PieceColor::BLACK), ChessPiece(PieceType::KNIGHT, PieceColor::BLACK), 
         ChessPiece(PieceType::BISHOP, PieceColor::BLACK), ChessPiece(PieceType::QUEEN, PieceColor::BLACK),
         ChessPiece(PieceType::KING, PieceColor::BLACK), ChessPiece(PieceType::BISHOP, PieceColor::BLACK),
         ChessPiece(PieceType::KNIGHT, PieceColor::BLACK), ChessPiece(PieceType::ROOK, PieceColor::BLACK)},
        // Rank 7 (black pawns)
        {ChessPiece(PieceType::PAWN, PieceColor::BLACK), ChessPiece(PieceType::PAWN, PieceColor::BLACK),
         ChessPiece(PieceType::PAWN, PieceColor::BLACK), ChessPiece(PieceType::PAWN, PieceColor::BLACK),
         ChessPiece(PieceType::PAWN, PieceColor::BLACK), ChessPiece(PieceType::PAWN, PieceColor::BLACK),
         ChessPiece(PieceType::PAWN, PieceColor::BLACK), ChessPiece(PieceType::PAWN, PieceColor::BLACK)},
        // Ranks 6-3 (empty)
        {ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece()},
        {ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece()},
        {ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece()},
        {ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece(), ChessPiece()},
        // Rank 2 (white pawns)
        {ChessPiece(PieceType::PAWN, PieceColor::WHITE), ChessPiece(PieceType::PAWN, PieceColor::WHITE),
         ChessPiece(PieceType::PAWN, PieceColor::WHITE), ChessPiece(PieceType::PAWN, PieceColor::WHITE),
         ChessPiece(PieceType::PAWN, PieceColor::WHITE), ChessPiece(PieceType::PAWN, PieceColor::WHITE),
         ChessPiece(PieceType::PAWN, PieceColor::WHITE), ChessPiece(PieceType::PAWN, PieceColor::WHITE)},
        // Rank 1 (white pieces)
        {ChessPiece(PieceType::ROOK, PieceColor::WHITE), ChessPiece(PieceType::KNIGHT, PieceColor::WHITE), 
         ChessPiece(PieceType::BISHOP, PieceColor::WHITE), ChessPiece(PieceType::QUEEN, PieceColor::WHITE),
         ChessPiece(PieceType::KING, PieceColor::WHITE), ChessPiece(PieceType::BISHOP, PieceColor::WHITE),
         ChessPiece(PieceType::KNIGHT, PieceColor::WHITE), ChessPiece(PieceType::ROOK, PieceColor::WHITE)}
    };

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255); // Dark gray background
        SDL_RenderClear(renderer);

        // ===== CHESSBOARD (220x220 - top-left) =====
        // Draw chessboard squares
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                SDL_Rect square = {col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
                
                // Alternate square colors
                if ((row + col) % 2 == 0) {
                    SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255); // Light squares
                } else {
                    SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255); // Dark squares
                }
                
                SDL_RenderFillRect(renderer, &square);
                
                // Draw chess piece
                ChessPiece piece = board[row][col];
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
        renderText(renderer, "HISTORY", BOARD_SIZE + 10, 15, 0);

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

        // Draw input title
        renderText(renderer, "INPUT ZONE", 10, BOARD_SIZE + 15, 1);
        renderText(renderer, "Click to select pieces", 10, BOARD_SIZE + 35, 1);

        // Update screen
        SDL_RenderPresent(renderer);

        // Cap the frame rate
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    cleanupChessPieceTextures();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

#else
#include <iostream>
void renderChessboardSDL() {
    std::cout << "SDL2 support not compiled in this build" << std::endl;
}
#endif
