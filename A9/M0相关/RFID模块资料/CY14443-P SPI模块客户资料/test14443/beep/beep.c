#include "avr/io.h"
#include "beep.h"
#include "delay/delay.h"
void initbeep()
{
	PORTB &=~(1<<PB0);
	DDRB |= (1<<PB0);
}

void beep_on()
{
	PORTB |=(1<<PB0);
	delayms(1);
	PORTB &=~(1<<PB0);
}

void beep_ok()
{
	PORTB |=(1<<PB0);
	delayms(150);
	PORTB &=~(1<<PB0);
}

void beep_error()
{
	PORTB |=(1<<PB0);
	delayms(20);
	PORTB &=~(1<<PB0);
	delayms(20);
	PORTB |=(1<<PB0);
	delayms(20);
	PORTB &=~(1<<PB0);
	delayms(20);
	PORTB |=(1<<PB0);
	delayms(20);
	PORTB &=~(1<<PB0);	
	
}


