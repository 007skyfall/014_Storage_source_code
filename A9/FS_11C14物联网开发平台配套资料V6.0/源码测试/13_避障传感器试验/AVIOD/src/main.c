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
*******************************************************************************/
int main(void)
{
	
  SystemInit();

  GPIOInit();						// Enables clock for GPIO
  /* Called for system library in core_cm0.h. */
  SysTick_Config(48000);


  SPI_IOConfig(0);
  SPI_Init(0, 8, 2);
  //NVIC_SetPriority(SSP0_IRQn, 1);
  OLED_Init_I();
  UARTInit(115200);
  NVIC_SetPriority(UART_IRQn, 2);
  delay_ms(250);
	
	
  
	 
	OLED_ClearScreen();
  OLED_DisStrLine(1, 0, "avoid");
	
	
	
 
  printf("\r\n");
  //led
	GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
  GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off LED1
	
	GPIOSetDir(PORT2, 6, 1);			// Set PIO2_6 to output
	GPIOSetValue(PORT2, 6, 1);		//  打开障碍物检测使能端
	
	//配置采集为普通IO，输入模式
	LPC_IOCON->R_PIO1_2 &= ~0x07;	
  LPC_IOCON->R_PIO1_2 |= 0x01;		/* CLK OUT */
	GPIOSetDir(PORT1, 2, 0);	
 
  while(1)
  {	
  
		if(GPIOGetValue(PORT1, 2))//当没有障碍物为高电平，有障碍物为低
		{
		delay_ms(250);
		OLED_DisStrLine(3, 0, "Leaving ");
			
		}
   	else	
		OLED_DisStrLine(3, 0, "Hitting ");
		delay_ms(250);
  }

}
 

/**** End of File ****/
