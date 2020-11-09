#include <termios.h>//串口控制
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define GPRS_TEM 10     //温度
#define GPRS_HUM 11	//湿度
#define GPRS_ILLU 12  	//光照
#define GPRS_STRANGER 13

struct message_info
{
	char cnnu[16];
	char phnu[16];
	char message[128];
};
struct pdu_info 
{
	char cnswap[32];
	char phswap[32];
};

/*
 *判断fd_gprs文件描述副是否正确
 * */
int check_fd_gprs(int fd_gprs);
	

/*串口初始化函数*/
void serial_init(int fd_gprs);
/* 
 * 交换函数
 *
 *
 * */
void swap(char number[],char swap[]);

/*
 *发送函数
 *
 *
 */
int send(int fd,char *cmgf,char *cmgs,char *message);
/*
 *拨打电话
 * */
int call_phone(int fd,struct message_info info);
/*
 *发送短信
 * */
int send_en_message(int fd,char *info,char *number);



