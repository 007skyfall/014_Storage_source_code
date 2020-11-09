#include <stdio.h>
#include <string.h>
#include "lpc11xx.h"
#include "clkconfig.h"
#include "gpio.h"
#include "uart.h"
#include "timer32.h"
#include "ssp.h"
#include "i2c.h"
#include "wdt.h"
#include "adc.h"
#include "rom_drivers.h"

#include "seg7led.h"
#include "oled2864.h" 
#include "light.h"
#include "acc.h"
#include "key.h"
#include "rfid.h"
#include "tools.h"
#include "spi_uart.h"
#include "temp_hum.h"
#include "collect_data.h"
#include "led_spk_fan.h"
#include "myrfid.h"
#include "menu.h"
#include "sys_init.h"

void init(void)
{
  	SystemInit();
	GPIOInit();						// Enables clock for GPIO
	CLKOUT_Setup(CLKOUTCLK_SRC_MAIN_CLK);	//set clock source :main clock
  	LPC_IOCON->PIO0_1 &= ~0x07;	
 	LPC_IOCON->PIO0_1 |= 0x01;		/* CLK OUT */

 	 // LED
 	GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
  	GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, close LED1

  	GPIOSetDir(PORT3, 1, 1);			// Set PIO3_1 to output
	GPIOSetValue(PORT3, 1, 1);		// PIO1_1 output 1, close LED2

    // Fan
    GPIOSetDir(PORT0, 2, 1);			// Set PIO0_2 to output    
    GPIOSetValue(PORT0, 2, 1);		// PIO0_2 output 1, close FAN

    // Speaker
    LPC_IOCON->R_PIO1_1 &= ~0x07;
    LPC_IOCON->R_PIO1_1 |= 0x01;
    GPIOSetDir(PORT1, 1, 1);			// Set PIO3_2 to output
    
	//GPIOSetValue(PORT1, 1, 1);		// PIO3_2 output 1,	Speaker turn off
  	 
    // 7 Segment LED
    Seg7Led_Init();
    Seg7Led_Put(' ');					// Output ' '

    // KEY_Init();

  /* Called for system library in core_cm0.h. */
    SysTick_Config(48000);

    SPI_IOConfig(0);
    SPI_Init(0, 8, 2);
    OLED_Init_I();

    UARTInit(115200);
    NVIC_SetPriority(UART_IRQn, 2);
	
	GPIOSetInterrupt(PORT1, 4, 1,0, 0 );
	GPIOIntEnable(PORT1,4);
	GPIOSetInterrupt(PORT0, 3, 1,0, 0 );
	GPIOIntEnable(PORT0,3);
	GPIOSetInterrupt(PORT2, 8, 1,0, 0 );
	GPIOIntEnable(PORT2,8);
	GPIOSetInterrupt(PORT3, 3, 1,0, 0 );
	GPIOIntEnable(PORT3,3);
	GPIOSetInterrupt(PORT1, 8, 1,0, 0 );
	GPIOIntEnable(PORT1,8);
	GPIOSetInterrupt(PORT1, 9, 1,0, 0 );
	GPIOIntEnable(PORT1,9);
	GPIOSetInterrupt(PORT1, 10, 1,0, 0 );
	GPIOIntEnable(PORT1,10);
	GPIOSetInterrupt(PORT1, 11, 1,0, 0 );
	GPIOIntEnable(PORT1,11);
    printf("\r\n\r\nFS_11C14 Main Menu\r\n");

    delay_ms(250);
    SPI_IOConfig(1);
    SPI_Init(1, 8, 2);

    SPI752_Init(1, 115200);
}

