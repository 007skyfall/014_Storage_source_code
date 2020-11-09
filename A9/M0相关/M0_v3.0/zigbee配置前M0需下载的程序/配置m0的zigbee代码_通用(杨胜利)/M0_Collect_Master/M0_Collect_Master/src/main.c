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
#include "crc16.h"


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
extern void FAN_Test(void);
extern void ADC_Test(dev_data_t *dev);
extern void CAN_Test_ROM(void);
extern void CAN_Test(void);

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern uint32_t p2_8_counter ;
/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : SysTick interrupt Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t send_flag = 0;
uint32_t display_tick=0	 ;
void SysTick_Handler(void)
{
  static uint32_t	Tick=0,tick_send=0,display_tick=0;
//  static uint8_t	cnt=0;

  Tick ++;
  tick_send++;
  display_tick++;
  if(display_tick >= 10000)
  {
  	display_tick  =0;
	OLED_DisStrLine(2-1 + 4, 0, "Frid :                 ");
	OLED_DisStrLine(2-1 + 6, 0, "                      ");
	
  	
  }
  if(tick_send >= 2000)
  {
  	tick_send =0;
	send_flag=1;
  	
  }

  if(Tick == 500)
  {
    GPIOSetValue(PORT3, 1, 0);		// PIO3_1 output 1, turn on LED2
	GPIOSetValue(PORT1, 1, 1);		// PIO1_1 output 1, Turn off Speaker

//	Seg7Led_Put(cnt);

//	cnt ++;
//	cnt &= 0x1f;
  }
  else if(Tick >= 1000)
  {
    GPIOSetValue(PORT3, 1, 1);		// PIO3_1 output 1, turn off LED2

	Tick = 0;
  }
}
/*******************************************************************************
* Function Name  : send_package
* Description    : send_package to uart 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void send_package(uint8_t type , dev_data_t *dev)
{
	uint8_t sbuf[20];
	uint16_t data_crc16;
	if(type==0x01)
	{
		sbuf[0]=0xaa;
		dev->dev_no =1;
		sbuf[1]=dev->dev_no;
		dev->type_data = type;
		sbuf[2]=dev->type_data;
		dev->length = 1;
		sbuf[3]=dev->length;
		dev->cmd = 0x01;
		sbuf[4]=dev->cmd;	
		data_crc16 =make_crc16(0,sbuf,5);
		sbuf[5]=(data_crc16&0xff00)>>8;
		sbuf[6]=data_crc16&0xff;
		UARTSend(sbuf, 7);
		ZigBee_Send(sbuf, 7);	

	}
	else if(type==0x02) 
	{
		sbuf[0]=0xaa;
		dev->dev_no =1;
		sbuf[1]=dev->dev_no;
		dev->type_data = type;
		sbuf[2]=dev->type_data;
		dev->length = 11;
		sbuf[3]=dev->length;
		sbuf[4]=dev->temp[0];
		sbuf[5]=dev->temp[1];
		sbuf[6]=dev->hum[0];
		sbuf[7]=dev->hum[1];
		sbuf[8]=dev->bright[0];
		sbuf[9]=dev->bright[1];
		sbuf[10]=dev->adc[0];
		sbuf[11]=dev->adc[1];
		sbuf[12]=dev->acc[0];
		sbuf[13]=dev->acc[1];
		sbuf[14]=dev->acc[2];
		data_crc16 =make_crc16(0,sbuf,15);
		sbuf[15]=(data_crc16&0xff00)>>8;
		sbuf[16]=data_crc16&0xff;
		UARTSend(sbuf, 17);
		ZigBee_Send(sbuf, 17);
	//	printf("%s\n",sbuf);
		
		
	}
	else if(type ==0x04) 
	{
		sbuf[0]=0xaa;
		dev->dev_no =1;
		sbuf[1]=dev->dev_no;
		dev->type_data = type;
		sbuf[2]=dev->type_data;
		dev->length = 6;
		sbuf[3]=dev->length;
		sbuf[4]=dev->rfid[0];
		sbuf[5]=dev->rfid[1];
		sbuf[6]=dev->rfid[2];		
		sbuf[7]=dev->rfid[3];
		sbuf[8]=dev->rfid[4];
		sbuf[9]=dev->rfid[5];
		data_crc16 =make_crc16(0,sbuf,10);
		sbuf[10]=(data_crc16&0xff00)>>8;
		sbuf[11]=data_crc16&0xff;
		UARTSend(sbuf, 12);
		
		ZigBee_Send(sbuf, 12);
		display_tick=0; //control display time
//		OLED_DisStrLine(2-1 + 6, 0, "                   ");
		
	}
	
}

void peripheral_init(void)
{
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

  // NVIC is installed inside UARTInit file.
  UARTInit(115200);
  //NVIC_SetPriority(UART_IRQn, 0);

 // printf("\r\n\r\nFS_11C14\r\n");

  
  GPIOSetValue(PORT3, 0, 0);		// PIO3_0 output 0, Turn on LED1
  GPIOSetValue(PORT3, 1, 0);		// PIO3_1 output 0, Turn on LED2
  delay_ms(200);
  GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off LED1
  GPIOSetValue(PORT3, 1, 1);		// PIO3_1 output 1, Turn off LED2

  GPIOSetDir(PORT2, 8, 0);
  GPIOSetValue(PORT2, 8, 1);		// PIO2_8 output 1,
   // port2_8 interrupt. edge, single trigger, falling edges.
  GPIOSetInterrupt(PORT2, 8, 0, 0, 0);
 // GPIOIntEnable(PORT2, 8);
 // GPIOIntDisable(PORT2,8);

  SPI_IOConfig(0);
  SPI_Init(0, 8, 2);
  // 初始化zigbee 部分
  SPI_IOConfig(1);
  SPI_Init(1, 8, 2);
	
}

void board_init(void)
{
  SPI752_Init(1, 115200);
  OLED_Init_I();
  ADCInit(ADC_CLK);	 //adc init
  I2CInit(I2CMASTER, 0); // i2c init
  acc_init();			 // acc init
	
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
//  Menu_TypeDef	menu;
//  uint32_t	key;
//  uint8_t sbuf[20];
  dev_data_t  *dev_data;
  dev_data_t  dev;
  dev_data = &dev;
  


  SystemInit();
  
  peripheral_init();
  board_init();
  
  
  delay_ms(20);
//  GPIOIntDisable(PORT2,8);
  send_package(1,dev_data);
  GPIOIntEnable(PORT2, 8);
  p2_8_counter =0;
  
  
  
  
 
  while(1)
  {
	if(send_flag ==1)
	{ 	
		send_flag =0;
		
		Temp_Hum_Test(dev_data);
		Light_Test(dev_data);
		send_package(2,dev_data);
		ADC_Test(dev_data);
		Axis3_Test(dev_data);		
	}
	if( p2_8_counter >=1)
	{
		p2_8_counter =0;
		RFID_Test(dev_data);
		send_package(4,dev_data);		
	}

 	
    


	// Go to sleep to save power between timer interrupts
    __WFI();
  }
}
 
/**** End of File ****/
