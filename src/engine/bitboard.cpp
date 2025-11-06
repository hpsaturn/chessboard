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

  // Bishop attacks (simplified - considers all moves without blocking)
  ChessBoard::Bitboard bishops = pieces[ChessBoard::BISHOP] & colors[attacking_color];
  while (bishops) {
    int square = __builtin_ctzll(bishops);
    attack_map |= bishop_attacks_base[square];
    bishops &= bishops - 1;
  }

  // Rook attacks (simplified)
  ChessBoard::Bitboard rooks = pieces[ChessBoard::ROOK] & colors[attacking_color];
  while (rooks) {
    int square = __builtin_ctzll(rooks);
    attack_map |= rook_attacks_base[square];
    rooks &= rooks - 1;
  }

  // Queen attacks (bishop + rook)
  ChessBoard::Bitboard queens = pieces[ChessBoard::QUEEN] & colors[attacking_color];
  while (queens) {
    int square = __builtin_ctzll(queens);
    attack_map |= bishop_attacks_base[square] | rook_attacks_base[square];
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

 bool ChessBoard::is_king_move_legal(ChessBoard::Square from, Square to) const {
   ChessBoard::Color moving_color = get_color_at(from);
   ChessBoard::Color opponent_color = (moving_color == ChessBoard::WHITE) ? ChessBoard::BLACK : ChessBoard::WHITE;

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
