/*********************************************************************************
 *      Copyright:  (C) 2019 wujinlong<547124558@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  div_char_nu.c
 *    Description:  This file function of CVTE interview question
 *                 
 *        Version:  1.0.0(2019年07月29日)
 *         Author:  wujinlong <547124558@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年07月29日 16时38分27秒"
 *                 
 ********************************************************************************/


/******************************************************************************************************
 * Description:Separate the number and letter in a string, with the number before and the letter after,
 *             allowing no extra space
 * Input Args:a string
 * Output Args:Ordered sequence
 * Return Value:char *,String header address
 ******************************************************************************************************/

#include <stdio.h>
#include <string.h>

char *div_char_nu(char *inputstr,size_t size)
{
    int i,j;
    char swap;

    for(i=0;i<size;i++)
    {
        for(j=0;j<size-1;j++)
        {

            if((inputstr[j] > '`' && inputstr[j] < '{') && (inputstr[j+1] > '/' && inputstr[j+1] < ':'))
            {
                swap = inputstr[j];
                inputstr[j] = inputstr[j+1];
                inputstr[j+1] = swap;
            }


        }

    }

    return inputstr;
}

int main()
{
    char str[] = "5cad5f12fw9fr";

    div_char_nu(str,sizeof(str));

    printf("the answer is:%s\n",str);

    return 0;
}
