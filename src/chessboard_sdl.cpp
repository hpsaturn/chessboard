#include <SDL2/SDL.h>

#include "definitions.h"

#include "modal_settings.h"
#include "modal_info.h"
#include "modal_states.h"
#include "modal_about.h"
#include "modal_help.h"

#include <iostream>
#include <string>
#include <vector>

#include "chess_game_logic.h"
#include "chess_pieces.h"
#include "chess_pieces_sdl.h"
#include "engine/uci_engine.h"
#include "config_manager.h"
#include "game_state_manager.h"
#include "engine/bitboard.h"

// UI state variables
bool pieceSelected = false;
uint8_t selectedRow = -1;
uint8_t selectedCol = -1;
uint8_t lastMoveStartRow = -1;  // Last black move
uint8_t lastMoveStartCol = -1;
uint8_t lastMoveEndRow = -1;
uint8_t lastMoveEndCol = -1;
uint8_t lastCheckRow = -1;     // Last check position
uint8_t lastCheckCol = -1;
uint8_t cursorRow = 6;   // Cursor position for keyboard navigation
uint8_t cursorCol = 4;   // Cursor position for keyboard navigation
bool mouseUsed = false;  // Flag for deselect cursor if Mouse is used
UCIEngine engine;
ChessBoard bitboard;
 
// Settings modal
SettingsModal* settingsModal = nullptr;
GameInfoModal* gameInfoModal = nullptr;
GameStatesModal* gameStatesModal = nullptr;
ConfigManager* configManager = nullptr;
HelpModal* helpModal = nullptr;
AboutModal* aboutModal = nullptr;
GameStateManager* stateManager = nullptr;

std::string pending_fen;
std::string pending_engine_move = "";
bool isEngineProcessing = false;

void resetBoard(ChessGame& chessGame) {
  chessGame.resetGame();
  pieceSelected = false;
  selectedRow = -1;
  selectedCol = -1;
  cursorRow = 6;
  cursorCol = 4;
  lastMoveStartRow = -1;
  lastMoveStartCol = -1;
  lastMoveEndRow = -1;
  lastMoveEndCol = -1;
  lastCheckCol = -1;
  lastCheckRow = -1;
  chessGame.pending_move.clear();
  engine.newGame();
  engine.sendCommand("easy");
  engine.sendCommand("random");
}

void showInfoModel(GameInfoModal* infoModal, ChessGame& chessGame) {
// Show game info modal
  if (gameInfoModal) {
    gameInfoModal->updateCapturedPieces(
      chessGame.getWhiteCapturedPieces(),
      chessGame.getBlackCapturedPieces()
    );

    bool negative = chessGame.getPointsWhite() - chessGame.getPointsBlack() < 0;
    int pointsDiff = abs(chessGame.getPointsWhite() - chessGame.getPointsBlack());
                                        
    gameInfoModal->setPoints((negative ? "-" : "+") + std::to_string(pointsDiff), negative); 
    gameInfoModal->show();
  }
}

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
      if (!(settingsModal->isVisible() || gameInfoModal->isVisible() || gameStatesModal->isVisible() || helpModal->isVisible() || aboutModal->isVisible())) {
        SDL_Quit();
        exit(0);
      }
      break;
    case SDLK_ESCAPE:
      // Deselect piece - only when modal is NOT visible
      if (!(settingsModal->isVisible() || gameInfoModal->isVisible() || gameStatesModal->isVisible() || helpModal->isVisible())) {
        pieceSelected = false;
        selectedRow = -1;
        selectedCol = -1;
      }
      break;
    case SDLK_s:
      // Show settings modal
      if (settingsModal) settingsModal->show();
      break;
    case SDLK_i:
      showInfoModel(gameInfoModal, chessGame); 
      break;
    case SDLK_F2:
      // Save current state slot
      std::cout << "[SDLG] saving state:" << std::endl;
      stateManager->addGameState(chessGame.boardToFEN(),"");
      break;
    case SDLK_F3:
      // Load last state slot
      std::cout << "[SDLG] loading state: " << stateManager->getLastGameState()->fen << std::endl;
      resetBoard(chessGame);
      chessGame.initializeBoard(stateManager->getLastGameState()->fen);
      break;
    case SDLK_F4:
      // Show game states modal
      if (gameStatesModal) gameStatesModal->show();
      break;
    case SDLK_h:
    case SDLK_F1:
      // Show help modal
      if (helpModal) helpModal->show();
      break;
    case SDLK_r:
      // Reset board
      resetBoard(chessGame); 
      break;
    case SDLK_F5:
      // Show about modal
      if (aboutModal) aboutModal->show();
      break;
      break;
  }
}

void handleMouseClick(int mouseX, int mouseY, ChessGame& chessGame) {
  // Check if click is within chessboard bounds
  if (mouseX >= 0 && mouseX < BOARD_SIZE && mouseY >= 0 && mouseY < BOARD_SIZE) {
    mouseUsed = true;
    int row = mouseY / SQUARE_SIZE;
    int col = mouseX / SQUARE_SIZE;

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
      // Move selected piece
      if (chessGame.movePiece(selectedRow, selectedCol, row, col)) {
        // Move successful - clear selection
        pieceSelected = false;
        selectedRow = -1;
        selectedCol = -1;
      } else {
        // Move failed, but allow selecting a different piece
        ChessPiece piece = chessGame.getPiece(row, col);
        if (!piece.isEmpty() && ((chessGame.isWhiteTurn() && piece.color == PieceColor::WHITE) ||
                                 (!chessGame.isWhiteTurn() && piece.color == PieceColor::BLACK))) {
          // Select the new piece
          selectedRow = row;
          selectedCol = col;
        } else {
          // Deselect if clicking on empty square or opponent piece
          pieceSelected = false;
          selectedRow = -1;
          selectedCol = -1;
        }
      }
    }
  }
}

void process_engine_move(ChessGame& chessGame, std::string& engine_move) {
  engine.addMoveToHistory(engine_move);
  int fromRow, fromCol, toRow, toCol;
  chessGame.fromChessMoveNotation(engine_move, fromRow, fromCol, toRow, toCol);
  chessGame.movePiece(fromRow, fromCol, toRow, toCol);
  lastMoveStartRow = fromRow;
  lastMoveStartCol = fromCol;
  lastMoveEndRow = toRow;
  lastMoveEndCol = toCol;
  int checkRow, checkCol;
  if (chessGame.isInCheck(checkRow, checkCol)) {
    std::cout << "[SDLG] Check detected!" << std::endl;
    lastCheckCol = checkCol;  // Notify to user check position
    lastCheckRow = checkRow;
  }
  chessGame.pending_move.clear();
  engine_move.clear();
  isEngineProcessing = false;
  std::cout << "[SDLG] FEN: \"" << chessGame.boardToFEN() << "\"" << std::endl;
}

UCIEngine::MoveCallback  cbOnEngineMove([](const std::string& move) {
    std::cout << "[SDLG] engine move callback received: " << move << std::endl;
    pending_engine_move = move;
});

// Main game loop
void mainLoop(ChessGame& chessGame, SDL_Renderer* renderer) {
  bool quit = false;
  SDL_Event e;

  // Frame rate limiting
  const int FPS = 60;
  const int frameDelay = 1000 / FPS;
  Uint32 frameStart;
  int frameTime;

  while (!quit) {
    frameStart = SDL_GetTicks();

    // Handle events on queue
    while (SDL_PollEvent(&e) != 0) {
      // User requests quit
      if (e.type == SDL_QUIT) {
        quit = true;
      }
      // Handle keyboard input
      else if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        // Let modals handle the event first
        if (!settingsModal->handleEvent(e) && !gameInfoModal->handleEvent(e) && !gameStatesModal->handleEvent(e) && !helpModal->handleEvent(e) && !aboutModal->handleEvent(e)) {
          handleKeyboardInput(e.key.keysym.sym, chessGame);
        }
      }
      // Handle mouse click
      else if (e.type == SDL_MOUSEBUTTONDOWN) {
        // Let modals handle the event first
        if (!settingsModal->handleEvent(e) && !gameInfoModal->handleEvent(e) && !aboutModal->handleEvent(e) && !helpModal->handleEvent(e) && !gameInfoModal->handleEvent(e)) {
          int mouseX, mouseY;
          SDL_GetMouseState(&mouseX, &mouseY);
          handleMouseClick(mouseX, mouseY, chessGame);
        }
      }
      // Handle other events for modals
      else {
        settingsModal->handleEvent(e);
        gameInfoModal->handleEvent(e);
        gameStatesModal->handleEvent(e);
        aboutModal->handleEvent(e);
      }
    }

    // New FEN to load
    if (!pending_fen.empty()) {
      resetBoard(chessGame);
      chessGame.initializeBoard(pending_fen);
      pending_fen.clear();
    }

    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black background
    SDL_RenderClear(renderer);

    // ===== CHESSBOARD =====
    // Draw chessboard squares
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        SDL_Rect square = {col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};

        // Highlight selected square
        if (pieceSelected && row == selectedRow && col == selectedCol) {
          SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Green for selected
          lastCheckCol = -1; // Clear last check highlight when selecting a piece
          lastCheckRow = -1;
        }
        // Highlight cursor position
        else if (row == cursorRow && col == cursorCol && !mouseUsed && !chessGame.pending_move.empty()) { 
          SDL_SetRenderDrawColor(renderer, 172, 83, 83, 255);  // Red light for cursor
        } else if (row == cursorRow && col == cursorCol && !mouseUsed && chessGame.pending_move.empty()) {
          SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  // Yellow for cursor 
        } else if (row == lastMoveStartRow && col == lastMoveStartCol) {
          SDL_SetRenderDrawColor(renderer, 153, 153, 153, 255);  // Last opponent move Start
        } else if (row == lastMoveEndRow && col == lastMoveEndCol) {
          SDL_SetRenderDrawColor(renderer, 102, 102, 102, 255);  // Last opponent move End
        } else if (row == lastCheckRow && col == lastCheckCol) {
          SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);  // Red for check
        }
        // Normal square colors
        else if ((row + col) % 2 == 0) {
          SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255);  // Light squares
        } else {
          SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255);  // Dark squares
        }

        SDL_RenderFillRect(renderer, &square);
      }
    }

    // Draw chess pieces
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        ChessPiece piece = chessGame.getPiece(row, col);
        if (!piece.isEmpty()) {
          renderChessPiece(renderer, col * SQUARE_SIZE, row * SQUARE_SIZE, piece);
        }
      }
    }

    // Render modal windows
    settingsModal->render();
    gameInfoModal->render();
    gameStatesModal->render();
    helpModal->render();
    aboutModal->render();

    // Update screen
    SDL_RenderPresent(renderer);

    // Send move to engine and update its move
    if (!chessGame.pending_move.empty() && !isEngineProcessing) {  
      std::cout << "[SDLG] sending move  : " << chessGame.pending_move << std::endl;
      isEngineProcessing = true; 
      if (chessGame.isFenMode()) {
        // engine_move = engine.sendMove(chessGame.boardToFEN());
        engine.sendMoveAsync(chessGame.boardToFEN(),cbOnEngineMove);
      }
        
      else {
        // engine_move = engine.sendMove(chessGame.pending_move);
        engine.sendMoveAsync(chessGame.pending_move,cbOnEngineMove); 
      }
    }

    if (!pending_engine_move.empty()) {
      process_engine_move(chessGame, pending_engine_move);
    }

    // Frame rate limiting
    frameTime = SDL_GetTicks() - frameStart;
    if (frameDelay > frameTime) {
      SDL_Delay(frameDelay - frameTime);
    }
  }
}

void renderChessboardSDL(std::string fen) {
  // Create ChessGame instance inside main to avoid global initialization issues
  ChessGame chessGame;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "[SDLG] SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    return;
  }

  // Create window
  SDL_Window* window =
      SDL_CreateWindow("Chess Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                       SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "[SDLG] Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return;
  }

  // Create renderer
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "[SDLG] Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
  }

  // Initialize chess piece textures
  if (!initChessPieceTextures(renderer)) {
    std::cerr << "[SDLG] Failed to initialize chess piece textures!" << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
  }
  gameStatesModal = new GameStatesModal(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, stateManager);

  configManager = new ConfigManager();
  stateManager = new GameStateManager();

  stateManager->loadGameStates();

  // Create settings modal
  settingsModal = new SettingsModal(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, configManager);
  gameInfoModal = new GameInfoModal(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
  gameStatesModal = new GameStatesModal(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, stateManager);
  helpModal = new HelpModal(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
  aboutModal = new AboutModal(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

  // Set settings change callback
  settingsModal->setOnSettingsChanged([&chessGame](const SettingsModal::Settings& settings) {
    std::cout << "[SDLG] Settings updated:" << std::endl;
    std::cout << "[SDLG]   Sound: " << (settings.soundEnabled ? "Enabled" : "Disabled") << std::endl;
    std::cout << "[SDLG]   Depth Difficulty: " << settings.depthDifficulty << std::endl;
    std::cout << "[SDLG]   Max Time Per Move: " << settings.maxTimePerMove << std::endl;
    std::cout << "[SDLG]   Match Time: " << settings.matchTime << std::endl;

    engine.setDifficult(settingsModal->getSettings().depthDifficulty); 
    engine.setMoveTime(settingsModal->getSettings().maxTimePerMove);
  });

  // Initialize engine
  if (engine.startEngine(true)) {
    // Initialize UCI protocol
    engine.sendCommand("uci");
    if (engine.waitForResponse("uciok")) {
      std::cout << "[SDLG] Engine is UCI compatible!" << std::endl;
    }
    engine.sendCommand("isready");
    if (engine.waitForResponse("readyok")) {
      std::cout << "[SDLG] Engine is ready!" << std::endl;
      engine.setDifficult(settingsModal->getSettings().depthDifficulty);
      engine.setMoveTime(settingsModal->getSettings().maxTimePerMove);
    }
    chessGame.initializeBoard(fen);
    resetBoard(chessGame);
  }

  // Set game state selection callback
  gameStatesModal->setOnStateSelected([&chessGame](const std::string& sfen) {
    std::cout << "[SDLG] Loading game state from FEN: " << sfen << std::endl;
    pending_fen = sfen;
  });

  gameInfoModal->setBlackTimer("05:00"); // testing
  gameInfoModal->setWhiteTimer("05:00");

  mainLoop(chessGame, renderer);

  // Cleanup
  delete settingsModal;
  delete gameStatesModal;
  delete gameInfoModal;
  delete helpModal;
  delete aboutModal;

  cleanupChessPieceTextures();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
