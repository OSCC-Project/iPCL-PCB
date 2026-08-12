#include "resp_monitor.h"
#include "gd32f4xx.h"
#include "gd32f4xx_rcu.h"
#include "gd32f4xx_spi.h"
#include "systick.h"
#include "SEGGER_RTT.h"
#include "ecgjudge.h"

/*ADS1292R命令定义*/
/*系统命令*/
#define WAKEUP	0X02	//从待机模式唤醒
#define STANDBY	0X04	//进入待机模式
#define RESET	0X06	//复位ADS1292R
#define START	0X08	//启动或转换
#define STOP	0X0A	//停止转换
#define OFFSETCAL	0X1A	//通道偏移校准
/*数据读取命令*/
#define RDATAC	0X10	//启用连续的数据读取模式,默认使用此模式
#define SDATAC	0X11	//停止连续的数据读取模式
#define RDATA	0X12	//通过命令读取数据;支持多种读回。
/*寄存器读取命令*/
#define	RREG	0X20	//读取001r rrrr 000n nnnn  这里定义的只有高八位，低8位在发送命令时设置
#define WREG	0X40	//写入010r rrrr 000n nnnn
/*	r rrrr=要读、写的寄存器地址
	  n nnnn=要读、写的数据*/

/*ADS1298内部寄存器地址定义*/
#define ID			0X00	//ID控制寄存器
#define CONFIG1		0X01	//配置寄存器1
#define CONFIG2		0X02	//配置寄存器2
#define CONFIG3		0X03	//配置寄存器3
#define LOFF		  0X04	//导联脱落控制寄存器
#define CH1SET		0X05	//通道1设置寄存器
#define CH2SET		0X06	//通道2设置寄存器
#define CH3SET		0X07	//通道3设置寄存器
#define CH4SET		0X08	//通道4设置寄存器
#define CH5SET		0X09	//通道5设置寄存器
#define CH6SET		0X0A	//通道6设置寄存器
#define CH7SET		0X0B	//通道7设置寄存器
#define CH8SET		0X0C	//通道8设置寄存器
#define RLD_SENS	0X0D	//右腿驱动选择寄存器
#define LOFF_SENS	0X0F	//导联脱落检测选择寄存器
#define LOFF_STAT	0X12	//导联脱落检测状态寄存器
#define	GPIO	  	0X14	//GPIO控制寄存器
#define WCT1      0x18
#define WCT2      0x19

low_pass_filter_t rlow_pass_filter = {0};
hight_pass_filter_t rhight_pass_filter = {0};



//ADS1298 低电平有效
void respmonInit() {
    Init_lowPass_alpha(&rlow_pass_filter, 0.010f, 4);
    Init_hightPass_alpha(&rhight_pass_filter, 0.010f, 0.1);
    spi_parameter_struct spistr;
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_SPI1);

    //SPI
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_14);
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_15);
    gpio_af_set(GPIOA, GPIO_AF_5, GPIO_PIN_9);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    //CS
    rcu_periph_clock_enable(RCU_GPIOD);
    gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_bit_write(GPIOD, GPIO_PIN_8, SET);
    //Other
    gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5);//RESET
    gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);//START
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);

    //delay_1ms(1000);
    spistr.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spistr.device_mode = SPI_MASTER;
    spistr.frame_size = SPI_FRAMESIZE_8BIT;
    spistr.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
    spistr.nss = SPI_NSS_SOFT;
    spistr.prescale = SPI_PSC_8;
    spistr.endian = SPI_ENDIAN_MSB;
    spi_init(SPI1, &spistr);
    spi_enable(SPI1);

    //respmon_CS_H;
    //respmon_START_H;
    //respmon_RST_L;
    delay_1ms(100);
    respmon_RST_H;
    delay_1ms(100);

    respmon_RST_L;
    delay_us(5);
    respmon_RST_H;
    delay_1ms(100);

    respmon_START_L;
    //delay_1ms(20);
    //respmon_START_H;
    //delay_1ms(20);
    //respmon_START_L;
    //delay_1ms(50);

    //respmon_CS_L;
    //respmon_START_L;

    SEGGER_RTT_SetTerminal(0);
    SEGGER_RTT_printf(0, "respmon ready\r\n");

}

/**对ADS1292R内部寄存器进行操作 **/
uint8_t respmon_REG(unsigned char com, unsigned data) {
    unsigned char i;
    //for(i = 0; i < 90; i++);
    delay_us(10);
    SPI_Transport_respmon(com);
    //for(i = 0; i < 90; i++);
    delay_us(10);
    SPI_Transport_respmon(0X00);
//for(i = 0; i < 90; i++);
    delay_us(10);
    if((com & 0x20) == 0x20) { //判断是否为读寄存器指令
        SPI_Transport_respmon(0X00);
        //for(i = 0; i < 90; i++);
        delay_us(10);
    }
    return (SPI_Transport_respmon(data));

}

/**通过SPI总线与ADS1292R通信**/
unsigned char SPI_Transport_respmon(unsigned char com) {
    while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE)); //检测发送寄存器是否为空，上一个数据发送完成
    spi_i2s_data_transmit(SPI1, com);//发送数据
    while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_RBNE)); //检测接收标志是否非空，发送完成
    return (spi_i2s_data_receive(SPI1));	//返回接收到的数据
}
/*读取72位的数据1100+LOFF_STAT[4:0]+GPIO[1:0]+13个0+2CHx24位，共9字节*/

int resp;
float fresp;
uint8_t rdid;
int bresp, tresp, bbresp;
float bfresp,ttresp,bbfresp = 0;


void getRespValue() {
    SPI_Transport_respmon(0X00);
    delay_us(5);
    uint8_t dat[27];
    int i;
    for(i = 0; i < 9; i++) {
        dat[i] = SPI_Transport_respmon(0X00);
        delay_us(5);
    }
    //SEGGER_RTT_printf(0, "value=");
    //for(i = 0; i < 27; i++) {
    //    SEGGER_RTT_printf(0, "%x ", dat[i]);
    //}
    //SEGGER_RTT_printf(0, dat);
    int ch1p = dat[3] << 16 | dat[4] << 8 | dat[5];
    if(dat[0] != 0xC0) {// ch1p >=16400000 ||   ||ch1p <=500000
        //SEGGER_RTT_printf(0, "skip \r\n");
    } else {
        resp = dat[3] << 16 | dat[4] << 8 | dat[5];
        //SEGGER_RTT_printf(0, "\r\n");
        if(resp >= 0x00800000) {
            resp -= 0x00800000;
        } else {
            resp += 0x00800000;
        }


        if(resp > bresp + 10000 || resp < bresp - 10000) {
            tresp = bbresp;
        } else {
            tresp = resp;
        }
        bbresp = bresp;
        bresp = resp;

        fresp = Low_pass_filter(&rlow_pass_filter, tresp / 10);
        fresp = (int)Hight_pass_filter(&rhight_pass_filter, fresp);
        ttresp = fresp;
        fresp = (fresp+bbfresp)/2;
        bbfresp = bfresp;
        bfresp = ttresp;

    }
}