/********************************************************************************
 *      Copyright:  (C) 2019 wujinlong<547124558@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  epoll_sever.h
 *    Description:  This head file head file of epoll_sever.c
 *
 *        Version:  1.0.0(2019年08月02日)
 *         Author:  wujinlong <547124558@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年08月02日 17时32分41秒"
 *                 
 ********************************************************************************/
#ifndef _EPOLL_SERVER_H
#define _EPOLL_SERVER_H
#include <getopt.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>//错误处理
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <poll.h>

#define MAXEVENT 512
#define BACKLOG 13

void printfhelp(void);

void sig_sigstp(int signum);

int epoll_server_start(int LISTENPORT);

#endif
