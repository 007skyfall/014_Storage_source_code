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
#include "clkconfig.h"
#include "gpio.h"
#include "uart.h"
#include "timer32.h"
#include "ssp.h"
#include "i2c.h"
#include "adc.h"
#include "rom_drivers.h"

#include "oled2864.h" 
#include "light.h"
#include "acc.h"
#include "key.h"
#include "rfid.h"
#include "tools.h"
#include "spi_uart.h"
#include "temp_hum.h"
#include "menu.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* External global variables -------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//__align(4) uint32_t	iap_test=0;
//char buf[32];

/* External functions --------------------------------------------------------*/
extern void AVIOD_Test(void);
extern void FLAME_Test(void);
extern void HALL_Test(void);
extern void COMPASS_Test(void);
extern void BALL_SWITCH_Test(void);
extern void Touch_Test(void);
extern void VIDEO_test(void);
extern void Tarck_Test(void);
extern void Shock_Test( void);
extern void GAS_Test( void);
extern void DISTANCE_Test( void);
extern void IrDA_test(void);
extern void RELAY_Test(void);
extern void LIGHT_A_Test(void); 
  uint32_t turn_flag=0;
//uint32_t  tes_k=0;
uint32_t mo=0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : SysTick interrupt Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void)
{
		  	mo++;
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
  Menu_TypeDef	menu;
  uint32_t	key;
  SystemInit();

  GPIOInit();						// Enables clock for GPIO

  CLKOUT_Setup(CLKOUTCLK_SRC_MAIN_CLK);
  LPC_IOCON->PIO0_1 &= ~0x07;	
  LPC_IOCON->PIO0_1 |= 0x01;		/* CLK OUT */

  KEY_Init();

  /* Called for system library in core_cm0.h. */
  SysTick_Config(48000);
  
 
  SPI_IOConfig(0);
  SPI_Init(0, 8, 2);
  OLED_Init_I();

  menu.max_numb = 11;
  menu.numb = 1;
  menu.title = "Main";
  menu.item[0] = "1. aviod";
  menu.item[1] = "2. flame";
  menu.item[2] = "3. hall";
  menu.item[3] = "4. campass";
  menu.item[4] = "5. ball-switch";
  menu.item[5] = "6. touch";

  menu.item[6] = "7. track";
  menu.item[7] = "8. gas";
  menu.item[8] = "9. IrDA_";
  menu.item[9] = "10.relay ";
  menu.item[10] = "11.distance ";

  Dis_Menu(menu);
  UARTInit(384000);
  NVIC_SetPriority(UART_IRQn, 2);
  delay_ms(250);
  
  while(1)
  {
    key = KEY_Read();
    switch(key)
	{
	  case KEY_UP:
	    if(menu.numb > 1)
		  menu.numb --;
		else
		  menu.numb = menu.max_numb;
		Dis_Menu(menu);
		break;

	  case KEY_DOWN:	    
		if(menu.numb < menu.max_numb)
		  menu.numb ++;
		else
		  menu.numb = 1;
		Dis_Menu(menu);
		break;
	  case KEY_SEL:
	    switch(menu.numb)
		{
		  case 1:   	    
		    AVIOD_Test();
		    break;
		  case 2:
			FLAME_Test();
		    break;
		  case 3: 	
			HALL_Test();
		    break;
		  case 4:		 	
			COMPASS_Test();
		    break;

		  case 5:
		    BALL_SWITCH_Test();
			break;

		  case 6:
		    Touch_Test();
			break;

		  case 7:
		  	   Tarck_Test();
			break;

		  case 8:
		   		 GAS_Test();
			break;

		  case 9:
		   	   IrDA_test();
		    break;

		 case 10:
		   	  	RELAY_Test();
		    break;

		case 11:
			  DISTANCE_Test();
			 break;
		  default:
		    break;
		}
	    break;
	  
	  case KEY_ESC:	    	    
        Dis_Menu(menu);
	//	printf("\r\n\r\nFS_11C14 Main Menu");		
	    break;
		
	  default:
	    break; 
	}

	if(key != KEY_NONE)
	{
	  delay_ms(250);
	}

	// Go to sleep to save power between timer interrupts
    __WFI();
  }
}
 

/**** End of File ****/
