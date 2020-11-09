/*
*********************************************************************************************************
*                                               uC/OS-II
*                                  Philips LPC2xxx ARM7-TDMI(S) Core
*
*
*                                 (c) Copyright 2005-2007, Arm Design
*                                          All Rights Reserved
*
*                                              App Config
*
* File      : APP_CFG.H
* Version   : V3.1
* By        : Old Arm
*
* For       : ARM7 or ARM9
* Mode      : Philips LPC2xxx
* Toolchain : ARM ADS V1.2
*********************************************************************************************************
*/

#ifndef  APP_CFG_H
#define  APP_CFG_H

#include "os_cfg.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TASK PRIORITIES
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if OS_TMR_EN > 0
#define  OS_TASK_TMR_PRIO		 200
#endif


#endif

