#ifndef test_h
#define test_h
#include <time.h>
#include <stdlib.h>
#include "checksum.h"
#include "data.h"

//֤��sq�����������޷�ʹ����checksum�����ͻ
void test_by_row()
{
	//ֻҪ��֤��ĳ��λ�ã����ȵ����п������Σ��໥֮�䲻�����hash��ͻ����
	printf("--------test_by_row------------\n");
	int HorseLegsStep[4][2] = {{ 0,1},{0,-1},{ -1,0},{1,0}};

	Bitboard knightleg(0,0,0);
	Bitboard arrBoard[90][16];

	//���һ��λ��
	for(int sq = 0; sq < 1; ++sq)
	{
	
		int bdcount = 0;
		//����ÿ������
		for (int t = 0; t <= 1; t++)
		{
			for(int b = 0; b <= 1; b++)
			{
				for (int l = 0; l <= 1; l++)
				{
					for (int r = 0; r <= 1; r++)
					{    //�������ҵĸ�����ϣ�Ŀ�������


						knightleg = Bitboard(0,0,0);
						//���λ����
						if(t)
						{
							int r = SqToRankEx[sq] + HorseLegsStep[0][1];//��
							int f = SqToFileEx[sq] + HorseLegsStep[0][0];//��
							if((r>= 0&& r<=9) && f>=0 && f<=8)
							{
								knightleg |=BIT((r*9+f)) ;
							}
						}
						if (b)
						{
							int r = SqToRankEx[sq] + HorseLegsStep[1][1];//��
							int f = SqToFileEx[sq] + HorseLegsStep[1][0];//��
							if((r>= 0&& r<=9) && f>=0 && f<=8)
							{
								knightleg |=BIT((r*9+f)) ;
							}
						}
						if (l)
						{
							int r = SqToRankEx[sq] + HorseLegsStep[2][1];//��
							int f = SqToFileEx[sq] + HorseLegsStep[2][0];//��
							if((r>= 0&& r<=9) && f>=0 && f<=8)
							{
								knightleg |=BIT((r*9+f)) ;
							}
						}
						if (r)
						{
							int r = SqToRankEx[sq] + HorseLegsStep[3][1];//��
							int f = SqToFileEx[sq] + HorseLegsStep[3][0];//��
							if((r>= 0&& r<=9) && f>=0 && f<=8)
							{
								knightleg |=BIT((r*9+f)) ;
							}
						}

						arrBoard[sq][bdcount++] = knightleg;	
						
					}
				}
			}
		}

	}

	for (int sq = 0; sq < 1; ++sq)
	{
		for (int i = 0; i < 16; i++)
		{
			for (int j = i+1;j < 16; j++ )
			{
				if (arrBoard[sq][i] != arrBoard[sq][j])
				{
					if (checksum(arrBoard[sq][i]) == checksum(arrBoard[sq][j]))
					{
						printf("conflit sq %d\n",sq);
                        arrBoard[sq][i].print();
                        arrBoard[sq][j].print();
                        printf("check %d\n", checksum(arrBoard[sq][i]));
					}
				}
			}
		}
	}


	printf("------��������г�ͻ�����Բ��ܰ���������--------------\n");
	getchar();
}
//֤��sq���������п���ʹ����checksum�����ͻ
void test_by_col()
{
	//ֻҪ��֤��ĳ��λ�ã����ȵ����п������Σ��໥֮�䲻�����hash��ͻ����
	printf("--------test_by_col------------\n");
	int HorseLegsStep[4][2] = {{ 0,1},{0,-1},{ -1,0},{1,0}};

	Bitboard knightleg(0,0,0);
	Bitboard arrBoard[90][16];

	//���һ��λ��
	for(int sq = 0; sq < 90; ++sq)
	{

		int bdcount = 0;
		//����ÿ������
		for (int t = 0; t <= 1; t++)
		{
			for(int b = 0; b <= 1; b++)
			{
				for (int l = 0; l <= 1; l++)
				{
					for (int r = 0; r <= 1; r++)
					{    //�������ҵĸ�����ϣ�Ŀ�������


						knightleg = Bitboard(0,0,0);
						//���λ����
						if(t)
						{
							int r = SqToRank[sq] + HorseLegsStep[0][1];//��
							int f = SqToFile[sq] + HorseLegsStep[0][0];//��
							if((r>= 0&& r<=8) && f>=0 && f<=9)
							{
								knightleg |=BIT((r*10+f)) ;
							}
						}
						if (b)
						{
							int r = SqToRank[sq] + HorseLegsStep[1][1];//��
							int f = SqToFile[sq] + HorseLegsStep[1][0];//��
							if((r>= 0&& r<=8) && f>=0 && f<=9)
							{
								knightleg |=BIT((r*10+f)) ;
							}
						}
						if (l)
						{
							int r = SqToRank[sq] + HorseLegsStep[2][1];//��
							int f = SqToFile[sq] + HorseLegsStep[2][0];//��
							if((r>= 0&& r<=8) && f>=0 && f<=9)
							{
								knightleg |=BIT((r*10+f)) ;
							}
						}
						if (r)
						{
							int r = SqToRank[sq] + HorseLegsStep[3][1];//��
							int f = SqToFile[sq] + HorseLegsStep[3][0];//��
							if((r>= 0&& r<=8) && f>=0 && f<=9)
							{
								knightleg |=BIT((r*10+f)) ;
							}
						}

						arrBoard[sq][bdcount++] = knightleg;

						//knightleg.print();

					}
				}
			}
		}

	}

	for (int sq = 0; sq < 90; ++sq)
	{
		for (int i = 0; i < 16; i++)
		{
			for (int j = i+1;j < 16; j++ )
			{
				if (arrBoard[sq][i] != arrBoard[sq][j])
				{
					if (checksum(arrBoard[sq][i]) == checksum(arrBoard[sq][j]))
					{
						printf("conflit sq %d\n",sq);
						arrBoard[sq][i].print();
						arrBoard[sq][j].print();
						printf("check %d\n", checksum(arrBoard[sq][i]));
					}
				}
			}
		}
	}


	printf("-------֤����Ҫʹ���ȵ�checksum����ͻ�����밴����������sq-------------\n");
	getchar();
}

//֤�����水�������е����ͬ������������
void test_by_col_knight_eye()
{
	//ֻҪ��֤��ĳ��λ�ã����۵����п������Σ��໥֮�䲻�����hash��ͻ����
	printf("--------test_by_col_knight_eye------------\n");
	int HorseEyeStep[4][2] = {{ 1,1},{-1,-1},{ -1,1},{1,-1}};

	Bitboard knightleg(0,0,0);
	Bitboard arrBoard[90][16];

	//���һ��λ��
	for(int sq = 0; sq < 90; ++sq)
	{

		int bdcount = 0;
		//����ÿ������
		for (int t = 0; t <= 1; t++)
		{
			for(int b = 0; b <= 1; b++)
			{
				for (int l = 0; l <= 1; l++)
				{
					for (int r = 0; r <= 1; r++)
					{    //�������ҵĸ�����ϣ�Ŀ�������


						knightleg = Bitboard(0,0,0);
						//���λ����
						if(t)
						{
							int r = SqToRank[sq] + HorseEyeStep[0][1];//��
							int f = SqToFile[sq] + HorseEyeStep[0][0];//��
							if((r>= 0&& r<=8) && f>=0 && f<=9)
							{
								knightleg |=BIT((r*10+f)) ;
							}
						}
						if (b)
						{
							int r = SqToRank[sq] + HorseEyeStep[1][1];//��
							int f = SqToFile[sq] + HorseEyeStep[1][0];//��
							if((r>= 0&& r<=8) && f>=0 && f<=9)
							{
								knightleg |=BIT((r*10+f)) ;
							}
						}
						if (l)
						{
							int r = SqToRank[sq] + HorseEyeStep[2][1];//��
							int f = SqToFile[sq] + HorseEyeStep[2][0];//��
							if((r>= 0&& r<=8) && f>=0 && f<=9)
							{
								knightleg |=BIT((r*10+f)) ;
							}
						}
						if (r)
						{
							int r = SqToRank[sq] + HorseEyeStep[3][1];//��
							int f = SqToFile[sq] + HorseEyeStep[3][0];//��
							if((r>= 0&& r<=8) && f>=0 && f<=9)
							{
								knightleg |=BIT((r*10+f)) ;
							}
						}

						arrBoard[sq][bdcount++] = knightleg;

						//knightleg.print();

					}
				}
			}
		}

	}

	for (int sq = 0; sq < 90; ++sq)
	{
		for (int i = 0; i < 16; i++)
		{
			for (int j = i+1;j < 16; j++ )
			{
				if (arrBoard[sq][i] != arrBoard[sq][j])
				{
					if (checksum(arrBoard[sq][i]) == checksum(arrBoard[sq][j]))
					{
						printf("conflit sq %d\n",sq);
						arrBoard[sq][i].print();
						arrBoard[sq][j].print();
						printf("check %d\n", checksum(arrBoard[sq][i]));
					}
				}
			}
		}
	}


	printf("-------֤����Ҫʹ���۵�checksum����ͻ�����밴����������sq-------------\n");
	getchar();
}

//test_by_col_knight_eyeͬ��Ҳ֤�������۵�checksumҲ����ͻ

void test_speed()
{
	//����checksum�ٶ�����ȡ����mask��ɵ�hashkey�ĸ�����

	//����checksum
	__int64 count = 99999999999;

	clock_t start = clock(); 
	for(__int64 i = 0; i < count; ++i)
	{
		checksum(KnightLegs[45]);
	}

	printf("%d\n",clock()-start);


    int HorseLegsStep[4][2] = {{ 0,1},{0,-1},{ -1,0},{1,0}};

	int k = 0;
	int s = 45;
	Bitboard board(0,0,0);
    start = clock();
	for(__int64 k = 0; k < count; ++k)
	{
		
		int b = 0;
		for (int i = 0; i < 4; ++i)
		{

			int r = SqToRank[s] + HorseLegsStep[0][1];//��
			int f = SqToFile[s] + HorseLegsStep[0][0];//��

			b |= (!!(BIT(SQ(r,f))&board))<<i;
			
		}
	}

	printf("%d\n",clock()-start);

	//��releaaseģʽ�£����ַ����ٶ��ϲ��޲���
	//���Եڶ��з���ִ�е�ָ��࣬���ܱ����������Ż���
	getchar();

}
#endif