#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include<errno.h>
int main(int argc,char **argv)
{
	char path1[50]= "/wujinlong/bus/w1/devices/";/* get DS18B20 path: /sys/bus/w1/devices/28-xxxx/w1_slave */
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
	printf("close successful\n");
	
	strncat(path1,temparr,sizeof(temparr));//src所指向的前n个字符添加到dest结尾处（覆盖原dest结尾处的'\0'添
	strncat(path1,path2,sizeof(path2));
	
	
	if((fd=open(path1,O_RDONLY))<0)
		printf("open failure:%s",strerror(errno));
	memset(buff,0,sizeof(buff));
	if((rv=read(fd,buff,sizeof(buff)))<0)
		printf("read failure:%s",strerror(errno));
	
	ptr=strstr(buff,"t=");
	ptr+=2;
	wendu=atof(ptr);
	printf("the temprature is %f\n",wendu);
	return 0;
}
