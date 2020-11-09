#include "usr_code.h"
#include  "hal_mcu.h"
#include  "hal_defs.h"
#include  "hal_types.h"
#include  "hal_timer.h"
#include "OnBoard.h"

uint8 *con_ok = "timer init  ok";
uint8 *con_err = "timer init err";

volatile uint16 time_countval = 0;

extern uint8 halTimerSetCount (uint8 hwtimerid, uint32 timePerTick);
extern uint8 halTimerSetPrescale (uint8 cc2430id, uint8 prescale);
extern uint8 halTimerSetOpMode (uint8 cc2430id, uint8 opMode);
extern uint8 halTimerSetChannelMode (uint8 cc2430id, uint8 channelMode);
extern void halTimerSendCallBack (uint8 timerId, uint8 channel, uint8 channelMode);



void timer_init(void)
{
  uint8 time_config;
  //halTimerSetCount(HAL_TIMER_0, TICK_TIME);
  //halTimerSetPrescale(HAL_TIMER_0, 0x10 << 2);
  //halTimerSetOpMode(HAL_TIMER_1, HAL_TIMER_MODE_CTC);
  //halTimerSetChannelMode(HAL_TIMER_0, HAL_TIMER_CH_MODE_OVERFLOW);
  
  time_config = HalTimerConfig(OSAL_TIMER, HAL_TIMER_MODE_CTC, HAL_TIMER_CHANNEL_SINGLE, HAL_TIMER_CH_MODE_OUTPUT_COMPARE, true, timer_overflow); 
  //after the halTimerRemap , the parameter "HAL_TIMER_0" is remapped as "HAL_TIMER_3"
  
  if(time_config != HAL_TIMER_OK)
  {
    HalUARTWrite(HAL_UART_PORT_0, con_err, 20);
    return;
  }
  else
    HalUARTWrite(HAL_UART_PORT_0, con_ok, 20);
      
  
  //HalTimerInterruptEnable(OSAL_TIMER, HAL_TIMER_CH_MODE_OVERFLOW, true);
  HalTimerStart(OSAL_TIMER, TICK_TIME); 
}

void timer_overflow(uint8 timerId, uint8 channel, uint8 channelMode)
{
  if(time_countval >= 1000)
  {
    time_countval = 0;
    HAL_TOGGLE_LED1();
  }
}












