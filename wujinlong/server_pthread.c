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

struct _ctr_pthread{
    int arg_fd;
    int p_over;
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

void *datadell(void *arg)
{
	struct _ctr_pthread *p_temp=(struct _ctr_pthread *)arg;
    p_temp->arg_fd;
    p_temp->p_over;
	char buff[1024];
	int rv=-1;
	if((rv=read(p_temp->arg_fd,buff,sizeof(buff))) < 0)
	{
		printf("read failure:%s\n",strerror(errno));
		close(p_temp->arg_fd);
		pthread_exit(NULL);
	}
	else if(rv==0)
	{
		printf("the socket has disconnect\n");
		close(p_temp->arg_fd);
		pthread_exit(NULL);
	}
	else if(rv>0)
	{
		printf("read data from client :%s\n",buff);
	}
	char p_device[20];
	char p_time[20];
	char p_tem[20];
	char *ptemp=NULL;
	char *ptemp2=NULL;
	ptemp=strstr(buff,"/");
	ptemp++;
	strncpy(p_device,buff,strlen(buff)-strlen(ptemp)-1);
	ptemp2=strstr(ptemp,"/");
	ptemp2++;
	strncpy(p_time,ptemp,strlen(ptemp)-strlen(ptemp2)-1);
	strncpy(p_tem,ptemp2,strlen(ptemp2));
	printf("get the temprature is %s\n",ptemp2);

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char * sql;
    char buf[1024];
	rc = sqlite3_open("wujinlong.db", &db);
	if( rc )
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	}
	else
	{
		fprintf(stderr, "Opened database successfully\n");
	}

	/* Create SQL statement */
    sql = "CREATE TABLE if not exists RPI_temdata("  
         "ID INT PRIMARY KEY," 
         "DEVICE           CHAR(30)    NOT NULL," 
         "TIME        CHAR(30)," 
         "TEMPRATURE         REAL );";

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "Table created successfully\n");
	}   
    snprintf(buf,sizeof(buf),"INSERT INTO RPI_temdata (DEVICE,TIME,TEMPRATURE) VALUES('%s','%s',%f);",p_device,p_time,atof(p_tem));
    sql = buf;

	/* Execute SQL statement */

    printf("char *sql=%s\n",sql);
	rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		fprintf(stdout, "Records created successfully\n");
	}	
	sqlite3_close(db);
	close(p_temp->arg_fd);
    p_temp->p_over=1;
    printf("pthread exit now ...\n");
	pthread_exit(NULL);//一般用于线程内部退出
}

int pthread_start(int arg_fd)
{
    struct _ctr_pthread ctr_pthread;
    struct _ctr_pthread *p_arg=&ctr_pthread;
    pthread_attr_t attr;
    pthread_t tid;
    p_arg->arg_fd=arg_fd;
    p_arg->p_over=0;

        //pthread_attr_t      thread_attr;
        if( pthread_attr_init(&attr) )
    {   
        printf("pthread_attr_init() failure: %s\n", strerror(errno));
        goto CleanUp; 
    }   
        if( pthread_attr_setstacksize(&attr, 120*1024) )
    {   
        printf("pthread_attr_setstacksize() failure: %s\n", strerror(errno));//线程栈的大小
        goto CleanUp;
    }   
        if( pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) )    
    {   
                printf("pthread_attr_setdetachstate() failure: %s\n", strerror(errno));//线程属性分离
        goto CleanUp;    
    }   
    printf("pthread_set ok!\n");
    if(pthread_create(&tid,&attr,datadell,(void *)p_arg) < 0)
    {   
        printf("create a pthread failure:%s\n",strerror(errno));
        goto CleanUp;
    }   

    printf("there create a new pthread to dell data.\n");
    if(!(p_arg->p_over))
    {
        sleep(2);
    }
    return arg_fd;

CleanUp:
     pthread_attr_destroy(&attr);
     return -1; 
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
    int LISTENPORT;

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
    int listen_fd;
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

	if(listen(listen_fd,BACKLOG) < 0)//int listen(int sockfd, int backlog);
	{
		printf("listen failure:%s\n",strerror(errno));
		return -3;
	}

	while(!g_cilentstop)
	{
		printf("Waitting for client connection...\n");
		new_fd=accept(listen_fd,(struct sockaddr *)&client_addr,&addrlen);
		//int accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen);
		if(new_fd<0)
		{
			printf("accept a new client failure;%s\n",strerror(errno));
            continue;
			//return -4;
		}
		printf("accept a new cilent:fd[%d]\n",new_fd);
				//new_fd,inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

       if (pthread_start(new_fd) < 0)
       {
           printf("a pthread start failure!\n");
       }
       continue;

		/*
		   
		 int pthread_create(
                 pthread_t *restrict_tidp,   //新创建的线程ID指向的内存单元。
                 const pthread_attr_t *restrict_attr,  //线程属性，默认为NULL
                 void *(*start_rtn)(void *), //新创建的线程从start_rtn函数的地址开始运行
                 void *restrict_arg //默认为NULL。若上述函数需要参数，将参数放入结构中并将地址作为arg传入。
                  );
		 */
	}
    close(listen_fd);
    return 0;

}




