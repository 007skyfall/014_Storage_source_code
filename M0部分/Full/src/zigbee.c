#include <stdint.h>
#include "ssp.h"
#include "spi_uart.h"
#include "device.h"
#include "zigbee.h"
#include "type.h"
#include "stdio.h"

uint8_t cmd;
uint8_t CmdBuf[1];

uint8_t ZigbeeRecvOverFlag; // zigbee接收完成标志

// Zigbee接收处理函数
// 返回值：1失败，temp命令掩码
uint8_t ZigbeeRecv(uint8_t storage_no)
{
	uint8_t temp = 0;
	if(ZigbeeRecvOverFlag == TRUE)
	{
		ZigbeeRecvOverFlag = FALSE;
		if(ZigBee_GetChar(&temp))
		{
			cmd = temp;			
			if((temp & STO_MASK) == storage_no << 6) // 仓库号正确
			{
				switch(temp & DEV_MASK) // 判断设备号
				{
					case FAN: // 排风扇命令字
							// 风扇命令处理函数
							FanCtrl(temp & CMD_MASK);
						break;
					case BEEP: // 蜂鸣器命令字
							// 蜂鸣器命令处理函数
							BeepCtrl(temp & CMD_MASK);
						break;
					case LED: // 照明灯命令字
							// 照明灯命令处理函数
							LedCtrl(temp & CMD_MASK);
						break;
					case SEG: // 数码管命令字
							// 数码管命令处理函数
							SegCtrl(temp & CMD_MASK);
						break;
					default :
						break;
				}
			}
			return temp;
		}
	}
	return 1;
}

void ZigbeeSend(char *data, uint8_t len)
{
  uint8_t i=0;
  for(i = 0; i < len; i++)
  {
		ZigBee_PutChar(data[i]);
  }
}

void InitZigbee(void)
{
	SPI_IOConfig(1);
  SPI_Init(1, 8, 2);
	SPI752_Init(1, 115200); // 其中初始初始化了p3.0中断
	ZigbeeRecvOverFlag = FALSE;
	CmdBuf[0] = 0; 
}


