/*******************************************************************************
* File:    led_spk_fan_adc.c 
* Author:  Farsight Design Team
* Version: V1.00
* Date:    2011.06.21
* Brief:   LED & Speaker & FAN & ADC program body
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
#include "timer32.h"
#include "adc.h"

#include "oled2864.h" 
#include "key.h"
#include "tools.h"
#include "menu.h"


extern volatile uint32_t timer32_0_counter;
volatile uint32_t period = 1500;
#define	SlaveAddress   0x3C	  //����������IIC�����еĴӵ�ַ
unsigned char BUF[8]={0};
extern unsigned long frequency;
extern void iic_start(void);
extern void iic_stop(void);
extern void iic_ack(uint8_t value);
extern void iic_NoAck(void);
extern uint32_t iic_testack(void);
extern void iic_write8bit(unsigned char input);
extern unsigned char iic_read8bit(void);
extern void Single_Write_HMC5883(unsigned char REG_Address,unsigned char REG_data);
extern void Init_HMC5883(void);
extern void search(unsigned char  mode);
extern void radio_write(void);

extern void auto_search(unsigned char  mode);  
                     
extern volatile uint32_t p3_2_counter;
extern volatile uint32_t p3_2_tc;                  

extern uint32_t turn_flag;
extern uint32_t mo;


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* External global variables -------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External functions --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : LED_Test
* Description    : LED test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
volatile int i;

void GAS_Test(void)
{
  Menu_TypeDef	menu;
  uint16_t	key;
  char 		buf[25];
  uint32_t	adv;
  LPC_IOCON->R_PIO1_1 &= ~0x07;
  LPC_IOCON->R_PIO1_1 |= 0x01;		//set PIO1_1 gpio function mode
  GPIOSetDir(PORT1, 1, 1);			// 
  GPIOSetValue(PORT1, 1, 1);		// 
  GPIOSetDir(PORT3, 0, 1);			//
  GPIOSetDir(PORT2, 6, 0);			// Set PIO2_6 to ���������ź�����
	
  ADCInit(ADC_CLK);

  menu.max_numb = 2;
  menu.numb = 1;
  menu.title = "gas";
  menu.item[0] = "display";
  menu.item[1] = " ";
  Dis_Menu(menu);
  delay_ms(200);
  while(menu.numb)
  {
	adv = ADCRead(3);//luyj.201308.4
	adv = adv*3;//3.3/1023*1000
	delay_ms(1);
	snprintf(buf, 13, "ADC : %dmV  ", adv);
	OLED_DisStrLine(7, 0, (uint8_t *)buf);
		if(!GPIOGetValue(PORT2, 6))//CO�����źŲɼ�	 ���Ʒ�����
		 {
			 GPIOSetValue(PORT1, 1, 0);		// 
			 delay_ms(5);
			 GPIOSetValue(PORT1, 1, 1);		// 
			 delay_ms(5);
			 GPIOSetValue(PORT1, 1, 0);		// 
			 delay_ms(5);
			 GPIOSetValue(PORT1, 1, 1);		// 
			 delay_ms(5);
          }
			else
			{
				GPIOSetValue(PORT1, 1, 1);		// 
            }
		
		if(adv>2500)//COģ��ֵ�Ƚ�
			GPIOSetValue(PORT3, 0, 1);		// PIO1_1 output 1, Turn off 
   	else
			GPIOSetValue(PORT3, 0, 0);		// PIO1_1 output 1, Turn on 

    key = KEY_Read();

    switch(key)
	{ 
	  case KEY_ESC:	    	    
        menu.numb = 0;		
	    break;
		
	  default:
	    break; 
	}
   delay_ms(250);

  }
}



RELAY_Test(void)
{
   Menu_TypeDef	menu;
   uint16_t	key;
   menu.max_numb = 2;
   menu.numb = 1;
   menu.title = "relay ";
   menu.item[0] = "see led1"; 

    GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
    GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off LED1
    GPIOSetDir(PORT2, 6, 1);			//�̵����Ŀ�������
    Dis_Menu(menu);
  while(menu.numb)
  {		
		  for(i=0; i<1000000; i++);
    {	GPIOSetValue(PORT3, 0, 1);
    	GPIOSetValue(PORT2, 6, 0);}	//�̵��� �����Ͽ������ձպ�

	for(i=0; i<1000000; i++);
   {	GPIOSetValue(PORT3, 0, 0);
	 GPIOSetValue(PORT2, 6, 1);}

		
  key = KEY_Read();
    switch(key)
	 {  
	  case KEY_ESC:	    	    
        menu.numb = 0;		
	    break;
		
	  default:
	    break; 
	 }
 }
   	 //�̵��� �����պϣ����նϿ�
	 
}


IrDA_test(void)
{
  Menu_TypeDef	menu;
  uint16_t	key;
  menu.max_numb = 2;
  menu.numb = 1;
  menu.title = "IrDA_ ";
  menu.item[0] = "see led1";

   //���òɼ�Ϊ��ͨIO������ģʽ
  LPC_IOCON->R_PIO1_2 &= ~0x07;	
  LPC_IOCON->R_PIO1_2 |= 0x01;		/* CLK OUT */
  GPIOSetDir(PORT2, 6, 0);			// Set PIO2_6 to �����ź�����
  GPIOSetDir(PORT3, 0, 1);			//	
  Dis_Menu(menu);
  delay_ms(250);
  
  while(menu.numb)
  {
  if(GPIOGetValue(PORT2, 6))//��⵽�ⲿ�ƶ��ĺ����źţ�LED������
		 {
		    GPIOSetValue(PORT3, 0, 0);		// 
			 delay_ms(5);
			 GPIOSetValue(PORT3, 0, 1);		//
			 delay_ms(5);
			 GPIOSetValue(PORT3, 0, 0);		//
			 delay_ms(5);
			 GPIOSetValue(PORT3, 0, 1);		//
			 delay_ms(5);
          }
		  else
		  {
			  GPIOSetValue(PORT3, 0, 1);		// 	   �ر�LED
          }		
		   delay_ms(250);
  key = KEY_Read();
    switch(key)
	 {  
	  case KEY_ESC:	    	    
        menu.numb = 0;		
	    break;
	  default:
	    break; 
	  }
  }	
}



void DISTANCE_Test( void)
{

uint32_t	cnt_last =0;
uint32_t	tc1, tc;//��¼systemֵ
uint32_t  cnt_mo, cnt_mo2;
uint8_t nalve=0;//����ͳ�ƺ���
uint32_t time_use=0;
uint32_t distance=0;
uint8_t disbuf[20]={0};
char buf[20]={0};
 uint16_t	key;
 Menu_TypeDef	menu;
  menu.max_numb = 2;
  menu.numb = 1;
  menu.title = "distance";
  menu.item[0] = " 0 ";

	GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
    GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off LED1
	
	GPIOSetDir(PORT2, 6, 1);			// Set PIO2_6 to output
	GPIOSetValue(PORT2, 6, 0);		// 
	
	//���òɼ�Ϊ��ͨIO������ģʽ
	LPC_IOCON->R_PIO1_2 &= ~0x07;	
    LPC_IOCON->R_PIO1_2 |= 0x01;		/* CLK OUT */
	
	GPIOSetDir(PORT1, 2, 0);	
	GPIOIntDisable(PORT1, 2);

  Dis_Menu(menu);
  delay_ms(250);

  while(menu.numb)
  { 
  	    turn_flag = SysTick->VAL;
		GPIOSetValue(PORT2, 6, 1);
		if((turn_flag > 480))//10us �ߵ�ƽ��ʹ���ź�
		{ GPIOSetValue(PORT3, 0, 0);	
		  while(SysTick->VAL  > (turn_flag-480));
		  GPIOSetValue(PORT2, 6, 0);
			 
		}else
		{ GPIOSetValue(PORT3, 0, 0);	
		   while((SysTick->VAL > (48000+turn_flag-480))|(SysTick->VAL <turn_flag));
		  GPIOSetValue(PORT2, 6, 0);
			 
         }
		//��ȡ����ʱ��
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
		
		//ʱ��ͳ��
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
      distance=(uint32_t)( time_use*17/100);//�������
			
		  printf(" distance=%d\r\n", distance);
		  disbuf[0]= (uint8_t)(distance/1000);
			disbuf[1]=(uint8_t)(distance%1000/100);
			disbuf[2]=(uint8_t)(distance%100/10);
			disbuf[3]=(uint8_t)(distance%10);
	
			if(distance < 4500)
			snprintf(buf, 16, "dts: %d%d%d%d mm ", disbuf[0], disbuf[1],disbuf[2],disbuf[3]);
		  OLED_DisStrLine(3, 0, (uint8_t *)buf);

			delay_ms(500);
		
	 key = KEY_Read();
    switch(key)
	{  
	  case KEY_ESC:	    	    
        menu.numb = 0;		
	    break;
		
	  default:
	    break; 
	 }
  }
 }






 
void Tarck_Test(void)
{
	
Menu_TypeDef	menu;
  uint16_t	key;
  

  menu.max_numb = 2;
  menu.numb = 1;
  menu.title = "tarch ";
  menu.item[0] = "see led";
  //menu.item[1] = "2.Turn off LED";
	
 // GPIOSetDir(PORT2, 6, 1);			// Set PIO2_6 to output
	//GPIOSetValue(PORT2, 6, 1);		//  ���ϰ�����ʹ�ܶ�
	
	//���òɼ�Ϊ��ͨIO������ģʽ
	LPC_IOCON->R_PIO1_2 &= ~0x07;	
  LPC_IOCON->R_PIO1_2 |= 0x01;		/* CLK OUT */
	GPIOSetDir(PORT1, 2, 0);	
	GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
	
  Dis_Menu(menu);
  delay_ms(250);
  
  while(menu.numb)
  {
   
   if(GPIOGetValue(PORT1, 2))//���ں����ϣ�Ϊ�ߵ�ƽ���ڰ�ֽ���ϣ�Ϊ�͵�ƽ��
		{
		delay_ms(250);
		OLED_DisStrLine(3, 0, "on  line");
			
		}
   	else	
		{OLED_DisStrLine(3, 0, "off line");
		delay_ms(250);}
		
		
	 key = KEY_Read();
    switch(key)
	{  
	  case KEY_ESC:	    	    
        menu.numb = 0;		
	    break;
		
	  default:
	    break; 
	 }
  }
 }


void Touch_Test(void)
{
Menu_TypeDef	menu;
  uint16_t	key;
  

  menu.max_numb = 2;
  menu.numb = 1;
  menu.title = "touch ";
  menu.item[0] = "see led";
  //menu.item[1] = "2.Turn off LED";
	
 // GPIOSetDir(PORT2, 6, 1);			// Set PIO2_6 to output
	//GPIOSetValue(PORT2, 6, 1);		//  ���ϰ�����ʹ�ܶ�
	
	//���òɼ�Ϊ��ͨIO������ģʽ
	LPC_IOCON->R_PIO1_2 &= ~0x07;	
  LPC_IOCON->R_PIO1_2 |= 0x01;		/* CLK OUT */
	GPIOSetDir(PORT1, 2, 0);	
	GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
	LPC_IOCON->R_PIO1_1 &= ~0x07;
  LPC_IOCON->R_PIO1_1 |= 0x01;	
	GPIOSetDir(PORT1, 1, 1);			// 
  GPIOSetValue(PORT1, 1, 1);		// 
  Dis_Menu(menu);
  delay_ms(250);
  
  while(menu.numb)
  {
   
   if(GPIOGetValue(PORT1, 2))//��û���ϰ���Ϊ�ߵ�ƽ�����ϰ���Ϊ��
		{
	    GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off 
			 GPIOSetValue(PORT1, 1, 1);		//
		}
   	else	
		{ GPIOSetValue(PORT3, 0, 0);		// PIO3_0 output 1, Turn off 
   	  GPIOSetValue(PORT1, 1, 0);	}	// 
	 key = KEY_Read();
    switch(key)
	{  
	  case KEY_ESC:	    	    
        menu.numb = 0;		
	    break;
		
	  default:
	    break; 
	 }
  }
 }

void BALL_SWITCH_Test(void)
{
  Menu_TypeDef	menu;
  uint16_t	key;
  

  menu.max_numb = 2;
  menu.numb = 1;
  menu.title = "BALL_SWITCH";
  menu.item[0] = "see led1";
  //menu.item[1] = "2.Turn off LED";
	
 // GPIOSetDir(PORT2, 6, 1);			// Set PIO2_6 to output
	//GPIOSetValue(PORT2, 6, 1);		//  ���ϰ�����ʹ�ܶ�
	
	//���òɼ�Ϊ��ͨIO������ģʽ
	LPC_IOCON->R_PIO1_2 &= ~0x07;	
  LPC_IOCON->R_PIO1_2 |= 0x01;		/* CLK OUT */
	GPIOSetDir(PORT1, 2, 0);	
	GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
  Dis_Menu(menu);
  delay_ms(250);
  
  while(menu.numb)
  {
   
   if(GPIOGetValue(PORT1, 2))//��û���ϰ���Ϊ�ߵ�ƽ�����ϰ���Ϊ��
		{
	    GPIOSetValue(PORT3, 0, 0);		// PIO3_0 output 1, Turn off 
		}
   	else	
	    GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off 
  
	 key = KEY_Read();
    switch(key)
	{  
	  case KEY_ESC:	    	    
        menu.numb = 0;		
	    break;
		
	  default:
	    break; 
	 }
  }
 }

void AVIOD_Test(void)
{
  Menu_TypeDef	menu;
  uint16_t	key;
  

  menu.max_numb = 2;
  menu.numb = 1;
  menu.title = "aviod";
  //menu.item[0] = "1.Turn on LED";
  //menu.item[1] = "2.Turn off LED";
	
  GPIOSetDir(PORT2, 6, 1);			// Set PIO2_6 to output
	GPIOSetValue(PORT2, 6, 1);		//  ���ϰ�����ʹ�ܶ�
	
	//���òɼ�Ϊ��ͨIO������ģʽ
	LPC_IOCON->R_PIO1_2 &= ~0x07;	
  LPC_IOCON->R_PIO1_2 |= 0x01;		/* CLK OUT */
	GPIOSetDir(PORT1, 2, 0);	
  Dis_Menu(menu);

  delay_ms(250);
  
  while(menu.numb)
  {
   
   if(GPIOGetValue(PORT1, 2))//��û���ϰ���Ϊ�ߵ�ƽ�����ϰ���Ϊ��
		{
	//	delay_ms(250);
	OLED_DisStrLine(3, 0, "Leaving ");
		
		}
   	else	
		OLED_DisStrLine(3, 0, "Hitting ");
	//	delay_ms(250);
  
	 key = KEY_Read();
    switch(key)
	{  
	  case KEY_ESC:	    	    
        menu.numb = 0;		
	    break;
		
	  default:
	    break; 
	 }
  }
 }

 /*
 campass
 
 */
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
 
 void COMPASS_Test(void)
{
  Menu_TypeDef	menu;
  uint16_t	key;
	int16_t x=0,y=0;
  double angle=0;
	uint8_t disbuf[20]={0};
  char buf[20]={0};
  menu.max_numb = 2;
  menu.numb = 1;
  menu.title = "CAMPASS";
  menu.item[0] = " value";
  menu.item[1] = "  ";
  Dis_Menu(menu);
  delay_ms(200);

  LPC_IOCON->R_PIO1_2 &= ~0x01f;	
  LPC_IOCON->R_PIO1_2 |= 0x11;		/* CLK OUT */
	GPIOSetDir(PORT1, 2, 1);			// use P1.2 as CLK; OUTPUT	
	
	GPIOSetDir(PORT2, 6, 1);			// use p2.6 as data;Set PIO2_6 to output
 	GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
  GPIOSetValue(PORT3, 0, 0);		// PIO3_0 output 1, Turn off LED1
  Init_HMC5883();
	delay_ms(400);
 
  while(menu.numb)
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
 
		 
		 key = KEY_Read();
    switch(key)
	{ 
	  case KEY_ESC:	    	    
        menu.numb = 0;		
	    break;
		
	  default:
	    break; 
	}
   delay_ms(1000);

  }
	
}
 
 
 

 /*******************************************************************************
* Function Name  : HaLL_Test
* Description    : HaLL test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
 HALL_Test(void)
 {
	 Menu_TypeDef	menu;
   uint16_t	key;
	 //���òɼ�Ϊ��ͨIO������ģʽ
	LPC_IOCON->R_PIO1_2 &= ~0x07;	
  LPC_IOCON->R_PIO1_2 |= 0x01;		/* CLK OUT */
	GPIOSetDir(PORT1, 2, 0);			// 
 	GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
  GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off LED1
	
  menu.max_numb = 2;
  menu.numb = 1;
  menu.title = "HALL";
//  menu.item[0] = "1.Turn on Speaker";
 // menu.item[1] = "2.Turn off Speaker";

  Dis_Menu(menu);

  delay_ms(200);
  
  while(menu.numb)
  {

		
	if(GPIOGetValue(PORT1, 2))//
		{
		delay_ms(250);
		OLED_DisStrLine(3, 0, "no ");
		GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off LED1	
		}
   	else
		{
    GPIOSetValue(PORT3, 0, 0);		// PIO3_0 output 1, Turn off LED1			
		OLED_DisStrLine(3, 0, "ok  ");
	} 

	  key = KEY_Read();

    switch(key)
	{ 
	  case KEY_ESC:	    	    
        menu.numb = 0;		
	    break;
		
	  default:
	    break; 
	}
   delay_ms(250);

  }
	

}


/*******************************************************************************
* Function Name  : Speaker_Test
* Description    : Speaker test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLAME_Test(void)
{
  Menu_TypeDef	menu;
  uint16_t	key;
	char 		buf[25];
  uint32_t	adv;
  LPC_IOCON->R_PIO1_1 &= ~0x07;
  LPC_IOCON->R_PIO1_1 |= 0x01;		//set PIO1_1 gpio function mode
	GPIOSetDir(PORT1, 1, 1);			// 
  GPIOSetValue(PORT1, 1, 1);		// 
	GPIOSetDir(PORT3, 0, 1);			//
	GPIOSetDir(PORT2, 6, 0);			// Set PIO2_6 to ���������ź�����
	
  ADCInit(ADC_CLK);

  menu.max_numb = 2;
  menu.numb = 1;
  menu.title = "FLAME";
  menu.item[0] = "display";
  menu.item[1] = " ";

  Dis_Menu(menu);

  delay_ms(200);
  
  while(menu.numb)
  {
	adv = ADCRead(3);//luyj.201308.4
	adv = adv*3;//3.3/1023*1000
		delay_ms(1);
	snprintf(buf, 13, "ADC : %dmV  ", adv);
	OLED_DisStrLine(7, 0, (uint8_t *)buf);
		if(GPIOGetValue(PORT2, 6))//���������źŲɼ�
		 {
			 GPIOSetValue(PORT1, 1, 0);		// 
			 delay_ms(5);
			 //GPIOSetValue(PORT1, 1, 1);		// 
			 delay_ms(5);
     }
			else
			{
				GPIOSetValue(PORT1, 1, 1);		// 
      }
		
		if(adv>3000)//ģ��ֵ�Ƚ�
			GPIOSetValue(PORT3, 0, 1);		// PIO1_1 output 1, Turn off 
   	else
			GPIOSetValue(PORT3, 0, 0);		// PIO1_1 output 1, Turn on 
		
		
    key = KEY_Read();

    switch(key)
	{ 
	  case KEY_ESC:	    	    
        menu.numb = 0;		
	    break;
		
	  default:
	    break; 
	}
   delay_ms(250);

  }
}

/*******************************************************************************
* Function Name  : FAN_Test
* Description    : FAN test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

 

/**** End of File ****/
