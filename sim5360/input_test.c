/*********************************************************************************
 *      Copyright:  (C) 2019 SCUEC
 *                  All rights reserved.
 *
 *       Filename:  input_test.c
 *    Description:  This file test fl2440 input
 *                 
 *        Version:  1.0.0(07/23/2019)
 *         Author:  Donald Shallwing <donald_shallwing@foxmail.com>
 *      ChangeLog:  1, Release initial version on "07/23/2019 09:08:12 AM"
 *                 
 ********************************************************************************/
#include <stdio.h>


int main()
{
    char rec_nu[12] = {0};
    char sendbuff[1024] = {0};
    printf("please enter the reciver:\n");
    scanf("%s",rec_nu) ;
       
    printf("please enter the message content:\n");
    scanf("%s",sendbuff) ;
    printf("phnumber = %s\n",rec_nu);
    printf("content =%s\n",sendbuff);
    return 0;
}
