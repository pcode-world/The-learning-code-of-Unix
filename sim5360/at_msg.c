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
        
        ptemp++;

        while( (*ptemp == ' ') || (*ptemp == '\r') || (*ptemp == '\t') || (*ptemp == '\n'))
        {
            ptemp++;
        }

        return ptemp;
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
    if(*times == maxtimes)
    {
        printf("try to testing many times,but was defeated.Please check AT command or restart the module and try again\n");
        exit(1);
    }

}

/******************************************************************************************************
 * Description:send AT command and save return value,just for return OK AT command
 * Input Args:fd,AT command,the location of the data,time to wait
 * Output Args:Data read from a serial port
 * Return Value:none
 ******************************************************************************************************/
void AT_test(int comport_fd,char *AT_command,char *readbuff,int readbuffsize,int sleeptime)
{
    int i = 0;

    int ret = -1;
    while(1)
    {
        try_times(&i,10);
        //printf("+++++Execute the AT command once +++++\n");
        
#if _DEBUG
        printf("excute : %s,length = %d\n",AT_command,strlen(AT_command));
#endif
        while(writeComport(comport_fd,AT_command,strlen(AT_command)) < 0)
        {
            sleep(sleeptime);
            try_times(&i,10);
        }
       
        /* 一定要读到AT指令的返回值,无回显：+OK一定大于3;有回显：大于命令长度 */
        if((ret = readComport(comport_fd,readbuff,readbuffsize)) < 3)
        {
            sleep(sleeptime);
            continue;
            try_times(&i,10);
        }

        if(seek_AT_str(readbuff,"ERROR",AT_SEARCH) != NULL)            
        {
            printf("AT command execution error, please check whether the command is fought, if correct, please ignore:\n");
#if _DEBUG
            printf("command retbuff = %s",readbuff);
#endif
            sleep(sleeptime);
            continue;
        }


#if _DEBUG
        printf("command retbuff = %s,length = %d\n",readbuff,strlen(readbuff));
            int f = strlen(readbuff);
            for(f;f != 0;f--)
            {
                printf("%02x ",*(unsigned char *)readbuff);
                readbuff++;
            }
            printf("\n");
#endif
        break;

    }

}

/* 判断模块状态 */
int judge_modulestate(int comport_fd)
{
    int int_times = 0;
    char temp[128];
    char AT_echo[] = "ATE0\r\n";
    char AT_hand[] = "AT\r\n";
    char AT_sim[] = "AT+CPIN?\r\n";
    char AT_register[] = "AT+CREG?\r\n";
    char AT_rssi[] = "AT+CSQ\r\n";
    char *pfind = NULL;

    while(1)
    {
        printf("begain to cheching module state\n");
        try_times(&int_times,10);
        memset(temp,0,sizeof(temp));

        /* 关闭/打开回显 */
        AT_test(comport_fd,AT_echo,temp,sizeof(temp),1);
        printf("close echo ok\n");
        /* 与串口连通 */

        AT_test(comport_fd,AT_hand,temp,sizeof(temp),1);
        printf("hand ok\n");

        /* SIM卡在位 */
        AT_test(comport_fd,AT_sim,temp,sizeof(temp),10);

        if(seek_AT_str(temp,"READY",AT_SEARCH) == NULL)
        {
            sleep(5);
            continue;
        }

        printf("sim card ok\n");

        /* 网络注册上 */
        AT_test(comport_fd,AT_register,temp,sizeof(temp),1);

        if((pfind = seek_AT_str(temp,",",AT_SEEK)) == NULL)
        {
            continue;
        }

        if((atoi(pfind) != 5) && (atoi(pfind) != 1))
        {
            printf("The network is not registered\n");
            sleep(3);
            continue;
        }

        printf("registe ok\n");

        /* 信号强度足够使用 */
        AT_test(comport_fd,AT_rssi,temp,sizeof(temp),1);

        if((pfind = seek_AT_str(temp,":",AT_SEEK)) == NULL)
        {
            continue;
        }

        if(atoi(pfind) < 2 || atoi(pfind) == 99)
        {
            printf(" signal strength = %s not be enough to use\n",pfind);
            sleep(3);
            continue;
        }

        printf("rssi ok\n");
        
        return 0;
    }

}


/******************************************************************************************************
 * Description:send message with english
 * Input Args:Content of want to sent
 * Output Args:none
 * Return Value:success 0,error -1
 ******************************************************************************************************/
int sendenglish(int comport_fd,unsigned char *telnu,char *msg)
{
    int waittimes = 0;
    char retbuff[32] = {0};
    char AT_msg_pattern[] = "AT+CMGF=1\r\n";
    char AT_choose[32] = "AT+CMGS=\"";
    char writebuff[1024];
    char *pfind = NULL;
    unsigned char tel[16] = {0};
    int i = 0;

    if(strlen(telnu) > sizeof(tel))
    {
        printf("the receive number  too long \n");
        return -1;
    }

    if(msg == NULL)
    {
        return -1;
        printf("The message content cannot be empty\n");
    }
    
    while(((*telnu) < ':') && ((*telnu) > '/'))
    {
        tel[i] = (*telnu);
        i++;
        telnu++;
    }
    
    strncat(AT_choose,tel,strlen(tel));

    AT_choose[strlen(AT_choose)] = '\"';
    AT_choose[strlen(AT_choose)] = '\r';
    AT_choose[strlen(AT_choose)] = '\n';
   
#if _DEBUG
    printf("receiver number = %s\n",AT_choose);
#endif
    if(strlen(msg) > sizeof(writebuff)-2)
    {
        printf("the message too long \n");
        return -1;
    }

    strncpy(writebuff,msg,strlen(msg));
    
    while(1)
    {
        memset(retbuff,0,sizeof(retbuff));

        AT_test(comport_fd,AT_msg_pattern,retbuff,sizeof(retbuff),1);
        
        if(strlen(telnu)+3 > sizeof(AT_choose)-13)
        {
            printf("the receiver number too long,Please enter again\n");
            return -1;
        }

        AT_test(comport_fd,AT_choose,retbuff,sizeof(retbuff),1);

        if(strstr(retbuff,">") != NULL)
        {
            writebuff[strlen(writebuff)-1] = 0x1A;//输入以ctr+z结束，0x1A
            /* 只写strlen(writebuff)   不会将0x1A写入，不会结尾 */
            while(writeComport(comport_fd,writebuff,strlen(writebuff)+3) <  0)
            {
                try_times(&waittimes,5);
            }

            printf("write OK\n");
            /* 检测是否发送成功 */
            while(1)
            {
                readComport(comport_fd,retbuff,sizeof(retbuff));
                printf("retbuff = %s\n",retbuff);
                
                waittimes++;
                    
                if(waittimes == 10)
                {
                    waittimes = 0;
                    printf("send message failure,pealse try again!\n");
                    return -1;
                }

                if((pfind = seek_AT_str(retbuff,"OK",AT_SEARCH)) == NULL)
                {
                    sleep(1);
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

int sendchinese(int comport_fd,char *sendbuff,int sendbuffsize)
{
    /* ... */
    return 0;
}

int read_msg_index_txt(int fd,char *retbuff,int buffsize,int index)
{
    int loops = 5;
    char AT_cmgr[16] = {0};//AT+CMGR=index读内存下标为index的短信
    snprintf(AT_cmgr,sizeof(AT_cmgr),"AT+CMGR=%d\r\n",index);
    while(loops != 0)
    {
        memset(retbuff,0,buffsize);
        AT_test(fd,AT_cmgr,retbuff,buffsize,1);

        if(seek_AT_str(retbuff,"+CMGR:",AT_SEARCH) == NULL)
        {
            continue;
            loops--;
        }
        printf("OoO read message from mermory successful\n");
        return 0;
    }

    return -1;
}

int msg_output(char *buff,int me_indexl)
{
    int len;
    char *pfind1 = NULL;
    char *pfind2 = NULL;
  
    if(buff == NULL)
    {
        return -1;
    }
    
    /* 内存中的下标 */
    printf("------------------------------------------------\n");
    printf("memory index:%d\n",me_indexl);
    
    if((pfind1 = seek_AT_str(buff,"+CMGR:",AT_SEEK)) == NULL)
    {
        return -1;
    }
    /* 状态 */
    if((pfind2 = seek_AT_str(pfind1,":",AT_SEEK)) == NULL)
    {
        return -1;
    }
    pfind2++;
    //printf("pfind = %c\n",*pfind2);
    if((*pfind2) == 'R')
    {
        pfind2+=4;

        if(*pfind2 == 'U')
        {
            printf("state:received unread\n");
        }

        else
            printf("state:received read\n");
    }

    else if((*pfind2) == 'S')
    {
        pfind2+=4;
        
        if(*pfind2 == 'U')
        {
            printf("state:save but not send\n");
        }
     
        else
            printf("state:save and send\n");
    }

    else
        printf("state:all message\n");
    /* 发送方 */
    if((pfind1 = seek_AT_str(pfind2,"+",AT_SEEK)) == NULL)
    {
        return -1;
    }
    pfind1+=2;
    printf("the number of the send this message:");
    while((*pfind1) != '"')
    {
        printf("%c",*pfind1);
        pfind1++;
    }
    printf("\n");

    /* 发送时间 */
    if((pfind2 = seek_AT_str(pfind1,"/",AT_SEEK)) == NULL)
    {
        return -1;
    }
    pfind2 -=3;
    printf("send data:20");
    while((*pfind2) != '+')
    {
        printf("%c",*pfind2);
        pfind2++;
    }
    *pfind2 = ' ';
    printf("\n");

    /* 大小 */
    printf("size:%d\n",atoi(pfind2));

    /* 内容 */
    printf("message content:");
    if((pfind1 = seek_AT_str(pfind2,"\"",AT_SEEK)) == NULL)
    {
        return -1;
    }
    pfind2 =pfind1;
    len = strlen(pfind1);
    *(pfind2+len-1) = '\0';
    *(pfind2+len-2) = '\0';
    *(pfind2+len-3) = '\0';
    *(pfind2+len-4) = '\n';//去掉末尾OK
    printf("%s",pfind1);
    printf("------------------------------------------------\n");
    return 0;
}

void del_msg(int comport_fd,int index)
{
    int times = 0;
    char readbuff[16] = {0};
    char AT_del[16] = {0};
    
    snprintf(AT_del,sizeof(AT_del),"AT+CMGD=%d\r\n",index);
 
    while(1)
    {
        try_times(&times,10);
        AT_test(comport_fd,AT_del,readbuff,sizeof(readbuff),1);

        if(seek_AT_str(readbuff,"OK",AT_SEARCH) == NULL)
        {
            continue;
        }
        printf("delete message ME[%d] ok\n",index);
        break;
    }
}

int wait_newmsg_txt(int comport_fd,int waittimes)
{
    char AT_cpms[] = "AT+CPMS=\"ME\",\"ME\",\"ME\"\r\n";//设置信息载体
    char AT_cmgf[] = "AT+CMGF=1\r\n";//txt模式
    char AT_cnmi[] = "AT+CNMI=2,1\r\n";//通知
    char readbuff[1024] = {0};
    int  me_indexl = -1;
    int  read_ret = -1;
    char *pfind = NULL;

    while(1)
    {
        printf("begain to set tell pattern\n");
        memset(readbuff,0,sizeof(readbuff));
        //try_times(&loop_times,2);

        AT_test(comport_fd,AT_cpms,readbuff,sizeof(readbuff),1);
            
        if(seek_AT_str(readbuff,"OK",AT_SEARCH) == NULL)
        {
            continue;
        }

        AT_test(comport_fd,AT_cmgf,readbuff,sizeof(readbuff),1);
            
        if(seek_AT_str(readbuff,"OK",AT_SEARCH) == NULL)
        {
            continue;
        }

        AT_test(comport_fd,AT_cnmi,readbuff,sizeof(readbuff),1);

        if(seek_AT_str(readbuff,"OK",AT_SEARCH) == NULL)
        {
            continue;
        }

        /* 等待串口出现提示+CMTI:"ME",index */
        memset(readbuff,0,sizeof(readbuff));

        printf("waitting for message come...\n");
            
        while(1)
        {
            read_ret = readComport_notime(comport_fd,readbuff,sizeof(readbuff));
            if(read_ret < 0)
            {
                printf("read comport failure:%s\n",strerror(errno));
                return -1;
            }

            else if(read_ret == 0)
            {
                continue;
            }

            else
            {
                printf("readbuff = %s",readbuff);
                
                if((seek_AT_str(readbuff,"+CMTI:",AT_SEARCH) != NULL))
                {
                    printf("new message coming\n");
                    pfind = seek_AT_str(readbuff,",",AT_SEEK);
                    me_indexl = atoi(pfind);
                    printf("me_indexl = %d\n",me_indexl);
                    memset(readbuff,0,sizeof(readbuff));
            
                    if(read_msg_index_txt(comport_fd,readbuff,sizeof(readbuff),me_indexl) < 0)
                    {
                        printf("read failure\n");
                        return -1;
                    }
                    
                    if(msg_output(readbuff,me_indexl) < 0) 
                    {
                        printf("output failure\n");
                        return -1;
                    }
                        
                    del_msg(comport_fd,me_indexl);
                    return 0;
                }

                continue;
            }
       
            sleep(waittimes);
        }

    }
}


int list_msg_txt(int comport_fd,int type)
{
    int arr_index[255];//创建一个与内存大小相当的数组用来存放未读短信的下标
    char AT_cmgf[] = "AT+CMGF=1\r\n";//txt模式
    char AT_cmgl[32] = {0};//列出相关短信的命令数组
    char readbuff[1024];
    int loop_times = 0;
    char *pfind = NULL;
    int index = 0;

    switch(type)
    {
        case TYPE_REC_UNREAD:
            strcpy(AT_cmgl,"AT+CMGL=\"REC UNREAD\"\r\n");
            break;
        case TYPE_REC_READ:
            strcpy(AT_cmgl,"AT+CMGL=\"REC READ\"\r\n");
            break;
        case TYPE_STO_UNSENT:
            strcpy(AT_cmgl,"AT+CMGL=\"STO UNSENT\"\r\n");
            break; 
        case TYPE_STO_SENT:
            strcpy(AT_cmgl,"AT+CMGL=\"STO SENT\"\r\n");
            break;
        case TYPE_ALL:
            strcpy(AT_cmgl,"AT+CMGL=\"ALL\"\r\n");
            break;
    }
    while(1)
    {
        memset(readbuff,0,sizeof(readbuff));
        memset(arr_index,-1,sizeof(arr_index));
        try_times(&loop_times,5);
            
        AT_test(comport_fd,AT_cmgf,readbuff,sizeof(readbuff),1);
        if(seek_AT_str(readbuff,"OK",AT_SEARCH) == NULL)
        {
            continue;
        }

        AT_test(comport_fd,AT_cmgl,readbuff,sizeof(readbuff),2);

        if(seek_AT_str(readbuff,"ERROR",AT_SEARCH) != NULL)
        {
            continue;//命令出错

        }

        /* 获取未读短信内存下标 */
        if((pfind = seek_AT_str(readbuff,"+CMGL:",AT_SEARCH)) == NULL)
        {
            printf("no unread text message\n");//找不到未读短信
            return -1;
        }

        else
        {
            /* 函数调用完指针指向 + */
            while((pfind = seek_AT_str(pfind,"+CMGL:",AT_SEARCH)) != NULL)
            {
                pfind+=6;//指向"后
                
                while( (*pfind == ' ') || (*pfind == '\r') || (*pfind == '\t') || (*pfind == '\n'))
                {
                    pfind++;
                }
                
                arr_index[index] = atoi(pfind);
                printf("arr_index[%d] = %d\n",index,arr_index[index]);
                index++;
            }
        }


        /* 分条读取短信 */
        index = 0;

        while(arr_index[index] >= 0)
        {
            memset(readbuff,0,sizeof(readbuff));
            
            if(read_msg_index_txt(comport_fd,readbuff,sizeof(readbuff),arr_index[index]) < 0)
            {
                continue;
            }
            
            if(msg_output(readbuff,arr_index[index]) < 0)
            {
                return -1;
            }

            del_msg(comport_fd,arr_index[index]);
            index++;

        }

        return 0;
    }
}
