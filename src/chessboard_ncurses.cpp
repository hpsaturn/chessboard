#include <ncurses.h>
#include <string>
#include <vector>
#include "chess_pieces.h"

// Color pairs for ncurses
#define WHITE_PIECE 1
#define BLACK_PIECE 2
#define WHITE_SQUARE 3
#define BLACK_SQUARE 4
#define HIGHLIGHT 5

void initColors() {
    start_color();
    init_pair(WHITE_PIECE, COLOR_WHITE, COLOR_BLACK);
    init_pair(BLACK_PIECE, COLOR_BLACK, COLOR_BLACK);
    init_pair(WHITE_SQUARE, COLOR_BLACK, COLOR_WHITE);
    init_pair(BLACK_SQUARE, COLOR_WHITE, COLOR_BLACK);
    init_pair(HIGHLIGHT, COLOR_YELLOW, COLOR_BLUE);
}

void renderChessboardNcurses() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    // Initialize colors if supported
    if (has_colors()) {
        initColors();
    }
    
    // Get terminal dimensions
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    // Calculate board position (centered)
    int board_start_y = (max_y - 20) / 2;
    int board_start_x = (max_x - 32) / 2;
    
    // Selected position
    int selected_row = 0;
    int selected_col = 0;
    
    bool quit = false;
    
    while (!quit) {
        clear();
        
        // Draw title
        attron(A_BOLD);
        mvprintw(1, (max_x - 25) / 2, "Chessboard - NCurses Mode");
        attroff(A_BOLD);
        
        // Draw chessboard
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                int y = board_start_y + row * 2;
                int x = board_start_x + col * 4;
                
                // Set square color
                if ((row + col) % 2 == 0) {
                    attron(COLOR_PAIR(WHITE_SQUARE));
                } else {
                    attron(COLOR_PAIR(BLACK_SQUARE));
                }
                
                // Highlight selected square
                if (row == selected_row && col == selected_col) {
                    attron(COLOR_PAIR(HIGHLIGHT));
                }
                
                // Draw square
                mvaddch(y, x, ' ');
                mvaddch(y, x + 1, ' ');
                mvaddch(y, x + 2, ' ');
                mvaddch(y, x + 3, ' ');
                
                // Draw piece
                ChessPiece piece = STANDARD_BOARD[row][col];
                if (!piece.isEmpty()) {
                    char pieceSymbol = getPieceSymbol(piece);
                    
                    // Set piece color
                    if (piece.color == PieceColor::WHITE) {
                        attron(COLOR_PAIR(WHITE_PIECE) | A_BOLD);
                    } else {
                        attron(COLOR_PAIR(BLACK_PIECE) | A_BOLD);
                    }
                    
                    mvaddch(y + 1, x + 2, pieceSymbol);
                    
                    // Reset attributes
                    attroff(COLOR_PAIR(WHITE_PIECE) | COLOR_PAIR(BLACK_PIECE) | A_BOLD);
                }
                
                // Reset square attributes
                attroff(COLOR_PAIR(WHITE_SQUARE) | COLOR_PAIR(BLACK_SQUARE) | COLOR_PAIR(HIGHLIGHT));
            }
        }
        
        // Draw coordinates
        for (int i = 0; i < 8; ++i) {
            mvprintw(board_start_y + i * 2, board_start_x - 2, "%d", 8 - i);
            mvprintw(board_start_y + 16, board_start_x + i * 4 + 2, "%c", 'a' + i);
        }
        
        // Draw piece information
        ChessPiece selected_piece = STANDARD_BOARD[selected_row][selected_col];
        mvprintw(board_start_y + 18, board_start_x, "Selected: [%c%c] ", 
                 'a' + selected_col, '8' - selected_row);
        
        if (!selected_piece.isEmpty()) {
            std::string piece_name;
            switch (selected_piece.type) {
                case PieceType::PAWN: piece_name = "Pawn"; break;
                case PieceType::ROOK: piece_name = "Rook"; break;
                case PieceType::KNIGHT: piece_name = "Knight"; break;
                case PieceType::BISHOP: piece_name = "Bishop"; break;
                case PieceType::QUEEN: piece_name = "Queen"; break;
                case PieceType::KING: piece_name = "King"; break;
                default: piece_name = "Unknown";
            }
            
            std::string color_name = (selected_piece.color == PieceColor::WHITE) ? "White" : "Black";
            printw("%s %s", color_name.c_str(), piece_name.c_str());
        } else {
            printw("Empty Square");
        }
        
        // Draw controls
        mvprintw(board_start_y + 20, board_start_x, "Controls: Arrow Keys - Move | Q - Quit | Enter - Select");
        
        refresh();
        
        // Handle input
        int ch = getch();
        switch (ch) {
            case 'q':
            case 'Q':
                quit = true;
                break;
            case KEY_UP:
                if (selected_row > 0) selected_row--;
                break;
            case KEY_DOWN:
                if (selected_row < 7) selected_row++;
                break;
            case KEY_LEFT:
                if (selected_col > 0) selected_col--;
                break;
            case KEY_RIGHT:
                if (selected_col < 7) selected_col++;
                break;
            case '\n':
            case KEY_ENTER:
                // Handle piece selection (placeholder for future move logic)
                break;
        }
    }
    
    endwin();
}
