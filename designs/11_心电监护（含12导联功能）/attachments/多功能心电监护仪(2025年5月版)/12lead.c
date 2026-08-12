#include "12lead.h"
#include "gd32f4xx.h"
#include "gd32f4xx_rcu.h"
#include "gd32f4xx_spi.h"
#include "systick.h"
#include "SEGGER_RTT.h"
#include <math.h>

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

int ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8;
float l1, l2, l3, avr, avl, avf, v1, v2, v3, v4, v5, v6;
float bsl2, bsl3, bsv1, bsv2, bsv3, bsv4, bsv5, bsv6;
float bbsl2, bbsl3, bbsv1, bbsv2, bbsv3, bbsv4, bbsv5, bbsv6;
float kl2[30], kl3[30], kv1[30], kv2[30], kv3[30], kv4[30], kv5[30], kv6[30];

float fbsl2, fbsl3, fbsv1, fbsv2, fbsv3, fbsv4, fbsv5, fbsv6;
float fl1, fl2, fl3, favr, favl, favf, fv1, fv2, fv3, fv4, fv5, fv6 = 0;
float nbsl1, nbsl2, nbsl3, nbsavr, nbsavl, nbsavnbs, nbsv1, nbsv2, nbsv3, nbsv4, nbsv5, nbsv6 = 0;

//ADS1298 低电平有效
void init12Lead() {
    spi_parameter_struct spistr;
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_SPI4);

    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_af_set(GPIOF, GPIO_AF_5, GPIO_PIN_7);
    gpio_af_set(GPIOF, GPIO_AF_5, GPIO_PIN_8);
    gpio_af_set(GPIOF, GPIO_AF_5, GPIO_PIN_9);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    rcu_periph_clock_enable(RCU_GPIOF);
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    gpio_bit_write(GPIOF, GPIO_PIN_6, SET);

    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);

    //delay_1ms(50);
    spistr.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spistr.device_mode = SPI_MASTER;
    spistr.frame_size = SPI_FRAMESIZE_8BIT;
    spistr.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
    spistr.nss = SPI_NSS_SOFT;
    spistr.prescale = SPI_PSC_8;
    spistr.endian = SPI_ENDIAN_MSB;
    spi_init(SPI4, &spistr);
    spi_enable(SPI4);

    //ads12Lead_CS_H;
    ads12Lead_START_H;
    //ads12Lead_RST_L;
    delay_1ms(10);
    ads12Lead_RST_H;
    delay_1ms(100);

    ads12Lead_RST_L;
    delay_us(5);
    ads12Lead_RST_H;
    delay_1ms(100);

    ads12Lead_START_L;
    delay_1ms(1);
    ads12Lead_START_H;
    delay_1ms(1);
    ads12Lead_START_L;
    delay_1ms(10);

    SEGGER_RTT_SetTerminal(0);
    SEGGER_RTT_printf(0, "12 lead ready\r\n");

}

/**对ADS1292R内部寄存器进行操作 **/
uint8_t a12lead_REG(unsigned char com, unsigned data) {
    unsigned char i;
    //for(i = 0; i < 45; i++);
    delay_us(10);
    SPI_Transport_12Lead(com);
    //for(i = 0; i < 45; i++);
    delay_us(10);
    SPI_Transport_12Lead(0X00);
    //for(i = 0; i < 45; i++);
    delay_us(10);
    if((com & 0x20) == 0x20) { //判断是否为读寄存器指令
        SPI_Transport_12Lead(0X00);
        //for(i = 0; i < 45; i++);
        delay_us(10);
    }
    return (SPI_Transport_12Lead(data));

}

/**通过SPI总线与ADS1292R通信**/
unsigned char SPI_Transport_12Lead(unsigned char com) {
    while(RESET == spi_i2s_flag_get(SPI4, SPI_FLAG_TBE)); //检测发送寄存器是否为空，上一个数据发送完成
    spi_i2s_data_transmit(SPI4, com);//发送数据
    while(RESET == spi_i2s_flag_get(SPI4, SPI_FLAG_RBNE)); //检测接收标志是否非空，发送完成
    return (spi_i2s_data_receive(SPI4));	//返回接收到的数据
}
/*读取72位的数据1100+LOFF_STAT[4:0]+GPIO[1:0]+13个0+2CHx24位，共9字节*/
void ADS_Read(unsigned char* data) {
    unsigned char i;
//	unsigned char j;
    ads12Lead_CS_L;
    for(i = 0; i < 27; i++) {
        *data = SPI_Transport_12Lead(0X00);
        data++;
    }
}
float bl2, bl3, bv1, bv2, bv3, bv4, bv5, bv6 = 800;
float eccbl2,eccbl3,eccbv1,eccbv2,eccbv3,eccbv4,eccbv5,eccbv6 = 800;
float eccbl2r,eccbl3r,eccbv1r,eccbv2r,eccbv3r,eccbv4r,eccbv5r,eccbv6r = 800;
uint8_t dat[27];
uint8_t loff;
int n, nk = 0;
int i;
int ch1p;
float nl1,nl2,nl3,nv1,nv2,nv3,nv4,nv5,nv6;
void a12lead_GetmV() {
		eccbl2r = bl2;
		eccbl3r = bl3;
		eccbv1r = bv1;
		eccbv2r = bv2;
		eccbv3r = bv3;
		eccbv4r = bv4;
		eccbv5r = bv5;
		eccbv6r = bv6;
    //point++;
    //SPI_Transport_12Lead(0X00);
    //delay_us(2);
    SPI_Transport_12Lead(0X00);
    delay_us(1);
    
    for(i = 0; i < 27; i++) {
        dat[i] = SPI_Transport_12Lead(0X00);
        delay_us(1);
    }
    //loff = a12lead_REG(0x20|0x12,0x00);
    //SEGGER_RTT_printf(0, "value=");
    //for(i = 0; i < 27; i++) {
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
            ch4 = dat[12] << 16 | dat[13] << 8 | dat[14];
            if(ch4 >= 0x00800000) {
                ch4 -= 0x00800000;
            } else {
                ch4 += 0x00800000;
            }
            ch5 = dat[15] << 16 | dat[16] << 8 | dat[17];
            if(ch5 >= 0x00800000) {
                ch5 -= 0x00800000;
            } else {
                ch5 += 0x00800000;
            }
            ch6 = dat[18] << 16 | dat[19] << 8 | dat[20];
            if(ch6 >= 0x00800000) {
                ch6 -= 0x00800000;
            } else {
                ch6 += 0x00800000;
            }
            ch7 = dat[21] << 16 | dat[22] << 8 | dat[23];
            if(ch7 >= 0x00800000) {
                ch7 -= 0x00800000;
            } else {
                ch7 += 0x00800000;
            }
            ch8 = dat[24] << 16 | dat[25] << 8 | dat[26];
            if(ch8 >= 0x00800000) {
                ch8 -= 0x00800000;
            } else {
                ch8 += 0x00800000;
            }


            //赋值，毛刺去除
            nl2 = (float)ch1 * 0.000047684;
            if(nl2 > bl2 + 4.2 || nl2 < bl2 - 4.2) {

            } else {
                l2 = nl2;
            }
            bl2 = nl2;

            nl3 = (float)ch2 * 0.000047684;
            if(nl3 > bl3 + 4.2 || nl3 < bl3 - 4.2) {

            } else {
                l3 = nl3;
            }
            bl3 = nl3;

            nv1 = (float)ch8 * 0.000047684;
            if(nv1 > bv1 + 4.2 || nv1 < bv1 - 4.2) {

            } else {
                v1 = nv1;
            }
            bv1 = nv1;

            nv2 = (float)ch3 * 0.000047684;
            if(nv2 > bv2 + 4.2 || nv2 < bv2 - 4.2) {

            } else {
                v2 = nv2;
            }
            bv2 = nv2;

            nv3 = (float)ch4 * 0.000047684;
            if(nv3 > bv3 + 4.2 || nv3 < bv3 - 4.2) {

            } else {
                v3 = nv3;
            }
            bv3 = nv3;

            nv4 = (float)ch5 * 0.000047684;
            if(nv4 > bv4 + 4.2 || nv4 < bv4 - 4.2) {

            } else {
                v4 = nv4;
            }
            bv4 = nv4;

            nv5 = (float)ch6 * 0.000047684;
            if(nv5 > bv5 + 4.2 || nv5 < bv5 - 4.2) {

            } else {
                v5 = nv5;
            }
            bv5 = nv5;

            nv6 = (float)ch7 * 0.000047684;
            if(nv6 > bv6 + 4.2 || nv6 < bv6 - 4.2) {

            } else {
                v6 = nv6;
            }
            bv6 = nv6;
						/*
						//ECC
						delay_us(50);
						//ECC-二次获取
						SPI_Transport_12Lead(0X00);
						delay_us(1);
						
						for(i = 0; i < 27; i++) {
							dat[i] = SPI_Transport_12Lead(0X00);
							SEGGER_RTT_printf(0, "%x",dat[i]);
							delay_us(1);
						}
						SEGGER_RTT_printf(0, "\r\n");
						ch1p = dat[3] << 16 | dat[4] << 8 | dat[5];
						if(dat[0] != 0xC0) {// ch1p >=16400000 ||   ||ch1p <=500000
								SEGGER_RTT_printf(0, "ERROR: ECC Fault \r\n");
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
								ch4 = dat[12] << 16 | dat[13] << 8 | dat[14];
								if(ch4 >= 0x00800000) {
										ch4 -= 0x00800000;
								} else {
										ch4 += 0x00800000;
								}
								ch5 = dat[15] << 16 | dat[16] << 8 | dat[17];
								if(ch5 >= 0x00800000) {
										ch5 -= 0x00800000;
								} else {
										ch5 += 0x00800000;
								}
								ch6 = dat[18] << 16 | dat[19] << 8 | dat[20];
								if(ch6 >= 0x00800000) {
										ch6 -= 0x00800000;
								} else {
										ch6 += 0x00800000;
								}
								ch7 = dat[21] << 16 | dat[22] << 8 | dat[23];
								if(ch7 >= 0x00800000) {
										ch7 -= 0x00800000;
								} else {
										ch7 += 0x00800000;
								}
								ch8 = dat[24] << 16 | dat[25] << 8 | dat[26];
								if(ch8 >= 0x00800000) {
										ch8 -= 0x00800000;
								} else {
										ch8 += 0x00800000;
								}
								
								//ECC-数据提取
								eccbl2 = (float)ch1 * 0.000047684;
								eccbl3 = (float)ch2 * 0.000047684;
								eccbv1 = (float)ch8 * 0.000047684;
								eccbv2 = (float)ch3 * 0.000047684;
								eccbv3 = (float)ch4 * 0.000047684;
								eccbv4 = (float)ch5 * 0.000047684;
								eccbv5 = (float)ch6 * 0.000047684;
								eccbv6 = (float)ch7 * 0.000047684;
								
								//ECC-数据比对筛选
								if(fabsf(eccbl2 - eccbl2r) > fabsf(bl2 - eccbl2r)){l2 = eccbl2;}
								if(fabsf(eccbl3 - eccbl3r) > fabsf(bl3 - eccbl3r)){l3 = eccbl3;}
								if(fabsf(eccbv1 - eccbv1r) > fabsf(bv1 - eccbv1r)){v1 = eccbv1;}
								if(fabsf(eccbv2 - eccbv2r) > fabsf(bv2 - eccbv2r)){v2 = eccbv2;}
								if(fabsf(eccbv3 - eccbv3r) > fabsf(bv3 - eccbv3r)){v3 = eccbv3;}
								if(fabsf(eccbv4 - eccbv4r) > fabsf(bv4 - eccbv4r)){v4 = eccbv4;}
								if(fabsf(eccbv5 - eccbv5r) > fabsf(bv5 - eccbv5r)){v5 = eccbv5;}
								if(fabsf(eccbv6 - eccbv6r) > fabsf(bv6 - eccbv6r)){v6 = eccbv6;}
						}

*/
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

                 nbsv2 = v2;
                if(nbsv2 > bbsv2 + 0.1 || nbsv2 < bbsv2 - 0.1) {

                } else {
                    bsv2 = nbsv2;
                }
                bbsv2 = nbsv2;

                 nbsv3 = v3;
                if(nbsv3 > bbsv3 + 0.1 || nbsv3 < bbsv3 - 0.1) {

                } else {
                    bsv3 = nbsv3;
                }
                bbsv3 = nbsv3;

                 nbsv4 = v4;
                if(nbsv4 > bbsv4 + 0.1 || nbsv4 < bbsv4 - 0.1) {

                } else {
                    bsv4 = nbsv4;
                }
                bbsv4 = nbsv4;

                 nbsv5 = v5;
                if(nbsv5 > bbsv5 + 0.1 || nbsv5 < bbsv5 - 0.1) {

                } else {
                    bsv5 = nbsv5;
                }
                bbsv5 = nbsv5;

                 nbsv6 = v6;
                if(nbsv6 > bbsv6 + 0.1 || nbsv6 < bbsv6 - 0.1) {

                } else {
                    bsv6 = nbsv6;
                }
                bbsv6 = nbsv6;
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

            for(int i = 0; i < 29; i++) {
                kv2[i] = kv2[i + 1];
            }
            kv2[29] = bsv2;
            fbsv2 = (kv2[0] + kv2[1] + kv2[2] + kv2[3] + kv2[4] + kv2[5] + kv2[6] + kv2[7] + kv2[8] + kv2[9] + kv2[10] + kv2[11] + kv2[12] + kv2[13] + kv2[14] + kv2[15] + kv2[16] + kv2[17] + kv2[18] + kv2[19] + kv2[20] + kv2[21] + kv2[22] + kv2[23] + kv2[24] + kv2[25] + kv2[26] + kv2[27] + kv2[28] + kv2[29]) / 30;
            fv2 = v2 - fbsv2;

            for(int i = 0; i < 29; i++) {
                kv3[i] = kv3[i + 1];
            }
            kv3[29] = bsv3;
            fbsv3 = (kv3[0] + kv3[1] + kv3[2] + kv3[3] + kv3[4] + kv3[5] + kv3[6] + kv3[7] + kv3[8] + kv3[9] + kv3[10] + kv3[11] + kv3[12] + kv3[13] + kv3[14] + kv3[15] + kv3[16] + kv3[17] + kv3[18] + kv3[19] + kv3[20] + kv3[21] + kv3[22] + kv3[23] + kv3[24] + kv3[25] + kv3[26] + kv3[27] + kv3[28] + kv3[29]) / 30;
            fv3 = v3 - fbsv3;

            for(int i = 0; i < 29; i++) {
                kv4[i] = kv4[i + 1];
            }
            kv4[29] = bsv4;
            fbsv4 = (kv4[0] + kv4[1] + kv4[2] + kv4[3] + kv4[4] + kv4[5] + kv4[6] + kv4[7] + kv4[8] + kv4[9] + kv4[10] + kv4[11] + kv4[12] + kv4[13] + kv4[14] + kv4[15] + kv4[16] + kv4[17] + kv4[18] + kv4[19] + kv4[20] + kv4[21] + kv4[22] + kv4[23] + kv4[24] + kv4[25] + kv4[26] + kv4[27] + kv4[28] + kv4[29]) / 30;
            fv4 = v4 - fbsv4;

            for(int i = 0; i < 29; i++) {
                kv5[i] = kv5[i + 1];
            }
            kv5[29] = bsv5;
            fbsv5 = (kv5[0] + kv5[1] + kv5[2] + kv5[3] + kv5[4] + kv5[5] + kv5[6] + kv5[7] + kv5[8] + kv5[9] + kv5[10] + kv5[11] + kv5[12] + kv5[13] + kv5[14] + kv5[15] + kv5[16] + kv5[17] + kv5[18] + kv5[19] + kv5[20] + kv5[21] + kv5[22] + kv5[23] + kv5[24] + kv5[25] + kv5[26] + kv5[27] + kv5[28] + kv5[29]) / 30;
            fv5 = v5 - fbsv5;

            for(int i = 0; i < 29; i++) {
                kv6[i] = kv6[i + 1];
            }
            kv6[29] = bsv6;
            fbsv6 = (kv6[0] + kv6[1] + kv6[2] + kv6[3] + kv6[4] + kv6[5] + kv6[6] + kv6[7] + kv6[8] + kv6[9] + kv6[10] + kv6[11] + kv6[12] + kv6[13] + kv6[14] + kv6[15] + kv6[16] + kv6[17] + kv6[18] + kv6[19] + kv6[20] + kv6[21] + kv6[22] + kv6[23] + kv6[24] + kv6[25] + kv6[26] + kv6[27] + kv6[28] + kv6[29]) / 30;
            fv6 = v6 - fbsv6;
            //SEGGER_RTT_printf(0, "\r\n");
        }
}

