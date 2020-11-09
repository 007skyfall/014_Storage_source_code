#include "led_spk_fan.h"
#include "seg7led.h"
#include "zigbee.h"
#include "oled2864.h"
#include "device.h"
#include "tools.h"
#include "type.h"

extern uint8_t To10msFlag;

uint8_t DeviceStatus;
uint8_t BeepRingCnt;

void InitAllDevice(void)
{
	InitFan(); // FAN
	InitSpeaker(); // SPEAKER
	InitLed(); // LED
	Seg7Led_Init();	// SEG
	InitZigbee(); // 初始化ZIGBEE
	InitOled();
	
	FanSpeed(0);
	Speaker(0); // 1开0关
	Led1Show(0); // 1开0关
	Seg7Led_Put(' ');
	BeepRingCnt = 0;
	DeviceStatus = 0;
}

void FanCtrl(uint8_t cmd)
{
	switch(cmd)
	{
		case FAN_OFF:
			DeviceStatus &= ~0x01;
			FanSpeed(0);
			break;
		case FAN_SPEED1:
			DeviceStatus |= 0x01;
			FanSpeed(1);
			break;
		case FAN_SPEED2:
			DeviceStatus |= 0x01;
			FanSpeed(2);
			break;
		case FAN_SPEED3:
			DeviceStatus |= 0x01;
			FanSpeed(3);
			break;
		default:
			break;
	}
}

void BeepCtrl(uint8_t cmd)
{
	switch(cmd)
	{
		case BEEP_OFF:
			DeviceStatus &= ~0x02;
			Speaker(0);	
			break;
		case BEEP_ON:
			DeviceStatus |= 0x02;
			Speaker(1);
			break;
		case BEEP_AUTO_OFF:
			break;
		case BEEP_AUTO_ON:
			break;
		default:
			break;
	}
}

void LedCtrl(uint8_t cmd)
{
	switch(cmd)
	{
		case LED_OFF:
			DeviceStatus &= ~0x04;
			Led1Show(0); // 1开0关
			break;
		case LED_ON:
			DeviceStatus |= 0x04;
			Led1Show(1); // 1开0关
			break;
		default:
			break;
	}
}

void SegCtrl(uint8_t cmd)
{
	if(cmd <= SEG_MAX)
	{
		DeviceStatus |= 0x08;
		Seg7Led_Put(cmd);
	}
	else if(cmd == SEG_OFF)
	{
		DeviceStatus &= ~0x08;
		Seg7Led_Put(' ');
	}
	else 
	{
		DeviceStatus &= ~0x08;
		Seg7Led_Put(' ');
	}
}

void ScanBeep(void)
{
	if(BeepRingCnt > 0)
		BeepRingCnt--;
	if(DeviceStatus & 0x02)
	{
		Speaker(1);	// 开
	}
	else if((BeepRingCnt == 0) && !(DeviceStatus & 0x02))
	{	
		Speaker(0); // 关
	}
	else if(BeepRingCnt > 0)  
	{
		Speaker(1);	// 开
	}	
}	


