#include <stdio.h>
#include <time.h>
#include <unistd.h>
int main(int argc,char *argv[])
{
	int printfhelp(void)
	{
		printf("-i,assign ip adress\n");
		printf("-p,assign port\n");
		printf("-h,ask for help\n");
	}
	extern char *optarg;
	extern int optind, opterr, optopt;
	int ret;
	while ((ret = getopt(argc, argv, "i:p:h::")) != -1)
	{
		switch(ret)
		{
			case 'i':
				printf("argv is %s\n",optarg);
				break;
			case 'p':
				printf("argv is %s\n",optarg);
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
	time_t timep;
	struct tm *p;
	time(&timep);
	p=gmtime(&timep);
	printf("%d-%d-%d ",(1900+p->tm_year), (1+p->tm_mon),p->tm_mday);
	printf("%d:%d:%d\n",p->tm_hour, p->tm_min, p->tm_sec);
	return 0;
}
