#include <avr/io.h>

#include "config.h"
#include "cy14443cmd.h"
#include "usart.h"
#include "delay/delay.h"

#include "beep/beep.h"
#include "app/key/key.h"


//#define DEBUG(...)
#define DEBUG usart_write

//硬件引脚定义
#define FLASH_MOSI PB5
#define FLASH_MISO PB6
#define FLASH_SCK		PB7
#define FLASH_CS	PB4

//spi初始化
//MAST模式
//时钟频率不超过3M
//MSB在前
//上升沿采样

void spi_init()
{
	//MISO设置为输入，上拉电阻关闭，其它默认为1
	PORTB |= _BV(FLASH_SCK)|_BV(FLASH_MOSI)|_BV(FLASH_CS)|_BV(PB2);	
	
	//SCK,MOSI和CS端口对应脚设置为输出
	DDRB |= _BV(FLASH_SCK)|_BV(FLASH_MOSI)|_BV(FLASH_CS);                            
	
	// SPI中断禁止, SPI使能, master模式, MSB 前,  SPI 模式 0, SCK频率Fcl/128
	SPCR = _BV(SPE)|_BV(MSTR)|_BV(SPR1)|_BV(SPR0);
}


//从SPI口输出一字节数据，同时读入一个字节
//输出字节读入的是上一个时钟周期从机发送的数据
//这一点千万注意识别

u8 spi_write(u8 data)
{
	cli();
	SPDR = data;
	while(!(SPSR & _BV(SPIF)));
	return SPDR;
}

//crc校验
u8 CheckSum(u8 *databuf)
{
	u8 tmp = databuf[0]-1;
	u8 chksum=databuf[0];
	while(tmp)
	{
		chksum^=databuf[tmp];
		tmp--;
	}
	if(databuf[databuf[0]]==chksum) return 1;
	else
		databuf[databuf[0]]=chksum;
	return 0;
}

//程序流程应该是
//1,初始化SPI口为MSB在前，速率不超过3M，上升沿采样
//2,调用spi_write函数发送命令序列给模块
//3,根据SPI时钟适当延时
//4.所有命令序列发送完毕，稍作延时等待模块响应
//5.再次调用spi_write发0或者其他非命令序列给模块
//6,从spi_write返回的数据中进行分析模块响应的数据包

int main(void)
{
	u8 tmp,a,x;
	u8 buf[16];		
	DDRA = 0xff;		//全部输出
	DDRB = 0xff;
	
	DDRD = 0x0;			//rxd in txd out 
	PORTD = 0xff;

	
	usart_init(9600);//9600
	for(a=0;a<1000;a++){asm("nop");}
	
	DEBUG("system\n");

	keyinit();//键盘初始化
	spi_init();
	
	initbeep();//蜂鸣器
	beep_ok();//蜂鸣器响一声
		
	tmp=0;
	
	while(1)//无限循环
	{
		tmp = getkey();//读键盘键值
		
		if(tmp!=255)//有按键
		{	
			PORTB &=~(1<<PB4);//模块片选SS=0，选中模块
			
			buf[0]=spi_write(0xaa);//命令头
			delayms(1);//适当延时，根据SPI时钟速率而定
			buf[1]=spi_write(0xbb);
			delayms(1);
			
/*			//这是另外一组测试命令组
			buf[2]=spi_write(0x02);//长度
			delayms(1);
			buf[3]=spi_write(0x01);//命令码，读序列号
			delayms(1);
			buf[4]=spi_write(0x3);//校验码
			delayms(1);
*/			
			buf[2]=spi_write(0x02);//长度
			delayms(1);
			buf[3]=spi_write(0x19);//命令码，读卡类型
			delayms(1);
			buf[4]=spi_write(0x1b);//校验码
			delayms(1);			

			delayms(200);//等待从机准备好数据
			delayms(200);

			
			buf[0]=spi_write(0);//开始读取数据，第一个都会来的字节是
								//发命令过程中最后一个字节
			delayms(1);
			buf[1]=spi_write(0);//读取的模块响应第二个字节 0xAA
			delayms(1);
			buf[2]=spi_write(0);//读取的模块响应第三个字节 0xBB
			delayms(1);
			buf[3]=spi_write(0);//读取的模块响应第四个字节 包长度，判断接下来需要接收的字节数
			delayms(1);			//最后一个字节，为校验码
			
			x = buf[3];
			for(tmp=0;tmp<x;tmp++)
			{
				buf[tmp]=spi_write(0);
				delayms(1);
			}
						
			PORTB |=(1<<PB4);//模块片选SS=1，模块无效
			
			DEBUG("\r\nsend data\r\n");
			for(tmp=0;tmp<x;tmp++)//将结果打印出来
			{
				DEBUG("buf[%x]=%x\r\n",tmp,buf[tmp]);
			}

		}
	}
	return 0;
}
