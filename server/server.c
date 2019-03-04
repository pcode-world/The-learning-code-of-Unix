void printfhelp(void)
{
        printf("-p,assign port\n");
        printf("-h,ask for help\n");
}

void sig_usr(int signum)
{
        if(g_cilentstop == signum)
        {
                g_cilentstop=1;
        }
}
int main(int argc,char *argv[])

{
        extern char *optarg;
        extern int optind, opterr, optopt;
        int ret;
	int LISTENPORT;

        while ((ret = getopt(argc, argv, "p:h::")) != -1)
        {
                switch(ret)
                {
                        case 'p':
                                LISTENPORT=atoi(optarg);//printf("argv is %s\n",optarg);
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

        signal(SIGUSR2, sig_usr);

	void server_init()
	{

        int listen_fd;
        struct sockaddr_in serveraddr;
        serveraddr.sin_family=AF_INET;//协议族
        serveraddr.sin_port=htons(LISTENPORT);  //端口绑定
        serveraddr.sin_addr.s_addr=inet_addr(INADDR_ANY);
        bzero(&(serveraddr.sin_zero), 8);
        listen_fd = socket(AF_INET,SOCK_STREAM,0);

        if(listen_fd < 0)
        {
                printf("creat socket failure:%s",strerror(errno));
		return -1;
        }

	if(bind(listen_fd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
		//int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
	{
		printf("bind failure:%s",strerror(errno));
		return -2;
	}
	if(listen(listen_fd,BACKLOG) < 0)//int listen(int sockfd, int backlog);
	{
		printf("listen failure:%s",strerror(errno));
		return -3;
	}
	}
	int new_fd;
	struct sockaddr_in client_addr;
	socklen_t addrlen;
	new_fd=accept(listen_fd,(struct sockaddr *)&client_addr,&addrlen);
	//int accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen);
	if(new_fd<0)
	{
		printf("accept a new client failure;%s",strerror(errno));
		return -4;
	}
	printf("accept a new cilent:fd[%d]\n",new_fd);
	






