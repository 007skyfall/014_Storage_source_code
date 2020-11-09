/*******************************************************************************
* File:    myrfid.c 
* Author:  LowKey Design Team
* Version: V1.00
* Date:    2012.04.07
* Brief:   Rfid program body
********************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

#include "LPC11xx.h"
#include "ssp.h"
#include "tools.h"
#include "oled2864.h"
#include "rfid.h"
#include "menu.h"
#include "gpio.h"
#include "myrfid.h"
#include "timer32.h" 
/* Includes End --------------------------------------------------------------*/

/* Struct --------------------------------------------------------------------*/
#if 0
struct sendGoods
{
	uint8_t head[3];	 //��ʶλst:
	uint8_t	type;		 //��������
	uint8_t snum;		 //�ֿ���
	uint8_t io;			 //�����ֿ�
	uint8_t goodsno;	 //��Ʒ���
	uint8_t goodsnum;	 //��Ʒ����
};
#endif
/* Struct End ----------------------------------------------------------------*/


char type = 'I';
//uint8_t tbuf[10] =  {0x0a, 0x21, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; 
const uint8_t RFID_READ_DATA_BLOCK_21[10] =  {0x0a, 0x21, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; 
void delay(uint8_t num);

/*******************************************************************************
* Function Name  : Rfid_Init
* Description    : Rfid Init program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void Rfid_Init(void)
//{
//	SPI_Init(0, 8, 128);
//	delay_ms(1);
//	GPIOSetDir(PORT2, 8, 0);
//    GPIOSetInterrupt(PORT2, 8, 1,0, 0 );	  //����RFID �ж�
//	GPIOIntEnable(PORT2,8);					  //��ʼ��ʹ��RFID �ж�
//
//	return;
//}
void Rfid_Init(void)
{
	SPI_Init(0, 8, 128); 
	delay_ms(1);
	GPIOSetDir(PORT2, 8, 0); //����Ƭѡ��ʹ��оƬ
	return;	
}

//read rfid, save in rbuf, return value is the length
/*******************************************************************************
* Function Name  : Rfid_Read
* Description    : Rfid Read program.
* Input          : None
* Output         : None
* Return         : i
*******************************************************************************/


/*******************************************************************************
* Function Name  : Rfid_Operation
* Description    : Rfid Operation program.
* Input          : None
* Output         : None
* Return         : rlen
*******************************************************************************/
void Rfid_Operation(uint8_t *rbuf)
{
	Rfid_Init();
	Rfid_Read(rbuf,(uint8_t *)RFID_READ_DATA_BLOCK_21);	
}


uint8_t Rfid_Read(uint8_t *rbuf,uint8_t *tbuf)
{
	uint8_t	chksum;
	uint32_t	i, j, rnumb;

//	uint8_t	tmp;

	delay_ms(100); //ԭ����
	RFID_CS(0);
	

	chksum = RFID_CheckSum(tbuf);  //���У����,chksum=42(0x2A)

	SPI_PutGet(0, 0xaa);		//����0xaa
	delay_ms(1);

	i = 0;
	rbuf[i++] = SPI_PutGet(0, 0xbb);  //����0xbb�����ҽ�������0xAA
	delay_ms(1);

	for(j=0; j<tbuf[0]; j++)
	{
		rbuf[i++] = SPI_PutGet(0, tbuf[j]);	//i=1��ʼ
    	delay_ms(1);
	}


	rbuf[i] = SPI_PutGet(0, chksum); //i=9,chksum=42(0x2A)
	i++;//ԭ��û��					//i=10

	delay_ms(2);

	rbuf[0] = 1; //ԭ��û��

	SPI_PutGet(0, 0);
	delay_ms(1);
  
	if(SPI_PutGet(0, 0) != 0xaa)   //����ִ��
	{
    	SPI_Init(0, 8, 2);
    	return 0;
	}

	delay_ms(1);
  
	if(SPI_PutGet(0, 0) != 0xbb)  //����ִ��
	{	//printf("hello");
    	SPI_Init(0, 8, 2);
    	return 0;
	}
	
	delay_ms(1);

switch(tbuf[1])
  {
    case 0x01:			  //��ͷ���ͣ�����8�ֽڵ�ģ���ͺţ�
	  rnumb = 8 + 2 + 1;
	  break;

	case 0x20:			  //����������4�ֽڿ����кţ�
	  rnumb = 4 + 2 + 1;
	  break;

	case 0x21:			   //�����ݿ飨����16�ֽڵĿ����ݣ�
	  rnumb = 16 + 2 + 1;
	  break;

	case 0x22:			  //д���ݿ飨����������Ϊ�յ�֡��
	  rnumb = 2 + 1;
	  break;

	default:
	  rnumb = 4 + 2 + 1;
	  break;
  }

	for(j=0, i=0; j<rnumb; j++, i++)
	{
    	rbuf[i] = SPI_PutGet(0, 0);
		delay_ms(1);
	}
	RFID_CS(1);
	delay_ms(1);
	SPI_Init(0, 8, 2);

	return i;	
}

//--------------------------------------------------------------------
void Speaker_OnOff(uint8_t value)
{
	LPC_IOCON->R_PIO1_1 = (LPC_IOCON->R_PIO1_1 & (~0x07)) | 0x01;
	GPIOSetDir(PORT1, 1, 1);			// ����GPIO1_1Ϊ���ģʽ
	GPIOSetValue(PORT1, 1, value);		// ��ֵ�����Ƿ�򿪷�����
	delay(100);
	GPIOSetValue(PORT1, 1, 1);		   // PIO1_1 output 1,	Speaker turn off

}

void Speaker_OnOffsw(uint8_t value)
{
	LPC_IOCON->R_PIO1_1 = (LPC_IOCON->R_PIO1_1 & (~0x07)) | 0x01;
	GPIOSetDir(PORT1, 1, 1);			// ����GPIO1_1Ϊ���ģʽ
	GPIOSetValue(PORT1, 1, value);		// ��ֵ�����Ƿ�򿪷�����
}
//--------------------------------------------------------------------
void fan_onoff(uint8_t value)
{
	LPC_IOCON->PIO0_2 = (LPC_IOCON->PIO0_1 & (~0x07));
	GPIOSetDir(PORT0, 2, 1);			
	GPIOSetValue(PORT0, 2, value);		 
	delay_ms(200);
	GPIOSetValue(PORT0, 2, 1);		
}
//--------------------------------------------------------------------



//display the number on oled
void Rfid_Display(uint8_t len, uint8_t *rbuf)
{
	int i,x;
	char buf[8];

	if (0==len)
	{
		OLED_DisStrLine(2-1 + 5, 0, "                     ");
		OLED_DisStrLine(2-1 + 6, 0, "Fail");
	//	printf("fail") ;
		return;
	}
	else
	{
	for(i=0, x=0; i<len; i++)
    {
		if(x <= 108)
		{
		  snprintf(buf, 4, "%02X ", rbuf[i]);
		  OLED_DisChar(2-1 + 5, x, 0, buf[0]);
          x += 6;
          OLED_DisChar(2-1 + 5, x, 0, buf[1]);
          x += 6;
          OLED_DisChar(2-1 + 5, x, 0, buf[2]);
          x += 6;
        }
		OLED_DisStrLine(2-1 + 6, 0, "OK");
    }
	}
	return;
}



void delay(uint8_t num)
{
    uint8_t i,j,k;
    
    for (i = 0; i<10; i++)
        for (j = 0; j<10; j++)
            for (k=0; k<num; k++);
}


