#include <string.h>
#include "timeflag.h"
#include "type.h"
#include "myadc.h"
#include "acc.h"
#include "light.h"
#include "temp_hum.h"
#include "key.h"
#include "rfid.h"
#include "device.h"
#include "sensor.h"

struct _EnvMsg
{
	uint8_t head[3];	//标识位st:
	uint8_t type;			//数据类型
	uint8_t snum;			//仓库编号
	uint8_t temp[2];	//温度
	uint8_t hum[2];		//湿度
	int8_t x;			 		//三轴信息
	int8_t y;			 
	int8_t z;			 
	uint32_t lux;			//光照
	uint32_t bet;			//电池电量
	uint32_t adc; 		//电位器信息
}EnvMsg;

extern uint8_t To10msFlag;

extern uint32_t Adc; // 全局变量
extern uint32_t Bat;
extern uint8_t AdcGetFinishFlag; // 全局标志

extern AccData Axis3; // 全局变量
extern uint8_t Axis3GetFinishFlag; // 全局标志

extern uint32_t Light; // 全局变量
extern uint8_t LightGetFinishFlag; // 全局标志

extern uint8_t temperature[2];
extern uint8_t humidity[2];
extern uint8_t	DHT11GetFinishFlag;

// 所有传感器初始化
void InitAllSensor(void)
{
	InitMyAdc(); // ADC
	InitKey(); // KEY
	InitLight(); // LIGHT
	InitAxis3(); // AXIS3
	InitDHT11(); // DHT11
	InitRfid(); // RFID
	memset(&EnvMsg, 0, 24);
}

// 10ms扫描一次
void ScanAllSensor(void)
{
	if(To10msFlag == TRUE)
	{
		ScanKey();
		ScanMyAdc();						
		ScanLight();
		ScanAxis3();
		ScanRfid(); // 扫描RFID
		ScanBeep();
	}
}

void SyncData(uint8_t sto_no)  		//把所有采集的数据同步到结构体EnvMsg中
{
	EnvMsg.head[0] = 's';
	EnvMsg.head[1] = 't';
	EnvMsg.head[2] = ':';
	EnvMsg.type = 'e';
	EnvMsg.snum = sto_no;
	if(DHT11GetFinishFlag == TRUE)
	{
		DHT11GetFinishFlag = FALSE;
		EnvMsg.temp[0] = temperature[0];
		EnvMsg.temp[1] = temperature[1];
		EnvMsg.hum[0] = humidity[0];
		EnvMsg.hum[1] = humidity[1];
	}
	if(LightGetFinishFlag == TRUE)
	{
		LightGetFinishFlag = FALSE;
		EnvMsg.lux = Light;
	}
	if(Axis3GetFinishFlag == TRUE)
	{
		Axis3GetFinishFlag = FALSE;
		EnvMsg.x = Axis3.X.Val;
		EnvMsg.y = Axis3.Y.Val;
		EnvMsg.z = Axis3.Z.Val;
	}
	if(AdcGetFinishFlag == TRUE)
	{
		AdcGetFinishFlag = FALSE;
		EnvMsg.bet = Bat;  
		EnvMsg.adc = Adc;
	}
}
/**** End of File ****/

