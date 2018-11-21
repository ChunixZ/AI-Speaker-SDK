#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int fd;
void print_usage(char *file)
{
    printf("Usage:\n");
    printf("eg. arg num is 101-106 \n");
    printf("%s 101\n", file);
}

void cp2532_signal_fun(int signum)
{
	unsigned char key_val;
	read(fd, &key_val, 1);
	printf("report key_val = %d\n", key_val);
}

int main(int argc, char **argv)
{
	unsigned char key_val;
	int ret;
	int Oflags;

	/* 启动信号驱动机制,将SIGIO信号同cp2532_signal_fun函数关联起来,一旦产生SIGIO信号,就会执行cp2532_signal_fun */
	signal(SIGIO, cp2532_signal_fun);
	
	fd = open("/dev/apt7l05sf", O_RDWR);
	if (fd < 0)
	{
		printf("can't open /dev/apt7l05sf!\n");
	}

	/* 
	 * STDIN_FILENO是打开的设备文件描述符,F_SETOWN用来决定操作是干什么的,getpid()是个系统调用
	 */
	fcntl(fd, F_SETOWN, getpid());  

	/* 获取打开设备文件描述符 */	
	Oflags = fcntl(fd, F_GETFL); 

	/* 
	 * 设置文件描述符的状态为oflags | FASYNC属性,一旦文件描述符被设置成具有FASYNC属性的状态
	 * 并切换到异步模式，系统会自动调用驱动中的fasync方法
	 */	
	fcntl(fd, F_SETFL, Oflags | FASYNC);  


	while (1)
	{
		sleep(1000);
	}
	
	return 0;
}

