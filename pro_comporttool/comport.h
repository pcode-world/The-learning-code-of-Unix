#include <termios.h>
typedef struct _comport{
	char parityb;//奇偶检验位
	int datab;//数据位
	int com_fd;//fd
	int baudspeed;//波特率
	int stopb;//停止位
	char path[32];//串口路径
}comport;


extern int openComport(comport *p_comport,struct termios *term);

extern int setComport(comport *p_comport,struct termios *term);

extern int readComport(int fd,char *buff,int buffsize);

extern int writeComport(int fd,char *buff,int buffsize);

extern int closeComport(int fd,struct termios *old_term);

