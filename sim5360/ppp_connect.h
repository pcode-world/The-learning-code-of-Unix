/********************************************************************************
 *      Copyright:  (C) 2019 wujinlong<547124558@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  ppp_connect.h
 *    Description:  This head file head file of ppp_connect.c
 *
 *        Version:  1.0.0(2019年07月26日)
 *         Author:  wujinlong <547124558@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年07月26日 14时09分47秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>      
#include <fcntl.h>
#include <stdlib.h>

#define RELAY_CUR 1
#define RELAY_END 2

int modify_file(FILE *fp,char *seekstr,char *newstr,int relay_loca);

int id_isp(char *cimi,int strsize);

int set_apn_call(int id_isp,FILE *fp);
