#include <stdio.h>
#include "LPC11xx.h"
#include "gpio.h"
#include "oled2864.h"
#include "tools.h"
#include "watchdog.h"

/****************************************************************************
 *   $Id:: wdt.c 3635 2010-06-02 00:31:46Z usb00423                         $
 *   Project: NXP LPC11xx WDT(Watchdog timer) example
 *
 *   Description:
 *     This file contains WDT code example which include WDT 
 *     initialization, WDT interrupt handler, and APIs for WDT
 *     reading.
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
//#include "wdt.h"

volatile uint32_t wdt_counter;

/*****************************************************************************
** Function name:		WDTHandler
**
** Descriptions:		Watchdog timer interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void WDT_IRQHandler(void)
{


#if 0//PROTECT_MODE
 	  uint32_t i;
  LPC_WDT->FEED = 0xAA;		/* Feeding sequence */
  LPC_WDT->FEED = 0x55;    
  /* Make sure feed sequence executed properly */
  for (i = 0; i < 0x80000; i++);

#endif

// LPC_WDT->MOD &= ~WDTOF;		/* clear the time-out flag and interrupt flag */
 //LPC_WDT->MOD &= ~WDINT;		/* clear the time-out flag and interrupt flag */
  OLED_ClearScreen();
  OLED_DisStrLine(1, 0, "   Watchdog Test!");
  OLED_DisStrLine(2-1 + 2, 0, " watchdog irq_handler Ok!");
  OLED_DisStrLine(2-1 + 4, 0, "    irq_finish                      ");
  delay_ms(2000);
  OLED_ClearScreen(); 
  return;
}

/*****************************************************************************
** Function name:		WDTInit
**
** Descriptions:		Initialize watchdog timer, install the
**						watchdog timer interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void WDTInit( void )
{
 // uint32_t i;

  /* Enable clock to WDT */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<15);		  	//system AHB clock control register

  LPC_SYSCON->WDTOSCCTRL = 0x03F;				//Watchdog oscillator control register
  LPC_SYSCON->PDRUNCFG &= ~(0x1<<6);			 //system osillator control register

  LPC_SYSCON->WDTCLKSEL = 0x02;		/* Select watchdog osc */
  LPC_SYSCON->WDTCLKUEN = 0x01;		/* Update clock */
  LPC_SYSCON->WDTCLKUEN = 0x00;		/* Toggle update register once */
  LPC_SYSCON->WDTCLKUEN = 0x01;
  while ( !(LPC_SYSCON->WDTCLKUEN & 0x01) );		/* Wait until updated */
  LPC_SYSCON->WDTCLKDIV = 2;		/* Divided by 128 */  
  wdt_counter = 0;

//  NVIC_EnableIRQ(WDT_IRQn);

  LPC_WDT->TC = WDT_FEED_VALUE ;	/* once WDEN is set, the WDT will start after feeding */


#if  WATCHDOG_RESET
  LPC_WDT->MOD = WDEN | WDRESET;
#else
  LPC_WDT->MOD = WDEN;
#endif

  LPC_WDT->FEED = 0xAA;		/* Feeding sequence */
  LPC_WDT->FEED = 0x55; 
     
  /* Make sure feed sequence executed properly */
 // for (i = 0; i < 0x80000; i++);

  /* For WDWARNINT test */
 LPC_WDT->WARNINT = 0x1FF;
 // OLED_DisStrLine(1, 0, " Watchdog Start!");

  return;
}

/*****************************************************************************
** Function name:		WDTFeed
**
** Descriptions:		Feed watchdog timer to prevent it from timeout
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void WDTFeed( void )
{
  LPC_WDT->FEED = 0xAA;		/* Feeding sequence */
  LPC_WDT->FEED = 0x55;
  //OLED_DisStrLine(2-1 + 2, 0, " feed Ok!");
//  OLED_DisStrLine(2-1 + 4, 0, "   feed     finish   ");
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

