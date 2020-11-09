#include <stdio.h>
#include <string.h>
#include "light.h"
#include "acc.h"
#include "temp_hum.h"
#include "myadc.h"
#include "oled2864.h"
#include "device.h"
#include "sensor.h"
#include "rfid.h"
#include "key.h"
#include "menu.h"
#include "zigbee.h"
#include "timeflag.h"
#include "control.h"
#include "type.h"

extern uint8_t To1SFlag;
extern uint8_t To100msFlag;
extern uint8_t DeviceStatus;		// 设备的状态
extern uint8_t BeepRingCnt; 
extern uint8_t RfidGetFinishFlag; // RFID刷卡标志

extern uint8_t temperature[2];
extern uint8_t humidity[2];
extern uint32_t Light;
extern uint32_t Bat;
extern uint32_t Adc;
extern AccData Axis3;

extern uint8_t KeyStatus;	// 键值
extern uint8_t CmdBuf[1]; // A8命令
extern struct _EnvMsg EnvMsg; // 环境信息

struct _Goods
{
	uint8_t head[3];	 //标识位st:
	uint8_t	type;		 //数据类型
	uint8_t snum;		 //仓库编号
	uint8_t io;			 //进出仓库
	uint8_t goodsno;	 //商品编号
	uint8_t goodsnum;	 //商品数量
}Goods; // 货物信息

uint8_t *Titile = NULL;
uint8_t EnvTableBuf[20] = {"RealTime Info"};
uint8_t StoTableBuf[20] = {"Goods Info"};
uint8_t EnvMenuBuf[12][20] = {"1. Tem:     0.0C   ",
															"2. Hum:     0.0%   ",
															"3. Ill:       0Lux ",
															"4. Bat:    0.00V   ",
															"5. Adc:    0.00V   ",
															"6. A.X:       0C   ",
															"7. A.Y:       0C   ",
															"8. A.Z:       0C   ",
															"9. Fan:     OFF    ",
															"a. Spk:     OFF    ",
															"b. Led:     OFF    ",
															"c. Seg:     OFF    "};
uint8_t StoMenuBuf[2][20]  = {"1. Type:    Apple  ",
														  "2. Num :        0  "};
Menu_TypeDef EnvMenu; // 环境信息界面
Menu_TypeDef StoMenu; // 货物信息界面
uint8_t CleanScreenFlag; // 清屏标志

#define GOODSTYPEMAX		7 // 最多7种货物
#define GOODSTYPEMIN		1 // 最少1种货物
uint8_t GoodsTypeIndex; // 货物种类指针
const uint8_t GoodsTypeBuf[GOODSTYPEMAX][16] = {" Apple", "Banana", "  Pear", "  Kiwi",  " Mango", "Pitaya", "Orange"};

#define GOODSNUMBERMAX	100
#define GOODSNUMBERMIN	0
uint8_t GoodsNumCount; 

uint8_t AddOrDecOptFlag; // 加减操作标志

void DealA8Cmd(uint8_t sto_no)
{
	char cmdbuf[1] = {0};
	if((cmdbuf[0] = ZigbeeRecv(sto_no)) != 1)
	{
		CmdBuf[0] =cmdbuf[0];
	}	
}

void SendEnvMsg(uint8_t sto_no)
{
	char sendbuf[24] = {0};
	SyncData(sto_no);
	if(To1SFlag == TRUE)
	{
		memcpy(sendbuf, &EnvMsg, 24);
		ZigbeeSend(sendbuf, 24);
	}
}

void SendStoMsg(uint8_t sto_no, uint8_t goodstype, uint8_t goodsnum)
{
	uint8_t buf[8] = {0};
	Goods.head[0] = 's';
	Goods.head[1] = 't';
	Goods.head[2] = ':';
	Goods.type = 'r';
	Goods.snum = sto_no;
	if(AddOrDecOptFlag == 0)
		Goods.io = 'I';
	else if(AddOrDecOptFlag == 1)
		Goods.io = 'O';
	Goods.goodsno = goodstype;
	Goods.goodsnum = goodsnum;        
	memcpy(buf, &Goods, 8);
	ZigbeeSend((char*)buf, 8);
}

void RefreshEnvDisp(void)
{	
	uint8_t buf[20] = {0};
	uint8_t len = 0;
	
	if(CleanScreenFlag == 1)
	{
		CleanScreenFlag = 0;
		OLED_ClearScreen();
	}
	if(To100msFlag == TRUE)
	{
		if(temperature[0] < 10)
			EnvMenuBuf[0][11] = ' ';
		else
			EnvMenuBuf[0][11] = temperature[0] / 10 + 0x30;
		EnvMenuBuf[0][12] = temperature[0] % 10 + 0x30;
		EnvMenuBuf[0][14] = temperature[1] % 10 + 0x30;
		
		if(humidity[0] < 10)
			EnvMenuBuf[1][11] = ' ';
		else
			EnvMenuBuf[1][11] = humidity[0] / 10 + 0x30;
		EnvMenuBuf[1][12] = humidity[0] % 10 + 0x30;
		EnvMenuBuf[1][14] = humidity[1] % 10 + 0x30;
		
		EnvMenuBuf[2][11] = ' ';
		EnvMenuBuf[2][12] = ' ';
		EnvMenuBuf[2][13] = ' ';
		EnvMenuBuf[2][14] = ' ';
		if(Light >= 1000)
			EnvMenuBuf[2][11] = Light / 1000 + 0x30;
		if(Light >= 100)
			EnvMenuBuf[2][12] = (Light / 100) % 10 + 0x30;
		if(Light >= 10)
			EnvMenuBuf[2][13] = (Light / 10) % 10 + 0x30;
		EnvMenuBuf[2][14] = Light % 10 + 0x30;

		EnvMenuBuf[3][11] = (uint8_t)(Bat * 3.3 / 1023) + 0x30;
		EnvMenuBuf[3][13] = (uint8_t)(Bat * 33  / 1023) % 10 + 0x30;
		EnvMenuBuf[3][14] = (uint8_t)(Bat * 330 / 1023) % 10 + 0x30;
		
		EnvMenuBuf[4][11] = (uint8_t)(Adc * 3.3 / 1023) + 0x30;
		EnvMenuBuf[4][13] = (uint8_t)(Adc * 33  / 1023) % 10 + 0x30;
		EnvMenuBuf[4][14] = (uint8_t)(Adc * 330 / 1023) % 10 + 0x30;

		if(Axis3.X.Angle >= 0)
		{
			snprintf((char*)buf, 20, "%.1f", Axis3.X.Angle);
			len = strlen((char*)buf);
			if(len == 3)
			{
				EnvMenuBuf[5][10] = ' ';
				EnvMenuBuf[5][11] = ' ';
				EnvMenuBuf[5][12] = buf[0];
				EnvMenuBuf[5][13] = buf[1];
				EnvMenuBuf[5][14] = buf[2];
			}
			else if(len == 4)
			{
				EnvMenuBuf[5][10] = ' ';
				EnvMenuBuf[5][11] = buf[0];
				EnvMenuBuf[5][12] = buf[1];
				EnvMenuBuf[5][13] = buf[2];
				EnvMenuBuf[5][14] = buf[3];
			}
		}
		else
		{
			snprintf((char*)buf, 20, "%.1f", Axis3.X.Angle);
			len = strlen((char*)buf);
			if(len == 4)
			{
				EnvMenuBuf[5][9] = ' ';
				EnvMenuBuf[5][10] = ' ';
				EnvMenuBuf[5][11] = buf[0];
				EnvMenuBuf[5][12] = buf[1];
				EnvMenuBuf[5][13] = buf[2];
				EnvMenuBuf[5][14] = buf[3];
			}
			else if(len == 5)
			{
				EnvMenuBuf[5][9] = ' ';
				EnvMenuBuf[5][10] = buf[0];
				EnvMenuBuf[5][11] = buf[1];
				EnvMenuBuf[5][12] = buf[2];
				EnvMenuBuf[5][13] = buf[3];
				EnvMenuBuf[5][14] = buf[4];
			}
		}
		for(len = 0; len < 20; len++)
			buf[len] = 0;
		len = 0;
		if(Axis3.Y.Angle >= 0)
		{
			snprintf((char*)buf, 20, "%.1f", Axis3.Y.Angle);
			len = strlen((char*)buf);
			if(len == 3)
			{
				EnvMenuBuf[6][10] = ' ';
				EnvMenuBuf[6][11] = ' ';
				EnvMenuBuf[6][12] = buf[0];
				EnvMenuBuf[6][13] = buf[1];
				EnvMenuBuf[6][14] = buf[2];
			}
			else if(len == 4)
			{
				EnvMenuBuf[6][10] = ' ';
				EnvMenuBuf[6][11] = buf[0];
				EnvMenuBuf[6][12] = buf[1];
				EnvMenuBuf[6][13] = buf[2];
				EnvMenuBuf[6][14] = buf[3];
			}
		}
		else
		{
			snprintf((char*)buf, 20, "%.1f", Axis3.Y.Angle);
			len = strlen((char*)buf);
			if(len == 4)
			{
				EnvMenuBuf[6][9] = ' ';
				EnvMenuBuf[6][10] = ' ';
				EnvMenuBuf[6][11] = buf[0];
				EnvMenuBuf[6][12] = buf[1];
				EnvMenuBuf[6][13] = buf[2];
				EnvMenuBuf[6][14] = buf[3];
			}
			else if(len == 5)
			{
				EnvMenuBuf[6][9] = ' ';
				EnvMenuBuf[6][10] = buf[0];
				EnvMenuBuf[6][11] = buf[1];
				EnvMenuBuf[6][12] = buf[2];
				EnvMenuBuf[6][13] = buf[3];
				EnvMenuBuf[6][14] = buf[4];
			}
		}		
		for(len = 0; len < 20; len++)
			buf[len] = 0;
		len = 0;
		if(Axis3.Z.Angle >= 0)
		{
			snprintf((char*)buf, 20, "%.1f", Axis3.Z.Angle);
			len = strlen((char*)buf);
			if(len == 3)
			{
				EnvMenuBuf[7][10] = ' ';
				EnvMenuBuf[7][11] = ' ';
				EnvMenuBuf[7][12] = buf[0];
				EnvMenuBuf[7][13] = buf[1];
				EnvMenuBuf[7][14] = buf[2];
			}
			else if(len == 4)
			{
				EnvMenuBuf[7][10] = ' ';
				EnvMenuBuf[7][11] = buf[0];
				EnvMenuBuf[7][12] = buf[1];
				EnvMenuBuf[7][13] = buf[2];
				EnvMenuBuf[7][14] = buf[3];
			}
		}
		else
		{
			snprintf((char*)buf, 20, "%.1f", Axis3.Z.Angle);
			len = strlen((char*)buf);
			if(len == 4)
			{
				EnvMenuBuf[7][9] = ' ';
				EnvMenuBuf[7][10] = ' ';
				EnvMenuBuf[7][11] = buf[0];
				EnvMenuBuf[7][12] = buf[1];
				EnvMenuBuf[7][13] = buf[2];
				EnvMenuBuf[7][14] = buf[3];
			}
			else if(len == 5)
			{
				EnvMenuBuf[7][9] = ' ';
				EnvMenuBuf[7][10] = buf[0];
				EnvMenuBuf[7][11] = buf[1];
				EnvMenuBuf[7][12] = buf[2];
				EnvMenuBuf[7][13] = buf[3];
				EnvMenuBuf[7][14] = buf[4];
			}
		}
		if(DeviceStatus & 0x01)
		{	
			EnvMenuBuf[8][12] = ' ';
			EnvMenuBuf[8][13] = 'O';
			EnvMenuBuf[8][14] = 'N';
		}
		else if(!(DeviceStatus & 0x01))
		{
			EnvMenuBuf[8][12] = 'O';
			EnvMenuBuf[8][13] = 'F';
			EnvMenuBuf[8][14] = 'F';
		}
		if(DeviceStatus & 0x02)
		{	
			EnvMenuBuf[9][12] = ' ';
			EnvMenuBuf[9][13] = 'O';
			EnvMenuBuf[9][14] = 'N';
		}
		else if(!(DeviceStatus & 0x02))
		{
			EnvMenuBuf[9][12] = 'O';
			EnvMenuBuf[9][13] = 'F';
			EnvMenuBuf[9][14] = 'F';
		}
		if(DeviceStatus & 0x04)
		{	
			EnvMenuBuf[10][12] = ' ';
			EnvMenuBuf[10][13] = 'O';
			EnvMenuBuf[10][14] = 'N';
		}
		else if(!(DeviceStatus & 0x04))
		{
			EnvMenuBuf[10][12] = 'O';
			EnvMenuBuf[10][13] = 'F';
			EnvMenuBuf[10][14] = 'F';
		}
		if(DeviceStatus & 0x08)
		{	
			EnvMenuBuf[11][12] = ' ';
			EnvMenuBuf[11][13] = 'O';
			EnvMenuBuf[11][14] = 'N';
		}
		else if(!(DeviceStatus & 0x08))
		{
			EnvMenuBuf[11][12] = 'O';
			EnvMenuBuf[11][13] = 'F';
			EnvMenuBuf[11][14] = 'F';
		}
		Dis_Menu(EnvMenu);
	}
}

void RefreshStoDisp(void)
{
	if(CleanScreenFlag == 0)
	{
		CleanScreenFlag = 1;
		OLED_ClearScreen();
	}
	if(To100msFlag == TRUE)
	{
		StoMenuBuf[0][11] = GoodsTypeBuf[GoodsTypeIndex - 1][0]; 
		StoMenuBuf[0][12] = GoodsTypeBuf[GoodsTypeIndex - 1][1]; 
		StoMenuBuf[0][13] = GoodsTypeBuf[GoodsTypeIndex - 1][2]; 
		StoMenuBuf[0][14] = GoodsTypeBuf[GoodsTypeIndex - 1][3]; 
		StoMenuBuf[0][15] = GoodsTypeBuf[GoodsTypeIndex - 1][4]; 
		StoMenuBuf[0][16] = GoodsTypeBuf[GoodsTypeIndex - 1][5]; 
		if(GoodsNumCount >= 100)
		{
			if(AddOrDecOptFlag == 1)
				StoMenuBuf[1][13] = '-';
			else
				StoMenuBuf[1][13] = ' ';
			StoMenuBuf[1][14] = GoodsNumCount / 100 + 0x30;
			StoMenuBuf[1][15] = (GoodsNumCount % 10) / 10 + 0x30;
			StoMenuBuf[1][16] = GoodsNumCount % 100 + 0x30;
		}
		else if(GoodsNumCount >= 10)
		{
			StoMenuBuf[1][13] = ' ';
			if(AddOrDecOptFlag == 1)
				StoMenuBuf[1][14] = '-';
			else
				StoMenuBuf[1][14] = ' ';
			StoMenuBuf[1][15] = GoodsNumCount / 10 + 0x30;
			StoMenuBuf[1][16] = GoodsNumCount % 10 + 0x30;
		}
		else
		{
			StoMenuBuf[1][13] = ' ';
			StoMenuBuf[1][14] = ' ';
			if(AddOrDecOptFlag == 1)
				StoMenuBuf[1][15] = '-';
			else
				StoMenuBuf[1][15] = ' ';
			StoMenuBuf[1][16] = GoodsNumCount + 0x30;
		}
		Dis_Menu(StoMenu);
	}
}

void Control(uint8_t sto_no)
{
	static uint8_t rfiddealflag = 0; // 0没有RFID 1有RFID
	static uint8_t keydealopt = 0; // 0确认货物类型操作 1确认货物数量操作 
	
	if(RfidGetFinishFlag == TRUE)   //在Sacnrifd()函数中变化的，即有刷卡
	{	
		if(rfiddealflag == 0)				// RFID 刷卡 
		{
			rfiddealflag = 1;					//当 rfiddealflag = 1时，会跳转到刷卡显示界面
			BeepRingCnt = 10;         //控制蜂鸣器响
		}
		else
		{
			BeepRingCnt = 40;
		}
		RfidGetFinishFlag = FALSE;         
	}
	
	if(rfiddealflag == 1)
	{
		if(keydealopt == 0)				// 0确认货物类型操作 1确认货物数量操作 
		{
			switch(KeyStatus)
			{
				case KEY_LEFT:
					GoodsTypeIndex++;
					if(GoodsTypeIndex > GOODSTYPEMAX)
						GoodsTypeIndex = GOODSTYPEMIN;
					BeepRingCnt = 10;
					KeyStatus = KEY_NONE;
					break;
				case KEY_RIGHT:
					GoodsTypeIndex--;
					if(GoodsTypeIndex < GOODSTYPEMIN)
						GoodsTypeIndex = GOODSTYPEMAX;
					BeepRingCnt = 10;
					KeyStatus = KEY_NONE;
					break;
				case KEY_SEL:
					keydealopt = 1;
					BeepRingCnt = 10;
					StoMenu.numb = 2;
					KeyStatus = KEY_NONE;
					break;
				case KEY_ESC:
					GoodsTypeIndex = GOODSTYPEMIN;
					keydealopt = 0;
					rfiddealflag = 0;
					BeepRingCnt = 10;
					StoMenu.numb = 1;
					KeyStatus = KEY_NONE;
					break;
				default:
					break;
			}
		}
		else if(keydealopt == 1)
		{
			switch(KeyStatus)
			{
				case KEY_UP:
					if(AddOrDecOptFlag == 0)
					{
						GoodsNumCount++;
						if(GoodsNumCount >= GOODSNUMBERMAX)
							GoodsNumCount = GOODSNUMBERMAX;
						BeepRingCnt = 10;
					}
					else if(AddOrDecOptFlag == 1)
					{
						GoodsNumCount--;
						if(GoodsNumCount <= GOODSNUMBERMIN)
						{
							GoodsNumCount = GOODSNUMBERMIN;
							AddOrDecOptFlag = 0;
						}
						BeepRingCnt = 10;
					}
					KeyStatus = KEY_NONE;
					break;
				case KEY_DOWN:
					if(GoodsNumCount == 0) 
						AddOrDecOptFlag = 1;
					if(AddOrDecOptFlag == 0)
					{
						GoodsNumCount--;
						if(GoodsNumCount <= GOODSNUMBERMIN)
						{
							GoodsNumCount = GOODSNUMBERMIN;
							AddOrDecOptFlag = 1;
						}
						BeepRingCnt = 10;
					}
					else if(AddOrDecOptFlag == 1)
					{
						GoodsNumCount++;
						if(GoodsNumCount >= GOODSNUMBERMAX)
							GoodsNumCount = GOODSNUMBERMAX;
						BeepRingCnt = 10;
					}
					KeyStatus = KEY_NONE;
					break;
				case KEY_SEL:
					SendStoMsg(sto_no, GoodsTypeIndex, GoodsNumCount);
					GoodsNumCount = GOODSNUMBERMIN;
					GoodsTypeIndex = GOODSTYPEMIN;
					AddOrDecOptFlag = 0;
					keydealopt = 0;
					rfiddealflag = 0;
					BeepRingCnt = 10;
					StoMenu.numb = 1;
					KeyStatus = KEY_NONE;
					break;
				case KEY_ESC:
					GoodsNumCount = GOODSNUMBERMIN;
					GoodsTypeIndex = GOODSTYPEMIN;
					AddOrDecOptFlag = 0;
					keydealopt = 0;
					rfiddealflag = 0;
					BeepRingCnt = 10;
					StoMenu.numb = 1;
					KeyStatus = KEY_NONE;
					break;
				default:
					break;
			}
		}
		RefreshStoDisp();
	}
	else
	{
		switch(KeyStatus)
		{
			case KEY_DOWN:
				EnvMenu.numb++;
				if(EnvMenu.numb > EnvMenu.max_numb)
					EnvMenu.numb = 1;
				BeepRingCnt = 10;
				KeyStatus = KEY_NONE;
				break;
			case KEY_UP:
				EnvMenu.numb--;
				if(EnvMenu.numb <= 0)
					EnvMenu.numb = EnvMenu.max_numb;
				BeepRingCnt = 10;
				KeyStatus = KEY_NONE;
				break;
			default:
				break;
		}
		RefreshEnvDisp();
	}
}

void InitEnvDisp(void)
{
	EnvMenu.max_numb = 12;
	EnvMenu.numb = 1;
	EnvMenu.title = Titile;
	EnvMenu.table = EnvTableBuf;
	EnvMenu.item[0] = EnvMenuBuf[0];
	EnvMenu.item[1] = EnvMenuBuf[1];
	EnvMenu.item[2] = EnvMenuBuf[2];
	EnvMenu.item[3] = EnvMenuBuf[3];
	EnvMenu.item[4] = EnvMenuBuf[4];
	EnvMenu.item[5] = EnvMenuBuf[5];
	EnvMenu.item[6] = EnvMenuBuf[6];
	EnvMenu.item[7] = EnvMenuBuf[7];
	EnvMenu.item[8] = EnvMenuBuf[8];
	EnvMenu.item[9] = EnvMenuBuf[9];
	EnvMenu.item[10]= EnvMenuBuf[10];
	EnvMenu.item[11]= EnvMenuBuf[11];
}
 
void InitStoDisp(void)  //货物仓库显示
{
	StoMenu.max_numb = 2;
	StoMenu.numb = 1;
	StoMenu.title = Titile;
	StoMenu.table = StoTableBuf;
	StoMenu.item[0] = StoMenuBuf[0];
	StoMenu.item[1] = StoMenuBuf[1];
}
void InitControl(uint8_t *title)
{
	GoodsTypeIndex = GOODSTYPEMIN;  	// 最少1种货物
	GoodsNumCount = GOODSNUMBERMIN;  	//货物最小总量
	AddOrDecOptFlag = 0;							// 加减操作标志
	Titile = title;										//标题
	InitEnvDisp();										//初始化环境
	InitStoDisp();										//初始化货物
	CleanScreenFlag = 0;							//清屏标志
}
