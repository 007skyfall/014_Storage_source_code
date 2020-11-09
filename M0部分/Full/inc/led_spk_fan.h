#ifndef __LED_SPK_FAN_H
#define __LED_SPK_FAN_H

#include <stdint.h>

void ScanFanPwm(void); //��ʱ��100usɨ��
void ScanbzzerPwm(void);   //��ʱ��10usɨ��
void FanSpeed(uint8_t speed); // 0��1��2��3�ٶ�
void Speaker(uint8_t pwm_set); // 1��0��
void Led1Show(uint8_t status); // 1��0��

void InitFan(void);
void InitSpeaker(void);
void InitLed(void);

#endif
