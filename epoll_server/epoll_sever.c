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
#include <sqlite3.h>
#include <string.h>
#include <sys/epoll.h>
#include <poll.h>

#define MAXEVENT 512
#define BACKLOG 13
#include "epoll_sever.h"
#include "datadell.c"

int g_cilentstop=0;
void sig_usr(int signum)
{
	if(g_cilentstop == signum)
	{
		g_cilentstop=1;
	}
}

int main(int argc,char *argv[])

{
    if(daemon(0,0) < 0)
    {
        printf("program use daemon fafailure:%s\n",strerror(errno));
    }
    extern char *optarg;
    extern int optind, opterr, optopt;
    int ret;
    int LISTENPORT=0;

    while ((ret = getopt(argc, argv, "p:h::")) != -1)
    {
        switch(ret)
        {
            case 'p':
                LISTENPORT=atoi(optarg);
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

    if(!LISTENPORT)
    {
            printfhelp();
            return -1;
    }
    int listen_fd,epfd;
    struct sockaddr_in serveraddr;
    serveraddr.sin_family=AF_INET;//协议族
    serveraddr.sin_port=htons(LISTENPORT);  //端口绑定
    serveraddr.sin_addr.s_addr=INADDR_ANY;
    bzero(&(serveraddr.sin_zero), 8);

    /*struct _ctr_pthread ctr_pthread;
    struct _ctr_pthread *p_arg=&ctr_pthread;*/
    int new_fd=-1;
    struct sockaddr_in client_addr;
    socklen_t addrlen;

    signal(SIGUSR2, sig_usr);

    listen_fd = socket(AF_INET,SOCK_STREAM,0);
    if(listen_fd < 0)
    {
        printf("creat socket failure:%s\n",strerror(errno));
        return -1;
    }
    
    if(bind(listen_fd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0) 
	    //int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
    {
	    printf("bind failure:%s\n",strerror(errno));
	    return -2;
    }
    if(listen(listen_fd,BACKLOG) < 0)
	    //int listen(int sockfd, int backlog);
    {
	    printf("listen failure:%s\n",strerror(errno));
	    return -3;
    }

    if(daemon(0,0) < 0)
    {   
	    printf("program use daemon fafailure:%s\n",strerror(errno));
    }

    if((epfd = epoll_create(MAXEVENT)) < 0) 
    {
	    printf("create a epfd failure:%s\n",strerror(errno)); return -4;
    }

	struct epoll_event ev; 
	struct epoll_event evlist[MAXEVENT];

	/****struct epoll_event define*****/
	  /* typedef union epoll_data
	   *{
	   *	void        *ptr;    / Pointer to user-defind data /
	   *	int        fd;    / File descriptor /
	   *	uint32_t    u32;    / 32-bit integer /
	   *	uint64_t    u64;    / 64-bit integer / 
	   *} epoll_data_t;
	   *struct epoll_event
	   *{
	   *	   uint32_t events; / epoll events(bit mask) /
	   *	   epoll_data_t data; / User data / 
	   *};
	   */

	int epnu = -1;
	ev.events = EPOLLIN;
	ev.data.fd = listen_fd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,listen_fd,&ev);
	//int epoll_ctl(int epfd, int op, int fd, struct epoll_event *ev)
	//int epoll_wait(int epfd, struct epoll_event *evlist, int maxevents, int timeout);
	//evlist检测到事件，将所有就绪的事件从内核事件表中复制到它的第二个参数指向的数组中。
        while(!g_cilentstop)
        {
                printf("Waitting for client connection...\n");
		if((epnu = epoll_wait(epfd,evlist,MAXEVENT,-1)) < 0)
		{
			printf("epoll_wait to failure:%s\n",strerror(errno));
			continue;
		}
		else if(epnu == 0)
		{
			printf("epoll wait timeout\n");
		}
		int i = 0;
		for(i;i<epnu;i++)
		{
			/***evlist中的fd出错***/
			if((evlist[i].events & EPOLLERR) || (evlist[i].events & POLLHUP))
			{
				printf("epoll_wait get a error fd[%d]: %s\n",evlist[i].data.fd, strerror(errno));
				epoll_ctl(epfd, EPOLL_CTL_DEL,evlist[i].data.fd, NULL);
				close(evlist[i].data.fd); 

			}
			else if(evlist[i].data.fd == listen_fd)
			{
				new_fd=accept(listen_fd,(struct sockaddr *)&client_addr,&addrlen);
				//int accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen);
				//new_fd,inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
				if(new_fd<0)
				{
					printf("accept a new client failure;%s\n",strerror(errno));
					continue; 
				}
				printf("accept a new cilent:fd[%d]\n",new_fd);
				ev.events = EPOLLIN;
				ev.data.fd = new_fd;
				if(epoll_ctl(epfd,EPOLL_CTL_ADD,new_fd,&ev) < 0)
				{
					printf("add a client fd to epoll failure;%s\n",strerror(errno));
					close(new_fd);
					continue;
				}
				
			}
			/***have connected fd***/
			else
			{
				if(datadell(evlist[i].data.fd) < 0)
				{
					printf("can't write to sqlite\n");
					continue;
				}
				close(evlist[i].data.fd);
				epoll_ctl(epfd, EPOLL_CTL_DEL,evlist[i].data.fd, NULL);
				
			}
		}
		 
        }

    close(listen_fd);
    printf("Receive exit signal,now exit...\n");
    return 0;

}
