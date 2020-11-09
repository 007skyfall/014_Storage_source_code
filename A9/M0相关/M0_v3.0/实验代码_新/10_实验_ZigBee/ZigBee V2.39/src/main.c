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
  char		buf[25];
  uint8_t	rd;
  uint16_t	key;
  uint32_t	i;

  uint8_t	temp[2], hum[2];

  uint32_t	lux;

  int32_t	xoff = 0;
  int32_t	yoff = 0;
  int32_t	zoff = 0;
  int8_t	x = 0;
  int8_t	y = 0;
  int8_t	z = 0;

  uint32_t	adv;
  float		vol;


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
  GPIOSetValue(PORT0, 2, 0);		// PIO0_2 output 0, close FAN

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

  menu.max_numb = 7;
  menu.numb = 1;
  menu.title = "ZigBee";
  menu.item[0] = "1.Send Hello ZigBee!";
  menu.item[1] = "2.Send Turn on FAN";
  menu.item[2] = "3.Send Turn off FAN";
  menu.item[3] = "4.Send Temp & Hum";
  menu.item[4] = "5.Send Light";
  menu.item[5] = "6.Send Axis-3";
  menu.item[6] = "7.Send Voltage";
  
  Dis_Menu(menu);

  UARTInit(115200);
  NVIC_SetPriority(UART_IRQn, 3);

  printf("\r\n\r\nZigBee");

  delay_ms(250);

  SPI_IOConfig(1);
  SPI_Init(1, 8, 2);

  SPI752_Init(1, 115200);

  I2CInit(I2CMASTER, 0);

  light_init();
  light_enable();
  light_setRange(LIGHT_RANGE_4000);

  acc_init();
  /* Assume base board in zero-g position when reading first value. */
  acc_read(&x, &y, &z);
  xoff = 0-x;
  yoff = 0-y;
  zoff = 0-z;

  ADCInit(ADC_CLK);

  x = 0;
  
  while(1)
  {
	if(UART0_GetChar(&rd))
    {
	  ZigBee_PutChar(rd);
    }

	if(ZigBee_GetChar(&rd))
    {
	  UART0_PutChar(rd);

	  OLED_DisChar(1, x, 0, rd);
	  x += 6;
	  if(x > 120)
	  {
	    x = 0;
	  }
	  
	  for(i=0; i<7; i++)
	  {
	    buf[i] = buf[i+1];
	  }
	  buf[i] = rd;

	  if((buf[0]=='F') && (buf[1]=='A')
	     && (buf[2]=='N') && (buf[3]==' ')
		 && (buf[4]=='O') && (buf[5]=='n')
		 && (buf[6]==' ') && (buf[7]==' '))
		{
		  GPIOSetValue(PORT0, 2, 1);		// Turn on Fan
		}
		else if((buf[0]=='F') && (buf[1]=='A')
	     && (buf[2]=='N') && (buf[3]==' ')
		 && (buf[4]=='O') && (buf[5]=='f')
		 && (buf[6]=='f') && (buf[7]==' '))
		{
		  GPIOSetValue(PORT0, 2, 0);		// Turn off Fan
		}
    }

    key = KEY_Read();

    switch(key)
	{
	  case KEY_UP:
	    if(menu.numb > 1)
		  menu.numb --;
		else
		  menu.numb = menu.max_numb;
		Dis_Menu(menu);
		x = 0;
		delay_ms(250);
		break;

	  case KEY_DOWN:	    
		if(menu.numb < menu.max_numb)
		  menu.numb ++;
		else
		  menu.numb = 1;
		Dis_Menu(menu);
		x = 0;
		delay_ms(250);
		break;

	  case KEY_SEL:
	    switch(menu.numb)
		{
		  case 1:		    
			ZigBee_PutString("Hello ZigBee!  ");
			printf("\r\nSend Hello ZigBee!  ");
		    break;

		  case 2:		    
			ZigBee_PutString("FAN On  ");
			printf("\r\nSend Turn On FAN  ");
		    break;

		  case 3:		    
			ZigBee_PutString("FAN Off  ");
			printf("\r\nSend Turn Off FAN  ");			
		    break;

		  case 4:
		    i = Read_Temp_Hum(temp, hum);

			if(i)
			{
		      snprintf(buf, 16, "Temp: -%d.%d  ", temp[0], temp[1]);
			  ZigBee_PutString((uint8_t *)buf);
			  snprintf(buf, 16, "Hum:  %d.%d  ", hum[0], hum[1]);
			  ZigBee_PutString((uint8_t *)buf);
		
			  printf("\r\nSend Temp: %d.%d   Hum:  %d.%d  ", temp[0], temp[1], hum[0], hum[1]);
		    }
		    else
		    {
		      printf("\r\nfail");
		    }			
		    break;

		  case 5:
		    lux = light_read();
		    snprintf(buf, 20, "Light: %d  ", lux);
		    ZigBee_PutString((uint8_t *)buf);
			printf("\r\nSend Light: %d  ", lux);
		    break;

		  case 6:
		    acc_read(&x, &y, &z);
		    x = x+xoff;
		    y = y+yoff;
		    z = z+zoff;
		
		    snprintf(buf, 20, "x:%2d  y:%2d  z:%2d  ", x, y, z);
		    ZigBee_PutString((uint8_t *)buf);
			printf("\r\nSend x:%2d  y:%2d  z:%2d  ", x, y, z);
		    break;

		  case 7:
		    adv = ADCRead(0);
			vol = adv*3.3/1023;
			adv = vol*10;
			snprintf(buf, 21, "Voltage: %d.%dV   ", adv/10, adv%10);
			ZigBee_PutString((uint8_t *)buf);
			printf("\r\nSend Voltage: %d.%dV  ", adv/10, adv%10);
		    break;

		  default:
		    break;
		}
		delay_ms(250);
	    break;
		
	  default:
	    break; 
	}
  }
}
 

/**** End of File ****/
