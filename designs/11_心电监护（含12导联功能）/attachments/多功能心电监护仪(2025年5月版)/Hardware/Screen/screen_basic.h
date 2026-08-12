#ifndef __LCD_H
#define __LCD_H
#include "gd32f4xx.h"
#include "systick.h"
#include "stdbool.h"


#define LCD_SDA_RCU  			RCU_GPIOE   // SDA
#define LCD_SDA_PORT  		    GPIOE
#define LCD_SDA_PIN    		    GPIO_PIN_6//GPIO_PIN_5  //v0.0.1新屏幕6，老屏幕5
#define LCD_SDA_WRITE(x)	    gpio_bit_write(LCD_SDA_PORT,LCD_SDA_PIN,x)
#define LCD_SCL_RCU  			RCU_GPIOE   // SCL
#define LCD_SCL_PORT  		    GPIOE
#define LCD_SCL_PIN    		    GPIO_PIN_2
#define LCD_SCL_WRITE(x)	    gpio_bit_write(LCD_SCL_PORT,LCD_SCL_PIN,x)
#define LCD_CS_RCU  			RCU_GPIOE   // CS
#define LCD_CS_PORT  		    GPIOE
#define LCD_CS_PIN    		    GPIO_PIN_4
#define LCD_CS_WRITE(x)	        gpio_bit_write(LCD_CS_PORT,LCD_CS_PIN,x)
#define LCD_RST_RCU  			RCU_GPIOD   // RST
#define LCD_RST_PORT  		    GPIOD
#define LCD_RST_PIN    		    GPIO_PIN_12
#define LCD_RST_WRITE(x)	    gpio_bit_write(LCD_RST_PORT,LCD_RST_PIN,x)
#define LCD_BLK_ON              gpio_bit_reset(GPIOD,GPIO_PIN_13)	// PD13
#define LCD_BLK_OFF             gpio_bit_set(GPIOD,GPIO_PIN_13)	// PD13
#define LCD_CS_Clr()            gpio_bit_reset(GPIOE,GPIO_PIN_4)	// PD13
#define LCD_CS_Set()            gpio_bit_set(GPIOE,GPIO_PIN_4)	// PD13
#define LCD_SDI_RCU  			RCU_GPIOC   // SDI
#define LCD_SDI_PORT  		    GPIOC
#define LCD_SDI_PIN    		    GPIO_PIN_1
#define LCD_SDI_WRITE(x)	    gpio_bit_write(LCD_SDI_PORT,LCD_SDI_PIN,x)


#define HORIZONTAL_SYNCHRONOUS_PULSE  10
#define HORIZONTAL_BACK_PORCH         150
#define ACTIVE_WIDTH                  800
#define HORIZONTAL_FRONT_PORCH        40

#define VERTICAL_SYNCHRONOUS_PULSE    10
#define VERTICAL_BACK_PORCH           140
#define ACTIVE_HEIGHT                 480
#define VERTICAL_FRONT_PORCH          15


extern uint8_t *ltdc_framebuf[2];              
extern uint8_t ltdc_lcd_framebuf0[800][480];       

//extern uint16_t ltdc_lcd_framebuf1[800][180]; 




//画笔颜色
#define WHITE        0xD0//D0?
#define BLACK        0x7F
#define BLUE         0x77  
#define PINK         0x63
#define RED          0x49//10?
#define GREEN        0x42 
#define LIGHTGREEN   0x61 
#define CYAN         0x28
#define YELLOW       0x20
#define ORANGE       0xf1
#define BROWN        0X23//?
#define GRAY         0X8b//C9 DC?


////扫描方向定义
//#define L2R_U2D  0 //从左到右,从上到下
//#define L2R_D2U  1 //从左到右,从下到上
//#define R2L_U2D  2 //从右到左,从上到下
//#define R2L_D2U  3 //从右到左,从下到上
//#define U2D_L2R  4 //从上到下,从左到右
//#define U2D_R2L  5 //从上到下,从右到左
//#define D2U_L2R  6 //从下到上,从左到右
//#define D2U_R2L  7 //从下到上,从右到左	 
//#define DFT_SCAN_DIR  L2R_U2D  //默认的扫描方向


//LCD的画笔颜色和背景色	   
extern uint16_t  POINT_COLOR;//默认红色    
extern uint16_t  BACK_COLOR; //背景颜色.默认为白色



//////////
void NT35510_Init(void);
void TLI_ConfigParam( 
int b_HORIZONTAL_SYNCHRONOUS_PULSE,
int b_HORIZONTAL_BACK_PORCH       ,
int b_HORIZONTAL_FRONT_PORCH      ,
int b_VERTICAL_SYNCHRONOUS_PULSE  ,
int b_VERTICAL_BACK_PORCH         ,
int b_VERTICAL_FRONT_PORCH        );
////////////////
void LCD_Init(void);//屏幕初始化
////////////////
void LTDC_Draw_Point(uint8_t layer, uint16_t x, uint16_t y, uint8_t color);/////画点
void LCD_Clear(uint8_t layer,uint8_t  color);//清屏函数   
void LCD_ShowChar(uint8_t layer,uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode);////在指定位置显示一个字符
void LCD_ShowString(uint8_t layer,uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t mode,uint8_t *p);//显示字符串
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t color);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t color);
void LCD_SolidRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t color);
///////////////
void Touch_Init(void);
unsigned char Touch_SPI_Transport(unsigned char com);
void Touch_Read();


void A4_DrawChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode);////在指定位置显示一个字符
void Side_DrawChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode);////在指定位置显示一个字符

#define Touch_CS_L gpio_bit_write(GPIOD,GPIO_PIN_0,RESET);
#define Touch_CS_H gpio_bit_write(GPIOD,GPIO_PIN_0,SET);

#endif  





/*
    
        rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI0);//AF5
    //TLI
    tli_config();
    tli_layer_enable(LAYER0);
    //tli_layer_enable(LAYER1);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();
    //触摸SPI CS低电平使能
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_7);
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_8);
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_9);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
  
    gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    gpio_bit_write(GPIOD,GPIO_PIN_0,SET);
  
    spi_parameter_struct spistr;
  
    spistr.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spistr.device_mode = SPI_MASTER;
    spistr.frame_size = SPI_FRAMESIZE_8BIT;
    spistr.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spistr.nss = SPI_NSS_SOFT;
    spistr.prescale = SPI_PSC_64;
    spistr.endian = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spistr);
    spi_enable(SPI0);*/