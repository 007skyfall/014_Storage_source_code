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
#include "gpio.h"
#include "uart.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* External global variables -------------------------------------------------*/
extern volatile uint32_t 	UARTCount;
extern volatile uint8_t		UARTBuffer[UART0_RBUF_SIZE];

/* Private variables ---------------------------------------------------------*/
/* External functions --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
  int	i;

  SystemInit();

  GPIOInit();						// Enables clock for GPIO



  /* NVIC is installed inside UARTInit file. */
  UARTInit(115200);

 // printf("\r\n\r\n-- UART Test --");
  UART0_printf("\r\n\r\n-- UART Test --");
 // printf("\r\nPlease input a number on the keyboard:\r\n");
  UART0_printf("\r\nPlease input a number on the keyboard:\r\n");


  scanf("%d", &i);
  printf("\r\n%d", i);

  printf("\r\n\r\nPlease input any key on the keyboard:\r\n");

  while(1)		/* Loop forever */
  {	
	if(UARTCount != 0)
	{
	  LPC_UART->IER = IER_THRE | IER_RLS;				// Disable RBR
	  UARTSend((uint8_t *)UARTBuffer, UARTCount);
	  UARTCount = 0;
	  LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;		// Re-enable RBR
	}
  }
}
 

/**** End of File ****/
