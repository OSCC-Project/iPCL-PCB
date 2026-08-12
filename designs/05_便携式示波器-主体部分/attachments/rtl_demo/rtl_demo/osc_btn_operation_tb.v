//==========================================================================
//  Author      ：alpha
//  Mail        ：1879136899@qq.com
//  Filename    ：
//  Created Time：
//  End Time    ：
//  Description ：
//==========================================================================
`timescale 1ns/100ps


module osc_btn_operation_tb();

reg		[ 0:0]		sys_clk_50m		    ;
reg		[ 0:0]		sys_rst_n		    ;

reg     [ 0:0]      btn_up              ;   // 通用操作：向上
reg     [ 0:0]      btn_done            ;   // 通用操作：向下
reg     [ 0:0]      btn_left            ;   // 通用操作：向左
reg     [ 0:0]      btn_right           ;   // 通用操作：向右
reg     [ 0:0]      btn_sure            ;   // 通用操作：确定
reg     [ 0:0]      osc_freq_btn        ;   // 示波器：时基选择
reg     [ 0:0]      osc_time_btn        ;   // 示波器：时间选择
reg     [ 0:0]      ch1_offset_btn      ;   // 通道一：偏置
reg     [ 0:0]      ch1_gain_btn        ;   // 通道一：增益
reg     [ 0:0]      ch2_offset_btn      ;   // 通道二：偏置
reg     [ 0:0]      ch2_gain_btn        ;   // 通道二：增益
reg     [ 0:0]      swg_wave_btn        ;   // 信号源：波形选择
reg     [ 0:0]      swg_freq_btn        ;   // 信号源：频率选择
reg     [ 0:0]      swg_amp_btn         ;   // 信号源：电压

wire    [ 3:0]      swg_wave_para       ; 
wire    [ 3:0]      swg_freq_para       ; 
wire    [ 3:0]      swg_amp_para        ; 

wire    [ 3:0]      ch1_offset_para     ; 
wire    [ 3:0]      ch2_offset_para     ; 
wire    [ 3:0]      ch1_gain_para       ; 
wire    [ 3:0]      ch2_gain_para       ;
    
//==============================待测试模块例化==============================
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

    .ch1_offset_para        (ch1_offset_para    )   ,
    .ch2_offset_para        (ch2_offset_para    )   ,
    .ch1_gain_para          (ch1_gain_para      )   ,
    .ch2_gain_para          (ch2_gain_para      )   
);


//================================iverilog=================================
initial begin
    $dumpfile("wave.vcd");
    $dumpvars(0,osc_btn_operation_tb);
end

//=================================时钟生成=================================
initial sys_clk_50m = 1'b0;
always begin
    #10 sys_clk_50m = ~sys_clk_50m;
end

//=================================激励产生=================================
initial begin
    sys_rst_n = 0;
    btn_up = 1;       
    btn_done = 1;      
    btn_left = 1;      
    btn_right = 1;     
    btn_sure = 1;      
    osc_freq_btn = 1;  
    osc_time_btn = 1;  
    ch1_offset_btn = 1;
    ch1_gain_btn = 1;  
    ch2_offset_btn = 1;
    ch2_gain_btn = 1;  
    swg_wave_btn = 1;  
    swg_freq_btn = 1;  
    swg_amp_btn = 1;   
    #101;
    sys_rst_n = 1;
    #101;
    #501;
    // ch1_offset_btn = 0;
    // #80_001;
    // ch1_offset_btn = 1;
    // #50_001;
    // btn_up = 0;
    // #80_001;
    // btn_up = 1;
    // #50_001;
    // btn_up = 0;
    // #90_001;
    // btn_up = 1;
    // #200_001;
    // btn_up = 0;
    // #60_001;
    // btn_up = 1;
    // #5_000_001;
    ch2_gain_btn = 0;
    #50_001;
    ch2_gain_btn = 1;
    #50_001;
    ch2_gain_btn = 0;
    #50_001;
    ch2_gain_btn = 1;
    #50_001;
    ch2_gain_btn = 0;
    #50_001;
    ch2_gain_btn = 1;
    #50_001;
    $finish;
end

endmodule