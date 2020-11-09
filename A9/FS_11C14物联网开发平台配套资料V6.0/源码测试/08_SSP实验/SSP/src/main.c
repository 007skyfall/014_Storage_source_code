/******************** (C) COPYRIGHT 2010 NXP Semiconductors************
* File Name          : main.c
* Author             : NXP Semiconductors
* Date First Issued  : 01/18/2010
* Description        : Main program body.
*******************************************************************************
*******************************************************************************
* History:
* 01/18/2010		 : V1.0		   initial version
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC11xx.h"                        /* LPC11xx definitions */
#include "gpio.h"
#include "ssp.h"
#if SSP_DEBUG
#include "uart.h"
#endif

/* Private define ------------------------------------------------------------*/
#define SSP_NUM			0

/* Private variables ---------------------------------------------------------*/
uint8_t src_addr[SSP_BUFSIZE]; 
uint8_t dest_addr[SSP_BUFSIZE];

/**
  * @brief  SSP_LoopbackTest
  * @param  port #
  * @retval None
  */
void SSP_LoopbackTest( uint8_t portNum )
{
  uint32_t i;

  if ( portNum == 0 )		  //SSP0
  {
#if !USE_CS
	/* Set SSEL pin to output low. */
	GPIOSetValue( PORT0, 2, 0 );
#endif
	i = 0;
	while ( i <= SSP_BUFSIZE )
	{
	  /* to check the RXIM and TXIM interrupt, I send a block data at one time 
	  based on the FIFOSIZE(8). */
	  SSP_Send( portNum, (uint8_t *)&src_addr[i], FIFOSIZE );
	  /* If RX interrupt is enabled, below receive routine can be
	  also handled inside the ISR. */
	  SSP_Receive( portNum, (uint8_t *)&dest_addr[i], FIFOSIZE );
	  i += FIFOSIZE;
	}
#if !USE_CS
	/* Set SSEL pin to output high. */
	GPIOSetValue( PORT0, 2, 1 );
#endif
  }
  else		 //SSP1
  {
#if !USE_CS
	/* Set SSEL pin to output low. */
	GPIOSetValue( PORT2, 0, 0 );
#endif
	i = 0;
	while ( i <= SSP_BUFSIZE )
	{
	  /* to check the RXIM and TXIM interrupt, I send a block data at one time 
	  based on the FIFOSIZE(8). */
	  SSP_Send( portNum, (uint8_t *)&src_addr[i], FIFOSIZE );
	  /* If RX interrupt is enabled, below receive routine can be
	  also handled inside the ISR. */
	  SSP_Receive( portNum, (uint8_t *)&dest_addr[i], FIFOSIZE );
	  i += FIFOSIZE;
	}
#if !USE_CS
	/* Set SSEL pin to output high. */
	GPIOSetValue( PORT2, 0, 1 );
#endif
  }
  return;
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main (void)
{
  uint32_t i;

  int8_t temp[2];

  
  SystemInit();


  UARTInit(115200);


  printf("\n\r-- SSP Test V1.20 --\n\r");

  SSP_IOConfig( SSP_NUM );	/* initialize SSP port, share pins with SPI1
						       on port2(p2.0-3). */
  SSP_Init( SSP_NUM );			
						
  for ( i = 0; i < SSP_BUFSIZE; i++ )
  {
	src_addr[i] = (uint8_t)i;
	dest_addr[i] = 0;
  }

  SSP_LoopbackTest( SSP_NUM );

  for ( i = 0; i < SSP_BUFSIZE; i++ )
  {
	if ( src_addr[i] != dest_addr[i] )
	{
	  UARTSend((uint8_t *)&temp[0], 2);
	  UARTSend("\n\r", 2);
      printf("\n\r SSP Loopback Test failed!\n\r");
	  while( 1 );			/* Verification failed */
	}
  }

  printf("\n\r SSP Loopback Test success!\n\r");

  while (1);
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/************* (C) COPYRIGHT 2010 NXP Semiconductors*****END OF FILE****/

