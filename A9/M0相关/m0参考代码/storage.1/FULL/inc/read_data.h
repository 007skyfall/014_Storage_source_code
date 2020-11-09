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
	uint8_t head[3];	 //��ʶλst:
	uint8_t type;		 //��������
	uint8_t snum;		 //�ֿ���
	uint8_t temp[2];	 //�¶�
	uint8_t hum[2];		 //ʪ��
	int8_t x;			 //������Ϣ
	int8_t y;			 
	int8_t z;			 
	uint32_t lex;		 //����
	uint32_t bet;		 //��ص���
	uint32_t adc; 		 //��λ����Ϣ
};

typedef struct 
{
    uint8_t type;
    uint8_t slave_address;
    uint8_t machine_num;
    
}message_t;
struct sendGoods
{
	uint8_t head[3];	 //��ʶλst:
	uint8_t	type;		 //��������
	uint8_t snum;		 //�ֿ���
	uint8_t io;			 //�����ֿ�
	uint8_t goodsno;	 //��Ʒ���
	uint8_t goodsnum;	 //��Ʒ����
};

typedef struct 
{
	uint8_t     	flg;      //�����  0�����  1������
	uint8_t 		rfid[6];    //�����к�
	uint32_t    	num;      	 //������TICK������
}storage_t;


uint8_t Read_Temperature_Humidity(uint8_t *,uint8_t  *);

uint32_t  Read_Light(void);

void Collect_Data(uint8_t ,uint8_t *, uint8_t *,uint32_t * ,char *);

void send_data(char *, uint8_t len);



#endif /* __TEMP_HUM_H */

/**** End of File ****/

