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

  SystemInit();

  GPIOInit();

  // LED
  GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
  GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off LED1
   GPIOSetDir(PORT2, 6, 1);			//继电器的控制引脚
  while(1)                          /* Loop forever */
  {
    for(i=0; i<5000000; i++);
    {	GPIOSetValue(PORT3, 0, 1);
    	GPIOSetValue(PORT2, 6, 0);}	//继电器 常开断开，常闭闭合

	for(i=0; i<5000000; i++);
   {	GPIOSetValue(PORT3, 0, 0);
	 GPIOSetValue(PORT2, 6, 1);}	 //继电器 常开闭合，常闭断开
  }
}
 

/**** End of File ****/
