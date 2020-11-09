#include "lpc11xx.h"
#include "type.h"
#include "adc.h"
#include "myadc.h"

uint32_t Adc; // 全局变量
uint32_t Bat;
uint8_t AdcGetFinishFlag; // 全局标志

void ScanMyAdc(void)
{
	static uint8_t AdcFilterCnt = 0;
	static uint32_t adcsum = 0;
	static uint32_t batsum = 0;
	uint32_t tadc,tbat;

	tadc = ADCRead(0);
	tbat = ADCRead(3);
	adcsum += tadc;
	batsum += tbat;

	AdcFilterCnt++;
	if(AdcFilterCnt >= 10)
	{
		AdcFilterCnt = 0;
		Adc = adcsum / 10;
		Bat = batsum / 10;
		adcsum = 0;
		batsum = 0;
		AdcGetFinishFlag = TRUE;
	}
}

void InitMyAdc(void)
{
	ADCInit(ADC_CLK);
	AdcGetFinishFlag = FALSE;
	Adc = 0;
	Bat = 0;
}	

