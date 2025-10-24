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

void initColors() {
    start_color();
    init_pair(WHITE_PIECE, COLOR_WHITE, COLOR_BLACK);
    init_pair(BLACK_PIECE, COLOR_BLACK, COLOR_WHITE);
    init_pair(WHITE_SQUARE, COLOR_BLACK, COLOR_WHITE);
    init_pair(BLACK_SQUARE, COLOR_WHITE, COLOR_BLACK);
    init_pair(HIGHLIGHT, COLOR_YELLOW, COLOR_BLUE);
    init_pair(BORDER_COLOR, COLOR_CYAN, COLOR_BLACK);
    init_pair(HISTORY_COLOR, COLOR_GREEN, COLOR_BLACK);
    init_pair(INPUT_COLOR, COLOR_YELLOW, COLOR_BLACK);
}

// Enhanced piece symbols using ASCII art
std::string getEnhancedPieceSymbol(const ChessPiece& piece) {
    if (piece.isEmpty()) return " ";
    
    if (piece.color == PieceColor::WHITE) {
        switch (piece.type) {
            case PieceType::PAWN:   return "♙";
            case PieceType::ROOK:   return "♖";
            case PieceType::KNIGHT: return "♘";
            case PieceType::BISHOP: return "♗";
            case PieceType::QUEEN:  return "♕";
            case PieceType::KING:   return "♔";
            default: return "?";
        }
    } else {
        switch (piece.type) {
            case PieceType::PAWN:   return "♟";
            case PieceType::ROOK:   return "♜";
            case PieceType::KNIGHT: return "♞";
            case PieceType::BISHOP: return "♝";
            case PieceType::QUEEN:  return "♛";
            case PieceType::KING:   return "♚";
            default: return "?";
        }
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
    if (has_colors()) {
        initColors();
    }
    
    // Get terminal dimensions
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    // Layout calculations
    int board_width = 36;  // 8 squares * 4 chars + borders
    int board_height = 18; // 8 squares * 2 lines + borders
    int history_width = 20;
    int input_height = 6;
    
    int board_start_y = (max_y - board_height) / 2;
    int board_start_x = (max_x - board_width - history_width) / 2;
    int history_start_x = board_start_x + board_width;
    int input_start_y = board_start_y + board_height;
    
    // Selected position
    int selected_row = 0;
    int selected_col = 0;
    std::string input_buffer = "";
    
    bool quit = false;
    
    while (!quit) {
        clear();
        
        // Draw title
        attron(A_BOLD);
        mvprintw(1, (max_x - 25) / 2, "Chessboard - NCurses Mode");
        attroff(A_BOLD);
        
        // Draw chessboard with borders
        attron(COLOR_PAIR(BORDER_COLOR));
        
        // Draw top border
        for (int x = board_start_x; x < board_start_x + board_width; x++) {
            mvaddch(board_start_y, x, ACS_HLINE);
        }
        
        // Draw bottom border
        for (int x = board_start_x; x < board_start_x + board_width; x++) {
            mvaddch(board_start_y + board_height, x, ACS_HLINE);
        }
        
        // Draw side borders and corners
        for (int y = board_start_y; y <= board_start_y + board_height; y++) {
            mvaddch(y, board_start_x, ACS_VLINE);
            mvaddch(y, board_start_x + board_width, ACS_VLINE);
        }
        
        // Draw corners
        mvaddch(board_start_y, board_start_x, ACS_ULCORNER);
        mvaddch(board_start_y, board_start_x + board_width, ACS_URCORNER);
        mvaddch(board_start_y + board_height, board_start_x, ACS_LLCORNER);
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
                
                // Draw piece using enhanced symbols
                ChessPiece piece = STANDARD_BOARD[row][col];
                if (!piece.isEmpty()) {
                    std::string pieceSymbol = getEnhancedPieceSymbol(piece);
                    
                    // Set piece color
                    if (piece.color == PieceColor::WHITE) {
                        attron(COLOR_PAIR(WHITE_PIECE) | A_BOLD);
                    } else {
                        attron(COLOR_PAIR(BLACK_PIECE) | A_BOLD);
                    }
                    
                    // Print the piece symbol (may be multi-byte UTF-8)
                    mvprintw(y, x + 1, "%s", pieceSymbol.c_str());
                    
                    // Reset attributes
                    attroff(COLOR_PAIR(WHITE_PIECE) | COLOR_PAIR(BLACK_PIECE) | A_BOLD);
                }
                
                // Reset square attributes
                attroff(COLOR_PAIR(WHITE_SQUARE) | COLOR_PAIR(BLACK_SQUARE) | COLOR_PAIR(HIGHLIGHT));
            }
        }
        
        // Draw coordinates
        for (int i = 0; i < 8; ++i) {
            mvprintw(board_start_y + 1 + i * 2, board_start_x - 1, "%d", 8 - i);
            mvprintw(board_start_y + board_height - 1, board_start_x + 2 + i * 4, "%c", 'a' + i);
        }
        
        // Draw move history panel
        attron(COLOR_PAIR(HISTORY_COLOR) | A_BOLD);
        mvprintw(board_start_y, history_start_x, "Move History");
        attroff(COLOR_PAIR(HISTORY_COLOR) | A_BOLD);
        
        for (size_t i = 0; i < moveHistory.size() && i < 16; ++i) {
            mvprintw(board_start_y + 2 + i, history_start_x, "%s", moveHistory[i].c_str());
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
            case '\n':
            case KEY_ENTER:
                if (!input_buffer.empty()) {
                    // Execute command (placeholder for future logic)
                    moveHistory.push_back(input_buffer);
                    input_buffer.clear();
                }
                break;
            case KEY_BACKSPACE:
            case 127:
            case 8:
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
