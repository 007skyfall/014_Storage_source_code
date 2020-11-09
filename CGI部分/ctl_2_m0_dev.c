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

char errflag = 0;

struct msg
{
	long type;
	long msgtype;
	unsigned char text[N];
};

struct msg msg_buf = {0, 0, {0}};

void ctrl_fan()
{
	char buf[4] = {0};
	cgiFormString("fan", buf, 4);
	if (strncmp(buf, "off", 3) == 0)
	{
		msg_buf.text[0] = ((STORAGE_NO - 48)) << 6 | (0 << 4) | (0 << 0);
	}
	else if(strncmp(buf,"on1", 3) == 0)
	{
		msg_buf.text[0] = ((STORAGE_NO - 48)) << 6 | (0 << 4) | (1 << 0);
	}
	else if(strncmp(buf, "on2", 3) == 0)
	{
		msg_buf.text[0] = ((STORAGE_NO - 48)) << 6 | (0 << 4) | (2 << 0);
	}
	else if(strncmp(buf, "on3", 3) == 0)
	{
		msg_buf.text[0] = ((STORAGE_NO - 48)) << 6 | (0 << 4) | (3 << 0);
	}
	else 
	{
		errflag = 1;
	}
	msg_buf.type = 1L;
	msg_buf.msgtype = 4L;
}

void ctrl_beep()
{
	char buf[8] = {0};
	cgiFormString("beep", buf, 8);
	if(strncmp(buf,"off", 3) == 0)
	{
		msg_buf.text[0] = ((STORAGE_NO - 48)) << 6 | (1 << 4) | (0 << 0);
	}
	else if(strncmp(buf, "on", 2) == 0)
	{
		msg_buf.text[0] = ((STORAGE_NO - 48)) << 6 | (1 << 4) | (1 << 0);
	}
	else if(strncmp(buf, "autooff", 7) == 0)
	{
		msg_buf.text[0] = ((STORAGE_NO - 48)) << 6 | (1 << 4) | (2 << 0);
	}
	else if(strncmp(buf, "autoon", 6) == 0)
	{
		msg_buf.text[0] = ((STORAGE_NO - 48)) << 6 | (1 << 4) | (3 << 0);
	}	
	else
	{
		errflag = 2;
	}
	msg_buf.type = 1L;
	msg_buf.msgtype = 4L;
}

void ctrl_led()
{
	char buf[4] = {0};
	cgiFormString("led", buf, 4);
	if(strncmp(buf,"off", 3) == 0)
	{
		msg_buf.text[0] = ((STORAGE_NO - 48)) << 6 | (2 << 4) | (0 << 0);
	}
	else if (strncmp(buf, "on", 2) == 0)
	{
		msg_buf.text[0] = ((STORAGE_NO - 48)) << 6 | (2 << 4) | (1 << 0);
	} 
	else 
	{
		errflag = 3;
	}
	msg_buf.type = 1L;
	msg_buf.msgtype = 4L;
}

void ctrl_seg()
{
	char buf[4] = {0};
	char segvalue[2] = {0};
	cgiFormString("seg", buf, 4);
	cgiFormString("seg_val", segvalue, 2);
	if (strncmp(buf, "on", 2) == 0)
	{
		msg_buf.text[0] = ((STORAGE_NO - 48)) << 6 | (3 << 4) | ((segvalue[0] - 48) << 0);	
	}
	else if(strncmp(buf,"off", 3) == 0)
	{
		msg_buf.text[0] = ((STORAGE_NO - 48)) << 6 | (3 << 4) | (0x0f << 0);
	}
	else 
	{
		errflag = 4;
	}
	msg_buf.type = 1L;
	msg_buf.msgtype = 4L;
}

int cgiMain()
{
	key_t key; 
	int msgid; 
	char device[6] = {0};
	
	memset(&msg_buf, 0, sizeof(msg_buf));

	cgiFormString("device", device, 6);
	switch(device[0])
	{
		case 'f': // fan
			ctrl_fan();
			break;
		case 'b': // beep
			ctrl_beep();
			break; 
		case 'l': // led
			ctrl_led();
			break;
		case 's': // seg
			ctrl_seg();
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
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"3; url=../control2.html\">");
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
