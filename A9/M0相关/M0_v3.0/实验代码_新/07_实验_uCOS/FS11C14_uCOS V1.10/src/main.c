/*******************************************************************************
* File:    main.c 
* Author:  Leo
* Version: V1.10
* Date:    2011.11.11
* Brief:   Main program body
********************************************************************************
********************************************************************************
* History:
* 2011.06.10:  V1.00		   initial version
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "ucos_ii.h"
#include "lpc11xx.h"
#include "gpio.h"
#include "uart.h"
#include "speaker_op.h"

#include "seg7led.h"
#include "key.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* External global variables -------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//__align(4) uint32_t	iap_test=0;

/* External functions --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* uC/OS-II ------------------------------------------------------------------*/
#define  Task_Start_PRIO		2
#define  Task_Led_PRIO			13
#define  Task_Seg7Led_PRIO		12
#define  Task_Key_PRIO			11
#define  Task_BeepFan_PRIO		15
#define  Task_Monitor_PRIO		OS_LOWEST_PRIO-3

#define  Task_Start_SS			64
#define  Task_Led_SS			64
#define  Task_Seg7Led_SS		64
#define  Task_Key_SS			64
#define  Task_BeepFan_SS		64
#define  Task_Monitor_SS		128

OS_STK  Task_Start_STK[Task_Start_SS];
OS_STK  Task_Led_STK[Task_Led_SS];
OS_STK  Task_Seg7Led_STK[Task_Seg7Led_SS];
OS_STK  Task_Key_STK[Task_Key_SS];
OS_STK  Task_BeepFan_STK[Task_BeepFan_SS];
OS_STK  Task_Monitor_STK[Task_Monitor_SS];

OS_EVENT    	*Key_Sem;
OS_EVENT		*Led_Mbox;


void Task_Start(void *pdata);
void Task_Led(void *pdata);
void Task_Seg7Led(void *pdata);
void Task_Key(void *pdata);
void Task_BeepFan(void *pdata);
void Task_Monitor(void *pdata);


/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
  SystemInit();

  GPIOInit();						// Enables clock for GPIO

  // LED
  GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
  GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, close LED1

  GPIOSetDir(PORT3, 1, 1);			// Set PIO3_1 to output
  GPIOSetValue(PORT3, 1, 1);		// PIO1_1 output 1, close LED2

  // Fan
  GPIOSetDir(PORT0, 2, 1);			// Set PIO0_2 to output    
  GPIOSetValue(PORT0, 2, 1);		// PIO0_2 output 0, turn off FAN

  // Speaker
  LPC_IOCON->R_PIO1_1 &= ~0x07;
  LPC_IOCON->R_PIO1_1 |= 0x01;
  speaker_op(0);


  // 7 Segment LED
  Seg7Led_Init();
  Seg7Led_Put(' ');					// Output ' '

  KEY_Init();
 
  GPIOSetValue(PORT3, 0, 0);		// PIO3_0 output 0, Turn on LED1
  GPIOSetValue(PORT3, 1, 0);		// PIO3_1 output 0, Turn on LED2
  //delay_ms(200);
  GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off LED1
  GPIOSetValue(PORT3, 1, 1);		// PIO3_1 output 1, Turn off LED2


  UARTInit(115200);
  NVIC_SetPriority(UART_IRQn, 1);

  printf("\r\n\r\nFS_11C14 uC/OS-II\r\n");


  //////////////////////////////////////////////////////////////////////////////
  // uC/OS-II Start
  //////////////////////////////////////////////////////////////////////////////
  OSInit();

  Key_Sem = OSSemCreate(0);
  Led_Mbox = OSMboxCreate((void *)0);

  OSTaskCreateExt(Task_Start, 
                  (void *)0, 
				  &Task_Start_STK[Task_Start_SS-1], 
				  Task_Start_PRIO, 
				  Task_Start_PRIO,	
				  &Task_Start_STK[0], 
				  Task_Start_SS,
				  (void *)0, 
				  OS_TASK_OPT_STK_CHK + OS_TASK_OPT_STK_CLR);

  OSTaskCreateExt(Task_Led,
  				  (void *)0, 
                  &Task_Led_STK[Task_Led_SS-1], 
				  Task_Led_PRIO, 
	              Task_Led_PRIO, 
				  &Task_Led_STK[0], 
				  Task_Led_SS, 
				  (void *)0, 
	              OS_TASK_OPT_STK_CHK + OS_TASK_OPT_STK_CLR);

  OSTaskCreateExt(Task_Seg7Led,
  			      (void *)0, 
                  &Task_Seg7Led_STK[Task_Seg7Led_SS-1], 
				  Task_Seg7Led_PRIO, 
	              Task_Seg7Led_PRIO, 
				  &Task_Seg7Led_STK[0], 
				  Task_Seg7Led_SS, 
				  (void *)0, 
	              OS_TASK_OPT_STK_CHK + OS_TASK_OPT_STK_CLR);

  OSTaskCreateExt(Task_Key,
  			      (void *)0, 
                  &Task_Key_STK[Task_Key_SS-1], 
				  Task_Key_PRIO, 
	              Task_Key_PRIO, 
				  &Task_Key_STK[0], 
				  Task_Key_SS, 
				  (void *)0, 
	              OS_TASK_OPT_STK_CHK + OS_TASK_OPT_STK_CLR);

  OSTaskCreateExt(Task_BeepFan,
  			      (void *)0, 
                  &Task_BeepFan_STK[Task_BeepFan_SS-1], 
				  Task_BeepFan_PRIO, 
	              Task_BeepFan_PRIO, 
				  &Task_BeepFan_STK[0], 
				  Task_BeepFan_SS, 
				  (void *)0, 
	              OS_TASK_OPT_STK_CHK + OS_TASK_OPT_STK_CLR);

  OSTaskCreateExt(Task_Monitor, 
  				  (void *)0, 
                  &Task_Monitor_STK[Task_Monitor_SS-1], 
				  Task_Monitor_PRIO, 
	              Task_Monitor_PRIO, 
				  &Task_Monitor_STK[0], 
				  Task_Monitor_SS, 
				  (void *)0, 
	              OS_TASK_OPT_STK_CHK + OS_TASK_OPT_STK_CLR);

  OSStart();
  //////////////////////////////////////////////////////////////////////////////
  // uC/OS-II End
  //////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////
// 开始任务
// 只用于uC/OS-II系统启动(开启系统时钟,统计任务初始化)
// 开始任务要开启系统时钟，所以需要设置为最高优先级
/////////////////////////////////////////////////////////////////////////////////
void Task_Start(void *pdata)
{
	OS_CPU_SysTickInit();
	OSStatInit();				// uC/OS-II 统计任务初始化
	OSTaskDel(OS_PRIO_SELF);	// 初始化以上内容后删除本任务，以减少任务调度时间
}

void Task_Led(void *pdata)
{
	uint32_t	cnt=0;

	while(1)
	{
		GPIOSetValue(PORT3, 0, 0);		// PIO3_0 output 0, Turn on LED1
        GPIOSetValue(PORT3, 1, 0);		// PIO3_1 output 0, Turn on LED2

		// 广播方式发送计数值指针(Mbox)
		OSMboxPostOpt(Led_Mbox, (void *)&cnt, OS_POST_OPT_BROADCAST);
		cnt ++;

        OSTimeDlyHMSM(0, 0, 0, 500);

        GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off LED1
        GPIOSetValue(PORT3, 1, 1);		// PIO3_1 output 1, Turn off LED2
						
		OSTimeDlyHMSM(0, 0, 0, 500);		
	}
}

void Task_Seg7Led(void *pdata)
{
	uint8_t		err;
	uint32_t	*led_cnt;

	while(1)
	{
		led_cnt = (uint32_t *)OSMboxPend(Led_Mbox, 0, &err);
		Seg7Led_Put((*led_cnt)%10);
	}
}

void Task_Key(void *pdata)
{
	while(1)
	{
		if((KEY_Read()==KEY_SEL) || (KEY_Read()==KEY_ESC))
		{
			OSSemPost(Key_Sem);
		}
		OSTimeDlyHMSM(0, 0, 0, 300);
	}
}

void Task_BeepFan(void *pdata)
{
	uint8_t		on_off=0;
	uint8_t		err;

	while(1)
	{
		OSSemPend(Key_Sem, 0, &err);

		on_off = ~on_off;

		if(on_off == 0)
		{
			GPIOSetValue(PORT0, 2, 1);		// PIO0_2 output 0, turn off FAN
			speaker_op(0);		// PIO1_1 output 1,	turn off Speaker 	
		}
		else
		{
			GPIOSetValue(PORT0, 2, 0);		// PIO0_2 output 0, turn on FAN
			speaker_op(1);		// PIO1_1 output 1,	turn on Speaker 
		}
	}
}

void Task_Monitor(void *pdata)
{
	OS_STK_DATA		stk;
	uint32_t		lu0;


	while(1)
	{
		lu0 = OSTimeGet() / OS_TICKS_PER_SEC;
		printf("\r\n\r\nSecond: %d,  MCU-Used: %d%c", lu0, OSCPUUsage, '%');

		printf("\r\n---------------------------");

		OSTaskStkChk(OS_LOWEST_PRIO-1, &stk);
		printf("\r\nStat:    %4u -%4u =%4u",
		            stk.OSUsed/4+stk.OSFree/4, stk.OSUsed/4, stk.OSFree/4);

		OSTaskStkChk(OS_LOWEST_PRIO, &stk);
		printf("\r\nIdle:    %4u -%4u =%4u",
		            stk.OSUsed/4+stk.OSFree/4, stk.OSUsed/4, stk.OSFree/4);

		OSTaskStkChk(Task_Led_PRIO, &stk);
		printf("\r\nLed:     %4u -%4u =%4u",
		            stk.OSUsed/4+stk.OSFree/4, stk.OSUsed/4, stk.OSFree/4);

		OSTaskStkChk(Task_Seg7Led_PRIO, &stk);
		printf("\r\nSeg7Led: %4u -%4u =%4u",
		            stk.OSUsed/4+stk.OSFree/4, stk.OSUsed/4, stk.OSFree/4);

		OSTaskStkChk(Task_Key_PRIO, &stk);
		printf("\r\nKey:     %4u -%4u =%4u",
		            stk.OSUsed/4+stk.OSFree/4, stk.OSUsed/4, stk.OSFree/4);

		OSTaskStkChk(Task_BeepFan_PRIO, &stk);
		printf("\r\nBeepFan: %4u -%4u =%4u",
		            stk.OSUsed/4+stk.OSFree/4, stk.OSUsed/4, stk.OSFree/4);

		OSTaskStkChk(Task_Monitor_PRIO, &stk);
		printf("\r\nMonitor: %4u -%4u =%4u",
		            stk.OSUsed/4+stk.OSFree/4, stk.OSUsed/4, stk.OSFree/4);
		
		OSTimeDlyHMSM(0, 0, 1, 0);
	}
}
 

/**** End of File ****/
