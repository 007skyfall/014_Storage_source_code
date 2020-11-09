/*
  the declearation of some functions of rfid
*/
#ifndef __MY_RFID_H__ 
#define __MY_RFID_H__ 

void Rfid_Init(void);

//uint8_t Rfid_Read(uint8_t *rbuf);

void Rfid_Display(uint8_t len, uint8_t *rbuf);

//uint8_t Rfid_Operation(uint8_t snum, char *sbuf, uint8_t *rbuf);

void Rfid_Operation(uint8_t *rbuf);

void Speaker_OnOff(uint8_t value);

void Speaker_OnOffsw(uint8_t value);

uint32_t Read_Rfidnum(uint8_t ,char *, uint8_t *);

void delay(uint8_t num);
uint8_t Rfid_Read(uint8_t *rbuf,uint8_t *tbuf);


#endif
