#include "comport.h"
#include "comport.c"
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include<errno.h>
#include <string.h>
#define ARRMAX 64
static int sig_exit = 0;

void showusage()
{
    printf("********This is the instruction manual**********\n");
    printf("option\n");
    printf("-p -b -d -s cannot be followed by Spaces !!!!!!!!\n");
    printf("-p --paritybit  ->Specifies parity checking,Default no check\n");
    printf("-b --baudrate   ->Specified baud rate,Default 115200\n");
    printf("-d --databit   ->Specified databit,Default 8\n");
    printf("-s --stopbit   ->Specified stopbit,Default 1\n");
    printf("-f --path    ->Specified The path of the serial device\n");
    printf("-h --help  ->print the usage\n");
}

void sighandler(int signum)
{
    if(signum == SIGINT)
    {
        sig_exit = 1;
    }
}

int main(int argc,char *argv[])

{
    extern char *optarg;
    extern int optind, opterr, optopt;
    int ch;
    struct timeval timeout;
    struct termios old_term;
    fd_set rset;
    int fd_arr[3];
    comport *comport_info = NULL;

    int rv = -1;
    char databuf[ARRMAX];

    if(NULL == (comport_info = initComport()))
    {
        return -1;
    }

    struct option opt[] = {
        {"paritybit",optional_argument,NULL,'p'},
        {"help",no_argument, NULL, 'h'},
        {"baudrate",optional_argument,NULL,'b'},
        {"datab",optional_argument,NULL,'d'},//{"",optional_argument,NULL,''},
        {"stopbit",optional_argument,NULL,'s'},
        {"path",required_argument,NULL,'f'},
        {0,         0,                 0,  0 }
    };


    while((ch = getopt_long(argc,argv,"p::b::d::s::f:h",opt,NULL)) != -1)
    {
        switch(ch)
        {
            case 'p':
                comport_info->paritybit = *optarg;
                break;
            case 'b':
                comport_info->baudrate = atoi(optarg);
                break;
            case 'd':
                comport_info->databit = atoi(optarg);
                break;
            case 's':
                comport_info->stopbit = atoi(optarg);
                break;
            case 'f':
                strncpy(comport_info->path,optarg,sizeof(comport_info->path));//字符数组不能用 = ，要用strncpy;
                break;
            case 'h':
                showusage();
                return 0;
            default:
                showusage();
                return -1;
        }
    }

    if(comport_info->path[0] == 0)
    {
        printf("Please enter the correct serial port  path\n");
        showusage();
        return -1;
    }

    signal(SIGINT,sighandler);

    if(openComport(comport_info,&old_term) < 0)
    {
        return -1;
    }

    fd_arr[0] = STDIN_FILENO;
    fd_arr[1] = comport_info->com_fd;
    while(!sig_exit)
    {
        timeout.tv_sec = 60;//延时60s
        timeout.tv_usec = 0;
        /* 如果将时间放在函数体初始化，每次select计时值是上一次的剩余时间 */
        FD_ZERO(&rset);
        FD_SET(fd_arr[0],&rset);
        FD_SET(fd_arr[1],&rset);
        memset(&databuf,0,sizeof(databuf));

        printf("get data from the STDIN_FILENO,press enter to end...\n");

        rv = select((comport_info->com_fd)+1, &rset, NULL,NULL, &timeout);  

        if(rv < 0) 
        { 
            printf("select failure: %s\n", strerror(errno));
            break;
        }
        
        else if(rv == 0) 
        {
            printf("select get timeout,exit now...\n"); 
            break;
        }

        else
        {
            //串口读事件发生
            if(FD_ISSET(comport_info->com_fd,&rset))
            {
                if(readComport(comport_info->com_fd,databuf,ARRMAX) < 0)
                {
                    break;
                }

                printf("read data from serialport:\n%s\n",databuf);
            }
            //标准输入读发生
            else
            {
                fgets(databuf,ARRMAX,stdin);//自动添加\0且不舍弃\n
                databuf[strlen(databuf)-1] = '\0';
                //printf("total %d bytes:%s\n",strlen(databuf),databuf);
                if(writeComport(comport_info->com_fd,databuf,strlen(databuf)) < 0)
                {
                    break;
                }
            }
        }
    }

    printf("program exit now and set the Save the serial port properties as the original properties...\n");
    closeComport(comport_info,&old_term);
    //comport_term(comport_info);

    return 0;

}
