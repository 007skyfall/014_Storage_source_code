#include <stdio.h>
#include <string.h>
#include "LPC11xx.h"
#include "collect_data.h"
#include "led_spk_fan.h"
#include "temp_hum.h"
#include "light.h"
#include "i2c.h"
#include "adc.h"
#include "acc.h"
#include "spi_uart.h"
#include "gpio.h"
#include "oled2864.h"
#include "uart.h"
#include "data_type.h"

extern uint8_t STORAGE_NUM;

void collect_data(tem_t *tem,hum_t *hum,light_t *lightp)
{
		uint8_t i;
		char buf[16];		

		i = Read_Temp_Hum(&(tem->ltem),&(tem->htem),&(hum->lhum),&(hum->hhum));
		I2CInit(I2CMASTER, 0);
	 	light_init();
		light_enable();
		light_setRange(LIGHT_RANGE_4000);
		
		lightp->light = light_read();


		if(i != 0)
		{
		
			OLED_ClearScreen();
			snprintf(buf, 16, "Temp: %d.%d ID:%d", tem->htem, tem->ltem,STORAGE_NUM);
			OLED_DisStrLine(1, 0, (uint8_t *)buf);
			snprintf(buf, 16, "Hum: %d.%d", hum->hhum, hum->lhum);
			OLED_DisStrLine(2, 0, (uint8_t *)buf);
			snprintf(buf, 20, "light %d    ", lightp->light);
            OLED_DisStrLine(3, 0, (uint8_t *)buf);
			
		}		
}

void send_data(uint8_t *data, uint8_t len)
{
    uint8_t i=0;
    for(i=0;i<len ;i++)
    {
        ZigBee_PutChar(data[i]); //一个一个发送
    }
}

void send_data_uart(uint8_t *data, uint8_t len)
{
    uint8_t i=0;
    for(i=0;i<len ;i++)
    {
		UART0_PutChar(data[i]);
    }
}


uint32_t adc_ret(adc_t *adc_c)
{
 	float		vol;
	char buf[16];

	ADCInit(ADC_CLK);
	adc_c->ad0 = ADCRead(0);
	vol = adc_c->ad0*3.7/1023;
	adc_c->ad0 = vol*10;
	adc_c->ad3 = ADCRead(3);
	vol = adc_c->ad3*3.7/1023;
	adc_c->ad3 = vol*10;
	snprintf(buf, 21, "Voltage: %d.%dV", adc_c->ad0/10, adc_c->ad0%10);
	OLED_DisStrLine(4, 0, (uint8_t *)buf);

	return 1;			
}

void acc_ret(acc_t *acc_r)
{
  char buf[16];

  int32_t	xoff = 0;
  int32_t	yoff = 0;
  int32_t	zoff = 0;
  acc_r->x = 0;
  acc_r->y = 0;
  acc_r->z = 0;  

  I2CInit(I2CMASTER, 0);

  acc_init();
 
  acc_read(&(acc_r->x), &(acc_r->y), &(acc_r->z));
  xoff = 0-acc_r->x;
  yoff = 0-acc_r->y;
  zoff = 0-acc_r->z;

    acc_read(&(acc_r->x), &(acc_r->y), &(acc_r->z));
    acc_r->x = acc_r->x+xoff;
    acc_r->y = acc_r->y+yoff;
    acc_r->z = acc_r->z+zoff;
  
    snprintf(buf, 20, "Acc x: %d  ", acc_r->x);
    OLED_DisStrLine(5, 0, (uint8_t *)buf);
	//printf("\r\nAcc x: %d,  ", x);

	snprintf(buf, 20, "Acc y: %d  ", acc_r->y);
    OLED_DisStrLine(6, 0, (uint8_t *)buf);
	//printf("Acc y: %d,  ", y);

	snprintf(buf, 20, "Acc z: %d  ", acc_r->z);
    OLED_DisStrLine(7, 0, (uint8_t *)buf);		
}

void get_state(state_t * sta)
{	
	if(GPIOGetValue(3,0))
		sta->led_state = led1_off;
		else
		sta->led_state = led1_on;
	if(GPIOGetValue(3,1))
		sta->led_state |= led2_off;
		else
		sta->led_state |= led2_on; 	

	if(GPIOGetValue(0,2))
		sta->fan_state = fan_off;
		else 
		sta->fan_state = fan_on;
	if(GPIOGetValue(1,1))
		sta->buzz_state = speaker_off;
		else
		sta->buzz_state = speaker_on;
	if(GPIOGetValue(2,11))
		sta->seven_led_state = seven_led_on;
		else
		sta->seven_led_state = seven_led_off;		
}

