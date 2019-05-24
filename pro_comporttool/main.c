#include "comport.h"
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#define ARRMAX 64
#include <stdio.h>
#include <unistd.h>
#include<errno.h>
#include <string.h>
static int sig_exit = 1;

void showusage()
{
    printf("********This is the instruction manual**********\n");
    printf("option\n");
    printf("-p -b -d -s cannot be followed by Spaces !!!!!!!!\n");
    printf("-p --parityb  ->Specifies parity checking,Default no check\n");
    printf("-b --baudspeed   ->Specified baud rate,Default 115200\n");
    printf("-d --datab   ->Specified databit,Default 8\n");
    printf("-s --stopb   ->Specified stopbit,Default 1\n");
    printf("-f --path    ->Specified The path of the serial device\n");
    printf("-h --help  ->print the usage\n");
}
void jugnu(char *p)
{
    if((' ' == *p) || (*p < '0') || (*p > ':'))
    {
        printf("Options cannot be followed by Spaces or not belong to [1-9]\n");
        _exit(-1);
    }
    
}
void jugchar(char *p)
{
    if(*p < 'A' || *p > 'Z')
    {
        printf("the option can only be followed by uppercase\n");
    }
}
void sighandler(int signum)
{
    if(signum == SIGINT)
    {
        sig_exit = 0;
    }
}
int main(int argc,char *argv[])

{
    extern char *optarg;
    extern int optind, opterr, optopt;
    int ch;
    //char *com_path = "/dev/ttyUSB0";
    comport comport_info;
    memset(&comport_info,0,sizeof(comport_info));
    comport_info.parityb='N';
    comport_info.datab = 8;
    comport_info.baudspeed =115200;
    comport_info.stopb = 1;
    //字符数组不能用 = ，要用strncpy;
    struct termios new_term,old_term;
    struct timeval timeout;
    timeout.tv_sec = 10;//延时10s
    timeout.tv_usec = 0;

    struct option opt[] = {
        {"parityb",optional_argument,NULL,'p'},
        {"help",no_argument, NULL, 'h'},
        {"baudspeed",optional_argument,NULL,'b'},
        {"datab",optional_argument,NULL,'d'},//{"",optional_argument,NULL,''},
        {"stopb",optional_argument,NULL,'s'},
        {"path",required_argument,NULL,'f'},
        {0,         0,                 0,  0 }
    };


    while((ch = getopt_long(argc,argv,"p::b::d::s::f:h",opt,NULL)) != -1)
    {
        switch(ch)
        {
            case 'p':
                jugchar(optarg);
                comport_info.parityb = *optarg;
                break;
            case 'b':
                jugnu(optarg);
                comport_info.baudspeed = atoi(optarg);break;
            case 'd':
                jugnu(optarg);
                comport_info.datab = atoi(optarg);break;
            case 's':
                jugnu(optarg);
                comport_info.stopb = atoi(optarg);break;
            case 'f':
                strncpy(comport_info.path,optarg,sizeof(comport_info.path));break;
            case 'h':
                showusage();return 0;
            default:
                showusage();return -1;
        }
    }

    if(comport_info.path[0] == 0)
    {
        printf("Please enter the correct serial port  path\n");
        showusage();
        return -1;
    }

    signal(SIGINT,sighandler);
    openComport(&comport_info,&new_term);
    new_term = old_term;
    setComport(&comport_info,&new_term);
    fd_set rset;
    int fd_arr[3];
    fd_arr[0] = STDIN_FILENO;
    fd_arr[1] = comport_info.com_fd;
    while(sig_exit)
    {
        FD_ZERO(&rset);
        FD_SET(fd_arr[0],&rset);
        FD_SET(fd_arr[1],&rset);
        char databuf[ARRMAX] = {0};

        printf("get data from the STDIN_FILENO,press enter to end...\n");

        fgets(databuf,ARRMAX,stdin);
        writeComport(comport_info.com_fd,databuf,ARRMAX);

        int rv = -1;
        rv = select(comport_info.com_fd+1, &rset, NULL,NULL, &timeout);  

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
            //标准输入读发生
            if(FD_ISSET(STDIN_FILENO,&rset))
            {
                if((read(STDIN_FILENO,databuf,ARRMAX)) < 0)
                {
                    return -1;
                }
                writeComport(comport_info.com_fd,databuf,ARRMAX);
                readComport(comport_info.com_fd,databuf,ARRMAX);
                printf("read data from serialport:\n%s",databuf);
            }
            
            //串口读事件发生
            if(FD_ISSET(comport_info.com_fd,&rset))
            {
                readComport(comport_info.com_fd,databuf,ARRMAX);
                printf("read data from serialport:\n%s",databuf);
            }
        }
    }
    printf("program exit now and set the Save the serial port properties as the original properties...\n");
    closeComport(comport_info.com_fd,&old_term);
    return 0;

}
