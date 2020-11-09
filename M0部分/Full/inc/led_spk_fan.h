#ifndef __LED_SPK_FAN_H
#define __LED_SPK_FAN_H

#include <stdint.h>

void ScanFanPwm(void); //定时器100us扫描
void ScanbzzerPwm(void);   //定时器10us扫描
void FanSpeed(uint8_t speed); // 0、1、2、3速度
void Speaker(uint8_t pwm_set); // 1开0关
void Led1Show(uint8_t status); // 1开0关

void InitFan(void);
void InitSpeaker(void);
void InitLed(void);

#endif
