#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include "cgic.h"

#define STORAGE_NO	'2'

#define N 32

struct msg
{
	long type;
	long msgtype;
	char text[N];
};

struct setEnv
{
	int temMAX; //10-50
	int temMIN; //10-50
	int humMAX; //10-50
	int humMIN; //10-50
	int illMAX; //10-500
	int illMIN; //10-500
};

int cgiMain()
{
	key_t key; 
	int msgid; 
	char buf[5] = {0}; //字符接收缓冲区
	char errflag = 0; //错误标志
	struct setEnv new; //环境变量临时结构体
	struct msg msg_buf = {0, 0, {0}}; //消息结构体
	
	memset(&msg_buf,0,sizeof(msg_buf));

	cgiFormString("temMAX", buf, 5);
	new.temMAX = atoi (buf); //字符转换为整型
	cgiFormString("temMIN", buf, 5);
	new.temMIN = atoi (buf);
	cgiFormString("humMAX", buf, 5);
	new.humMAX = atoi (buf);
	cgiFormString("humMIN", buf, 5);
	new.humMIN = atoi (buf);
	cgiFormString("illMAX", buf, 5);
	new.illMAX = atoi (buf);
	cgiFormString("illMIN", buf, 5);
	new.illMIN = atoi (buf);

	if(new.temMAX < new.temMIN || new.temMAX > 100)
		errflag |= 1 << 5;
	if(new.temMAX < new.temMIN || new.temMIN < 0)
		errflag |= 1 << 4;		
	if(new.humMAX < new.humMIN || new.humMAX > 100)
		errflag |= 1 << 3;
	if(new.humMAX < new.humMIN || new.humMIN < 0)
		errflag |= 1 << 2;		
	if(new.illMAX < new.illMIN || new.illMAX > 4000)
		errflag |= 1 << 1;
	if(new.illMAX < new.illMIN || new.illMIN < 0)
		errflag |= 1 << 0;
	if(errflag)
		goto skip;
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
		
	msg_buf.type = 1L;
	msg_buf.msgtype = 5L;
	msg_buf.text[0] = STORAGE_NO;
//	msg_buf.text[0] = '2';
	memcpy (msg_buf.text+1, &new, 24);
	
	msgsnd(msgid, &msg_buf, sizeof(msg_buf)-sizeof(long), 0);
#endif
skip:	
	cgiHeaderContentType("text/html"); 
	fprintf(cgiOut, "<HTML><HEAD>"); 
	fprintf(cgiOut, "<TITLE>操作提示</TITLE></HEAD>"); 
	fprintf(cgiOut, "<BODY BGCOLOR=\"#EEF2FB\">"); 
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"3; url=../setting2.html\">");
	if(errflag)
		fprintf(cgiOut, "<H2>提交<font color=\"#FF0000\" size=\"+3\">失败！</font>请仔细检查参数设置！本页面3秒后自动关闭。</H2>");
	else
		fprintf(cgiOut, "<H2>提交<font color=\"#FF0000\" size=\"+3\">成功！</font>请点击环境信息查看！本页面3秒后自动关闭。</H2>");	
	if(errflag & (1 << 5))
		fprintf(cgiOut, "<H2>温度上限：<font color=\"#FF0000\">%d</font></H2>", new.temMAX);
	else
		fprintf(cgiOut, "<H2>温度上限：%d</H2>", new.temMAX);
	if(errflag & (1 << 4))
		fprintf(cgiOut, "<H2>温度下限：<font color=\"#FF0000\">%d</font></H2>", new.temMIN);
	else
		fprintf(cgiOut, "<H2>温度下限：%d</H2>", new.temMIN);
	if(errflag & (1 << 3))
		fprintf(cgiOut, "<H2>湿度上限：<font color=\"#FF0000\">%d</font></H2>", new.humMAX);
	else
		fprintf(cgiOut, "<H2>湿度上限：%d</H2>", new.humMAX);
	if(errflag & (1 << 2))
		fprintf(cgiOut, "<H2>湿度下限：<font color=\"#FF0000\">%d</font></H2>", new.humMIN);
	else
		fprintf(cgiOut, "<H2>湿度下限：%d</H2>", new.humMIN);
	if(errflag & (1 << 1))
		fprintf(cgiOut, "<H2>光度上限：<font color=\"#FF0000\">%d</font></H2>", new.illMAX);
	else
		fprintf(cgiOut, "<H2>光度上限：%d</H2>", new.illMAX);
	if(errflag & (1 << 0))
		fprintf(cgiOut, "<H2>光度下限：<font color=\"#FF0000\">%d</font></H2>", new.illMIN);
	else
		fprintf(cgiOut, "<H2>光度下限：%d</H2>", new.illMIN);
	fprintf(cgiOut, "<script type=\"text/jscript\">");
	fprintf(cgiOut, "setTimeout(\"self.close()\", 3000)");
	fprintf(cgiOut, "</script>");
	fprintf(cgiOut, "</BODY>"); 
	fprintf(cgiOut, "</HTML>");
	fflush(stdout);	
	
	return 0; 
}
