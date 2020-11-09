/*******************************************************************************
* File:    spi_uart.c 
* Author:  FarSight Design Team
* Version: V1.00
* Date:    2011.06.21
* Brief:   SPI to UART program
*******************************************************************************/ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>

#include "LPC11xx.h"
#include "gpio.h"
#include "ssp.h"
#include "uart.h"
#include "key.h"
#include "oled2864.h"
#include "spi_uart.h"
#include "tools.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SPI752_RBUF_0_NUMB	128
#define SPI752_RBUF_1_NUMB	128

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint8_t	SPI752_rbuf_0[SPI752_RBUF_0_NUMB];
volatile uint16_t	SPI752_rbuf_0_ip = 0;
volatile uint16_t	SPI752_rbuf_0_op = 0;
volatile uint8_t	SPI752_rbuf_1[SPI752_RBUF_1_NUMB];
volatile uint16_t	SPI752_rbuf_1_ip = 0;
volatile uint16_t	SPI752_rbuf_1_op = 0;


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : SPI752_IRQ_Process
* Description    : SPI - UART receive interrupt process.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI752_IRQ_Process(void)
{
  uint8_t	rd;

  rd = SPI752_RegRead(0, SPI752_IIR_R);
  rd &= 0x3f;  
  if(rd == 0x04)
  {
    SPI752_rbuf_0[SPI752_rbuf_0_ip] = SPI752_RegRead(0, SPI752_RHR_R);
    SPI752_rbuf_0_ip ++;
	if(SPI752_rbuf_0_ip >= SPI752_RBUF_0_NUMB)
	  SPI752_rbuf_0_ip = 0;
  }
  else
  {
    rd = SPI752_RegRead(0, SPI752_RHR_R);
  }

  rd = SPI752_RegRead(1, SPI752_IIR_R);
  rd &= 0x3f;  
  if(rd == 0x04)
  {
    SPI752_rbuf_1[SPI752_rbuf_1_ip] = SPI752_RegRead(1, SPI752_RHR_R);
    SPI752_rbuf_1_ip ++;
	if(SPI752_rbuf_1_ip >= SPI752_RBUF_1_NUMB)
	  SPI752_rbuf_1_ip = 0;
  }
  else
  {
    rd = SPI752_RegRead(1, SPI752_RHR_R);
  }
}


/*******************************************************************************
* Function Name  : SPI752_RegRead
* Description    : Read Registor.
* Input          : - Channel : 0 & 1.
*                  - Reg : 0x00 - 0x0f.
* Output         : None
* Return         : Registor value.
*******************************************************************************/
uint8_t SPI752_RegRead(uint8_t Channel, uint8_t Reg)
{
  uint8_t	rd;

  SPI_UART_CS(0);
  SPI_PutGet(1, SPI752_READ | (Reg<<3) | (Channel<<1));
  rd = SPI_PutGet(1, 0);
  SPI_UART_CS(1);

  return rd;
}

/*******************************************************************************
* Function Name  : SPI752_RegWrite
* Description    : Write 8bit data to Registor.
* Input          : - Channel : 0 & 1.
*                  - Reg : 0x00 - 0x0f.
*                  - Data : 8bit data.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI752_RegWrite(uint8_t Channel, uint8_t Reg, uint8_t Data)
{
  SPI_UART_CS(0);
  SPI_PutGet(1, SPI752_WRITE | (Reg<<3) | (Channel<<1));
  SPI_PutGet(1, Data);
  SPI_UART_CS(1);
}


/*
uint16_t spi_uart_PutGet_IO(uint16_t tData)
{
  uint32_t	i;

  //SPI_UART_CS(0);
	
  for(i=0; i<8; i++)
  {
    SPI_UART_CLK(0);
    SPI_UART_MOSI((tData&0x80)>>7);
    tData = tData << 1;
    SPI_UART_CLK(1);
  }

  //SPI_UART_CS(1);
  return 0;
}
*/


/*******************************************************************************
* Function Name  : SPI752_Init
* Description    : Set channel 0 & 1 baud rate. The range is 300-230400 Baud.
*                  The crystal input frequency is 14745600Hz.
*				   The default value of prescaler after reset is divide-by-1.
*                  The format is: 8N1
* Input          : - Channel : 0 & 1.
*                  - Baud : 300-230400 Baud.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI752_Init(uint8_t Channel, uint32_t Baud)
{
  uint16_t	rd;

  // Disable sleep
  SPI752_RegWrite(Channel, SPI752_LCR_RW, 0xBF);
  rd = SPI752_RegRead(Channel, SPI752_EFR_RW);
  SPI752_RegWrite(Channel, SPI752_EFR_RW, rd | 0x10);

  SPI752_RegWrite(Channel, SPI752_LCR_RW, 0x03);
  rd = SPI752_RegRead(Channel, SPI752_IER_RW);
  SPI752_RegWrite(Channel, SPI752_IER_RW, rd & (~0x10));

  rd = SPI752_RegRead(Channel, SPI752_MCR_RW);
  SPI752_RegWrite(Channel, SPI752_MCR_RW, rd & (~0x80));

  // Set baud rate & 8N1 format
  SPI752_RegWrite(Channel, SPI752_LCR_RW, 0x83);
  rd = (14745600/16) / Baud;
  SPI752_RegWrite(Channel, SPI752_DLL_RW, rd);
  SPI752_RegWrite(Channel, SPI752_DLH_RW, rd>>8);
  SPI752_RegWrite(Channel, SPI752_LCR_RW, 0x03);


  SPI752_RegRead(Channel, SPI752_RHR_R);

  // use port3_3 as input event, SPI752 interrupt.
  GPIOSetDir(PORT3, 3, 0);
  // port3_3 interrupt. edge, single trigger, falling edges.
  GPIOSetInterrupt(PORT3, 3, 0, 0, 0);
  GPIOIntEnable(PORT3, 3);

  // Set SPI752 RXDx interrupt Enable.
  //rd = SPI752_RegRead(1, SPI752_IER_RW);  
  //SPI752_RegWrite(Channel, SPI752_IER_RW, rd | 0x01);
  SPI752_RegWrite(Channel, SPI752_IER_RW, 0x01);

  SPI752_RegRead(Channel, SPI752_RHR_R);
}


/*******************************************************************************
* Function Name  : SPI752_PutChar
* Description    : Use SPI572 channel 0 & 1 send a byte.
* Input          : - Channel : 0 & 1.
*                  - Ch : 8bit data.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI752_PutChar(uint8_t Channel, uint8_t Ch)
{
  while(!(SPI752_RegRead(Channel, SPI752_LSR_R)&0x20));
  SPI752_RegWrite(Channel, SPI752_THR_W, Ch);
}

void RS485_PutChar(uint8_t Ch)
{
  SPI752_PutChar(0, Ch);
}

void RS485_PutString(uint8_t *pStr)
{
  while(*pStr)
  {
    SPI752_PutChar(0, *pStr);
	pStr ++;
  }
}

void ZigBee_PutChar(uint8_t Ch)
{
  SPI752_PutChar(1, Ch);
}

void ZigBee_PutString(uint8_t *pStr)
{
  while(*pStr)
  {
    SPI752_PutChar(1, *pStr);
	pStr ++;
  }
}

/*******************************************************************************
* Function Name  : SPI752_GetChar
* Description    : Use SPI572 channel 0 & 1 receive a byte.
* Input          : - Channel : 0 & 1.
*                  - *Ch : 8bit data.
* Output         : None
* Return         : - 0, none receive
*                  - 1, get a byte
*******************************************************************************/
uint8_t SPI752_GetChar(uint8_t Channel, uint8_t *Ch)
{
  if(Channel == 0)
  {
    if(SPI752_rbuf_0_op != SPI752_rbuf_0_ip)
	{
	  *Ch = SPI752_rbuf_0[SPI752_rbuf_0_op];
	  SPI752_rbuf_0_op ++;
	  if(SPI752_rbuf_0_op >= SPI752_RBUF_0_NUMB)
	    SPI752_rbuf_0_op = 0;
	  return 1;
	}
  }
  else
  {
    if(SPI752_rbuf_1_op != SPI752_rbuf_1_ip)
	{
	  *Ch = SPI752_rbuf_1[SPI752_rbuf_1_op];
	  SPI752_rbuf_1_op ++;
	  if(SPI752_rbuf_1_op >= SPI752_RBUF_1_NUMB)
	    SPI752_rbuf_1_op = 0;
	  return 1;
	}
  }

  return 0;
}

uint8_t RS485_GetChar(uint8_t *Ch)
{
  return SPI752_GetChar(0, Ch);
}

uint8_t ZigBee_GetChar(uint8_t *Ch)
{
  return SPI752_GetChar(1, Ch);
}


/*******************************************************************************
* Function Name  : SPI_UART_Test
* Description    : SPI to UART test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_UART_Test(void)
{
  uint8_t	rd=rd;
   
  OLED_ClearScreen();
  OLED_DisStrLine(0, 0, "SPI-UART");
  printf("\r\nSPI-UART\r\n");
  
  SPI_IOConfig(1);
  SPI_Init(1, 8, 2);

  SPI752_Init(0, 115200);
  SPI752_Init(1, 115200);

  while(KEY_Read() != KEY_ESC)
  {	
	SPI752_PutChar(0, 0x55);

	ZigBee_PutChar(0x55);
	ZigBee_PutChar(0xaa);
		
	delay_ms(200);

	while(ZigBee_GetChar(&rd))
	{
	  printf("%02X ", rd);
	}
  }
}

/*******************************************************************************
* Function Name  : ZigBee_Menu_01
* Description    : ZigBee test Menu.
* Input          : - numb, Function number
* Output         : None
* Return         : None
*******************************************************************************/
void ZigBee_Menu_01(uint32_t numb)
{
  OLED_ClearScreen();

  if((numb>=1) && (numb<=6))
  {
    OLED_DisStrLine(0, 0, "ZigBee");
  
    if(numb == 1)
      OLED_DisStrLine(2-1 + 1, 1, "1.Send: Hello ZigBee!");
    else
  	  OLED_DisStrLine(2-1 + 1, 0, "1.Send: Hello ZigBee!");
  
    if(numb == 2)
      OLED_DisStrLine(2-1 + 2, 1, "2.Clear screen");
    else
  	  OLED_DisStrLine(2-1 + 2, 0, "2.Clear screen");
  }

  printf("\r\nZigBee\r\n");
}

/*******************************************************************************
* Function Name  : ZigBee_Test
* Description    : ZigBee test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ZigBee_Test(void)
{
  uint8_t	rd;
  uint16_t	menu_numb, key, x;

  
  SPI_IOConfig(1);
  SPI_Init(1, 8, 2);

  SPI752_Init(1, 115200);

  x = 0;
  menu_numb = 1;
  ZigBee_Menu_01(menu_numb);

  delay_ms(250);
  
  while(menu_numb)
  {
	if(UART0_GetChar(&rd))
    {
	  ZigBee_PutChar(rd);
	  //UART0_PutChar(rd);
    }

	if(ZigBee_GetChar(&rd))
    {
	  OLED_DisChar(2-1 + 6, x, 0, rd);
	  x += 6;
	  if(x > 120)
	  {
	    //OLED_DisStrLine(2-1 + 6, 0, "                     ");
	    x = 0;
	  }
	  UART0_PutChar(rd);
    }

    key = KEY_Read();

    switch(key)
	{
	  case KEY_UP:
	    if(menu_numb > 1)
		  menu_numb --;
		else
		  menu_numb = 2;
		ZigBee_Menu_01(menu_numb);
		break;

	  case KEY_DOWN:	    
		if(menu_numb < 2)
		  menu_numb ++;
		else
		  menu_numb = 1;
		ZigBee_Menu_01(menu_numb);
		break;

	  case KEY_SEL:
	    switch(menu_numb)
		{
		  case 1:		    
			ZigBee_PutString("Hello ZigBee! ");
			printf("Send: Hello ZigBee! ");
		    break;

		  case 2:		    
			OLED_DisStrLine(2-1 + 6, 0, "                     ");
		    break;

		  default:
		    break;
		}
	    break;
	  
	  case KEY_ESC:	    	    
        menu_numb = 0;		
	    break;
		
	  default:
	    break; 
	}

	if((key!=KEY_NONE) && (menu_numb!=0))
	{
	  delay_ms(250);
	}
  }
}

/*******************************************************************************
* Function Name  : RS485_Menu_01
* Description    : RS485 test Menu.
* Input          : - numb, Function number
* Output         : None
* Return         : None
*******************************************************************************/
void RS485_Menu_01(uint32_t numb)
{
  OLED_ClearScreen();

  if((numb>=1) && (numb<=6))
  {
    OLED_DisStrLine(0, 0, "RS-485");
  
    if(numb == 1)
      OLED_DisStrLine(2-1 + 1, 1, "1.Send: Hello RS-485!");
    else
  	  OLED_DisStrLine(2-1 + 1, 0, "1.Send: Hello RS-485!");
  
    if(numb == 2)
      OLED_DisStrLine(2-1 + 2, 1, "2.Clear screen");
    else
  	  OLED_DisStrLine(2-1 + 2, 0, "2.Clear screen");
  }

  printf("\r\nRS-485\r\n");
}

/*******************************************************************************
* Function Name  : RS485_Test
* Description    : RS485 test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RS485_Test(void)
{
  uint8_t	rd;
  uint16_t	menu_numb, key, x;

  
  SPI_IOConfig(1);
  SPI_Init(1, 8, 2);

  SPI752_Init(0, 115200);

  x = 0;
  menu_numb = 1;
  RS485_Menu_01(menu_numb);

  delay_ms(250);
  
  while(menu_numb)
  {
	if(UART0_GetChar(&rd))
    {
	  RS485_PutChar(rd);
	  UART0_PutChar(rd);
    }

	if(RS485_GetChar(&rd))
    {
	  OLED_DisChar(2-1 + 6, x, 0, rd);
	  x += 6;
	  if(x > 120)
	  {
	    //OLED_DisStrLine(2-1 + 6, 0, "                     ");
	    x = 0;
	  }
	  UART0_PutChar(rd);
    }

    key = KEY_Read();

    switch(key)
	{
	  case KEY_UP:
	    if(menu_numb > 1)
		  menu_numb --;
		else
		  menu_numb = 2;
		RS485_Menu_01(menu_numb);
		break;

	  case KEY_DOWN:	    
		if(menu_numb < 2)
		  menu_numb ++;
		else
		  menu_numb = 1;
		RS485_Menu_01(menu_numb);
		break;

	  case KEY_SEL:
	    switch(menu_numb)
		{
		  case 1:		    
			RS485_PutString("Hello RS-485! ");
			printf("Send: Hello RS-485! ");
		    break;

		  case 2:		    
			OLED_DisStrLine(2-1 + 6, 0, "                     ");
		    break;

		  default:
		    break;
		}
	    break;
	  
	  case KEY_ESC:	    	    
        menu_numb = 0;		
	    break;
		
	  default:
	    break; 
	}

	if((key!=KEY_NONE) && (menu_numb!=0))
	{
	  delay_ms(250);
	}
  }
}


/**** End of File ****/


