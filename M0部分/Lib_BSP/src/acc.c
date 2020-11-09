/*****************************************************************************
 *   acc.c:  Driver for the MMA7455 Accelerometer
 *
 *   Copyright(C) 2009, Embedded Artists AB
 *   All rights reserved.
 *
 ******************************************************************************/

/*
 * NOTE: I2C must have been initialized before calling any functions in this
 * file.
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "lpc11xx.h"
#include "type.h"
#include "i2c.h"
#include "acc.h"
#include "math.h"

#define ACC_GRAVITY		9.8   /*重力加速度的值*/

AccData Axis3; // 全局变量
uint8_t Axis3GetFinishFlag; // 全局标志

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

#define ACC_I2C_ADDR    (0x1D << 1) //实际地址0x3A

#define ACC_ADDR_XOUTL  0x00
#define ACC_ADDR_XOUTH  0x01
#define ACC_ADDR_YOUTL  0x02
#define ACC_ADDR_YOUTX  0x03
#define ACC_ADDR_ZOUTL  0x04
#define ACC_ADDR_ZOUTH  0x05
#define ACC_ADDR_XOUT8  0x06
#define ACC_ADDR_YOUT8  0x07
#define ACC_ADDR_ZOUT8  0x08
#define ACC_ADDR_STATUS 0x09
#define ACC_ADDR_DETSRC 0x0A
#define ACC_ADDR_TOUT   0x0B
#define ACC_ADDR_I2CAD  0x0D
#define ACC_ADDR_USRINF 0x0E
#define ACC_ADDR_WHOAMI 0x0F
#define ACC_ADDR_XOFFL  0x10
#define ACC_ADDR_XOFFH  0x11
#define ACC_ADDR_YOFFL  0x12
#define ACC_ADDR_YOFFH  0x13
#define ACC_ADDR_ZOFFL  0x14
#define ACC_ADDR_ZOFFH  0x15
#define ACC_ADDR_MCTL   0x16
#define ACC_ADDR_INTRST 0x17
#define ACC_ADDR_CTL1   0x18
#define ACC_ADDR_CTL2   0x19
#define ACC_ADDR_LDTH   0x1A
#define ACC_ADDR_PDTH   0x1B
#define ACC_ADDR_PW     0x1C
#define ACC_ADDR_LT     0x1D
#define ACC_ADDR_TW     0x1E

#define ACC_MCTL_MODE(m) ((m) << 0)
#define ACC_MCTL_GLVL(g) ((g) << 2)


#define ACC_STATUS_DRDY 0x01
#define ACC_STATUS_DOVR 0x02
#define ACC_STATUS_PERR 0x04


/******************************************************************************
 * External global variables
 *****************************************************************************/

/******************************************************************************
 * Local variables
 *****************************************************************************/

static uint8_t getStatus(void)
{
    uint8_t buf[1];

    buf[0] = ACC_ADDR_STATUS;
    I2CWrite(ACC_I2C_ADDR, buf, 1);
    I2CRead(ACC_I2C_ADDR, buf, 1);

    return buf[0];
}

static uint8_t getModeControl(void)
{
    uint8_t buf[1];

    buf[0] = ACC_ADDR_MCTL;
    I2CWrite(ACC_I2C_ADDR, buf, 1);
    I2CRead(ACC_I2C_ADDR, buf, 1);

    return buf[0];
}

static void setModeControl(uint8_t mctl)
{
    uint8_t buf[2];

    buf[0] = ACC_ADDR_MCTL;
    buf[1] = mctl;
    I2CWrite(ACC_I2C_ADDR, buf, 2);
}

/******************************************************************************
 * Local Functions
 *****************************************************************************/

/******************************************************************************
 * Public Functions
 *****************************************************************************/

/******************************************************************************
 *
 * Description:
 *    Initialize the ISL29003 Device
 *
 *****************************************************************************/
void acc_init (void)
{

    /* set to measurement mode by default */

    setModeControl( (ACC_MCTL_MODE(ACC_MODE_MEASURE)
            | ACC_MCTL_GLVL(ACC_RANGE_2G) ));

}

/******************************************************************************
 *
 * Description:
 *    Read accelerometer data
 *
 * Params:
 *   [out] x - read x value
 *   [out] y - read y value
 *   [out] z - read z value
 *
 *****************************************************************************/
void acc_read (int8_t *x, int8_t *y, int8_t *z)
{
    uint8_t buf[1];

    /* wait for ready flag */
    while ((getStatus() & ACC_STATUS_DRDY) == 0);

    /*
     * Have experienced problems reading all registers
     * at once. Change to reading them one-by-one.
     */
    buf[0] = ACC_ADDR_XOUT8;
    I2CWrite(ACC_I2C_ADDR, buf, 1);
    I2CRead(ACC_I2C_ADDR, buf, 1);

    *x = (int8_t)buf[0];

    buf[0] = ACC_ADDR_YOUT8;
    I2CWrite(ACC_I2C_ADDR, buf, 1);
    I2CRead(ACC_I2C_ADDR, buf, 1);

    *y = (int8_t)buf[0];

    buf[0] = ACC_ADDR_ZOUT8;
    I2CWrite(ACC_I2C_ADDR, buf, 1);
    I2CRead(ACC_I2C_ADDR, buf, 1);

    *z = (int8_t)buf[0];
}

/******************************************************************************
 *
 * Description:
 *    Set the g-Range
 *
 * Params:
 *   [in] range - the g-Range
 *
 *****************************************************************************/
void acc_setRange(acc_range_t range)
{
    uint8_t mctl = 0;

    mctl = getModeControl();

    mctl &= ~(0x03 << 2);
    mctl |= ACC_MCTL_GLVL(range);

    setModeControl(mctl);
}

/******************************************************************************
 *
 * Description:
 *    Set sensor mode
 *
 * Params:
 *   [in] mode - the mode to set
 *
 *****************************************************************************/
void acc_setMode(acc_mode_t mode)
{
    uint8_t mctl = 0;

    mctl = getModeControl();

    mctl &= ~(0x03 << 0);
    mctl |= ACC_MCTL_MODE(mode);

    setModeControl(mctl);
}

// 三轴计算：原始值、加速度、角度值
void DealAxis3(void)
{
	int8_t tx,ty,tz;
	if((Axis3.X.Val & 0x80) == 0x00) /*读出的原始值为正数	*/
	{
		tx = Axis3.X.Val;		
		Axis3.X.Acc = (float)(-1)*tx*ACC_GRAVITY/64; /*将原始值转换为加速度，2g量程,64LSB/g;  乘以 -1 为方向处理*/ 
		if(tx>=64) /*将原始值转换为角度*/
			Axis3.X.Angle = 90.0;	/*加速度值大于 1g */		 
		else
			Axis3.X.Angle = asin((float)tx/64)*57.32; /*加速度小于1 g， Angle = asin(Acc/9.8)*57.32; 弧度制转换57.32 = 180/3.14*/
	}	  
	else /*读出的原始值为负数 */
	{
		tx =	Axis3.X.Val; 	 
		/*二补码转换*/	
		tx -= 1;
		tx = ~tx;	
		Axis3.X.Acc = (float)tx*ACC_GRAVITY/64;	/*将原始值转换为加速度*/
		if(tx>=64) /*将原始值转换为角度，乘以 -1 为方向处理*/
			Axis3.X.Angle = -90.0;
		else		
			Axis3.X.Angle = (-1)*asin((float)tx/64)*57.32; /* Angle = asin(Acc/9.8)*57.32 ; 弧度制转换57.32 = 180/3.14*/   
	}  
	if((Axis3.Y.Val & 0x80) == 0x00) /*读出的原始值为正数	*/
	{
		ty = Axis3.Y.Val;		
		Axis3.Y.Acc = (float)(-1)*ty*ACC_GRAVITY/64; /*将原始值转换为加速度，2g量程,64LSB/g;  乘以 -1 为方向处理*/ 
		if(ty>=64) /*将原始值转换为角度*/
			Axis3.Y.Angle = 90.0;	/*加速度值大于 1g */		 
		else
			Axis3.Y.Angle = asin((float)ty/64)*57.32; /*加速度小于1 g， Angle = asin(Acc/9.8)*57.32; 弧度制转换57.32 = 180/3.14*/
	}	  
	else /*读出的原始值为负数 */
	{
		ty =	Axis3.Y.Val; 	 
		/*二补码转换*/	
		ty -= 1;
		ty = ~ty;	
		Axis3.Y.Acc = (float)ty*ACC_GRAVITY/64;	/*将原始值转换为加速度*/
		if(ty>=64) /*将原始值转换为角度，乘以 -1 为方向处理*/
			Axis3.Y.Angle = -90.0;
		else		
			Axis3.Y.Angle = (-1)*asin((float)ty/64)*57.32; /* Angle = asin(Acc/9.8)*57.32 ; 弧度制转换57.32 = 180/3.14*/   
	}
	if((Axis3.Z.Val & 0x80) == 0x00) /*读出的原始值为正数	*/
	{
		tz = Axis3.Z.Val;		
		Axis3.Z.Acc = (float)(-1)*tz*ACC_GRAVITY/64; /*将原始值转换为加速度，2g量程,64LSB/g;  乘以 -1 为方向处理*/ 
		if(tz>=64) /*将原始值转换为角度*/
			Axis3.Z.Angle = 90.0;	/*加速度值大于 1g */		 
		else
			Axis3.Z.Angle = asin((float)tz/64)*57.32; /*加速度小于1 g， Angle = asin(Acc/9.8)*57.32; 弧度制转换57.32 = 180/3.14*/
	}	  
	else /*读出的原始值为负数 */
	{
		tz =	Axis3.Z.Val; 	 
		/*二补码转换*/	
		tz -= 1;
		tz = ~tz;	
		Axis3.Z.Acc = (float)tz*ACC_GRAVITY/64;	/*将原始值转换为加速度*/
		if(tz>=64) /*将原始值转换为角度，乘以 -1 为方向处理*/
			Axis3.Z.Angle = -90.0;
		else		
			Axis3.Z.Angle = (-1)*asin((float)tz/64)*57.32; /* Angle = asin(Acc/9.8)*57.32 ; 弧度制转换57.32 = 180/3.14*/   
	}
}

// 扫描xyz三轴原始值
void ScanAxis3(void) 
{
	static uint8_t AccFilterCnt = 0;
	static int16_t xsum = 0;
	static int16_t ysum = 0;
	static int16_t zsum = 0;
	int8_t tx,ty,tz;

	acc_read(&tx,&ty,&tz);
	xsum += tx;
	ysum += ty;
	zsum += tz;

	AccFilterCnt++;
	if(AccFilterCnt >= 10)
	{
		AccFilterCnt = 0;
		Axis3.X.Val = xsum / 10;
		Axis3.Y.Val = ysum / 10;
		Axis3.Z.Val = zsum / 10;
		DealAxis3();
		xsum = 0;
		ysum = 0;
		zsum = 0;
		Axis3GetFinishFlag = TRUE;
	}
}

void InitAxis3(void)
{
	acc_init();
	Axis3GetFinishFlag = FALSE;
	Axis3.X.Val = 0;
	Axis3.X.Acc = 0.0;
	Axis3.X.Angle = 0.0;
	Axis3.Y.Val  = 0;
	Axis3.Y.Acc = 0.0;
	Axis3.Y.Angle = 0.0;
	Axis3.Z.Val  = 0;
	Axis3.Z.Acc = 0.0;
	Axis3.Z.Angle = 0.0;
}
