/*
  GPIOSetValue(PORT1, 2, 1);// use P1.2 as CLK; OUTPUT	
	GPIOSetDir(PORT1, 2, 1);			// use P1.2 as CLK; OUTPUT		
	GPIOSetDir(PORT2, 6, 1);			// use p2.6 as data;Set PIO2_6 to output
	GPIOSetValue(PORT2, 6, 1);
*/
#include <stdio.h>
#include "lpc11xx.h"
#include "clkconfig.h"
#include "gpio.h"

void Delay5us()//7us
{
	uint32_t turn_flag;
 		turn_flag = SysTick->VAL;
	
		if((turn_flag >360) )//7us 高电平；使能信号
		{ 	
		  while(SysTick->VAL  > (turn_flag-360));	 
		}else
		{ 	
			while((SysTick->VAL > (48000+turn_flag-360))|(SysTick->VAL <turn_flag)); 
    }
}


void iic_start(void)
{
	/*set as output*/
	GPIOSetDir(PORT2, 6, 1);

	GPIOSetValue(PORT2, 6, 1);
	GPIOSetValue(PORT1, 2, 1);
  Delay5us();
 //SDA=0;
  GPIOSetValue(PORT2, 6, 0);
  Delay5us();
 //SCL=0;
	GPIOSetValue(PORT1, 2, 0);
}

void iic_stop(void)
{
		/*set as output*/
	GPIOSetDir(PORT2, 6, 1);
	
	GPIOSetValue(PORT2, 6, 0);
	GPIOSetValue(PORT1, 2, 1);
	Delay5us();
	GPIOSetValue(PORT2, 6, 1);
	Delay5us();
	
}

void iic_ack(uint8_t value)
{
		/*set as output*/
	GPIOSetDir(PORT2, 6, 1);
	if(value)
	 {GPIOSetValue(PORT2, 6, 1);}
   else
	 {GPIOSetValue(PORT2, 6, 0);}
  //SDA=0
	
  //SCL=1;
	GPIOSetValue(PORT1, 2, 1);
  Delay5us();
  //SCL=0;
	GPIOSetValue(PORT1, 2, 0);
  Delay5us();
  //SDA=1;
//	GPIOSetValue(PORT2, 6, 1);
}

/*ack after recive data */
uint32_t iic_testack(void)//等待应答
{
	
 uint32_t ErrorBit=0;
 GPIOSetDir(PORT2, 6, 1);

 GPIOSetValue(PORT1, 2, 1);
 GPIOSetDir(PORT2, 6, 0);
 Delay5us();
 ErrorBit=GPIOGetValue(PORT2, 6);
	while(GPIOGetValue(PORT2, 6)) //draw to low by slave
	{  Delay5us();
		if(GPIOGetValue(PORT2, 6))
		{	Delay5us();
		break;}
  }
 //SCL=0;
 GPIOSetValue(PORT1, 2, 0);
 Delay5us();
 return ErrorBit;
}

/*send one byte to  slave part;*/
void iic_write8bit(unsigned char input)
{
  unsigned char temp;
	unsigned char send=0;
	GPIOSetDir(PORT2, 6, 1);
	
 for(temp=8;temp!=0;temp--)
 {
  send=(input&0x80);
	 if(send)
		 	GPIOSetValue(PORT2, 6, 1);
		 else
			GPIOSetValue(PORT2, 6, 0);
		 
 // Delayus(40);
//SCL=1;	
 GPIOSetValue(PORT1, 2, 1);
 Delay5us();
 // SCL=0;	
 GPIOSetValue(PORT1, 2, 0);
 Delay5us();
  input=input<<1;
 }
 iic_testack();
}


/*read one byte from slave part;*/
uint8_t iic_read8bit(void)
{
  uint8_t temp,rbyte=0;
	
	GPIOSetDir(PORT2, 6, 1);
	GPIOSetValue(PORT2, 6, 1);
	
	GPIOSetDir(PORT2, 6, 0);
	
 for(temp=8;temp!=0;temp--)
 {
 // SCL=1;	
	GPIOSetValue(PORT1, 2, 1);
  Delay5us();
	 
  rbyte=rbyte<<1;
	 
	if(GPIOGetValue(PORT2, 6))
	rbyte |=0x01;	
	
	GPIOSetValue(PORT1, 2, 0);
	Delay5us();
 }
 return rbyte; 
}
