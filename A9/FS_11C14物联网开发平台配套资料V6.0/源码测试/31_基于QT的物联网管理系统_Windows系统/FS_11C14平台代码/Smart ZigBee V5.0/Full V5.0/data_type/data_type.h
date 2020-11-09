#ifndef __DATA_TYPE_H
#define __DATA_TYPE_H
#include <stdint.h>
#include <stdlib.h>
 
#define 	START 		'S'
#define 	BROADCAST   0xFF
/*---------------the definition of machine_id------*/
#define buzz_id 		1
#define fan_id 			2
#define seven_led_id 	3
#define led1_id 		4
#define led2_id 		5
/*---------------end of machine_id-----------------*/


/*---------------definition of tag-----------------*/
#define  start_machine_t 0xAA	 //开机
#define  data_flow_t 	 0xBB	 //数据采集
#define  rfid_msg_t      0xCC	 //rfid信息
#define  command_tag_t	 0xDD	 //命令
#define  key_msg_t 		 0xEE    //按键
#define  other_type_t	 0x00	 //其他（未定义）
/*---------------end of tag------------------------*/

/*---------------THE DEFINITION OF STATE-----------*/
#define led1_on 		1 << 0
#define led1_off		0 << 0
#define led2_on 		1 << 1
#define led2_off 		1 << 0
#define fan_on 			1 << 0
#define fan_off 		0 << 0
#define fan_low 		1 << 1
#define fan_mid 		1 << 2
#define fan_high 		1 << 3				
#define speaker_on 		1
#define speaker_off 	0
#define seven_led_on 	1
#define seven_led_off 	0
/*--------------END OF STATE------------------------*/

/*------------definition of command-----------------*/
#define on_led1 		0
#define off_led1		1
#define on_speaker 		2
#define off_speaker 	3
#define on_fan 			4
#define on_fan_low 		5
#define on_fan_mid 		6
#define on_fan_high 	7
#define off_fan 		8
#define on_seven_led    9
#define off_seven_led	10
#define off_machine		11
/*--------------end of command---------------------*/



/*--------------definition of key------------------*/
#define key_up	  		0
#define key_down  		1
#define key_left  		2
#define key_right 		3
#define key_sel   		4
#define key_esc   		5
/*----------------end of key----------------------*/


/*----------------definition of datatype-----------*/
#define data_r 			'r'			//发送RFID信息
#define data_c 			'c'			//接收命令信息
#define data_e 			'e'			//发送环境信息
#define data_k 			'k'			//发送按键信息
/*----------------end of datatype------------------*/

/*------------------definition of length-----------*/
#define tem_len    		sizeof(tem_t)
#define hum_len	   		sizeof(hum_t)
#define state_len  		sizeof(state_t)
#define adc_len	   		sizeof(adc_t)
#define acc_len    		sizeof(acc_t)
#define light_len  		sizeof(light_t)
#define data_len   		sizeof(data_t)
#define rfid_len   		sizeof(rfid_t)
#define env_len    		sizeof(env_msg_t)
#define command_len 	sizeof(command_t)
#define key_len    		sizeof(key_t)
#define message_len 	sizeof(message_t)
#define message_tag_len sizeof(message_tag_t)

/*----------------end of length-------------------*/



typedef enum _message_tag_t
{
	start_machine = start_machine_t,
	data_flow	  = data_flow_t,
	rfid_msg 	  = rfid_msg_t,
	command 	  = command_tag_t,
	key 		  = key_msg_t,
	other_type	  = other_type_t
}message_tag_t;					  //标记消息类型

typedef struct _tem_t
{
	uint8_t ltem;
	uint8_t htem;
}tem_t;							//温度信息

typedef struct _hum_t			//湿度信息
{
	uint8_t lhum;
	uint8_t hhum;
}hum_t;

typedef struct _light_t			//光照信息
{
	uint32_t light;
}light_t;

typedef struct _acc_t			//三轴加速度信息
{
	int8_t x;
	int8_t y;
	int8_t z;
}acc_t;

typedef struct _adc_t		   	//A/D转换信息
{
	uint32_t ad0; 				//data from A/D channel 0
	uint32_t ad3;				//data from A/D channel 1
}adc_t;


typedef struct _state_t		 	//设备状态
{
	uint8_t led_state;
	uint8_t fan_state;
	uint8_t buzz_state;
	uint8_t seven_led_state;
}state_t;

typedef struct _rfid_t			//RFID相关信息
{
	uint32_t id;				//序列号
	uint8_t datablock[16];		//数据块
	uint8_t purse[4];			//钱包
	uint8_t eeprom[4]; 			//E2PROM值
}rfid_t;

typedef struct _command_t
{
	uint8_t operate_id;			//define the object operated
	uint8_t operation;			//define the action object should do 
}command_t;


typedef struct _key_t	   		//M0按键定义
{
	uint8_t key_all;
}key_t;

typedef struct _env_msg_t		//环境信息
{
	tem_t tem;			     	//温度
	hum_t hum;				 	//湿度
	acc_t acc;				 	//三轴加速度
	adc_t adc;				 	//A/D转换
	light_t light;			 	//光照值
	state_t state;			 	//设备状态
}env_msg_t;

typedef struct _other_type_d
{
	uint8_t other_data[20];
}other_type_d;

typedef union _data_t		 	//可选择发送的信息类型
{
	rfid_t rfid;
	command_t command;
	env_msg_t env_msg;
	key_t key;
	other_type_d other_msg;
}data_t;

typedef struct					//发送消息格式
{
	message_tag_t tag;
	uint8_t slave_address;
	uint8_t data_length;
 	data_t data;
	uint16_t crc;
}message_t;




#endif
