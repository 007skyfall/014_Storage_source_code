/*******************************************************************************
* File:    rfid.c 
* Author:  FarSight Design Team
* Version: V1.00
* Date:    2011.06.21
* Brief:   RFID program
*******************************************************************************/ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC11xx.h"
#include "gpio.h"
#include "ssp.h"
#include "oled2864.h"
#include "key.h"
#include "tools.h"
#include "rfid.h"
#include "menu.h"
#include "string.h"
#include "myrfid.h"

struct sendGoods
{
	uint8_t head[3];	 //标识位st:
	uint8_t	type;		 //数据类型
	uint8_t snum;		 //仓库编号
	uint8_t io;			 //进出仓库
	uint8_t goodsno;	 //商品编号
	uint8_t goodsnum;	 //商品数量
};

const uint8_t RFID_READ_MOD_TYPE_01[2] = {0x02, 0x01};
const uint8_t RFID_READ_CARD_20[2] = {0x02, 0x20};
//const uint8_t RFID_READ_DATA_BLOCK_21[10] =  {0x0a, 0x21, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; 
const uint8_t RFID_WRITE_DATA_BLOCK_22[26] = {0x1a, 0x22, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                              0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
										      0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10};

//uint8_t tbuf[10]; //fc 
//uint8_t r_key[10] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff};
//uint8_t wbuf[16] = {0x02,0x00,0x01,0x01,0x00,0x0b,0x00,0x18,0x00,0x10,0x0f,0x0c,0x0d,0x0e,0x05,0x07};
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
  uint8_t	numb, chksum;
  uint32_t	i;

  numb = databuf[0]-1;
  for(i=0; i<databuf[0]; i++)
  {
    if(databuf[i] == 0xaa)
	  numb ++;
  }

  chksum = databuf[0];
  while(numb)
  {
    chksum ^= databuf[numb];
    numb --;
  }

  return chksum;
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
  uint32_t	i, j, rnumb;

  SPI_Init(0, 8, 128);
  delay_ms(1);
  RFID_CS(0);
  //tbuf[0]=0x02;
  //tbuf[1]=0x20;

  chksum = RFID_CheckSum(tbuf);	   //求出校验码,chksum=42(0x2A)

  SPI_PutGet(0, 0xaa);		//发送0xaa
  delay_ms(1);

  i = 0;
  rbuf[i++] = SPI_PutGet(0, 0xbb);	  //发送0xbb，并且接收数据0xAA
 // i ++;
  delay_ms(1);

  for(j=0; j<tbuf[0]; j++)
  {
    rbuf[i] = SPI_PutGet(0, tbuf[j]);	 //i=1开始
	i ++;
    delay_ms(1);
  }

  rbuf[i] = SPI_PutGet(0, chksum); //i=9,chksum=42(0x2A)
  i ++;	 //原本没有				   //i=10

  delay_ms(500);

  rbuf[0] = 1;

  SPI_PutGet(0, 0);
  delay_ms(1);
  
  if(SPI_PutGet(0, 0) != 0xaa)	   //不会执行
  {
    SPI_Init(0, 8, 2);
    return 0;
  }

  delay_ms(1);
  
  if(SPI_PutGet(0, 0) != 0xbb)	    //不会执行
  {
    SPI_Init(0, 8, 2);
    return 0;
  }
	
  delay_ms(1);
	 
  switch(tbuf[1])
  {
    case 0x01:
	  rnumb = 8 + 2 + 1;  //读头类型（返回8字节的模块型号）
	  break;

	case 0x20:
	  rnumb = 4 + 2 + 1; //读卡（返回4字节卡序列号）
	  break;

	case 0x21:
	  rnumb = 16 + 2 + 1; //读数据块（返回16字节的块内容）
	  break;

	case 0x22:
	  rnumb = 2 + 1;	 //写数据块（返回数据域为空的帧）
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


 



/**** End of File ****/







