/*********************************************************************************
 *      Copyright:  (C) 2019 wujinlong<547124558@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  arr2_find_pos.c
 *    Description:  This file Find a number in a two-dimensional array whose value is at column maximum, row minimum, 
 *                  and output position
 *                 
 *        Version:  1.0.0(2019年08月05日)
 *         Author:  wujinlong <547124558@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年08月05日 11时09分10秒"
 *                 
 ********************************************************************************/
#define N 3
#include <stdio.h>

void fun_find_nu(int (*arr)[N])
{
    int find = 0;
    int col,row,i,j,max;

    /**arr[i][j]*/

    /* 遍历所有列 */
    for(j=0;j<N;j++)
    {
        col = j;
        max = arr[j][0];
        row = 0;
        /*遍历一列的所有元素,找到最大值*/
        for(i=0;i<N;i++)
        {
            if(arr[i][j] > max)
            {
                max = arr[i][j];
                row = i;//记录所在行号
            }

        }

        /* 与所在行比较 */
        for(j=0;j<N;j++)
        {
            if(arr[row][j] < max)
            {
                break;
            }

            if(j == N-1)
            {
                find = 1;
            }

        }

        if(find == 1)
        {
            printf("arr[%d][%d] = %d\n",row,col,max);
            return;
        }

    }

    if(find)
    {
        printf("It is not exist!\n");
    }

}

int main()
{
    int i,j;
    int testarr[N][N] = {{1,8,3},{4,5,6},{3,8,9}};

    fun_find_nu(testarr);

    for(i=0;i<3;i++)
        for(j=0;j<3;j++)
            printf("%d ",testarr[i][j]);
    printf("\n");
    return 0;
}

