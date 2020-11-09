
#ifndef _UART_H
	#define _UART_H

	#define USART_ECHO	1
	
	#define BUFFER_SIZE	50
	
	volatile unsigned char buffercounter;
	char usart_rx_buffer[BUFFER_SIZE];
	
	char *rx_buffer_pointer_in;
	char *rx_buffer_pointer_out;
	
	struct {
			volatile unsigned char usart_ready:1;
			volatile unsigned char usart_rx_ovl:1;
			volatile unsigned char usart_disable:1; //benötigt für ftp2com
			}usart_status ;
	
	//----------------------------------------------------------------------------
	
	#include <avr/interrupt.h>
	#include <avr/pgmspace.h>
	#include <stdlib.h>
	#include <stdarg.h>
	#include <ctype.h>
	#include <string.h>
	#include "config.h"

	
	#if defined (__AVR_ATmega32__)
		#define USR UCSRA
		#define UCR UCSRB
		#define UBRR UBRRL
		#define EICR EICRB
		#define USART_RX USART_RXC_vect  
	#endif
	//----------------------------------------------------------------------------
	
	void usart_init(unsigned int baudrate); 
	unsigned char getc();
	void usart_write_char(char c);
	void usart_write_str(char *str);
	
	void usart_write_P (const char *Buffer,...);
	#define usart_write(format, args...)   usart_write_P(PSTR(format) , ## args)
	
	//----------------------------------------------------------------------------

#endif //_UART_H
