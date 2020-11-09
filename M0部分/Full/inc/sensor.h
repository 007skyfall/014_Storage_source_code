#ifndef __SENSOR_H 
#define __SENSOR_H

#include <stdint.h>

void InitAllSensor(void);
void ScanAllSensor(void);
void SyncData(uint8_t sto_no);

#endif
