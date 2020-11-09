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
#include "tools.h"


volatile uint32_t period = 1500;
void speaker_op(uint8_t pwm_set)  //´«1Ïì ´«0²»Ïì
{
	switch(pwm_set)
	{
		case 1:
			init_timer32PWM(1, period, MATCH0);
			LPC_TMR32B1->PR = 0x7f;
			enable_timer32(1);

	   	break;
		case 0:
		//	init_timer32PWM(1, 0, MATCH0);
		//	delay_ms(50);
			disable_timer32(1);
		break;
		default:
			break;
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
  
  GPIOInit();
  SysTick_Config(48000); //set systick timer
  // Speaker
  LPC_IOCON->R_PIO1_1 &= ~0x07;
  LPC_IOCON->R_PIO1_1 |= 0x01;		//set PIO1_1 gpio function mode
  GPIOSetDir(PORT1, 1, 1);			// Set PIO3_2 to output
  GPIOSetValue(PORT1, 1, 1);		// PIO3_2 output 1,	Speaker turn off
  //speaker_op(1);
  while(1)                          /* Loop forever */
  {
	speaker_op(1);			  //turn on Speaker
	delay_ms(1000);				   
	speaker_op(0);		       //turn off Speaker
	delay_ms(1000);
  }
}
 

/**** End of File ****/
