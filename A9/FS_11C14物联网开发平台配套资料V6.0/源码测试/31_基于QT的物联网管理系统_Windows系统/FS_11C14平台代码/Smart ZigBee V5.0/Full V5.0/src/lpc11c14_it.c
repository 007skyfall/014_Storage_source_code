/****************************************************************************
 *   $Id:: gpio.c 4068 2010-07-30 02:25:13Z usb00423                        $
 *   Project: NXP LPC11xx GPIO example
 *
 *   Description:
 *     This file contains GPIO code example which include GPIO 
 *     initialization, GPIO interrupt handler, and related APIs for 
 *     GPIO access.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#include "LPC11xx.h"			/* LPC11xx Peripheral Registers */
#include "gpio.h"
#include "spi_uart.h"

volatile uint32_t gpio0_counter = 0;
volatile uint32_t p0_1_counter  = 0;

volatile uint32_t gpio1_counter = 0;
volatile uint32_t p1_1_counter  = 0;

volatile uint32_t gpio2_counter = 0;
volatile uint32_t p2_8_counter  = 0;

volatile uint32_t gpio3_counter = 0;
volatile uint32_t p3_1_counter  = 0;
volatile uint32_t p3_2_counter  = 0;
volatile uint32_t p3_2_tc;

char zigbee_flag = 0;
char rfid_flag	 = 0;
char up_flag 	 = 0;
char down_flag   = 0;
char left_flag   = 0;
char right_flag  = 0;
char esc_flag	 = 0;
char sel_flag    = 0;

/*****************************************************************************
** Function name:		PIOINT0_IRQHandler
**
** Descriptions:		Use one GPIO pin(port0 pin1) as interrupt source
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void PIOINT0_IRQHandler(void)
{
  uint32_t regVal;

  gpio0_counter++;
  regVal = GPIOIntStatus( PORT0, 1 );
  if ( regVal )
  {
	p0_1_counter++;
	GPIOIntClear( PORT0, 1 );
  }	
  
  if(GPIOIntStatus(PORT0,3))
  {
  	sel_flag = 1;
	GPIOIntClear(PORT0, 3);
  }
  	
  return;
}

/*****************************************************************************
** Function name:		PIOINT1_IRQHandler
**
** Descriptions:		Use one GPIO pin(port1 pin1) as interrupt source
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void PIOINT1_IRQHandler(void)
{

	if(GPIOIntStatus(PORT1,4))
  {
  	esc_flag = 1;
	GPIOIntClear(PORT1, 4);
  }

  if(GPIOIntStatus(PORT1, 5))
  {
	p3_2_tc = SysTick->VAL;      

	p3_2_counter++;
	GPIOIntClear(PORT1, 5);
  }	

  if(GPIOIntStatus(PORT1,8))
  {
  	up_flag = 1;
	GPIOIntClear(PORT1, 8);
  }

  if(GPIOIntStatus(PORT1,9))
  {
  	down_flag = 1;
	GPIOIntClear(PORT1, 9);
  }

  if(GPIOIntStatus(PORT1,10))
  {
  	left_flag = 1;
	GPIOIntClear(PORT1, 10);
  }

  if(GPIOIntStatus(PORT1,11))
  {
  	right_flag = 1;
	GPIOIntClear(PORT1, 11);
  }

}

/*****************************************************************************
** Function name:		PIOINT2_IRQHandler
**
** Descriptions:		Use one GPIO pin(port2 pin1) as interrupt source
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void PIOINT2_IRQHandler(void)
{  
  if(GPIOIntStatus(PORT2, 8))
  {     
	rfid_flag = 1;
	GPIOIntDisable(PORT2,8);
	GPIOIntClear(PORT2, 8);
  }		
}

/*****************************************************************************
** Function name:		PIOINT3_IRQHandler
**
** Descriptions:		Use one GPIO pin(port3 pin1) as interrupt source
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void PIOINT3_IRQHandler(void)
{
 if(GPIOIntStatus(PORT3, 2))
  {
	GPIOIntClear(PORT3, 2);
  }
 
  if(GPIOIntStatus(PORT3, 3))
  {
  	//printf("888\r\n");
    zigbee_flag = 1;
	//printf("888\r\n");
	//GPIOIntDisable(PORT3,3);
    ZigBee_IRQ_Process();
	GPIOIntClear(PORT3, 3);
  }
}


/******************************************************************************
**                            End Of File
******************************************************************************/
