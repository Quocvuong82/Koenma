#ifndef POSTION_H
#define POSTION_H
#include <cassert>
#include <cstddef>
#include <string>

#include "bitboard.h"
#include "types.h"
#include "data.h"
#include "movegen.h"

class Position;

struct CheckInfo {

	explicit CheckInfo(const Position&);

	Bitboard dcCandidates;
	Bitboard dcCannonCandidates;
	Bitboard pinned;
	Bitboard checkSq[PIECE_TYPE_NB];
	Square ksq;
};

/// The StateInfo struct stores information needed to restore a Position
/// object to its previous state when we retract a move. Whenever a move
/// is made on the board (by calling Position::do_move), a StateInfo
/// object must be passed as a parameter.

struct StateInfo {

	Bitboard checkersBB;
	PieceType capturedType;
	StateInfo* previous;
};

class Position
{
public:
	Position(){};

	const Bitboard& pieces() const;
	Bitboard pieces(Color c) const;
	Bitboard pieces(Color c, PieceType pt) const;
    Square king_square(Color c) const;

	Color side_to_move() const;

	template<PieceType Pt> 
	const Square* list(Color c) const;

	template<PieceType> Bitboard attacks_from(Square s) const;
	template<PieceType> Bitboard attacks_from(Square s, Color c) const;
	Bitboard attackers_to(Square s) const;
	Bitboard checkers(Color checker, Square ksq) const;
	

	Bitboard pinned_pieces(Color c) const;
	Bitboard check_blockers(Color c, Color kingColor) const;
	Bitboard discovered_check_candidates() const;
	Bitboard discovered_cannon_check_candidates() const;
	Bitboard discovered_cannon_face_king() const;
	Bitboard checkers() const;

	bool gives_check(Move m, const CheckInfo& ci)const;

	bool is_empty(Square s) const;
	Piece piece_on(Square s) const;

	// Text input/output
	void set(const std::string& fen);
	const std::string fen() const;
	const std::string pretty(Move m = MOVE_NONE) const;

	void clear();

	void put_piece(Square s, Color c, PieceType pt);
	void remove_piece(Square s, Color c, PieceType pt);
	void move_piece(Square from, Square to, Color c, PieceType pt);
	Piece piece_moved(Move m) const;

	bool pos_is_ok(int* failedStep = NULL) const;

	void do_move(Move m, StateInfo& newSt, const CheckInfo& ci, bool moveIsCheck);
	void undo_move(Move m);

	bool in_check(Color c);

	bool legal(Move m, const Bitboard& pinned) ;
private:

    Bitboard occupied;

	Piece    board[SQUARE_NB];
	Bitboard byTypeBB[PIECE_TYPE_NB];
	Bitboard byColorBB[COLOR_NB];

	int      pieceCount[COLOR_NB][PIECE_TYPE_NB];
	Square   pieceList[COLOR_NB][PIECE_TYPE_NB][16];
	int      index[SQUARE_NB];

	Color    sideToMove;
    int      gamePly;

	StateInfo* st;
	StateInfo startState;
};

inline const Bitboard& Position::pieces() const
{
   return occupied;
}
inline Bitboard Position::pieces(Color c) const
{
   return byColorBB[c];
}
inline Bitboard Position::pieces(Color c, PieceType pt) const
{
   return byColorBB[c] & byTypeBB[pt];
}
inline Color Position::side_to_move() const
{
   return sideToMove;
}
template<PieceType Pt>
inline const Square* Position::list(Color c) const{
	return pieceList[c][Pt];
}
template<>
inline Bitboard Position::attacks_from<ROOK>(Square s) const
{
	return rook_attacks_bb(s, occupied);
}

template<>
inline Bitboard Position::attacks_from<CANNON>(Square s) const
{
	return cannon_control_bb(s, occupied);
}
template<>
inline Bitboard Position::attacks_from<KNIGHT>(Square s) const
{
	return knight_attacks_bb(s,occupied);
}
template<>
inline Bitboard Position::attacks_from<BISHOP>(Square s) const {
	return bishop_attacks_bb(s,occupied);
}
template<>
inline Bitboard Position::attacks_from<ADVISOR>(Square s) const {
	return AdvisorAttackTo[s];
}
template<>
inline Bitboard Position::attacks_from<KING>(Square s) const {
	return KingAttackTo[s];
}
template<>
inline Bitboard Position::attacks_from<PAWN>(Square s, Color c) const {
	return PawnAttackTo[c][s];
}

inline void Position::put_piece(Square s, Color c, PieceType pt) {

	board[s] = make_piece(c, pt);
	byTypeBB[ALL_PIECES] |= s;
	byTypeBB[pt] |= s;
	byColorBB[c] |= s;
	index[s] = pieceCount[c][pt]++;
	pieceList[c][pt][index[s]] = s;

	occupied      ^= s;
	
}

inline void Position::move_piece(Square from, Square to, Color c, PieceType pt) {

	// index[from] is not updated and becomes stale. This works as long
	// as index[] is accessed just by known occupied squares.
	Bitboard from_to_bb = SquareBB[from] ^ SquareBB[to];
	byTypeBB[ALL_PIECES] ^= from_to_bb;
	byTypeBB[pt] ^= from_to_bb;
	byColorBB[c] ^= from_to_bb;
	board[from] = NO_PIECE;
	board[to] = make_piece(c, pt);
	index[to] = index[from];
	pieceList[c][pt][index[to]] = to;

	occupied      ^= from;
	occupied      ^= to;
}

inline void Position::remove_piece(Square s, Color c, PieceType pt) {

	// WARNING: This is not a reversible operation. If we remove a piece in
	// do_move() and then replace it in undo_move() we will put it at the end of
	// the list and not in its original place, it means index[] and pieceList[]
	// are not guaranteed to be invariant to a do_move() + undo_move() sequence.
	byTypeBB[ALL_PIECES] ^= s;
	byTypeBB[pt] ^= s;
	byColorBB[c] ^= s;
	/* board[s] = NO_PIECE; */ // Not needed, will be overwritten by capturing
	Square lastSquare = pieceList[c][pt][--pieceCount[c][pt]];
	index[lastSquare] = index[s];
	pieceList[c][pt][index[lastSquare]] = lastSquare;
	pieceList[c][pt][pieceCount[c][pt]] = SQ_NONE;

	occupied      ^= s;

}

inline bool Position::is_empty(Square s) const {
	return board[s] == NO_PIECE;
}

inline Piece Position::piece_on(Square s) const {
	return board[s];
}

inline Piece Position::piece_moved(Move m) const {
	return board[from_sq(m)];
}
inline Square Position::king_square(Color c) const {
	return pieceList[c][KING][0];
}
inline Bitboard Position::pinned_pieces(Color c) const {
	return check_blockers(c, c);
}
inline Bitboard Position::checkers() const {
	return st->checkersBB;
}

#endif