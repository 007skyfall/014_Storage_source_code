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
#include "tools.h"

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
  OLED_ClearScreen();
  OLED_DisStrLine(1, 0, "VOLTAGE");
  OLED_DisStrLine(3, 0, "touch");
  printf("\r\n");
  //beer
	LPC_IOCON->R_PIO1_1 &= ~0x07;
  LPC_IOCON->R_PIO1_1 |= 0x01;		//set PIO1_1 gpio function mode
	GPIOSetDir(PORT1, 1, 1);			// 
  GPIOSetValue(PORT1, 1, 1);		// 
	GPIOSetDir(PORT3, 0, 1);			//
	
  ADCInit(ADC_CLK);

  while(1)
  {	
  adv = ADCRead(3);//luyj.201308.4
	adv = adv*3.3/1023*1000;
	snprintf(buf, 13, "ADC : %dmV  ", adv);
	OLED_DisStrLine(7, 0, (uint8_t *)buf);
	printf("%d.%d%d%dV  ", adv/1000, adv%1000/100, adv%1000%100/10, adv%1000%100%10);
		
		if(adv>2900)//模拟值比较
		{
			 OLED_DisStrLine(3, 0, "         ");
			 GPIOSetValue(PORT3, 0, 1);		// PIO1_1 output 1, Turn off 
			 GPIOSetValue(PORT1, 1, 1);		// 
   	}
		else
		{  OLED_DisStrLine(3, 0, "touching");
			 GPIOSetValue(PORT3, 0, 0);		// PIO1_1 output 1, Turn on 
		 	GPIOSetValue(PORT1, 1, 0);		// 
		}
		  delay_ms(100);
  }
}
 

/**** End of File ****/
