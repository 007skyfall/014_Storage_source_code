#include "lpc11xx.h"
#include "tools.h"

static uint8_t  fac_us=0;//us延时倍乘数
static uint16_t fac_ms=0;//ms延时倍乘数

//初始化延迟函数
//SYSTICK的时钟固定为HCLK时钟的1/2
//SYSCLK:系统时钟eg:48MHz
void InitDelay(uint8_t sysclk)	 
{
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk; 	//选择系统时钟48MHz
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
	fac_us=sysclk;	//为系统时钟的1/1
	fac_ms=(uint16_t)fac_us*1000;//代表每个ms需要的systick时钟数
}								    

//延时nus
//注意nus的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//0xffffff >= nus * 48
//nus<=0xffffff / 48 = 349525us
void Delay_us(uint32_t nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=nus*fac_us - 1; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	 
}

//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//0xffffff >= nms * 48 * 1000
//nms<=0xffffff / 48000 = 349ms
void Delay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;		   
	SysTick->LOAD=(uint32_t)nms*fac_ms - 1;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	  	    
} 

