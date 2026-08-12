#include "screen_basic.h"
#include "stdio.h"
#include "string.h" 
#include "font.h" 
#include "printer.h"

//LCD的画笔颜色和背景色	   
uint16_t  POINT_COLOR=0x0000;	//画笔颜色
uint16_t  BACK_COLOR=0xFFFF;  //背景色 

uint8_t luts[256][3] = {0x00};

int x,y = 0;
bool press = 0;
  


typedef struct
{
  uint32_t n;
  uint32_t p;
  uint32_t r;
  uint32_t v;
} TLI_CLK_t;

TLI_CLK_t tli_clk;

// LCD对应外部SRAM地址
#define LCD_FRAME_BUF_ADDR 0XC0000000
uint8_t *ltdc_framebuf[2];
uint8_t ltdc_lcd_framebuf0[800][480];                 //图层0
//uint16_t ltdc_lcd_framebuf1[800][180]; //图层1
//#define LCD_800X4800


void TLI_ConfigParam(
    int b_HORIZONTAL_SYNCHRONOUS_PULSE,
    int b_HORIZONTAL_BACK_PORCH,
    int b_HORIZONTAL_FRONT_PORCH,
    int b_VERTICAL_SYNCHRONOUS_PULSE,
    int b_VERTICAL_BACK_PORCH,
    int b_VERTICAL_FRONT_PORCH)
{

  tli_parameter_struct tli_init_struct;
  tli_layer_parameter_struct tli_layer_init_struct;

  /* initialize the parameters of structure */
    tli_struct_para_init(&tli_init_struct);
    tli_layer_struct_para_init(&tli_layer_init_struct);
    rcu_periph_clock_enable(RCU_TLI);
    /* enable the periphral clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);
    /* TLI pins AF configure */
    gpio_af_set(GPIOA, GPIO_AF_14, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_11 | GPIO_PIN_12);
    gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_0); // PB0
    gpio_af_set(GPIOB, GPIO_AF_14, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11);
    gpio_af_set(GPIOC, GPIO_AF_14, GPIO_PIN_6 | GPIO_PIN_7);
    gpio_af_set(GPIOD, GPIO_AF_14, GPIO_PIN_3);
    gpio_af_set(GPIOF, GPIO_AF_14, GPIO_PIN_10);
    gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_11);
    gpio_af_set(GPIOG, GPIO_AF_9, GPIO_PIN_10 | GPIO_PIN_12);
    /* configure TLI GPIO */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_11 | GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_11 | GPIO_PIN_12);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_0 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6 | GPIO_PIN_7);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_6 | GPIO_PIN_7);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_3);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_10);
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
    
    
    gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_13);
    

    b_HORIZONTAL_SYNCHRONOUS_PULSE = HORIZONTAL_SYNCHRONOUS_PULSE;
    b_HORIZONTAL_BACK_PORCH        = HORIZONTAL_BACK_PORCH       ;
    b_HORIZONTAL_FRONT_PORCH       = HORIZONTAL_FRONT_PORCH      ;
    b_VERTICAL_SYNCHRONOUS_PULSE   = VERTICAL_SYNCHRONOUS_PULSE  ;
    b_VERTICAL_BACK_PORCH          = VERTICAL_BACK_PORCH         ;
    b_VERTICAL_FRONT_PORCH         = VERTICAL_FRONT_PORCH        ;
    /* configure PLLSAI to generate TLI clock */
    if(ERROR == rcu_pllsai_config(192, 2, 3)) {
        while(1) {
        }
    }
    //rcu_tli_clock_div_config(RCU_PLLSAIR_DIV1);

    rcu_osci_on(RCU_PLLSAI_CK);
    


    if(ERROR == rcu_osci_stab_wait(RCU_PLLSAI_CK)) {
        while(1) {
        }
    }

    /* configure TLI parameter struct */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;
    /* LCD display timing configuration */
    tli_init_struct.synpsz_hpsz = HORIZONTAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.synpsz_vpsz = VERTICAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1;
    tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;
    tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH - 1;
    tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT - 1;
    tli_init_struct.totalsz_htsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH + HORIZONTAL_FRONT_PORCH - 1;
    tli_init_struct.totalsz_vtsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT + VERTICAL_FRONT_PORCH - 1;
    /* configure LCD background R,G,B values */
    tli_init_struct.backcolor_red = 0xFF;
    tli_init_struct.backcolor_green = 0xFF;
    tli_init_struct.backcolor_blue = 0xFF;
    tli_init(&tli_init_struct);

    /* TLI layer1 configuration */
    tli_layer_init_struct.layer_window_leftpos = b_HORIZONTAL_SYNCHRONOUS_PULSE + b_HORIZONTAL_BACK_PORCH;
    tli_layer_init_struct.layer_window_rightpos = (ACTIVE_WIDTH + b_HORIZONTAL_SYNCHRONOUS_PULSE + b_HORIZONTAL_BACK_PORCH - 1);
    tli_layer_init_struct.layer_window_toppos = b_VERTICAL_SYNCHRONOUS_PULSE + b_VERTICAL_BACK_PORCH;
    tli_layer_init_struct.layer_window_bottompos = (ACTIVE_HEIGHT + b_VERTICAL_SYNCHRONOUS_PULSE + b_VERTICAL_BACK_PORCH - 1);

    /* TLI window pixel format configuration */
    tli_layer_init_struct.layer_ppf = LAYER_PPF_L8;
    /* TLI window specified alpha configuration */
    tli_layer_init_struct.layer_sa = 255; //
    /* TLI layer default alpha R,G,B value configuration */
    tli_layer_init_struct.layer_default_blue = 0xFF;
    tli_layer_init_struct.layer_default_green = 0xFF;
    tli_layer_init_struct.layer_default_red = 0xFF;
    tli_layer_init_struct.layer_default_alpha = 0X00; // ???
    /* TLI window blend configuration */
    tli_layer_init_struct.layer_acf1 = LAYER_ACF1_SA;
    tli_layer_init_struct.layer_acf2 = LAYER_ACF2_SA;
    /* TLI layer frame buffer base address configuration */
    ltdc_framebuf[1] = (uint16_t *)&ltdc_lcd_framebuf0;

    //  memcpy(ltdc_lcd_framebuf1, gImage_Image_RGB565, sizeof(gImage_Image_RGB565));
    tli_layer_init_struct.layer_frame_bufaddr = (uint32_t)&ltdc_lcd_framebuf0;

    //  tli_layer_init_struct.layer_frame_bufaddr = (uint32_t)&gImage_Image_RGB565;
    tli_layer_init_struct.layer_frame_line_length = ((ACTIVE_WIDTH * 2) + 3);
    tli_layer_init_struct.layer_frame_buf_stride_offset = (ACTIVE_WIDTH * 2);
    tli_layer_init_struct.layer_frame_total_line_number = ACTIVE_HEIGHT; // ACTIVE_HEIGHT;
    tli_layer_init(LAYER1, &tli_layer_init_struct);

    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_dither_config(TLI_DITHER_ENABLE);

    
}




//屏幕初始化
void LCD_Init(void)
{
    //printf("Start TLI_ConfigParam\r\n");
    TLI_ConfigParam(0,0,0,0,0,0);
    tli_layer_lut_parameter_struct tls;
    ipa_background_parameter_struct bkf;
    //tli_lut_struct_para_init(&tls);
    rcu_periph_clock_enable(RCU_IPA);
    ipa_destination_parameter_struct  ipa_destination_init_struct;
    ipa_foreground_parameter_struct   ipa_fg_init_struct;
    //ipa_background_lut_init(0xFF,BACKGROUND_PPF_RGB888,(uint32_t)&luts);
    tli_layer_disable(LAYER0);
    tli_layer_enable(LAYER1); 
    /* reload layer0 and layer1 configuration */
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    tli_enable();
    //tls.layer_table_addr = (uint32_t)&luts;
    tls.layer_lut_channel_red = 0xFF;
    tls.layer_lut_channel_green = 0xFF;
    tls.layer_lut_channel_blue = 0xFF;
    tli_lut_init(LAYER1,&tls);
    tli_lut_enable(LAYER1);
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
    LCD_Clear(1,WHITE);
    LCD_BLK_OFF;
}


/////画点
void LTDC_Draw_Point(uint8_t layer, uint16_t x, uint16_t y, uint8_t color)
{
  if(x > 800 || x <0 || y>480 || y<0){
    return;
  }
  *(uint8_t *)(ltdc_framebuf[layer] + 1 * (ACTIVE_WIDTH * (y) + x)) = color;
  //ltdc_lcd_framebuf0[y][x] = color;
}
//清屏函数   
void LCD_Clear(uint8_t layer,uint8_t  color)
{
    uint32_t index=0;
    uint32_t totalpoint=ACTIVE_WIDTH;
    totalpoint*=ACTIVE_HEIGHT; //得到总点数
    //for(index=0;index<(totalpoint);index++)
    //{
    //    ltdc_framebuf[layer][index] = color;
    //}
    memset(&ltdc_lcd_framebuf0,color,sizeof ltdc_lcd_framebuf0);
} 

//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(uint8_t layer,uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{  
    uint8_t temp,t1,t;
    uint16_t y0=y;
    uint8_t csize;
    uint8_t nsize;
    if(size == 121){
      csize=(12/8+((12%8)?1:0))*(12/2);		//得到字体一个字符对应点阵集所占的字节数	
      nsize = 12;
    }else if(size == 161){
      csize=(16/8+((16%8)?1:0))*(16/2);		//得到字体一个字符对应点阵集所占的字节数	
      nsize = 16;
    }else if(size == 241){
      csize=(24/8+((24%8)?1:0))*(24/2);		//得到字体一个字符对应点阵集所占的字节数	
      nsize = 24;
    }else{
      csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
      nsize = size;
    }
    num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
    if(size == 48){
      if(num+' ' == '-'){
        num = '*' - ' ';
      }else if(num+' ' == '?'){
        num = '+' - ' ';
      }else if(num+' ' == '/'){
        num = ',' - ' ';
      }else{
        num -= 16;
      }
    }
    if(size>100){
      for(t=0;t<(csize*2);t++)
    {   
        if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
        else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
        else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
        else if(size==48)temp=asc2_4824[num][t];	//调用2412字体
        else if(size==121)temp=asc2_1206c[num][t]; 	 	//调用1206字体
        else if(size==161)temp=asc2_1608c[num][t];	//调用1608字体
        else if(size==241)temp=asc2_2412c[num][t];	//调用2412字体
        else return;								//没有的字库
        for(t1=0;t1<8;t1++)
        {			    
            if(temp&0x80)LTDC_Draw_Point(layer,x,y,POINT_COLOR);
            else if(mode==0)LTDC_Draw_Point(layer,x,y,BACK_COLOR);
            temp<<=1;
            y++;
            if(y>=ACTIVE_HEIGHT)return;		//超区域了
            if((y-y0)==nsize)
            {
                y=y0;
                x++;
                if(x>=ACTIVE_WIDTH)return;	//超区域了
                break;
            }
        }  	 
    }
    }else{
    for(t=0;t<csize;t++)
    {   
        if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
        else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
        else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
        else if(size==48)temp=asc2_4824[num][t];	//调用2412字体
        else if(size==121)temp=asc2_1206c[num][t]; 	 	//调用1206字体
        else if(size==161)temp=asc2_1608c[num][t];	//调用1608字体
        else if(size==241)temp=asc2_2412c[num][t];	//调用2412字体
        else return;								//没有的字库
        for(t1=0;t1<8;t1++)
        {			    
            if(temp&0x80)LTDC_Draw_Point(layer,x,y,POINT_COLOR);
            else if(mode==0)LTDC_Draw_Point(layer,x,y,BACK_COLOR);
            temp<<=1;
            y++;
            if(y>=ACTIVE_HEIGHT)return;		//超区域了
            if((y-y0)==nsize)
            {
                y=y0;
                x++;
                if(x>=ACTIVE_WIDTH)return;	//超区域了
                break;
            }
        }  	 
    }  }
}   
//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void LCD_ShowString(uint8_t layer,uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t mode,uint8_t *p)
{         
	uint8_t x0=x;
	width+=x;
	height+=y;
  uint8_t nsize;
  if(size == 121){
      nsize = 12;
    }else if(size == 161){
      nsize = 16;
    }else if(size == 241){
      nsize = 24;
    }else{
      nsize = size;
    }
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        
        if(x>=width){x=x0;y+=nsize;}
        if(y>=height)break;//退出
        LCD_ShowChar(layer,x,y,*p,size,mode);
        if(size >100){
          x+=nsize;
        }else{
        x+=nsize/2;
        }
        p++;
    }  
}

void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t color)
{
  uint16_t t;
  int xerr = 0, yerr = 0, delta_x, delta_y, distance;
  int incx, incy, uRow, uCol;
  delta_x = x2 - x1; /* 计算坐标增量 */
  delta_y = y2 - y1;
  uRow = x1;
  uCol = y1;
  if (delta_x > 0) /* 设置单步方向 */
    incx = 1;
  else if (delta_x == 0) /* 垂直线 */
    incx = 0;
  else
  {
    incx = -1;
    delta_x = -delta_x;
  }

  if (delta_y > 0)
    incy = 1;
  else if (delta_y == 0) /* 水平线 */
    incy = 0;
  else
  {
    incy = -1;
    delta_y = -delta_y;
  }

  if (delta_x > delta_y) /*选取基本增量坐标轴  */
    distance = delta_x;
  else
    distance = delta_y;

  for (t = 0; t <= distance; t++) /* 画线输出 */
  {

    LTDC_Draw_Point(1,uRow, uCol,color); /* 画点 */
    xerr += delta_x;
    yerr += delta_y;
    if (xerr + distance / 2 > distance)
    {
      xerr -= distance;
      uRow += incx;
    }
    if (yerr + distance / 2 > distance)
    {
      yerr -= distance;
      uCol += incy;
    }
  }
}

/**
 * 画矩形
*/
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t color)
{
  LCD_DrawLine(x1, y1, x2, y1,color);
  LCD_DrawLine(x1, y1, x1, y2,color);
  LCD_DrawLine(x2, y1, x2, y2,color);
  LCD_DrawLine(x2, y2, x1, y2,color);
}

int ji;
void LCD_SolidRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t color)
{
    for(ji=y1;ji<=y2;ji++){
      LCD_DrawLine(x1, ji, x2, ji,color);
    }
}

void Touch_Init(){
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI2);//AF6
    //触摸SPI CS低电平使能
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_af_set(GPIOB, GPIO_AF_6, GPIO_PIN_3);
    gpio_af_set(GPIOB, GPIO_AF_6, GPIO_PIN_4);
    gpio_af_set(GPIOB, GPIO_AF_6, GPIO_PIN_5);
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
    spistr.prescale = SPI_PSC_32;
    spistr.endian = SPI_ENDIAN_MSB;
    spi_init(SPI2, &spistr);
    spi_enable(SPI2);
    Touch_CS_L;
}

uint16_t xraw,yraw,g,f;
void Touch_Read(){
//写公共变量x，y，按下press，直接用算的那俩，还要校准
  Touch_SPI_Transport(0b10110000);
  xraw = Touch_SPI_Transport(0x00);
  xraw<<=5;
  xraw += Touch_SPI_Transport(0b11000000)>>3;
  yraw = Touch_SPI_Transport(0x00);
  yraw<<=5;
  yraw +=Touch_SPI_Transport(0x00)>>3;
  //xraw = 4095-xraw;
  //yraw = 4095-yraw;
}

unsigned char Touch_SPI_Transport(unsigned char com) {
    while(RESET == spi_i2s_flag_get(SPI2, SPI_FLAG_TBE)); //检测发送寄存器是否为空，上一个数据发送完成
    spi_i2s_data_transmit(SPI2, com);//发送数据
    while(RESET == spi_i2s_flag_get(SPI2, SPI_FLAG_RBNE)); //检测接收标志是否非空，发送完成
    return (spi_i2s_data_receive(SPI2));	//返回接收到的数据
}

//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void A4_DrawChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{  
    uint8_t temp,t1,t;
    uint16_t y0=y;
    uint8_t csize;
    uint8_t nsize;
    if(size == 121){
      csize=(12/8+((12%8)?1:0))*(12/2);		//得到字体一个字符对应点阵集所占的字节数	
      nsize = 12;
    }else if(size == 161){
      csize=(16/8+((16%8)?1:0))*(16/2);		//得到字体一个字符对应点阵集所占的字节数	
      nsize = 16;
    }else if(size == 241){
      csize=(24/8+((24%8)?1:0))*(24/2);		//得到字体一个字符对应点阵集所占的字节数	
      nsize = 24;
    }else{
      csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
      nsize = size;
    }
    num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
    if(size == 48){
      if(num+' ' == '-'){
        num = '*' - ' ';
      }else if(num+' ' == '?'){
        num = '+' - ' ';
      }else{
        num -= 16;
      }
    }
    if(size>100){
      for(t=0;t<(csize*2);t++)
    {   
        if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
        else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
        else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
        else if(size==121)temp=asc2_1206c[num][t]; 	 	//调用1206字体
        else if(size==161)temp=asc2_1608c[num][t];	//调用1608字体
        else if(size==241)temp=asc2_2412c[num][t];	//调用2412字体
        else return;								//没有的字库
        for(t1=0;t1<8;t1++)
        {			    
            if(temp&0x80)a4buffer[x][y] = 1;
            else if(mode==0)a4buffer[x][y] = 0;
            temp<<=1;
            y++;
            if(y>=1728)return;		//超区域了
            if((y-y0)==nsize)
            {
                y=y0;
                x++;
                if(x>=2400)return;	//超区域了
                break;
            }
        }  	 
    }
    }else{
    for(t=0;t<csize;t++)
    {   
        if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
        else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
        else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
        else if(size==121)temp=asc2_1206c[num][t]; 	 	//调用1206字体
        else if(size==161)temp=asc2_1608c[num][t];	//调用1608字体
        else if(size==241)temp=asc2_2412c[num][t];	//调用2412字体
        else return;								//没有的字库
        for(t1=0;t1<8;t1++)
        {			    
            if(temp&0x80)a4buffer[x][y] = 1;
            else if(mode==0)a4buffer[x][y] = 0;
            temp<<=1;
            y++;
            if(y>=1728)return;		//超区域了
            if((y-y0)==nsize)
            {
                y=y0;
                x++;
                if(x>=2400)return;	//超区域了
                break;
            }
        }  	 
    }  }
}   



void Side_DrawChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{  
    uint8_t temp,t1,t;
    uint16_t y0=y;
    uint8_t csize;
    uint8_t nsize;
    if(size == 121){
      csize=(12/8+((12%8)?1:0))*(12/2);		//得到字体一个字符对应点阵集所占的字节数	
      nsize = 12;
    }else if(size == 161){
      csize=(16/8+((16%8)?1:0))*(16/2);		//得到字体一个字符对应点阵集所占的字节数	
      nsize = 16;
    }else if(size == 241){
      csize=(24/8+((24%8)?1:0))*(24/2);		//得到字体一个字符对应点阵集所占的字节数	
      nsize = 24;
    }else{
      csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
      nsize = size;
    }
    num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
    if(size == 48){
      if(num+' ' == '-'){
        num = '*' - ' ';
      }else if(num+' ' == '?'){
        num = '+' - ' ';
      }else{
        num -= 16;
      }
    }
    if(size>100){
      for(t=0;t<(csize*2);t++)
    {   
        if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
        else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
        else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
        else if(size==48)temp=asc2_4824[num][t];	//调用4824字体
        else if(size==121)temp=asc2_1206c[num][t]; 	 	//调用1206字体
        else if(size==161)temp=asc2_1608c[num][t];	//调用1608字体
        else if(size==241)temp=asc2_2412c[num][t];	//调用2412字体
        else return;								//没有的字库
        for(t1=0;t1<8;t1++)
        {			    
            if(temp&0x80)sidebar12[x][y] = 1;
            else if(mode==0)sidebar12[x][y] = 0;
            temp<<=1;
            y++;
            if(y>=1728)return;		//超区域了
            if((y-y0)==nsize)
            {
                y=y0;
                x++;
                if(x>=2400)return;	//超区域了
                break;
            }
        }  	 
    }
    }else{
    for(t=0;t<csize;t++)
    {   
        if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
        else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
        else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
        else if(size==48)temp=asc2_4824[num][t];	//调用4824字体
        else if(size==121)temp=asc2_1206c[num][t]; 	 	//调用1206字体
        else if(size==161)temp=asc2_1608c[num][t];	//调用1608字体
        else if(size==241)temp=asc2_2412c[num][t];	//调用2412字体
        else return;								//没有的字库
        for(t1=0;t1<8;t1++)
        {			    
            if(temp&0x80)sidebar12[x][y] = 1;
            else if(mode==0)sidebar12[x][y] = 0;
            temp<<=1;
            y++;
            if(y>=1728)return;		//超区域了
            if((y-y0)==nsize)
            {
                y=y0;
                x++;
                if(x>=2400)return;	//超区域了
                break;
            }
        }  	 
    }  }
}   
