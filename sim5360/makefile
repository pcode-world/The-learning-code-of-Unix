cc = /opt/xtools/arm920t/bin/arm-linux-gcc
#cc=gcc
all:
	${cc} at_msg.c comport.c pthread.c epoll_sever.c  -o test_at_msg -lpthread
	sz test_at_msg
clean:
	rm -r test_at_msg
