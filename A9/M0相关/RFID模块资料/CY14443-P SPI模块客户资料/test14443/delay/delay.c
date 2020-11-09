#include "delay.h"

void delay()
{
	unsigned long a;
	for(a=0;a<6000;a++){asm("nop");}
}
void delayms(int dl)
{
	while(dl--) 
		delay();
}

