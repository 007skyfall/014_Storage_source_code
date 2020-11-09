#ifndef _CONFIG_H_
#define _CONFIG_H_	

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
	
#define u8 unsigned char
#define u16 unsigned int
#define u32 unsigned long
			
#define U8 unsigned char
#define U16 unsigned int
#define U32 unsigned long	

	
/***********************************************************/
//for newstatic			

typedef unsigned char	BYTE;				// 8-bit
typedef unsigned int	WORD;				// 16-bit
typedef unsigned long	DWORD;				// 32-bit

// define I/O interface

#define LED_PIN1_DDR			DDC6
#define LED_PIN1				PC6
#define LED_PIN2_DDR			DDC7
#define LED_PIN2				PC7
#define LED_PORT				PORTC
#define LED_DDR					DDRC

#define LOW(uint) (uint&0xFF)
#define HIGH(uint) ((uint>>8)&0xFF)

#endif //_CONFIG_H


