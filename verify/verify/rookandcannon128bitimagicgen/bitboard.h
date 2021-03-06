#ifndef BITBOARD_H
#define BITBOARD_H
#include "platform.h"

//位棋盘，棋盘映射如下：
//bit map to square as follow:
/*
81, 82, 83, 84, 85, 86, 87, 88, 89,
72, 73, 74, 75, 76, 77, 78, 79, 80,
63, 64, 65, 66, 67, 68, 69, 70, 71,
54, 55, 56, 57, 58, 59, 60, 61, 62,
45, 46, 47, 48, 49, 50, 51, 52, 53,
36, 37, 38, 39, 40, 41, 42, 43, 44,
27, 28, 29, 30, 31, 32, 33, 34, 35,
18, 19, 20, 21, 22, 23, 24, 25, 26,
9,  10, 11, 12, 13, 14, 15, 16, 17,
0,  1,  2,  3,  4,  5,  6,  7,  8, 
*/

//向左旋转90度位棋盘映射如下：
//left rotate 90 degree square mapping as follow:
/*
89, 80, 71, 62, 53, 44, 35, 26, 17, 8,
88, 79, 70, 61, 52, 43, 34, 25, 16, 7,
87, 78, 69, 60, 51, 42, 33, 24, 15, 6,
86, 77, 68, 59, 50, 41, 32, 23, 14, 5,
85, 76, 67, 58, 49, 40, 31, 22, 13, 4,
84, 75, 66, 57, 48, 39, 30, 21, 12, 3,
83, 74, 65, 56, 47, 38, 29, 20, 11, 2,
82, 73, 64, 55, 46, 37, 28, 19, 10, 1,
81, 72, 63, 54, 45, 36, 27, 18, 9,  0,
*/

class Bitboard
{
	// 0~44bit and 0~44 bit
public:
	static const uint64_t BIT_MASK = 0x1FFFFFFFFFFF;
public:
	uint64_t bb[2];

	Bitboard(){ bb[0] = bb[1] = 0;}
	Bitboard(uint64_t low, uint64_t hig){bb[0] = low&BIT_MASK; bb[1] = hig&BIT_MASK;}
	
	operator bool()const{
		return bb[0]||bb[1];
	}

	int operator == (const Bitboard& board)const{
		return bb[0] == board.bb[0] &&
			   bb[1] == board.bb[1] ;
	}

	int operator != (const Bitboard& board)const{
		return bb[0] != board.bb[0] ||
			   bb[1] != board.bb[1] ;
	}

	Bitboard operator ~()const{
		return Bitboard(~bb[0],~bb[1]);//最高取反产生垃圾数据
	}

	Bitboard operator &(const Bitboard& board)const{
		return Bitboard(bb[0]&board.bb[0],bb[1]&board.bb[1]);
	}

	Bitboard operator |(const Bitboard& board)const{
        return Bitboard(bb[0]|board.bb[0],bb[1]|board.bb[1]);
	}

	Bitboard operator ^(const Bitboard &board) const 
	{
		return Bitboard(bb[0]^board.bb[0],bb[1]^board.bb[1]);
	}

	Bitboard &operator &=(const Bitboard &board) 
	{
		bb[0] &= board.bb[0];
		bb[1] &= board.bb[1];

		return *this;
	}

	Bitboard &operator |=(const Bitboard &board) 
	{
		bb[0] |= board.bb[0];
		bb[1] |= board.bb[1];
		return *this;
	}

	Bitboard &operator ^=(const Bitboard &board) 
	{
		bb[0] ^= board.bb[0];
		bb[1] ^= board.bb[1];

		return *this;
	}

	Bitboard operator <<(int bit) 
	{ 
		//最高取反产生垃圾数据
		if (bit < 0)
			return *this >> -bit;
		else if (bit <= 45)
            return Bitboard(bb[0] << bit, bb[1] << bit | bb[0] >> (45 - bit));
		else if(bit <= 90)
			return Bitboard(0, bb[0]<<(bit-45));
		else
			return Bitboard(0, 0);
	}

	Bitboard operator >>(int bit) 
	{
		if (bit < 0)
			return *this << -bit;
		else if(bit <= 45)
			return Bitboard(bb[0] >> bit | bb[1] << (45 - bit), bb[1] >> bit );
		else if(bit <= 90)
            return Bitboard(bb[1] >> (bit - 45), 0);
		else
			return Bitboard(0, 0);
	}

	Bitboard &operator <<=(int bit)
	{
		if (bit < 0){
			*this >>= -bit;
		} 
		else if (bit <= 45)
		{
			bb[1] <<= bit;
			bb[1] |= bb[0]>>(45-bit);
			bb[0] <<= bit;

			bb[0] &= BIT_MASK;//垃圾数据清理
			bb[1] &= BIT_MASK;//垃圾数据清理
		}
		else if (bit <= 90)
		{
			bb[1] = bb[0]<<(bit-45);
			bb[0] = 0;

			bb[1] &= BIT_MASK;//垃圾数据清理
		}
		else
		{
			bb[0] = 0;
			bb[1] = 0;
		}

		return *this;
	}

	Bitboard &operator >>=(int bit)
	{
		if (bit < 0){
			*this <<= -bit;
		} 
		else if (bit <= 45)
		{
            bb[0] >>= bit;
            bb[0] |= bb[1] << (45 - bit);
			bb[1] >>= bit;

			bb[0] &= BIT_MASK;//垃圾数据清理
		}
		else if (bit <= 90)
		{
			bb[0] = bb[1] >> (bit - 45);
			bb[1] = 0;
		}
		else
		{ 
            bb[0] = 0;
            bb[1] = 0;
		}

		return *this;
	}

	void pop_lsb()
	{
		if (bb[0])
		{
			bb[0] &= bb[0] - 1;
		}
		else if (bb[1])
		{
			bb[1] &= bb[1] - 1;
		}

	}

	//for debug
	void print(FILE* fp)//打印正常棋盘，未旋转的；
	{
		fprintf(fp,"\n");
		
		int shift[90] = {
			81, 82, 83, 84, 85, 86, 87, 88, 89,
			72, 73, 74, 75, 76, 77, 78, 79, 80,
			63, 64, 65, 66, 67, 68, 69, 70, 71,
			54, 55, 56, 57, 58, 59, 60, 61, 62,
			45, 46, 47, 48, 49, 50, 51, 52, 53,
			36, 37, 38, 39, 40, 41, 42, 43, 44,
			27, 28, 29, 30, 31, 32, 33, 34, 35,
			18, 19, 20, 21, 22, 23, 24, 25, 26,
			9,  10, 11, 12, 13, 14, 15, 16, 17,
			0,  1,  2,  3,  4,  5,  6,  7,  8,
		};
		Bitboard one(0x1,0);
		for (int i = 0; i < 90; ++i)
		{
            Bitboard t = one<<shift[i];
            fprintf(fp,"%d",(t&(*this)) ? 1 : 0);

			if ((i+1)%9 == 0)
			{
				fprintf(fp,"\n");
			}

			if(i+1 == 45)
			{
               fprintf(fp,"---------\n");
			}
		}
	}

	void printl90X(FILE* fp)//正确的，打印已经旋转的位棋盘
	{
		fprintf(fp,"\n");

		int shift[10] = {0,10,20,30,40,50,60,70,80,90};       

		Bitboard one(0x1,0);
		for(int i = 0 ; i< 9; ++i)
		{
			Bitboard t = (*this)>>shift[8-i];
			for(int j = 0; j < 10; ++j)
			{
				fprintf(fp,"%d",(t&(one<<j)) ? 1 : 0);

			}
			fprintf(fp,"\n");
		}
	}

	void printl90(FILE* fp)//未旋转的棋盘，打印其旋转之后的棋盘效果
	{
		fprintf(fp,"\n");

		int shift[90] = {
			89, 80, 71, 62, 53, 44, 35, 26, 17, 8,
			88, 79, 70, 61, 52, 43, 34, 25, 16, 7,
			87, 78, 69, 60, 51, 42, 33, 24, 15, 6,
			86, 77, 68, 59, 50, 41, 32, 23, 14, 5,
			85, 76, 67, 58, 49, 40, 31, 22, 13, 4,
			84, 75, 66, 57, 48, 39, 30, 21, 12, 3,
			83, 74, 65, 56, 47, 38, 29, 20, 11, 2,
			82, 73, 64, 55, 46, 37, 28, 19, 10, 1,
			81, 72, 63, 54, 45, 36, 27, 18, 9,  0,
		};
		Bitboard one(0x1,0);
		for (int i = 0; i < 90; ++i)
		{
			Bitboard t = one<<shift[i];
			fprintf(fp,"%d",(t&(*this)) ? 1 : 0);

			if ((i+1)%10 == 0)
			{
				fprintf(fp,"\n");
			}
		}


	}

	void printall(FILE* fp)
	{
		int shift[10] = {0,9,18,27,36,45,54,63,72,81};  

		fprintf(fp,"\n");

		Bitboard one(0x1,0);
		for(int i = 0 ; i< 10; ++i)
		{
			Bitboard t = (*this)>>shift[9-i];
			for(int j = 0; j < 9; ++j)
			{
				fprintf(fp,"%d",(t&(one<<j)) ? 1 : 0);

			}
			fprintf(fp,"\n");
		}

		Bitboard t = (*this)>>90;
		for(int i = 0; i < 6; ++i)
		{
			fprintf(fp,"%d",(t&(one<<i)) ? 1 : 0);
		}
		fprintf(fp,"\n");
	}

};
#endif