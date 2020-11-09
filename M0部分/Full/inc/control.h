#ifndef __CONTROL_H 
#define __CONTROL_H

#include <stdint.h>

void DealA8Cmd(uint8_t sto_no);
void SendEnvMsg(uint8_t sto_no);
void Control(uint8_t sto_no);
void InitControl(uint8_t *title);

#endif
