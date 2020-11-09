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
#define  ADDR     	39	   //��9������36,37,38,39��
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
 //��ʼ����ƵоƬ
  	PcdReset();
	PcdAntennaOff();	//�ر�����
	PcdAntennaOn();		//������
while(1)
 {	  
 
     	 //Ѱ��
         status = PcdRequest(PICC_REQALL, g_ucTempbuf);
         if (status != MI_OK)//�Ƿ��д���
         {    
		      LED_GREEN_OFF;//LED2��             
			  continue;           //�Ƴ�����ѭ��
         }      
	 	//����ײ����
         status = PcdAnticoll(g_ucTempbuf);//ִ�гɹ�g_ucTempbuf[]�õ��������к�
         if (status != MI_OK)
         {    continue;    }
 
         status = PcdSelect(g_ucTempbuf);	        //ѡ��Ƭ
         if (status != MI_OK)
         {    continue;    }
      
		  //��֤��Ƭ����	
#ifndef   MOD
         status = PcdAuthState(PICC_AUTHENT1A, ADDR, DefaultKey, g_ucTempbuf);	//(ADDR + 1)/4 = int��
#else
		 status = PcdAuthState(PICC_AUTHENT1A, ADDR, keya, g_ucTempbuf);	    //(ADDR + 1)/4 = int�� 
#endif    
         if (status != MI_OK)
          {   
		  	continue; 
		  }
		 LED3_ON;
	
	//	 status = PcdWrite(ADDR, defkey);
         //�������������ĵ�����block ����ԿA ����λ ��ԿB��д�����ݣ�����keya control keyb ��ֵ��
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
		
         status = PcdWrite(ADDR, defkey);//д��Կ
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
		   
         //����ַ������
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
         
		 PcdHalt();		//		��������
		  
		}  
}
/************* (C) COPYRIGHT farsight*****END OF FILE****/
