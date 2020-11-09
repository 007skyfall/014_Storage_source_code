#include <stdint.h>
#include "type.h"
#include "timeflag.h"

uint8_t To10msFlag=FALSE;
uint8_t To10msCnt;
uint8_t To100msFlag=FALSE;
uint8_t To100msCnt;
uint8_t To1SFlag=FALSE;
uint8_t To1SCnt;

void InitTimeFlag(void)
{
	To10msFlag=FALSE;
	To10msCnt=1;
	To100msFlag=FALSE;
	To100msCnt=10;
	To1SFlag=FALSE;
	To1SCnt=100;
}	
/**
void ScanTimeCnt(void)
{
	if(To10msCnt > 0)  
		To10msCnt--;
	if(To100msCnt > 0) 
		To100msCnt--;
	if(To1SCnt > 0) 
		To1SCnt--;
}
   **/
void ScanTimeFlag(void)
{
	To10msFlag=FALSE;
	To100msFlag=FALSE;
	To1SFlag=FALSE;

	if(To10msCnt <= 0)	
	{
		To10msCnt=1;
		To10msFlag=TRUE;
	}
	if(To100msCnt <= 0)	
	{
		To100msCnt=10;	
		To100msFlag=TRUE;
	}
	if(To1SCnt <= 0)
	{
		To1SCnt=100;	
		To1SFlag=TRUE;
	}
}
