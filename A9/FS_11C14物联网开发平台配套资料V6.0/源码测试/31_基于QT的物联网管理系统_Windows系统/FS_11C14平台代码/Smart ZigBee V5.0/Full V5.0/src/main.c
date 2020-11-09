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
#include "lpc11xx.h"
#include "clkconfig.h"
#include "gpio.h"
#include "uart.h"
#include "timer32.h"
#include "ssp.h"
#include "i2c.h"
#include "wdt.h"
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
#include "collect_data.h"
#include "led_spk_fan.h"
#include "myrfid.h"
#include "menu.h"
#include "data_type.h"
#include "file_operation.h"
#include "sys_init.h"


uint8_t 		STORAGE_NUM = 0;  			  //设备号（发送开机命令后由上层节点返回）
message_tag_t   message_tag_s;
tem_t 			tem_s;
hum_t 			hum_s;
light_t 		light_s;
acc_t 			acc_s;
adc_t       	adc_s;
state_t 		state_s;
rfid_t 			rfid_s;
command_t 		command_s;
key_t 			key_s;
env_msg_t 		env_msg_s;
data_t			data_s;
message_t 		message_s;
message_t  		message_r;
 	  	
volatile uint32_t counter1 = 0;
volatile uint32_t counter2 = 0;
static uint8_t		   cnt = 0;

extern char zigbee_flag;
extern char rfid_flag;
extern char up_flag;
extern char down_flag;
extern char left_flag;
extern char right_flag;
extern char esc_flag;
extern char sel_flag;
extern uint8_t numb;
extern uint8_t	SPI752_rbuf_1[];

uint8_t  k = 0;
uint8_t  j = 0;
uint8_t	 rx[48] = {0};
uint8_t  rbuf[48] = {0};
uint8_t  *p;
uint32_t rfid_id;
char 	 dis_buf[30] = {0};



const unsigned char  aucCRCHi[] = {

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 

    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40

};

const unsigned char aucCRCLo[] = {

    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,

0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,

    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,

    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,

    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,

    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,

    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,

    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 

    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,

    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,

    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,

    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,

    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 

    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,

    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,

    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,

    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,

    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,

    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,

    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,

    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,

    0x41, 0x81, 0x80, 0x40
};

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
  
  Tick ++;
  if(Tick == 500)
  {
    GPIOSetValue(PORT3, 1, 0);		// PIO3_1 output 1, turn on LED2
//	Seg7Led_Put(cnt);
	cnt ++;
	cnt &= 0x1f;
  }
  else if(Tick >= 1000)
  {
  	counter1++;
	counter2++;
    GPIOSetValue(PORT3, 1, 1);		// PIO3_1 output 1, turn off LED2
	Tick = 0;
  }
}

/****************************************************************************
* Function Name : CRC
*****************************************************************************/
/*
ushort usMBCRC16( uint8_t * pucFrame,uint8_t usLen )

{
    uint8_t ucCRCHi = 0x0;
    uint8_t ucCRCLo = 0x0;
    
	int iIndex;

    while( usLen-- )
    {

		iIndex = ucCRCLo ^ *( pucFrame++ );

        ucCRCLo = (uint8_t)( ucCRCHi ^ aucCRCHi[iIndex] );

        ucCRCHi = aucCRCLo[iIndex];

    }
    
	return (ushort)( ucCRCHi << 8 | ucCRCLo );
}
*/

/********************************************************************************
*Function Name : collect the command
*********************************************************************************/



/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/


int main(void)
{
	p = rx;

	init();
	fill_message(&message_s,start_machine,0,message_len,NULL,0);
	send_message_zigbee(&message_s);
	 
	while( 1 )
	{	  
	
		if(zigbee_flag == 0)
		{
			memset(&message_s,message_len,0);
			fill_message(&message_s,start_machine,0,message_len,NULL,0);
			send_message_zigbee(&message_s);
			delay_ms(2000);
			continue;
		}
		else
			break;
	}
		
		//zigbee_flag = 0;
	    //GPIOIntEnable(PORT3,3);
/*******************************************************************************************
*	method:			 threeway handshaking 												   *
*	the fiset handshake:   slave machine send a request to register a id.If master 		   *
*						   don't give an anwser,slave keep sending request.				   *
*	the second handshake:  slave wait for the answer from master.						   *
*																						   *
*	the third handshake:   slave send the id get from master,and send to master again.	   *
*                          In the data field,the first byte is filled with STORAGE_NUM,	   *
*         				   used to confirm the message.Then wait for the answer from master*
*******************************************************************************************/					
		while(1)	
		{
			if(k < 36) 
			{
				zigbee_flag = 0;
				ZigBee_GetChar(p);
				p++;
				k++;												
			}
			else
				break;
		}				
		k = 0;
		p = rx;

		if(p[0] == start_machine)
		{
			STORAGE_NUM = p[1];
		}

		memset(&message_r,36,0);
		memcpy(&message_r,rx,36);

		memset(&message_s,message_len,0);
		p = (uint8_t *)(&message_s);
		p[3] = STORAGE_NUM;
		fill_message(&message_s,start_machine,STORAGE_NUM,message_len,NULL,0);
		send_message_zigbee(&message_s);

		while(1)
		{
			if(zigbee_flag == 0)
				continue;
		
	    /*
		while(1)	
		{
			if(k < 36) 
			{
				zigbee_flag = 0;
				ZigBee_GetChar(p);
				p++;
				k++;											
			}
			else
				break;
		}
						
		k = 0;
		p = rx;
		printf("\r\n--STORAGE:%x",STORAGE_NUM);
		printf("\r\n--p[3]:%x",p[3]);
		*/
			zigbee_flag = 0;
			printf("\r\n---------SPI752_rbuf_1[0]:%x",SPI752_rbuf_1[0]);
			printf("\r\n---------SPI752_rbuf_1[1]:%x",SPI752_rbuf_1[1]);
			printf("\r\n---------SPI752_rbuf_1[2]:%x",SPI752_rbuf_1[2]);
			printf("\r\n---------SPI752_rbuf_1[3]:%x",SPI752_rbuf_1[3]);
			printf("\r\n--------STORAGE_NUM:%x",STORAGE_NUM);

			if((SPI752_rbuf_1[0] == start_machine) && (SPI752_rbuf_1[3] == STORAGE_NUM))
				break;
			else
				 continue;
	}
/***************************************************************************************
*	section:  sending message(the environment message or the rfid message)			   *
*			  receive message from the master and give matching method.				   *
*   dwescription:																	   *
*			  designed to send message to master every two seconds.Time is controlled*
*			  by the SysTick().														   *
*			  receive message is triggered by the interrupt.According to the treaty,   *
*			  slave operates correctly.												   *
***************************************************************************************/		
    while(1)
    {
					
		if(counter1 > 2)
	 	{
			memset(&message_s,message_len,0);
			collect_data(&tem_s,&hum_s,&light_s);
			adc_ret(&adc_s);
	    	acc_ret(&acc_s);	
			get_state(&state_s);
			fill_env(&env_msg_s,tem_s,hum_s,acc_s,adc_s,light_s,state_s);	 
			fill_data(&data_s,data_e,NULL,NULL,&env_msg_s,NULL); 
			fill_message(&message_s,data_flow,STORAGE_NUM,message_len,&data_s,0);
			send_message_zigbee(&message_s); 	
			counter1 = 0;
		}
	 	
		if(counter2 > 3)
		{
			WDTFeed();
			counter2 = 0;
		}	
		memcpy(&message_r,SPI752_rbuf_1,36);
		//if ((message_r.tag == command) && ((message_r.slave_address == STORAGE_NUM) || (message_r.slave_address == BROADCAST)))
		if ((message_r.tag == command) && (message_r.slave_address == STORAGE_NUM))
		{
				switch (message_r.data.command.operate_id)
				{
					case on_led1:
  			       	 	GPIOSetValue(PORT3, 0, 0);		 //turn on led1
					break;
					case off_led1:
  			        	GPIOSetValue(PORT3, 0, 1);		//turn off led1
					break;
					case on_fan:
						GPIOSetValue(PORT0, 2, 0);
					break;
					case off_fan:
						GPIOSetValue(PORT0, 2, 1);
					break;
					case on_seven_led:
						Seg7Led_Put(cnt);
					break;
					case off_seven_led:
						Seg7Led_Put(' ');
					break;
					case on_speaker:
				 		speaker_op(1);
					break;
					case off_speaker:
					 	speaker_op(0);
					break;
					
					case off_machine:
						GPIOSetValue(PORT3, 0, 1);
						GPIOSetValue(PORT0, 2, 1);
						Seg7Led_Put(' ');
						speaker_op(0);
						OLED_ClearScreen();
						snprintf(dis_buf, 16, "I'm died");
						OLED_DisStrLine(1, 0, (uint8_t *)dis_buf);
						SysTick->CTRL  = ~(SysTick_CTRL_CLKSOURCE_Msk | 
                  						  SysTick_CTRL_TICKINT_Msk   | 
                 						  SysTick_CTRL_ENABLE_Msk);
						for(;;){;}
						
					break;
												
				 	default:
				 		printf("not write command");
				 	break;
		 		
				}
		}
/*************************************************************************************
*	when the master send broadcast message,the slave will stop working and display	 *
*   "I'm died"																	     *
*************************************************************************************/
		else if((message_r.tag == command) && (message_r.slave_address == BROADCAST))
		{
			if(message_r.data.command.operate_id == off_machine)
			{
				GPIOSetValue(PORT3, 0, 1);
				GPIOSetValue(PORT0, 2, 1);
				Seg7Led_Put(' ');
				speaker_op(0);
				OLED_ClearScreen();
				snprintf(dis_buf, 16, "I'm died");
				OLED_DisStrLine(1, 0, (uint8_t *)dis_buf);
				SysTick->CTRL  = ~(SysTick_CTRL_CLKSOURCE_Msk | 
                  				SysTick_CTRL_TICKINT_Msk   | 
                 				SysTick_CTRL_ENABLE_Msk);
				for(;;){;}	
			}
		}
/***************************************************************************************
*	The system designed that the slave can control the master.						   *
*	Through pressing different buttons,different methods of master will be operated.   *
***************************************************************************************/	  	
		if(up_flag)
		{
			up_flag = 0;
			fill_key(&key_s,key_up);
			fill_data(&data_s,data_k,NULL,NULL,NULL,&key_s);
			fill_message(&message_s,key,STORAGE_NUM,message_len,&data_s,0);
			send_message_zigbee(&message_s);
		}

		if(down_flag)
		{
			down_flag = 0;
			fill_key(&key_s,key_down);
			fill_data(&data_s,data_k,NULL,NULL,NULL,&key_s);
			fill_message(&message_s,key,STORAGE_NUM,message_len,&data_s,0);	
			send_message_zigbee(&message_s);
		}

		if(left_flag)
		{
			left_flag = 0;
			fill_key(&key_s,key_left);
			fill_data(&data_s,data_k,NULL,NULL,NULL,&key_s);
			fill_message(&message_s,key,STORAGE_NUM,message_len,&data_s,0);
			send_message_zigbee(&message_s);		
		}

		if(right_flag)
		{
		 	right_flag = 0;
			fill_key(&key_s,key_right);
			fill_data(&data_s,data_k,NULL,NULL,NULL,&key_s);
			fill_message(&message_s,key,STORAGE_NUM,message_len,&data_s,0);	
			send_message_zigbee(&message_s);
		}

		if(sel_flag)
		{
			sel_flag = 0;
			fill_key(&key_s,key_sel);
			fill_data(&data_s,data_k,NULL,NULL,NULL,&key_s);
			fill_message(&message_s,key,STORAGE_NUM,message_len,&data_s,0);	
			send_message_zigbee(&message_s);
		}

		if(esc_flag)
		{
			esc_flag = 0;
			fill_key(&key_s,key_esc);
			fill_data(&data_s,data_k,NULL,NULL,NULL,&key_s);
			fill_message(&message_s,key,STORAGE_NUM,message_len,&data_s,0);	
			send_message_zigbee(&message_s);
		}
/****************************************************************************************
*	When there is a card next to the RFID Module,FS_11C14 will send the message of card *
*   to master.																			*
*****************************************************************************************/

		if(rfid_flag)
		{
			rfid_flag = 0;
			if(Rfid_Operation(STORAGE_NUM,rbuf))
			{  
			   //Speaker_OnOff(0);
			   memset(&message_s,message_len,0);
			   memcpy(&rfid_id,rbuf,sizeof(rbuf));
			   fill_rfid(&rfid_s,rfid_id,NULL,NULL,NULL);
			   fill_data(&data_s,data_r,&rfid_s,NULL,NULL,NULL);
			   fill_message(&message_s,rfid_msg,STORAGE_NUM,message_len,&data_s,0);
			   send_message_zigbee(&message_s);	
			} 
			GPIOIntEnable(PORT2,8);
		}

		  
   		 __WFI(); 	 
  
     }
 
}

/************************************* End of File *****************************************/
