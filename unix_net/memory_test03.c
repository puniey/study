//测试全部变量 静态变量初始化
#include "unp.h"

int func1()
{
	int a = 5;
	return a;
}

const char *gString = "hello";
//int gNum1 = strlen(gString);
//const int gNum2 = strlen(gString);
//int gNum3 = func1();
int gNum4 = sizeof(gString);
//static int gNum5 = func1();
int main(void)
{
	int num = 5;
    //int nArray1[strlen(gString)];
	//int nArray2[func1()];
	const int num2 = num + 1;
	int nArray3[num2];
	static int numm = func1();
	//static int gNum5 = num;
	//printf("gNum1:%d\n", gNum1);
	//printf("gNum2:%d\n", gNum2);
	//printf("gNum3:%d\n", gNum3);
    printf("gNum4:%d\n", gNum4);
    //printf("gNum5:%d\n", gNum5);
	printf("sizeof nArray3:%d\n", sizeof(nArray3));
	return 0;
}
