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
  OLED_ClearScreen();
  OLED_DisStrLine(0, 0, "All");

  I2CInit(I2CMASTER, 0);

  light_init();
  light_enable();
  light_setRange(LIGHT_RANGE_4000);
}
 

/**** End of File ****/
