/*********************************************************************************
 *      Copyright:  (C) 2019 wujinlong<547124558@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  mtk_2017.c
 *    Description:  This file mtk exam of 2017
 *                 
 *        Version:  1.0.0(2019年08月04日)
 *         Author:  wujinlong <547124558@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年08月04日 16时20分27秒"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>

void print_str(char *str,size_t length)
{
    unsigned char arr[128] = {0};
    int ch;
    int i = 0;

    for(i;i<length;i++)
    {
        ch = str[i];
        arr[ch]++;
        printf("ch = %d\n",ch);

    }

    for(i=0;i<128;i++)
    {
        if(arr[i] == 0)
        {
            continue;
        }
        printf("%c%d",i,arr[i]);
    }
    printf("\n");
}

int main()
{
    char *p = "abGD_eiqngeow#i*gew*earr";

    print_str(p,strlen(p));
    return 0;
}

