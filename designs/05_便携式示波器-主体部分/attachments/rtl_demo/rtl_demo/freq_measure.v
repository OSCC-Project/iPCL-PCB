//==========================================================================
//  Author      ：alpha
//  Mail        ：1879136899@qq.com
//  Filename    ：
//  Created Time：
//  End Time    ：
//  Description ：
//==========================================================================
`timescale 1ns/100ps

module freq_measure(
    //system signal
    input  wire     [ 0:0]      pll_clk_200m    ,       // 频率计时钟
    input  wire     [ 0:0]      sys_rst_n       ,       // 系统复位
    //user signal
    input  wire     [ 0:0]      measure_signal  ,       // 输入待测信号
    output reg      [26:0]      freq_result             // 信号频率输出
    );

    //--------------------------------------------------------------------
    //------------------------------参数定义------------------------------
    //--------------------------------------------------------------------
    
    parameter       TIME_1S     =       28'd199_999_999      ;

    //--------------------------------------------------------------------
    //------------------------------逻辑功能------------------------------
    //--------------------------------------------------------------------

    // 计时1s：
    wire                    add_time_1s_cnt        ;
    wire                    end_time_1s_cnt        ;
    reg     [ 28 - 1: 0]    time_1s_cnt            ;

    always @ (posedge pll_clk_200m or negedge sys_rst_n)begin
        if(sys_rst_n == 1'b0)begin
            time_1s_cnt <= 0;
        end
        else if(add_time_1s_cnt)begin
            if(end_time_1s_cnt)
                time_1s_cnt <= 0;
            else
                time_1s_cnt <= time_1s_cnt + 1'b1;
        end
    end

    assign add_time_1s_cnt = 1'b1;       
    assign end_time_1s_cnt = add_time_1s_cnt && time_1s_cnt == TIME_1S;      


    // 时序逻辑，预置闸门信号：
    reg     [ 0:0]      preset_gate     ;

    always @ (posedge pll_clk_200m or negedge sys_rst_n)begin
        if(sys_rst_n == 1'b0)begin
            preset_gate <= 1'b0;
        end
        else if(time_1s_cnt == TIME_1S) 
            preset_gate <= ~preset_gate;
        else 
            preset_gate <= preset_gate;
    end

    // 实际闸门信号：
    reg         real_gate       ;                     

    always @ (posedge measure_signal)       
        if(preset_gate == 1'b1)
            real_gate <= preset_gate;        
        else 
            real_gate <= 1'b0;        
    

    // 计数器，对待测信号进行计数：
    reg     [ 27 - 1: 0]    measure_signal_cnt            ;

    always @ (negedge measure_signal)begin
        if(real_gate == 1'b1)
            measure_signal_cnt <= measure_signal_cnt + 1;
        else 
            measure_signal_cnt <= 1'b0;
    end

    // 锁存输出：
    always @ (negedge real_gate)begin
        freq_result <= measure_signal_cnt;
    end

endmodule
