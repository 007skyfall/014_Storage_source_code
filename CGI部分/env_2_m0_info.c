#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "sem.h"
#include "cgic.h"
#include "data_global.h"

#define ACC_GRAVITY		9.8   /*重力加速度的值*/

struct _XYZ
{
	signed char Val;	/*寄存器的原始值*/
	float Acc;			/*加速度值*/
	float Angle; 		/*角度值*/
};

struct _AccData
{
	struct _XYZ X;
	struct _XYZ Y;
	struct _XYZ Z;
};

struct _AccData Axis3 = {
	{0, 0.0, 0},
	{0, 0.0, 0},
	{0, 0.0, 0},
}; // 全局变量

void DealAxis3(void);

#define STORAGE_NO	2 // 仓库号

#define N 32 // 共享内存大小（其实只需要知道共享内存首地址，所以在这大小无所谓）

// 共享内存结构体
struct shm_addr
{
	char cgi_status;
	char qt_status;
	struct env_info_clien_addr rt_status;
};

struct _rt_info{
	unsigned char sto_status;				// 0:open 1:close
	unsigned char led_status;
	unsigned char beep_status;
	unsigned char fan_status;
	unsigned char seg_status;
	signed char x;
	signed char y;
	signed char z;
	float tem;
	float temMIN;
	float temMAX;
	float hum;
	float humMIN;
	float humMAX;
	float ill;
	float illMIN;
	float illMAX;
	float bat;
	float adc;
};

int cgiMain()
{
	key_t key;
	int semid;
	int shmid;
	struct shm_addr *shm_buf = NULL;

	struct _rt_info rt_buf = 
	{
		1,1,1,1,1,
		1,2,3,
		50.6,10.0,50.0,
		9.3,10.0,50.0,
		300.6,10.0,500.0,
		3.40,2.30,
	};

	if((key = ftok("/app", 'i')) < 0)
	{
		perror("ftok");
		exit(1);
	}
	
	if((semid  = semget(key, 1, 0666)) < 0)
	{
		perror("semget");
		exit(1);
	}

	if((shmid = shmget(key, N, 0666 )) == -1)
	{
		perror("shmget");
		exit(1);
	}
	
	if((shm_buf = (struct shm_addr*)shmat(shmid, NULL, 0)) == (void*)-1)
	{
		perror("shmat");
		exit(1);
	}
	
	sem_p(semid,0);

	rt_buf.sto_status = shm_buf->rt_status.storage_no[STORAGE_NO].storage_status;
	rt_buf.led_status = shm_buf->rt_status.storage_no[STORAGE_NO].led_status;
	rt_buf.beep_status = shm_buf->rt_status.storage_no[STORAGE_NO].buzzer_status;
	rt_buf.fan_status = shm_buf->rt_status.storage_no[STORAGE_NO].fan_status;
	rt_buf.seg_status = shm_buf->rt_status.storage_no[STORAGE_NO].seg_status;
	rt_buf.x = shm_buf->rt_status.storage_no[STORAGE_NO].x;
	rt_buf.y = shm_buf->rt_status.storage_no[STORAGE_NO].y;
	rt_buf.z = shm_buf->rt_status.storage_no[STORAGE_NO].z;
	rt_buf.tem = shm_buf->rt_status.storage_no[STORAGE_NO].temperature;
	rt_buf.temMIN = shm_buf->rt_status.storage_no[STORAGE_NO].temperatureMIN;
	rt_buf.temMAX = shm_buf->rt_status.storage_no[STORAGE_NO].temperatureMAX;
	rt_buf.hum = shm_buf->rt_status.storage_no[STORAGE_NO].humidity;
	rt_buf.humMIN = shm_buf->rt_status.storage_no[STORAGE_NO].humidityMIN;
	rt_buf.humMAX = shm_buf->rt_status.storage_no[STORAGE_NO].humidityMAX;
	rt_buf.ill = shm_buf->rt_status.storage_no[STORAGE_NO].illumination;
	rt_buf.illMIN = shm_buf->rt_status.storage_no[STORAGE_NO].illuminationMIN;
	rt_buf.illMAX = shm_buf->rt_status.storage_no[STORAGE_NO].illuminationMAX;
	rt_buf.bat = shm_buf->rt_status.storage_no[STORAGE_NO].battery;
	rt_buf.adc = shm_buf->rt_status.storage_no[STORAGE_NO].adc;

	sem_v (semid, 0);
	
	cgiHeaderContentType("text/html"); 
	fprintf(cgiOut, "<HTML><HEAD>"); 
	fprintf(cgiOut, "<TITLE>当前实时数据</TITLE>");  
	fprintf(cgiOut, "<style type=\"text/css\"> body,td,th {font-size: 12px;margin-left: 0px;margin-top: 0px;}</style></HEAD>"); 
	fprintf(cgiOut, "<BODY BGCOLOR=\"#F8F9FA\">"); 

	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"1\">");
	if (rt_buf.sto_status)
	{
		fprintf(cgiOut, "<table width=\"100%%\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">");
		// 实时数据表头
		fprintf(cgiOut, 	"<tr bgcolor=\"#DEE4E6\">");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">链接</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">名称</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">最新数值</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">正常范围</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">状态</td>");
		fprintf(cgiOut, 	"</tr>");
		
		// 温度采集	
		fprintf(cgiOut, 	"<tr bgcolor=\"#f2f2f2\">");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\"><img src=\"../images/on.png\" width=\"16\" height=\"16\" /></td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">环境温度</td>");
		if(rt_buf.tem > rt_buf.temMAX)
		{	
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\"><font color=\"#FF0000\">%0.1f℃</font></td>", rt_buf.tem);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">%0.1f-%0.1f℃</td>", rt_buf.temMIN, rt_buf.temMAX);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">过高！</td>");
		}
		else if(rt_buf.tem < rt_buf.temMIN)
		{
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\"><font color=\"#FF0000\">%0.1f℃</font></td>", rt_buf.tem);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">%0.1f-%0.1f℃</td>", rt_buf.temMIN, rt_buf.temMAX);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">过低！</td>");
		}
		else
		{
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">%0.1f℃</td>", rt_buf.tem);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">%0.1f-%0.1f℃</td>", rt_buf.temMIN, rt_buf.temMAX);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">正常！</td>");
		}
		fprintf(cgiOut, 	"</tr>");
		
		// 湿度采集
		fprintf(cgiOut, 	"<tr>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\"><img src=\"../images/on.png\" width=\"16\" height=\"16\" /></td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">环境湿度</td>");
		if(rt_buf.hum > rt_buf.humMAX)
		{
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\"><font color=\"#FF0000\">%0.1f％</font></td>", rt_buf.hum);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">%0.1f-%0.1f％</td>", rt_buf.humMIN, rt_buf.humMAX);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">过高！</td>");
		}
		else if(rt_buf.hum < rt_buf.humMIN)
		{
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\"><font color=\"#FF0000\">%0.1f％</font></td>", rt_buf.hum);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">%0.1f-%0.1f％</td>", rt_buf.humMIN, rt_buf.humMAX);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">过低！</td>");
		}	
		else
		{
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">%0.1f％</td>", rt_buf.hum);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">%0.1f-%0.1f％</td>", rt_buf.humMIN, rt_buf.humMAX);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">正常！</td>");
		}
		fprintf(cgiOut, 	"</tr>");
		
		// 光照采集
		fprintf(cgiOut, 	"<tr bgcolor=\"#f2f2f2\">");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\"><img src=\"../images/on.png\" width=\"16\" height=\"16\" /></td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">环境光照</td>");
		if(rt_buf.ill > rt_buf.illMAX)
		{
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\"><font color=\"#FF0000\">%0.1flux</font></td>", rt_buf.ill);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">%0.1f-%0.1flux</td>", rt_buf.illMIN, rt_buf.illMAX);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">过高！</td>");
		}
		else if(rt_buf.ill < rt_buf.illMIN)
		{
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\"><font color=\"#FF0000\">%0.1flux</font></td>", rt_buf.ill);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">%0.1f-%0.1flux</td>", rt_buf.illMIN, rt_buf.illMAX);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">过低！</td>");
		}	
		else
		{
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">%0.1flux</td>", rt_buf.ill);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">%0.1f-%0.1flux</td>", rt_buf.illMIN, rt_buf.illMAX);
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">正常！</td>");
		}
		fprintf(cgiOut, 	"</tr>");
		
		// 电量采集
		fprintf(cgiOut, 	"<tr>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\"><img src=\"../images/on.png\" width=\"16\" height=\"16\" /></td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">电池电量</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">%0.2fV</td>", rt_buf.bat);
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">70%%-100%%</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">-</td>");
		fprintf(cgiOut, 	"</tr>");
		
		// ADC采集	
		fprintf(cgiOut, 	"<tr bgcolor=\"#f2f2f2\">");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\"><img src=\"../images/on.png\" width=\"16\" height=\"16\" /></td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">模拟电压</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">%0.2fV</td>", rt_buf.adc);
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">-</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">-</td>");
		fprintf(cgiOut, 	"</tr>");
		
		// 三轴信息
		Axis3.X.Val = rt_buf.x; // 全局变量
		Axis3.Y.Val = rt_buf.y; // 全局变量
		Axis3.Z.Val = rt_buf.z; // 全局变量
		DealAxis3();
		fprintf(cgiOut, 	"<tr>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\"><img src=\"../images/on.png\" width=\"16\" height=\"16\" /></td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">设备姿态</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\"><table width=\"200\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\">");
		fprintf(cgiOut, 			"<tr align=\"center\" valign=\"middle\">");
		fprintf(cgiOut, 					"<td width=\"25%%\">&nbsp;</td>");
		fprintf(cgiOut, 					"<td width=\"25%%\">X</td>");
		fprintf(cgiOut, 					"<td width=\"25%%\">Y</td>");
		fprintf(cgiOut, 					"<td width=\"25%%\">Z</td>");
		fprintf(cgiOut, 			"</tr>");
		fprintf(cgiOut, 			"<tr align=\"center\" valign=\"middle\">");
		fprintf(cgiOut, 					"<td>原始值</td>");
		fprintf(cgiOut, 					"<td>%d</td>", rt_buf.x);
		fprintf(cgiOut, 					"<td>%d</td>", rt_buf.y);
		fprintf(cgiOut, 					"<td>%d</td>", rt_buf.z);
		fprintf(cgiOut, 			"</tr>");
		fprintf(cgiOut, 			"<tr align=\"center\" valign=\"middle\">");
		fprintf(cgiOut, 					"<td>加速度</td>");
		fprintf(cgiOut, 					"<td>%0.1f</td>", Axis3.X.Acc);
		fprintf(cgiOut, 					"<td>%0.1f</td>", Axis3.Y.Acc);
		fprintf(cgiOut, 					"<td>%0.1f</td>", Axis3.Z.Acc);
		fprintf(cgiOut, 			"</tr>");
		fprintf(cgiOut, 			"<tr align=\"center\" valign=\"middle\">");
		fprintf(cgiOut, 					"<td>角度值</td>");
		fprintf(cgiOut, 					"<td>%0.1f</td>", Axis3.X.Angle);
		fprintf(cgiOut, 					"<td>%0.1f</td>", Axis3.Y.Angle);
		fprintf(cgiOut, 					"<td>%0.1f</td>", Axis3.Z.Angle);
		fprintf(cgiOut, 			"</tr>");
		fprintf(cgiOut, 		"</table></td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">-</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">-</td>");
		fprintf(cgiOut, 	"</tr>");
/*		// 照明灯
		fprintf(cgiOut, 	"<tr bgcolor=\"#f2f2f2\">");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\"><img src=\"../images/on.png\" width=\"16\" height=\"16\" /></td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">照明灯</td>");
		if(rt_buf.led_status)
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">开</td>");
		else
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">关</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">开、关</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">-</td>");
		fprintf(cgiOut, 	"</tr>");
		// 数码管
		fprintf(cgiOut, 	"<tr>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\"><img src=\"../images/off.png\" width=\"16\" height=\"16\" /></td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">数码管</td>");
		if(rt_buf.seg_status)
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">0-9</td>");
		else		
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">关</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">0-9、关</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">-</td>");
		fprintf(cgiOut, 	"</tr>");
		// 蜂鸣器
		fprintf(cgiOut, 	"<tr bgcolor=\"#f2f2f2\">");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\"><img src=\"../images/on.png\" width=\"16\" height=\"16\" /></td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">蜂鸣器</td>");
		if(rt_buf.beep_status)
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">开</td>");
		else
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">关</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">开、关</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">-</td>");
		fprintf(cgiOut, 	"</tr>");
		// 空调扇
		fprintf(cgiOut, 	"<tr>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\"><img src=\"../images/on.png\" width=\"16\" height=\"16\" /></td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">空调扇</td>");
		if(rt_buf.fan_status)
			fprintf(cgiOut, 	"<td height=\"30\" align=\"center\" valign=\"middle\">3档</td>");
		else
			fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">关</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">1-3档、关</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\">-</td>");
		fprintf(cgiOut, 	"</tr>"); 
		*/
		fprintf(cgiOut, "</table>");
	}
	else
	{
		fprintf(cgiOut, "<H2>网络异常！</H2>");	
	}
	fprintf(cgiOut, "<script type=\"text/jscript\">");
	fprintf(cgiOut, "setTimeout(\"self.close()\", 3000)");
	fprintf(cgiOut, "</script>");
	fprintf(cgiOut, "</BODY>"); 
	fprintf(cgiOut, "</HTML>");
	fflush(stdout);

	return 0;
}

// 三轴计算：原始值、加速度、角度值
void DealAxis3(void)
{
	int8_t tx,ty,tz;
	if((Axis3.X.Val & 0x80) == 0x00)
	{
		tx = Axis3.X.Val;		
		Axis3.X.Acc = (float)(-1)*tx*ACC_GRAVITY/64; 
		if(tx>=64)
			Axis3.X.Angle = 90.0;	
		else
			Axis3.X.Angle = asin((float)tx/64)*57.32;
	}	  
	else
	{
		tx =	Axis3.X.Val; 	 
		tx -= 1;
		tx = ~tx;	
		Axis3.X.Acc = (float)tx*ACC_GRAVITY/64;
		if(tx>=64)
			Axis3.X.Angle = -90.0;
		else		
			Axis3.X.Angle = (-1)*asin((float)tx/64)*57.32;
	}  
	if((Axis3.Y.Val & 0x80) == 0x00) 
	{
		ty = Axis3.Y.Val;		
		Axis3.Y.Acc = (float)(-1)*ty*ACC_GRAVITY/64; 
		if(ty>=64)
			Axis3.Y.Angle = 90.0;			 
		else
			Axis3.Y.Angle = asin((float)ty/64)*57.32; 
	}	  
	else 
	{
		ty =	Axis3.Y.Val; 	 	
		ty -= 1;
		ty = ~ty;	
		Axis3.Y.Acc = (float)ty*ACC_GRAVITY/64;	
		if(ty>=64) 
			Axis3.Y.Angle = -90.0;
		else		
			Axis3.Y.Angle = (-1)*asin((float)ty/64)*57.32;   
	}
	if((Axis3.Z.Val & 0x80) == 0x00)
	{
		tz = Axis3.Z.Val;		
		Axis3.Z.Acc = (float)(-1)*tz*ACC_GRAVITY/64;
		if(tz>=64)
			Axis3.Z.Angle = 90.0;		 
		else
			Axis3.Z.Angle = asin((float)tz/64)*57.32; 
	}	  
	else 
	{
		tz =	Axis3.Z.Val; 	 
		tz -= 1;
		tz = ~tz;	
		Axis3.Z.Acc = (float)tz*ACC_GRAVITY/64;
		if(tz>=64)
			Axis3.Z.Angle = -90.0;
		else		
			Axis3.Z.Angle = (-1)*asin((float)tz/64)*57.32;
	}
}




