#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_type.h"
#include "file_operation.h"
#include "collect_data.h"

uint8_t buffer[60];
/*
void set_start_command(message_t *message)	 		//ÉèÖÃ¿ª»úÃüÁî
{
	message->data.command.operate_id = STORAGE_NUM;
	message->data.command.operation = START;
}

  */
/*****************************************************************************
** Function name:		fill_tem
**
** Descriptions:		fill the structure	tem_t
**
** parameters:			l_tem--the low byte of temperature,
						h_tem--the high byte of temperature
** Returned value:		None
** 
*****************************************************************************/

void fill_tem(tem_t *tem,uint8_t l_tem,uint8_t h_tem)
{
	tem->ltem = l_tem;
	tem->htem = h_tem;	
}

/*****************************************************************************
** Function name:		fill_hum
**
** Descriptions:		fill the structure	hum_t
**
** parameters:			l_hum--the low byte of humidity,
						h_hum--the high byte of humidity
** Returned value:		None
** 
*****************************************************************************/

void fill_hum(hum_t *hum,uint8_t l_hum,uint8_t h_hum)
{
	hum->lhum = l_hum;
	hum->hhum = h_hum;
}

/*****************************************************************************
** Function name:		fill_light
**
** Descriptions:		fill the structure	light_t
**
** parameters:			light_value---the value reading from the light sensor
** Returned value:		None
** 
*****************************************************************************/

void fill_light(light_t *lix,uint32_t light_value)
{
	lix->light = light_value;
}

/*****************************************************************************
** Function name:		fill_acc
**
** Descriptions:		fill the structure	acc_t
**
** parameters:			x_value--the value of the x axis,
						y_value--the value of the y axis,
						z_value--the value of the z axis
** Returned value:		None
** 
*****************************************************************************/

void fill_acc(acc_t *acc,uint8_t x_value,uint8_t y_value,uint8_t z_value)
{
	acc->x = x_value;
	acc->y = y_value;
	acc->z = z_value;
}

/*****************************************************************************
** Function name:		fill_adc
**
** Descriptions:		fill the structure	adc_t
**
** parameters:			ad0_value--the value of the A/D conversion,
						ad3_value--the value of the VBAT,
						z_value--the value of the z axis
** Returned value:		None
** 
*****************************************************************************/

void fill_adc(adc_t *adc,uint32_t ad0_value,uint32_t ad3_value)
{
	adc->ad0 = ad0_value;
	adc->ad3 = ad3_value; 
}

/*****************************************************************************
** Function name:		fill_key
**
** Descriptions:		fill the structure	key_t
**
**key_t:  a structure that describe the key pressed by user
**
** parameters:			k:the key value 
** Returned value:		None
** 
*****************************************************************************/

void fill_key(key_t *key,uint8_t k)
{
	key->key_all = k;
}

/*****************************************************************************
** Function name:		fill_state
**
** Descriptions:		fill the structure	state_t
**						the state_t structure describe the state of the machines of the LPC_11C14
**
** parameters:			led--the state of led1,led2,
						fan--the state of the fan,
						buzz--the state of the buzz
						seven_led--the state of the seven_led
** Returned value:		None
** 
*****************************************************************************/

void fill_state(state_t *state,uint8_t led,uint8_t fan,uint8_t buzz,uint8_t seven_led)
{
	state->led_state = led;
	state->fan_state = fan;
	state->buzz_state = buzz;
	state->seven_led_state = seven_led;
}

/*****************************************************************************
** Function name:		fill_rfid
**
** Descriptions:		fill the structure	rfid_t
**
** parameters:			uid:  the id of the card
						data:  the data of the block in card
						pur:  the value of the purse in card
						eprom: the value of the eeprom in card
** Returned value:		None
** 
*****************************************************************************/

void fill_rfid(rfid_t *rfid,uint32_t uid,uint8_t *data,uint8_t *pur,uint8_t *eprom)
{
	rfid->id = uid;
	memcpy(rfid->datablock,data,16);
	memcpy(rfid->purse,pur,4);
	memcpy(rfid->eeprom,eprom,4);
}

/*****************************************************************************
** Function name:		fill_env
**
** Descriptions:		fill the structure	env_msg_t
**
** parameters:			h :  structure used to store the humidity
						t :  structure used to store the tempetature
						ac:  structure used to store the aixs sensor
						ac:  structure used to store the A/D
						s :  structure used to store the state of machine
** Returned value:		None
** 
*****************************************************************************/

void fill_env(env_msg_t *env_msg,tem_t t,hum_t h,acc_t ac,adc_t ad,light_t l,state_t s)
{
	memcpy(&env_msg->tem,&t,tem_len);
	memcpy(&env_msg->hum,&h,hum_len);
	memcpy(&env_msg->acc,&ac,acc_len);
	memcpy(&env_msg->adc,&ad,adc_len);
	memcpy(&env_msg->light,&l,light_len);
	memcpy(&env_msg->state,&s,state_len);
}

/*****************************************************************************
** Function name:		fill_data
**
** Descriptions:		fill the structure	data_t
**
** parameters:			datatype :  the type of message want to send
						r 		 :  rfid message
						c        :  command message
						k		 :  key message
** Returned value:		None
** 
*****************************************************************************/

void fill_data(data_t *data,uint8_t datatype,rfid_t *r,command_t *c,env_msg_t *e,key_t *k)
{
	switch(datatype)
	{
		case 'r':
			memcpy(&data->rfid,r,rfid_len);
		break;
		case 'c':
			memcpy(&data->command,c,command_len);
		break;
		case 'e':
			memcpy(&data->env_msg,e,env_len);
		break;
		case 'k':
			memcpy(&data->key,k,key_len);
		break;
		default:
		break;
	}	
}

/*****************************************************************************
** Function name:		fill_message
**
** Descriptions:		fill the structure	message_t ,used to send message
**
** parameters:			ta 		:  the tag of message 
						slave 	:  decide which machine the message will be sent to 
						length  :  length of message
						data	:  the data field of message
** Returned value:		None
** 
*****************************************************************************/

void fill_message(message_t *message,message_tag_t ta,uint8_t slave,uint8_t length,data_t *data_come,uint16_t CRC)
{
	message->tag = ta;
	message->slave_address = slave;
	message->data_length = length;
	memcpy(&message->data,data_come,data_len);
	message->crc = CRC;	
}

/*****************************************************************************
** Function name:		send_message_uart
**
** Descriptions:		use UART to send message,then you can get the message from UART buffer
**
** parameters:			message : message to be sent
** Returned value:		None
** 
*****************************************************************************/

void send_message_uart(message_t *message)
{
	memcpy(buffer,message,message_len);
	send_data_uart(buffer,message_len);	
}

/*****************************************************************************
** Function name:		send_message_zigbee
**
** Descriptions:		use ZIGBEE to send message
**
** parameters:			message : message to be sent
** Returned value:		None
** 
*****************************************************************************/

void send_message_zigbee(message_t *message)
{
	memcpy(buffer,message,message_len);
	send_data(buffer,message_len);
}


  


























































