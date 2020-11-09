#ifndef KEY_H_
#define KEY_H_

#include "config.h"
#include "usart.h"


#ifndef DEBUG
#define DEBUG usart_write
//#define DEBUG(...)

#endif
 
#define KBIN_LOAD()  PORTD |= (1<<PD4)
#define KBIN_UNLOAD()  PORTD &= ~(1<<PD4)

#define KBIN_DATA_ISSET()  (PIND&(1<<PD6))==(1<<PD6)

#define KBIN_CK_H()  PORTD |= (1<<PD5)
#define KBIN_CK_L()  PORTD &= ~(1<<PD5)


#define K_1 1
#define K_2 2
#define K_3 3

#define K_4 4
#define K_5 5
#define K_6 6

#define K_7 7
#define K_8 8
#define K_9 9
#define K_0 0

//*
#define K_E 11
//#
#define K_H 14

#define K_C 12
#define K_OK 13

#define K_UP 15
#define K_DN 16

void keyinit();
u8 getkey();
u16 getinput();  //获得8个输入


#endif
