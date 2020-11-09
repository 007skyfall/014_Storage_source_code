/*******************************************************************************
* File:    main.c 
* Author:  Smart Team
* Version: V1.00
* Date:    2011.06.21
* Brief:   Main program body
********************************************************************************
********************************************************************************
* History:
* 2012.04.22:  V1.00		   initial version
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "lpc11xx.h"
#include "clkconfig.h"
#include "gpio.h"
#include "uart.h"
#include "timer32.h"
#include "ssp.h"
#include "i2c.h"
#include "rom_drivers.h"

#include "oled2864.h" 
#include "tools.h"
#include "spi_uart.h"
#include "crc16.h"
#include "watchdog.h"

/* Private typedef -----------------------------------------------------------*/
#define led_on   0
#define led_off  1


//#define banana 0x00


/*******************函数声明***********************/
//extern volatile uint32_t timer32_0_counter; //蜂鸣器 函数
volatile uint32_t period = 1500;
void speaker_op(uint8_t pwm_set)  //传1响 传0不响
{
	switch(pwm_set)
	{
		case 1:
			LPC_TMR32B1->PR = 0x7f;
			init_timer32PWM(1, period, MATCH0);
			enable_timer32(1);
	   	break;
		case 0:
			init_timer32PWM(1, 0, MATCH0);
			delay_ms(50);
			disable_timer32(1);
		break;
	}
}

/*************end***函数声明***end*****************/

/*******************全局变量***********************/
//int i;



//static uint32_t	Tick=0;  //全局计时  Tick

void SysTick_Handler(void)
{

}


/*******************************************************************************
* Function Name  : main
* Description    : 
*******************************************************************************/
int main(void)
{
	int i=0;
  SystemInit();

  GPIOInit();						// Enables clock for GPIO
 
  UARTInit(115200);
  printf("\r\n\r\nFS_11C14\r\n");

  SysTick_Config(48000);

  GPIOSetDir(PORT1, 9, 0);
  GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
  GPIOSetValue(PORT3, 0, 0);		// PIO3_0 output 1, Turn on LED1


  SPI_IOConfig(0);
  SPI_Init(0, 8, 2);
  OLED_Init_I();
  OLED_DisStrLine(1, 0, "   Watchdog Test!");	
    for(i=0; i<250000; i++);
	GPIOSetValue(PORT3, 0, 0);
	for(i=0; i<250000; i++);
	GPIOSetValue(PORT3, 0, 1);

  WDTInit();
  while(1)
  {	 
	   if(!GPIOGetValue(PORT1, 9))//DOWN  五向按键的向下的按键
	   {
	    WDTFeed();
		 GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off LED1
	   }	
  }
}

/**** End of File ****/
