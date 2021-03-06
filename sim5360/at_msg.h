/********************************************************************************
 *      Copyright:  (C) 2019 wujinlong<547124558@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  at_msg.h
 *    Description:  This head file head file of at_msg.c
 *
 *        Version:  1.0.0(2019年07月19日)
 *         Author:  wujinlong <547124558@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年07月19日 18时34分04秒"
 *                 
 ********************************************************************************/

#ifndef _AT_MSG_H
#define _AT_MSG_H
#define _DEBUG 0

#include <string.h>
#include <stdio.h>
#include <unistd.h>

enum
{
    /* pattern */
    AT_SEARCH = 0,
    AT_SEEK,

    /* message type */
    TYPE_REC_UNREAD,//Received but not read
    TYPE_REC_READ,//Received and read
    TYPE_STO_UNSENT,//Saved but not sent
    TYPE_STO_SENT,//Saved and sent
    TYPE_ALL,//all
};

char *seek_AT_str(char *spe_str,char *exp_str,int pattern);

void try_times(int *times,int maxtimes);

void AT_test(int comport_fd,char *AT_command,char *readbuff,int readbuffsize,int sleeptime);

int judge_modulestate(int comport_fd);

int sendenglish(int comport_fd,const char *telnu,const char *msg);

int sendchinese(int comport_fd,char *sendbuff,int sendbuffsize);

int read_msg_index_txt(int fd,char *retbuff,int buffsize,int index);

int msg_output(char *buff,int index);

void del_msg(int comport_fd,int index);

int wait_newmsg_txt(int comport_fd,int waittimes);

int list_msg_txt(int comport_fd,int type);

#endif
