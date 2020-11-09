#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include "cgic.h"

#define N 32

char errflag = 0;

struct msg
{
	long type;
	long msgtype;
	unsigned char text[N];
};

struct msg msg_buf = {0, 0, {0}};

void ctrl_led(char s[4], int num)
{
	int i = num;
	char buf[4] = {0};
	cgiFormString(s, buf, 4);
	if (strncmp(buf, "on", 2) == 0)
	{
		msg_buf.text[0] = (1 << (i + 4)) | (1 << i);
	}
	else if(strncmp(buf,"off", 3) == 0)
	{
		msg_buf.text[0] = 1 << (i + 4);
	}
	else 
	{
		errflag = 1;
	}
	msg_buf.type = 1L;
	msg_buf.msgtype = 1L;
}

void ctrl_beep(char s[4])
{
	char buf[4] = {0};
	cgiFormString(s, buf, 4);
	if (strncmp(buf, "on", 2) == 0)
	{
		msg_buf.text[0] = 1;
	}
	else if(strncmp(buf,"off", 3) == 0)
	{
		msg_buf.text[0] = 0;
	}
	else 
	{
		errflag = 1;
	}
	msg_buf.type = 1L;
	msg_buf.msgtype = 2L;
}

int cgiMain()
{
	key_t key; 
	int msgid; 
	char device[6] = {0};
	
	memset(&msg_buf, 0, sizeof(msg_buf));

	cgiFormString("device", device, 6);
	switch(device[3])
	{
		case '1':
			ctrl_led("led1", 0);
			break;
		case '2':
			ctrl_led("led2", 1);
			break;
		case '3':
			ctrl_led("led3", 2);
			break;
		case '4':
			ctrl_led("led4", 3);
			break;
		case 'p':
			ctrl_beep("beep");
			break;
		default : 
			goto err;
	}
	if(errflag)
		goto err;
#if 1
	if((key = ftok("/app", 'g')) < 0)
	{
		perror("ftok");
		exit(1);
	}

	if((msgid = msgget(key, 0666)) < 0)
	{
		perror("msgget");
		exit(1);
	}

	msgsnd(msgid, &msg_buf, sizeof(msg_buf)-sizeof(long), 0);
#endif
	
	device[4] = ' ';
	cgiHeaderContentType("text/html"); 
	fprintf(cgiOut, "<HTML><HEAD>"); 
	fprintf(cgiOut, "<TITLE>操作提示</TITLE></HEAD>"); 
	fprintf(cgiOut, "<BODY BGCOLOR=\"#EEF2FB\">"); 
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"3; url=../control1.html\">");
	fprintf(cgiOut, "<H2>设置<font color=\"#FF0000\" size=\"+3\">成功！</font>本页面3秒后自动关闭。</H2>");
	fprintf(cgiOut, "<H2>%s设备掩码%x</H2>", device, msg_buf.text[0]);
	fprintf(cgiOut, "<script type=\"text/jscript\">");
	fprintf(cgiOut, "setTimeout(\"self.close()\", 3000)");
	fprintf(cgiOut, "</script>");
	fprintf(cgiOut, "</BODY>"); 
	fprintf(cgiOut, "</HTML>");
	fflush(stdout);	
	return 0;

err:
	device[4] = ' ';
	cgiHeaderContentType("text/html"); 
	fprintf(cgiOut, "<HTML><HEAD>"); 
	fprintf(cgiOut, "<TITLE>操作提示</TITLE></HEAD>"); 
	fprintf(cgiOut, "<BODY BGCOLOR=\"#EEF2FB\">"); 
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"3; url=../control2.html\">");
	fprintf(cgiOut, "<H2>设置<font color=\"#FF0000\" size=\"+3\">失败！</font>未知错误，请重试！本页面3秒后自动关闭。</H2>");
	fprintf(cgiOut, "<H2><font color=\"#FF0000\">%s</font>设备掩码<font color=\"#FF0000\">%x</font></H2>", device, msg_buf.text[0]);
	fprintf(cgiOut, "<script type=\"text/jscript\">");
	fprintf(cgiOut, "setTimeout(\"self.close()\", 3000)");
	fprintf(cgiOut, "</script>");
	fprintf(cgiOut, "</BODY>"); 
	fprintf(cgiOut, "</HTML>");
	fflush(stdout);	
	return 0;
}
