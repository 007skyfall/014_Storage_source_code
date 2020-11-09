#include <avr/io.h>

#include "config.h"
#include "cy14443cmd.h"
#include "usart.h"
#include "delay/delay.h"

#include "beep/beep.h"
#include "app/key/key.h"


//#define DEBUG(...)
#define DEBUG usart_write

//Ӳ�����Ŷ���
#define FLASH_MOSI PB5
#define FLASH_MISO PB6
#define FLASH_SCK		PB7
#define FLASH_CS	PB4

//spi��ʼ��
//MASTģʽ
//ʱ��Ƶ�ʲ�����3M
//MSB��ǰ
//�����ز���

void spi_init()
{
	//MISO����Ϊ���룬��������رգ�����Ĭ��Ϊ1
	PORTB |= _BV(FLASH_SCK)|_BV(FLASH_MOSI)|_BV(FLASH_CS)|_BV(PB2);	
	
	//SCK,MOSI��CS�˿ڶ�Ӧ������Ϊ���
	DDRB |= _BV(FLASH_SCK)|_BV(FLASH_MOSI)|_BV(FLASH_CS);                            
	
	// SPI�жϽ�ֹ, SPIʹ��, masterģʽ, MSB ǰ,  SPI ģʽ 0, SCKƵ��Fcl/128
	SPCR = _BV(SPE)|_BV(MSTR)|_BV(SPR1)|_BV(SPR0);
}


//��SPI�����һ�ֽ����ݣ�ͬʱ����һ���ֽ�
//����ֽڶ��������һ��ʱ�����ڴӻ����͵�����
//��һ��ǧ��ע��ʶ��

u8 spi_write(u8 data)
{
	cli();
	SPDR = data;
	while(!(SPSR & _BV(SPIF)));
	return SPDR;
}

//crcУ��
u8 CheckSum(u8 *databuf)
{
	u8 tmp = databuf[0]-1;
	u8 chksum=databuf[0];
	while(tmp)
	{
		chksum^=databuf[tmp];
		tmp--;
	}
	if(databuf[databuf[0]]==chksum) return 1;
	else
		databuf[databuf[0]]=chksum;
	return 0;
}

//��������Ӧ����
//1,��ʼ��SPI��ΪMSB��ǰ�����ʲ�����3M�������ز���
//2,����spi_write���������������и�ģ��
//3,����SPIʱ���ʵ���ʱ
//4.�����������з�����ϣ�������ʱ�ȴ�ģ����Ӧ
//5.�ٴε���spi_write��0�����������������и�ģ��
//6,��spi_write���ص������н��з���ģ����Ӧ�����ݰ�

int main(void)
{
	u8 tmp,a,x;
	u8 buf[16];		
	DDRA = 0xff;		//ȫ�����
	DDRB = 0xff;
	
	DDRD = 0x0;			//rxd in txd out 
	PORTD = 0xff;

	
	usart_init(9600);//9600
	for(a=0;a<1000;a++){asm("nop");}
	
	DEBUG("system\n");

	keyinit();//���̳�ʼ��
	spi_init();
	
	initbeep();//������
	beep_ok();//��������һ��
		
	tmp=0;
	
	while(1)//����ѭ��
	{
		tmp = getkey();//�����̼�ֵ
		
		if(tmp!=255)//�а���
		{	
			PORTB &=~(1<<PB4);//ģ��ƬѡSS=0��ѡ��ģ��
			
			buf[0]=spi_write(0xaa);//����ͷ
			delayms(1);//�ʵ���ʱ������SPIʱ�����ʶ���
			buf[1]=spi_write(0xbb);
			delayms(1);
			
/*			//��������һ�����������
			buf[2]=spi_write(0x02);//����
			delayms(1);
			buf[3]=spi_write(0x01);//�����룬�����к�
			delayms(1);
			buf[4]=spi_write(0x3);//У����
			delayms(1);
*/			
			buf[2]=spi_write(0x02);//����
			delayms(1);
			buf[3]=spi_write(0x19);//�����룬��������
			delayms(1);
			buf[4]=spi_write(0x1b);//У����
			delayms(1);			

			delayms(200);//�ȴ��ӻ�׼��������
			delayms(200);

			
			buf[0]=spi_write(0);//��ʼ��ȡ���ݣ���һ�����������ֽ���
								//��������������һ���ֽ�
			delayms(1);
			buf[1]=spi_write(0);//��ȡ��ģ����Ӧ�ڶ����ֽ� 0xAA
			delayms(1);
			buf[2]=spi_write(0);//��ȡ��ģ����Ӧ�������ֽ� 0xBB
			delayms(1);
			buf[3]=spi_write(0);//��ȡ��ģ����Ӧ���ĸ��ֽ� �����ȣ��жϽ�������Ҫ���յ��ֽ���
			delayms(1);			//���һ���ֽڣ�ΪУ����
			
			x = buf[3];
			for(tmp=0;tmp<x;tmp++)
			{
				buf[tmp]=spi_write(0);
				delayms(1);
			}
						
			PORTB |=(1<<PB4);//ģ��ƬѡSS=1��ģ����Ч
			
			DEBUG("\r\nsend data\r\n");
			for(tmp=0;tmp<x;tmp++)//�������ӡ����
			{
				DEBUG("buf[%x]=%x\r\n",tmp,buf[tmp]);
			}

		}
	}
	return 0;
}
