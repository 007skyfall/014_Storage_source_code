#include "key.h"
#include "avr/io.h"

#include "../../beep/beep.h"
#include "../../bklight/bklight.h"


void keyinit()
{
	DDRD |= (1<<PIND4)|(1<<PIND5);
	PORTD |= (1<<PIND4)|(1<<PIND5);
	PORTD &= ~(1<<PIND6);
	KBIN_UNLOAD();
}

u8 keypad[] = {1,2};

void delayk()
{
	u8 a;
	for(a=0;a<60;a++){asm("nop");}
}

u8 getkey()
{
	u16 tmp=0;
	u16 keyv;

	tmp = getinput();//
	if(tmp!=0xffff)//有按键，必有一个为0
	{
		delayk();
		if(tmp==getinput())//表明不是干扰
		{
			keyv=tmp;//保存低位
			while(tmp==getinput())
			{
				beep_on();
			}
			switch(keyv)
			{
				case 0xf7ff:return K_1;break;
				case 0xfbff:return K_2;break;
				case 0xfdff:return K_3;break;
				case 0x7fff:return K_4;break;
				
				case 0xbfff:return K_5;break;
				case 0xdfff:return K_6;break;
				case 0xfff7:return K_7;break;
				case 0xfffb:return K_8;break;
				
				case 0xfffd:return K_9;break;
				case 0xffef:return K_E;break;
				case 0xffdf:return K_0;break;
				case 0xffbf:return K_H;break;
				
				case 0xfeff:return K_UP;break;
				case 0xefff:return K_DN;break;
				case 0xfffe:return K_C;break;
				case 0xff7f:return K_OK;break;
			}
		}else return 255;//是干扰直接返回
	}
	return 255;
}

u16 getinput()  //获得16个输入
{
    u16 Data=0;
    u8 BitCounter=16;
    //in_c = 0;
    KBIN_CK_L();
    	
    //inload = 1;
    KBIN_LOAD();
	
    while(BitCounter){
   
        Data <<= 1;
       // if(in_d == 1) Data += 1;
        if (KBIN_DATA_ISSET()) 
        {
        	Data += 1;
        	
        }
        //in_c = 1;
        KBIN_CK_H();
        //in_c = 0; 
        KBIN_CK_L();  
                 
        BitCounter --;
    };  
    //inload = 0;
    KBIN_UNLOAD();
    return Data;
}
