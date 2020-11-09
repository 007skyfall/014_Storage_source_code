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
#include "rom_drivers.h"

#include "oled2864.h" 
#include "tools.h"
#include "spi_uart.h"

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
  uint8_t	rd;
  int8_t	x = 2;
  int8_t	y = 0;
  int8_t 	i = 0;
  int8_t	j = 6;

  uint8_t	buf[12] = "ZigBee Test:";
  SystemInit();

  GPIOInit();						// Enables clock for GPIO

  LPC_IOCON->PIO0_1 &= ~0x07;	
  LPC_IOCON->PIO0_1 |= 0x01;		/* CLK OUT */

  /* Called for system library in core_cm0.h. */
  SysTick_Config(48000);

  SPI_IOConfig(0);
  SPI_Init(0, 8, 2);
  //NVIC_SetPriority(SSP0_IRQn, 1);
  OLED_Init_I();

  UARTInit(115200);
  NVIC_SetPriority(UART_IRQn, 2);

  printf("\r\n\r\nZigBee");

  delay_ms(250);

  SPI_IOConfig(1);
  SPI_Init(1, 8, 2);

  SPI752_Init(1, 115200);

  
  while(1)
  {
  	for(i=0; i<12; i++)
	{
  		OLED_DisChar(1, i+j, 0, buf[i]);
		j += 6;
 	}
	j = 6;

	if(UART0_GetChar(&rd))
    {
	  ZigBee_PutChar(rd);
    }


	if(ZigBee_GetChar(&rd))
    {
	  UART0_PutChar(rd);

	  OLED_DisChar(x, y, 0, rd);
	  y += 6;
	  if(y > 120)
	  {
	  	x++;
    	if(x > 7)
		{
			OLED_ClearScreen();
			x = 2;
		}
	    y = 0;
	  }	  
    }
		
  }
}
 

/**** End of File ****/
