#ifndef __MYRFID_H
#define __NYRFID_H

void Rfid_Init(void);
uint8_t Rfid_Read(uint8_t *rbuf,uint8_t choice);
//uint8_t Rfid_Read(uint8_t *rbuf);
void Rfid_Write(uint8_t *wbuf);
void Rfid_Display(uint8_t len, uint8_t *rbuf);
uint8_t Rfid_Operation(uint8_t snum,uint8_t *rbuf);
void Speaker_OnOff(uint8_t value);
//void Speaker_On();
void Speaker_Error(uint8_t value);
uint32_t Read_Rfidnum(uint8_t ,uint8_t *, uint8_t *);

#endif
