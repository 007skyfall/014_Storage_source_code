/*****************************************************************************
 *   light.h:  Header file for ISL29003 Light-to-Digital Output Sensor
 *
 *   Copyright(C) 2009, Embedded Artists AB
 *   All rights reserved.
 *
******************************************************************************/
#ifndef __ACC_H
#define __ACC_H

#include <stdint.h>

struct _XYZ
{
	int8_t Val;		/*�Ĵ�����ԭʼֵ*/
	float Acc;		/*���ٶ�ֵ*/
	float Angle; 	/*�Ƕ�ֵ*/
};

typedef struct
{
	struct _XYZ X;
	struct _XYZ Y;
	struct _XYZ Z;
} AccData;

typedef enum
{
    ACC_MODE_STANDBY,
    ACC_MODE_MEASURE,
    ACC_MODE_LEVEL, /* level detection */
    ACC_MODE_PULSE, /* pulse detection */
} acc_mode_t;

typedef enum
{
    ACC_RANGE_8G,
    ACC_RANGE_2G,
    ACC_RANGE_4G,
} acc_range_t;

void acc_init (void);
void acc_read (int8_t *x, int8_t *y, int8_t *z);
void acc_setRange(acc_range_t range);
void acc_setMode(acc_mode_t mode);

void ScanAxis3(void);
void InitAxis3(void);

#endif /* end __ACC_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/
