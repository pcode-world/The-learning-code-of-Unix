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
#include <netinet/in.h> //定义结构体sockaddr_in(区别sockaddr)
#define SERVERPORT 9418
#define bufsize 1000

int main(int argc,char **argv)
{

        pid_t pid,p_pit;
        int socket_server_fd,statloc;



        socket_server_fd=socket(AF_INET,SOCK_STREAM,0); //（协议族，类型，特定类型）
        if(socket_server_fd<0)
        {   
                printf("creat a socket faliuer:%s",strerror(errno));
        }   


        struct sockaddr_in serveraddr,clientaddr;//初始化bind()中第二个参数的结构体
        serveraddr.sin_family=AF_INET;//协议族
        serveraddr.sin_port=htons(SERVERPORT);  //端口
        serveraddr.sin_addr.s_addr=INADDR_ANY;  //IP  INADDR_ANY 可以不用htonl()转换成网络字节序
        bzero(&(serveraddr.sin_zero), 8); //将serveraddr.sin_zero置为0
        if(bind(socket_server_fd,(struct sockaddr *)&serveraddr,sizeof(struct sockaddr))<0)
                printf("bind faliuer:%s",strerror(errno));


        listen(socket_server_fd,12);


        int new_socket_server_fd;
        socklen_t len;

        while(1)
        {
        printf("waiting connect...\n");
        if((new_socket_server_fd=accept(socket_server_fd,(struct sockaddr *)&clientaddr,&len))<0)
                /*int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen*/
                printf("accept failuer:%s",strerror(errno));
        printf("A new client connect succussfully!\n");


        pid=fork();

        if(pid<0)
		printf("creat child process faliuer:%s",strerror(errno));
        else if(pid==0)
        {

                char buf[bufsize];
                int rv=-1;
                memset(buf,0,bufsize);


                if((rv=read(new_socket_server_fd,buf,sizeof(buf)))<0)
			printf("read failure:%s",strerror(errno));

                else if(rv==0)//客户端断开连接
                {
                        printf("client have discnnected!\n");
                        close(new_socket_server_fd);
                        exit(0);
                }
		else if(rv>0)
			{
				printf("read %d bytes from client:%s\n",strlen(buf),buf);
				
			}

                        char temp[strlen(buf)];
                for(int i=0;i<strlen(buf);i++)
                {
                        temp[i]=toupper(buf[i]);
                }
                if(write(new_socket_server_fd,temp,sizeof(temp))<0)
                        printf("write faliuer:%s",strerror(errno));

                printf("close cilent socket[%d],and a child process continue exit\n",new_socket_server_fd);
                close(new_socket_server_fd);
                exit(0);



        }
        else if(pid>0)
        {

                close(new_socket_server_fd);


                p_pit=waitpid(-1,&statloc,WCONTINUED);//pid_t waitpid(pid_t pid,int *statloc, int options);
                continue;
        }


        }
        close(socket_server_fd);
        return 0;
}
