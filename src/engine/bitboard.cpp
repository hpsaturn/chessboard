#include "bitboard.h"

void ChessBoard::initialize_attack_tables() {
  // Knight moves (8 possible directions)
  int knight_dx[] = {2, 2, 1, 1, -1, -1, -2, -2};
  int knight_dy[] = {1, -1, 2, -2, 2, -2, 1, -1};

  // King moves (8 directions)
  int king_dx[] = {1, 1, 1, 0, 0, -1, -1, -1};
  int king_dy[] = {1, 0, -1, 1, -1, 1, 0, -1};

  // Pawn attacks (captures)
  int white_pawn_dx[] = {1, 1};
  int white_pawn_dy[] = {-1, 1};
  int black_pawn_dx[] = {-1, -1};
  int black_pawn_dy[] = {-1, 1};

  for (int square = 0; square < 64; ++square) {
    int x = square % 8;
    int y = square / 8;

    // Initialize knight attacks
    knight_attacks[square] = 0;
    for (int i = 0; i < 8; ++i) {
      int new_x = x + knight_dx[i];
      int new_y = y + knight_dy[i];
      if (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8) {
        knight_attacks[square] |= 1ULL << (new_y * 8 + new_x);
      }
    }

    // Initialize king attacks
    king_attacks[square] = 0;
    for (int i = 0; i < 8; ++i) {
      int new_x = x + king_dx[i];
      int new_y = y + king_dy[i];
      if (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8) {
        king_attacks[square] |= 1ULL << (new_y * 8 + new_x);
      }
    }

    // Initialize pawn attacks
    pawn_attacks[ChessBoard::WHITE][square] = 0;
    for (int i = 0; i < 2; ++i) {
      int new_x = x + white_pawn_dy[i];  // Note: swapped for little-endian
      int new_y = y + white_pawn_dx[i];
      if (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8) {
        pawn_attacks[ChessBoard::WHITE][square] |= 1ULL << (new_y * 8 + new_x);
      }
    }

    pawn_attacks[ChessBoard::BLACK][square] = 0;
    for (int i = 0; i < 2; ++i) {
      int new_x = x + black_pawn_dy[i];
      int new_y = y + black_pawn_dx[i];
      if (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8) {
        pawn_attacks[ChessBoard::BLACK][square] |= 1ULL << (new_y * 8 + new_x);
      }
    }

    // Initialize basic sliding piece attacks (simplified - no magic bitboards)
    rook_attacks_base[square] = 0;
    bishop_attacks_base[square] = 0;

    // Rook: horizontal and vertical
    for (int dir = 0; dir < 4; ++dir) {
      int dx = (dir == 1) ? 1 : (dir == 3) ? -1 : 0;
      int dy = (dir == 0) ? 1 : (dir == 2) ? -1 : 0;

      int new_x = x + dx;
      int new_y = y + dy;
      while (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8) {
        rook_attacks_base[square] |= 1ULL << (new_y * 8 + new_x);
        new_x += dx;
        new_y += dy;
      }
    }

    // Bishop: diagonals
    for (int dir = 0; dir < 4; ++dir) {
      int dx = (dir == 0 || dir == 1) ? 1 : -1;
      int dy = (dir == 0 || dir == 3) ? 1 : -1;

      int new_x = x + dx;
      int new_y = y + dy;
      while (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8) {
        bishop_attacks_base[square] |= 1ULL << (new_y * 8 + new_x);
        new_x += dx;
        new_y += dy;
      }
    }
  }
}

void ChessBoard::set_initial_position() {
  // Clear all bitboards
  for (int i = 0; i < 6; ++i) pieces[i] = 0;
  for (int i = 0; i < 2; ++i) colors[i] = 0;

  // Set up pawns
  pieces[ChessBoard::PAWN] = 0xFF00ULL | (0xFF000000000000ULL);  // ranks 2 and 7

  // Set up other pieces
  pieces[ChessBoard::KNIGHT] = (1ULL << ChessBoard::B1) | (1ULL << ChessBoard::G1) | (1ULL << ChessBoard::B8) | (1ULL << ChessBoard::G8);
  pieces[ChessBoard::BISHOP] = (1ULL << ChessBoard::C1) | (1ULL << ChessBoard::F1) | (1ULL << ChessBoard::C8) | (1ULL << ChessBoard::F8);
  pieces[ChessBoard::ROOK] = (1ULL << ChessBoard::A1) | (1ULL << ChessBoard::H1) | (1ULL << ChessBoard::A8) | (1ULL << ChessBoard::H8);
  pieces[ChessBoard::QUEEN] = (1ULL << ChessBoard::D1) | (1ULL << ChessBoard::D8);
  pieces[ChessBoard::KING] = (1ULL << ChessBoard::E1) | (1ULL << ChessBoard::E8);

  // Set up colors
  colors[ChessBoard::WHITE] = 0xFFFFULL;              // ranks 1 and 2
  colors[ChessBoard::BLACK] = 0xFFFF000000000000ULL;  // ranks 7 and 8

  side_to_move = ChessBoard::WHITE;

  // Initialize castling rights
  castling_rights[ChessBoard::WHITE][0] = castling_rights[ChessBoard::WHITE][1] = true;  // kingside, queenside
  castling_rights[ChessBoard::BLACK][0] = castling_rights[ChessBoard::BLACK][1] = true;
}

ChessBoard::Piece ChessBoard::get_piece_at(ChessBoard::Square square) const {
  ChessBoard::Bitboard mask = 1ULL << square;
  for (int piece = ChessBoard::PAWN; piece <= ChessBoard::KING; ++piece) {
    if (pieces[piece] & mask) {
      return static_cast<ChessBoard::Piece>(piece);
    }
  }
  return ChessBoard::NONE;
}

ChessBoard::Color ChessBoard::get_color_at(ChessBoard::Square square) const {
  ChessBoard::Bitboard mask = 1ULL << square;
  if (colors[ChessBoard::WHITE] & mask) return ChessBoard::WHITE;
  if (colors[ChessBoard::BLACK] & mask) return ChessBoard::BLACK;
  return ChessBoard::BOTH;  // Empty square
}

ChessBoard::Bitboard ChessBoard::get_sliding_attacks(Square square, Piece piece) const {
  Bitboard attacks = 0;
  Bitboard occupancy = colors[WHITE] | colors[BLACK];

  int x = square % 8;
  int y = square / 8;

  std::vector<std::pair<int, int>> directions;

  if (piece == ROOK || piece == QUEEN) {
    directions.insert(directions.end(), {{0, 1}, {1, 0}, {0, -1}, {-1, 0}});
  }
  if (piece == BISHOP || piece == QUEEN) {
    directions.insert(directions.end(), {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}});
  }

  for (const auto& [dx, dy] : directions) {
    int new_x = x + dx;
    int new_y = y + dy;

    while (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8) {
      int current_square = new_y * 8 + new_x;
      attacks |= (1ULL << current_square);

      if (occupancy & (1ULL << current_square)) {
        break;
      }

      new_x += dx;
      new_y += dy;
    }
  }

  return attacks;
}

ChessBoard::Bitboard ChessBoard::compute_attack_map(ChessBoard::Color attacking_color) const {
  ChessBoard::Bitboard attack_map = 0;
  ChessBoard::Bitboard occupancy = colors[ChessBoard::WHITE] | colors[ChessBoard::BLACK];

  // Pawn attacks
  ChessBoard::Bitboard pawns = pieces[ChessBoard::PAWN] & colors[attacking_color];
  while (pawns) {
    int square = __builtin_ctzll(pawns);
    attack_map |= pawn_attacks[attacking_color][square];
    pawns &= pawns - 1;  // Clear least significant bit
  }

  // Knight attacks
  ChessBoard::Bitboard knights = pieces[ChessBoard::KNIGHT] & colors[attacking_color];
  while (knights) {
    int square = __builtin_ctzll(knights);
    attack_map |= knight_attacks[square];
    knights &= knights - 1;
  }

  // King attacks
  ChessBoard::Bitboard kings = pieces[ChessBoard::KING] & colors[attacking_color];
  while (kings) {
    int square = __builtin_ctzll(kings);
    attack_map |= king_attacks[square];
    kings &= kings - 1;
  }

  // // Bishop attacks (simplified - considers all moves without blocking)
  // ChessBoard::Bitboard bishops = pieces[ChessBoard::BISHOP] & colors[attacking_color];
  // while (bishops) {
  //   int square = __builtin_ctzll(bishops);
  //   attack_map |= bishop_attacks_base[square];
  //   bishops &= bishops - 1;
  // }

  // Bishop attacks
  Bitboard bishops = pieces[BISHOP] & colors[attacking_color];
  while (bishops) {
    int square = __builtin_ctzll(bishops);
    attack_map |= get_sliding_attacks(static_cast<Square>(square), BISHOP);
    bishops &= bishops - 1;
  }

  // Rook attacks (simplified)
  // ChessBoard::Bitboard rooks = pieces[ChessBoard::ROOK] & colors[attacking_color];
  // while (rooks) {
  //   int square = __builtin_ctzll(rooks);
  //   attack_map |= rook_attacks_base[square];
  //   rooks &= rooks - 1;
  // }

  // Rook attacks
  Bitboard rooks = pieces[ROOK] & colors[attacking_color];
  while (rooks) {
    int square = __builtin_ctzll(rooks);
    attack_map |= get_sliding_attacks(static_cast<Square>(square), ROOK);
    rooks &= rooks - 1;
  }

  // Queen attacks (bishop + rook)
  // ChessBoard::Bitboard queens = pieces[ChessBoard::QUEEN] & colors[attacking_color];
  // while (queens) {
  //   int square = __builtin_ctzll(queens);
  //   attack_map |= bishop_attacks_base[square] | rook_attacks_base[square];
  //   queens &= queens - 1;
  // }

  // Queen attacks
  Bitboard queens = pieces[QUEEN] & colors[attacking_color];
  while (queens) {
    int square = __builtin_ctzll(queens);
    attack_map |= get_sliding_attacks(static_cast<Square>(square), QUEEN);
    queens &= queens - 1;
  }

  return attack_map;
}

bool ChessBoard::is_castling_move_legal(ChessBoard::Square from, Square to) const {
  ChessBoard::Color moving_color = get_color_at(from);
  int rank = (moving_color == ChessBoard::WHITE) ? 0 : 7;  // rank 0 for white, 7 for black

  // Kingside castling
  if (to == ChessBoard::G1 + rank * 8) {
    return is_kingside_castling_legal(moving_color);
  }
  // Queenside castling
  else if (to == ChessBoard::C1 + rank * 8) {
    return is_queenside_castling_legal(moving_color);
  }

  return false;
}

bool ChessBoard::is_kingside_castling_legal(ChessBoard::Color color) const {
  if (!castling_rights[color][0]) return false;  // No kingside castling right

  int rank = (color == ChessBoard::WHITE) ? 0 : 7;
  Square king_square = static_cast<ChessBoard::Square>(ChessBoard::E1 + rank * 8);
  Square f_square = static_cast<ChessBoard::Square>(ChessBoard::F1 + rank * 8);
  Square g_square = static_cast<ChessBoard::Square>(ChessBoard::G1 + rank * 8);

  // Check if squares between king and rook are empty
  ChessBoard::Bitboard occupancy = colors[ChessBoard::WHITE] | colors[ChessBoard::BLACK];
  if (occupancy & ((1ULL << f_square) | (1ULL << g_square))) {
    return false;
  }

  // Check if king is not in check and doesn't move through check
  ChessBoard::Color opponent_color = (color == ChessBoard::WHITE) ? ChessBoard::BLACK : ChessBoard::WHITE;
  ChessBoard::Bitboard opponent_attacks = compute_attack_map(opponent_color);

  return !(opponent_attacks & (1ULL << king_square)) && !(opponent_attacks & (1ULL << f_square)) &&
         !(opponent_attacks & (1ULL << g_square));
}

bool ChessBoard::is_queenside_castling_legal(ChessBoard::Color color) const {
  if (!castling_rights[color][1]) return false;  // No queenside castling right

  int rank = (color == ChessBoard::WHITE) ? 0 : 7;
  Square king_square = static_cast<ChessBoard::Square>(ChessBoard::E1 + rank * 8);
  Square d_square = static_cast<ChessBoard::Square>(ChessBoard::D1 + rank * 8);
  Square c_square = static_cast<ChessBoard::Square>(ChessBoard::C1 + rank * 8);
  Square b_square = static_cast<ChessBoard::Square>(ChessBoard::B1 + rank * 8);

  // Check if squares between king and rook are empty
  ChessBoard::Bitboard occupancy = colors[ChessBoard::WHITE] | colors[ChessBoard::BLACK];
  if (occupancy & ((1ULL << d_square) | (1ULL << c_square) | (1ULL << b_square))) {
    return false;
  }

  // Check if king is not in check and doesn't move through check
  ChessBoard::Color opponent_color = (color == ChessBoard::WHITE) ? ChessBoard::BLACK : ChessBoard::WHITE;
  ChessBoard::Bitboard opponent_attacks = compute_attack_map(opponent_color);

  return !(opponent_attacks & (1ULL << king_square)) && !(opponent_attacks & (1ULL << d_square)) &&
         !(opponent_attacks & (1ULL << c_square));
}

/**
 * Find the square of the king for the given color
 * @param color: Color of the king to find
 * @return Square where the king is located, or NO_SQ if not found
 */
ChessBoard::Square ChessBoard::find_king_square(Color color) const {
  Bitboard king_bitboard = pieces[KING] & colors[color];
  if (king_bitboard == 0) {
    return NO_SQ;
  }
  return static_cast<Square>(__builtin_ctzll(king_bitboard));
}

/**
 * Make a move on the board (for simulation purposes)
 * Note: This is a simplified version that doesn't handle special moves
 */
void ChessBoard::make_move_on_board(Square from, Square to) {
  Piece moving_piece = get_piece_at(from);
  Color moving_color = get_color_at(from);

  if (moving_piece == NONE) return;

  // Clear the source square
  pieces[moving_piece] &= ~(1ULL << from);
  colors[moving_color] &= ~(1ULL << from);

  // Clear the destination square (capture)
  Piece captured_piece = get_piece_at(to);
  if (captured_piece != NONE) {
    Color captured_color = get_color_at(to);
    pieces[captured_piece] &= ~(1ULL << to);
    colors[captured_color] &= ~(1ULL << to);
  }

  // Set the destination square
  pieces[moving_piece] |= (1ULL << to);
  colors[moving_color] |= (1ULL << to);

  // Switch side to move
  side_to_move = (side_to_move == WHITE) ? WHITE : BLACK;
}

bool ChessBoard::is_king_move_legal(ChessBoard::Square from, Square to) const {
  ChessBoard::Color moving_color = get_color_at(from);
  ChessBoard::Color opponent_color =
      (moving_color == ChessBoard::WHITE) ? ChessBoard::BLACK : ChessBoard::WHITE;

  // Basic validation
  if (from == to) return false;
  if (get_piece_at(from) != ChessBoard::KING) return false;

  // Check if destination is occupied by friendly piece
  ChessBoard::Color dest_color = get_color_at(to);
  if (dest_color == moving_color) return false;

  // Check if move is within king's movement pattern
  ChessBoard::Bitboard king_moves = king_attacks[from];
  if (!(king_moves & (1ULL << to))) {
    // Not a normal king move, check for castling
    return is_castling_move_legal(from, to);
  }

  // For normal moves: compute opponent's attack map and check if destination is safe
  ChessBoard::Bitboard opponent_attacks = compute_attack_map(opponent_color);
  return !(opponent_attacks & (1ULL << to));
}

/**
 * Check if a king move is legal using row,col coordinates
 * @param from_row, from_col: source coordinates
 * @param to_row, to_col: destination coordinates
 * @return true if move is legal
 */
bool ChessBoard::is_king_move_legal(int from_row, int from_col, int to_row, int to_col) const {
  Square from_square = from_row_col(from_row, from_col);
  Square to_square = from_row_col(to_row, to_col);
  return is_king_move_legal(from_square, to_square);
}

/**
 * Check if the king of the given color is in check
 * @param king_color: Color of the king to check
 * @return true if the king is in check
 */
bool ChessBoard::is_king_in_check(Color king_color) const {
  Color opponent_color = (king_color == WHITE) ? BLACK : WHITE;

  // Find the king's square
  Square king_square = find_king_square(king_color);
  if (king_square == NO_SQ) {
    throw std::runtime_error("King not found on the board");
  }

  // Compute opponent's attack map and check if king's square is attacked
  Bitboard opponent_attacks = compute_attack_map(opponent_color);
  return (opponent_attacks & (1ULL << king_square)) != 0;
}

/**
 * Check if the king of the given color is in check (using row,col)
 * @param king_color: Color of the king to check
 * @return true if the king is in check
 */
bool ChessBoard::is_king_in_check_row_col(int king_row, int king_col) const {
  Color king_color = get_color_at(from_row_col(king_row, king_col));
  return is_king_in_check(king_color);
}

/**
 * Check if the current side to move is in check
 * @return true if current side to move is in check
 */
bool ChessBoard::is_current_player_in_check() const { return is_king_in_check(side_to_move); }

/**
 * Check if the move would leave the king in check (simulates the move)
 * @param from: source square
 * @param to: destination square
 * @return true if the move would leave the king in check
 */
bool ChessBoard::would_move_leave_king_in_check(Square from, Square to) const {
  // Create a temporary copy of the board and make the move
  ChessBoard temp_board = *this;
  temp_board.make_move_on_board(from, to);
  std::cout << "[GAME] simulated board: " << std::endl;
  temp_board.print_board();
  std::cout << "\n";
  // Check if the king is in check after the move
  return temp_board.is_current_player_in_check();
}

// Set up a custom position for testing
ChessBoard::Square ChessBoard::set_custom_position(const std::string& fen) {
  // Clear the board
  for (int i = 0; i < 6; ++i) pieces[i] = 0;
  for (int i = 0; i < 2; ++i) colors[i] = 0;

  // Simple FEN parser for basic test positions
  // This is a simplified parser - only handles piece placement
  int rank = 7;
  int file = 0;

  Square king;

  std::string sfen = fen.substr(0, fen.find(' '));  // Get only piece placement part

  for (char c : sfen) {
    if (c == '/') {
      rank--;
      file = 0;
    } else if (isdigit(c)) {
      file += c - '0';
    } else {
      Piece piece;
      Color color = isupper(c) ? WHITE : BLACK;

      switch (tolower(c)) {
        case 'p':
          piece = PAWN;
          break;
        case 'n':
          piece = KNIGHT;
          break;
        case 'b':
          piece = BISHOP;
          break;
        case 'r':
          piece = ROOK;
          break;
        case 'q':
          piece = QUEEN;
          break;
        case 'k':
          piece = KING;
          break;
        default:
          continue;
      }

      int square = rank * 8 + file;
      if (piece == KING && color == WHITE) {
        king = static_cast<Square>(square);
      }
      set_piece(static_cast<Square>(square), piece, color);
      file++;
    }
    side_to_move = WHITE;
  }

  // Reset castling rights for custom positions
  castling_rights[WHITE][0] = castling_rights[WHITE][1] = false;
  castling_rights[BLACK][0] = castling_rights[BLACK][1] = false;
  
  return king;
}

// Helper function to set a piece on the board
void ChessBoard::set_piece(Square square, Piece piece, Color color) {
  Bitboard mask = 1ULL << square;

  // Clear any existing piece on this square
  for (int i = 0; i < 6; ++i) {
    pieces[i] &= ~mask;
  }
  colors[WHITE] &= ~mask;
  colors[BLACK] &= ~mask;

  // Set the new piece
  pieces[piece] |= mask;
  colors[color] |= mask;
}

// =========================================================================
// CONVERSION METHODS - For compatibility with your old system
// =========================================================================

/**
 * Convert from (row, col) coordinates to bitboard Square enum
 * @param row: 0-7 where 0 is rank 1 (bottom) and 7 is rank 8 (top)
 * @param col: 0-7 where 0 is file a (left) and 7 is file h (right)
 * @return Square enum value
 */
ChessBoard::Square ChessBoard::from_row_col(int row, int col) const {
  if (row < 0 || row > 7 || col < 0 || col > 7) {
    throw std::out_of_range("Row and col must be between 0 and 7");
  }
  // Convert from your system to standard bitboard system:
  // Your system: row 0 = rank 8, row 7 = rank 1
  // Bitboard: row 0 = rank 1, row 7 = rank 8
  int bitboard_row = 7 - row;
  int bitboard_col = col;
  return static_cast<Square>(bitboard_row * 8 + col);
}

/**
 * Convert from bitboard Square enum to (row, col) coordinates
 * @param square: Square enum value
 * @return pair of (row, col) where row 0 is rank 1, row 7 is rank 8
 */
std::pair<int, int> ChessBoard::to_row_col(Square square) const {
  if (square < 0 || square > 63) {
    throw std::out_of_range("Square must be between 0 and 63");
  }
  int row = square / 8;
  int col = square % 8;
  return {row, col};
}

// =========================================================================
// TESTING AND DEBUGGING METHODS 
// =========================================================================

void ChessBoard::print_board() const {
  std::cout << "  a b c d e f g h\n";
  for (int rank = 7; rank >= 0; --rank) {
    std::cout << (rank + 1) << " ";
    for (int file = 0; file < 8; ++file) {
      int square = rank * 8 + file;
      Piece piece = get_piece_at(static_cast<ChessBoard::Square>(square));
      Color color = get_color_at(static_cast<ChessBoard::Square>(square));

      char symbol = '.';
      if (piece != ChessBoard::NONE) {
        const char* symbols = "pnbrqk";
        symbol = symbols[piece];
        if (color == ChessBoard::WHITE) symbol = toupper(symbol);
      }
      std::cout << symbol << " ";
    }
    std::cout << "\n";
  }
}

// Print the attack map for visualization
void ChessBoard::print_attack_map(Color color) const {
  Bitboard attacks = compute_attack_map(color);
  std::cout << (color == WHITE ? "White" : "Black") << " attack map:\n";
  std::cout << "  a b c d e f g h\n";
  for (int rank = 7; rank >= 0; --rank) {
    std::cout << (rank + 1) << " ";
    for (int file = 0; file < 8; ++file) {
      int square = rank * 8 + file;
      if (attacks & (1ULL << square)) {
        std::cout << "X ";
      } else {
        std::cout << ". ";
      }
    }
    std::cout << "\n";
  }
}

 // Test function to demonstrate king move validation with two rooks
void ChessBoard::test_king_with_two_rooks() {
  std::cout << "=== TEST 1: King trapped by two rooks ===\n";

  // Set up a position where white king is on e1 and black has rooks on a3 and h3
  set_custom_position();

  std::cout << "Position: White king on e1, Black rooks on a3 and h3\n";
  print_board();
  std::cout << "\n";

  print_attack_map(BLACK);
  std::cout << "\n";

  Square white_king = E2;

  std::cout << "Testing white king moves from e1:\n";
  std::vector<std::pair<std::string, Square>> test_moves = {
      {"e2-d1", D1}, {"e2-d2", D2}, {"e2-e1", E1}, {"e2-f1", F1}, {"e2-f3", F3}};

  for (const auto& [move_name, to_square] : test_moves) {
    bool legal = is_king_move_legal(white_king, to_square);
    std::cout << move_name << ": " << (legal ? "Legal" : "Illegal") << "\n";
  }
}

void ChessBoard::test_fen_load_queen_test() {
  std::cout << "\n=== TEST 2: Complex position ===\n";

  Square white_king = set_custom_position("r3kb1r/1p4pp/pQp2p2/1P1pp3/1P1P3P/3PPNPq/1B1N1P2/2R1K3 b KQkq - 0 1");
  print_board();
  std::cout << "\n";
  print_attack_map(BLACK);
  std::cout << "\n";

  std::cout << "Testing white king moves from c2:\n";
  std::vector<std::pair<std::string, Square>> test_moves = {
      {"e1-e1", E1}, {"e1-e2", E2}, {"e1-f1", F1}, {"e1-d1", D1}};

  for (const auto& [move_name, to_square] : test_moves) {
    bool legal = is_king_move_legal(white_king, to_square);
    std::cout << move_name << ": " << (legal ? "Legal" : "Illegal") << "\n";
  }
}

void ChessBoard::test_complex_position() {
  std::cout << "\n=== TEST 2: Complex position ===\n";

  // Set up a more complex position using FEN-like string
  set_custom_position("3k4/8/8/8/3r4/8/2K5/3r4");
  // Black king on d8, white king on c2, black rooks on d4 and d1

  std::cout << "Position: White king on c2, Black king on d8, Black rooks on d4 and d1\n";
  print_board();
  std::cout << "\n";

  print_attack_map(BLACK);
  std::cout << "\n";

  Square white_king = C2;

  std::cout << "Testing white king moves from c2:\n";
  std::vector<std::pair<std::string, Square>> test_moves = {
      {"c2-b1", B1}, {"c2-b2", B2}, {"c2-b3", B3}, {"c2-c1", C1},
      {"c2-c3", C3}, {"c2-d1", D1}, {"c2-d2", D2}, {"c2-d3", D3}};

  for (const auto& [move_name, to_square] : test_moves) {
    bool legal = is_king_move_legal(white_king, to_square);
    std::cout << move_name << ": " << (legal ? "Legal" : "Illegal") << "\n";
  }
}

// Test function for king in check with escape routes
void ChessBoard::test_king_escape() {
  std::cout << "\n=== TEST 3: King can escape check ===\n";

  // White king on e1, black rook on e8, but escape squares available
  set_custom_position("4r3/8/8/8/8/8/8/4K3");

  std::cout << "Position: White king on e1, Black rook on e8\n";
  print_board();
  std::cout << "\n";

  print_attack_map(BLACK);
  std::cout << "\n";

  Square white_king = E1;

  std::cout << "Testing white king moves from e1:\n";
  std::vector<std::pair<std::string, Square>> test_moves = {
      {"e1-d1", D1}, {"e1-d2", D2}, {"e1-e2", E2}, {"e1-f1", F1}, {"e1-f2", F2}, {"e1-e1", E1}};

  for (const auto& [move_name, to_square] : test_moves) {
    bool legal = is_king_move_legal(white_king, to_square);
    std::cout << move_name << ": " << (legal ? "Legal" : "Illegal") << "\n";
  }
}
