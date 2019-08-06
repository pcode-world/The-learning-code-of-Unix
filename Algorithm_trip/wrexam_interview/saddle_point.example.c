/*********************************************************************************
 *      Copyright:  (C) 2019 wujinlong<547124558@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  saddle_point.c
 *    Description:  This file find the saddle point
 *                 
 *        Version:  1.0.0(2019年08月05日)
 *         Author:  wujinlong <547124558@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年08月05日 17时00分40秒"
 *                 
 ********************************************************************************/
#include <stdio.h>
#define N 3
#define M 3                   /*  数组为4行5列 */
int main()
{
    int i,j,k,a[N][M],max,maxj,flag;
    printf("please input matrix:\n");
    for (i=0;i<N;i++)           /*  输入数组 */
        for (j=0;j<M;j++)
            scanf("%d",&a[i][j]);

    for (i=0;i<N;i++)
    {
        max=a[i][0];               /*  开始时假设a[i][0]最大 */
        maxj=0;                    /*  将列号0赋给maxj保存 */
        for (j=0;j<M;j++)/*  找出第i行中的最大数 */

            if (a[i][j]>max)
            {
                max=a[i][j];         /*  将本行的最大数存放在max中 */
                maxj=j;              /*  将最大数所在的列号存放在maxj中 */
            }

        flag=1;                   /*  先假设是鞍点，以flag为1代表 */
        
        for (k=0;k<N;k++)
            
            if (max>a[k][maxj])     /*  将最大数和其同列元素相比 */
            {
                flag=0;             /*  如果max不是同列最小，表示不是鞍点令flag1为0 */
                continue;
            }

        if(flag)                  /*  如果flag1为1表示是鞍点 */
        {
            printf("a[%d][%d]=%d\n",i,maxj,max);   /*  输出鞍点的值和所在行列号 */
            break;
        }
    }
        return 0;
}

