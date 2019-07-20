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

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define AT_SEARCH 0
#define AT_SEEK 1

char *seek_AT_str(char *spe_str,char *exp_str,int pattern);

void try_times(int *times,int maxtimes);

int AT_test(int comport_fd,char *AT_command,char *readbuff,int readbuffsize,int sleeptime);

int judge_modulestate(int comport_fd);

int sendenglish(int comport_fd,char *sendbuff,int sendbuffsize);

#endif
