#include "LPC11xx.h"
#include "gpio.h"
#include "tools.h"
#include "type.h"
#include "temp_hum.h"

uint8_t temperature[2];
uint8_t humidity[2];
uint8_t	DHT11GetFinishFlag;

//�ȴ�DHT11�Ļ�Ӧ
//����1:δ��⵽DHT11�Ĵ���
//����0:����
uint8_t DHT11_Check(void) 	   
{
	uint8_t retry=0;
	GPIOSetDir(PORT1, 5, 0); //SET INPUT
  while((GPIOGetValue(PORT1, 5)&0x20) && retry<100) // DHT11������40~80us  GPIOGetValue�����ǵõ����ŵĵ�ƽֵ
	{
		retry++;
		Delay_us(1);
	};
	if(retry>=100)return 1;
	else retry=0;
  while (!(GPIOGetValue(PORT1, 5)&0x20) && retry<100) // DHT11���ͺ���ٴ�����40~80us GPIOGetValue�����ǵõ����ŵĵ�ƽֵ
	{
		retry++;
		Delay_us(1);
	};
	if(retry>=100)return 1;	
	return 0;
}

//��DHT11��ȡһ��λ
//����ֵ��1/0
uint8_t DHT11_Read_Bit(void) 			 
{
 	uint8_t retry=0;
	while((GPIOGetValue(PORT1, 5)&0x20) && retry<100) //�ȴ���Ϊ�͵�ƽ
	{
		retry++;
		Delay_us(1);
	}
	retry=0;
	while(!(GPIOGetValue(PORT1, 5)&0x20) && retry<100)//�ȴ���ߵ�ƽ
	{
		retry++;
		Delay_us(1);
	}
	Delay_us(40);//�ȴ�40us
	if((GPIOGetValue(PORT1, 5)&0x20))
		return 1;
	else return 0;		   
}

//��DHT11��ȡһ���ֽ�
//����ֵ������������
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

//��DHT11��ȡһ������
//temp:�¶�ֵ(��Χ:0~50��)
//humi:ʪ��ֵ(��Χ:20%~90%)
//����ֵ��0,����;1,��ȡʧ��
uint8_t ReadDHT11(uint8_t *temp, uint8_t *humi)    
{        
 	uint8_t buf[5];
	uint8_t i;
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//��ȡ40λ����
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

// ��ʱ��10ms�ж�ɨ��DHT11
void TimeScanDHT11(void)
{
	static uint8_t runcnt = 0;
	
	switch(runcnt)
	{
		case 0:
			GPIOSetDir(PORT1, 5, 1); // SET OUTPUT
			GPIOSetValue(PORT1, 5, 0); // ����DQ����
			runcnt = 1;
			break;
		case 1: // 10ms
			runcnt = 2; 
			break;
		case 2: // 20ms
			GPIOSetValue(PORT1, 5, 1); //DQ=1 
			Delay_us(30); //��������20~40us
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
	GPIOSetValue(PORT1, 5, 1); // �����
	DHT11GetFinishFlag = FALSE;
	temperature[0] = 0;
	temperature[1] = 0;
	humidity[0] = 0;
	humidity[1] = 0;
}


