#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include "chess_pieces_sdl.h"
#include "chess_game_logic.h"
#include "engine/uci_engine.h"
#include "settings_modal.h"
#include "gameinfo_modal.h"
#include "config_manager.h"

// Undefine conflicting macros from definitions.h
#ifdef SCREEN_WIDTH
#undef SCREEN_WIDTH
#endif
#ifdef SCREEN_HEIGHT
#undef SCREEN_HEIGHT
#endif
#ifdef BOARD_SIZE
#undef BOARD_SIZE
#endif
#ifdef SQUARE_SIZE
#undef SQUARE_SIZE
#endif

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Board dimensions and position
const int BOARD_SIZE = 480;
const int BOARD_X = (SCREEN_WIDTH - BOARD_SIZE) / 2;
const int BOARD_Y = (SCREEN_HEIGHT - BOARD_SIZE) / 2;
const int SQUARE_SIZE = BOARD_SIZE / 8;

// Global variables
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

// Cursor position for keyboard navigation
uint8_t cursorRow = 4;
uint8_t cursorCol = 4;   // Cursor position for keyboard navigation
bool mouseUsed = false;  // Flag for deselect cursor if Mouse is used
UCIEngine engine;

// Settings modal
SettingsModal* settingsModal = nullptr;
GameInfoModal* gameInfoModal = nullptr;
ConfigManager* configManager = nullptr;

// Handle keyboard input for piece selection and movement
void handleKeyboardInput(SDL_Keycode key, ChessGame& chessGame) {
  mouseUsed = false;

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
    case SDLK_RETURN:
    case SDLK_SPACE:
      // Select/deselect piece or make move
      if (!chessGame.isPieceSelected()) {
        // Select piece
        chessGame.selectPiece(cursorRow, cursorCol);
      } else {
        // Try to move piece
        chessGame.movePiece(chessGame.getSelectedRow(), chessGame.getSelectedCol(), cursorRow, cursorCol);
        chessGame.deselectPiece();
      }
      break;
    case SDLK_ESCAPE:
      // Deselect piece
      chessGame.deselectPiece();
      break;
  }
}

// Render the chess board
void renderBoard(ChessGame& chessGame) {
  // Clear screen
  SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
  SDL_RenderClear(renderer);

  // Draw board background
  SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
  SDL_Rect boardRect = {BOARD_X - 10, BOARD_Y - 10, BOARD_SIZE + 20, BOARD_SIZE + 20};
  SDL_RenderFillRect(renderer, &boardRect);

  // Draw chess board squares
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      SDL_Rect squareRect = {BOARD_X + col * SQUARE_SIZE, BOARD_Y + row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
      
      // Alternate square colors
      if ((row + col) % 2 == 0) {
        SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255); // Light squares
      } else {
        SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255);  // Dark squares
      }
      SDL_RenderFillRect(renderer, &squareRect);

      // Highlight selected square
      if (chessGame.isPieceSelected() && chessGame.getSelectedRow() == row && chessGame.getSelectedCol() == col) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 128); // Yellow highlight
        SDL_RenderFillRect(renderer, &squareRect);
      }

      // Highlight cursor position (only if no piece is selected and mouse not used)
      if (!chessGame.isPieceSelected() && !mouseUsed && cursorRow == row && cursorCol == col) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 128); // Green cursor
        SDL_RenderFillRect(renderer, &squareRect);
      }

      // Highlight valid moves
      if (chessGame.isPieceSelected()) {
        auto validMoves = chessGame.getValidMoves(chessGame.getSelectedRow(), chessGame.getSelectedCol());
        for (const auto& move : validMoves) {
          if (move.first == row && move.second == col) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 128); // Blue for valid moves
            SDL_RenderFillRect(renderer, &squareRect);
          }
        }
      }
    }
  }

  // Draw pieces
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      const ChessPiece& piece = chessGame.getPiece(row, col);
      if (!piece.isEmpty()) {
        renderChessPiece(renderer, BOARD_X + col * SQUARE_SIZE, BOARD_Y + row * SQUARE_SIZE, piece, SQUARE_SIZE / 60);
      }
    }
  }

  // Draw board coordinates
  TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);
  if (font) {
    SDL_Color textColor = {255, 255, 255, 255};
    
    // Draw file letters (a-h)
    for (int col = 0; col < 8; col++) {
      std::string fileLabel = std::string(1, 'a' + col);
      SDL_Surface* surface = TTF_RenderText_Blended(font, fileLabel.c_str(), textColor);
      SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
      int texW, texH;
      SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
      SDL_Rect dstRect = {BOARD_X + col * SQUARE_SIZE + SQUARE_SIZE/2 - texW/2, BOARD_Y + BOARD_SIZE + 5, texW, texH};
      SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
      SDL_DestroyTexture(texture);
      SDL_FreeSurface(surface);
    }

    // Draw rank numbers (1-8)
    for (int row = 0; row < 8; row++) {
      std::string rankLabel = std::to_string(8 - row);
      SDL_Surface* surface = TTF_RenderText_Blended(font, rankLabel.c_str(), textColor);
      SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
      int texW, texH;
      SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
      SDL_Rect dstRect = {BOARD_X - 20, BOARD_Y + row * SQUARE_SIZE + SQUARE_SIZE/2 - texH/2, texW, texH};
      SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
      SDL_DestroyTexture(texture);
      SDL_FreeSurface(surface);
    }

    TTF_CloseFont(font);
  }

  // Render modals if they are visible
  if (settingsModal && settingsModal->isVisible()) {
    settingsModal->render();
  }
  
  if (gameInfoModal && gameInfoModal->isVisible()) {
    gameInfoModal->render();
  }

  // Present renderer
  SDL_RenderPresent(renderer);
}

void renderChessboardSDL(std::string fen) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    return;
  }

  // Initialize SDL_image
  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
    SDL_Quit();
    return;
  }

  // Initialize SDL_ttf
  if (TTF_Init() == -1) {
    std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
    IMG_Quit();
    SDL_Quit();
    return;
  }

  // Create window
  window = SDL_CreateWindow("Chess Game - SDL",
                           SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED,
                           SCREEN_WIDTH, SCREEN_HEIGHT,
                           SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return;
  }

  // Create renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return;
  }

  // Initialize chess pieces
  if (!initChessPieceTextures(renderer)) {
    std::cerr << "Failed to initialize chess pieces!" << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return;
  }

  // Create chess game
  ChessGame chessGame;

  // Create config manager
  configManager = new ConfigManager();
  
  // Create settings modal
  settingsModal = new SettingsModal(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, configManager);
  gameInfoModal = new GameInfoModal(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

  // Set settings change callback
  settingsModal->setOnSettingsChanged([&chessGame](const SettingsModal::Settings& settings) {
    std::cout << "[SDLG] Settings updated:" << std::endl;
    std::cout << "[SDLG]   Sound: " << (settings.soundEnabled ? "Enabled" : "Disabled") << std::endl;
    std::cout << "[SDLG]   Depth Difficulty: " << settings.depthDifficulty << std::endl;
    std::cout << "[SDLG]   Max Time Per Move: " << settings.maxTimePerMove << std::endl;
    std::cout << "[SDLG]   Match Time: " << settings.matchTime << std::endl;
  });

  // Main game loop
  bool quit = false;
  SDL_Event e;

  while (!quit) {
    // Handle events
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
      
      // Handle modal events first
      if (settingsModal && settingsModal->isVisible() && settingsModal->handleEvent(e)) {
        continue; // Modal handled the event
      }
      
      if (gameInfoModal && gameInfoModal->isVisible() && gameInfoModal->handleEvent(e)) {
        continue; // Modal handled the event
      }

      switch (e.type) {
        case SDL_KEYDOWN:
          if (e.key.keysym.sym == SDLK_s && !settingsModal->isVisible() && !gameInfoModal->isVisible()) {
            settingsModal->show();
          } else if (e.key.keysym.sym == SDLK_i && !settingsModal->isVisible() && !gameInfoModal->isVisible()) {
            gameInfoModal->show();
          } else if (!settingsModal->isVisible() && !gameInfoModal->isVisible()) {
            handleKeyboardInput(e.key.keysym.sym, chessGame);
          }
          break;

        case SDL_MOUSEBUTTONDOWN:
          if (e.button.button == SDL_BUTTON_LEFT && !settingsModal->isVisible() && !gameInfoModal->isVisible()) {
            mouseUsed = true;
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            
            // Convert mouse coordinates to board coordinates
            if (mouseX >= BOARD_X && mouseX < BOARD_X + BOARD_SIZE &&
                mouseY >= BOARD_Y && mouseY < BOARD_Y + BOARD_SIZE) {
              int col = (mouseX - BOARD_X) / SQUARE_SIZE;
              int row = (mouseY - BOARD_Y) / SQUARE_SIZE;
              
              if (!chessGame.isPieceSelected()) {
                // Select piece
                chessGame.selectPiece(row, col);
              } else {
                // Try to move piece
                chessGame.movePiece(chessGame.getSelectedRow(), chessGame.getSelectedCol(), row, col);
                chessGame.deselectPiece();
              }
            }
          }
          break;
      }
    }

    // Render
    renderBoard(chessGame);

    // Cap frame rate
    SDL_Delay(16); // ~60 FPS
  }

  // Cleanup
  delete settingsModal;
  delete gameInfoModal;
  delete configManager;
  cleanupChessPieceTextures();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}
