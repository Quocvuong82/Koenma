// rookandcannonimagicgen.cpp : �������̨Ӧ�ó������ڵ㡣
//

//#include "stdafx.h"
#include <Windows.h>
#include "data.h"

int main()
{
	printf("64bit Bitboard is ok\n");
	printf("can find all imagic!\n");
	printf("in magic_index <<18 is key\n");	

	printf("rook need %d byte == %dm memory\n", 1081344*16, (1081344*16)/1024/1024);

	printf("%d %dk\n",1<<15, (1 << 15)*2/1024);
	
	//gen imagic 
	init_data();
	
	system("pause");
	return 0;
}

