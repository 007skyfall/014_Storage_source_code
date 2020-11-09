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
#include <string.h>
#include <stdlib.h>
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
#include "speaker_op.h"
#include "key.h"
#include "rfid.h"
#include "tools.h"
#include "spi_uart.h"
#include "temp_hum.h"
#include "menu.h"

#include "read_data.h"
#include "send_data.h"
#include "sys_init.h"
#include "myrfid.h"


#define  start_machine       0xAA   //开始时机器的编号
/*单字节取位数*/
#define	 H7_6				 0xc0	//用于取出6、7位
#define	 H5_4				 0x30	//用于取出4、5位
#define	 L3_0				 0x0f	//用于取出低4位

//----------------------
/*设备类型*/
#define	 FAN				 0x00   //设备类型   风扇
#define  SPEAKER			 0x10   //设备类型   蜂鸣器
#define  LED 				 0x20   //设备类型   LED
#define  LED7			   	 0x30   //设备类型   数码管

//----------------------
/*设备参数*/
#define  FAN_OFF			 0x00	//关闭风扇
#define  FAN_ON1 			 0x01	//打开风扇一档
#define  FAN_ON2 			 0x02	//打开风扇二档
#define  FAN_ON3 			 0x03	//打开风扇三档

#define  LED_OFF	 		 0x00	//关闭LED
#define  LED_ON     		 0x01   //打开LED       

#define  LED7_OFF  	 		 0x01	//关闭数码管
#define  LED7_ON  	 		 0x00	//打开数码管

#define  SPEAKER_OFF 		 0x00	//关闭蜂鸣器
#define  SPEAKER_ON			 0x01	//打开风蜂鸣器
#define  SPEAKER_START_OFF	 0x02	//关闭蜂鸣器状态
#define  SPEAKER_START_ON	 0x03	//打开蜂鸣器状态

/* Critical Value Set ----------------------------------------------------------*/


/*温度、湿度、亮度、电量  临界值*/
#define max_tmp 			  50	//温度最大值
#define min_tmp 			  10	//温度最小值
//----------------------------------------------
#define max_hum 		      50	//湿度最大值
#define min_hum 			  10	//湿度最小值
//----------------------------------------------
#define max_lig 			  500	//光度最大值
#define min_lig 			  10	//光度最小值
//----------------------------------------------
//电池电量最低报警值
#define min_battery1 		  3		//电量整数位
#define min_battery2 		  30	//电量小数位

/* Critical Value Set End -------------------------------------------------------*/
uint8_t  card_flag = 0;
uint8_t STORAGE_NUM = 1;             //仓库编号
extern uint8_t	SPI752_rbuf_1[];
volatile uint32_t time_counter0=0 ;
volatile uint32_t time_counter1=0 ;
storage_t     storage_s;
extern char type ;
char sbuf[8];
uint8_t snum = 1;  
extern int pwm;
volatile char rfid_flag;
extern char zigbee_flag; 
extern char warn_flag;
uint8_t automatic_flag = 1;
int8_t xyz=0;
char beep_mask ;
/* Global Variable Define End --------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* External global variables -------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* External functions --------------------------------------------------------*/
extern void LED_Test(void);
extern void Speaker_Test(void);
extern void Fan_Test(void);
extern void ADC_Test(void);
extern void CAN_Test_ROM(void);
extern void CAN_Test(void);
extern void OLED_Test(void);
extern void Temp_Hum_Test(void);
extern void Axis3_Test(void);
extern void Light_Test(void);

char send_flag = 0;  
uint8_t temp; 
uint8_t   *p = &temp;
uint8_t  env_flag = 0;
message_t message;   
char sdbuf[8];
char buf[32]; 
uint8_t beep_flag=0; 

uint8_t A[6][128] = {
//创科联动
/*
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,
0x60,0x30,0x18,0x1C,0x0E,0x1E,0x30,0x70,0x60,0x40,0xC0,0xF0,0x10,0x00,0x80,0xF0,
0x3C,0x0C,0x00,0x00,0x00,0x00,0x18,0x18,0x18,0xE8,0x78,0x0C,0x0C,0x04,0x06,0x00,
0x00,0x78,0x70,0x00,0x00,0xE0,0x7C,0x0E,0x02,0x00,0x00,0x00,0x00,0x00,0x8C,0xFC,
0xBC,0x8C,0x8C,0xCC,0xFC,0x1C,0x0C,0x6C,0x40,0x40,0xDE,0xC8,0x40,0x70,0x58,0x4E,
0x46,0x22,0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0xC0,
0xC0,0xC0,0xC0,0xF0,0xFE,0xCE,0xC2,0xC0,0xC0,0x40,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xE3,0x7D,0x0F,
0x03,0x43,0x43,0x63,0x3F,0x0F,0x01,0xC0,0xF0,0x3E,0x07,0x00,0xE0,0x7C,0x0F,0x01,
0x00,0x00,0x00,0x80,0xC3,0x63,0xF3,0xFF,0x1F,0xB3,0x73,0x43,0x03,0x01,0x8C,0x9E,
0x98,0x80,0xC0,0xF8,0xDF,0xC3,0x40,0x60,0x00,0x00,0x00,0x00,0xE0,0x7C,0x1F,0x11,
0xD0,0xF8,0x1E,0x87,0x10,0x10,0x10,0x90,0xF0,0xFC,0x1F,0x11,0x10,0x10,0x10,0x10,
0x00,0x00,0x00,0x00,0x84,0xC2,0x72,0x3A,0x0E,0x06,0xE2,0xE2,0x02,0x02,0x80,0xC0,
0x70,0x3C,0x0F,0x01,0x80,0xF0,0x3C,0x0F,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x3F,0x33,0x30,0x30,
0x30,0x30,0x1C,0x0C,0x00,0x00,0x00,0x01,0x31,0x30,0x38,0x1F,0x03,0x00,0x00,0x00,
0x00,0x02,0x03,0x01,0x30,0x3E,0x07,0x00,0x00,0x03,0x03,0x03,0x01,0x01,0x01,0x01,
0x71,0x3E,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x06,0x07,0x03,0x02,0x33,0x3F,
0x07,0x31,0x31,0x18,0x0C,0x06,0x03,0x01,0x00,0x07,0x1F,0x3C,0x18,0x00,0x00,0x00,
0x00,0x00,0x06,0x07,0x03,0x02,0x03,0x03,0x31,0x31,0x1B,0x0F,0x0C,0x06,0x03,0x01,
0x30,0x18,0x18,0x1E,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,*/

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xC0,0xC0,0x60,0x60,0x60,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0xE0,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,
0x0C,0xCC,0xFC,0x3F,0x0F,0x0C,0x0C,0x04,0x00,0x00,0x00,0x10,0x98,0x88,0x8C,0x8C,
0xCC,0xDC,0xFC,0x38,0x00,0x00,0x00,0xC0,0xF8,0x7C,0x34,0x18,0x08,0x0C,0x0C,0x04,
0x00,0x00,0x00,0x00,0xF0,0xF8,0xCC,0x8C,0x8C,0x8C,0x8C,0x1C,0x18,0x00,0x00,0x00,
0x00,0x00,0xC0,0xF8,0x3C,0x04,0x00,0x00,0x00,0x00,0x40,0xF0,0xF8,0x8C,0x8C,0x8C,
0xDC,0x7C,0x38,0x0C,0x0C,0x04,0xC0,0xF8,0x7F,0x17,0x18,0x0C,0x0C,0xFC,0xFC,0x38,
0x00,0x00,0x00,0x08,0x0C,0xCC,0xFC,0x3F,0x0F,0x0C,0x0C,0x04,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,
0x1E,0x0F,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x1F,0x19,0x19,0x19,0x09,0x15,
0x1E,0x1F,0x01,0x00,0x00,0x10,0x1E,0x0F,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x0C,0x1C,0x18,0x18,0x18,0x18,0x0D,0x0F,0x07,0x00,0x00,0x00,0x00,0x00,
0x10,0x1E,0x0F,0x01,0x00,0x00,0x00,0x60,0xF0,0x9E,0x8F,0x8D,0x8D,0x8D,0x8D,0x4D,
0x7C,0x38,0x00,0x00,0x10,0x1E,0x0F,0x01,0x00,0x00,0x00,0x10,0x1E,0x0F,0x01,0x00,
0x00,0x00,0x00,0x00,0x0E,0x1F,0x19,0x18,0x18,0x08,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
uint8_t B[6][128];
uint8_t Nu[200];
void beep()
{
    beep_flag=1;
    delay_ms(200);
    beep_flag=0;
	speaker_op(0);
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
  static uint32_t	Tick=0,Tick_other=0;

  Tick ++;
  Tick_other++;
  if (beep_flag==1)
  {
    speaker_op(1);
  }
  else 
  {
    speaker_op(0);
  }
  if(Tick_other >=100) //100毫秒执行一次
  {
  	Tick_other=0;
  }

/*PORT3_1 LED2 每秒亮一次*/
  if(Tick == 500)
  {
    GPIOSetValue(PORT3, 1, 0);		// PIO3_1 output 1, turn on LED2
    time_counter0++;
    time_counter1++;

  }
  else if(Tick >= 1000)
  {
    GPIOSetValue(PORT3, 1, 1);		// PIO3_1 output 1, turn off LED2   
	Tick = 0;
  }
}
void Rfid_answer(void)
{
//	uint8_t key = 0,Nu = 100;
//	uint8_t	chksum;
	uint8_t rbuf[20] = {0};
	uint8_t id[6] = {0};
//	uint16_t oled_flag = 0;

//	char buf[6] = {0};
//	uint16_t num = 0;
//	uint8_t flag = 0;
	uint32_t i = 0;
	struct sendGoods sgd;
	 char flag1 = 'r';
    char type = 'I';
 
	//delay_ms(200);
	//speaker_op(0);
	Rfid_Operation(rbuf);
	//delay_ms(500);
	if(rbuf[0] == 1 && (rbuf[1]==255||rbuf[1]==0))
		return;

//	card_flag = 1;
//	speaker_op(1);
	beep();

	delay_ms(200);
//	speaker_op(0);
	//	chksum	= RFID_CheckSum(rbuf);
	//	if (chksum == rbuf[rbuf[0]])
		
	//	} else {
	//		return 1;
	//	}   

	//GPIOIntDisable(PORT1,4);                //使能按键中断

//	OLED_ClearScreen();
	memset(&storage_s,0,storage_len);
	for(i=0;i<6;i++)
	{
		storage_s.rfid[i] = rbuf[i];
		printf("%d ",storage_s.rfid[i]);
	}
	printf("\n");
	sprintf((char *)id,"%02x %02x %02x %02x %02x %02x\0",rbuf[0],rbuf[1],rbuf[2],rbuf[3],rbuf[4],rbuf[5]);
	//OLED_DisStr(0,1,0,"the id is:");
		  //chksum == RFID_CheckSum(rbuf);
		  //if(chksum == rbuf[rbuf[0]]){
			rbuf[6] = 0x04;
			rbuf[7] = 0x05;
		    sgd.head[0] = 's';
            sgd.head[1] = 't';
            sgd.head[2] = ':';
            sgd.type = flag1;
            sgd.snum = STORAGE_NUM;
            sgd.io = type;
            sgd.goodsno = rbuf[6];
            sgd.goodsnum = rbuf[7];
		//	sgd.goodsno ='0x01';
           // sgd.goodsnum = '0x49';
            
            memcpy(sbuf,&sgd,8);
		//	}
			
		//	OLED_DisStrLine(1,0,id);
	
	       send_data(sbuf,8);

} 

/* SysTick_Handler End --------------------------------------------------------*/

void fill_message(message_t *msg,uint8_t machine_id,uint8_t address)
{
    msg->type = 'c';
    msg->machine_num = machine_id;
    msg->slave_address = address;
}

int sure(uint8_t y)
{
	char i;
	for(i = 0;i < 100;i++)
	{
		if(y == Nu[i])
			return 1;
	}
	return 0;
}
/*******************************************************************************
*函数名       ：开机LOGO
*函数功能     ：展示一段开机动画
********************************************************************************/
void Logo(void)
{
	int i, j;
	char n,m;
	uint8_t x,y,*q = (uint8_t *)B;
	srand(6);
	for(i = 0;i<768;i++)
		q[i] = 0xff;
	show(q);
	delay_ms(700);
	i = j = 0;

/***********恢复把A 和 B都变成LOGO*********/
	for(i = 0;i<768;i++)
		q[i] = ((uint8_t *)A)[i];
	show((uint8_t *)A);
	delay_ms(700);
				
/*************横渐灭 ***********/
	srand(2);
	j = 0;
	while(j!=32) {
	 	 y = rand() % 48;
		 if(y > 32 || y < 1)
		 	continue;
		 if(sure(y))
		 	continue;
		 n = y & 0x07;
		 m = y >> 3;

		 for(i = 0; i < 128; i++) {
		  	  A[m][i] &= ~(1<<n);			  			  
		 } 
		 Nu[j++] = y; 
		 show((uint8_t *)A);
		 delay_ms(20);
	}
	delay_ms(350);
/*************竖渐亮***********/	
	srand(5);
	j = 0;
	while(j<95) {
	 	 x = rand() % 128;
		 if(x > 110 || x < 15)
		 	continue;
		 if(sure(x))
		 	continue;

		 for(i = 0; i < 6; i++) {
		  	  A[i][x] = B[i][x];			  			  
		 } 
		 Nu[j++] = x; 
		 show((uint8_t *)A);
	}	
}
uint8_t three_handshake(void)
{
    char ch='a';
    char *p_ch=&ch;
   
	OLED_DisStrLine(1,0,"connect to a8.....  ");
   
    while(1)
    {
    	while( 1 )
	{	  
		if(zigbee_flag == 0)
		{
            fill_message(&message,start_machine,0);
			send_data((char *)&message,3);
			delay_ms(2000);
			continue;
		}
		else
			break;
	}
    	while(zigbee_flag == 1)	
		{
				zigbee_flag = 0;
				ZigBee_GetChar(p);												
		}				
        if (*p == 'Y')
          {
           send_data(p_ch,1);
           break;
          }
    }
    OLED_ClearScreen();
    OLED_DisStrLine(3,0,"connect is ok!");
    delay_ms(1000);
    return 0;
}



void alarm_and_display(char *sendbuf)
{
    char buf[32];
	char Axis_3[32];	

	char autobuf[32];
    uint8_t temperature[2];
	uint8_t humidity[2];
    uint32_t light[1];
    struct sendEnv *envMsg = NULL;
    uint32_t tmp0 = 0, tmp1 = 0;
	uint32_t adc0 = 0, adc1 = 0;
    uint8_t l=0,t=0,h=0;
    envMsg = (struct sendEnv *)sendbuf;
        if (beep_mask == 'K')
        {
              Speaker_OnOff(0);
        }
        if (warn_flag == 1)
        {
             Speaker_OnOff(0);
             GPIOIntEnable(PORT1,4);
        }
        if (warn_flag == 2)
        {   
            Speaker_OnOff(1);
            warn_flag = 0;
        }

    	if(time_counter0 >= 1 )	 //每秒采集一次信息
		{	
			time_counter0 = 0;				 	
			Collect_Data(STORAGE_NUM,temperature,humidity,light,sendbuf);
			if(strlen(sendbuf)>5)		//采集数据长度大于20
			{
				send_data(sendbuf, 24);		//调用ZigBee发送数据函数
 
                memset(buf,0,32);		  //清buf
                OLED_DisStrLine(0,0,"storage one");
				//snprintf(buf, 32,"T:%d.%d H:%d.%d L:%d  ",temperature[0],temperature[1],humidity[0],humidity[1],light[0]);
				//snprintf(buf,16,"TEMP:%d.%d\n",temperature[0],temperature[1]);
				snprintf(buf, 16, "Temp: %d.%d  ", temperature[0], temperature[1]);
				OLED_DisStrLine(2, 0, "                      ");
			    OLED_DisStrLine(2, 0, (uint8_t *)buf);
//				OLED_DisChar(2, 0, 0,(uint8_t *)buf);

				//sprintf(buf,"HUMP:%d.%d\n",humidity[0],humidity[1]);
				snprintf(buf, 16, " Hum: %d.%d  ", humidity[0],humidity[1]);
				OLED_DisStrLine(3, 0, (uint8_t *)buf);

				//snprintf(buf,"LIGHT:%d\n",light[0]);
				snprintf(buf, 20, "Light:%d    ", light[0]);
				OLED_DisStrLine(4, 0, (uint8_t *)buf);
				
		//-----------------------------------
	  
                if (time_counter1 >= 30)
                {
                    automatic_flag = 1;
                }
  				if(automatic_flag == 1)		
				{	     
					snprintf(autobuf, 16, "AUTO :  %s    ","ON");
					//OLED_DisStrLine(3, 0, (uint8_t *)autobuf);
 //-------------------------------------------------------------------------------------------------------------
		
					if((temperature[0] < min_tmp) || (temperature[0] > max_tmp) )	//温度异常报警
						{ 
						   t = 1;
						   beep_mask = 'K';
						   Speaker_OnOff(0);
				     	   snprintf(buf, 32, "BEEP STATUS : %c",beep_mask);
		   				   //OLED_DisStrLine(5, 0, (uint8_t *)buf);

						   if(temperature[0] > max_tmp)			   //高温报警启动风
						   {
						   	   enable_timer32(0);
					 	 	   pwm = 500;
						 	   if(pwm <= 50 )
								{
									snprintf(buf, 16, "FAN : %s    ","OFF");
									//OLED_DisStrLine(7, 0, (uint8_t *)buf);
									disable_timer32(0); 
								}
								else
								{
									snprintf(buf, 16, "FAN :  %d    ",pwm);
									//OLED_DisStrLine(7, 0, (uint8_t *)buf);
								}
						   }
						}	
						
					if((temperature[0] >= min_tmp) && (temperature[0] <= max_tmp))	//温度正常
					{
					   t = 0;
					   if(temperature[0] <= max_tmp)
					   {
					   	  disable_timer32(0);
					      GPIOSetDir(PORT0, 2, 1);
					      GPIOSetValue(PORT0, 2, 1);
					      pwm = 50;
					      if(pwm <= 50 )
							{
								snprintf(buf, 16, "FAN :  %s    ","OFF");
								//OLED_DisStrLine(7, 0, (uint8_t *)buf);
								disable_timer32(0); 
							}
							else
							{
								snprintf(buf, 16, "FAN :  %d    ",pwm);
								//OLED_DisStrLine(7, 0, (uint8_t *)buf);
							}
					   }
					   
					}
			
//-------------------------------------------------------------------------------------------------------------
  	 			
				   if((humidity[0] < min_hum) || (humidity[0] > max_hum))	//湿度异常报警
					{
					   h = 1;
						   //	beep_mask = 'K';
						
						   	//Speaker_OnOff(0);
							beep_flag = 1;
				     	    snprintf(buf, 32, "BEEP STATUS : %c",beep_mask);
		   				    //OLED_DisStrLine(5, 0, (uint8_t *)buf);
							
					} 

					if((humidity[0] >= min_hum) && (humidity[0] <= max_hum))	//湿度正常
					{
					   h = 0;
					}

//-------------------------------------------------------------------------------------------------------------
		 	 	  
				  if((light[0] < min_lig ) || (light[0] > max_lig))	//光度异常报警
					{

					   l = 1;
						   	beep_mask = 'K';
						   	Speaker_OnOffsw(0);
				     	    snprintf(buf, 32, "BEEP STATUS : %c",beep_mask);
		   				    //OLED_DisStrLine(5, 0, (uint8_t *)buf);		
					}

					if((light[0] >= min_lig) && (light[0] <= max_lig))	//光度正常
					{
					  l = 0;
					}
									
					if(!(t+h+l+xyz))		//所有报警标志位和为0时停止报警
					{
					   beep_mask = 'G';			 
					   GPIOSetValue(PORT1, 1, 1);
					   snprintf(buf, 32, "BEEP STATUS : %c",beep_mask);
					   //OLED_DisStrLine(5, 0, (uint8_t *)buf);
					}			
			       }
			      else 
			        {
			            xyz=0;
			        }
             }

            snprintf(Axis_3, 20, "x:%-3d  y:%-3d  z:%-3d ", envMsg->x,envMsg->y,envMsg->z);
        	OLED_DisStrLine(7, 0, (uint8_t *)Axis_3);
            tmp1 = ((envMsg->bet)*3.6)/1024.0;   					 //取出电池电压整数位
        	tmp0 = (((envMsg->bet)*3.6)/1024.0 - tmp1) * 100;		 //取出电池电压小数位
            adc1 = ((envMsg->adc)*3.6)/1024.0;   					 //取出电池电压整数位
        	adc0 = (((envMsg->adc)*3.6)/1024.0 - adc1) * 100;		 //取出电池电压小数位
            sprintf(buf,"BAT:%d.%dV ",tmp1, tmp0);
        	OLED_DisStrLine(5, 0, (uint8_t *)buf);
			sprintf(buf,"ADC:%d.%dV",adc1,adc0);
			OLED_DisStrLine(6,0,(uint8_t *)buf);
		}	

}

void receive_command(void)
{
    char autobuf[20];
    char buf[16];

    		if (zigbee_flag == 1)
		{
			zigbee_flag = 0;
			if (ZigBee_GetChar (p))		 //获取一个字节数据
			{
				if ((*p & H7_6) == STORAGE_NUM << 6)	//判断6、7位
				{
					time_counter1 = 0;					//自动报警计时器置0
					if(automatic_flag == 1 )			
					{
						snprintf(autobuf, 16, "AUTO :  %s    ","OFF");	 //液晶屏显示关闭自动报警函数
						//OLED_DisStrLine(1, 0, (uint8_t *)autobuf);

						if(*p & L3_0 == SPEAKER_OFF)			  //判断低4位是否为关闭蜂鸣器
						{
						   	Speaker_OnOffsw(1);	  
						}
                        
					automatic_flag = 0 ;				// 自动报警标识位置0
					}
					switch (*p & H5_4)				   //判断4、5位的设备类型
					{
						case FAN:
						{
							switch (*p & L3_0)			//判断低4位设备参数
							{
								case FAN_OFF:				   //关闭风扇
								{
								OLED_DisStrLine(1, 0, "                      ");
								   OLED_DisStrLine(1, 0, "FAN_OFF");
								   
								   disable_timer32(0);			//Disable timer
								   GPIOSetDir(PORT0, 2, 1);		//设置GPIO0_2为输出模式
								   GPIOSetValue(PORT0, 2, 1);	//赋0关闭风扇
								   pwm = 10;				    //pwm为外部引用，给time32函数传参，从而控制timer32_0_counter改变转数
								   break;
								}
								case FAN_ON1:				//风扇一档
								{
								OLED_DisStrLine(1, 0, "                      ");
								OLED_DisStrLine(1, 0, "FAN_ON1");
								   enable_timer32(0);	   //Enable timer
								   pwm = 500;
								   break;
								}
								case FAN_ON2:				 //风扇二档
								{
								OLED_DisStrLine(1, 0, "                      ");
								OLED_DisStrLine(1, 0, "FAN_ON2");
								   enable_timer32(0);		
								   pwm = 250;
								   break;
								}
								case FAN_ON3:				//风扇三档
								{
								OLED_DisStrLine(1, 0, "                      ");
								OLED_DisStrLine(1, 0, "FAN_ON3");
								   enable_timer32(0);
								   pwm = 100;
								   break;
								}
								default :
									break;
							} 
							break;
						} 
						case SPEAKER:
						{
							switch (*p & L3_0)
							{
								case SPEAKER_OFF:
								{
								OLED_DisStrLine(1, 0, "                      ");
								OLED_DisStrLine(1, 0, "SPEAKER_OFF");
								 beep_mask = 'G';
								beep_flag  = 0;
								//	Speaker_OnOffsw(1);	  //调用Speaker_OnOffsw函数赋1关闭蜂鸣器
									break;
								}		  
								case SPEAKER_ON:
								{
								OLED_DisStrLine(1, 0, "                      ");
								OLED_DisStrLine(1, 0, "SPEAKER_ON");									
								beep_mask = 'K';
								beep_flag = 1;
							
									break;
								}
								case SPEAKER_START_ON:
							 	{
									automatic_flag=1;	  //仅改变蜂鸣器状态
									break;
								}
							 	case SPEAKER_START_OFF:
							 	{
									beep_mask = 'G';
									break;
								}
								default:
									break;
							}  
							break;
						} 
						case LED :
						{
							switch (*p & L3_0)
							{
								case LED_ON: 
								{
								OLED_DisStrLine(1, 0, "                      ");	
								OLED_DisStrLine(1, 0, "LED_ON");
									GPIOSetValue(PORT3, 0, 0);	   //打开LED1 
									break;
								}
								case LED_OFF:
								{
								OLED_DisStrLine(1, 0, "                      ");
								OLED_DisStrLine(1, 0, "LED_OFF");
									GPIOSetValue(PORT3, 0, 1);	   //关闭LED1
									break;
								}
								default :
									break;
							}
							break;
						}
						case LED7:
						{
							switch ((*p & L3_0)==L3_0)
							{
								case LED7_ON: 
								{
									Seg7Led_Put(*p & L3_0);
									break;
								}
								case LED7_OFF:
							 	{
									Seg7Led_Put(' ');
									break;
								}
								default:
									break;
							}
							break;
						} 
						default:
							break;
					}

					if(pwm <= 50 )
					{
					snprintf(buf, 16, "FAN :  %s    ","OFF");
					//	OLED_DisStrLine(1, 0, (uint8_t *)buf);
						disable_timer32(0); 
			
					}
					else
					{
						snprintf(buf, 16, "FAN :  %d    ",pwm);
					//	OLED_DisStrLine(1, 0, (uint8_t *)buf);
					}
				}
			} 		
			GPIOIntEnable(PORT3,3);			//重新使能ZigBee中断

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
    char sendbuf[24];
	
	time_counter1 = 0;
    init();
	beep();
	Logo();
 

    three_handshake();
    beep();
    OLED_ClearScreen(); 
	while(1)
    { 
        alarm_and_display(sendbuf);
       	Rfid_answer();				  //读卡	
        receive_command();
    }
}

