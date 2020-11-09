#ifndef __DEVICE_H
#define __DEVICE_H

#include <stdint.h>

#define STO_MASK	0xc0 // �ֿ������
#define DEV_MASK	0x30 // �豸������ 
#define CMD_MASK	0x0f // ����������

#define FAN		0x00 // �ŷ���������
#define BEEP	0x10 // ������������
#define LED		0x20 // ������������
#define SEG		0x30 // �����������

#define FAN_OFF			0x00
#define FAN_SPEED1	0x01
#define FAN_SPEED2	0x02
#define FAN_SPEED3	0x03

#define BEEP_OFF			0x00
#define BEEP_ON				0x01
#define BEEP_AUTO_OFF	0x02
#define BEEP_AUTO_ON	0x03

#define LED_OFF		0x00
#define LED_ON		0x01

#define SEG_MIN		0x00
#define SEG_MAX		0x09
#define SEG_OFF		0x0f

void InitAllDevice(void);
void FanCtrl(uint8_t cmd);
void BeepCtrl(uint8_t cmd);
void LedCtrl(uint8_t cmd);
void SegCtrl(uint8_t cmd);
void ScanBeep(void);

#endif
