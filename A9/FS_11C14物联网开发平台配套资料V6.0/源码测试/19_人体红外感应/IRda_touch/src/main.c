/*******************************************************************************
* File:    main.c 
* Author:  Farsight Design Team
* Version: V1.00
* Date:    2011.06.21
* Brief:   Main program body
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
#include "ssp.h"
#include "i2c.h"
#include "adc.h"
#include "speaker_op.h"
#include "rom_drivers.h"

#include "seg7led.h"
#include "oled2864.h" 
#include "tools.h"
#include "spi_uart.h"
   
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* External global variables -------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//__align(4) uint32_t	iap_test=0;
//char buf[32];

/* External functions --------------------------------------------------------*/
extern void LED_Test(void);
extern void Speaker_Test(void);
extern void ADC_Test(void);

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : SysTick interrupt Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void)
{
 
}
 

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
ͨ������Ƿ����ƶ��ĺ����źţ��������LED1��û����ر�LED1.
*******************************************************************************/
int main(void)
{
	
  SystemInit();
  GPIOInit();						// Enables clock for GPIO
  SysTick_Config(48000);
  GPIOSetDir(PORT2, 6, 0);			// Set PIO2_6 to �����ź�����
  GPIOSetDir(PORT3, 0, 1);			//
  while(1)
  {	
 		if(GPIOGetValue(PORT2, 6))//��⵽�ⲿ�ƶ��ĺ����źţ�LED������
		 {
		    GPIOSetValue(PORT3, 0, 0);		// 
			 delay_ms(5);
			 GPIOSetValue(PORT3, 0, 1);		//
			 delay_ms(5);
			 GPIOSetValue(PORT3, 0, 0);		//
			 delay_ms(5);
			 GPIOSetValue(PORT3, 0, 1);		//
			 delay_ms(5);
          }
		  else
		  {
			  GPIOSetValue(PORT3, 0, 1);		// 	   �ر�LED
            }
		  delay_ms(250);
  }

}
 

/**** End of File ****/
