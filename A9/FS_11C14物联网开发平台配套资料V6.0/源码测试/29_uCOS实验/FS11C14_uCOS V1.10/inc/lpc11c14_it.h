/*******************************************************************************
* File:    lpc11c14_it.h 
* Author:  Farsight Design Team
* Version: V1.00
* Date:    2011.11.09
* Brief:   Interrupt program body
*******************************************************************************/ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LPC11C14_IT_H
#define __LPC11C14_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __LPC11C14_IT_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
