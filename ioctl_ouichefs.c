#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include<sys/ioctl.h>
#include <string.h>
#include "ioctl_ouichefs.h"
char buff[100] = "coca";


int main(int argc, char **argv)
{
	int fd = open("/dev/ouichefs", O_WRONLY);
	
	ioctl(fd, QUICK_CLEAN);
	return 0;
}
