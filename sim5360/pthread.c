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

#define PRI_CALL 3
#define PRI_MEG 2
#define PRI_NET 1
typedef struct{
    int req;
    int exc;
    pthread_mutex_t lock;
}CTR;

void *fun_msg(void *p_ctr)
{
    CTR *temp_pctr = (CTR *)p_ctr;
    printf("this is the thread MSG\n");
    while(1)
    {
        //没有命令到来
        if(temp_pctr->exc != PRI_MEG)
        {
            sleep(1);
        }

        else
        {
            printf("begain to send mesage...\n");
            sleep(15);
            if(pthread_mutex_lock(&temp_pctr->lock) != 0)
            {
                printf("lock failure:%s\n",strerror(errno));
                continue;
            }

            temp_pctr->req = 0;
            temp_pctr->exc = 0;

            if(pthread_mutex_unlock(&temp_pctr->lock) != 0)
            {
                printf("unlock failure:%s\n",strerror(errno));
                exit(-4);
            }
            printf("thread msg over!\n");
        }

    }
}
void *fun_net(void *p_ctr)
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

    printf("this is the thread NET\n");
    while(1)
    {
        if(temp_pctr->exc != PRI_NET)
        {
            sleep(1);
        }

        else
        {
            printf("begain to net...\n");
            sleep(15);
            
            if(pthread_mutex_lock(&temp_pctr->lock) != 0)
            {
                printf("lock failure:%s\n",strerror(errno));
                continue;
            }

            temp_pctr->req = 0;
            temp_pctr->exc = 0;

            if(pthread_mutex_unlock(&temp_pctr->lock) != 0)
            {
                printf("unlock failure:%s\n",strerror(errno));
                exit(-4);
            }

            printf("thread net is over!\n");
        }

    }

}

int main(int argc,char *argv[])
{
    CTR ctr;
    char cmd_buff[16];
    pthread_attr_t thread_attr; 
    CTR *pctr = &ctr;
    pctr->req = 0;
    pctr->exc = 0;

    pthread_t tid_msg,tid_net;

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

    if(pthread_create(&tid_msg,&thread_attr,fun_msg,(void *)pctr) != 0)
    {
        printf("create thread MSG failure:%s\n",strerror(errno));
        goto cleanup;
    }

    if(pthread_create(&tid_net,&thread_attr,fun_net,(void *)pctr) != 0)
    {
        printf("create thread NET failure:%s\n",strerror(errno));
        goto cleanup;
    }

    pthread_attr_destroy(&thread_attr);

    while(1)
    {
        printf("this is the thread CTR\n");

        printf("please enter the command with call or send message or net\n");
        printf("please enter the 'q' to quik\n");

        fgets(cmd_buff,sizeof(cmd_buff),stdin);

        if(strcmp(cmd_buff,"q\n") == 0)
        {
            break;
        }

        //添加请求
        if(pthread_mutex_lock(&ctr.lock) != 0)
        {
            printf("lock failure:%s\n",strerror(errno));
            continue;
        }

        pctr->req = getreq(cmd_buff,sizeof(cmd_buff));

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
                pthread_cancel(tid_net);//这里只会出现一种情况MSG>NET
                usleep(1000);

                if(pthread_mutex_lock(&ctr.lock) != 0)
                {
                    printf("lock failure:%s\n",strerror(errno));
                    continue;
                }

                pctr->exc = PRI_MEG;

                if(pthread_mutex_unlock(&ctr.lock) != 0)
                {
                    printf("unlock failure:%s\n",strerror(errno));
                    break;
                }

                if(pthread_create(&tid_net,&thread_attr,fun_net,(void *)pctr) != 0)
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


        printf("req = %d\nexc = %d\n",pctr->req,pctr->exc);
        sleep(1);

    }

    pthread_mutex_destroy(&ctr.lock); 

    return 0;
cleanup:
    pthread_mutex_destroy(&ctr.lock);
    pthread_attr_destroy(&thread_attr);
    return -3;
}

int getreq(char *buff,int buffsize)
{
    buff[strlen(buff)-1] = '\0';

    if(strcmp(buff,"call") == 0)
    {
        return 3;
    }

    else if(strcmp(buff,"send message") == 0)
    {
        return 2;
    }

    else if(strcmp(buff,"net") == 0)
    {
        return 1;
    }

    else 
    {
        printf("cmd not define!\n");
        return 0;
    }
}
