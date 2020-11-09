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
unsigned char count1=0;
unsigned char count2=0;
extern volatile uint32_t UARTCount;
extern volatile uint8_t UARTBuffer[BUFSIZE];
unsigned char  	modkey1[16]= {0x12,0x34,0x56,0x78,0x9a,0xbc,0x7f,0x07,0x88,0x69,0xbc,0x9a,0x78,0x56,0x34,0x12};
unsigned char  	modkey2[16]= {0x12,0x34,0x56,0x78,0x9a,0xbc,0x6f,0x06,0x99,0x69,0xbc,0x9a,0x78,0x56,0x34,0x12};
unsigned char  	data1[16]= {0xff,0xff,0xff,0xff,0xED,0xff,0xff,0x87,0xff,0xff,0x56,0xff,0x01,0xFE,0xff,0xFE};
unsigned char 	keya[6]={0x12,0x34,0x56,0x78,0x9a,0xbc} ;
unsigned char 	keyb[6]={0xbc,0x9a,0x78,0x56,0x34,0x12};
//M1卡的某一块写为如下格式，则该块为钱包，可接收扣款和充值命令
//4字节金额（低字节在前）＋4字节金额取反＋4字节金额＋1字节块地址＋1字节块地址取反＋1字节块地址＋1字节块地址取反 


unsigned char data2[4]      = {0x01,0,0,0};
unsigned char DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
unsigned char databuf[30];
unsigned char r_buf[32];
/*
#define  LED_GREEN_OFF	GPIOSetValue( PORT3, 4, 1)
#define  LED_GREEN_ON	GPIOSetValue( PORT3, 4, 0)
#define  LED3_OFF	    GPIOSetValue( PORT3, 5, 1)
#define  LED3_ON	    GPIOSetValue( PORT3, 5, 0)
*/


void delay(void)
{
	unsigned int temp ;
	for(temp = 500; temp !=0; temp--);


}

 /*******************************************************************************
* Function Name  : RFID_CheckSum
* Description    : Init RFID control.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/	 
unsigned char RFID_CheckSum(unsigned char *databuf)
{ 
  unsigned char	numb=0, chksum =0;
	for(numb=0;numb<databuf[0];numb++)
    {	
		chksum += databuf[numb];
	}

  return chksum;
}  
   

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

   	LED_GREEN_OFF;
	LED3_OFF;
 
  	PcdReset();		    //初始化射频芯片
	PcdAntennaOff();	//关闭天线
	PcdAntennaOn();		//打开天线
while(1)
 {
     	 //寻卡
         status = PcdRequest(PICC_REQALL, g_ucTempbuf);
         if (status != MI_OK)//是否有错误
         {               
			  continue;           //退出本次循环
         }      
	 	//防冲撞处理
         status = PcdAnticoll(g_ucTempbuf);//执行成功g_ucTempbuf[]得到卡的序列号
         if (status != MI_OK)
         {    continue;    }
         //选择卡片
         status = PcdSelect(g_ucTempbuf);
         if (status != MI_OK)
         {    continue;    }
         //验证卡片密码
       status = PcdAuthState(PICC_AUTHENT1A, 1, DefaultKey, g_ucTempbuf);	//地址满足  (地址+1)/4  != int
         if (status != MI_OK)
         {   printf("password is wrong  !!!!!!!!!\n\r"); continue;    }
		  LED3_ON;
         //向卡写入数据
         status = PcdWrite(1, modkey1);		  //地址满足  (地址+1)/4  != int
         if (status != MI_OK)
         {    
		  	LED3_OFF;
		 	continue;    
		 }   		 
         //读地址的数据
      	status = PcdRead(1, g_ucTempbuf);
         if (status != MI_OK)
         {  
		   	LED_GREEN_OFF;
			LED3_OFF;
            continue; 
         }
		 else
         {
			LED_GREEN_ON;
			//uart打印读到的数据；
		 	UARTSend(g_ucTempbuf,16); 
		 }
		 //命令卡片进入休眠状态
		 PcdHalt();
		LED_GREEN_OFF;
		LED3_OFF;
		}  
}


/************* (C) COPYRIGHT farsight*****END OF FILE****/
