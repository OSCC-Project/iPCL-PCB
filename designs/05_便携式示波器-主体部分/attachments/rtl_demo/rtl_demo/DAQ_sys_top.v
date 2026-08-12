//==========================================================================
//  Author       ：alpha
//  Mail         ：1879136899@qq.com
//  Filename     ：
//  Create time  ：
//  Description  ：
//==========================================================================
`timescale 1ns/100ps


module DAQ_sys_top(
    //system signal
    input	wire				sys_clk_50m			,
    input	wire				sys_rst_n			,
    //user signal(dac0_AD5304)(dac0) 
	output	wire	[ 0:0]	    dac0_sclk    	    ,
	output	wire	[ 0:0]	    dac0_cs_n	        ,
	output	wire	[ 0:0]	    dac0_d_out  	    ,	
    //user signal(dac0_AD5304)(dac1) 
	output	wire	[ 0:0]	    dac1_sclk    	    ,
	output	wire	[ 0:0]	    dac1_cs_n	        ,
	output	wire	[ 0:0]	    dac1_d_out  	    ,
    //user signal(DAC904E) 
	output	wire	[ 0:0]	    dac2_clk_165m	    ,
	output	wire	[ 9:0]	    dac2_data	        ,
    output	wire	[ 0:0]	    awg_protect	        ,
    //user signal(AD9288)(adc1) 
	output	wire	[ 0:0]	    adc1_clka           ,
    output	wire	[ 0:0]	    adc1_clkb           ,
	input	wire	[ 7:0]	    adc1_data_a 	    ,
    input	wire	[ 7:0]	    adc1_data_b         ,
    output	wire	[ 7:0]	    adc1_data           ,
    //user signal(AD9288)(adc2) 
	output	wire	[ 0:0]	    adc2_clka           ,
    output	wire	[ 0:0]	    adc2_clkb           ,
	input	wire	[ 7:0]	    adc2_data_a	        ,
    input	wire	[ 7:0]	    adc2_data_b	        ,
    output	wire	[ 7:0]	    adc2_data           ,
    //user signal(uart)
    input	wire	[ 0:0]		rxd_line            ,
    output	wire	[ 0:0]		txd_line            ,
    //user signal(led && key)
    input	wire	[ 2:0]		key_in              ,
    output	wire	[ 2:0]		osc_led             ,
    output	wire	[ 0:0]		c401_led            ,
    //user signal(tft_lcd)
    output  wire    [ 0:0]      tft_clk_32m         ,
    output  wire    [ 0:0]      tft_hsync           ,
    output  wire    [ 0:0]      tft_vsync           ,
    output  wire    [ 0:0]      tft_en              ,
    output  wire    [23:0]      tft_rgb888          ,
    //user signal(ui_btn)
    input   wire    [ 0:0]      btn_up              ,   // 通用操作：向上
    input   wire    [ 0:0]      btn_done            ,   // 通用操作：向下
    input   wire    [ 0:0]      btn_left            ,   // 通用操作：向左
    input   wire    [ 0:0]      btn_right           ,   // 通用操作：向右
    input   wire    [ 0:0]      btn_sure            ,   // 通用操作：确定
    input   wire    [ 0:0]      osc_freq_btn        ,   // 示波器：时基选择
    input   wire    [ 0:0]      osc_time_btn        ,   // 示波器：时间选择
    input   wire    [ 0:0]      ch1_offset_btn      ,   // 通道一：偏置
    input   wire    [ 0:0]      ch1_gain_btn        ,   // 通道一：增益
    input   wire    [ 0:0]      ch2_offset_btn      ,   // 通道二：偏置
    input   wire    [ 0:0]      ch2_gain_btn        ,   // 通道二：增益
    input   wire    [ 0:0]      swg_wave_btn        ,   // 信号源：波形选择
    input   wire    [ 0:0]      swg_freq_btn        ,   // 信号源：频率选择
    input   wire    [ 0:0]      swg_amp_btn         ,   // 信号源：电压
    //user signal(freq_measure in) 
    input   wire    [ 0:0]      ch1_freq_input      , 
    input   wire    [ 0:0]      ch2_freq_input      ,
    //user signal(CHANNEL CTRL) 
    output  wire    [ 0:0]      ch1_dc              ,
    output  wire    [ 0:0]      ch1_attenuator      ,
    output  wire    [ 0:0]      ch2_dc              ,
    output  wire    [ 0:0]      ch2_attenuator      ,
    output  wire    [ 0:0]      channel_ctrl                    
);

//--------------------------------------------------------------------
//------------------------- parameter define -------------------------
//--------------------------------------------------------------------
parameter       BPS_9600        =   3'd0            ;
parameter       BPS_19200       =   3'd1            ;
parameter       BPS_38400       =   3'd2            ;
parameter       BPS_57600       =   3'd3            ;
parameter       BPS_115200      =   3'd4            ;

parameter       DAC1_FREQ_CTRL  =   32'd10_000      ;       // 单位：Hz
parameter       DAC1_PHASE_CTRL =   12'd00          ;       // 单位：°
parameter       DAC1_AMP_CTRL   =   3'd2            ;       // 单位：V


//--------------------------------------------------------------------
//------------------------- variable define --------------------------
//--------------------------------------------------------------------
wire                            pll_clk_100m        ; 
wire                            pll_clk_200m        ; 
wire                            adc_clk_100m        ; 

wire            [  7:0]         receive_data        ;
wire            [  0:0]         receive_finish      ;

wire            [  2:0]         key_flag            ;

wire            [  0:0]         dac0_done           ;
wire            [  0:0]         dac1_done           ;

// wire            [  7:0]         adc1_data/*synthesis noprune */           ;
// wire            [  7:0]         adc2_data           ;

wire            [  7:0]         ch1_trig_level      ;
wire            [  7:0]         ch2_trig_level      ;

wire            [ 26:0]         ch1_freq_result     ;
wire            [ 26:0]         ch2_freq_result     ;

wire            [  7:0]         test_sine_data      ;
wire            [  7:0]         test_square_data    ;

wire            [  3:0]         swg_wave_para       ;
wire            [  3:0]         swg_freq_para       ; 
wire            [  3:0]         swg_amp_para        ; 

//--------------------------------------------------------------------
//-------------------------- logic function --------------------------
//--------------------------------------------------------------------

// pll clock manager module： 
pll_clk_manager             pll_clk_manager_inst(
    .areset                 (~sys_rst_n         )   ,
    .inclk0                 (sys_clk_50m        )   ,

    .c0                     (tft_clk_32m        )   ,
    .c1                     (dac2_clk_165m      )   ,
    .c2                     (adc_clk_100m       )   ,
    .c3                     (pll_clk_200m       )   ,
    .c4                     (pll_clk_100m       )   ,
    .locked                 (                   )    
);

// UART transfrom module：                   
uart                        uart_init(
    .sys_clk_50m            (sys_clk_50m        )   ,   // 系统时钟：50MHz
    .sys_rst_n              (sys_rst_n          )   ,   // 系统复位：低电平有效  
    .bps_select             (BPS_115200         )   ,   // 波特率选择
    .data_a                 (                   )   ,   // 待发送数据a
    .data_b                 (receive_data       )   ,   // 待发送数据b
    .data_c                 (                   )   ,   // 待发送数据c
    .data_d                 (                   )   ,   // 待发送数据d
    .data_e                 (                   )   ,   // 待发送数据e
    .data_f                 (                   )   ,   // 待发送数据f
    .data_g                 (                   )   ,   // 待发送数据g
    .receive_data           (receive_data       )   ,   // 串口接收数据
    .receive_finish         (receive_finish     )   ,   // 串口接收数据完成
    .in_uart_rx             (rxd_line           )   ,   // 串口接收数据信号 
    .out_uart_tx            (txd_line           )       // 串口发送数据信号 
);  

// // dac904e drive module：
// DAC904E_drive               DAC904E_drive_init(
//     .pll_clk_165m	        (dac2_clk_165m      )	,
//     .sys_rst_n		        (sys_rst_n          )	,
//     .dac2_freq_ctrl         (32'd3436000        )   ,   // DAC2 --> freq
//     .dac2_phase_ctrl        (12'd0              )   ,	// DAC2 --> phase	
//     .dac2_amp_ctrl          (                   )   ,   // DAC2 --> amp
//     .dac2_data              (dac2_data          )     
// );

// 3PD5651 drive module：
dds_drive                   dds_drive_init(
    .pll_clk_165m		    (pll_clk_100m       )   ,
    .sys_rst_n			    (sys_rst_n	        )   ,
    .freq_ctrl              (receive_data       )   ,
    .swg_freq_para          (swg_freq_para      )   ,
    .swg_wave_para          (swg_wave_para      )   ,
    .phase_ctrl             (0                  )   ,	
    .amp_ctrl               (0                  )   ,			
    .da_data                (dac2_data          )   
);
 
assign  awg_protect = 1'b0;

// control_DAC data gain(dac0):
dac_ad5304_drive			dac0_ad5304_drive_init(
    .sys_clk_50m            (pll_clk_100m       )   ,
    .sys_rst_n			    (sys_rst_n          )   ,
    .start                  (1'b1               )   ,
    .channal_num            (2'b00              )   ,
    .dac_data               (8'd32              )   ,
    .done                   (dac0_done          )   ,         
    .sclk                   (dac0_sclk          )   ,
    .cs_n                   (dac0_cs_n          )   ,		
    .d_out                  (dac0_d_out         )
);

// control_DAC data gain(dac1):
dac_ad5304_drive			dac1_ad5304_drive_init(
    .sys_clk_50m            (pll_clk_100m       )   ,
    .sys_rst_n			    (sys_rst_n          )   ,
    .start                  (1'b1               )   ,
    .channal_num            (2'b01              )   ,
    .dac_data               (8'd64              )   ,
    .done                   (dac1_done          )   ,         
    .sclk                   (dac1_sclk          )   ,
    .cs_n                   (dac1_cs_n          )   ,		
    .d_out                  (dac1_d_out         )
);

// // ADC sample_rate set module:
// adc_sample_rate_set         adc_sample_rate_set_init(
//     .pll_clk_200m           (pll_clk_200m       )   ,
//     .ch1_freq_result        (ch1_freq_result    )   ,
//     .adc1_clka              (adc1_clka          )   ,
//     .adc1_clkb              (adc1_clkb          )   ,
//     .ch2_freq_result        (ch2_freq_result    )   ,
//     .adc2_clka              (adc2_clka          )   ,
//     .adc2_clkb              (adc2_clkb          )   
// );

// AD9288 drive module(adc1):
assign  channel_ctrl = 1'b0;
assign  ch1_dc = 1'b0;
assign  ch1_attenuator = 1'b1;

assign adc1_clka = sys_clk_50m;
assign adc1_clkb = sys_clk_50m;

ad9288_drive                adc1_ad9288_drive_init(
    .sys_clk_50m			(pll_clk_100m       )   ,
    .sys_rst_n			    (sys_rst_n	        )   ,
    .adc_data_a             (adc1_data_a        )   ,			
    .adc_data_b             (adc1_data_b        )   ,
    .adc_data               (adc1_data          )   
);

// AD9288 drive module(adc2):
assign  ch2_dc = 1'b0;
assign  ch2_attenuator = 1'b0;

assign adc2_clka = sys_clk_50m;
assign adc2_clkb = sys_clk_50m;

ad9288_drive                adc2_ad9288_drive_init(
    .sys_clk_50m			(sys_clk_50m        )   ,
    .sys_rst_n			    (sys_rst_n	        )   ,
    .adc_data_a             (adc2_data_a        )   ,			
    .adc_data_b             (adc2_data_b        )   ,
    .adc_data               (adc2_data          )   
);

// // key check module：
// key_check                   key_check_init(
//     .sys_clk_50m			(sys_clk_50m        )   ,
//     .sys_rst_n			    (sys_rst_n          )   ,
//     .key_in                 ({ch2_offset_btn, 2'b0}     )   ,
//     .key_flag			    (key_flag           )
// );

// // led disp module：
// led                         led_init(
//     .sys_clk_50m		    (sys_clk_50m        )	,
//     .sys_rst_n			    (sys_rst_n          )   ,
//     .led_flag               (key_flag[0]        )   ,
//     .c401_led       	    (c401_led           )   ,
//     .key_flag               (key_flag           )   ,
//     .osc_led                (osc_led            )
// );

// freq calc module:CH1
freq_measure                ch1_freq_measure_init(
    .pll_clk_200m           (sys_clk_50m        )   ,  
    .sys_rst_n              (sys_rst_n          )   ,  
    .measure_signal         (ch1_freq_input     )   ,  
    .freq_result            (ch1_freq_result    )   
);  

// freq calc module:CH2
freq_measure                ch2_freq_measure_init(
    .pll_clk_200m           (sys_clk_50m        )   ,  
    .sys_rst_n              (sys_rst_n          )   ,  
    .measure_signal         (ch2_freq_input     )   ,  
    .freq_result            (ch2_freq_result    )   
);  

// test wave gain module:(data rate = 100MHz)
simulation_wave_gain        simulation_wave_gain_init(
    .sys_clk_50m            (pll_clk_200m       )   ,
    .sys_rst_n              (sys_rst_n          )   ,
    .test_sine_data         (test_sine_data     )   ,
    .test_square_data       (test_square_data   )   
);

// display module:
ui_display_top              ui_display_top_init(
    .sys_clk_50m			(tft_clk_32m        )   ,
    .sys_rst_n			    (sys_rst_n          )   ,
    .adc1_clk               (pll_clk_200m       )   ,
    .adc2_clk               (pll_clk_200m       )   ,
    .ch1_trig_level         (ch1_trig_level     )   ,
    .ch2_trig_level         (ch2_trig_level     )   ,
    .ch1_freq_result        (ch1_freq_result    )   ,
    .ch2_freq_result        (ch2_freq_result    )   ,
    .ch1_adc_data           (test_sine_data     )   ,
    .ch2_adc_data           (adc2_data          )   ,

    .vga_vsync              (tft_vsync          )   ,
    .vga_hsync              (tft_hsync          )   ,
    .vga_en                 (tft_en             )   ,  
    .vga_rgb888             (tft_rgb888         )	,

    .btn_up                 (btn_up             )   ,   // 通用操作：向上
    .btn_done               (btn_done           )   ,   // 通用操作：向下
    .btn_left               (btn_left           )   ,   // 通用操作：向左
    .btn_right              (btn_right          )   ,   // 通用操作：向右
    .btn_sure               (btn_sure           )   ,   // 通用操作：确定
    .osc_freq_btn           (osc_freq_btn       )   ,   // 示波器：时基选择
    .osc_time_btn           (osc_time_btn       )   ,   // 示波器：时间选择
    .ch1_offset_btn         (ch1_offset_btn     )   ,   // 通道一：偏置
    .ch1_gain_btn           (ch1_gain_btn       )   ,   // 通道一：增益
    .ch2_offset_btn         (ch2_offset_btn     )   ,   // 通道二：偏置
    .ch2_gain_btn           (ch2_gain_btn       )   ,   // 通道二：增益
    .swg_wave_btn           (swg_wave_btn       )   ,   // 信号源：波形选择
    .swg_freq_btn           (swg_freq_btn       )   ,   // 信号源：频率选择
    .swg_amp_btn            (swg_amp_btn        )   ,   // 信号源：电压

    .swg_wave_para          (swg_wave_para      )   ,
    .swg_freq_para          (swg_freq_para      )   ,
    .swg_amp_para           (swg_amp_para       )   
);

endmodule