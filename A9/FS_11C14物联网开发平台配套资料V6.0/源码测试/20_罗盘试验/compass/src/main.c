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
#define	SlaveAddress   0x3C	  //����������IIC�����еĴӵ�ַ
unsigned char BUF[8]={0};
/*HMC5883 FUNCTIONS*/
//***************************************************

void Single_Write_HMC5883(unsigned char REG_Address,unsigned char REG_data)
{
    iic_start();                  //��ʼ�ź�
    iic_write8bit(SlaveAddress);   //�����豸��ַ+д�ź�
    iic_write8bit(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf 
    iic_write8bit(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf
    iic_stop();                   //����ֹͣ�ź�
}
//********���ֽڶ�ȡ�ڲ��Ĵ���*************************
/*
uchar Single_Read_HMC5883(uchar REG_Address)
{  uchar REG_data;
   iic_start();                          //��ʼ�ź�
   iic_write8bit(SlaveAddress);           //�����豸��ַ+д�ź�
   iic_write8bit(REG_Address);                   //���ʹ洢��Ԫ��ַ����0��ʼ	
   iic_start();                         //��ʼ�ź�
   iic_write8bit(SlaveAddress+1);         //�����豸��ַ+���ź�
   REG_data=iic_read8bit();              //�����Ĵ�������
	 iic_ack(1);   
	 iic_stop();                           //ֹͣ�ź�
    return REG_data; 
}*/
//******************************************************
//
//��������HMC5883�ڲ��Ƕ����ݣ���ַ��Χ0x3~0x5
//
//******************************************************
void Multiple_read_HMC5883(void)
{   uint8_t i;
    iic_start();                          //��ʼ�ź�
    iic_write8bit(SlaveAddress);           //�����豸��ַ+д�ź�
    iic_write8bit(0x03);                   //���ʹ洢��Ԫ��ַ����0x3��ʼ	
    iic_start();                          //��ʼ�ź�
    iic_write8bit(SlaveAddress+1);         //�����豸��ַ+���ź�
	 for (i=0; i<6; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
    {
        BUF[i] = iic_read8bit();          //BUF[0]�洢����
        if (i == 5 )
        {
          iic_ack(1);                //���һ��������Ҫ��NOACK
        }
        else
        {
          iic_ack(0);                //��ӦACK
       }
   }
    iic_stop();                          //ֹͣ�ź�
    delay_ms(5);
}

//��ʼ��HMC5883��������Ҫ��ο�pdf�����޸�****
void Init_HMC5883()
{
     Single_Write_HMC5883(0x02,0x00);  //������ȡ��
	   //Single_Write_HMC5883(0x01,0x40);  //�������
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
  
	//���òɼ�Ϊ��ͨIO������ģʽ
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
		
    Multiple_read_HMC5883();      //�����������ݣ��洢��BUF��
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
		//���������ҪУ������ƫ�Ǻ������ĵ���λ���й�
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
