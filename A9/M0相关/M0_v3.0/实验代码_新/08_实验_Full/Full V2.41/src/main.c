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

#include "seg7led.h"
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
extern void LED_Test(void);
extern void Speaker_Test(void);
extern void Fan_Test(void);
extern void ADC_Test(void);
extern void CAN_Test_ROM(void);
extern void CAN_Test(void);



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
  static uint32_t	Tick=0;
  static uint8_t	cnt=0;

  Tick ++;

  if(Tick == 500)
  {
    GPIOSetValue(PORT3, 1, 0);		// PIO3_1 output 1, turn on LED2

	Seg7Led_Put(cnt);

	cnt ++;
	cnt &= 0x1f;
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
  Menu_TypeDef	menu;
  uint32_t	key;


  SystemInit();

  GPIOInit();						// Enables clock for GPIO

  CLKOUT_Setup(CLKOUTCLK_SRC_MAIN_CLK);
  LPC_IOCON->PIO0_1 &= ~0x07;	
  LPC_IOCON->PIO0_1 |= 0x01;		/* CLK OUT */

  // LED
  GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
  GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, close LED1

  GPIOSetDir(PORT3, 1, 1);			// Set PIO3_1 to output
  GPIOSetValue(PORT3, 1, 1);		// PIO1_1 output 1, close LED2

  // Fan
  GPIOSetDir(PORT0, 2, 1);			// Set PIO0_2 to output    
  GPIOSetValue(PORT0, 2, 1);		// PIO0_2 output 1, close FAN

  // Speaker
  LPC_IOCON->R_PIO1_1 &= ~0x07;
  LPC_IOCON->R_PIO1_1 |= 0x01;
  GPIOSetDir(PORT1, 1, 1);			// Set PIO3_2 to output
  GPIOSetValue(PORT1, 1, 1);		// PIO3_2 output 1,	Speaker turn off

  // 7 Segment LED
  Seg7Led_Init();
  Seg7Led_Put(' ');					// Output ' '

  KEY_Init();

  /* Called for system library in core_cm0.h. */
  SysTick_Config(48000);
  
  GPIOSetValue(PORT3, 0, 0);		// PIO3_0 output 0, Turn on LED1
  GPIOSetValue(PORT3, 1, 0);		// PIO3_1 output 0, Turn on LED2
  delay_ms(200);
  GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off LED1
  GPIOSetValue(PORT3, 1, 1);		// PIO3_1 output 1, Turn off LED2

  SPI_IOConfig(0);
  SPI_Init(0, 8, 2);
  OLED_Init_I();

  menu.max_numb = 12;
  menu.numb = 1;
  menu.title = "Main";
  menu.item[0] = "1. LED";
  menu.item[1] = "2. Speaker";
  menu.item[2] = "3. Fan";
  menu.item[3] = "4. OLED";
  menu.item[4] = "5. Temp & Hum";
  menu.item[5] = "6. Light";
  menu.item[6] = "7. Axis-3";
  menu.item[7] = "8. Voltage";
  menu.item[8] = "9. RFID";
  menu.item[9] = "10. ZigBee";
  menu.item[10] = "11.RS-485";
  menu.item[11] = "12.CAN";

  Dis_Menu(menu);

  UARTInit(115200);
  NVIC_SetPriority(UART_IRQn, 2);

  printf("\r\n\r\nFS_11C14 Main Menu");

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
		    printf("\r\n\r\nLED Test");		    
		    LED_Test();
		    break;

		  case 2:
		    printf("\r\n\r\nSpeaker Test");
			Speaker_Test();
		    break;

		  case 3:
		    printf("\r\n\r\nFan Test");		
			Fan_Test();
		    break;

		  case 4:
		    printf("\r\n\r\nOLED Test");		
			OLED_Test();
		    break;

		  case 5:
		    printf("\r\n\r\nTemperature & Humidity Test");
			Temp_Hum_Test();
			break;

		  case 6:
		    printf("\r\n\r\nLight Test");
			Light_Test();
			break;

		  case 7:
		    printf("\r\n\r\n3 Axis Accelerometer Test");
			Axis3_Test();
			break;

		  case 8:
		    printf("\r\n\r\nVoltage Test");
			ADC_Test();
			break;

		  case 9:
		    printf("\r\n\r\nRFID Test");
		    RFID_Test();
		    break;

		  case 10:
		    printf("\r\n\r\nZigBee Test");
		    ZigBee_Test();
            break;

		  case 11:
		    printf("\r\n\r\nRS-485 Test");
		    RS485_Test();
            break;

		  case 12:
		    printf("\r\n\r\nCAN Test");
		    CAN_Test();
            break;

		  default:
		    break;
		}
	    break;
	  
	  case KEY_ESC:	    	    
        Dis_Menu(menu);
		printf("\r\n\r\nFS_11C14 Main Menu");		
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
