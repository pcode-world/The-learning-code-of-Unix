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
#include <pthread.h>
#include <sqlite3.h>
#include <string.h>
#define BACKLOG 13

#include "crc-itu-t.h"
#include "crc-itu-t.c"
#include <stdio.h>
#include <string.h>

#define TLV_FIXED_SIZE 5 // ex Value
#define TLV_SIZE 9
#define BUFSIZE 64
#define PACK_HEADER 0xff
#define MAXDATALEN 8
enum{
        NAME = 1,
        DEVICE,
        TEMPRATURE,
        HUMIDITY,

};


int g_cilentstop=0;
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
	int LISTENPORT = 0;

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
	if(!LISTENPORT)
	{
		printfhelp();
		return -1;
	}

        signal(SIGUSR2, sig_usr);
        int listen_fd;
        struct sockaddr_in serveraddr;
        serveraddr.sin_family=AF_INET;//协议族
        serveraddr.sin_port=htons(LISTENPORT);  //端口绑定
        serveraddr.sin_addr.s_addr=INADDR_ANY;
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
	int new_fd;
	struct sockaddr_in client_addr;
	socklen_t addrlen;
	while(!g_cilentstop)
	{
		printf("Waitting for client connection...\n");
		new_fd=accept(listen_fd,(struct sockaddr *)&client_addr,&addrlen);
		//int accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen);
		if(new_fd<0)
		{
			printf("accept a new client failure;%s",strerror(errno));
			return -4;
		}
		printf("accept a new cilent:fd[%d]\n",new_fd);
				//new_fd,inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
		
		
		
			char buf[64];
			memset(buf,0,sizeof(buf));
			int remain = 0;
			int rv = -1;
			while(1)
			{
				printf("waitting for data coming...\n");
				rv = read(new_fd,&buf[remain],sizeof(buf)-remain);

				if(rv < 0)
				{
					printf("read failure;%s\n",strerror(errno));
					close(new_fd);
					break;
				}
				else if(rv == 0)
				{
					printf("the client has disconnected\n");

					close(new_fd);
					break;
				}

				remain+=rv;
				printf("rv = %d\n",rv);
				printf("remain = %d\n",remain);
				printf("bufsize = %d\n",(int)sizeof(buf));
				remain = unpacktlv(buf,sizeof(buf),rv,remain);
				if(remain < 0)
				{
					return;
				}
				printf("There are the contents of the buffer\n");
				printf("**************************\n");
				int i = 1;
				for(i;i<sizeof(buf)+1;i++)
				{
					printf("0x%02x ",(unsigned char)buf[i-1]);
					if((i%16) == 0)
					{
						printf("\n");
					}
				}
				printf("\n**************************\n");

			}

			printf("the fd = %d has closed\n",new_fd);
			close(new_fd);
	}

	close(listen_fd);

}

int unpacktlv(char *buf,int bufsize,int readret,int remain)
{
	/***longth can not parse***/
	if(remain < (TLV_FIXED_SIZE+1))
	{
		printf("the data is too little that Can't parse data\n");
		//*remain = readret;
		printf("remain = %d\n",remain);
		return remain;
	}

	/***begain to look head***/
	int current = 0;
	for(current;current<remain;current++)
	{
		/***current is head***/
		if((unsigned char)buf[current] == PACK_HEADER)
		{
			printf("current = %d\n",current);

			/***fond head but data is incomplete***/
			if((remain-current)<(TLV_FIXED_SIZE+1))
			{
				printf("1:This data is incomplete\n");
				memmove(buf,&buf[current],remain-current);
				remain = remain-current;
				printf("remain = %d\n",remain);
				return remain;
			}

			int help = current;
			help+=2;

			/***data is incomplete***/
			if((buf[help]>(remain-current)) && ((buf[help]-TLV_FIXED_SIZE)<MAXDATALEN))
			{
				printf("2:This data is incomplete\n");
				memmove(buf,&buf[current],remain-current);
				remain = remain-current;
				printf("remain = %d\n",remain);
				return remain;
			}
			/*
			if(help > (remain-1))
			{
				printf("This data is incomplete\n");
				memmove(buf,&buf[current],remain-current);
				remain = remain-current;
				printf("remain = %d\n",remain);
				return remain;

			}*/

			/***find  data length exceeded***/
			else if((unsigned char)(buf[help]-TLV_FIXED_SIZE) > (bufsize-3))
			{
				printf("the data too long,Data length may be wrong or Buffer is too small\n");
				continue;
			}

			/***The length of the right,begain to calculate CRC***/
			int datalen = 0;
			datalen = (unsigned char)buf[help];//取值时一定要转换成无符号类型
			unsigned short crc16;
			help = help+datalen-TLV_FIXED_SIZE+1;
			printf("help = %d\n",help);
			crc16 = crc_itu_t(MAGIC_CRC,&buf[current],help-current);
			printf("crc16 = %d\n",crc16);

			/***crc right***/			  
			if(crc16 == bytes_to_ushort(&buf[help],2))
			{
				//char data[bufsize];
				printf("this is the right data\n");
				if(buf[current+1] == TEMPRATURE)
				{
					int i = 0;
					for(i;i<datalen;i++)
					{
						printf("0x%02x ",(unsigned char)buf[current]);
						current++;

					}
					printf("\n");

					memmove(buf,&buf[current],remain-current);
					remain = remain-current;
					printf("remain = %d\n",remain);
					current = 0;
					continue;
				}
				else
				{
					printf("the tag not define\n");
					int i = 0;
					for(i;i<datalen;i++)
					{
						printf("0x%02x ",(unsigned char)buf[current]);
					}
					printf("\n");
					memmove(buf,&buf[current],remain-current);
					remain = remain-current;
					printf("remain = %d\n",remain);
					continue;
				}
				


			}
			else
			{
				//find the head and Not the last
				printf("Not the right head!\n");
				/*memmove(buf,&buf[current+1],remain-current);
				remain = remain-current;
				printf("remain = %d\n",remain);*/
				continue;
			}
		}

	}
	printf("Find the end but can not look for other head from buf\n");
	remain = 0;
	return remain;
}




