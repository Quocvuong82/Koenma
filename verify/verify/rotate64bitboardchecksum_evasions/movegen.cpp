#include "movegen.h"
#include "position.h"
#include "data.h"


#define SERIALIZE(b) while(b) mlist->mlist[mlist->size++].move = make_move(from, pop_lsb(&b))

//inline Square pop_lsb32(uint32_t& b){
//    
//	Square s = BSFTable[(((b^(b-1))* DeBruijn_32)>>27)];
//	
//	b &= b-1;
//
//	return s;
//}

//#define SERIALIZE(b) while(b.bb[0]){ mlist->mlist[mlist->size++].move = make_move(from, pop_lsb32(b.bb[0]));} while(b.bb[1]){ mlist->mlist[mlist->size++].move = make_move(from, Square(pop_lsb32(b.bb[1]) + 32));} while(b.bb[2]){ mlist->mlist[mlist->size++].move = make_move(from, Square(pop_lsb32(b.bb[2]) + 64));}


static const char* PieceMap[COLOR_NB] = { " PBANCRK", " pbancrk" };

static std::string piece_to_chinese(char p)
{
	switch(p)
	{
	case ' ': return std::string(" ");
	case 'P': return std::string("��");
	case 'p': return std::string("��");
	case 'B': return std::string("��");
	case 'b': return std::string("��");
	case 'A': return std::string("ʿ");
	case 'a': return std::string("��");
	case 'N': return std::string("��");
	case 'n': return std::string("�R");
	case 'C': return std::string("��");
	case 'c': return std::string("��");
	case 'R': return std::string("��");
	case 'r': return std::string("܇");
	case 'K': return std::string("˧");
	case 'k': return std::string("��");
	}

	return std::string();
}

std::string move_to_chinese(const Position& pos, Move m)
{
	Color us = pos.side_to_move();
	Square from = from_sq(m);
	Square to = to_sq(m);
	Piece pc = pos.piece_on(from);
	PieceType pt = type_of(pc);

	char p = PieceMap[us][pt];

	std::string move = "\n";

	move += piece_to_chinese(p);
	move += square_to_string(from);
	move += square_to_string(to);

	return move;
}
void gen_evasion_moves(Position& pos, MoveList* mlist);
void gen_all_evasion_moves(Position& pos, MoveList* mlist, Color us, const Bitboard& target, Square exclued);
void gen_nonevasion_moves(Position& pos, MoveList* mlist);

bool move_is_legal(Position& pos, ExtMove move)
{
   
	//for debug
	//return true;
	//
	//bool legal;
	
	//undo_t undo[1];
	//pos.do_move(move.move, undo);

    //legal = !pos.in_check(~pos.side_to_move());

	//pos.undo_move(move.move, undo);

	//return legal;


	
	Move m = move.move;
	assert(is_ok(m));	

	Color us = pos.side_to_move();
	Square from = from_sq(m);
	Square to   = to_sq(m);

	assert(color_of(pos.piece_moved(m)) == us);
	assert(pos.piece_on(pos.king_square(us)) == make_piece(us, KING));

	PieceType pfr = type_of(pos.piece_on(from));
	PieceType pto= type_of(pos.piece_on(to));

	Bitboard pawns   = pos.pieces(~us, PAWN);
	Bitboard knights = pos.pieces(~us, KNIGHT);
	Bitboard cannons = pos.pieces(~us, CANNON);
	Bitboard rooks = pos.pieces(~us, ROOK);

	Bitboard  occ    = pos.pieces();
	Bitboard  occl90 = pos.piecesl90();

	occl90 ^= square_rotate_l90(from);
	occ    ^= from;

	if(pto == NO_PIECE_TYPE)
	{
		occl90 ^= square_rotate_l90(to);
		occ    ^= to;
	}

	Square ksq = pos.king_square(us);
	if(ksq == from)
		ksq = to;

	if (pto != NO_PIECE_TYPE)
	{
		switch(pto)
		{
		case PAWN:
           pawns ^= to;
		   break;
		case KNIGHT:
           knights ^= to;
		   break;
		case ROOK:
           rooks ^= to;
		    break;
		case CANNON:
           cannons ^= to;
            break;
		}
	}

	//�������ַ�ʽʱ����һ��

	//if((RookAttackMask[ksq]& cannons) &&(cannon_control_bb(ksq, occ,occl90) & cannons)) return false;
	//if((RookAttackMask[ksq]& rooks) && (rook_attacks_bb(ksq,occ,occl90)& rooks) ) return false;
	//if((KnightAttackMask[ksq]&knights) && (knight_attacks_from(ksq, occ,occl90)&knights) ) return false;
	//if((pawn_attacks_from(~us,ksq) & pawns) ) return false;

	//if((RookAttackMask[ksq]& pos.king_square(~us)) && (rook_attacks_bb(ksq,occ,occl90)& pos.king_square(~us))) return false;//����

	if (bitboard_and_bitboard(RookAttackMask[ksq],cannons) && bitboard_and_bitboard(cannon_control_bb(ksq, occ,occl90) , cannons))
	{
		return false;
	}
	if (bitboard_and_bitboard(RookAttackMask[ksq],rooks) && bitboard_and_bitboard(rook_attacks_bb(ksq,occ,occl90), rooks))
	{
		return false;
	}
	if (bitboard_and_bitboard(KnightAttackMask[ksq],knights) && bitboard_and_bitboard(knight_attacks_from(ksq, occ,occl90),knights))
	{
		return false;
	}
	if (bitboard_and_bitboard(pawn_attacks_from(~us,ksq),pawns))
	{
		return false;
	}
	if (bitboard_and_square(RookAttackMask[ksq],pos.king_square(~us)) && bitboard_and_square(rook_attacks_bb(ksq,occ,occl90),pos.king_square(~us)))
	{
		return false;
	}

	return true;
}

static void gen_rook_moves(const Position& pos, MoveList* mlist)
{
	Color    us     = pos.side_to_move();
	Bitboard target = ~pos.pieces(us);
	const Square* pl = pos.list<ROOK>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		Bitboard att = pos.attacks_from<ROOK>(from)&target;

		SERIALIZE(att);
	}
}
static void gen_knight_moves(const Position& pos, MoveList* mlist)
{
	Color    us     = pos.side_to_move();
	Bitboard target = ~pos.pieces(us);
	const Square* pl = pos.list<KNIGHT>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		Bitboard att = pos.attacks_from<KNIGHT>(from)&target;

		SERIALIZE(att);
	}
}
static void gen_cannon_moves(const Position& pos, MoveList* mlist)
{
	Color    us     = pos.side_to_move();
	Bitboard target = pos.pieces(~us);
	Bitboard empty  = ~pos.pieces();
	const Square* pl = pos.list<CANNON>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		Bitboard att = pos.attacks_from<CANNON>(from)&target;

		SERIALIZE(att);

		Bitboard natt = pos.attacks_from<ROOK>(from)&empty;
		SERIALIZE(natt);
	}
}
//�������ַ�ʽ�ǲ��еģ���attacks_fromЧ�ʸ�
static void gen_pawn_moves(const Position& pos, MoveList* mlist)
{
	Color    us     = pos.side_to_move();
	Bitboard target = ~pos.pieces(us);
	Bitboard pawns  = pos.pieces(us, PAWN);

	const Square   Up       = (us == WHITE ? DELTA_N  : DELTA_S);
	const Square   Right    = (us == WHITE ? DELTA_E : DELTA_W);
	const Square   Left     = (us == WHITE ? DELTA_W : DELTA_E);

	const Bitboard MaskBB   =  PawnMask[us];

	Bitboard attup = shift_bb(pawns,Up) & MaskBB & target;
	Bitboard attleft = shift_bb(pawns,Left) & MaskBB & target;
	Bitboard attright= shift_bb(pawns,Right) & MaskBB & target;

	while(attup){
		Square to = pop_lsb(&attup);
		Square from = to - (Up);		
		mlist->mlist[mlist->size++].move = make_move(from, to);
	}

	while(attleft){
		Square to = pop_lsb(&attleft);
		Square from = to - (Left);		
		mlist->mlist[mlist->size++].move = make_move(from, to);
	}

	while(attright){
		Square to = pop_lsb(&attright);
		Square from = to - (Right);		
		mlist->mlist[mlist->size++].move = make_move(from, to);
	}
}
static void gen_bishop_moves(const Position& pos, MoveList* mlist)
{
	Color    us     = pos.side_to_move();
	Bitboard target = ~pos.pieces(us);
	const Square* pl = pos.list<BISHOP>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		Bitboard att = pos.attacks_from<BISHOP>(from)&target;

		SERIALIZE(att);
	}
}
static void gen_advisor_moves(const Position& pos, MoveList* mlist)
{
	Color    us     = pos.side_to_move();
	Bitboard target = ~pos.pieces(us);
	const Square* pl = pos.list<ADVISOR>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		Bitboard att = pos.attacks_from<ADVISOR>(from)&target;

		SERIALIZE(att);
	}
}
static void gen_king_moves(const Position& pos, MoveList* mlist)
{
	Color    us     = pos.side_to_move();
	Bitboard target = ~pos.pieces(us);
	const Square* pl = pos.list<KING>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		Bitboard att = pos.attacks_from<KING>(from)&target;

		SERIALIZE(att);
	}
}
//�ú���û��bug
void gen_legal_moves(Position& pos, MoveList* mlist)
{
	gen_rook_moves(pos, mlist);
	gen_knight_moves(pos, mlist);
	gen_cannon_moves(pos, mlist);
	gen_pawn_moves(pos, mlist);
	gen_bishop_moves(pos, mlist);
	gen_advisor_moves(pos, mlist);
	gen_king_moves(pos, mlist);

	uint32_t n = mlist->size;
	for (uint32_t i = 0; i < n; )
	{
		if (!move_is_legal(pos, mlist->mlist[i]))
		{
			mlist->mlist[i].move = mlist->mlist[--n].move;
		}
		else
		{
			++i;
		}
	}
	mlist->size = n;
}
//�ú���Ч�ʸߣ������溯�����һ��
void gen_legal_moves_with_evasions(Position& pos, MoveList* mlist)
{
	//gen_legal_moves(pos, mlist);
	Bitboard pinned = pos.pinned_pieces(pos.side_to_move());
	Square ksq = pos.king_square(pos.side_to_move());
	Bitboard cannonsfib = pos.discovered_cannon_face_king();

	if (pos.checkers()) { 

		gen_evasion_moves(pos, mlist);
		//gen_nonevasion_moves(pos, mlist);
	}
	else {
		gen_nonevasion_moves(pos, mlist);
	}

	uint32_t n = mlist->size;
	for (uint32_t i = 0; i < n; )
	{
		
		//////��ǣ�ƻ�king�ƶ�
		//if ( (pinned || from_sq(mlist->mlist[i].move) == ksq) && !move_is_legal(pos, mlist->mlist[i])/*!pos.legal(mlist->mlist[i].move, pinned)*/) {
		//	mlist->mlist[i].move = mlist->mlist[--n].move;
		//}
		//else
		//{
		//	++i;
		//}

		//����ּ���������ˣ������������Ч��
		if ( (cannonsfib & to_sq(mlist->mlist[i].move)) && from_sq(mlist->mlist[i].move) != ksq) {
			mlist->mlist[i].move = mlist->mlist[--n].move;
		}
		//else if ((pinned || from_sq(mlist->mlist[i].move) == ksq) && !pos.legal(mlist->mlist[i].move, pinned)) {
		//	mlist->mlist[i].move = mlist->mlist[--n].move;
		//}
		else if ( (pinned & from_sq(mlist->mlist[i].move)) && !move_is_legal(pos, mlist->mlist[i])) {
			mlist->mlist[i].move = mlist->mlist[--n].move;
		}
		else if ( (/*pinned || */from_sq(mlist->mlist[i].move) == ksq /*|| (cannonsfib & to_sq(mlist->mlist[i].move))*/) && !move_is_legal(pos, mlist->mlist[i])) {
			mlist->mlist[i].move = mlist->mlist[--n].move;
		}
		else {

			//if (!move_is_legal(pos, mlist->mlist[i])) {
			//	printf("%s\n", pos.pretty().c_str());
			//	printf("%s ", move_to_chinese(pos, mlist->mlist[i].move).c_str());
			//	pos.checkers().print(stdout);

			//	pos.checkers(~pos.side_to_move(), pos.king_square(pos.side_to_move())).print(stdout);;
			//	getchar();
			//}

			++i;

		}

			//if (!move_is_legal(pos, mlist->mlist[i]))
			//{
			//	mlist->mlist[i].move = mlist->mlist[--n].move;
			//}
			//else
			//{
			//	++i;
			//}
		
		
	}
	mlist->size = n;
}
void gen_evasion_moves(Position& pos, MoveList* mlist)
{
	assert(pos.checkers());	
	
	Color us = pos.side_to_move();
	Square ksq = pos.king_square(us);
	Bitboard sliderAttacks;
	Bitboard sliders = pos.checkers() & ~pos.pieces(~us, KNIGHT) & ~pos.pieces(~us, PAWN);

	Bitboard occ = pos.pieces();
	Bitboard occl90 = pos.piecesl90();
	while (sliders)
	{		
		Square  checksq = pop_lsb(&sliders);

		switch (type_of(pos.piece_on(checksq)))
		{
		case ROOK:
			sliderAttacks |= RookAttackMask[checksq];
			break;
		case CANNON:
			sliderAttacks |= cannon_control_bb(checksq, occ, occl90);
			sliderAttacks |= cannon_super_control_bb(checksq, occ, occl90);
			break;
		default:
			break;
		}
	}

	Bitboard b = pos.attacks_from<KING>(ksq)&~pos.pieces(us)&~sliderAttacks;
	Square from = ksq;

	SERIALIZE(b);

	
	//rook �ԣ��� cannon �Ե��� knight �Ե� pawn ��

	Bitboard target;

	if (pos.checkers().more_than_one())
	{
		assert(pop_count(pos.checkers()) > 1);

		//printf("%s\n", pos.pretty().c_str());

		//���������
		//����ӽ���ֻ�ܵ�
		//������Ľ�������ȷ
		//������Ĳ�����Ȼ����ݼ��ϲ����з������Ϸ�Χ����ܴ�
		//��Ϊ�ཫ�������ϡ�٣����Բ���ȫ���Ϸ��з�����

		int size = mlist->size;
		gen_rook_moves(pos, mlist);
		gen_knight_moves(pos, mlist);
		gen_cannon_moves(pos, mlist);
		gen_pawn_moves(pos, mlist);
		gen_bishop_moves(pos, mlist);
		gen_advisor_moves(pos, mlist);
		int nn = mlist->size;
		for (uint32_t i = size; i < nn; )
		{
			if (!move_is_legal(pos, mlist->mlist[i]))
			{
				mlist->mlist[i].move = mlist->mlist[--nn].move;
			}
			else
			{
				++i;
			}
		}
		mlist->size = nn;
		//��,ÿ�������ϵĽ���
		//target = ~target;

		//Bitboard checkers = pos.checkers();
		//while (checkers)
		//{
		//	Square  checksq = pop_lsb(&sliders);
		//	switch (type_of(pos.piece_on(checksq)))
		//	{
		//	case ROOK:
		//		target &= between_bb(checksq, ksq);
		//		break;
		//	case CANNON:
		//		target &= between_bb(checksq, ksq)&~occ;
		//		break;
		//	case KNIGHT:
		//		target &= KnightLeg[checksq] & KnightEye[ksq];
		//		break;
		//	default:
		//		break;
		//	}
		//}

	}
	else
	{
		assert(pop_count(pos.checkers()) == 1);

		//

		//�ԣ���
		Square checksq = lsb(pos.checkers());
		PieceType pt = type_of(pos.piece_on(checksq));
		if (pt == CANNON) {
			MoveList list;
#if 0			
			gen_rook_moves(pos, &list);
			gen_knight_moves(pos, &list);
			gen_cannon_moves(pos, &list);
			gen_pawn_moves(pos, &list);
			gen_bishop_moves(pos, &list);
			gen_advisor_moves(pos, &list);
			//gen_king_moves(pos, mlist);

			uint32_t n = (&list)->size;
			for (uint32_t i = 0; i < n; )
			{
				if (!move_is_legal(pos, (&list)->mlist[i]))
				{
					(&list)->mlist[i].move = (&list)->mlist[--n].move;
				}
				else
				{
					++i;
				}
			}
			(&list)->size = n;
#endif


			int nSize = mlist->size;
			//��
			Square midsq = lsb(between_bb(checksq, ksq)&occ);
			if (color_of(pos.piece_on(midsq)) == us) {
				
				switch (type_of(pos.piece_on(midsq))) {
				case ROOK: {
					Bitboard att = pos.attacks_from<ROOK>(midsq)&(~between_bb(checksq, ksq)) & ~pos.pieces(us);
					Square from = midsq;
					SERIALIZE(att);
					break;
				}
				case CANNON: {
					Bitboard att = pos.attacks_from<CANNON>(midsq)&pos.pieces(~us);
					Square from = midsq;
					SERIALIZE(att);

					Bitboard natt = pos.attacks_from<ROOK>(midsq)&(~pos.pieces())&(~between_bb(checksq, ksq));
					SERIALIZE(natt);
					break;
				}

				case KNIGHT: {
					Bitboard att = pos.attacks_from<KNIGHT>(midsq)& ~pos.pieces(us);
					Square from = midsq;
					SERIALIZE(att);
					break;
				}

				case PAWN: {
					Bitboard att = pos.attacks_from<PAWN>(midsq, us)& ~pos.pieces(us)&(~between_bb(checksq, ksq));
					Square from = midsq;
					SERIALIZE(att);
					break;
				}

				case BISHOP: {
					Bitboard att = pos.attacks_from<BISHOP>(midsq)&~pos.pieces(us);
					Square from = midsq;
					SERIALIZE(att);
					break;
				}

				case ADVISOR: {
					Bitboard att = pos.attacks_from<ADVISOR>(midsq)&~pos.pieces(us);
					Square from = midsq;
					SERIALIZE(att);
					break;
				}

				case KING:
				default:
					break;
				}
			}

			//�� ��
			target = (between_bb(checksq, ksq)&~occ) | checksq;
			if (target) {
				gen_all_evasion_moves(pos, mlist, us, target, midsq);
			}
#if 0			
			//int nn = mlist->size;
			//for (uint32_t i = nSize; i < nn; )
			//{
			//	if (!move_is_legal(pos, mlist->mlist[i]))
			//	{
			//		mlist->mlist[i].move = mlist->mlist[--nn].move;
			//	}
			//	else
			//	{
			//		++i;
			//	}
			//}
			//mlist->size = nn;
			//
			//if ((&list)->size != (mlist->size - nSize))
			//{
			//	printf("%s\n", pos.pretty().c_str());

			//	for (int k = 0; k < (&list)->size; ++k) {
			//		printf("%s ", move_to_chinese(pos, (&list)->mlist[k].move).c_str());
			//	}
			//	printf("\n");

			//	for (int k = 0; k < (mlist)->size; ++k) {
			//		printf("%s ", move_to_chinese(pos, (mlist)->mlist[k].move).c_str());
			//	}
			//	printf("\n");

			//	

			//	assert((&list)->size == (mlist->size-nSize));
			//}
#endif

		}
		else {
			MoveList list;
#if 0			
			gen_rook_moves(pos, &list);
			gen_knight_moves(pos, &list);
			gen_cannon_moves(pos, &list);
			gen_pawn_moves(pos, &list);
			gen_bishop_moves(pos, &list);
			gen_advisor_moves(pos, &list);
			//gen_king_moves(pos, mlist);

			uint32_t n = (&list)->size;
			for (uint32_t i = 0; i < n; )
			{
				if (!move_is_legal(pos, (&list)->mlist[i]))
				{
					(&list)->mlist[i].move = (&list)->mlist[--n].move;
				}
				else
				{
					++i;
				}
			}
			(&list)->size = n;
#endif
			int nSize = mlist->size;
			switch (pt)
			{
			case ROOK:
				target |= between_bb(checksq, ksq) | checksq;
				break;
			//case CANNON:
			//	target |= (between_bb(checksq, ksq)&~occ) | checksq;
			//	break;
			case KNIGHT:
				//knight������king���۵Ĳ���checksq������
				target |= (KnightLeg[checksq] & KnightEye[ksq]) | checksq;
				break;
			case PAWN:
				target |= checksq;
				break;
			default:
				break;
			}

			if(target) 	  gen_all_evasion_moves(pos, mlist, us, target, SQ_NONE);

			//int nn = mlist->size;
			//for (uint32_t i = nSize; i < nn; )
			//{
			//	if (!move_is_legal(pos, mlist->mlist[i]))
			//	{
			//		mlist->mlist[i].move = mlist->mlist[--nn].move;
			//	}
			//	else
			//	{
			//		++i;
			//	}
			//}
			//mlist->size = nn;

			//if ((&list)->size != (mlist->size - nSize))
			//{
			//	printf("%s\n", pos.pretty().c_str());

			//	for (int k = 0; k < (&list)->size; ++k) {
			//		printf("%s ", move_to_chinese(pos, (&list)->mlist[k].move).c_str());
			//	}
			//	printf("\n");

			//	for (int k = 0; k < (mlist)->size; ++k) {
			//		printf("%s ", move_to_chinese(pos, (mlist)->mlist[k].move).c_str());
			//	}
			//	printf("\n");



			//	assert((&list)->size == (mlist->size - nSize));
			//}
		}		
	}	
}

void gen_rook_evasion_moves(Position& pos, MoveList* mlist, Color us, const Bitboard& target, Square exclued)
{
	//Color    us = pos.side_to_move();
	//Bitboard target = ~pos.pieces(us);
	const Square* pl = pos.list<ROOK>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		if (from == exclued)	continue;

		Bitboard att = pos.attacks_from<ROOK>(from)&target;

		SERIALIZE(att);
	}
}
void gen_cannon_evasion_moves(Position& pos, MoveList* mlist, Color us, const Bitboard& target, Square exclued)
{
	//Color    us = pos.side_to_move();
	//Bitboard target = pos.pieces(~us);
	Bitboard empty = ~pos.pieces();
	const Square* pl = pos.list<CANNON>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		if (from == exclued)	continue;

		Bitboard att = pos.attacks_from<CANNON>(from)&target&pos.pieces(~us);

		SERIALIZE(att);

		Bitboard natt = pos.attacks_from<ROOK>(from)&empty&target;
		SERIALIZE(natt);
	}
}
void gen_knight_evasion_moves(Position& pos, MoveList* mlist, Color us, const Bitboard& target, Square exclued)
{
	//Color    us = pos.side_to_move();
	//Bitboard target = ~pos.pieces(us);
	const Square* pl = pos.list<KNIGHT>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		if (from == exclued)	continue;

		Bitboard att = pos.attacks_from<KNIGHT>(from)&target;

		SERIALIZE(att);
	}
}
void gen_pawn_evasion_moves(Position& pos, MoveList* mlist, Color us, const Bitboard& target, Square exclued)
{
	//Color    us = pos.side_to_move();
	//Bitboard target = ~pos.pieces(us);
	Bitboard pawns = pos.pieces(us, PAWN);

	const Square   Up = (us == WHITE ? DELTA_N : DELTA_S);
	const Square   Right = (us == WHITE ? DELTA_E : DELTA_W);
	const Square   Left = (us == WHITE ? DELTA_W : DELTA_E);

	const Bitboard MaskBB = PawnMask[us];

	Bitboard attup = shift_bb(pawns, Up) & MaskBB & target;
	Bitboard attleft = shift_bb(pawns, Left) & MaskBB & target;
	Bitboard attright = shift_bb(pawns, Right) & MaskBB & target;

	while (attup) {
		Square to = pop_lsb(&attup);
		Square from = to - (Up);

		if (from == exclued)	continue;

		mlist->mlist[mlist->size++].move = make_move(from, to);
	}

	while (attleft) {
		Square to = pop_lsb(&attleft);
		Square from = to - (Left);

		if (from == exclued)	continue;

		mlist->mlist[mlist->size++].move = make_move(from, to);
	}

	while (attright) {
		Square to = pop_lsb(&attright);
		Square from = to - (Right);

		if (from == exclued)	continue;
		mlist->mlist[mlist->size++].move = make_move(from, to);
	}
}
void gen_bishop_evasion_moves(Position& pos, MoveList* mlist, Color us, const Bitboard& target, Square exclued)
{
	//Color    us = pos.side_to_move();
	//Bitboard target = ~pos.pieces(us);
	const Square* pl = pos.list<BISHOP>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		if (from == exclued)	continue;

		Bitboard att = pos.attacks_from<BISHOP>(from)&target;

		SERIALIZE(att);
	}
}
void gen_advisor_evasion_moves(Position& pos, MoveList* mlist, Color us, const Bitboard& target, Square exclued)
{
	//Color    us = pos.side_to_move();
	//Bitboard target = ~pos.pieces(us);
	const Square* pl = pos.list<ADVISOR>(us);
	for (Square from = *pl; from != SQ_NONE; from = *++pl)
	{
		if (from == exclued)	continue;

		Bitboard att = pos.attacks_from<ADVISOR>(from)&target;

		SERIALIZE(att);
	}
}
void gen_all_evasion_moves(Position& pos, MoveList* mlist, Color us, const Bitboard& target, Square exclued)
{
	gen_rook_evasion_moves(pos, mlist, us, target, exclued);
	gen_cannon_evasion_moves(pos, mlist, us, target, exclued);
	gen_knight_evasion_moves(pos, mlist, us, target, exclued);
	gen_pawn_evasion_moves(pos, mlist, us, target, exclued);
	gen_bishop_evasion_moves(pos, mlist, us, target, exclued);
	gen_advisor_evasion_moves(pos, mlist, us, target, exclued);
}
void gen_nonevasion_moves(Position& pos, MoveList* mlist)
{
	gen_rook_moves(pos, mlist);
	gen_knight_moves(pos, mlist);
	gen_cannon_moves(pos, mlist);
	gen_pawn_moves(pos, mlist);
	gen_bishop_moves(pos, mlist);
	gen_advisor_moves(pos, mlist);
	gen_king_moves(pos, mlist);
}