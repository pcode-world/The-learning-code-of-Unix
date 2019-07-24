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
    char rec_nu[12] = {0};
    char sendbuff[1024] = {0};
    struct termios old_term;
    comport *comport_info = NULL;
    comport_info = initComport();

    strcpy(comport_info->path, "/dev/ttyUSB2");

    printf("please enter the reciver:\n");
    scanf("%s",rec_nu) ;

    printf("please enter the message content:\n");
    //scanf("%s",sendbuff) ;
    fgets(sendbuff,sizeof(sendbuff),stdin);

    if(openComport(comport_info,&old_term) < 0)
    {
        return -1;
    }
    printf("open successful\n");

    judge_modulestate(comport_info->com_fd);

    
    sendenglish(comport_info->com_fd,rec_nu,sendbuff);

    list_msg_txt(comport_info->com_fd,TYPE_ALL);

    wait_newmsg_txt(comport_info->com_fd,10);

    Comport_term(comport_info);

    return 0;
}

