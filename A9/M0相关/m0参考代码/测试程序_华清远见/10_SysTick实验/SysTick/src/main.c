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
																				   
void SysTick_Handler(void)
{
  static uint32_t	Tick=0;

  Tick ++;

  if(Tick == 500)
  {
    GPIOSetValue(PORT3, 1, 0);		// PIO3_1 output 1, turn on LED2
  }
  else if(Tick >= 1000)
  {
    GPIOSetValue(PORT3, 1, 1);		// PIO3_1 output 1, turn off LED2

	Tick = 0;
  }
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

  // LED
  GPIOSetDir(PORT3, 1, 1);			// Set PIO3_1 to output
  GPIOSetValue(PORT3, 1, 1);		// PIO1_1 output 1, Turn off LED2

  /* Called for system library in core_cm0.h. */
  SysTick_Config(48000);
  
  // Go to sleep to save power between timer interrupts
  __WFI();

  while(1);
}
 

/**** End of File ****/
