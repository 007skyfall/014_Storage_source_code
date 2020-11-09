#include "lpc11xx.h"
#include "gpio.h"
#include "key.h"
#include "type.h"

uint8_t KeyStatus;

void InitKey(void)
{
	GPIOSetDir(PORT1, 11, 0); // SW_Up
	GPIOSetDir(PORT1, 10, 0); // SW_Down 
	GPIOSetDir(PORT1, 8, 0); // SW_Left
 	GPIOSetDir(PORT1, 9, 0); // SW_Right
  	GPIOSetDir(PORT0, 3, 0); // SW_Sel 
  	GPIOSetDir(PORT1, 4, 0); // SW_Esc
	
	KeyStatus = KEY_NONE;
}

void ScanKey(void)
{
	static uint8_t keyfiltercnt[6] = {0};
	static uint8_t keyupdateflag[6] = {0};

	KeyStatus = KEY_NONE;

	if((GPIOGetValue(PORT1, 11) & 0x800) == 0) // "UP" key is pressed
	{
		keyfiltercnt[0]++;
		if(keyfiltercnt[0] > 10)
		{
			keyfiltercnt[0] = 0;
			if(keyupdateflag[0] == 0)
			{
				KeyStatus = KEY_UP;
				keyupdateflag[0] = 1;
				return ;
			}
		}
	}
	else
	{
		keyupdateflag[0] = 0;
		keyfiltercnt[0] = 0;
	}
	if((GPIOGetValue(PORT1, 10) & 0x400) == 0) // "DOWN" key is pressed 
	{
		keyfiltercnt[1]++;
		if(keyfiltercnt[1] > 10)
		{
			keyfiltercnt[1] = 0;
			if(keyupdateflag[1] == 0)
			{
				KeyStatus = KEY_DOWN;
				keyupdateflag[1] = 1;
				return ;
			}
		}
	}
	else
	{
		keyupdateflag[1] = 0;
		keyfiltercnt[1] = 0;
	}
	if((GPIOGetValue(PORT1, 8) & 0x100) == 0) // "LEFT" key is pressed 
	{
		keyfiltercnt[2]++;
		if(keyfiltercnt[2] > 10)
		{
			keyfiltercnt[2] = 0;
			if(keyupdateflag[2] == 0)
			{
				KeyStatus = KEY_LEFT;
				keyupdateflag[2] = 1;
				return ;
			}
		}
	}
	else
	{
		keyupdateflag[2] = 0;
		keyfiltercnt[2] = 0;
	}
	if((GPIOGetValue(PORT1, 9) & 0x200) == 0) // "RIGHT" key is pressed 
	{
		keyfiltercnt[3]++;
		if(keyfiltercnt[3] > 10)
		{
			keyfiltercnt[3] = 0;
			if(keyupdateflag[3] == 0)
			{
				KeyStatus = KEY_RIGHT;
				keyupdateflag[3] = 1;
				return ;
			}
		}
	}
	else
	{
		keyupdateflag[3] = 0;
		keyfiltercnt[3] = 0;
	}
	if((GPIOGetValue(PORT0, 3) & 0x08) == 0) // "SEL" key is pressed 
	{
		keyfiltercnt[4]++;
		if(keyfiltercnt[4] > 10)
		{
			keyfiltercnt[4] = 0;
			if(keyupdateflag[4] == 0)
			{
				KeyStatus = KEY_SEL;
				keyupdateflag[4] = 1;
				return ;
			}
		}
	}
	else
	{
		keyupdateflag[4] = 0;
		keyfiltercnt[4] = 0;
	}
	if((GPIOGetValue(PORT1, 4) & 0x10) == 0) // "ESC" key is pressed 
	{
		keyfiltercnt[5]++;
		if(keyfiltercnt[5] > 10)
		{
			keyfiltercnt[5] = 0;
			if(keyupdateflag[5] == 0)
			{
				KeyStatus = KEY_ESC;
				keyupdateflag[5] = 1;
				return ;
			}
		}
	}
	else
	{
		keyupdateflag[5] = 0;
		keyfiltercnt[5] = 0;
	}
}
