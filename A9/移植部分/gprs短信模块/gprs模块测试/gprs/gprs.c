#include <stdio.h>
#include "gprs.h"

/*
	int fd_gprs;
	fd_gprs = open("/dev/myadc",O_RDWR|O_NOCTTY|O_NDELAY);//读写；不允许进程管理串口；
											//	非阻塞	
*/
int check_fd_gprs(int fd_gprs)
{
	if(fd_gprs < 0)
	{
		perror("fd_gprs is error!");
		return fd_gprs;
	}
	return 1;
}


void  serial_init(int fd)
{
	if (check_fd_gprs(fd) < 0){
		return;
	}
	struct termios options;
	tcgetattr(fd, &options);
	options.c_cflag |= ( CLOCAL | CREAD );/*input mode flag:ignore modem 
										  control lines; Enable receiver */
	options.c_cflag &= ~CSIZE;
	options.c_cflag &= ~CRTSCTS;
	options.c_cflag |= CS8;
	options.c_cflag &= ~CSTOPB; //停止位
	options.c_iflag |= IGNPAR;	// 忽略校验错误
	options.c_oflag = 0;// 无输出模式
	options.c_lflag = 0; //本地模式禁用
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	tcsetattr(fd,TCSANOW,&options);
}

void swap(char number[],char swap[])
{
	char *ch1 = "86";
	char tmp[16];
	int i;
	memset(swap,0,32);
	memset(tmp,0,16);
	strcpy(swap,number);
	strcat(swap,"f");
	strcat(ch1,swap);
	strcpy(swap,ch1);
	for(i = 0;i <= strlen(swap) - 1;i += 2){
		tmp[i + 1] = swap[i];
		tmp[i] = swap[i + 1];
	}
	strcpy(swap,tmp);
}
/*
 *cmgf 选择短信发送格式
 *
 *cmgs 发送短信
 */

int send(int fd,char *cmgf,char *cmgs,char *message)//发送函数，用于AT指令的发送
{
	int nread,nwrite;
	char buff[128];
	char reply[128];

	memset(buff,0,sizeof(buff));
	strcpy(buff,"AT\r");
	nwrite = write(fd,buff,strlen(buff));
	printf("1_nwrite = %d, %s\n", nwrite, buff);

	memset(reply,0,sizeof(reply));
	sleep(1);
	nread = read(fd,reply,sizeof(reply));
	printf("2_nread = %d, %s\n", nread, reply);
/*发送方式*/
	memset(buff,0,sizeof(buff));
	strcpy(buff,"AT+CMGF=");
	strcat(buff,cmgf);
	strcat(buff,"\r");
	nwrite = write(fd,buff,strlen(buff));
	printf("3_nwrite = %d, %s\n", nwrite, buff);

	memset(reply,0,sizeof(reply));
	sleep(1);
	nread = read(fd,reply,sizeof(reply));
	printf("4_nread = %d, %s\n", nread, reply);

	memset(buff,0,sizeof(buff));
	strcpy(buff,"AT+CMGS=");
	strcat(buff,cmgs);
	strcat(buff,"\r");
	nwrite = write(fd,buff,strlen(buff));
	printf("5_nwrite = %d, %s\n", nwrite, buff);

	memset(reply,0,sizeof(reply));
	sleep(1);
	nread = read(fd,reply,sizeof(reply));
	printf("6_nread = %d, %s\n", nread, reply);
/**/
	memset(buff,0,sizeof(buff));
	strcpy(buff,message);
	nwrite = write(fd,buff,strlen(buff));
	printf("7_nwrite = %d, %s\n", nwrite, buff);

	memset(reply,0,sizeof(reply));
	sleep(1);
	nread = read(fd,reply,sizeof(reply));
	printf("8_nread = %d, %s\n", nread, reply);

	printf("call send over\n");	
}
int send_en_message(int fd_gprs,char *info,char *number)//发送处理函数，处理字符串
{
	char *cmgf = "1";//文本模式发送
	int conter = 0;
	char cmgs[16] = {'\0'};
	char buf[128];
	strcpy(buf,info);
	strcat(buf,"\x1a");//ctrl+z 的ASC 值26
	strcat(cmgs,number);

	send(fd_gprs,cmgf,cmgs,buf);
#if 1
	printf("send message success\n");
#endif
	return 0;
}
/*
 *gprs_ctl 控制
 *cmd 为命令 
 *
 *1：SEND MESSAGE
 *2：CALL TELEPHONE 
 *10:温度过高
 *11:湿度过高
 *12:光照过强
 *13：陌生人员进入
 *
 * 成功返回 0 ； 错误返回 -1
 * */
int gprs_ctl(int fd_gprs,int  cmd)
{	
	if (check_fd_gprs(fd_gprs) < 0){
		return -1;
	}
	char *number = "18810207835";
	switch (cmd)
	{		
		case GPRS_TEM:
			send_en_message(fd_gprs,"the temprature is too high",number);break;
		case GPRS_HUM:
			send_en_message(fd_gprs,"the humidity is too high",number);break;
		case GPRS_ILLU:
			send_en_message(fd_gprs,"the illumination is too high",number);break;
		case GPRS_STRANGER:
			send_en_message(fd_gprs,"stranger has come in",number);break;
	}

	return 0;
}
