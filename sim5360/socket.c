/*********************************************************************************
 *      Copyright:  (C) 2019 wujinlong<547124558@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket.c
 *    Description:  This file create socket fd
 *                 
 *        Version:  1.0.0(2019年07月24日)
 *         Author:  wujinlong <547124558@qq.com>
 *      ChangeLog:  1, Release initial version on "2019年07月24日 11时31分32秒"
 *                 
 ********************************************************************************/
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

#define BACKLOG 13

typedef struct _str_socket
{
    int domain;
    int type;
    uint32_t port;
    char ip[32];
}str_socket;

str_socket *socket_init(int arg_domain,int arg_type,uint32_t arg_port,char *arg_ip)
{
    str_socket *pstr_socket;

    if((pstr_socket = (str_socket *)malloc(sizeof(str_socket))) == NULL)
    {
        return NULL;
    }
    
    memset(pstr_socket,0,sizeof(str_socket));
    pstr_socket->domain = arg_domain;
    pstr_socket->type = arg_type;
    pstr_socket->port = arg_port;
    strncpy(pstr_socket->ip,arg_ip,32);

    return pstr_socket;
}

void socket_destroy(str_socket **p)
{
    free(*p);
    *p == NULL;
}

int create_socket_server(str_socket *socket_info)
{
    int fd = -1;
    struct sockaddr_in info;
    struct sockaddr_in client_addr;
    int new_fd = -1;
    char buf[64] = {0};
    socklen_t addrlen;
    
    if(socket_info == NULL)
    {
        return -1;
    }

    fd = socket(socket_info->domain,socket_info->type,0);

    if(fd < 0)
    {
        printf("create socket failure: %s\n", strerror(errno)); 
        return -2;
    }

    if(socket_info->port != 0)
    {
        info.sin_family = socket_info->domain;
        info.sin_port = htonl(socket_info->port);

        if(socket_info->ip == NULL)
        {
            info.sin_addr.s_addr = htonl(INADDR_ANY);
        }

        else
        {
            info.sin_addr.s_addr = inet_addr(socket_info->ip);
        }

        memset(&(info.sin_zero),0,8);

        if(bind(fd,(struct sockaddr *)&info,sizeof(info)) < 0)
        {
            printf("bind failure:%s\n",strerror(errno));
            //printf("socket_info->ip = %s\n",socket_info->ip);
            return -3;
        }
    }

    else
    {
        printf("please input the port\n");
        return -4;
    }

    if(listen(fd,BACKLOG) < 0)//int listen(int sockfd, int backlog);
    {
        printf("listen failure:%s\n",strerror(errno));
        return -5;
    }

    while(1)
    {
        printf("Waitting for client connection...\n");

        new_fd = accept(fd,(struct sockaddr *)&client_addr,&addrlen);

        if(new_fd<0)
        {
            printf("accept a new client failure;%s\n",strerror(errno));
            continue;
        }

        printf("connected\n");
    read(new_fd,buf,sizeof(buf));

    printf("buf = %s\n",buf);
    close_socket_fd(new_fd);

    return fd;

    }


}

int close_socket_fd(int fd)
{
    close(fd);
}


int main()
{
    int server_fd = -1;
    str_socket *psocket_info;
    
    if((psocket_info = socket_init(AF_INET,SOCK_STREAM,6421,"127.0.0.1")) == NULL)
    {
        return -1;
    }

    create_socket_server(psocket_info);

    close_socket_fd(server_fd);
    socket_destroy(&psocket_info);

    return 0;

}
