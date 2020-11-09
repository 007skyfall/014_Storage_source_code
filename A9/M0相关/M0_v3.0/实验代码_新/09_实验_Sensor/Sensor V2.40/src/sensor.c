/*******************************************************************************
* File:    led_spk_fan_adc.c 
* Author:  Farsight Design Team
* Version: V1.00
* Date:    2011.06.21
* Brief:   LED & Speaker & FAN & ADC program body
********************************************************************************
********************************************************************************
* History:
* 2011.05.10:  V1.00		   initial version
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "lpc11xx.h"
#include "clkconfig.h"
#include "gpio.h"
#include "uart.h"
#include "timer32.h"
#include "adc.h"
#include "i2c.h"

#include "oled2864.h" 
#include "key.h"
#include "tools.h"

#include "temp_hum.h"
#include "light.h"
#include "acc.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* External global variables -------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External functions --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : ADC_Test
* Description    : ADC test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_Test(void)
{
  char 		buf[25];
  uint32_t	adv;
  float		vol;

  OLED_ClearScreen();
  OLED_DisStrLine(0, 0, "Voltage");
  printf("\r\n");

  ADCInit(ADC_CLK);

  while(KEY_Read() != KEY_ESC)
  {
    adv = ADCRead(0);
	vol = adv*3.3/1023;
	adv = vol*10;
	snprintf(buf, 21, "%d.%dV   ", adv/10, adv%10);
	OLED_DisStrLine(2, 0, (uint8_t *)buf);
	printf("%d.%dV  ", adv/10, adv%10);

//	adv = ADCRead(3);
//	snprintf(buf, 16, "ADC3: %d   ", adv);
//	OLED_DisStrLine(3, 0, (uint8_t *)buf);
//	printf("\r\nADC3: %d", adv);

	delay_ms(250);
  }
}

/*******************************************************************************
* Function Name  : All_Sensor_Test
* Description    : Temp_Hum & Light & Axis-3 & Voltage test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void All_Sensor_Test(void)
{
  char 		buf[25];

  uint8_t	temp[2], hum[2];
  uint32_t	i;

  uint32_t	lux;

  int32_t	xoff = 0;
  int32_t	yoff = 0;
  int32_t	zoff = 0;
  int8_t	x = 0;
  int8_t	y = 0;
  int8_t	z = 0;

  uint32_t	adv;
  float		vol;

  
  OLED_ClearScreen();
  OLED_DisStrLine(0, 0, "All");

  I2CInit(I2CMASTER, 0);

  light_init();
  light_enable();
  light_setRange(LIGHT_RANGE_4000);

  acc_init();
  /* Assume base board in zero-g position when reading first value. */
  acc_read(&x, &y, &z);
  xoff = 0-x;
  yoff = 0-y;
  zoff = 0-z;

  ADCInit(ADC_CLK);

  while(KEY_Read() != KEY_ESC)
  {
	i = Read_Temp_Hum(temp, hum);

	if(i)
	{
	  if(temp[0]&0x80)
	  {
	    temp[0] &= 0x7f;
        snprintf(buf, 16, "Temp: -%d.%d  ", temp[0], temp[1]);
	  }
	  else
	  {
	    snprintf(buf, 16, "Temp: %d.%d  ", temp[0], temp[1]);
	  }
	  OLED_DisStrLine(2, 0, (uint8_t *)buf);
	  snprintf(buf, 16, "Hum:  %d.%d  ", hum[0], hum[1]);
	  OLED_DisStrLine(3, 0, (uint8_t *)buf);

	  printf("\r\n\r\nTemp: %d.%d", temp[0], temp[1]);
	  printf("\r\nHum:  %d.%d", hum[0], hum[1]);
    }
    else
    {
      printf("\r\nfail");
    }

	lux = light_read();
    snprintf(buf, 20, "Light: %d   ", lux);
    OLED_DisStrLine(4, 0, (uint8_t *)buf);
	printf("\r\nLight: %d", lux);

    acc_read(&x, &y, &z);
    x = x+xoff;
    y = y+yoff;
    z = z+zoff;

    snprintf(buf, 20, "x:%2d  y:%2d  z:%2d   ", x, y, z);
    OLED_DisStrLine(5, 0, (uint8_t *)buf);
	printf("\r\nx:%2d  y:%2d  z:%2d", x, y, z);

    adv = ADCRead(0);
	vol = adv*3.3/1023;
	adv = vol*10;
	snprintf(buf, 21, "Voltage: %d.%dV   ", adv/10, adv%10);
	OLED_DisStrLine(6, 0, (uint8_t *)buf);
	printf("\r\nVoltage: %d.%dV  ", adv/10, adv%10);


	delay_ms(200);
    if(KEY_Read() == KEY_ESC)
	  break;

	delay_ms(200);
    if(KEY_Read() == KEY_ESC)
	  break;

	delay_ms(200);
    if(KEY_Read() == KEY_ESC)
	  break;

	delay_ms(200);
    if(KEY_Read() == KEY_ESC)
	  break;

	delay_ms(200);
    if(KEY_Read() == KEY_ESC)
	  break;
  }
}
 

/**** End of File ****/
