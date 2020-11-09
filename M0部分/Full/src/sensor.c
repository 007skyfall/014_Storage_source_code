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
	uint8_t head[3];	//��ʶλst:
	uint8_t type;			//��������
	uint8_t snum;			//�ֿ���
	uint8_t temp[2];	//�¶�
	uint8_t hum[2];		//ʪ��
	int8_t x;			 		//������Ϣ
	int8_t y;			 
	int8_t z;			 
	uint32_t lux;			//����
	uint32_t bet;			//��ص���
	uint32_t adc; 		//��λ����Ϣ
}EnvMsg;

extern uint8_t To10msFlag;

extern uint32_t Adc; // ȫ�ֱ���
extern uint32_t Bat;
extern uint8_t AdcGetFinishFlag; // ȫ�ֱ�־

extern AccData Axis3; // ȫ�ֱ���
extern uint8_t Axis3GetFinishFlag; // ȫ�ֱ�־

extern uint32_t Light; // ȫ�ֱ���
extern uint8_t LightGetFinishFlag; // ȫ�ֱ�־

extern uint8_t temperature[2];
extern uint8_t humidity[2];
extern uint8_t	DHT11GetFinishFlag;

// ���д�������ʼ��
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

// 10msɨ��һ��
void ScanAllSensor(void)
{
	if(To10msFlag == TRUE)
	{
		ScanKey();
		ScanMyAdc();						
		ScanLight();
		ScanAxis3();
		ScanRfid(); // ɨ��RFID
		ScanBeep();
	}
}

void SyncData(uint8_t sto_no)  		//�����вɼ�������ͬ�����ṹ��EnvMsg��
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

