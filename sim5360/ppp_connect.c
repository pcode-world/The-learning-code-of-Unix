/*********************************************************************************
 *      Copyright:  (C) 2019 wujinlong<547124558@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  ppp_connect.c
 *    Description:  This file function of ppp_connect need
 *                 
 *        Version:  1.0.0(2019年07月26日)
 *         Author:  wujinlong <547124558@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年07月26日 12时34分32秒"
 *                 
 ********************************************************************************/
#include "ppp_connect.h"


/******************************************************************************************************
 * Description:Modifies the contents of a file at the specified string location
 * Input Args:open FILE *fp,String to locate,Content to be changed,relative location,
 *       relay_loca:
 *                 RELAY_CUR 1:Changes from the position where the specified string begins
 *                 RELAY_END 2:Modifies from the end of the specified string
 *                 use '\0' to fill.
 * Output Args:none
 * Return Value:error -1;true or not change 0
 ******************************************************************************************************/
int modify_file(FILE *fp,char *seekstr,char *newstr,int relay_loca)
{
    long file_len = 0;
    long fileseek = 0;
    char temp[1024] = {0};
    char buff[1024] = {0};
    char *p_str = NULL;

    if(seekstr == NULL)
    {
        return 0;
    }

    fseek(fp, 0, SEEK_END);//结尾

    if((file_len = ftell(fp)) == -1)
    {
        return -1;

    }

    fseek(fp, 0, SEEK_SET);//开头

    while(fgets(buff,sizeof(buff),fp) != NULL)
    {
        if(buff[sizeof(buff)-1] != '\0')
        {
            printf("The contents of the read line are too long\n");
            return -1;
        }

        if((p_str = strstr(buff,seekstr)))
        {
            long temp_seek = 0;
            long nextline = 0;
            size_t size = file_len-ftell(fp)+2;
            void *p_mem = NULL;

            nextline = ftell(fp);
            
            if(!(p_mem = malloc(size)))
            {
                return -1;
            }

            memset(p_mem,0,size);
            temp_seek = ftell(fp) - strlen(p_str);//记录文件偏移量

            /* size_t fread(void *buf, size_t size, size_t count, FILE *fp);
             * size_t fwrite(const void * buf, size_t size, size_t count, FILE *fp);*/
            fread(p_mem,1,file_len-ftell(fp),fp);//读到分配的内存

            fileseek = ftell(fp);
            
            if(RELAY_CUR == relay_loca)
            {
                fseek(fp,temp_seek, SEEK_SET);
            }

            else if(RELAY_END == relay_loca)
            {
                fseek(fp,temp_seek+strlen(seekstr), SEEK_SET);

            }

            else
            {
                return -1;
            }

            fprintf(fp,"%s",newstr);//写入
            fwrite(p_mem,1,size,fp);//写回
            
            if((fileseek = ftell(fp)) < file_len)
            {
                while((fileseek = ftell(fp)) != file_len)
                {
                    fprintf(fp,"%c",'\0');
                }
            }

            fileseek = ftell(fp);
            free(p_mem);
            p_mem = NULL;
            fseek(fp, 0, SEEK_END);//结尾

            if((file_len = ftell(fp)) == -1)
            {
                return -1;

            }

            return 0;
        }

        memset(temp,0,sizeof(temp));
    }

}

/******************************************************************************************************
 * Description:Get the operator through cimi
 * Input Args:AT command return string,and length
 * Output Args:none
 * Return Value:error -1;China Mobile 1;China Unicom 2;China Telecom  3;China Tietong 4
 ******************************************************************************************************/

int id_isp(char *cimi,int strsize)
{
    int i = 0;

    if(cimi == NULL)
    {
        return -1;
    }

    while( (*cimi == ' ') || (*cimi == '\r') || (*cimi == '\t') || (*cimi == '\n'))
    {
        cimi++;
        i++;
        if(i == strsize)
        {
            return -1;
        }
    }

    if(*(cimi++) == '4' && *(cimi++) == '6' && *(cimi++) == '0')
    {
        if((*cimi) == '2')
        {
            return 4;
        }

        cimi++;
        switch(*cimi)
        {
            case '0':
            case '2':
            case '7':
                return 1;
            case '1':
            case '6':
                return 2;
            case '3':
            case '5':
                return 3;
            default:
                return -1;
        }
    }

    else
    {
        printf("welcome to china!please use MCC=460\n");
        return -1;
    }
    //CIMI = MCC+MNC+MSIN   460 01 55
    //中国移动系统使用00,02,07,中国联通GSM系统使用01,06,中国电信CDMA系统使用03,05,中国铁通系统使用20;
}

int set_apn_call(int id_isp,FILE *fp)
{

    char buff[128] = {0};
    char AT_CGDCONT[64] = {0};
    char AT_ATD[16] = {0};
    long fileseek = 0;
    char temp[128] = {0};
    char bak[1024] = {0};

    if(id_isp < 1 && fp == NULL)
    {
        return -1;
    }

    switch(id_isp)
    {
        case 1:
            snprintf(AT_CGDCONT,sizeof(AT_CGDCONT),"AT+CGDCONT=1,\"IP\",\"CMNET\",,0,0");
            snprintf(AT_ATD,sizeof(AT_ATD),"*99***1#");
            break;
        case 2:
            snprintf(AT_CGDCONT,sizeof(AT_CGDCONT),"AT+CGDCONT=1,\"IP\",\"3GNET\",,0,0");
            snprintf(AT_ATD,sizeof(AT_ATD),"*99#    ");
            break;
        case 3:
            snprintf(AT_CGDCONT,sizeof(AT_CGDCONT),"AT+CGDCONT=1,\"IP\",\"     \",,0,0");
            snprintf(AT_ATD,sizeof(AT_ATD),"#777    ");
            break;
        default:
            return -1;
    }

}

/*  int set_acont_pwd(int id_isp,FILE *fp)
    {
    char buff[128] = {0};
    long fileseek = 0;
    FILE *fp;
    char temp[128] = {0};

    if(id_isp < 1)
    {
    return -1;
    }

    switch(id_isp)
    {
    case 1:
    break;
    case 2:
    break;
    case 3:
    default:
    return -1;
    }

    }

    int connect_gprs()
    {
    }

    int set_route(char *gateway)
    {}
    int ppp_init(char *script_file_path,char *At_serial_path)
    {}
    int ppp_destroy()
    {}
    int ppp_connect()
    {}
    int ppp_disconnect()
    {}*/

