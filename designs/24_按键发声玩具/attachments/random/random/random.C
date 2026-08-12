																																//Project: random.prj
// Device: C10S01X
// Memory: Flash 1KX14b, EEPROM 256X8b, SRAM 64X8b
// Author: 
//Company: 
//Version:
//   Date: 
//===========================================================
//===========================================================
#include	"SYSCFG.h";
#include <time.h>
//===========================================================
//Variable definition
//===========================================================
#define  TXIO		PA2  	//串口的发送脚
#define  RXIO		PA4  	//串口的接收脚
#define  uchar     unsigned char 
#define  uint      unsigned int
#define  ulong     unsigned long
unsigned char  Key_Old_Data;
unsigned char num = 0;
unsigned char FCount;
unsigned char ReadAPin;
//===========================================================
//Funtion name：interrupt ISR
//parameters：无
//returned value：无
//===========================================================



void POWER_INITIAL(void)
{ 	OSCCON = 0B00100001;			   //BIT7=0-->WDT 32KHZ  BIT6:4=010-->500kHZ/2=250kHZ,4US/T
	INTCON = 0;  					           //暂禁止所有中断
    PORTA = 0B00000000;
    TRISA = 0B00010000;	              //根据您的需要设定输入或输出，1为输入，0为输出，低位(BIT0)对应PA0,高位对应pa7
	OPTION = 0B10001000;		      	//Bit3=1 WDT MODE,PS=000=1:1 WDT RATE
    WPUA = 0B00000000;     	//PA端口上拉控制 1-开上拉 0-关上拉	
    MSCKCON = 0B00010000;	 	 			                  
}


void interrupt ISR(void)	//
{ 
  //PA电平变化中断**********************
	 if(PAIE && PAIF)		
    {
		ReadAPin = PORTA; 	//读取PORTA数据清PAIF标志
		PAIF = 0;  			//清PAIF标志位
		PAIE = 0;  			//暂先禁止PA2中断
		IOCA4 =0;  			//禁止PA2电平变化中断
	 
    }
    
}
void DelayUs(unsigned char Time)
{
	unsigned char a;
	for(a=0;a<Time;a++)
	{
		NOP();
	}
}
 
void DelayMs(unsigned char Time)
{
	unsigned char a;
	for(a=0;a<Time;a++)
	{
		 	DelayUs(11); 	//快1%
	}
}

void DelayS(unsigned char Time)
{
	unsigned char a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<10;b++)
		{
		 	DelayMs(100); 	//快1%
		}
	}
}

void PA4_Level_Change_INITIAL(void)
{
	TRISA4 =1; 				//SET PA2 INPUT
	ReadAPin = PORTA;		//清PA电平变化中断
	PAIF =0;   				//清PA INT中断标志位
    IOCA4 =1;  				//使能PA2电平变化中断
	PAIE =1;   				//使能PA INT中断
   //GIE =1;    			//使能全局中断
}
//===========================================================
//Funtion name：main
//parameters：无
//returned value：无
//===========================================================
void WByte(unsigned char input)
{
	                                        //发送起始位
	unsigned char i=8;
	TXIO = 0;
	DelayMs(7);
	unsigned char a;                  //发送8位数据位
	while(i--)
	{
		if(input&0x01) 						//先传低位
		{
			TXIO=1;
            for(a=0;a<3;a++)
            {
                DelayUs(4);
            }
           TXIO=0;
            for(a=0;a<1;a++)
            {
                DelayUs(4);
            }
		}
		else
		{
			TXIO=1;
            for(a=0;a<1;a++)
            {
                DelayUs(4);
            }
           TXIO=0;
            for(a=0;a<3;a++)
            {
                DelayUs(4);
            }
		}    
		input=input>>1;
	}
	                                        //发送校验位(无)
	                                        //发送结束位
	T0IE=0;
} 

unsigned int address()
{
    unsigned int b;
    b = rand()%10; //随机地址，输入值为实际音频数量
    b = b+1;
    return b;
}

main()
{
    	POWER_INITIAL();						//系统初始化
        Key_Old_Data = 1;
        unsigned int i;
        unsigned int sleepflag;
        sleepflag = 0;
         srand((unsigned int)time(NULL));
	while(1)
    {   
     TXIO =1;
		if(RXIO==1)
		{
			if(Key_Old_Data==0)
					{
					for(i=0;i<150;i++)
						{
							DelayMs(100);
							if(RXIO==0)
							{
								break;
							}
						}
						if(i>=100)
						{
						sleepflag = 1;
                        PA4_Level_Change_INITIAL();			//初始化外部中断
		GIE = 1;							//开总中断
		SLEEP(); 							//睡眠
						}
					}
        		Key_Old_Data=1;
        }
		if(RXIO==0)
         {
		if(Key_Old_Data==1)
		{
		   if(sleepflag==1)
		   {
				TXIO=0;
				DelayMs(45);
				TXIO=1;
				DelayMs(15);
				sleepflag=0;
			}
			num = address();
			WByte(num);
			Key_Old_Data=0;
         }
		}

	}
}
//===========================================================
