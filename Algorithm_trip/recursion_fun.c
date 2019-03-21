#include <stdio.h>

void recursion_fun(int stop,int stop2)
{
	if(!stop)
	{
		return;
	}
	if(!stop2)
	{
		return;
	}
	printf("begain to excute recursion_fun\n");
	recursion_fun(--stop,stop2);
	printf("stop is %d\n",stop);//后面未执行的保存到栈中
	recursion_fun(stop,--stop2);
	printf("stop2 is %d\n",stop);


}
int main(void)
{
	recursion_fun(2,2);
	return 0;
}
/*
begain to excute recursion_fun
begain to excute recursion_fun
stop is 0
stop2 is 0 //恢复之后程序运行顺序不变
stop is 1
begain to excute recursion_fun
stop is 0
stop2 is 0
stop2 is 1
*/
