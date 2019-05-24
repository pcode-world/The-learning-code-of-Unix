#include<stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<errno.h>
#include <strings.h>
#include <string.h>
#include "comport.h"
int openComport(comport *p_comport,struct termios *term)
{
	int serial_fd = -1;
	if((serial_fd = open(p_comport->path,O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
	{
		printf("open a serialport failure:%s\n",strerror(errno));
		_exit(-1);
	}
	if(isatty(serial_fd) == 0)
	{
		printf("open fd is not a terminal device\n");
		_exit(-1);
	}

	p_comport->com_fd = serial_fd;
	if(tcgetattr(serial_fd,term) < 0)
	{
		printf("tcgetattr failure:%s\n",strerror(errno));
		return -3;
	}
	
}

int _set_baudrate(struct termios *term,speed_t baudspeed)
{
	if(cfsetispeed(term,baudspeed) < 0)
	{
		printf("cfsetispeed failure:%s\n",strerror(errno));
		return -1;
	}
	if(cfsetospeed(term,baudspeed) < 0)
	{
		printf("cfsetospeed failure:%s\n",strerror(errno));
		return -2;
	}
}
void set_baudrate(int baudspeed,struct termios *term)
{
	/*0 50 75 110 134 150 200 300 600 1200 1800 2400 4800 9600 19200 38400 57600 115200 230400*/
	switch(baudspeed)
	{
		case 0:
			_set_baudrate(term, B0);break;
		case 50:
			_set_baudrate(term, B50);break;
		case 75:
			_set_baudrate(term, B75);break;
		case 110:
			_set_baudrate(term, B110);break;
		case 134:
			_set_baudrate(term, B134);break;
		case 150:
			_set_baudrate(term, B150);break;
		case 200:
			_set_baudrate(term, B200);break;
		case 300:
			_set_baudrate(term, B300);break;
		case 600:
			_set_baudrate(term, B600);break;
		case 1200:
			_set_baudrate(term, B1200);break;
		case 1800:
			_set_baudrate(term, B1800);break;
		case 2400:
			_set_baudrate(term, B2400);break;
		case 4800:
			_set_baudrate(term, B4800);break;
		case 9600:
			_set_baudrate(term, B9600);break;
		case 19200:
			_set_baudrate(term, B19200);break;
		case 38400:
			_set_baudrate(term, B38400);break;
		case 57600:
			_set_baudrate(term, B57600);break;
		case 115200:
			_set_baudrate(term, B115200);break;
		case 230400:
			_set_baudrate(term, B230400);break;
		default:
			_set_baudrate(term, B115200);break;
	}
}
void set_data_bit(int datab,struct termios *term)
{
	term->c_cflag &= ~CSIZE;
	switch (datab)
	{
		case 8:
			term->c_cflag |= CS8;
			break;
		case 7:
			term->c_cflag |= CS7;
			break;
		case 6:
			term->c_cflag |= CS6;
			break;
		case 5:
			term->c_cflag |= CS5;
			break;
		default:
			term->c_cflag |= CS8;
			break;
	}

}
void set_parity(char parityb,struct termios *term)
{
	switch (parityb) 
	{
		case 'N':                  /* no parity check */
			term->c_cflag &= ~PARENB;
			break;
		case 'E':                  /* 偶检验 */
			term->c_cflag |= PARENB;
			term->c_cflag &= ~PARODD;
			break;
		case 'O':                  /* 奇校验 */
			term->c_cflag |= PARENB;
			term->c_cflag |= PARODD;
			break;
		default:                   /* no parity check */
			term->c_cflag &= ~PARENB;
			break;
	}

}
void set_stopbit(int stopb,struct termios *term)
{
	if (2 == stopb)
	{
		term->c_cflag |= CSTOPB;  /* 2 stop bits */
	} 
	else
	{
		term->c_cflag &= ~CSTOPB; /* 1 stop bit */
	}

}

int setComport(comport *p_comport,struct termios *term)
{
    term->c_oflag &= ~OPOST;
	set_stopbit(p_comport->stopb,term);
	set_parity(p_comport->parityb,term);
	set_data_bit(p_comport->datab,term);
	set_baudrate(p_comport->baudspeed,term);
	tcflush(p_comport->com_fd,TCIOFLUSH);
	if((tcsetattr(p_comport->com_fd,TCSANOW,term)) != 0)
	{
		printf("tcsetattr failure:%s\n",strerror(errno));
		return -1;
	}
    printf("set comport ok!\n");
    printf("The details are printed below\n");
    system("sudo sudo stty -F /dev/ttyUSB0 -a");

}

int readComport(int fd,char *buff,int buffsize)
{
    memset(buff,0,buffsize);
	if(buff == NULL)
	{
		printf("Cannot pass in null pointer\n");
		return -1;
	}
	int rv = -1;
	if((rv = read(fd,buff,buffsize)) < 0)
	{
		printf("read failure:%s\n",strerror(errno));
		return -1;
	}
	return rv;
}

int writeComport(int fd,char *buff,int buffsize)
{
	if(buff == NULL)
	{
		printf("Cannot pass in null pointer\n");
		return -1;
	}
	int rv = -1; 
	if((rv = write(fd,buff,buffsize)) < 0)
	{
		printf("write failure:%s\n",strerror(errno));
	}
	printf("write data to serialport:\n%s",buff);
    usleep(10*1000);
	return rv;
}
int closeComport(int fd,struct termios *old_term)
{
	tcflush(fd,TCIOFLUSH);
	if((tcsetattr(fd,TCSANOW,old_term)) != 0)
	{
		printf("Set to the original property failure:%s\n",strerror(errno));
	}
	close(fd);
}	
