#include <stdio.h>
#include "gprs.h"

int main(int argc,char **argv)
{

	int fd_gp;
	fd_gp =open("/dev/s3c2410_serial1", O_RDWR | O_NOCTTY | O_NDELAY);
	
	if(fd_gp < 0)
	{
		perror("fd_gp");
		return 0;
	}
	printf("open GPRS success\n");
	
	serial_init(fd_gp);
	
	gprs_ctl(fd_gp,GPRS_STRANGER);


return 0;
}
