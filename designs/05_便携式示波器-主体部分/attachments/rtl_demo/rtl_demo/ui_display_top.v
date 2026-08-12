//==========================================================================
//  Author       ：alpha
//  Mail         ：1879136899@qq.com
//  Filename     ：
//  Create time  ：
//  Description  ：
//==========================================================================
`timescale 1ns/100ps

module ui_display_top(
    //system signal
    input	wire				sys_clk_50m			,
    input	wire				sys_rst_n			,
    //user signal
    input	wire				adc1_clk			,
    input	wire				adc2_clk			,
    input   wire    [ 7:0]      ch1_trig_level      ,
    input   wire    [ 7:0]      ch2_trig_level      ,
    input   wire    [ 7:0]      ch1_adc_data        ,
    input   wire    [ 7:0]      ch2_adc_data        ,
    input   wire    [26:0]      ch1_freq_result     ,
    input   wire    [26:0]      ch2_freq_result     ,

    output  wire                vga_vsync           ,
    output  wire                vga_hsync           ,
    output  wire                vga_en              ,  
    output  wire    [23:0]      vga_rgb888 			,

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

    output  wire    [ 3:0]      swg_wave_para       ,
    output  wire    [ 3:0]      swg_freq_para       , 
    output  wire    [ 3:0]      swg_amp_para            
);

//--------------------------------------------------------------------
//------------------------- parameter define -------------------------
//--------------------------------------------------------------------
parameter       TIME_1S_MAX     =   26'd2_000_000   ;

//--------------------------------------------------------------------
//------------------------- variable define --------------------------
//--------------------------------------------------------------------
wire    [12:0]      column              ;
wire    [12:0]      row                 ;

wire    [ 7:0]      ch1_wave_data       ;
wire    [ 7:0]      ch2_wave_data       ;

wire    [ 7:0]      ch1_display_data    ;
wire    [ 7:0]      ch2_display_data    ;

wire    [47:0]      ch1_calc_para       ;
wire    [47:0]      ch2_calc_para       ;

reg     [25:0]      time_1s             ;

wire    [ 3:0]      osc_freq_para       ;
wire    [ 3:0]      ch1_offset_para     ;
wire    [ 3:0]      ch2_offset_para     ;
wire    [ 3:0]      ch1_gain_para       ;
wire    [ 3:0]      ch2_gain_para       ;


//--------------------------------------------------------------------
//-------------------------- logic function --------------------------
//--------------------------------------------------------------------

// rgb888 signal gain module:
tft_lcd_drive               tft_lcd_drive_init(             
    .clk_40m                (sys_clk_50m        )   ,
    .rst_n                  (sys_rst_n          )   ,
    .column                 (column             )   ,
    .row                    (row                )   ,       
    .vga_hsync              (vga_hsync          )   ,
    .vga_vsync              (vga_vsync          )   ,
    .vga_en                 (vga_en             )     
);

// 计算参数：
para_calc                   para_calc_init(
    .sys_clk_50m            (sys_clk_50m        )   ,
    .sys_rst_n              (sys_rst_n          )   ,
    .ch1_wave_data          (ch1_adc_data       )   ,
    .ch2_wave_data          (ch2_adc_data       )   ,
    .ch1_calc_para          (ch1_calc_para      )   ,
    .ch2_calc_para          (ch2_calc_para      )   
);

// 跨时钟域处理数据：ch1
adc1_data_to_display        ch1_adc_data_to_display_init(
    .sys_rst_n			    (sys_rst_n		    )   ,
    .osc_freq_para          (osc_freq_para      )   ,
    .column                 (column             )   ,
    .dpram_wr_clk           (adc1_clk           )   ,   // 写时钟：100MHz
    .dpram_in_data          (ch1_adc_data       )   ,	// 写数据
    .dpram_rd_clk_32m       (sys_clk_50m        )   ,   // 都时钟：32MHz
    .dpram_out_data       	(ch1_wave_data      )   	// 读数据
);

// 跨时钟域处理数据：ch2
adc2_data_to_display        ch2_adc_data_to_display_init(
    .sys_rst_n			    (sys_rst_n		    )   ,
    .osc_freq_para          (osc_freq_para      )   ,
    .column                 (column             )   ,
    .dpram_wr_clk           (adc2_clk           )   ,   // 写时钟：100MHz
    .dpram_in_data          (ch2_adc_data       )   ,	// 写数据
    .dpram_rd_clk_32m       (sys_clk_50m        )   ,   // 都时钟：32MHz
    .dpram_out_data       	(ch2_wave_data      )   	// 读数据
);

// 按键操作
osc_btn_operation           osc_btn_operation_init(
    .sys_clk_50m	        (sys_clk_50m	    )   ,
    .sys_rst_n	            (sys_rst_n	        )   ,

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
    .swg_amp_para           (swg_amp_para       )   ,

    .osc_freq_para          (osc_freq_para      )   ,
    .ch1_offset_para        (ch1_offset_para    )   ,
    .ch2_offset_para        (ch2_offset_para    )   ,
    .ch1_gain_para          (ch1_gain_para      )   ,
    .ch2_gain_para          (ch2_gain_para      )   
);

// 数据预处理：（增益 && 偏置）
wave_preprocessing          wave_preprocessing_init(
    .sys_clk_50m	        (sys_clk_50m	    )   ,
    .sys_rst_n	            (sys_rst_n	        )   ,
    .ch1_initial_data       (ch1_wave_data      )   ,
    .ch2_initial_data       (ch2_wave_data      )   ,
    .ch1_gain_select        (ch1_gain_para      )   ,
    .ch2_gain_select        (ch2_gain_para      )   ,
    .ch1_offset_select      (ch1_offset_para    )   ,
    .ch2_offset_select      (ch2_offset_para    )   ,
    .ch1_display_data       (ch1_display_data   )   ,
    .ch2_display_data       (ch2_display_data   )    
);

// 例化图像信息模块：
osc_ui_display              osc_ui_display_init(           
    .clk_40m                (sys_clk_50m        )   ,
    .rst_n                  (sys_rst_n          )   ,
    .ch1_trig_level         (ch1_trig_level     )   ,
    .ch2_trig_level         (ch2_trig_level     )   ,
    .ch1_wave_data          (ch1_display_data   )   ,
    .ch2_wave_data          (ch2_display_data   )   ,
    .ch1_freq_result        (197092             )   ,
    .ch2_freq_result        (739                )   ,
    .ch1_calc_para          (ch1_calc_para      )   ,
    .ch2_calc_para          (ch2_calc_para      )   ,
    .column                 (column             )   ,
    .row                    (row                )   ,
    .vga_en                 (vga_en             )   ,
    .vga_rgb888             (vga_rgb888         )
);



endmodule