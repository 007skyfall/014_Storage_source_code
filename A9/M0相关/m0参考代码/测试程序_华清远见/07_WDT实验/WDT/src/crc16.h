
/****************************************************************************
 *   $Id:: rcr16.h 4085 2010-07-30 22:31:04Z usb00423                         $
 *   Project: NXP LPC11xx ADC example
 *
 *   Description:
 *     This file contains ADC code header definition.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#ifndef __CRC16_H 
#define __CRC16_H

extern uint16_t make_crc16(uint16_t crc, uint8_t *buffer, uint32_t len);

#endif /* end  */
/*****************************************************************************
**                            End Of File
******************************************************************************/
