#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include "cgic.h"

//#define DEBUG

#define N 32

struct msg
{
	long type;
	long msgtype;
	char text[N];
};

struct setWifi
{
	char cmd[16];
	char ipaddr[16];
	char gateway[16];
};

int cgiMain()
{
#ifndef DEBUG
	
	key_t key; 
	int msgid; 
	struct setWifi wifi = {{0}, {0}};
	struct msg msg_buf;
	
	memset(&msg_buf,0,sizeof(msg_buf));
	
	cgiFormString("submit_set_wifi", wifi.cmd, 16);
	cgiFormString("ipaddr",wifi.ipaddr,16);
	cgiFormString("gateway",wifi.gateway,16);

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
	
	msg_buf.type = 1L;
	msg_buf.msgtype = 11L;
	strcpy(msg_buf.text,wifi.ipaddr);
	if(wifi.cmd[1] == 188)
	{
		strcat(msg_buf.text,"n");
	}
	else if(wifi.cmd[1] == 133)
	{
		strcat(msg_buf.text,"f");
	}
	else 
	{
		strcat(msg_buf.text,"f");
	}
	strcat(msg_buf.text,wifi.gateway);
	msgsnd(msgid, &msg_buf,sizeof(msg_buf)-sizeof(long),0);
#else
	struct setWifi wifi = {{0}, {0}};
#endif
	cgiHeaderContentType("text/html"); 
	fprintf(cgiOut, "<HTML><HEAD>"); 
	fprintf(cgiOut, "<TITLE>操作提示</TITLE></HEAD>"); 
	fprintf(cgiOut, "<BODY BGCOLOR=\"#EEF2FB\">"); 
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"3; url=../setting1.html\">");
	fprintf(cgiOut, "<H2>提交<font color=\"#FF0000\" size=\"+3\">成功！</font>本页面3秒后自动关闭。</H2>");
	fprintf(cgiOut, "<H2>cmd：%s</H2>", wifi.cmd);
	fprintf(cgiOut, "<H2>IP设置：%s</H2>", wifi.ipaddr);
	fprintf(cgiOut, "<H2>网关设置：%s</H2>", wifi.gateway);
	fprintf(cgiOut, "<script type=\"text/jscript\">");
	fprintf(cgiOut, "setTimeout(\"self.close()\", 3000)");
	fprintf(cgiOut, "</script>");
	fprintf(cgiOut, "</BODY>"); 
	fprintf(cgiOut, "</HTML>");	
	fflush(stdout);
	return 0;
}
