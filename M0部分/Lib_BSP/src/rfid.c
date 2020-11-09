/*******************************************************************************
* File:    rfid.c 
* Author:  FarSight Design Team
* Version: V1.00
* Date:    2011.06.21
* Brief:   RFID program
*******************************************************************************/ 

/* Includes ------------------------------------------------------------------*/
#include "LPC11xx.h"
#include "gpio.h"
#include "ssp.h"
#include "tools.h"
#include "rfid.h"
#include "type.h"

uint8_t RfidGetFinishFlag;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const uint8_t RFID_READ_MOD_TYPE_01[2] = {0x02, 0x01};
const uint8_t RFID_READ_TYPE_19[2] = {0x02, 0x19};
const uint8_t RFID_READ_CARD_20[2] = {0x02, 0x20};
const uint8_t RFID_READ_DATA_BLOCK_21[10] =  {0x0a, 0x21, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; 
const uint8_t RFID_WRITE_DATA_BLOCK_22_0[26] = {0x1a, 0x22, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                              0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08,
										      0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00}; 
const uint8_t RFID_WRITE_DATA_BLOCK_22_1[26] = {0x1a, 0x22, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                              0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
										      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}; 


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : RFID_CheckSum
* Description    : Init RFID control.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t RFID_CheckSum(uint8_t *databuf)
{ 
	unsigned char	numb, chksum=0;;

 	numb = databuf[0]-1;

	for(numb=0;numb<databuf[0];numb++)
	{
    	chksum += databuf[numb]; 
	} 
  	return chksum;
}
/*****************************************************************************
** Function name:		SSP_Send
**
** Descriptions:		Send a block of data to the SSP port, the 
**						first parameter is the buffer pointer, the 2nd 
**						parameter is the block length.
**
** parameters:			port #, buffer pointer, and the block length
** Returned value:		None
** 
*****************************************************************************/

static unsigned short SPI0_Send( unsigned char  portNum, unsigned char  buf )
{
 	
  
	if ( portNum == 0 )
	{
	  GPIOSetValue( PORT2, 7, 0 ); 
	  while (( !(LPC_SSP0->SR & SSPSR_TNF)||(LPC_SSP0->SR & SSPSR_BSY)) != 0 );
	  LPC_SSP0->DR = buf;
	  while ( LPC_SSP0->SR & SSPSR_BSY );
	      /* Wait until the Busy bit is cleared */
     while((LPC_SSP0->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE);
   GPIOSetValue( PORT2, 7, 1);
  }
  return	  LPC_SSP0->DR;
}
/*******************************************************************************
* Function Name  : RFID_Operate
* Description    : Output segment to LED.
* Input          : - numb : display number.
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t RFID_Operate(uint8_t *tbuf, uint8_t *rbuf)
{
  uint8_t	chksum;
  uint32_t 	i, j, rnumb;

  SPI_Init(0, 8, 128);

  chksum = RFID_CheckSum(tbuf);

  SPI0_Send(0, 0xaa);
  //delay_ms(1);

  i = 0;
  rbuf[i] = SPI0_Send(0, 0xbb);
  i ++;
  //delay_ms(1);

  for(j=0; j<tbuf[0]; j++)
  {
    rbuf[i] = SPI0_Send(0, tbuf[j]);
	i ++;
    Delay_ms(1);
  }

  rbuf[i] = SPI0_Send(0, chksum);
  i ++;

   Delay_ms(200);

  rbuf[0] = 1;
 
  if(SPI0_Send(0, 0) != 0xaa)
  {
    SPI_Init(0, 8, 2);
    return 0;
  }

  Delay_ms(1);
  
  if(SPI0_Send(0, 0) != 0xbb)
  {
    SPI_Init(0, 8, 2);
    return 0;
  }
	
  Delay_ms(1);
	 
  switch(tbuf[1])
  {
    case 0x01:
	  rnumb = 8 + 2 + 1;
	  break;

	case 0x20:
	  rnumb = 4 + 2 + 1;
	  break;

	case 0x21:
	  rnumb = 16 + 2 + 1;
	  break;

	case 0x22:
	  rnumb = 2 + 1;
	  break;

	default:
	  rnumb = 4 + 2 + 1;
	  break;
  }

  for(j=0, i=0; j<=rnumb; j++, i++)
  {
    rbuf[i] = SPI0_Send(0, 0);
	Delay_ms(1);
  }		
  SPI_Init(0, 8, 2);

  return i;
}
	
// 10ms扫描一次
void ScanRfid(void)
{
	static uint8_t rfidfiltercnt = 0; // 刷卡滤波肌计数
	static uint8_t rfidstatus = 1; // 刷开抬起 1抬起 0未抬起
	
	if((GPIOGetValue(PORT2, 8)&0x100) == 0) // 得到P2_8引脚的电平，如果为电平有刷卡
	{
		
		rfidfiltercnt++;										
		if(rfidfiltercnt > 10)
		{
			rfidfiltercnt = 0; // 100ms滤波
			if(rfidstatus == 1) // 是第一次刷卡  //
			{
				rfidstatus = 2; // 不是第一次刷卡,就是卡一直在放rfid上
				RfidGetFinishFlag = TRUE; // 刷卡成功
			}
		}
	}
	else // 无刷卡
	{
		rfidfiltercnt = 0;
		rfidstatus = 1;
	}
}

void InitRfid(void)
{
	GPIOSetDir(PORT2, 8, 0);
	GPIOSetValue(PORT2, 8, 1);
	RfidGetFinishFlag = FALSE;         //rfid 刷卡标志位，FALSE 没有卡，TURE 有卡
}

/**** End of File ****/


