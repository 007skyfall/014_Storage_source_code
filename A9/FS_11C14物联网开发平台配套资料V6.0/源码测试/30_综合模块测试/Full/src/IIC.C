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
#define	SlaveAddress   0x3C	  //定义器件在IIC总线中的从地址
#define max_freq 108000
#define min_freq 87500

unsigned char radio_write_data[5]={0x29,0xc2,0x20,0x11,0x00};        //要写入TEA5767的数据
unsigned char radio_read_data[5];        //TEA5767读出的状态
//unsigned int default_pll=0x301d;         //0x29f9;        //默认存台的pll,95.8MHz
unsigned long frequency;
uint16_t pll;
 

extern void delay_ms(uint32_t ms);
//extern unsigned char BUF[8];
//////////////////////////////////////////
void Delayus(unsigned int number)
{
 while(number--);
}
void iic__start(void)
{
	/*set as output*/
	
	GPIOSetDir(PORT2, 6, 1);
 //SDA=1;
	GPIOSetValue(PORT2, 6, 1);
 Delayus(40);
//SCL=1
	GPIOSetValue(PORT1, 2, 1);
 Delayus(40);
 //SDA=0;
 GPIOSetValue(PORT2, 6, 0);
 Delayus(40);
 //SCL=0;
	 GPIOSetValue(PORT1, 2, 0);
 Delayus(40);
}

void iic__stop(void)
{
		/*set as output*/

	GPIOSetDir(PORT2, 6, 1);
// SCL=0;
	GPIOSetValue(PORT1, 2, 0);
 Delayus(40);
// SDA=0;
	GPIOSetValue(PORT2, 6, 0);
 Delayus(40);
 //SCL=1;
	GPIOSetValue(PORT1, 2, 1);
 Delayus(40);
 //SDA=1
	GPIOSetValue(PORT2, 6, 1);
 Delayus(40);
}

void iic__ack(void)
{
		/*set as output*/
	
	GPIOSetDir(PORT2, 6, 1);
  //SDA=0
	GPIOSetValue(PORT2, 6, 0);
  Delayus(40);
  //SCL=1;
	GPIOSetValue(PORT1, 2, 1);
  Delayus(40);
  //SCL=0;
	GPIOSetValue(PORT1, 2, 0);
  Delayus(40);
  //SDA=1;
	GPIOSetValue(PORT2, 6, 1);
  Delayus(40);
}
void iic__NoAck(void)
{
	/*set as output*/
	
	GPIOSetDir(PORT2, 6, 1);
	
 //SDA=1;
	GPIOSetValue(PORT2, 6, 1);
 Delayus(40);
 //SCL=1;
	GPIOSetValue(PORT1, 2, 1);
 Delayus(40);
 //SCL=0;
	GPIOSetValue(PORT1, 2, 0);
 Delayus(40);
 //SDA=0;
	GPIOSetValue(PORT2, 6, 0);
}

uint32_t iic__testack(void)
{
	
 uint32_t ErrorBit;
 
 GPIOSetDir(PORT2, 6, 1);
	
 //SDA=1;	
 GPIOSetValue(PORT2, 6, 1);
 Delayus(40);
 //SCL=1;
 GPIOSetValue(PORT1, 2, 1);
 GPIOSetDir(PORT2, 6, 0);
 Delayus(40);
 ErrorBit=GPIOGetValue(PORT2, 6);
 Delayus(40);
 //SCL=0;
 GPIOSetValue(PORT1, 2, 0);
 return ErrorBit;
}

void iic__write8bit(unsigned char input)
{
 unsigned char temp;
	unsigned char send;
	
	GPIOSetDir(PORT2, 6, 1);
	
 for(temp=8;temp!=0;temp--)
 {
  send=(input&0x80);
	 if(send)
		 	GPIOSetValue(PORT2, 6, 1);
		 else
			GPIOSetValue(PORT2, 6, 0);
		 
  Delayus(40);
//SCL=1;	
	GPIOSetValue(PORT1, 2, 1);
  Delayus(40);
 // SCL=0;	
	GPIOSetValue(PORT1, 2, 0);
  Delayus(40);
  input=input<<1;
 }
}

unsigned char iic__read8bit(void)
{
  unsigned char temp,rbyte=0;
	
	
	
	GPIOSetDir(PORT2, 6, 0);
	
 for(temp=8;temp!=0;temp--)
 {
 // SCL=1;	
	GPIOSetValue(PORT1, 2, 1);
  Delayus(40);
	 
  rbyte=rbyte<<1;
	 
	if(GPIOGetValue(PORT2, 6))
	{
  rbyte|=1;
	}
 // SCL=0;	
	 GPIOSetValue(PORT1, 2, 0);
	 Delayus(40);
 }
 return rbyte; 
}
/////////////////////////////////////////

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

// void iic__ack(void)
// {
// 		/*set as output*/
// 	
// 	GPIOSetDir(PORT2, 6, 1);
//   //SDA=0
// 	GPIOSetValue(PORT2, 6, 0);
//   Delayus(40);
//   //SCL=1;
// 	GPIOSetValue(PORT1, 2, 1);
//   Delayus(40);
//   //SCL=0;
// 	GPIOSetValue(PORT1, 2, 0);
//   Delayus(40);
//   //SDA=1;
// 	GPIOSetValue(PORT2, 6, 1);
//   Delayus(40);
// }
void iic_NoAck(void)
{
	/*set as output*/
	
	GPIOSetDir(PORT2, 6, 1);
	
 //SDA=1;
	GPIOSetValue(PORT2, 6, 1);
 Delayus(40);
 //SCL=1;
	GPIOSetValue(PORT1, 2, 1);
 Delayus(40);
 //SCL=0;
	GPIOSetValue(PORT1, 2, 0);
 Delayus(40);
 //SDA=0;
	GPIOSetValue(PORT2, 6, 0);
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


//初始化HMC5883，根据需要请参考pdf进行修改****
void Init_HMC5883(void)
{
     Single_Write_HMC5883(0x02,0x00);  //连续读取，
}


/******************************************************************************
********************************************************************************
REDIO
****************************************************************************
*/
void get_pll(void)
{
    unsigned char hlsi;
   // unsigned int twpll=0;
    hlsi=radio_write_data[2]&0x10;  //HLSI位
    if (hlsi)
        pll=(uint16_t)((float)((frequency+225)*4)/(float)32.768);    //频率单位:k
    else
        pll=(uint16_t)((float)((frequency-225)*4)/(float)32.768);    //频率单位:k
}


//由PLL计算频率
void get_frequency(void)
{
    unsigned char hlsi;
    uint16_t npll=0;
    npll=pll;
    hlsi=radio_write_data[2]&0x10;
    if (hlsi)
        frequency=(unsigned long)((float)(npll)*(float)8.192-225);    //频率单位:KHz
    else
        frequency=(unsigned long)((float)(npll)*(float)8.192+225);    //频率单位:KHz
}
/*
***radio_write
***
***
*/
void radio_write(void)
{
    unsigned char i;
    iic__start();
    iic__write8bit(0xc0);        //TEA5767写地址
    if(!iic__testack())
    {
        for(i=0;i<5;i++)
        {
            iic__write8bit(radio_write_data[i]);
            iic__ack();
        }
    }
    iic__stop();    
}
/*
***radio_read
//读TEA5767状态,并转换成频率
***
*/
void radio_read(void)
{
    unsigned char i;
    unsigned char temp_l,temp_h;
    pll=0;
    iic__start();
    iic__write8bit(0xc1);        //TEA5767读地址
    if(!iic__testack())
    {
        for(i=0;i<5;i++)
        {
            radio_read_data[i]=iic_read8bit();
           iic__ack();
        }
    }
    iic__stop();
    temp_l=radio_read_data[1];
    temp_h=radio_read_data[0];
    temp_h&=0x3f;
    pll=temp_h*256+temp_l;
    get_frequency();
}
/*
***search channedl
*/
void search(unsigned char  mode)
{
    radio_read();  
    if(mode)
    {
        frequency+=100;
        if(frequency>max_freq)
            frequency=min_freq;
    }
    else
    {
        frequency-=100;
        if(frequency<min_freq)
           frequency=max_freq;
    }          
    get_pll();
    radio_write_data[0]=pll/256;
    radio_write_data[1]=pll%256;
		radio_write_data[2]=0x20;
    radio_write_data[3]=0x11;
    radio_write_data[4]=0x00;
    radio_write();
		
} 
//自动搜台,mode=1,频率增加搜台; mode=0:频率减小搜台,不过这个好像不能循环搜台
void auto_search(unsigned char  mode)
{
    radio_read();        
    if(mode)
    {
        radio_write_data[2]=0xa0;//向上搜索
        frequency+=20;
        if(frequency>max_freq)
            frequency=min_freq;
    }
    else
    {
		radio_write_data[2]=0x20; 
        frequency-=20;
        if(frequency<min_freq)
            frequency=max_freq;
    }          
    get_pll();

    radio_write_data[0]=pll/256+0x40;      //加0x40是将SM置为1 为自动搜索模式
    radio_write_data[1]=pll%256;    
    radio_write_data[3]=0x11;              //SSL1和SSL0控制搜索停止条件             
    radio_write_data[4]=0x40;
		
    radio_write();
    radio_read();
    while(!(radio_read_data[0]&0x80))     //搜台成功标志
    {
        radio_read();
    }   
   
} 

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

