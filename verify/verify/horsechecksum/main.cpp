#include <stdio.h>
#include <time.h>
#include <memory.h>
#include "data.h"
#include "test.h"

enum ETEST{
	eOne = 0,
	eTwo = 10,
};
int main()
{
	 init();

	 //����ע�ͣ����ɲ���
	 //test_by_row();//���԰������е�λ�����޷����������۵�λ����
	 //test_by_col();//��֤�������е�λ���̿��Զ����ȣ�
	 //test_by_col_knight_eye();//������λ����
	 //test_speed();
	 //test_memcpy();
     test_memcmp();

	 

	 getchar();
	return 0;
}