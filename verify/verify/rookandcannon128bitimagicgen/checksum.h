#ifndef CHECKSUM_H
#define CHECKSUM_H
#include "bitboard.h"

//ʹ���۵�λ���̣�Ҫ���ǣ�square���밴���������У�����ת��λ�������ǰ����������еģ����Էǳ��ʺ��۵�λ����
inline uint8_t checksum(const Bitboard& board) 
{
	uint32_t  Temp32 = board.bb[0] | board.bb[1] | board.bb[2];
	uint16_t  Temp16 = ((uint16_t *) &Temp32)[0] | ((uint16_t *) &Temp32)[1];
	return (uint8_t) (((uint8_t *) &Temp16)[0] | ((uint8_t *) &Temp16)[1]);
}

inline Bitboard duplicate(uint8_t checksum, const Bitboard& occlegoreye) 
{
	uint16_t Temp16;
	uint32_t Temp32;

	((uint8_t *) &Temp16)[0] = ((uint8_t *) &checksum)[0];
	((uint8_t *) &Temp16)[1] = ((uint8_t *) &checksum)[0];
	((uint16_t *) &Temp32)[0] = Temp16;
	((uint16_t *) &Temp32)[1] = Temp16;

	return Bitboard(Temp32, Temp32, Temp32) &  occlegoreye;	 //����checksum��������Ȼ������۵�λ����
}
#endif