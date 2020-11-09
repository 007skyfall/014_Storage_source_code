 /*******************************************************************************
* File:    Read_Data.c 
* Author:  Dota Design Team
* Version: V1.00
* Date:    2012.04.07
* Brief:   Read Data program
*******************************************************************************/ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "lpc11xx.h"
#include "read_data.h"
#include "temp_hum.h"
#include "light.h"
#include "i2c.h"
#include "spi_uart.h"
#include "gpio.h"
#include "adc.h"
#include "acc.h"

/* Includes End --------------------------------------------------------------*/

/* Struct End ----------------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Read_Temperature_Humidity
* Description    : Read Temperature and Humidity program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t Read_Temperature_Humidity(uint8_t *temp, uint8_t *hum)
{
    return (Read_Temp_Hum(temp, hum));
}


/*******************************************************************************
* Function Name  : Read_Light
* Description    : Read Light program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/ 
uint32_t Read_Light(void)
{
  return (light_read());
}

/*******************************************************************************
* Function Name  : Collect_Data
* Description    : Collect Data program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/      
void Collect_Data(uint8_t snum,uint8_t *temp, uint8_t *hum,uint32_t *light,char *data)
{
        int i = 0;
	    char flag = 'e';
	    uint32_t battery = 0,adc = 0;
	    int8_t read_x= 0,read_y= 0,read_z= 0;
        i = Read_Temperature_Humidity(temp, hum);   
        light[0] = Read_Light();	 	   
	   	acc_get(&read_x, &read_y, &read_z);
		battery =  ADCRead(3);
		adc	    =  ADCRead(0);
		       
       if(i != 0)
       {
			struct sendEnv envMsg;
			envMsg.head[0] = 's';
			envMsg.head[1] = 't';
			envMsg.head[2] = ':';
			envMsg.type = flag;
			envMsg.snum = snum;
			envMsg.temp[0] = temp[0];
			envMsg.temp[1] = temp[1];
			envMsg.hum[0] = hum[0];
			envMsg.hum[1] = hum[1];
			envMsg.lex = light[0];
			envMsg.x = read_x;
			envMsg.y = read_y;
			envMsg.z = read_z;
			envMsg.bet = battery;  
			envMsg.adc = adc;       
			memcpy (data, &envMsg, 24);
       }
}
 
/**** End of File ****/

