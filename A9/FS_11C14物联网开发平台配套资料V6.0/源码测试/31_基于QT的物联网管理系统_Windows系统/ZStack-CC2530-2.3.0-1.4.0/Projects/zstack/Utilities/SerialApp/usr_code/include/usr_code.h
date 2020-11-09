#ifndef __USR_CODE_H
#define __USR_CODE_H
#include "hal_types.h"


void timer_init(void);
void timer_overflow(uint8 timerId, uint8 channel, uint8 channelMode);

#endif

