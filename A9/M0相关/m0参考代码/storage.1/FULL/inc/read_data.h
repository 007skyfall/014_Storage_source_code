/*******************************************************************************
* File:    temp_hum.h 
* Author:  FarSight Design Team
* Version: V1.00
* Date:    2011.06.21
* Brief:   Temperature & humidity head
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __READ_LIGHT_H
#define __READ_LIGHT_H

#include "lpc11xx.h"
#define storage_len     sizeof(storage_t)
struct sendEnv
{
	uint8_t head[3];	 //标识位st:
	uint8_t type;		 //数据类型
	uint8_t snum;		 //仓库编号
	uint8_t temp[2];	 //温度
	uint8_t hum[2];		 //湿度
	int8_t x;			 //三轴信息
	int8_t y;			 
	int8_t z;			 
	uint32_t lex;		 //光照
	uint32_t bet;		 //电池电量
	uint32_t adc; 		 //电位器信息
};

typedef struct 
{
    uint8_t type;
    uint8_t slave_address;
    uint8_t machine_num;
    
}message_t;
struct sendGoods
{
	uint8_t head[3];	 //标识位st:
	uint8_t	type;		 //数据类型
	uint8_t snum;		 //仓库编号
	uint8_t io;			 //进出仓库
	uint8_t goodsno;	 //商品编号
	uint8_t goodsnum;	 //商品数量
};

typedef struct 
{
	uint8_t     	flg;      //出入库  0：入库  1：出库
	uint8_t 		rfid[6];    //卡序列号
	uint32_t    	num;      	 //数量和TICK数合用
}storage_t;


uint8_t Read_Temperature_Humidity(uint8_t *,uint8_t  *);

uint32_t  Read_Light(void);

void Collect_Data(uint8_t ,uint8_t *, uint8_t *,uint32_t * ,char *);

void send_data(char *, uint8_t len);



#endif /* __TEMP_HUM_H */

/**** End of File ****/

