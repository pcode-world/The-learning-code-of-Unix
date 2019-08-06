/*********************************************************************************
 *      Copyright:  (C) 2019 wujinlong<547124558@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  pthread.c
 *    Description:  This file is thread  of MSG , NET and CTR sys
 *                 
 *        Version:  1.0.0(2019年07月16日)
 *         Author:  wujinlong <547124558@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年07月16日 20时07分52秒"
 *                 
 ********************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include "at_msg.h"
#include "comport.h"
#include "ppp_connect.h"
#include "epoll_sever.h"

#define PRI_CALL 3
#define PRI_SENDMEG 2
#define PRI_READMEG 1

static int g_stop;

typedef struct{
    int req;
    int exc;
    int srcfd;
    int serial_fd;
    int occupy;
    pthread_mutex_t lock;
}CTR;

void sig_sigint(int signum)
{
    if(signum == SIGINT)
    {
        g_stop = 1;
    }
}

int getreq(char *buff,int buffsize);

void *fun_sendmsg(void *p_ctr);

void *fun_rvmsg(void *p_ctr);

int main(int argc,char *argv[])
{
    extern char *optarg;
    extern int optind, opterr, optopt;
    int listenport=0;
    char cmd_buff[1024];
    int ret_fd;
    int ret;
    struct termios old_term;
    comport *comport_info = NULL;

    CTR ctr;
    pthread_attr_t thread_attr; 
    pthread_t tid_sendmsg,tid_rvmsg;

    while ((ret = getopt(argc, argv, "p:h::")) != -1) 
    {   
        switch(ret)
        {   
            case 'p':
                listenport=atoi(optarg);
                //printf("argv is %s\n",optarg);
                break;
            case ':':
                printf("option:%c missing argument\n",optopt);
                break;
            case 'h':
                printfhelp();
                break;
            default:
                printf("please enter the correct argument,-h for help\n");
        }   
    }   

    if(!listenport)
    {   
        printfhelp();
        return -1; 
    } 

    CTR *pctr = &ctr;
    pctr->req = 0;
    pctr->exc = 0;
    pctr->srcfd = -1;
    pctr->occupy = 0;

    comport_info = initComport();
    strcpy(comport_info->path, "/dev/ttyUSB2");

    signal(SIGINT, sig_sigint);

    if(openComport(comport_info,&old_term) < 0)
    {
        return -1;
    }

    pctr->serial_fd = comport_info->com_fd;

    printf("open successful\n");

    if(pthread_attr_init(&thread_attr) != 0)
    {
        printf("Failed to allocate memory to modify thread properties:%s\n",strerror(errno));
        return -1;
    }

    if(pthread_mutex_init(&ctr.lock,NULL) != 0)
    {
        printf("mutex init failure:%s\n",strerror(errno));
        pthread_attr_destroy(&thread_attr);
        return -2;
    }

    if(pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED) != 0)
    {
        printf("setdetachstate failure:%s\n",strerror(errno));
        goto cleanup;
    }

    if(pthread_create(&tid_sendmsg,&thread_attr,fun_sendmsg,(void *)pctr) != 0)
    {
        printf("create thread MSG failure:%s\n",strerror(errno));
        goto cleanup;
    }

    if(pthread_create(&tid_rvmsg,&thread_attr,fun_rvmsg,(void *)pctr) != 0)
    {
        printf("create thread NET failure:%s\n",strerror(errno));
        goto cleanup;
    }

    pthread_attr_destroy(&thread_attr);

    if((ret_fd = epoll_server_start(listenport)) < 0)
    {
        printf("socket failure:%s\n",strerror(errno));
        goto cleanup; 
    }

    while(!g_stop)
    {
        printf("this is the thread CTR\n");
        
        if(pctr->occupy == 0)
        {
            int f_client = -1;
            memset(cmd_buff,0,sizeof(cmd_buff));
            /* 没有程序执行，获取命令 */
            if((f_client = read(ret_fd,cmd_buff,sizeof(cmd_buff))) < 0)
            {
                printf("read failure:%s\n",strerror(errno));
                break;
            }

            else if(f_client == 0)
            {
                printf("client disconnect\n");
                return 0;
            }
            printf("read %s from client\n",cmd_buff);

            //添加请求
            if(pthread_mutex_lock(&ctr.lock) != 0)
            {
                printf("lock failure:%s\n",strerror(errno));
                continue;
            }

            pctr->req = getreq(cmd_buff,sizeof(cmd_buff));
            pctr->srcfd = ret_fd;

            if(pthread_mutex_unlock(&ctr.lock) != 0)
            {
                printf("unlock failure:%s\n",strerror(errno));
                break;
            }

            if(pctr->req == 0)
            {
                continue;
            }

            /***********线程控制************/

            //没有正在执行的线程，直接执行
            if(pctr->exc == 0)
            {
                if(pthread_mutex_lock(&ctr.lock) != 0)
                {
                    printf("lock failure:%s\n",strerror(errno));
                    continue;
                }

                pctr->exc = pctr->req;

                if(pthread_mutex_unlock(&ctr.lock) != 0)
                {
                    printf("unlock failure:%s\n",strerror(errno));
                    break;
                }

            }

            //正在执行的线程exc>0
            else if(pctr->exc > 0)
            {
                //优先级比正在执行的高,打断正在执行的线程
                if(pctr->req > pctr->exc)
                {
                    pthread_cancel(tid_rvmsg);//这里只会出现一种情况sendMSG>rvmsg
                    usleep(1000);

                    if(pthread_mutex_lock(&ctr.lock) != 0)
                    {
                        printf("lock failure:%s\n",strerror(errno));
                        continue;
                    }

                    pctr->exc = PRI_SENDMEG;

                    if(pthread_mutex_unlock(&ctr.lock) != 0)
                    {
                        printf("unlock failure:%s\n",strerror(errno));
                        break;
                    }

                    if(pthread_create(&tid_rvmsg,&thread_attr,fun_rvmsg,(void *)pctr) != 0)
                    {
                        printf("create thread NET failure:%s\n",strerror(errno));
                        goto cleanup;
                    }
                }

                //优先级比正在执行的低,忽略
                else
                {
                    continue;
                }

            }

            else
            {
                printf("Thread is illegal!\n");
                exit(-1);
            }
        }
        
        sleep(2);
        printf("req = %d\nexc = %d\n",pctr->req,pctr->exc);
    }


    closeComport(comport_info,&old_term);
    Comport_term(comport_info);
    pthread_mutex_destroy(&ctr.lock); 

    return 0;
cleanup:
    pthread_mutex_destroy(&ctr.lock);
    pthread_attr_destroy(&thread_attr);
    closeComport(comport_info,&old_term);
    Comport_term(comport_info);
    
    return -3;
}

int getreq(char *buff,int buffsize)
{
    buff[buffsize-1] = '\0';

    if(strcmp(buff,"call") == 0)
    {
        return 3;
    }

    else if(strcmp(buff,"send message") == 0)
    {
        return 2;
    }

    else if(strcmp(buff,"read message") == 0)
    {
        return 1;
    }

    else 
    {
        printf("cmd not define!\n");
        return 0;
    }
}

void *fun_sendmsg(void *p_ctr)
{
    char msgbuff[1024] = {0};
    char rec_nu[12] = {0};
    CTR *temp_pctr = (CTR *)p_ctr;
    printf("this is the thread MSG\n");
    
    while(1)
    {
        //没有命令到来
        if(temp_pctr->exc != PRI_SENDMEG)
        {
            sleep(1);
        }

        else
        {
            printf("begain to send mesage...\n");
            
            judge_modulestate(temp_pctr->serial_fd);
            
            printf("wait for mesage content\n");

            temp_pctr->occupy = 1;

            if(read(temp_pctr->srcfd,msgbuff,sizeof(msgbuff)) < 0)
            {
                printf("read failure:%s\n",strerror(errno));
                continue;
            }

            printf("msgbuff = %s\n",msgbuff);
            printf("wait for recevie number\n");
            
            if(read(temp_pctr->srcfd,rec_nu,sizeof(rec_nu)) < 0)
            {
                printf("read failure:%s\n",strerror(errno));
                continue;
            }

            printf("rec_nu = %s\n",rec_nu);

            if(sendenglish(temp_pctr->serial_fd,rec_nu,msgbuff) < 0)
            {
                continue;
            }

            temp_pctr->occupy = 0;
            //close(temp_pctr->srcfd);

            if(pthread_mutex_lock(&temp_pctr->lock) != 0)
            {
                printf("lock failure:%s\n",strerror(errno));
                continue;
            }

            temp_pctr->req = 0;
            temp_pctr->exc = 0;
            //temp_pctr->srcfd = -1;

            if(pthread_mutex_unlock(&temp_pctr->lock) != 0)
            {
                printf("unlock failure:%s\n",strerror(errno));
                exit(-4);
            }
            printf("thread msg over!\n");
        }

    }
}

void *fun_rvmsg(void *p_ctr)
{
    int oldstate;
    int oldtype;
    CTR *temp_pctr = (CTR *)p_ctr;
    //使能可取消该线程
    if(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&oldstate) != 0)
    {
        printf("setcanclesate failure:%s\n",strerror(errno));
        exit(-2);
    }

    //设置取消类型，异步取消，不遇到取消点
    if(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&oldtype) != 0)
    {
        printf("pthread_setcanceltype failure:%s\n",strerror(errno));
        exit(-3);
    }

    printf("this is the thread read msg\n");
    
    while(1)
    {
        if(temp_pctr->exc != PRI_READMEG)
        {
            sleep(1);
        }

        else
        {
            printf("begain to open unread meg...\n");
            
            judge_modulestate(temp_pctr->serial_fd);
            
            list_msg_txt(temp_pctr->serial_fd,TYPE_REC_UNREAD);

            wait_newmsg_txt(temp_pctr->serial_fd,10);
            
            //close(temp_pctr->srcfd);

            if(pthread_mutex_lock(&temp_pctr->lock) != 0)
            {
                printf("lock failure:%s\n",strerror(errno));
                continue;
            }

            temp_pctr->req = 0;
            temp_pctr->exc = 0;
            //temp_pctr->srcfd = -1;

            if(pthread_mutex_unlock(&temp_pctr->lock) != 0)
            {
                printf("unlock failure:%s\n",strerror(errno));
                exit(-4);
            }

            printf("thread net is over!\n");
        }

    }

}
