/*********************************************************************************
 *      Copyright:  (C) 2019 wujinlong<547124558@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  test_at_msg.c
 *    Description:  This file test at_msg.c 
 *                 
 *        Version:  1.0.0(2019年07月19日)
 *         Author:  wujinlong <547124558@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年07月19日 19时00分20秒"
 *                 
 ********************************************************************************/

#include "at_msg.h"
#include "comport.h"
#include "at_msg.c"
#include "comport.c"
int main(void)
{
    struct termios old_term;
    comport *comport_info = NULL;

    comport_info = initComport();

    strcpy(comport_info->path, "/dev/ttyUSB2");

    openComport(comport_info,&old_term);

    printf("comport_info->paritybit = %c\n",comport_info->paritybit);
    printf("comport_info->com_fd = %d\n",comport_info->com_fd);
    printf("comport_info->databit = %d\n",comport_info->databit);
    printf("comport_info->baudrate = %d\n",comport_info->baudrate);
    printf("comport_info->stopbit = %d\n",comport_info->stopbit);
    printf("comport_info->isopen = %d\n",comport_info->isopen);
    printf("comport_info->flowctl = %d\n",comport_info->flowctl);
    printf("comport_info->path = %s\n",comport_info->path);

    printf("open successful\n");

    judge_modulestate(comport_info->com_fd);

    sendenglish(comport_info->com_fd,NULL,0);

    return 0;
}

