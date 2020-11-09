#include "LPC11xx.h"
#include "gpio.h"
#include "tools.h"
#include "type.h"
#include "temp_hum.h"

uint8_t temperature[2];
uint8_t humidity[2];
uint8_t	DHT11GetFinishFlag;

//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
uint8_t DHT11_Check(void) 	   
{
	uint8_t retry=0;
	GPIOSetDir(PORT1, 5, 0); //SET INPUT
  while((GPIOGetValue(PORT1, 5)&0x20) && retry<100) // DHT11会拉低40~80us  GPIOGetValue函数是得到引脚的电平值
	{
		retry++;
		Delay_us(1);
	};
	if(retry>=100)return 1;
	else retry=0;
  while (!(GPIOGetValue(PORT1, 5)&0x20) && retry<100) // DHT11拉低后会再次拉高40~80us GPIOGetValue函数是得到引脚的电平值
	{
		retry++;
		Delay_us(1);
	};
	if(retry>=100)return 1;	
	return 0;
}

//从DHT11读取一个位
//返回值：1/0
uint8_t DHT11_Read_Bit(void) 			 
{
 	uint8_t retry=0;
	while((GPIOGetValue(PORT1, 5)&0x20) && retry<100) //等待变为低电平
	{
		retry++;
		Delay_us(1);
	}
	retry=0;
	while(!(GPIOGetValue(PORT1, 5)&0x20) && retry<100)//等待变高电平
	{
		retry++;
		Delay_us(1);
	}
	Delay_us(40);//等待40us
	if((GPIOGetValue(PORT1, 5)&0x20))
		return 1;
	else return 0;		   
}

//从DHT11读取一个字节
//返回值：读到的数据
uint8_t DHT11_Read_Byte(void)    
{        
  uint8_t i,dat;
  dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1;
	    dat|=DHT11_Read_Bit();
  }						    
  return dat;
}

//从DHT11读取一次数据
//temp:温度值(范围:0~50°)
//humi:湿度值(范围:20%~90%)
//返回值：0,正常;1,读取失败
uint8_t ReadDHT11(uint8_t *temp, uint8_t *humi)    
{        
 	uint8_t buf[5];
	uint8_t i;
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//读取40位数据
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3]) == buf[4])
		{
			*(humi + 0)=buf[0];
			*(humi + 1)=buf[1];
			*(temp + 0)=buf[2];
			*(temp + 1)=buf[3];
		}
	}else return 1;
	return 0;	    
}

// 定时器10ms中断扫描DHT11
void TimeScanDHT11(void)
{
	static uint8_t runcnt = 0;
	
	switch(runcnt)
	{
		case 0:
			GPIOSetDir(PORT1, 5, 1); // SET OUTPUT
			GPIOSetValue(PORT1, 5, 0); // 拉低DQ启动
			runcnt = 1;
			break;
		case 1: // 10ms
			runcnt = 2; 
			break;
		case 2: // 20ms
			GPIOSetValue(PORT1, 5, 1); //DQ=1 
			Delay_us(30); //主机拉高20~40us
			if(ReadDHT11(temperature, humidity) == 0)
			{
				DHT11GetFinishFlag = TRUE;
			}
			runcnt++;
			break;
		case 20:
			runcnt = 0;	
			break;
		default:
			runcnt++;
			break;
	}
}

void InitDHT11(void)
{
	GPIOSetDir(PORT1, 5, 1); // SET OUTPUT
	GPIOSetValue(PORT1, 5, 1); // 输出高
	DHT11GetFinishFlag = FALSE;
	temperature[0] = 0;
	temperature[1] = 0;
	humidity[0] = 0;
	humidity[1] = 0;
}


