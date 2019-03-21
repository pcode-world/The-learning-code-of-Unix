#include <stdio.h>
#define ARRAY_SIZE(x)       (sizeof(x)/sizeof(x[0]))


void merge(int arr[],int L,int mid,int R)
{
	int pl = L;
	int pr = mid+1;
	int temp[R-L+1];
	int tp = 0;

	while((pl <= mid) && (pr <= R))
	{
		temp[tp++] = arr[pl] < arr[pr] ? arr[pl++] : arr[pr++];
	}
	while(pl <= mid)
	{
		temp[tp++] = arr[pl++];
	}
	while(pr <= R)
	{
		temp[tp++] = arr[pr++];
	}

	tp = 0;
	while(L <= R)
	{
		arr[L++] = temp[tp++];
	}

}

void mergesort(int arr[],int L,int R)
{
	if(L == R)
		return;
	int mid=L+((R-L)>>1);//防止溢出 
	mergesort(arr,L,mid);
	mergesort(arr,mid+1,R);
	merge(arr,L,mid,R);
}
int main(void)
{
	int i = 0;
	int test[]={9,8,6,4,2,1,9,5};
	mergesort(test,0,ARRAY_SIZE(test)-1);
	while(i < ARRAY_SIZE(test))
	{
		printf("%d ",test[i++]);
	}
	printf("\n");
	return 0;
}
