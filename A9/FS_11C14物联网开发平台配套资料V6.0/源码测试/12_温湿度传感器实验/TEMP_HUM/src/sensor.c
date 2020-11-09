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
  }

/**** End of File ****/
