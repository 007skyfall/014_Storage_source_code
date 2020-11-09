#ifndef __FILE_OPERATION_H
#define __FILE_OPERATION_H

//void set_start_command(message_t *message);
void fill_tem(tem_t *tem,uint8_t l_tem,uint8_t h_tem);
void fill_hum(hum_t *hum,uint8_t l_hum,uint8_t h_hum);
void fill_light(light_t *lix,uint32_t light_value);
void fill_acc(acc_t *acc,uint8_t x_value,uint8_t y_value,uint8_t z_value);
void fill_adc(adc_t *adc,uint32_t ad0_value,uint32_t ad3_value);	
void fill_key(key_t *key,uint8_t k);
void fill_state(state_t *state,uint8_t led,uint8_t fan,uint8_t buzz,uint8_t seven_led);
void fill_env(env_msg_t *env_msg,tem_t t,hum_t h,acc_t ac,adc_t ad,light_t l,state_t s);
void fill_rfid(rfid_t *rfid,uint32_t uid,uint8_t *data,uint8_t *pur,uint8_t *eprom);
void fill_data(data_t *data,uint8_t datatype,rfid_t *r,command_t *c,env_msg_t *e,key_t *k);
void fill_message(message_t *message,message_tag_t ta,uint8_t slave,uint8_t length,data_t *data_come,uint16_t CRC);
void send_message_uart(message_t *message);
void send_message_zigbee(message_t *message);


#endif
