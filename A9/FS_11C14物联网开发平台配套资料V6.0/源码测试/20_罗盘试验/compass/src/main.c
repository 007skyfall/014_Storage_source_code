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
#include <math.h>
#include "lpc11xx.h"
#include "clkconfig.h"
#include "gpio.h"
#include "uart.h"
#include "ssp.h"
#include "speaker_op.h"
#include "rom_drivers.h"
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



//IIC use the extern functions
//extern void Delayus(unsigned int number);

extern void iic_start(void);
extern void iic_stop(void);
extern void iic_ack(uint8_t value);
extern void iic_NoAck(void);
extern uint32_t iic_testack(void);
extern void iic_write8bit(unsigned char input);
extern unsigned char iic_read8bit(void);
#define	SlaveAddress   0x3C	  //定义器件在IIC总线中的从地址
unsigned char BUF[8]={0};
/*HMC5883 FUNCTIONS*/
//***************************************************

void Single_Write_HMC5883(unsigned char REG_Address,unsigned char REG_data)
{
    iic_start();                  //起始信号
    iic_write8bit(SlaveAddress);   //发送设备地址+写信号
    iic_write8bit(REG_Address);    //内部寄存器地址，请参考中文pdf 
    iic_write8bit(REG_data);       //内部寄存器数据，请参考中文pdf
    iic_stop();                   //发送停止信号
}
//********单字节读取内部寄存器*************************
/*
uchar Single_Read_HMC5883(uchar REG_Address)
{  uchar REG_data;
   iic_start();                          //起始信号
   iic_write8bit(SlaveAddress);           //发送设备地址+写信号
   iic_write8bit(REG_Address);                   //发送存储单元地址，从0开始	
   iic_start();                         //起始信号
   iic_write8bit(SlaveAddress+1);         //发送设备地址+读信号
   REG_data=iic_read8bit();              //读出寄存器数据
	 iic_ack(1);   
	 iic_stop();                           //停止信号
    return REG_data; 
}*/
//******************************************************
//
//连续读出HMC5883内部角度数据，地址范围0x3~0x5
//
//******************************************************
void Multiple_read_HMC5883(void)
{   uint8_t i;
    iic_start();                          //起始信号
    iic_write8bit(SlaveAddress);           //发送设备地址+写信号
    iic_write8bit(0x03);                   //发送存储单元地址，从0x3开始	
    iic_start();                          //起始信号
    iic_write8bit(SlaveAddress+1);         //发送设备地址+读信号
	 for (i=0; i<6; i++)                      //连续读取6个地址数据，存储中BUF
    {
        BUF[i] = iic_read8bit();          //BUF[0]存储数据
        if (i == 5 )
        {
          iic_ack(1);                //最后一个数据需要回NOACK
        }
        else
        {
          iic_ack(0);                //回应ACK
       }
   }
    iic_stop();                          //停止信号
    delay_ms(5);
}

//初始化HMC5883，根据需要请参考pdf进行修改****
void Init_HMC5883()
{
     Single_Write_HMC5883(0x02,0x00);  //连续读取，
	   //Single_Write_HMC5883(0x01,0x40);  //增益调整
}
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
  int16_t x=0,y=0;
	//int16_t z=0;
  double angle=0;
	uint8_t disbuf[20]={0};
  char buf[20]={0};
  SystemInit();

  GPIOInit();						// Enables clock for GPIO

  /* Called for system library in core_cm0.h. */
  SysTick_Config(48000);
  delay_ms(500);
  SPI_IOConfig(0);
  SPI_Init(0, 8, 2);
  //NVIC_SetPriority(SSP0_IRQn, 1);
  OLED_Init_I();
 
  UARTInit(115200);
  NVIC_SetPriority(UART_IRQn, 2);
  delay_ms( 100);
	OLED_ClearScreen();
  OLED_DisStrLine(1, 0, "COMPASS");
  
	//配置采集为普通IO，输入模式
	LPC_IOCON->R_PIO1_2 &= ~0x01f;	
  LPC_IOCON->R_PIO1_2 |= 0x11;		/* CLK OUT */
	GPIOSetDir(PORT1, 2, 1);			// use P1.2 as CLK; OUTPUT	
	
	GPIOSetDir(PORT2, 6, 1);			// use p2.6 as data;Set PIO2_6 to output
 	GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
  GPIOSetValue(PORT3, 0, 0);		// PIO3_0 output 1, Turn off LED1
  Init_HMC5883();
	delay_ms(400);
	
  while(1)
  {	
		
    Multiple_read_HMC5883();      //连续读出数据，存储在BUF中
    x=BUF[0] << 8 | BUF[1]; //Combine MSB and LSB of X Data output register
   // z=BUF[2] << 8 | BUF[3]; //Combine MSB and LSB of Z Data output register
    y=BUF[4] << 8 | BUF[5]; //Combine MSB and LSB of Y Data output register
		angle= atan2((double)y,(double)x) * (180 / 3.1415) + 180; // angle in degrees
		
    angle=angle*10;
		
		disbuf[0]= (uint8_t)(((uint32_t)angle)/1000);
	    disbuf[1]=(uint8_t)(((uint32_t)angle)%1000/100);
		disbuf[2]=(uint8_t)(((uint32_t)angle)%100/10);
		disbuf[3]=(uint8_t)(((uint32_t)angle)%10);
		snprintf(buf, 16, "dts: %d%d%d.%d ", disbuf[0], disbuf[1],disbuf[2],disbuf[3]);
		OLED_DisStrLine(2, 0, (uint8_t *)buf);
		
		angle=angle/10;	
		//方向计算需要校正：磁偏角和所处的地里位置有关
		 if((angle < 22.5) || (angle > 337.5 ))
        OLED_DisStrLine(3, 0,"South     ");
     if((angle > 22.5) && (angle < 67.5 ))
        OLED_DisStrLine(3, 0,"South-West");
     if((angle > 67.5) && (angle < 112.5 ))
        OLED_DisStrLine(3, 0,"West      ");
     if((angle > 112.5) && (angle < 157.5 ))
        OLED_DisStrLine(3, 0,"North-West");
     if((angle > 157.5) && (angle < 202.5 ))
        OLED_DisStrLine(3, 0,"North     ");
     if((angle > 202.5) && (angle < 247.5 ))
        OLED_DisStrLine(3, 0,"North-East");
     if((angle > 247.5) && (angle < 292.5 ))
        OLED_DisStrLine(3, 0,"East      ");
     if((angle > 292.5) && (angle < 337.5 ))
        OLED_DisStrLine(3, 0,"South-East");
		 
		delay_ms(500);
   }
}

/**** End of File ****/
