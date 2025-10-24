#include <ncurses.h>
#include <string>
#include <vector>
#include "chess_pieces.h"
#include "chess_pieces_ncurses.h"

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

void drawNcursesChessboard() {
    clear();
    
    // Board dimensions (8x8 grid with borders)
    const int board_width = 33;  // 8 squares * 4 chars + 1 border
    const int board_height = 17; // 8 squares * 2 rows + 1 border
    
    // Use top-left positioning instead of center
    int board_start_x = 2;  // Fixed left margin
    int board_start_y = 1;  // Fixed top margin
    
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);
    
    // Calculate history panel position
    int history_start_x = board_start_x + board_width + 2;
    int history_width = max_x - history_start_x - 2;
    
    // Calculate input zone position
    int input_start_y = board_start_y + board_height + 2;
    
    // Draw chessboard border with ncurses lines
    attron(COLOR_PAIR(BORDER_COLOR));
    
    // Top border
    mvaddch(board_start_y, board_start_x, ACS_ULCORNER);
    for (int i = 1; i < board_width; i++) {
        mvaddch(board_start_y, board_start_x + i, ACS_HLINE);
    }
    mvaddch(board_start_y, board_start_x + board_width, ACS_URCORNER);
    
    // Middle rows and vertical borders
    for (int row = 1; row < board_height; row++) {
        if (row % 2 == 1) {
            // Grid intersections
            mvaddch(board_start_y + row, board_start_x, ACS_LTEE);
            for (int col = 1; col < board_width; col++) {
                if (col % 4 == 0) {
                    mvaddch(board_start_y + row, board_start_x + col, ACS_PLUS);
                } else {
                    mvaddch(board_start_y + row, board_start_x + col, ACS_HLINE);
                }
            }
            mvaddch(board_start_y + row, board_start_x + board_width, ACS_RTEE);
        } else {
            // Vertical borders
            mvaddch(board_start_y + row, board_start_x, ACS_VLINE);
            for (int col = 1; col < board_width; col++) {
                if (col % 4 == 0) {
                    mvaddch(board_start_y + row, board_start_x + col, ACS_VLINE);
                } else {
                    mvaddch(board_start_y + row, board_start_x + col, ' ');
                }
            }
            mvaddch(board_start_y + row, board_start_x + board_width, ACS_VLINE);
        }
    }
    
    // Bottom border
    mvaddch(board_start_y + board_height, board_start_x, ACS_LLCORNER);
    for (int i = 1; i < board_width; i++) {
        mvaddch(board_start_y + board_height, board_start_x + i, ACS_HLINE);
    }
    mvaddch(board_start_y + board_height, board_start_x + board_width, ACS_LRCORNER);
    
    attroff(COLOR_PAIR(BORDER_COLOR));
    
    // Draw chess pieces using the standard board
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const ChessPiece& piece = STANDARD_BOARD[row][col];
            int y = board_start_y + 1 + row * 2;
            int x = board_start_x + 2 + col * 4;
            
            bool is_selected = (row == selected_row && col == selected_col);
            renderNcursesPiece(y, x, piece, is_selected);
        }
    }
    
    // Draw history panel
    attron(COLOR_PAIR(HISTORY_COLOR));
    mvprintw(board_start_y, history_start_x, "Move History");
    
    int history_y = board_start_y + 1;
    for (size_t i = 0; i < moveHistory.size() && history_y < max_y - 5; i++) {
        mvprintw(history_y++, history_start_x, "%s", moveHistory[i].c_str());
    }
    attroff(COLOR_PAIR(HISTORY_COLOR));
    
    // Draw input zone
    attron(COLOR_PAIR(INPUT_COLOR));
    mvprintw(input_start_y, board_start_x, "Enter move: ");
    mvprintw(input_start_y, board_start_x + 12, "%s_", input_buffer.c_str());
    mvprintw(input_start_y + 1, board_start_x, "Arrow keys: Navigate | Enter: Select | q: Quit");
    
    // Show selected piece info
    const ChessPiece& selected_piece = STANDARD_BOARD[selected_row][selected_col];
    std::string piece_info = "Selected: ";
    if (selected_piece.isEmpty()) {
        piece_info += "Empty square";
    } else {
        piece_info += std::string(selected_piece.color == PieceColor::WHITE ? "White " : "Black ");
        switch (selected_piece.type) {
            case PieceType::PAWN:   piece_info += "Pawn"; break;
            case PieceType::ROOK:   piece_info += "Rook"; break;
            case PieceType::KNIGHT: piece_info += "Knight"; break;
            case PieceType::BISHOP: piece_info += "Bishop"; break;
            case PieceType::QUEEN:  piece_info += "Queen"; break;
            case PieceType::KING:   piece_info += "King"; break;
            default: piece_info += "Unknown";
        }
    }
    mvprintw(input_start_y + 2, board_start_x, "%s", piece_info.c_str());
    attroff(COLOR_PAIR(INPUT_COLOR));
    
    refresh();
}

void renderChessboardNcurses() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    initNcursesColors();
    
    int ch;
    bool running = true;
    
    while (running) {
        drawNcursesChessboard();
        ch = getch();
        
        switch (ch) {
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
            case 'q':
            case 'Q':
                running = false;
                break;
            case '\n':
                // Handle enter key for piece selection
                break;
            default:
                // Handle text input for moves
                if (ch >= 32 && ch <= 126) {
                    input_buffer += static_cast<char>(ch);
                } else if (ch == KEY_BACKSPACE || ch == 127) {
                    if (!input_buffer.empty()) {
                        input_buffer.pop_back();
                    }
                }
                break;
        }
    }
    
    endwin();
}
