#include "comport.h"

comport *initComport()
{
    comport *comport_info = NULL;

    if((comport_info = (comport *)malloc(sizeof(comport))) == NULL)
    {
        return NULL;
    }
    memset(comport_info,0,sizeof(comport));
    comport_info->paritybit='N';
    comport_info->com_fd = -1;
    comport_info->databit = 8;
    comport_info->baudrate =115200;
    comport_info->stopbit = 1;
    comport_info->isopen = 0;
    return comport_info;
}

void comport_term(comport *pcomport)
{
    if(NULL == pcomport)
    {
        return;
    }

    if(!pcomport->isopen)
    {
        close(pcomport->com_fd);
    }

    free(pcomport);
    pcomport = NULL;
}

int _set_baudrate(struct termios *term,speed_t baudrate)
{
	if(cfsetispeed(term,baudrate) < 0)
	{
		printf("cfsetispeed failure:%s\n",strerror(errno));
		return -1;
	}

	if(cfsetospeed(term,baudrate) < 0)
	{
		printf("cfsetospeed failure:%s\n",strerror(errno));
		return -2;
	}
    return 0;
}

void set_baudrate(int baudrate,struct termios *term)
{
	/*0 50 75 110 134 150 200 300 600 1200 1800 2400 4800 9600 19200 38400 57600 115200 230400*/
	switch(baudrate)
	{
		case 0:
			_set_baudrate(term, B0);
            break;
		case 50:
			_set_baudrate(term, B50);
            break;
		case 75:
			_set_baudrate(term, B75);
            break;
		case 110:
			_set_baudrate(term, B110);
            break;
		case 134:
			_set_baudrate(term, B134);
            break;
		case 150:
			_set_baudrate(term, B150);
            break;
		case 200:
			_set_baudrate(term, B200);
            break;
		case 300:
			_set_baudrate(term, B300);
            break;
		case 600:
			_set_baudrate(term, B600);
            break;
		case 1200:
			_set_baudrate(term, B1200);
            break;
		case 1800:
			_set_baudrate(term, B1800);
            break;
		case 2400:
			_set_baudrate(term, B2400);
            break;
		case 4800:
			_set_baudrate(term, B4800);
            break;
		case 9600:
			_set_baudrate(term, B9600);
            break;
		case 19200:
			_set_baudrate(term, B19200);
            break;
		case 38400:
			_set_baudrate(term, B38400);
            break;
		case 57600:
			_set_baudrate(term, B57600);
            break;
		case 115200:
			_set_baudrate(term, B115200);
            break;
		case 230400:
			_set_baudrate(term, B230400);
            break;
		default:
			_set_baudrate(term, B115200);
            break;
	}
}

void set_data_bit(int databit,struct termios *term)
{
	term->c_cflag &= ~CSIZE;
	switch (databit)
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

void set_parity(char paritybit,struct termios *term)
{
	switch (paritybit) 
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

void set_stopbit(int stopbit,struct termios *term)
{
	if (2 == stopbit)
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
    term->c_cflag |= CLOCAL | CREAD;//用于本地连接和接收使用
    term->c_oflag = 0; //输出模式 
    term->c_lflag = 0; //不激活终端模式
	set_stopbit(p_comport->stopbit,term);
	set_parity(p_comport->paritybit,term);
	set_data_bit(p_comport->databit,term);
	set_baudrate(p_comport->baudrate,term);
	tcflush(p_comport->com_fd,TCIOFLUSH);

	if((tcsetattr(p_comport->com_fd,TCSANOW,term)) != 0)
	{
		printf("tcsetattr failure:%s\n",strerror(errno));
		return -1;
	}

    printf("set comport ok!\n");
    //printf("The details are printed below\n");
    //system("sudo sudo stty -F /dev/ttyUSB0 -a");
    return 0;

}

int openComport(comport *p_comport,struct termios *old_term)
{
    struct termios new_term;
	int serial_fd = -1;

    if((NULL == p_comport) && (NULL == old_term))
    {
        return -1;
    }

    if(p_comport->isopen)
    {
        closeComport(p_comport,old_term);
    }

	if((serial_fd = open(p_comport->path,O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
	{
		printf("open a serialport failure:%s\n",strerror(errno));
        return -1;
	}


	if(isatty(serial_fd) == 0)
	{
		printf("open fd is not a terminal device\n");
        close(serial_fd);
        return -1;
	}

	p_comport->com_fd = serial_fd;
    p_comport->isopen = 1;

    if(tcgetattr(serial_fd,old_term) < 0)
	{
		printf("tcgetattr failure:%s\n",strerror(errno));
		return -3;
	}

    memset(&new_term,0,sizeof(new_term));

    if(tcgetattr(serial_fd,&new_term) < 0)
	{
		printf("tcgetattr failure:%s\n",strerror(errno));
		return -3;
    }

    setComport(p_comport,&new_term);
    return 0;
	
}

int readComport(int fd,char *buff,int buffsize)
{
	int rv = -1;

	if(buff == NULL)
	{
		printf("Cannot pass in null pointer\n");
		return -1;
    }
    
    memset(buff,0,buffsize);
    if((rv = read(fd,buff,buffsize)) < 0)
	{
		printf("read failure:%s\n",strerror(errno));
		return -1;
	}
	return rv;
}

int writeComport(int fd,char *buff,int buffsize)
{
	int rv = -1;

	if(buff == NULL)
	{
		printf("Cannot pass in null pointer\n");
		return -1;
	}

	if((rv = write(fd,buff,buffsize)) < 0)
	{
		printf("write failure:%s\n",strerror(errno));
        return -1;
	}

    usleep(10*1000);
	return rv;
}

int closeComport(comport *pcomport,struct termios *old_term)
{
    if(pcomport == NULL && old_term == NULL)
    {
        return -1;
    }

	tcflush(pcomport->com_fd,TCIOFLUSH);

	if((tcsetattr(pcomport->com_fd,TCSANOW,old_term)) != 0)
	{
		printf("Set to the original property failure:%s\n",strerror(errno));
        return -1;
	}

    pcomport->isopen = 0;
	close(pcomport->com_fd);
    free(pcomport);
    pcomport = NULL;
    return 0;
}	
