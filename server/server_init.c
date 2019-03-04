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
		printf("creat socket failure:%s",strerror(errno)); return -1;
	}
	if(bind(listen_fd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
		//int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
	{
		printf("bind failure:%s",strerror(errno)); return -2; 
	}
	if(listen(listen_fd,BACKLOG) < 0)//int listen(int sockfd, int backlog);
	{
		printf("listen failure:%s",strerror(errno));
		return -3;
	}
}   
