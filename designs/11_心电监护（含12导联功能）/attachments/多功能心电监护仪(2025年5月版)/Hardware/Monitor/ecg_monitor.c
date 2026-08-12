#include "ecg_monitor.h"
#include "gd32f4xx.h"
#include "gd32f4xx_rcu.h"
#include "gd32f4xx_spi.h"
#include "systick.h"
#include "SEGGER_RTT.h"

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

extern int ch1, ch2, ch3;
extern float l1, l2, l3, avr, avl, avf, v1;
extern float bsl2, bsl3, bsv1;
extern float bbsl2, bbsl3, bbsv1;
extern float kl2[30], kl3[30], kv1[30];

extern float fbsl2, fbsl3, fbsv1;
extern float fl1, fl2, fl3, favr, favl, favf, fv1;
extern float nbsl1, nbsl2, nbsl3, nbsavr, nbsavl, nbsavnbs, nbsv1;

//ADS1298 低电平有效
void monecgInit() {
    spi_parameter_struct spistr;
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_SPI3);
  
    //SPI
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_af_set(GPIOE, GPIO_AF_5, GPIO_PIN_2);
    gpio_af_set(GPIOE, GPIO_AF_5, GPIO_PIN_5);
    gpio_af_set(GPIOE, GPIO_AF_5, GPIO_PIN_6);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    
    //CS
    rcu_periph_clock_enable(RCU_GPIOE);
    gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    gpio_bit_write(GPIOE, GPIO_PIN_4, SET);
    //Other
    gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_13);//RESET
    gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_3);//START
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    //delay_1ms(1000);
    spistr.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spistr.device_mode = SPI_MASTER;
    spistr.frame_size = SPI_FRAMESIZE_8BIT;
    spistr.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
    spistr.nss = SPI_NSS_SOFT;
    spistr.prescale = SPI_PSC_8;
    spistr.endian = SPI_ENDIAN_MSB;
    spi_init(SPI3, &spistr);
    spi_enable(SPI3);
    
    //monecg_CS_H;
    monecg_START_H;
    //monecg_RST_L;
    delay_1ms(100);
    monecg_RST_H;
    delay_1ms(100);
    
    monecg_RST_L;
    delay_us(5);
    monecg_RST_H;
    delay_1ms(200);
    
    monecg_START_L;
    delay_1ms(5);
    monecg_START_H;
    delay_1ms(5);
    monecg_START_L;
    delay_1ms(10);
    

    SEGGER_RTT_SetTerminal(0);
    SEGGER_RTT_printf(0, "monecg ready\r\n");

}

/**对ADS1292R内部寄存器进行操作 **/
uint8_t monecg_REG(unsigned char com,unsigned data)
{
	unsigned char i;
	//for(i=0;i<45;i++);
  delay_us(10);
	SPI_Transport_monecg(com);
	//for(i=0;i<45;i++);
  delay_us(10);
	SPI_Transport_monecg(0X00);
	//for(i=0;i<45;i++);
  delay_us(10);
	if((com&0x20)==0x20)//判断是否为读寄存器指令
	{	SPI_Transport_monecg(0X00);
		//for(i=0;i<45;i++);	
    delay_us(10);
	}
	return (SPI_Transport_monecg(data));

}

/**通过SPI总线与ADS1292R通信**/
unsigned char SPI_Transport_monecg(unsigned char com)
{
	while (RESET == spi_i2s_flag_get(SPI3, SPI_FLAG_TBE));//检测发送寄存器是否为空，上一个数据发送完成
	spi_i2s_data_transmit(SPI3, com);//发送数据      
	while (RESET == spi_i2s_flag_get(SPI3, SPI_FLAG_RBNE));//检测接收标志是否非空，发送完成  
	return (spi_i2s_data_receive(SPI3));	//返回接收到的数据
}
/*读取72位的数据1100+LOFF_STAT[4:0]+GPIO[1:0]+13个0+2CHx24位，共9字节*/	

extern float bl2, bl3, bv1;
extern uint8_t dat[27];
extern uint8_t loff;
extern int n, nk ;
int jntm;
extern int ch1p;
extern float nl1,nl2,nl3,nv1;
void monitor_GetmV() {
    //point++;
    //SPI_Transport_12Lead(0X00);
    //delay_us(2);
    SPI_Transport_monecg(0X00);
    //for(int i=0;i<90;i++){__NOP;};
    delay_us(1);
    
    for(jntm = 0; jntm< 27; jntm++) {
        dat[jntm] = SPI_Transport_monecg(0X00);
        //for(int i=0;i<90;i++){__NOP;};
        delay_us(1);
    }
    //loff = monecg_REG(0x20|0x12,0x00);
    //SEGGER_RTT_printf(0, "value=");
    //for(int i = 0; i < 27; i++) {
    //    SEGGER_RTT_printf(0, "%x ", dat[i]);
    //}
    //SEGGER_RTT_printf(0,"LOFF: %x \r\n", loff);
        ch1p = dat[3] << 16 | dat[4] << 8 | dat[5];
        if(dat[0] != 0xC0) {// ch1p >=16400000 ||   ||ch1p <=500000
            //SEGGER_RTT_printf(0, "skip \r\n");
        } else {//1 = 47.683nV =
            ch1 = dat[3] << 16 | dat[4] << 8 | dat[5];
            if(ch1 >= 0x00800000) {
                ch1 -= 0x00800000;
            } else {
                ch1 += 0x00800000;
            }
            ch2 = dat[6] << 16 | dat[7] << 8 | dat[8];
            if(ch2 >= 0x00800000) {
                ch2 -= 0x00800000;
            } else {
                ch2 += 0x00800000;
            }
            ch3 = dat[9] << 16 | dat[10] << 8 | dat[11];
            if(ch3 >= 0x00800000) {
                ch3 -= 0x00800000;
            } else {
                ch3 += 0x00800000;
            }


            //赋值，毛刺去除
            nl2 = (double)ch1 * 0.000047684;
            if(nl2 > bl2 + 4.2 || nl2 < bl2 - 4.2) {

            } else {
                l2 = nl2;
            }
            bl2 = nl2;

            nl3 = (double)ch2 * 0.000047684;
            if(nl3 > bl3 + 4.2 || nl3 < bl3 - 4.2) {

            } else {
                l3 = nl3;
            }
            bl3 = nl3;

            nv1 = (double)ch3 * 0.000047684;
            if(nv1 > bv1 + 4.2 || nv1 < bv1 - 4.2) {

            } else {
                v1 = nv1;
            }
            bv1 = nv1;


            //基线处理
            n++;
            if(n >= 5) {
                n = 0;
                 nbsl2 = l2;
                if(nbsl2 > bbsl2 + 0.1 || nbsl2 < bbsl2 - 0.1) {

                } else {
                    bsl2 = nbsl2;
                }
                bbsl2 = nbsl2;

                 nbsl3 = l3;
                if(nbsl3 > bbsl3 + 0.1 || nbsl3 < bbsl3 - 0.1) {

                } else {
                    bsl3 = nbsl3;
                }
                bbsl3 = nbsl3;

                 nbsv1 = v1;
                if(nbsv1 > bbsv1 + 0.1 || nbsv1 < bbsv1 - 0.1) {

                } else {
                    bsv1 = nbsv1;
                }
                bbsv1 = nbsv1;
                
            }
            //统一中值滤波
            for(int i = 0; i < 29; i++) {
                kl2[i] = kl2[i + 1];
            }
            kl2[29] = bsl2;
            fbsl2 = (kl2[0] + kl2[1] + kl2[2] + kl2[3] + kl2[4] + kl2[5] + kl2[6] + kl2[7] + kl2[8] + kl2[9] + kl2[10] + kl2[11] + kl2[12] + kl2[13] + kl2[14] + kl2[15] + kl2[16] + kl2[17] + kl2[18] + kl2[19] + kl2[20] + kl2[21] + kl2[22] + kl2[23] + kl2[24] + kl2[25] + kl2[26] + kl2[27] + kl2[28] + kl2[29]) / 30;
            fl2 = l2 - fbsl2;

            for(int i = 0; i < 29; i++) {
                kl3[i] = kl3[i + 1];
            }
            kl3[29] = bsl3;
            fbsl3 = (kl3[0] + kl3[1] + kl3[2] + kl3[3] + kl3[4] + kl3[5] + kl3[6] + kl3[7] + kl3[8] + kl3[9] + kl3[10] + kl3[11] + kl3[12] + kl3[13] + kl3[14] + kl3[15] + kl3[16] + kl3[17] + kl3[18] + kl3[19] + kl3[20] + kl3[21] + kl3[22] + kl3[23] + kl3[24] + kl3[25] + kl3[26] + kl3[27] + kl3[28] + kl3[29]) / 30;
            fl3 = l3 - fbsl3;

            fl1 = fl2 - fl3;
            favr = -0.5 * (fl1 + fl2);
            favl = fl1 - 0.5 * fl2;
            favf = fl2 - 0.5 * fl1;

            for(int i = 0; i < 29; i++) {
                kv1[i] = kv1[i + 1];
            }
            kv1[29] = bsv1;
            fbsv1 = (kv1[0] + kv1[1] + kv1[2] + kv1[3] + kv1[4] + kv1[5] + kv1[6] + kv1[7] + kv1[8] + kv1[9] + kv1[10] + kv1[11] + kv1[12] + kv1[13] + kv1[14] + kv1[15] + kv1[16] + kv1[17] + kv1[18] + kv1[19] + kv1[20] + kv1[21] + kv1[22] + kv1[23] + kv1[24] + kv1[25] + kv1[26] + kv1[27] + kv1[28] + kv1[29]) / 30;
            fv1 = v1 - fbsv1;
        }
}

