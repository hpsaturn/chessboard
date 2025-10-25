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
#define INPUT_HEIGHT 60

// Game state variables
ChessPiece board[8][8];
std::vector<std::string> moveHistory;
bool pieceSelected = false;
int selectedRow = -1;
int selectedCol = -1;
bool whiteTurn = true;

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
    std::string fromNotation = toChessNotation(fromRow, fromCol);
    std::string toNotation = toChessNotation(toRow, toCol);
    std::string move = fromNotation + toNotation;
    
    // Add to history
    moveHistory.push_back(move);
    
    // Perform the move
    board[toRow][toCol] = board[fromRow][fromCol];
    board[fromRow][fromCol] = ChessPiece();
    
    // Switch turns
    whiteTurn = !whiteTurn;
    
    return true;
}

// Handle mouse click for piece selection and movement
void handleMouseClick(int mouseX, int mouseY) {
    // Check if click is within chessboard
    if (mouseX < 0 || mouseX >= BOARD_SIZE || mouseY < 0 || mouseY >= BOARD_SIZE) {
        return;
    }
    
    int col = mouseX / SQUARE_SIZE;
    int row = mouseY / SQUARE_SIZE;
    
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

    // Initialize chess piece textures
    if (!initChessPieceTextures(renderer)) {
        std::cerr << "Failed to initialize chess piece textures!" << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // Initialize the board
    initializeBoard();
    moveHistory.clear();

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    handleMouseClick(e.button.x, e.button.y);
                }
            } else if (e.type == SDL_KEYDOWN) {
                // Handle keyboard input for piece selection/movement
                switch (e.key.keysym.sym) {
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
                        whiteTurn = true;
                        break;
                }
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
                    SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255); // Green highlight
                }
                // Alternate square colors
                else if ((row + col) % 2 == 0) {
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
        renderText(renderer, "HISTORY", BOARD_SIZE + 2, 15, 0);

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
