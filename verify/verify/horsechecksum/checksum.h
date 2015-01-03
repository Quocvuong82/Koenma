#ifndef checksum_h
#define checksum_h
#include "bitboard.h"

//���̾��������۵����۵���8������;����Ϊ���Ⱥ����۵�hash key
//һ����256�����
inline int checksum(const Bitboard& board) 
{
	unsigned long  Temp32 = board.Low | board.Mid | board.Hi;
	unsigned short Temp16 = ((unsigned short *) &Temp32)[0] | ((unsigned short *) &Temp32)[1];
	return (int) (((unsigned char *) &Temp16)[0] | ((unsigned char *) &Temp16)[1]);
}


//չ��У��ͣ���������checksum()�෴���䷵��ֵ�����Ȼ����۵�λ���̣�occ��λ�����е����Ȼ�����
//�������ֻ���������ŷ�Ԥ����������Ӧ��
//��Ԥ�����з����ͨ�������������checksum�����Ȼ�����λ���̶�Ӧ��ϵ
inline Bitboard duplicate(int checksum, const Bitboard& occ) 
{
	unsigned short Temp16;
	unsigned long Temp32;

	((unsigned char *) &Temp16)[0] = ((unsigned char *) &checksum)[0];
	((unsigned char *) &Temp16)[1] = ((unsigned char *) &checksum)[0];
	((unsigned short *) &Temp32)[0] = Temp16;
	((unsigned short *) &Temp32)[1] = Temp16;

	return Bitboard(Temp32, Temp32, Temp32) &  occ;	 //����checksum��������Ȼ������۵�λ����
}

#endif