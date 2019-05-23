#include "comport.c"

int main()

{
	char *com_path = "/dev/ttyUSB0";
	comport comport_info;
	comport_info.parityb='N';
	comport_info.datab = 8;
	comport_info.baudspeed =115200;
	comport_info.stopb = 1;
	comport_info.path = com_path;
	struct termios new_term,old_term;

	openComport(&comport_info,&new_term);
	new_term = old_term;

	setComport(&comport_info,&new_term);
	char buf[32] = "hello";
	char buff[16];
	writeComport(comport_info.com_fd,buf,sizeof(buf));
    sleep(1);
	readComport(comport_info.com_fd,buff,sizeof(buff));
	closeComport(comport_info.com_fd,&old_term);
}
