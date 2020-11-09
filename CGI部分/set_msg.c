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

struct msg
{
	long type;
	long msgtype;
	char text[N];
};

struct setMsg{
	char phone_num[12];
	char center_num[12];
};

int nophonenum(const char s[])
{
	int i = 0;
	if(s[11] != '\0')
		return 1;
	if(s[0] != '1')
		return 1;
	for(i = 1; i < 11; i++)
	{
		if(s[i] < '0' || s[i] > '9')
			return 1;
	}
	return 0;
}

int cgiMain()
{
	key_t key; 
	int msgid; 
	struct setMsg phone = {{0}, {0}};
	struct msg msg_buf = {0, 0, {0}};
	
	memset(&msg_buf,0,sizeof(msg_buf));

	cgiFormString("phonenum", phone.phone_num, 12);
	cgiFormString("centernum", phone.center_num, 12);

	if(nophonenum(phone.phone_num))
		goto errphone;
	if(nophonenum(phone.center_num))
		goto errcenter;	
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
	msg_buf.msgtype = 10L;
	strcpy(msg_buf.text, phone.phone_num);
	strcat(msg_buf.text, phone.center_num);

	msgsnd(msgid, &msg_buf, sizeof(msg_buf)-sizeof(long), 0);
#endif
	cgiHeaderContentType("text/html"); 
	fprintf(cgiOut, "<HTML><HEAD>"); 
	fprintf(cgiOut, "<TITLE>操作提示</TITLE></HEAD>"); 
	fprintf(cgiOut, "<BODY BGCOLOR=\"#EEF2FB\">"); 
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"3; url=../setting1.html\">");
	fprintf(cgiOut, "<H2>提交<font color=\"#FF0000\" size=\"+3\">成功！</font>本页面3秒后自动关闭。</H2>");
	fprintf(cgiOut, "<H2>报警电话：%s</H2>", phone.phone_num);
	fprintf(cgiOut, "<H2>中心号码：%s</H2>", phone.center_num);
	fprintf(cgiOut, "<script type=\"text/jscript\">");
	fprintf(cgiOut, "setTimeout(\"self.close()\", 3000)");
	fprintf(cgiOut, "</script>");
	fprintf(cgiOut, "</BODY>"); 
	fprintf(cgiOut, "</HTML>");
	fflush(stdout);	
	return 0;

errphone:
	cgiHeaderContentType("text/html"); 
	fprintf(cgiOut, "<HTML><HEAD>"); 
	fprintf(cgiOut, "<TITLE>操作提示</TITLE></HEAD>"); 
	fprintf(cgiOut, "<BODY BGCOLOR=\"#EEF2FB\">"); 
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"3; url=../setting1.html\">");
	fprintf(cgiOut, "<H2>提交<font color=\"#FF0000\" size=\"+3\">失败！</font>请仔细检查电话号码！本页面3秒后自动关闭。</H2>");
	fprintf(cgiOut, "<H2>报警电话：<font color=\"#FF0000\">%s</font></H2>", phone.phone_num);
	fprintf(cgiOut, "<script type=\"text/jscript\">");
	fprintf(cgiOut, "setTimeout(\"self.close()\", 3000)");
	fprintf(cgiOut, "</script>");
	fprintf(cgiOut, "</BODY>"); 
	fprintf(cgiOut, "</HTML>");
	fflush(stdout);	
	return 0;

errcenter:
	cgiHeaderContentType("text/html"); 
	fprintf(cgiOut, "<HTML><HEAD>"); 
	fprintf(cgiOut, "<TITLE>操作提示</TITLE></HEAD>"); 
	fprintf(cgiOut, "<BODY BGCOLOR=\"#EEF2FB\">"); 
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"3; url=../setting1.html\">");
	fprintf(cgiOut, "<H2>提交<font color=\"#FF0000\" size=\"+3\">失败！</font>请仔细检查中心号码！本页面3秒后自动关闭。</H2>");
	fprintf(cgiOut, "<H2>中心号码：<font color=\"#FF0000\">%s</font></H2>", phone.center_num);
	fprintf(cgiOut, "<script type=\"text/jscript\">");
	fprintf(cgiOut, "setTimeout(\"self.close()\", 3000)");
	fprintf(cgiOut, "</script>");
	fprintf(cgiOut, "</BODY>"); 
	fprintf(cgiOut, "</HTML>");
	fflush(stdout);	
	return 0;
}
