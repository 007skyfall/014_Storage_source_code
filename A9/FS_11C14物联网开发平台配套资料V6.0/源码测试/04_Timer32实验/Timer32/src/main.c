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
#include "lpc11xx.h"
#include "gpio.h"
#include "timer32.h"

/* External global variables -------------------------------------------------*/
extern volatile uint32_t timer32_0_counter;

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

  // Fan
  GPIOSetDir(PORT0, 2, 1);			// Set PIO0_2 to output    
  GPIOSetValue(PORT0, 2, 1);		// PIO0_2 output 0, Turn off Fan

  GPIOSetDir(PORT3, 0, 1);			// Set PIO3_2 to output
  GPIOSetValue(PORT3, 0, 1);		// PIO3_2 output 1,	Turn off Speaker 

  
  init_timer32(0, TIME_INTERVAL);
  enable_timer32(0);

  while(1)
  {
    if(timer32_0_counter < 50)
	{
	  GPIOSetValue(PORT3, 0, 0);
	}
	else if((timer32_0_counter >= 50) && (timer32_0_counter < 100))
	{
	  GPIOSetValue(PORT3, 0, 1);
	}
	else
	{
	  timer32_0_counter = 0;
	}	
  }
}
 

/**** End of File ****/
