/*******************************************************************************
* File:    temp_hum.c 
* Author:  FarSight Design Team
* Version: V1.00
* Date:    2011.06.21
* Brief:   Temperature & humidity program
*******************************************************************************/ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC11xx.h"
#include "gpio.h"
#include "oled2864.h"
#include "key.h"
#include "tools.h"
#include "temp_hum.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern volatile uint32_t p3_2_counter;
extern volatile uint32_t p3_2_tc;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : Read_Temp_Hum
* Description    : Read Temp & Hum.
* Input          : - temp : temp buf pointer.
*                  - hum : hum buf pointer.
* Output         : - temp[0] : temp x. part
*                  - temp[1] : temp .x part
*                  - hum[0] : hum x. part
*                  - hum[1] : hum .x part
* Return         : - 1, checksum ok
*                  - 0, checksum fail
*******************************************************************************/
uint32_t Read_Temp_Hum(uint8_t *temp, uint8_t *hum)
{
  uint32_t	cnt_last;
  uint8_t	temp10, temp01, hum10, hum01, chksum, chk;
  uint32_t	tc1, tc;
  uint32_t	i;

  p3_2_counter = 0;
  cnt_last = p3_2_counter;

  GPIOSetDir(PORT3, 2, 1);			// Set PIO1_5 to output
  GPIOSetValue(PORT3, 2, 0);
  delay_ms(30);
  GPIOSetValue(PORT3, 2, 1);

  GPIOSetDir(PORT3, 2, 0);
  GPIOSetInterrupt(PORT3, 2, 0, 0, 0);
  GPIOIntEnable(PORT3, 2);

  for(i=0; i<3; i++)
  {
    GPIOSetInterrupt(PORT3, 2, 0, 0, i&0x01);
    while(p3_2_counter == cnt_last);
    cnt_last = p3_2_counter;
  }

  for(i=0; i<40; i++)
  {
	GPIOSetInterrupt(PORT3, 2, 0, 0, 1);
    while(p3_2_counter == cnt_last);
    cnt_last = p3_2_counter;
	tc1 = p3_2_tc;

	GPIOSetInterrupt(PORT3, 2, 0, 0, 0);
    while(p3_2_counter == cnt_last);
    cnt_last = p3_2_counter;

	if(p3_2_tc < tc1)
	{
	  tc = tc1 - p3_2_tc;
	}
	else
	{
	  tc = 48000 - (p3_2_tc - tc1);
	}

	if(i < 8)
	{
	  temp10 <<= 1;
	  if(tc >= 2328)
	    temp10 |= 0x01;
	}
	else if(i < 16)
	{
	  temp01 <<= 1;
	  if(tc >= 2328)
	    temp01 |= 0x01;
	}
	else if(i < 24)
	{
	  hum10 <<= 1;
	  if(tc >= 2328)
	    hum10 |= 0x01;
	}
	else if(i < 32)
	{
	  hum01 <<= 1;
	  if(tc >= 2328)
	    hum01 |= 0x01;
	}
	else
	{
	  chksum <<= 1;
	  if(tc >= 2328)
	    chksum |= 0x01;
	}
  }

  GPIOSetInterrupt(PORT3, 2, 0, 0, 1);
  while(p3_2_counter == cnt_last);

  *temp = temp10;
  *(temp+1) = temp01;

  *hum = hum10;
  *(hum+1) = hum01;

  chk = temp10;
  chk += temp01;
  chk += hum10;
  chk += hum01;

  if(chk == chksum)
    return 1;
  else
    return 0;
}

/*******************************************************************************
* Function Name  : Temp_Hum_Test
* Description    : Temp & Hum func test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Temp_Hum_Test(void)
{
  uint8_t	temp[2], hum[2];
  char		buf[20];
  uint32_t	i;

  OLED_ClearScreen();
  OLED_DisStrLine(0, 0, "Temp & Hum");
  printf("\r\nTemp & Hum\r\n");

  while(1)
  {
    i = Read_Temp_Hum(temp, hum);

	if(i)
	{
      snprintf(buf, 16, "Temp: %d.%d  ", temp[0], temp[1]);
	  OLED_DisStrLine(2, 0, (uint8_t *)buf);
	  snprintf(buf, 16, "Hum:  %d.%d  ", hum[0], hum[1]);
	  OLED_DisStrLine(3, 0, (uint8_t *)buf);

	  printf("\r\nTemp: %d.%d", temp[0], temp[1]);
	  printf("\r\nHum:  %d.%d", hum[0], hum[1]);
    }
    else
    {
      printf("\r\nfail");
    }

	delay_ms(200);
    if(KEY_Read() == KEY_ESC)
	  break;

	delay_ms(200);
    if(KEY_Read() == KEY_ESC)
	  break;

	delay_ms(200);
    if(KEY_Read() == KEY_ESC)
	  break;

	delay_ms(200);
    if(KEY_Read() == KEY_ESC)
	  break;

	delay_ms(200);
    if(KEY_Read() == KEY_ESC)
	  break;
  } 
}


/**** End of File ****/


