#include "lpc11xx.h"
#include "tools.h"

static uint8_t  fac_us=0;//us��ʱ������
static uint16_t fac_ms=0;//ms��ʱ������

//��ʼ���ӳٺ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/2
//SYSCLK:ϵͳʱ��eg:48MHz
void InitDelay(uint8_t sysclk)	 
{
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk; 	//ѡ��ϵͳʱ��48MHz
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	fac_us=sysclk;	//Ϊϵͳʱ�ӵ�1/1
	fac_ms=(uint16_t)fac_us*1000;//����ÿ��ms��Ҫ��systickʱ����
}								    

//��ʱnus
//ע��nus�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//0xffffff >= nus * 48
//nus<=0xffffff / 48 = 349525us
void Delay_us(uint32_t nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=nus*fac_us - 1; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ����	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	 
}

//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//0xffffff >= nms * 48 * 1000
//nms<=0xffffff / 48000 = 349ms
void Delay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;		   
	SysTick->LOAD=(uint32_t)nms*fac_ms - 1;//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           //��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	  	    
} 

