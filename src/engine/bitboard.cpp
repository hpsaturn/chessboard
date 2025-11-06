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
    pawn_attacks[WHITE][square] = 0;
    for (int i = 0; i < 2; ++i) {
      int new_x = x + white_pawn_dy[i];  // Note: swapped for little-endian
      int new_y = y + white_pawn_dx[i];
      if (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8) {
        pawn_attacks[WHITE][square] |= 1ULL << (new_y * 8 + new_x);
      }
    }

    pawn_attacks[BLACK][square] = 0;
    for (int i = 0; i < 2; ++i) {
      int new_x = x + black_pawn_dy[i];
      int new_y = y + black_pawn_dx[i];
      if (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8) {
        pawn_attacks[BLACK][square] |= 1ULL << (new_y * 8 + new_x);
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
  pieces[PAWN] = 0xFF00ULL | (0xFF000000000000ULL);  // ranks 2 and 7

  // Set up other pieces
  pieces[KNIGHT] = (1ULL << B1) | (1ULL << G1) | (1ULL << B8) | (1ULL << G8);
  pieces[BISHOP] = (1ULL << C1) | (1ULL << F1) | (1ULL << C8) | (1ULL << F8);
  pieces[ROOK] = (1ULL << A1) | (1ULL << H1) | (1ULL << A8) | (1ULL << H8);
  pieces[QUEEN] = (1ULL << D1) | (1ULL << D8);
  pieces[KING] = (1ULL << E1) | (1ULL << E8);

  // Set up colors
  colors[WHITE] = 0xFFFFULL;              // ranks 1 and 2
  colors[BLACK] = 0xFFFF000000000000ULL;  // ranks 7 and 8

  side_to_move = WHITE;

  // Initialize castling rights
  castling_rights[WHITE][0] = castling_rights[WHITE][1] = true;  // kingside, queenside
  castling_rights[BLACK][0] = castling_rights[BLACK][1] = true;
}

Piece ChessBoard::get_piece_at(Square square) const {
  Bitboard mask = 1ULL << square;
  for (int piece = PAWN; piece <= KING; ++piece) {
    if (pieces[piece] & mask) {
      return static_cast<Piece>(piece);
    }
  }
  return NONE;
}

Color ChessBoard::get_color_at(Square square) const {
  Bitboard mask = 1ULL << square;
  if (colors[WHITE] & mask) return WHITE;
  if (colors[BLACK] & mask) return BLACK;
  return BOTH;  // Empty square
}

Bitboard ChessBoard::compute_attack_map(Color attacking_color) const {
  Bitboard attack_map = 0;
  Bitboard occupancy = colors[WHITE] | colors[BLACK];

  // Pawn attacks
  Bitboard pawns = pieces[PAWN] & colors[attacking_color];
  while (pawns) {
    int square = __builtin_ctzll(pawns);
    attack_map |= pawn_attacks[attacking_color][square];
    pawns &= pawns - 1;  // Clear least significant bit
  }

  // Knight attacks
  Bitboard knights = pieces[KNIGHT] & colors[attacking_color];
  while (knights) {
    int square = __builtin_ctzll(knights);
    attack_map |= knight_attacks[square];
    knights &= knights - 1;
  }

  // King attacks
  Bitboard kings = pieces[KING] & colors[attacking_color];
  while (kings) {
    int square = __builtin_ctzll(kings);
    attack_map |= king_attacks[square];
    kings &= kings - 1;
  }

  // Bishop attacks (simplified - considers all moves without blocking)
  Bitboard bishops = pieces[BISHOP] & colors[attacking_color];
  while (bishops) {
    int square = __builtin_ctzll(bishops);
    attack_map |= bishop_attacks_base[square];
    bishops &= bishops - 1;
  }

  // Rook attacks (simplified)
  Bitboard rooks = pieces[ROOK] & colors[attacking_color];
  while (rooks) {
    int square = __builtin_ctzll(rooks);
    attack_map |= rook_attacks_base[square];
    rooks &= rooks - 1;
  }

  // Queen attacks (bishop + rook)
  Bitboard queens = pieces[QUEEN] & colors[attacking_color];
  while (queens) {
    int square = __builtin_ctzll(queens);
    attack_map |= bishop_attacks_base[square] | rook_attacks_base[square];
    queens &= queens - 1;
  }

  return attack_map;
}

bool ChessBoard::is_castling_move_legal(Square from, Square to) const {
  Color moving_color = get_color_at(from);
  int rank = (moving_color == WHITE) ? 0 : 7;  // rank 0 for white, 7 for black

  // Kingside castling
  if (to == G1 + rank * 8) {
    return is_kingside_castling_legal(moving_color);
  }
  // Queenside castling
  else if (to == C1 + rank * 8) {
    return is_queenside_castling_legal(moving_color);
  }

  return false;
}

bool ChessBoard::is_kingside_castling_legal(Color color) const {
  if (!castling_rights[color][0]) return false;  // No kingside castling right

  int rank = (color == WHITE) ? 0 : 7;
  Square king_square = static_cast<Square>(E1 + rank * 8);
  Square f_square = static_cast<Square>(F1 + rank * 8);
  Square g_square = static_cast<Square>(G1 + rank * 8);

  // Check if squares between king and rook are empty
  Bitboard occupancy = colors[WHITE] | colors[BLACK];
  if (occupancy & ((1ULL << f_square) | (1ULL << g_square))) {
    return false;
  }

  // Check if king is not in check and doesn't move through check
  Color opponent_color = (color == WHITE) ? BLACK : WHITE;
  Bitboard opponent_attacks = compute_attack_map(opponent_color);

  return !(opponent_attacks & (1ULL << king_square)) && !(opponent_attacks & (1ULL << f_square)) &&
         !(opponent_attacks & (1ULL << g_square));
}

bool ChessBoard::is_queenside_castling_legal(Color color) const {
  if (!castling_rights[color][1]) return false;  // No queenside castling right

  int rank = (color == WHITE) ? 0 : 7;
  Square king_square = static_cast<Square>(E1 + rank * 8);
  Square d_square = static_cast<Square>(D1 + rank * 8);
  Square c_square = static_cast<Square>(C1 + rank * 8);
  Square b_square = static_cast<Square>(B1 + rank * 8);

  // Check if squares between king and rook are empty
  Bitboard occupancy = colors[WHITE] | colors[BLACK];
  if (occupancy & ((1ULL << d_square) | (1ULL << c_square) | (1ULL << b_square))) {
    return false;
  }

  // Check if king is not in check and doesn't move through check
  Color opponent_color = (color == WHITE) ? BLACK : WHITE;
  Bitboard opponent_attacks = compute_attack_map(opponent_color);

  return !(opponent_attacks & (1ULL << king_square)) && !(opponent_attacks & (1ULL << d_square)) &&
         !(opponent_attacks & (1ULL << c_square));
}

 bool ChessBoard::is_king_move_legal(Square from, Square to) const {
   Color moving_color = get_color_at(from);
   Color opponent_color = (moving_color == WHITE) ? BLACK : WHITE;

   // Basic validation
   if (from == to) return false;
   if (get_piece_at(from) != KING) return false;

   // Check if destination is occupied by friendly piece
   Color dest_color = get_color_at(to);
   if (dest_color == moving_color) return false;

   // Check if move is within king's movement pattern
   Bitboard king_moves = king_attacks[from];
   if (!(king_moves & (1ULL << to))) {
     // Not a normal king move, check for castling
     return is_castling_move_legal(from, to);
   }

   // For normal moves: compute opponent's attack map and check if destination is safe
   Bitboard opponent_attacks = compute_attack_map(opponent_color);
   return !(opponent_attacks & (1ULL << to));
}

void ChessBoard::print_board() const {
  std::cout << "  a b c d e f g h\n";
  for (int rank = 7; rank >= 0; --rank) {
    std::cout << (rank + 1) << " ";
    for (int file = 0; file < 8; ++file) {
      int square = rank * 8 + file;
      Piece piece = get_piece_at(static_cast<Square>(square));
      Color color = get_color_at(static_cast<Square>(square));

      char symbol = '.';
      if (piece != NONE) {
        const char* symbols = "pnbrqk";
        symbol = symbols[piece];
        if (color == WHITE) symbol = toupper(symbol);
      }
      std::cout << symbol << " ";
    }
    std::cout << "\n";
  }
}
