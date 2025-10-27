// Refactored SDL Chessboard - Separated Game Logic from SDL Rendering
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include "chess_pieces.h"
#include "chess_pieces_sdl.h"
#include "chess_game_logic.h"
#include "engine/uci_engine.h"

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 320
#define BOARD_SIZE 260
#define SQUARE_SIZE (BOARD_SIZE/8)
#define HISTORY_WIDTH 60
#define INPUT_HEIGHT 60

// UI state variables
bool pieceSelected = false;
uint8_t selectedRow = -1;
uint8_t selectedCol = -1;
uint8_t cursorRow = 0;    // Cursor position for keyboard navigation
uint8_t cursorCol = 0;    // Cursor position for keyboard navigation
UCIEngine engine(true);
std::string pending_move = "";

// Handle keyboard input for piece selection and movement
void handleKeyboardInput(SDL_Keycode key, ChessGame& chessGame) {
    switch (key) {
        case SDLK_UP:
            if (cursorRow > 0) cursorRow--;
            break;
        case SDLK_DOWN:
            if (cursorRow < 7) cursorRow++;
            break;
        case SDLK_LEFT:
            if (cursorCol > 0) cursorCol--;
            break;
        case SDLK_RIGHT:
            if (cursorCol < 7) cursorCol++;
            break;
        case SDLK_SPACE:
            // Select/deselect piece at cursor position
            if (!pieceSelected) {
                // Select a piece
                ChessPiece piece = chessGame.getPiece(cursorRow, cursorCol);
                if (!piece.isEmpty() && ((chessGame.isWhiteTurn() && piece.color == PieceColor::WHITE) || 
                                         (!chessGame.isWhiteTurn() && piece.color == PieceColor::BLACK))) {
                    selectedRow = cursorRow;
                    selectedCol = cursorCol;
                    pieceSelected = true;
                }
            } else {
                // Deselect piece
                pieceSelected = false;
                selectedRow = -1;
                selectedCol = -1;
            }
            break;
        case SDLK_RETURN:
            // Move selected piece to cursor position
            if (pieceSelected) {
                if (chessGame.movePiece(selectedRow, selectedCol, cursorRow, cursorCol)) {
                    // Move successful
                    pieceSelected = false;
                    selectedRow = -1;
                    selectedCol = -1;
                }
            }
            break;
        case SDLK_q:
            // Quit the game
            SDL_Quit();
            exit(0);
            break;
        case SDLK_ESCAPE:
            // Deselect piece
            pieceSelected = false;
            selectedRow = -1;
            selectedCol = -1;
            break;
        case SDLK_r:
            // Reset board
            chessGame.resetGame();
            pieceSelected = false;
            selectedRow = -1;
            selectedCol = -1;
            cursorRow = 0;
            cursorCol = 0;
            pending_move.clear();
            engine.newGame();
            break;
    }
}

void handleMouseClick(int mouseX, int mouseY, ChessGame& chessGame) {
    // Check if click is within chessboard
    if (mouseX < 0 || mouseX >= BOARD_SIZE || mouseY < 0 || mouseY >= BOARD_SIZE) {
        return;
    }
    
    int col = mouseX / SQUARE_SIZE;
    int row = mouseY / SQUARE_SIZE;
    
    // Update cursor position to match mouse click
    cursorRow = row;
    cursorCol = col;
    
    if (!pieceSelected) {
        // Select a piece
        ChessPiece piece = chessGame.getPiece(row, col);
        if (!piece.isEmpty() && ((chessGame.isWhiteTurn() && piece.color == PieceColor::WHITE) || 
                                 (!chessGame.isWhiteTurn() && piece.color == PieceColor::BLACK))) {
            selectedRow = row;
            selectedCol = col;
            pieceSelected = true;
        }
    } else {
        // Move the selected piece
        if (chessGame.movePiece(selectedRow, selectedCol, row, col)) {
            // Move successful
            pieceSelected = false;
            selectedRow = -1;
            selectedCol = -1;
        } else {
            // Invalid move - try to select a different piece
            ChessPiece piece = chessGame.getPiece(row, col);
            if (!piece.isEmpty() && ((chessGame.isWhiteTurn() && piece.color == PieceColor::WHITE) || 
                                     (!chessGame.isWhiteTurn() && piece.color == PieceColor::BLACK))) {
                selectedRow = row;
                selectedCol = col;
            } else {
                // Deselect if clicking empty square or opponent piece
                pieceSelected = false;
                selectedRow = -1;
                selectedCol = -1;
            }
        }
    }
}

void renderChessboardSDL() {
    // Create ChessGame instance inside main to avoid global initialization issues
    ChessGame chessGame;
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }
    
    // Create window
    SDL_Window* window = SDL_CreateWindow("Chessboard with Pieces",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
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
    
    // Initialize chess pieces
    if (!initChessPieceTextures(renderer)) {
        std::cerr << "Failed to initialize chess pieces!" << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // Initialize engine
    if (engine.startEngine()) {
      // Initialize UCI protocol
      engine.sendCommand("uci");
      if (engine.waitForResponse("uciok")) {
        std::cout << "Engine is UCI compatible!" << std::endl;
      }

      engine.sendCommand("isready");
      if (engine.waitForResponse("readyok")) {
        std::cout << "Engine is ready!" << std::endl;
      }
    }

    // Main loop flag
    bool quit = false;
    
    // Event handler
    SDL_Event e;
    
    // Frame rate limiting
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;
    
    // Main game loop
    while (!quit) {
        frameStart = SDL_GetTicks();
        
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            // Handle keyboard input
            else if (e.type == SDL_KEYDOWN) {
                handleKeyboardInput(e.key.keysym.sym, chessGame);
            }
            // Handle mouse click
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                handleMouseClick(mouseX, mouseY, chessGame);
            }
        }
        
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);
        
        // ===== CHESSBOARD =====
        // Draw chessboard squares
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                SDL_Rect square = {col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
                
                // Highlight selected square
                if (pieceSelected && row == selectedRow && col == selectedCol) {
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for selected
                }
                // Highlight cursor position
                else if (row == cursorRow && col == cursorCol) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow for cursor
                }
                // Normal square colors
                else if ((row + col) % 2 == 0) {
                    SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255); // Light squares
                } else {
                    SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255); // Dark squares
                }
                
                SDL_RenderFillRect(renderer, &square);
                
                // Draw piece if present
                ChessPiece piece = chessGame.getPiece(row, col);
                if (!piece.isEmpty()) {
                    renderChessPiece(renderer, col * SQUARE_SIZE, row * SQUARE_SIZE, piece);
                }
            }
        }

        // ===== HISTORY PANEL (right side) =====
        SDL_Rect historyArea = {BOARD_SIZE, 0, HISTORY_WIDTH, SCREEN_HEIGHT};
        SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255); // Dark blue background
        SDL_RenderFillRect(renderer, &historyArea);

        // Draw history title
        renderText(renderer, "HISTORY", BOARD_SIZE + 2, 15, 0);

        // Draw move history
        const std::vector<std::string>& moveHistory = chessGame.getMoveHistory();
        int historyY = 40;
        for (size_t i = 0; i < moveHistory.size() && historyY < SCREEN_HEIGHT - 20; i++) {
            renderText(renderer, moveHistory[i], BOARD_SIZE + 2, historyY, 2);
            historyY += 20;
        }

        // ===== INPUT PANEL (bottom) =====
        SDL_Rect inputArea = {0, BOARD_SIZE, BOARD_SIZE, INPUT_HEIGHT};
        SDL_SetRenderDrawColor(renderer, 50, 30, 30, 255); // Dark red background
        SDL_RenderFillRect(renderer, &inputArea);

        // Draw input border
        SDL_SetRenderDrawColor(renderer, 150, 100, 100, 255); // Light red border
        SDL_Rect inputBorder = {0, BOARD_SIZE, BOARD_SIZE, 2};
        SDL_RenderFillRect(renderer, &inputBorder);

        // Draw input title
        renderText(renderer, "INPUT ZONE", 10, BOARD_SIZE + 15, 1);
        
        // Display current turn
        std::string turnText = "Turn: " + std::string(chessGame.isWhiteTurn() ? "WHITE" : "BLACK");
        renderText(renderer, turnText.c_str(), 10, BOARD_SIZE + 35, 1);
        
        // Display selection status
        if (pieceSelected) {
            std::string selectedText = "Selected: " + chessGame.toChessNotation(selectedRow, selectedCol);
            renderText(renderer, selectedText.c_str(), 10, BOARD_SIZE + 55, 1);
        } else {
            renderText(renderer, "Click to select piece", 10, BOARD_SIZE + 55, 1);
        }

        // Display cursor position
        std::string cursorText = "Cursor: " + chessGame.toChessNotation(cursorRow, cursorCol);
        renderText(renderer, cursorText.c_str(), 10, BOARD_SIZE + 75, 1);
        
        // Update screen
        SDL_RenderPresent(renderer);
        
        // Frame rate limiting to reduce CPU usage
        SDL_Delay(frameDelay);

        // Handle engine moves
        if (!pending_move.empty()) {
          std::string engine_move = engine.sendMove(pending_move);
          std::cout << "Engine responded: " << engine_move << std::endl;
          engine.addMoveToHistory(engine_move);
          int fromRow, fromCol, toRow, toCol;
          chessGame.fromChessMoveNotation(engine_move, fromRow, fromCol, toRow, toCol);
          chessGame.movePiece(fromRow, fromCol, toRow, toCol);
          pending_move.clear();
          engine_move.clear();
        }
    }

    // Cleanup
    cleanupChessPieceTextures();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
