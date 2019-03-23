#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include<netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include<errno.h>

int g_cilentstop = 0;

void printfhelp(void)
{
	printf("-i,assign ip adress\n");
	printf("-p,assign port\n");
	printf("-t,Specified sampling time\n");
	printf("-h,ask for help\n");
}

void sig_usr(int signum)
{
	if(SIGUSR1 == signum)
	{
		g_cilentstop=1;
	}
}

float get_parameter()
{
	char path1[50]= "/sys/bus/w1/devices/";/* get DS18B20 path: /sys/bus/w1/devices/28-xxxx/w1_slave */
	char path2[10]="/w1_slave";
	char buff[256];
	DIR *dirp;
	int fd=-1;
	int rv=-1;
	char *ptr;
	char temparr[20];
	float wendu;
	struct dirent * p_dirrent;/*d_name（文件名）和d_type(文件类型)，%d显示的值4代表目录，8代表文件*/

	if((dirp=opendir(path1))==NULL)	
		printf("opendir failure:%s\n",strerror(errno));//DIR *opendir(const char *path)


	while((p_dirrent=readdir(dirp))!=NULL)
	{
		if(strstr(p_dirrent->d_name,"28-"))
		{
			strncpy(temparr,p_dirrent->d_name,sizeof(temparr));
			break;
		}

	}
	closedir(dirp);
	
	strncat(path1,temparr,strlen(temparr));//src所指向的前n个字符添加到dest结尾处（覆盖原dest结尾处的'\0'添
	strncat(path1,path2,strlen(path2));
	

	if((fd=open(path1,O_RDONLY))<0)
	{
		printf("open failure:%s\n",strerror(errno));
	}
	memset(buff,0,sizeof(buff));
	if((rv=read(fd,buff,sizeof(buff)))<0)
	{
		printf("read failure:%s\n",strerror(errno));
	}
	
	ptr=strstr(buff,"t=");
	ptr+=2;
	wendu=atof(ptr)/1000;
/*	memset(buff,0,sizeof(buff));
	size_t sr; 
	FILE *fp;
	 fp=fopen(path1,"r");
	 sr=fread(buff,10,10,fp);//size_t fread(void *buffer, size_t size, size_t count, FILE *stream);
        ptr=strstr(buff,"t=");
          ptr+=2;
           printf("the ptr-> is %c\n",*ptr);
            wendu=atof(ptr)/1000;
            printf("the temprature is :%f\n",wendu);*/
	 return wendu;
}
int main(int argc,char *argv[])

{
	int SLEEPTIME=30;
	char * SERVER_IP = NULL;
	int SERVERPORT = -1;
	float wendu;
    extern char *optarg;
    extern int optind, opterr, optopt;
    int ret;

        while ((ret = getopt(argc, argv, "i:p:t::h::")) != -1) 
        {   
                switch(ret)
                {   
                        case 'i':
                                SERVER_IP=optarg;//printf("argv is %s\n",optarg);
                                break;
                        case 'p':
                                SERVERPORT=atoi(optarg);//printf("argv is %s\n",optarg);
                                break;
			case 't':
				SLEEPTIME=atoi(optarg);
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
	if((SERVER_IP == NULL) || (SERVERPORT < 0))
	{
		printfhelp();
		return 0;
	}

	signal(SIGUSR1, sig_usr);

	/*
	 *func的值是常量SIG_IGN、常量SIG_DFL或当接到此信号后要调用的函数的地址。如果指定SIG_IGN，
	 *则向内核表示忽略此信号（记住有两个信号SIGKILL和SIGSTOP不能忽略）。如果指定SIG_DFL，则表
	 *示接到此信号后的动作是系统默认动作。当指定函数地址时，则在信号发生时，调用该函数，*/
    while(!g_cilentstop)
    {
        wendu=get_parameter();
        printf("Get the temprature is %f\n",wendu);

        char dest_str[1024];
        char *wday[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
        time_t timep;
        size_t size;
        size=sizeof(dest_str);
        struct tm *p; 
        time(&timep);
        p=localtime(&timep); /*取得当地时间*/
        snprintf(dest_str,size,"RPIwujinlong/%d-%d-%d %d:%d:%d/%f\n",(1900+p->tm_year),(1+p->tm_mon),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec,wendu);
	printf("dest_str:%s\n",dest_str);
        //int snprintf(char* dest_str,size_t size,const char* format,...);
        int client_socketfd;
        socklen_t addrlen;
        struct sockaddr_in serveraddr;
        serveraddr.sin_family=AF_INET;//协议族 
        serveraddr.sin_port=htons(SERVERPORT);  //端口SERVERPORT 
        serveraddr.sin_addr.s_addr=inet_addr(SERVER_IP);
        //IP  INADDR_ANY 可以不用htonl()转换成网络字节序
	    /*1.把ip地址转化为用于网络传输的二进制数值
	     * int inet_aton(const char *cp, struct in_addr *inp);
	     *
	     *
	     * 2.将网络传输的二进制数值转化为成点分十进制的ip地址
	     * char *inet_ntoa(struct in_addr in);  */
        bzero(&(serveraddr.sin_zero), 8);
        client_socketfd = socket(AF_INET,SOCK_STREAM,0);
        if(client_socketfd < 0)
        {
            printf("creat socket failure:%s\n",strerror(errno));
        }

        addrlen=sizeof(serveraddr);
        if((connect(client_socketfd,(struct sockaddr *)&serveraddr,addrlen))<0)
        {
            printf("connect to server failure:%s\n",strerror(errno));
            goto closefd;
        }//   !!!
		/*int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);*/
        printf("%s will be send to server\n",dest_str);
        
        if(write(client_socketfd,dest_str,strlen(dest_str))<0)
        {
            printf("write data to server failure:%s\n",strerror(errno));
            goto closefd;

        }

closefd:
		close(client_socketfd);

        printf("wait 30s Continue to sampling...\n");
        if(sleep(30))
        {
            printf("Receive a signal,sleep over!\n");
        }
    }
    
    printf("Receive exit signal,exit now\n");
	return 0;

}



