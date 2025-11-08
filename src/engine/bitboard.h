#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <array>
#include <bitset>
#include <iostream>
#include <vector>


class ChessBoard {
private:
    // Bitboard definitions (Little-endian rank-file mapping)
 using Bitboard = uint64_t;

 // Piece types
 enum Piece { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NONE };

 // Colors
 enum Color { WHITE, BLACK, BOTH };

 // Squares
 enum Square {
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8, NO_SQ
 };

 // Precomputed attack tables
 std::array<Bitboard, 64> knight_attacks;
 std::array<Bitboard, 64> king_attacks;
 std::array<std::array<Bitboard, 64>, 2> pawn_attacks;  // [color][square]

 // Magic bitboard tables for sliding pieces (simplified version)
 std::array<Bitboard, 64> rook_attacks_base;
 std::array<Bitboard, 64> bishop_attacks_base;

 // Board state
 std::array<Bitboard, 6> pieces;  // [PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING]
 std::array<Bitboard, 2> colors;  // [WHITE, BLACK]
 Color side_to_move;

 // Castling rights
 bool castling_rights[2][2];  // [color][kingside/queenside]

public:
 ChessBoard() {
   initialize_attack_tables();
   set_initial_position();
 }

private:
 void initialize_attack_tables();
 void set_initial_position();

 Piece get_piece_at(Square square) const;
 Color get_color_at(Square square) const;
 Bitboard get_sliding_attacks(Square square, Piece piece) const;
 Bitboard compute_attack_map(Color attacking_color) const;

 bool is_castling_move_legal(Square from, Square to) const;
 bool is_kingside_castling_legal(Color color) const;
 bool is_queenside_castling_legal(Color color) const;
 Square find_king_square(Color color) const;
 void make_move_on_board(Square from, Square to);

public:
 typedef Square Square;
 typedef Piece Piece;
 typedef Color Color;
 bool is_king_move_legal(Square from, Square to) const;
 bool is_king_move_legal(int from_row, int from_col, int to_row, int to_col) const;
 bool is_king_in_check(Color king_color) const;
 bool is_king_in_check_row_col(int king_row, int king_col) const;
 bool is_current_player_in_check() const;
 bool would_move_leave_king_in_check(Square from, Square to) const;
 Square from_row_col(int row, int col) const;
 std::pair<int, int> to_row_col(Square square) const;

 void set_piece(Square square, Piece piece, Color color);
 Square set_custom_position(const std::string& fen = "");
 
 void print_board() const;
 void print_attack_map(Color color) const;

 void test_complex_position();
 void test_king_escape();
 void test_king_with_two_rooks();
 void test_fen_load_queen_test();
};

#endif // BITBOARD_H

