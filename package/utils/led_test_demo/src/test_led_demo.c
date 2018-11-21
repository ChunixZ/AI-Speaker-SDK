
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

void print_usage(char *file)
{
     printf("Usage:\n");
     printf("eg. arg num is 100-111 \n");
     printf("%s 100\n", file);
}

int main(int argc, char **argv)
{
	int fs;
	int val;
	unsigned int buf[1];
	
	fs = open("/dev/led1_dev", O_RDWR);
	if (fs < 0)
	{
		printf("can't open /dev/led1_dev\n");
		return -1;
	}
if (argc == 2)
         {
             buf[0] = strtoul(argv[1], NULL, 0);
             ioctl(fs,buf[0]);                               //使用值传入设置参数  
	}else{
		print_usage(argv[0]);
                return 0;
	}
  close(fs);  
	return 0;
}

