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
#include "speaker_op.h"
#include "rom_drivers.h"

#include "seg7led.h"
#include "oled2864.h" 
#include "tools.h"
#include "spi_uart.h"
   
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
extern void ADC_Test(void);
extern volatile uint32_t p3_2_counter;
extern volatile uint32_t p3_2_tc;

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
	 uint32_t	cnt_last =0;
	 uint32_t	tc1, tc;//记录system值
	 uint32_t  cnt_mo, cnt_mo2;
   uint8_t nalve=0;//用于统计毫秒
   uint32_t time_use=0;
	 uint32_t distance=0;
	 uint8_t disbuf[20]={0};
   char buf[20]={0};
   SystemInit();

  GPIOInit();						// Enables clock for GPIO
  /* Called for system library in core_cm0.h. */
  SysTick_Config(48000);


  SPI_IOConfig(0);
  SPI_Init(0, 8, 2);
  //NVIC_SetPriority(SSP0_IRQn, 1);
  OLED_Init_I();
  UARTInit(115200);
  NVIC_SetPriority(UART_IRQn, 2);

	OLED_ClearScreen();
    OLED_DisStrLine(1, 0, "distance");
  //led
	GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
    GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off LED1
	
	GPIOSetDir(PORT2, 6, 1);			// Set PIO2_6 to output
	GPIOSetValue(PORT2, 6, 0);		// 
	
	//配置采集为普通IO，输入模式
	LPC_IOCON->R_PIO1_2 &= ~0x07;	
    LPC_IOCON->R_PIO1_2 |= 0x01;		/* CLK OUT */
	
	GPIOSetDir(PORT1, 2, 0);	
	GPIOIntDisable(PORT1, 2);
     printf(" now\r\n");

  while(1)
  {	 
		
		turn_flag = SysTick->VAL;
		GPIOSetValue(PORT2, 6, 1);
		if((turn_flag > 480))//10us 高电平；使能信号
		{ GPIOSetValue(PORT3, 0, 0);	
		  while(SysTick->VAL  > (turn_flag-480));
		  GPIOSetValue(PORT2, 6, 0);
			 
		}else
		{ GPIOSetValue(PORT3, 0, 0);	
		   while((SysTick->VAL > (48000+turn_flag-480))|(SysTick->VAL <turn_flag));
		  GPIOSetValue(PORT2, 6, 0);
			 
         }
		//获取传播时间
		 GPIOSetInterrupt(PORT1, 2, 0, 0, 1);
		 GPIOIntEnable(PORT1, 2);
	     mo=0;
	     cnt_last=p3_2_counter  ;
    while(p3_2_counter == cnt_last);
         cnt_last = p3_2_counter;
	     tc1 = p3_2_tc;
		 cnt_mo = mo ;
	  GPIOSetInterrupt(PORT1, 2, 0, 0, 0);
    while(p3_2_counter == cnt_last);
		 cnt_last = p3_2_counter;
		 cnt_mo2=mo;
		 GPIOIntDisable(PORT1, 2);
		
		//时间统计
			nalve=cnt_mo2-cnt_mo;
		
		if(p3_2_tc < tc1)
	   {
			 if(nalve>0)
			 {
					nalve=nalve-1;
			  tc= 48000 - (p3_2_tc - tc1);
        }
				else
				{ tc = tc1 - p3_2_tc;}
	   }
	   else
	    {
	        tc= 48000 - (p3_2_tc - tc1);
					nalve=nalve-1;
	    }	
		  time_use =  (nalve*1000+tc/48);
      distance=(uint32_t)( time_use*17/100);//计算距离
			
		  printf(" distance=%d\r\n", distance);
		  disbuf[0]= (uint8_t)(distance/1000);
			disbuf[1]=(uint8_t)(distance%1000/100);
			disbuf[2]=(uint8_t)(distance%100/10);
			disbuf[3]=(uint8_t)(distance%10);
	
			if(distance < 4500)
			snprintf(buf, 16, "dts: %d%d%d%d mm ", disbuf[0], disbuf[1],disbuf[2],disbuf[3]);
		  OLED_DisStrLine(3, 0, (uint8_t *)buf);

			delay_ms(500);
   }
  
		

}
 

/**** End of File ****/
