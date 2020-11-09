	 /*******************************************************************************
* File:    Send_Data.c 
* Author:  Dota Design Team
* Version: V1.00
* Date:    2012.04.07
* Brief:   Send Data program body
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "LPC11xx.h"
#include "send_data.h"
#include "spi_uart.h"
/* Includes End --------------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Send_Data
* Description    : Send Data program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void send_data(char *data, uint8_t len)
{
    uint8_t i=0;
    for(i=0;i<len;i++)
    {
        ZigBee_PutChar(data[i]);
    }
}

/**** End of File ****/

