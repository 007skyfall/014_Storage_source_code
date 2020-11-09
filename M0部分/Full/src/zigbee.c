#include <stdint.h>
#include "ssp.h"
#include "spi_uart.h"
#include "device.h"
#include "zigbee.h"
#include "type.h"
#include "stdio.h"

uint8_t cmd;
uint8_t CmdBuf[1];

uint8_t ZigbeeRecvOverFlag; // zigbee������ɱ�־

// Zigbee���մ�����
// ����ֵ��1ʧ�ܣ�temp��������
uint8_t ZigbeeRecv(uint8_t storage_no)
{
	uint8_t temp = 0;
	if(ZigbeeRecvOverFlag == TRUE)
	{
		ZigbeeRecvOverFlag = FALSE;
		if(ZigBee_GetChar(&temp))
		{
			cmd = temp;			
			if((temp & STO_MASK) == storage_no << 6) // �ֿ����ȷ
			{
				switch(temp & DEV_MASK) // �ж��豸��
				{
					case FAN: // �ŷ���������
							// �����������
							FanCtrl(temp & CMD_MASK);
						break;
					case BEEP: // ������������
							// �������������
							BeepCtrl(temp & CMD_MASK);
						break;
					case LED: // ������������
							// �������������
							LedCtrl(temp & CMD_MASK);
						break;
					case SEG: // �����������
							// ������������
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
	SPI752_Init(1, 115200); // ���г�ʼ��ʼ����p3.0�ж�
	ZigbeeRecvOverFlag = FALSE;
	CmdBuf[0] = 0; 
}


