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
#define BORDER_COLOR 6
#define HISTORY_COLOR 7
#define INPUT_COLOR 8

// Sample move history for demonstration
static std::vector<std::string> moveHistory = {
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

static int selected_row = 0;
static int selected_col = 0;
static std::string input_buffer = "";

void initNcursesColors() {
    if (has_colors()) {
        start_color();
        init_pair(WHITE_PIECE, COLOR_WHITE, COLOR_BLACK);
        init_pair(BLACK_PIECE, COLOR_BLACK, COLOR_BLACK);
        init_pair(WHITE_SQUARE, COLOR_BLACK, COLOR_WHITE);
        init_pair(BLACK_SQUARE, COLOR_WHITE, COLOR_BLACK);
        init_pair(HIGHLIGHT, COLOR_BLACK, COLOR_YELLOW);
        init_pair(BORDER_COLOR, COLOR_CYAN, COLOR_BLACK);
        init_pair(HISTORY_COLOR, COLOR_GREEN, COLOR_BLACK);
        init_pair(INPUT_COLOR, COLOR_YELLOW, COLOR_BLACK);
    }
}

// Use letter-based piece representation like character mode
char getNcursesPieceSymbol(const ChessPiece& piece) {
    if (piece.isEmpty()) return ' ';
    
    switch (piece.type) {
        case PieceType::PAWN:   return (piece.color == PieceColor::WHITE) ? 'P' : 'p';
        case PieceType::ROOK:   return (piece.color == PieceColor::WHITE) ? 'R' : 'r';
        case PieceType::KNIGHT: return (piece.color == PieceColor::WHITE) ? 'N' : 'n';
        case PieceType::BISHOP: return (piece.color == PieceColor::WHITE) ? 'B' : 'b';
        case PieceType::QUEEN:  return (piece.color == PieceColor::WHITE) ? 'Q' : 'q';
        case PieceType::KING:   return (piece.color == PieceColor::WHITE) ? 'K' : 'k';
        default: return ' ';
    }
}

void renderChessboardNcurses() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    // Initialize colors if supported
    initNcursesColors();
    
    bool quit = false;
    
    while (!quit) {
        clear();
        
        // Get terminal dimensions
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);
        
        // Calculate layout
        int board_width = 33;  // 8 squares * 4 chars + 1 border
        int board_height = 17; // 8 squares * 2 + 1 border
        int board_start_x = (max_x - board_width) / 2;
        int board_start_y = 2;
        
        int history_start_x = board_start_x + board_width + 2;
        int history_width = max_x - history_start_x - 2;
        
        int input_start_y = board_start_y + board_height + 2;
        
        // Draw chessboard with proper ncurses lines
        attron(COLOR_PAIR(BORDER_COLOR));
        
        // Draw top border
        mvaddch(board_start_y, board_start_x, ACS_ULCORNER);
        for (int i = 1; i < board_width; i++) {
            mvaddch(board_start_y, board_start_x + i, ACS_HLINE);
        }
        mvaddch(board_start_y, board_start_x + board_width, ACS_URCORNER);
        
        // Draw side borders and inner grid
        for (int row = 1; row < board_height; row++) {
            // Left border
            if (row % 2 == 0) {
                mvaddch(board_start_y + row, board_start_x, ACS_LTEE);
            } else {
                mvaddch(board_start_y + row, board_start_x, ACS_VLINE);
            }
            
            // Inner grid
            for (int col = 1; col < board_width; col++) {
                if (row % 2 == 0) {
                    // Horizontal lines
                    mvaddch(board_start_y + row, board_start_x + col, ACS_HLINE);
                } else if (col % 4 == 0) {
                    // Vertical lines
                    mvaddch(board_start_y + row, board_start_x + col, ACS_VLINE);
                } else if (row % 2 == 0 && col % 4 == 0) {
                    // Cross intersections
                    mvaddch(board_start_y + row, board_start_x + col, ACS_PLUS);
                }
            }
            
            // Right border
            if (row % 2 == 0) {
                mvaddch(board_start_y + row, board_start_x + board_width, ACS_RTEE);
            } else {
                mvaddch(board_start_y + row, board_start_x + board_width, ACS_VLINE);
            }
        }
        
        // Draw bottom border
        mvaddch(board_start_y + board_height, board_start_x, ACS_LLCORNER);
        for (int i = 1; i < board_width; i++) {
            mvaddch(board_start_y + board_height, board_start_x + i, ACS_HLINE);
        }
        mvaddch(board_start_y + board_height, board_start_x + board_width, ACS_LRCORNER);
        
        attroff(COLOR_PAIR(BORDER_COLOR));
        
        // Draw chessboard squares and pieces
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                int y = board_start_y + 1 + row * 2;
                int x = board_start_x + 2 + col * 4;
                
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
                
                // Draw square background
                for (int i = 0; i < 3; i++) {
                    mvaddch(y, x + i, ' ');
                }
                
                // Draw piece using letter symbols
                ChessPiece piece = STANDARD_BOARD[row][col];
                if (!piece.isEmpty()) {
                    char pieceSymbol = getNcursesPieceSymbol(piece);
                    
                    // Set piece color
                    if (piece.color == PieceColor::WHITE) {
                        attron(COLOR_PAIR(WHITE_PIECE) | A_BOLD);
                    } else {
                        attron(COLOR_PAIR(BLACK_PIECE) | A_BOLD);
                    }
                    
                    // Print the piece symbol
                    mvaddch(y, x + 1, pieceSymbol);
                    
                    // Reset attributes
                    attroff(COLOR_PAIR(WHITE_PIECE) | COLOR_PAIR(BLACK_PIECE) | A_BOLD);
                }
                
                // Reset square attributes
                attroff(COLOR_PAIR(WHITE_SQUARE) | COLOR_PAIR(BLACK_SQUARE) | COLOR_PAIR(HIGHLIGHT));
            }
        }
        
        // Draw coordinates
        attron(A_BOLD);
        for (int i = 0; i < 8; ++i) {
            mvprintw(board_start_y + 1 + i * 2, board_start_x - 1, "%d", 8 - i);
            mvprintw(board_start_y + board_height - 1, board_start_x + 2 + i * 4, "%c", 'a' + i);
        }
        attroff(A_BOLD);
        
        // Draw move history panel
        attron(COLOR_PAIR(HISTORY_COLOR) | A_BOLD);
        mvprintw(board_start_y, history_start_x, "Move History");
        attroff(COLOR_PAIR(HISTORY_COLOR) | A_BOLD);
        
        // Draw history entries
        int history_y = board_start_y + 1;
        for (size_t i = 0; i < moveHistory.size() && history_y < max_y - 5; i++) {
            mvprintw(history_y++, history_start_x, "%s", moveHistory[i].c_str());
        }
        
        // Draw input zone
        attron(COLOR_PAIR(INPUT_COLOR) | A_BOLD);
        mvprintw(input_start_y, board_start_x, "Input: %s_", input_buffer.c_str());
        attroff(COLOR_PAIR(INPUT_COLOR) | A_BOLD);
        
        // Draw piece information
        ChessPiece selected_piece = STANDARD_BOARD[selected_row][selected_col];
        mvprintw(input_start_y + 2, board_start_x, "Selected: [%c%c] ", 
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
        mvprintw(input_start_y + 4, board_start_x, "Controls: Arrows-Move | Type-Input | Enter-Execute | Q-Quit");
        
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
            case '\n': // Enter key
                if (!input_buffer.empty()) {
                    // Add to history
                    moveHistory.push_back(input_buffer);
                    input_buffer.clear();
                }
                break;
            case KEY_BACKSPACE:
            case 127: // Backspace
                if (!input_buffer.empty()) {
                    input_buffer.pop_back();
                }
                break;
            default:
                if (ch >= 32 && ch <= 126) { // Printable characters
                    input_buffer += static_cast<char>(ch);
                }
                break;
        }
    }
    
    endwin();
}
