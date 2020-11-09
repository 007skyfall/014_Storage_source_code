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


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* External global variables -------------------------------------------------*/
extern volatile uint32_t timer32_0_counter;

/* Private variables ---------------------------------------------------------*/
/* External functions --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


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
  GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
  GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off LED1

 // GPIOSetDir(PORT3, 1, 1);			// Set PIO3_1 to output
 // GPIOSetValue(PORT3, 1, 1);		// PIO1_1 output 1, Turn off LED2

  // Fan
  GPIOSetDir(PORT0, 2, 1);			// Set PIO0_2 to output    
  GPIOSetValue(PORT0, 2, 1);		// PIO0_2 output 0, Turn off Fan

  /* Called for system library in core_cm0.h. */
  SysTick_Config(48000);
  
  init_timer32(0, TIME_INTERVAL);
  NVIC_SetPriority(TIMER_32_0_IRQn, 3);
  enable_timer32(0);

  while(1)
  {
    if(timer32_0_counter < 50)
	{
	  GPIOSetValue(PORT3, 0, 0); //µãÁÁled
	}
	else if((timer32_0_counter >= 50) && (timer32_0_counter < 100))
	{
	  GPIOSetValue(PORT3, 0, 1);	// Ï¨Ãðled
	}
	else
	{
	  timer32_0_counter = 0;
	}
		
	// Go to sleep to save power between timer interrupts
    __WFI();
  }
}
 

/**** End of File ****/
