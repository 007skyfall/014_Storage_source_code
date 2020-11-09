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
#include "ssp.h"
#include "i2c.h"
#include "rom_drivers.h"

#include "oled2864.h" 
#include "tools.h"
#include "spi_uart.h"
unsigned char disbuf[8][128];

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : SysTick interrupt Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void)
{
 
}
 int Pixel(uint8_t row, uint16_t col)
{
  uint8_t	y, temp;
  if((row>64)||(col>128)) {return -1;}
  col = col-1;
  y = (row-1)/8;
  Set_Start_Page(y);
  Set_Start_Column(col);
  temp = (row-1)%8;
  switch(temp)
  {
  	case 0:
		
           Write_Data(0x01 | disbuf[y][col]);
		   disbuf[y][col] =  disbuf[y][col] | 0x01;
		   break;
   	case 1:
           Write_Data(0x02 | disbuf[y][col]);
		   disbuf[y][col] =  disbuf[y][col] | 0x02;
		   break;
   	case 2:
           Write_Data(0x04 | disbuf[y][col]);
			disbuf[y][col] =  disbuf[y][col] | 0x04;
		   break;
   	case 3:
           Write_Data(0x08 | disbuf[y][col]);
		   disbuf[y][col] =  disbuf[y][col] | 0x08;
		   break;
   	case 4:
           Write_Data(0x10 | disbuf[y][col]);
		   disbuf[y][col] =  disbuf[y][col] | 0x10;
		   break;
   	case 5:
           Write_Data(0x20 | disbuf[y][col]);
		   disbuf[y][col] =  disbuf[y][col] | 0x20;
		   break;
   	case 6:
           Write_Data(0x40 | disbuf[y][col]);
		   disbuf[y][col] =  disbuf[y][col] | 0x40;
		   break;
   	case 7:
           Write_Data(0x80 | disbuf[y][col]);
		   disbuf[y][col] =  disbuf[y][col] | 0x80;
		   break;
    default:
			break;
  }
  return 0;
}   
/**************************************************************8
param: x,y : fyrst Pyxel   
	   l   : length
	   w   : wydth

***************************************************************/
 int rectangle(int rowbit, int colbit, int l, int w)
 {
	int temprowbit;
	int tempcolbit;
	if((rowbit+l)>129||(colbit+w)>65)	  {return -1;}
	for(tempcolbit = colbit; tempcolbit < colbit+l; tempcolbit++)
	{
		Pixel(rowbit, tempcolbit);			//----
		Pixel(rowbit+w-1, tempcolbit);
	}
   for(temprowbit = rowbit; temprowbit < rowbit+w; temprowbit++)
   {
   		Pixel(temprowbit, colbit);
		Pixel(temprowbit, colbit+l-1);

   }
		return 0;
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

  LPC_IOCON->PIO0_1 &= ~0x07;	
  LPC_IOCON->PIO0_1 |= 0x01;		/* CLK OUT */

  /* Called for system library in core_cm0.h. */
  SysTick_Config(48000);

  SPI_IOConfig(0);
  SPI_Init(0, 8, 2);
  NVIC_SetPriority(SSP0_IRQn, 1);
  OLED_Init_I();
  OLED_DisStrLine(1, 0, "   OLED Test!");
  
  OLED_DisStrLine(2, 0, "   hello World!");
  delay_ms(2000);
	
  OLED_ClearScreen();		
  while(1)	
  {
  	delay_ms(250);
  	rectangle(1, 1, 128, 64);
  	rectangle(10, 10, 110, 50);
  	rectangle(15, 15, 100, 40);
  	rectangle(20, 20, 90, 30);
  	rectangle(25, 25, 80, 20);
  	rectangle(30, 30, 70, 10);
  	rectangle(33, 35, 60, 4);
	delay_ms(250);
	Fill_RAM(0x00);
	delay_ms(250);
	rectangle(33, 35, 60, 4);
	rectangle(30, 30, 70, 10);
	rectangle(25, 25, 80, 20);
	rectangle(20, 20, 90, 30);
	rectangle(15, 15, 100, 40);
	rectangle(10, 10, 110, 50);
	rectangle(1, 1, 128, 64);
	delay_ms(250);
	Fill_RAM(0x00);
  }
  
 // while(1);
 // OLED_Test();
    
}
 

/**** End of File ****/
