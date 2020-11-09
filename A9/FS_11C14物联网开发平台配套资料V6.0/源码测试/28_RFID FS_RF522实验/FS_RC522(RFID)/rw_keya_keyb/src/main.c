/******************** (C) COPYRIGHT farsight************
* File Name          : main.c
* Author             : zxq
* Date First Issued  : 03/18/2012
* Description        : 
*/
#include <stdio.h>

#include "LPC11xx.h"
#include "uart.h"
#include "gpio.h"
#include "rc522.h"
#include "ssp.h"
volatile unsigned char command[50];
unsigned char a = 0;
unsigned char global=0;
extern volatile uint8_t UARTBuffer[BUFSIZE];
unsigned char  	defkey[16]= {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x07,0x80,0x69,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char  	modkey[16]= {0x12,0x34,0x56,0x78,0x9a,0xbc,0x08,0x77,0x8f,0x69,0xbc,0x9a,0x78,0x56,0x34,0x12};

unsigned char 	keya[6]={0x12,0x34,0x56,0x78,0x9a,0xbc} ;
unsigned char 	keyb[6]={0xbc,0x9a,0x78,0x56,0x34,0x12};

unsigned char DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
/*
#define  LED_GREEN_OFF	GPIOSetValue( PORT3, 4, 1)
#define  LED_GREEN_ON	GPIOSetValue( PORT3, 4, 0)
#define  LED3_OFF	    GPIOSetValue( PORT3, 5, 1)
#define  LED3_ON	    GPIOSetValue( PORT3, 5, 0)
*/
//#define  MOD     	1
#define  ADDR     	39	   //第9个区，36,37,38,39；
#define  DATADDR    38

int main (void) 
{

  unsigned char status = 0;
  unsigned char g_ucTempbuf[30]; 

  SystemInit();

  UARTInit(115200);
  SSP_IOConfig( 0 );
  SSP_Init( 0 );
  printf("start check RFID card !!!!!!!!!\n\r");
  GpioRc522();
/*
#define  LED_GREEN_OFF	
#define  LED_GREEN_ON
#define  LED3_OFF	    
#define  LED3_ON	    
*/    
 //初始化射频芯片
  	PcdReset();
	PcdAntennaOff();	//关闭天线
	PcdAntennaOn();		//打开天线
while(1)
 {	  
 
     	 //寻卡
         status = PcdRequest(PICC_REQALL, g_ucTempbuf);
         if (status != MI_OK)//是否有错误
         {    
		      LED_GREEN_OFF;//LED2灭             
			  continue;           //推出本次循环
         }      
	 	//防冲撞处理
         status = PcdAnticoll(g_ucTempbuf);//执行成功g_ucTempbuf[]得到卡的序列号
         if (status != MI_OK)
         {    continue;    }
 
         status = PcdSelect(g_ucTempbuf);	        //选择卡片
         if (status != MI_OK)
         {    continue;    }
      
		  //验证卡片密码	
#ifndef   MOD
         status = PcdAuthState(PICC_AUTHENT1A, ADDR, DefaultKey, g_ucTempbuf);	//(ADDR + 1)/4 = int；
#else
		 status = PcdAuthState(PICC_AUTHENT1A, ADDR, keya, g_ucTempbuf);	    //(ADDR + 1)/4 = int； 
#endif    
         if (status != MI_OK)
          {   
		  	continue; 
		  }
		 LED3_ON;
	
	//	 status = PcdWrite(ADDR, defkey);
         //向卡里任意扇区的第三个block （密钥A 控制位 密钥B）写入数据，更改keya control keyb 的值；
#ifndef	 	MOD
		status = PcdRead( ADDR, g_ucTempbuf);
	
         if (status != MI_OK)
         {   
		 //	UARTSend("read data failed !! ",20);          
            continue; 
         }
		 else
         {
		   	LED_GREEN_ON;
		 	UARTSend(g_ucTempbuf,16);
		 }	
		
         status = PcdWrite(ADDR, defkey);//写迷钥
         if (status != MI_OK)
         {    
		 	continue;    
		 } 
#endif
        /*status = PcdWrite(DATADDR, defkey);
         if (status != MI_OK)
         {    
		 	UARTSend("write data failed !! ",21);
		 	continue;    
		   } */
		   
         //读地址的数据
      status = PcdRead( ADDR, g_ucTempbuf);
         if (status != MI_OK)
         {   
		 //	UARTSend("read data failed !! ",20);          
            continue; 
         }
		 else
         {
		   	LED_GREEN_ON;
		 	UARTSend(g_ucTempbuf,16);
		 }
         
		 PcdHalt();		//		进入休眠
		  
		}  
}
/************* (C) COPYRIGHT farsight*****END OF FILE****/
