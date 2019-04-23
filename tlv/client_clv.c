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

#include "crc-itu-t.h"
#include "crc-itu-t.c"
/* TLV Packet format: 
 *
 *+------------+---------+------------+-----------+-----------+
 *| Header(1B) | Tag(1B) | Length(1B) | Value(4B) | CRC16(2B) |
 *+------------+---------+------------+-----------+-----------+
 */
#define TLV_FIXED_SIZE 5 // ex Value
#define TLV_SIZE 8
#define BUFSIZE 64
#define PACK_HEADER 0xff
/*define tag, enum Automatically add 1*/
enum{
	NAME = 1,
	DEVICE,
	TEMPRATURE,
	HUMIDITY,

};

float get_tep();
void getdevicename(char *buf,int bufsize);
void gettime(char *dest_str,int dest_strsize);
int pack_tep(char *tep,int tepsize);
int packtlv(char *buf,int bufsize,char *data,int datalen,int packtype);
int g_cilentstop=0;

void sig_usr(int signum)
{
	if(SIGUSR1 == signum)
	{   
		g_cilentstop=1;
	
	}   
}

void printfhelp(void)
{
	printf("-i,assign ip adress\n");
	printf("-p,assign port\n");
	printf("-t,Specified sampling time\n");
	printf("-h,ask for help\n");
}

int main(int argc,char *argv[])

{
	int SLEEPTIME=30;
	char * SERVER_IP = NULL;
	int SERVERPORT = -1;
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
		char buf[BUFSIZE];

        	int client_socketfd;
        	socklen_t addrlen;
        	struct sockaddr_in serveraddr;
	        serveraddr.sin_family=AF_INET;//协议族 
	        serveraddr.sin_port=htons(SERVERPORT);  //端口SERVERPORT 
	        serveraddr.sin_addr.s_addr=inet_addr(SERVER_IP);
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
		}
		//   !!!
		/*int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);*/
		char tep[10];
		char dev[20];
		char time[30];
		pack_tep(tep,sizeof(tep));
		
	        packtlv(buf,sizeof(buf),tep,5,TEMPRATURE);
        	if(write(client_socketfd,buf,strlen(buf))<0)
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

int packtlv(char *buf,int bufsize,char *data,int datalen,int packtype)
{
	int offset = 0;
	unsigned short crc16 = 0;
	if((!buf) || (!data) || (bufsize < TLV_SIZE))
	{
		printf("the function packtlv_tem() with error parameters\n");
		return;
	}

	/***head***/
	buf[offset] = PACK_HEADER;
	offset+=1;

	/***tag***/
	buf[offset] = packtype;
	offset+=1;
	if(datalen > (bufsize-TLV_FIXED_SIZE))
	{

		printf("the data is too long,please change the size and enter again\n");
	}

	/***length***/
	buf[offset] = TLV_FIXED_SIZE+datalen;
	offset+=1;

	/***value***/
	int i = 0;
	for(i;i<datalen;i++)
	{
		buf[offset++] = data[i];
	}
	
	/***crc***/
	/*Calculate the CRC value*/
	crc16 = crc_itu_t(MAGIC_CRC, buf, offset);
	/*Adds a two-byte CRC value to packet buf*/
	ushort_to_bytes(&buf[offset], crc16);
	offset += 2;
	return offset;

}
int pack_tep(char *tep,int tepsize)
{
	if(tep ==NULL && tepsize < TLV_FIXED_SIZE+2+2)
		return;
	float f_tep = get_tep();
	unsigned short crc16 = 0;
	int part_int = (int)f_tep;
	int part_decm = (int)(f_tep-part_int)*100;
	int offset = 0;
	printf("f_tep = %f\n",f_tep);
        /***head***/
        tep[offset] = PACK_HEADER;
        offset+=1;

        /***tag***/
        tep[offset] = TEMPRATURE;
        offset+=1;

        /***length***/
        tep[offset] = TLV_FIXED_SIZE+2;
        offset+=1;

        /***value***/
   	tep[offset] = part_int;
	offset++;
	tep[offset] = part_decm;
	offset++;
        /***crc***/
        /*Calculate the CRC value*/
        crc16 = crc_itu_t(MAGIC_CRC, tep, offset);
        /*Adds a two-byte CRC value to packet buf*/
        ushort_to_bytes(&tep[offset], crc16);
        offset += 2;
        return offset;

}


float get_tep()
{

	char path1[50]= "/sys/bus/w1/devices/";
	/* get DS18B20 path: /sys/bus/w1/devices/28-xxxx/w1_slave */
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
		return;
	}
	memset(buff,0,sizeof(buff));
	if((rv=read(fd,buff,sizeof(buff)))<0)
	{
		printf("read failure:%s\n",strerror(errno));
		return;
	}
	
	ptr=strstr(buff,"t=");
	ptr+=2;
	wendu=atof(ptr)/1000;
	return wendu;
}


void gettime(char *dest_str,int dest_strsize)
{
	if((!dest_str) && (dest_strsize<128))
	{
		return;
	}
        //char dest_str[1024];
        char *wday[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
        time_t timep;
        //size_t size;
        struct tm *p; 
        time(&timep);
        p=localtime(&timep); /*取得当地时间*/
        snprintf(dest_str,dest_strsize,"%d-%d-%d %d:%d:%d\n",(1900+p->tm_year),(1+p->tm_mon),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
}

void getdevicename(char *buf,int bufsize)
{
	if((!buf) && (bufsize<20))
	{
		return;
	}
	buf="RPI_WUJINLONG";
}




