/*********************************************************************************
 *      Copyright:  (C) 2019 wujinlong<547124558@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  test.c
 *    Description:  This file test arr \0
 *                 
 *        Version:  1.0.0(2019年07月18日)
 *         Author:  wujinlong <547124558@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年07月18日 17时12分06秒"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "at_msg.h"
#include "comport.h"
#include "at_msg.c"
#include "comport.c"

int main()
{
    comport set_comport;
    struct termios old_term;
    comport *comport_info = NULL;
    comport_info = initComport();
    set_comport = *comport_info;
    Comport_term(comport_info);

    char *pfind = NULL;
    char AT_hand[] = "AT\r\n";
    char te[] = "jsajd:2,54";
    printf("leng = %d\n",strlen(AT_hand));
    printf("size = %d\n",sizeof(AT_hand));
    pfind = strstr(te,":");
    pfind++;
    printf("chang to int %d\n",atoi(pfind));
    return 0 ;
}

/* answer leng = 4，size =5自动加上\0 */
