// Main SDL Chessboard Logic
#define HAVE_SDL2
#ifdef HAVE_SDL2
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include "chess_pieces.h"
#include "chess_pieces_sdl.h"
#include "engine/uci_engine.h"

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 320
#define BOARD_SIZE 260
#define SQUARE_SIZE (BOARD_SIZE/8)
#define HISTORY_WIDTH 60
#define INPUT_HEIGHT 60

// Game state variables
ChessPiece board[8][8];
std::vector<std::string> moveHistory;
bool pieceSelected = false;
int selectedRow = -1;
int selectedCol = -1;
int cursorRow = 0;    // Cursor position for keyboard navigation
int cursorCol = 0;    // Cursor position for keyboard navigation
bool whiteTurn = true;
UCIEngine engine;
std::string pending_move = "";

// Convert board coordinates to chess notation
std::string toChessNotation(int row, int col) {
    std::string notation;
    notation += 'a' + col;
    notation += '8' - row;
    return notation;
}

// Convert chess notation to board coordinates
bool fromChessNotation(const std::string& notation, int& row, int& col) {
    if (notation.length() != 2) return false;
    
    col = notation[0] - 'a';
    row = '8' - notation[1];
    
    return (col >= 0 && col < 8 && row >= 0 && row < 8);
}

// Initialize the chessboard with standard starting position
void initializeBoard() {
    // Copy standard board
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            board[row][col] = STANDARD_BOARD[row][col];
        }
    }
}

// Check if a move is valid (basic validation - can be enhanced later)
bool isValidMove(int fromRow, int fromCol, int toRow, int toCol) {
    // Can't move to the same square
    if (fromRow == toRow && fromCol == toCol) return false;
    
    ChessPiece fromPiece = board[fromRow][fromCol];
    ChessPiece toPiece = board[toRow][toCol];
    
    // Can't move empty squares
    if (fromPiece.isEmpty()) return false;
    
    // Can't capture own pieces
    if (!toPiece.isEmpty() && fromPiece.color == toPiece.color) return false;
    
    // Basic pawn movement (can be enhanced with proper chess rules)
    if (fromPiece.type == PieceType::PAWN) {
        int direction = (fromPiece.color == PieceColor::WHITE) ? -1 : 1;
        
        // Forward move
        if (fromCol == toCol && toRow == fromRow + direction && toPiece.isEmpty()) {
            return true;
        }
        
        // Initial double move
        if (fromCol == toCol && 
            ((fromPiece.color == PieceColor::WHITE && fromRow == 6 && toRow == 4) ||
             (fromPiece.color == PieceColor::BLACK && fromRow == 1 && toRow == 3)) &&
            toPiece.isEmpty() && board[fromRow + direction][fromCol].isEmpty()) {
            return true;
        }
        
        // Capture
        if (abs(fromCol - toCol) == 1 && toRow == fromRow + direction && !toPiece.isEmpty()) {
            return true;
        }
    }
    
    // For other pieces, allow any move (basic implementation)
    return true;
}

// Move a piece and record the move
bool movePiece(int fromRow, int fromCol, int toRow, int toCol) {
    if (!isValidMove(fromRow, fromCol, toRow, toCol)) {
        return false;
    }
    
    // Record the move in chess notation
    std::string move = toChessNotation(fromRow, fromCol) + toChessNotation(toRow, toCol);
    
    // Perform the move
    board[toRow][toCol] = board[fromRow][fromCol];
    board[fromRow][fromCol] = ChessPiece(); // Empty square
   
    moveHistory.push_back(move);
    if (whiteTurn) {
      pending_move = move;
    }
    // Switch turns
    whiteTurn = !whiteTurn;
    
    return true;
}

// Handle keyboard input for piece selection and movement
void handleKeyboardInput(SDL_Keycode key) {
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
                ChessPiece piece = board[cursorRow][cursorCol];
                if (!piece.isEmpty() && ((whiteTurn && piece.color == PieceColor::WHITE) || 
                                         (!whiteTurn && piece.color == PieceColor::BLACK))) {
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
                if (movePiece(selectedRow, selectedCol, cursorRow, cursorCol)) {
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
            initializeBoard();
            moveHistory.clear();
            pieceSelected = false;
            selectedRow = -1;
            selectedCol = -1;
            cursorRow = 0;
            cursorCol = 0;
            whiteTurn = true;
            break;
    }
}

void handleMouseClick(int mouseX, int mouseY) {
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
        ChessPiece piece = board[row][col];
        if (!piece.isEmpty() && ((whiteTurn && piece.color == PieceColor::WHITE) || 
                                 (!whiteTurn && piece.color == PieceColor::BLACK))) {
            selectedRow = row;
            selectedCol = col;
            pieceSelected = true;
        }
    } else {
        // Move the selected piece
        if (movePiece(selectedRow, selectedCol, row, col)) {
            // Move successful
            pieceSelected = false;
            selectedRow = -1;
            selectedCol = -1;
        } else {
            // Invalid move - try to select a different piece
            ChessPiece piece = board[row][col];
            if (!piece.isEmpty() && ((whiteTurn && piece.color == PieceColor::WHITE) || 
                                     (!whiteTurn && piece.color == PieceColor::BLACK))) {
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
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "SDL Chessboard with Interactive Pieces",
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

    // Initialize chess pieces
    if (!initChessPieceTextures(renderer)) {
        std::cerr << "Failed to initialize chess pieces!" << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // Initialize the board
    initializeBoard();
    moveHistory.clear();

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

      engine.newGame();
    }

    bool quit = false;
    SDL_Event e;

    // Frame rate limiting
    const int FPS = 30;
    const int frameDelay = 1000 / FPS;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    handleMouseClick(e.button.x, e.button.y);
                }
            } else if (e.type == SDL_KEYDOWN) {
                handleKeyboardInput(e.key.keysym.sym);
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
                if (!board[row][col].isEmpty()) {
                    renderChessPiece(renderer, col * SQUARE_SIZE, row * SQUARE_SIZE, board[row][col]);
                }
            }
        }

        // ===== HISTORY PANEL (right side) =====
        SDL_Rect historyArea = {BOARD_SIZE, 0, HISTORY_WIDTH, SCREEN_HEIGHT};
        SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255); // Dark blue background
        SDL_RenderFillRect(renderer, &historyArea);

        // Draw history title
        renderText(renderer, "HISTORY", BOARD_SIZE + 10, 15, 0);

        // Draw move history
        int historyY = 40;
        for (size_t i = 0; i < moveHistory.size() && historyY < SCREEN_HEIGHT - 20; i++) {
            renderText(renderer, moveHistory[i], BOARD_SIZE + 10, historyY, 1);
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
        std::string turnText = "Turn: " + std::string(whiteTurn ? "WHITE" : "BLACK");
        renderText(renderer, turnText.c_str(), 10, BOARD_SIZE + 35, 1);
        
        // Display selection status
        if (pieceSelected) {
            std::string selectedText = "Selected: " + toChessNotation(selectedRow, selectedCol);
            renderText(renderer, selectedText.c_str(), 10, BOARD_SIZE + 55, 1);
        } else {
            renderText(renderer, "Click to select piece", 10, BOARD_SIZE + 55, 1);
        }

        // Display cursor position
        std::string cursorText = "Cursor: " + toChessNotation(cursorRow, cursorCol);
        renderText(renderer, cursorText.c_str(), 10, BOARD_SIZE + 75, 1);
        // Update screen
        SDL_RenderPresent(renderer);
        
        // Frame rate limiting to reduce CPU usage
        SDL_Delay(frameDelay);

        if (!pending_move.empty()) {
          pending_move.clear();
          std::string engine_move = engine.sendMove(pending_move);
          std::cout << "Engine responded: " << engine_move << std::endl;
        }
    }

    // Cleanup
    cleanupChessPieceTextures();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

#endif // HAVE_SDL2
