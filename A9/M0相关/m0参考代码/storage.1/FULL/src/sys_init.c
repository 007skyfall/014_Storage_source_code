#include "allinc.h"

 void init(void)
 {

        SystemInit();
        GPIOInit();						// Enables clock for GPIO
		CLKOUT_Setup(CLKOUTCLK_SRC_MAIN_CLK);  //set clock source :main clock

		SysTick_Config(48000);

		LPC_IOCON->PIO0_1 &= ~0x07;	
		LPC_IOCON->PIO0_1 |= 0x01;		/* CLK OUT */
		
		// LED
		GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
		GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, close LED1
		
		GPIOSetDir(PORT3, 1, 1);			// Set PIO3_1 to output
		GPIOSetValue(PORT3, 1, 1);		// PIO1_1 output 1, close LED2
		
		// Fan
		GPIOSetDir(PORT0, 2, 1);			// Set PIO0_2 to output    
		GPIOSetValue(PORT0, 2, 1);			//close fan 
		
		// Speaker
		LPC_IOCON->R_PIO1_1 &= ~0x07;
		LPC_IOCON->R_PIO1_1 |= 0x01;
		GPIOSetDir(PORT1, 1, 1);			// Set PIO3_2 to output
		GPIOSetValue(PORT1, 1, 1);		// PIO3_2 output 1,	Speaker turn off
		
		// 7 Segment LED
		Seg7Led_Init();
		Seg7Led_Put(' ');					// Output ' '
		
		KEY_Init();	//按键初始化，本项目没有使用按键，不接受按键控制
		
		/* Called for system library in core_cm0.h. */
	//	SysTick_Config(48000);
		
        SPI_IOConfig(0);							  //初始化OLED
        SPI_Init(0, 8, 2);
        OLED_Init_I();
		
		UARTInit(115200);  //设置串口波特率
				
		GPIOSetValue(PORT3, 0, 0);		// PIO3_0 output 0, Turn on LED1
		GPIOSetValue(PORT3, 1, 0);		// PIO3_1 output 0, Turn on LED2
		delay_ms(200);
		GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, Turn off LED1
		GPIOSetValue(PORT3, 1, 1);		// PIO3_1 output 1, Turn off LED2
	
	//	GPIOSetValue(PORT2, 8, 1);
		GPIOSetDir(PORT2, 8, 0);
		
		GPIOSetValue(PORT2, 8, 1);		// PIO2_8 output 1,
		//GPIOSetDir(PORT1,4,0);
        GPIOSetInterrupt(PORT1,4,0,0,0);
        GPIOIntEnable(PORT1,4);
		SPI_IOConfig(0);
		SPI_Init(0, 8, 2);

		SPI_IOConfig(1);//	Zigbee 初始化
		SPI_Init(1, 8, 2);

        SPI752_Init(1, 115200);
	    OLED_Init_I();
	    ADCInit(ADC_CLK);
	    I2CInit(I2CMASTER, 0);
	    acc_init();
        light_init();
	    light_enable();
	    light_setRange(LIGHT_RANGE_4000);
        init_timer32(0,4800);
        
         NVIC_SetPriority(TIMER_32_0_IRQn, 2);
         NVIC_SetPriority(SysTick_IRQn, 1);
//        NVIC_SetPriority(UART_IRQn, 4);
//         NVIC_SetPriority(EINT1_IRQn, 0);
//        NVIC_SetPriority(EINT2_IRQn, 3);


}

