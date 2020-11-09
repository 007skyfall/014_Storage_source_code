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


/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void) 
{
  unsigned int	i;

  SystemInit();	 //初始化系统的时钟

  GPIOInit();

  // LED
  GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
  GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off LED1

  while(1)                          /* Loop forever */
  {
    for(i=0; i<250000; i++);
	  GPIOSetValue(PORT3, 0, 0); //led1亮

	for(i=0; i<250000; i++);
	GPIOSetValue(PORT3, 0, 1);	//led1灭
  }
}
 

/**** End of File ****/
