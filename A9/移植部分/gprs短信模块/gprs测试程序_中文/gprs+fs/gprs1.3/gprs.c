#include "gprs.h"

void serial_init(int fd)
{
	struct termios options;
	tcgetattr(fd,&options);
	options.c_cflag |=(CLOCAL | CREAD);
	options.c_cflag &= ~CSIZE;
	options.c_cflag &= ~CRTSCTS;
	options.c_cflag |= CS8;
	options.c_cflag &= ~CSTOPB;
	options.c_iflag |= IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	
	if((cfsetispeed(&options,B9600)) == -1)
	{
		printf("cfsetospeed is erro\n");
		exit(-1);
	}

	//cfsetospeed(&options,B115200);
	tcsetattr(fd,TCSANOW,&options);

}


void gprs_sendMessage(int news,char num[N])
{
	printf("%s\n",num);
	int fd;
	char at[N] = "AT\r";
	char at_cmgf[N] = "AT+CMGF=0\r";
	printf("%s\n",at_cmgf);
	char at_cmgs[N] = "AT+CMGS=19\r";
	printf("%s\n",at_cmgs);
	char buf_thief[N] ;
	char buf_fire[N] ;
	char buf_center[N] = "0891683108100005F011000D91";
	char buf_num[N];
	memset(buf_num,0,sizeof(buf_num));
	strcpy(buf_num,"86");
	strcat(buf_num,num);
	strcat(buf_num,"F");
	printf("%s\n",buf_num);
	
	int i = 0;
	char tmp;

	for(i = 0; i < 14; i++)
	{
		if((i%2) == 0)
		{
			tmp = buf_num[i];
			buf_num[i] = buf_num[i+1];
			buf_num[i+1] = tmp;
		}
	}
	printf("%s\n",buf_num);

	char buf_n[N] = "000800";
	char buf_msg[N] = "0A67095C0F5077FF01FF01\x1a";

	memset(buf_thief,0,sizeof(buf_thief));
	strcat(buf_thief,buf_center);
	strcat(buf_thief,buf_num);
	strcat(buf_thief,buf_n);
	strcat(buf_thief,buf_msg);
	//char buf_msg1[N] = "164F60662F62117684773CFF01621172314F60554AFF01\x1a";
    //char buf_msg1[N] = "0A6709706B60C5FF01FF01\x1a";
	char buf_msg1[N] = "044F60597D\x1a";
	//char buf_msg1[N] = "024F60\x1a";

	memset(buf_fire,0,sizeof(buf_fire));
	strcat(buf_fire,buf_center);
	strcat(buf_fire,buf_num);
	strcat(buf_fire,buf_n);
	strcat(buf_fire,buf_msg1);
	printf("%s\n",buf_msg);
	if((fd = open("/dev/s3c2410_serial1",O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
	{
		perror("fail to open serial port");
		exit(-1);
	}
	
	serial_init(fd);
	
	if(write(fd,at,strlen(at)) == -1)
	{
		perror("fail to write AT");
		exit(-1);
	}
    int nread;
    char reply[128];
	nread = read(fd,reply,sizeof(reply));
//	printf("nread : %d,reply : %s\n", nread, reply);
	if(strcmp(reply ,"ERROR") == 0 || nread == 9 || nread == -1 || nread == 41){

		printf("failed ,please check your card !\n");
 //     	printf("%s\n",reply);
		exit(-1);
	}
	printf("ok ,send over!");
	
	sleep(1);

	if(write(fd,at_cmgf,strlen(at_cmgf)) == -1)
	{
		perror("fail to write AT+CMGF");
		exit(-1);
	}

	sleep(1);

	if(write(fd,at_cmgs,strlen(at_cmgs)) == -1)
	{
		perror("fail to write AT+CMGS");
		exit(-1);
	}

	sleep(1);
	
	if(news == THIEF)
	{
		if(write(fd,buf_thief,strlen(buf_thief)) == -1)
		{
			perror("fail to write AT+CMGF");
			exit(-1);
		}
	}
	if(news == FIRE)
	{
		printf("%s\n",buf_msg);
		if(write(fd,buf_fire,strlen(buf_fire)) == -1)
		{
			perror("fail to write AT+CMGF");
			exit(-1);
		}
	}

}

