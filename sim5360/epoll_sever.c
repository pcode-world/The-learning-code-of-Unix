#include "epoll_sever.h"

int g_cilentstop = 0;

void printfhelp(void)
{
    printf("-p,assign port\n");
    printf("-h,ask for help\n");
}

void sig_usr(int signum)
{
    if(SIGTSTP == signum)
    {
        g_cilentstop=1;
    }

    if(SIGINT == signum)
    {
        g_cilentstop=1;
    }
}

int epoll_server_start(int LISTENPORT)
{
    int ret;
    int listen_fd,epfd;
    struct sockaddr_in serveraddr;
    int new_fd=-1;
    struct sockaddr_in client_addr;
    socklen_t addrlen;
    struct epoll_event ev; 
    struct epoll_event evlist[MAXEVENT];
    int epnu = -1;

    serveraddr.sin_family=AF_INET;//协议族
    serveraddr.sin_port=htons(LISTENPORT);  //端口绑定
    serveraddr.sin_addr.s_addr=INADDR_ANY;
    bzero(&(serveraddr.sin_zero), 8);

    signal(SIGTSTP, sig_usr);

    listen_fd = socket(AF_INET,SOCK_STREAM,0);

    if(listen_fd < 0)
    {
        printf("creat socket failure:%s\n",strerror(errno));
        return -1;
    }

    if(bind(listen_fd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0) 
    {
        printf("bind failure:%s\n",strerror(errno));
        return -2;
    }

    if(listen(listen_fd,BACKLOG) < 0)
    {
        printf("listen failure:%s\n",strerror(errno));
        return -3;
    }

    if((epfd = epoll_create(MAXEVENT)) < 0) 
    {
        printf("create a epfd failure:%s\n",strerror(errno)); return -4;
    }

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

    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,listen_fd,&ev);
    //int epoll_ctl(int epfd, int op, int fd, struct epoll_event *ev)
    //int epoll_wait(int epfd, struct epoll_event *evlist, int maxevents, int timeout);
    //evlist检测到事件，将所有就绪的事件从内核事件表中复制到它的第二个参数指向的数组中。
    while(!g_cilentstop)
    {
        int i = 0;
        printf("Waitting for client connection,ctr+c to quik...\n");

        if((epnu = epoll_wait(epfd,evlist,MAXEVENT,-1)) < 0)
        {
            printf("epoll_wait to failure:%s\n",strerror(errno));
            continue;
        }

        else if(epnu == 0)
        {
            printf("epoll wait timeout\n");
        }

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
                printf("connected\n");
                //close(evlist[i].data.fd);
                epoll_ctl(epfd, EPOLL_CTL_DEL,evlist[i].data.fd, NULL);
                return evlist[i].data.fd;

            }
        }

    }

    close(listen_fd);
    printf("Receive exit signal,now exit...\n");
    return 0;

}
