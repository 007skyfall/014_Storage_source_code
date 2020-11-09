#ifndef __ZIGBEE_H 
#define __ZIGBEE_H

#include <stdint.h>

uint8_t ZigbeeRecv(uint8_t storage_no);
void ZigbeeSend(char *data, uint8_t len);
void InitZigbee(void);

#endif

