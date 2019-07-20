/*********************************************************************************
 *      Copyright:  (C) 2019 wujinlong<547124558@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  at_msg.c
 *    Description:  This file is send AT command function
 *                 
 *        Version:  1.0.0(2019年07月18日)
 *         Author:  wujinlong <547124558@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年07月18日 15时30分36秒"
 *                 
 ********************************************************************************/

#include "comport.h"
#include "at_msg.h"
/******************************************************************************************************
 * Description:Find the first address of the input expected string
 * Input Args:Specified AT command string and pattern
 *            pattern AT_SEARCH:just Search string 
 *            pattern AT_SEEK:Returns the address of the last non-null character of the specified string
 * Output Args:none
 * Return Value:the first address of the input expected string
 ******************************************************************************************************/
char *seek_AT_str(char *spe_str,char *exp_str,int pattern)
{
    char *ptemp = NULL;
    if(strlen(spe_str) < strlen(exp_str) || spe_str == NULL)
    {
        printf("The specified string is too long\n");
        return NULL;
    }

    if(pattern == AT_SEARCH)
    {
        return strstr(spe_str,exp_str);
    }
    
    else if(pattern == AT_SEEK)
    {
        if((ptemp = strstr(spe_str,exp_str)) == NULL)
        {
            printf("not find\n");
            return NULL;
        }
        while( (*ptemp == ' ') || (*ptemp == '\r') || (*ptemp == '\t') || (*ptemp == '\n'))
        {
            ptemp++;
            printf("ptemp = %c\n",*ptemp);
        }
        return ++ptemp;
    }

    else
    {
        printf("pattern not define\n");
        return NULL;
    }
}

/* 尝试次数 */
void try_times(int *times,int maxtimes)
{
    (*times)++;
    sleep(5);
    if(*times == maxtimes)
    {
        printf("try to testing many times,but was defeated.Please check it and restart the module and try again\n");
        exit(1);
    }

}

/******************************************************************************************************
 * Description:send AT command and save return value
 * Input Args:fd,AT command,the location of the data,time to wait
 * Output Args:Data read from a serial port
 * Return Value:none
 ******************************************************************************************************/
int AT_test(int comport_fd,char *AT_command,char *readbuff,int readbuffsize,int sleeptime)
{
    int i = 0;
    while(1)
    {
        
        try_times(&i,50);
        if(writeComport(comport_fd,AT_command,strlen(AT_command)) < 0)
        {
            sleep(sleeptime);
            continue;
        }
       
        /* 一定要读到AT指令的返回值,无回显：+OK一定大于3;有回显：大于命令长度 */
        if(readComport(comport_fd,readbuff,readbuffsize) < 3)
        {
            sleep(sleeptime);
            continue;
        }

        printf("readbuff = %s\n",readbuff);
        printf("exctue +++++\n");
        return 0;

    }

}
/* 判断模块状态 */
int judge_modulestate(int comport_fd)
{
    int int_times = 0;
    char temp[512];
    char AT_echo[] = "ATE0\r\n";
    char AT_hand[] = "AT\r\n";
    char AT_sim[] = "AT+CPIN?\r\n";
    char AT_register[] = "AT+CREG?\r\n";
    char AT_rssi[] = "AT+CSQ\r\n";
    char *pfind = NULL;

    while(1)
    {
        try_times(&int_times,15);
        memset(temp,0,sizeof(temp));

        /* 关闭/打开回显 */
        AT_test(comport_fd,AT_echo,temp,sizeof(temp),1);
        if(seek_AT_str(temp,"OK",AT_SEARCH) == NULL)
        {
            continue;
        }
        printf("close echo ok\n");
        /* 与串口连通 */
        AT_test(comport_fd,AT_hand,temp,sizeof(temp),1);

        if(seek_AT_str(temp,"OK",AT_SEARCH) == NULL)
        {
            continue;
        }

        printf("hand ok\n");

        /* SIM卡在位 */
        AT_test(comport_fd,AT_sim,temp,sizeof(temp),10);

        if(seek_AT_str(temp,"READY",AT_SEARCH) == NULL)
        {
            continue;
        }

        printf("sim card ok\n");

        /* 网络注册上 */
        AT_test(comport_fd,AT_register,temp,sizeof(temp),1);

        if((pfind = seek_AT_str(temp,",",AT_SEEK)) == NULL)
        {
            printf("continue\n");
            continue;
        }

        if((atoi(pfind) != 5) && (atoi(pfind) != 1))
        {
            printf("pfind = %s\n",pfind);
            pfind = NULL;
            continue;
        }

        printf("registed \n");

        /* 信号强度足够使用 */
        AT_test(comport_fd,AT_rssi,temp,sizeof(temp),1);

        if((pfind = seek_AT_str(temp,":",AT_SEEK)) == NULL)
        {
            continue;
        }

        if(atoi(pfind) < 2 || atoi(pfind) == 99)
        {
            printf("pfind = %s\n",pfind);
            pfind = NULL;
            continue;
        }

        printf("rssi ok\n");

        break;
    }

    return 0;

}


/******************************************************************************************************
 * Description:send message with english
 * Input Args:Content of want to sent
 * Output Args:none
 * Return Value:success 0,error -1
 ******************************************************************************************************/
int sendenglish(int comport_fd,char *sendbuff,int sendbuffsize)
{
    int waittimes = 0;
    char AT_msg_pattern[] = "AT+CMGF=1\r\n";
    char AT_choose[32] = "AT+CMGS=\"";
    char readbuff[1024];
    char *pfind = NULL;

    while(1)
    {
        memset(readbuff,0,sizeof(readbuff));
        AT_test(comport_fd,AT_msg_pattern,readbuff,sizeof(readbuff),1);

        if((pfind = seek_AT_str(readbuff,"OK",AT_SEARCH)) == NULL)
        {
            printf("pfind = %s\n",pfind);
            continue;
        }

        printf("Please enter the receiver number\n");
        memset(readbuff,0,sizeof(readbuff));
        fgets(readbuff,sizeof(readbuff),stdin);
        
        if(strlen(readbuff)+3 > sizeof(readbuff))
        {
            printf("the receiver number too long,Please enter again\n");
            continue;
        }

        readbuff[strlen(readbuff)-1] = '\"';
        readbuff[strlen(readbuff)] = '\r';
        readbuff[strlen(readbuff)+3] = '\n';
        strncat(AT_choose,readbuff,16);//短信号码可以为11 - 13位+3

        AT_test(comport_fd,AT_choose,readbuff,sizeof(readbuff),1);

        if(seek_AT_str(readbuff,">",AT_SEARCH) != NULL)
        {
            printf("Please enter the message\n");
            memset(readbuff,0,sizeof(readbuff));
            fgets(readbuff,sizeof(readbuff),stdin);
            readbuff[strlen(readbuff)-1] = 0x1A;//输入以ctr+结束，0x1A
            while(writeComport(comport_fd,readbuff,strlen(readbuff)+5) <  0)
            {
                try_times(&waittimes,10);
            }

            while(1)
            {
                readComport(comport_fd,readbuff,sizeof(readbuff));

                if((pfind = seek_AT_str(readbuff,"OK",AT_SEARCH)) == NULL)
                {
                    sleep(1);
                    waittimes++;
                    
                    if(waittimes == 30)
                    {
                        waittimes = 0;
                        printf("send message failure,pealse try again!\n");
                        return -1;
                    }

                    continue;
                }

                break;
            }
            printf("OoO send message succeed\n");
            return 0;
        }
        else
        {
            continue;
        }

    }
    return -1;

}

int sendchinese(char *sendbuff,int sendbuffsize)
{
    /* ... */
    return 0;
}
