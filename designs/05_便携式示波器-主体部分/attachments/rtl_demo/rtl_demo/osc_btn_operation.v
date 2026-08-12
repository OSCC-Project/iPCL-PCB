//==========================================================================
//  Author       ：alpha
//  Mail         ：1879136899@qq.com
//  Filename     ：
//  Create time  ：
//  Description  ：
//                 mode_operation == 1 : CH1 offset add
//                 mode_operation == 2 : CH1 offset dev
//                 mode_operation == 3 : CH2 offset add
//                 mode_operation == 4 : CH2 offset dev
//                 mode_operation == 5 : CH1 gain add
//                 mode_operation == 6 : CH1 gain dev
//                 mode_operation == 7 : CH2 gain add
//                 mode_operation == 8 : CH2 gain dev
//                 mode_operation == 9 : swg wave_toggle
//                 mode_operation == 10 : swg freq add
//                 mode_operation == 11 : swg freq dev
//                 mode_operation == 12 : swg amp add
//                 mode_operation == 13 : swg amp dev
//==========================================================================
`timescale 1ns/100ps

module osc_btn_operation(
    //system signal
    input	wire				sys_clk_50m			,
    input	wire				sys_rst_n			,
    //user signal
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

    output  reg     [ 3:0]      swg_wave_para       , 
    output  reg     [ 3:0]      swg_freq_para       , 
    output  reg     [ 3:0]      swg_amp_para        , 

    output  reg     [ 3:0]      osc_freq_para       , 
    output  reg     [ 3:0]      ch1_offset_para     , 
    output  reg     [ 3:0]      ch2_offset_para     ,   
    output  reg     [ 3:0]      ch1_gain_para       ,   
    output  reg     [ 3:0]      ch2_gain_para               
);


//--------------------------------------------------------------------
//------------------------- parameter define -------------------------
//--------------------------------------------------------------------
// parameter		TIME_10MS_MAX       =   24'd500         ;
parameter		TIME_5MS_MAX        =   24'd250_000     ;
parameter		TIME_10MS_MAX       =   24'd1_000_000   ;
parameter		TIME_200MS_MAX      =   5'd10           ;
parameter		TIME_3S_MAX         =   9'd150          ;

parameter		IDLE                =   3'd0            ;
parameter		WAIT_FUNC_BTN_DONE  =   3'd1            ;
parameter		WAIT_FUNC_20MS      =   3'd2            ;
parameter		CHECK_FUNC_BTN      =   3'd3            ;
parameter		WAIT_3S             =   3'd4            ;
parameter		WAIT_DIR_200MS      =   3'd5            ;

//--------------------------------------------------------------------
//------------------------- variable define --------------------------
//--------------------------------------------------------------------
reg     [ 0:0]      func_btn                ;
reg     [ 0:0]      direction_btn           ;

reg     [23:0]      time_5ms_cnt            ;
reg     [23:0]      time_10ms_cnt           ;
reg     [ 4:0]      time_200ms_cnt          ;
reg     [ 8:0]      time_3s_cnt             ;

reg     [ 2:0]      btn_state_c             ;
reg     [ 2:0]      btn_state_n             ;

wire    [ 0:0]      idle_to_wait_func       ;
wire    [ 0:0]      wait_func_to_20ms       ;
wire    [ 0:0]      wait_20ms_to_check_func ;
wire    [ 0:0]      check_func_to_3s        ;
wire    [ 0:0]      check_func_to_idle      ;
wire    [ 0:0]      wait_3s_to_200ms        ;
wire    [ 0:0]      wait_3s_to_idle         ;
wire    [ 0:0]      wait_200ms_to_3s        ;

reg     [ 3:0]      para_func               ;

reg     [ 0:0]      r0_freq_level           ;
reg     [ 0:0]      r1_freq_level           ;
reg     [ 0:0]      r2_freq_level           ;
reg     [ 0:0]      r0_time_level           ;
reg     [ 0:0]      r1_time_level           ;
reg     [ 0:0]      r2_time_level           ;
reg     [ 0:0]      r0_ch1_gain_level       ;
reg     [ 0:0]      r1_ch1_gain_level       ;
reg     [ 0:0]      r2_ch1_gain_level       ;
reg     [ 0:0]      r0_ch2_gain_level       ;
reg     [ 0:0]      r1_ch2_gain_level       ;
reg     [ 0:0]      r2_ch2_gain_level       ;

wire    [ 0:0]      osc_freq_edge           ;
wire    [ 0:0]      osc_time_edge           ;
wire    [ 0:0]      ch1_gain_edge           ;
wire    [ 0:0]      ch2_gain_edge           ;

reg     [ 0:0]      time_5ms_cnt_flag       ;

//--------------------------------------------------------------------
//-------------------------- logic function --------------------------
//--------------------------------------------------------------------
// 功能按键选择：
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        func_btn <= 1'b1;
    end
    else if((ch1_offset_btn == 1'b0) || (ch2_offset_btn == 1'b0) || (swg_freq_btn == 1'b0) || (swg_amp_btn == 1'b0))begin
        func_btn <= 1'b0;
    end
    else begin
        func_btn <= 1'b1;
    end
end

// 方向按键选择：
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        direction_btn <= 1'b1;
    end
    else if((btn_up == 1'b0) || (btn_done == 1'b0) || (btn_left == 1'b0)  || (btn_right == 1'b0))begin
        direction_btn <= 1'b0;
    end
    else begin
        direction_btn <= 1'b1;
    end
end

// 计时10ms:
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        time_10ms_cnt <= 24'd0;
    end
    else if(time_10ms_cnt == TIME_10MS_MAX - 1'b1)begin
        time_10ms_cnt <= 24'd0;
    end
    else if((btn_state_c == WAIT_FUNC_20MS) || (btn_state_c == WAIT_DIR_200MS) || (btn_state_c == WAIT_3S))begin
        time_10ms_cnt <= time_10ms_cnt + 24'd1;
    end
    else begin
        time_10ms_cnt <= 24'd0;
    end
end

// 计时200ms:
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)
        time_200ms_cnt <= 5'd0;
    else if(time_200ms_cnt == TIME_200MS_MAX - 1'b1)
        time_200ms_cnt <= 5'd0;
    else if(btn_state_c == WAIT_DIR_200MS)begin
        if(time_10ms_cnt == TIME_10MS_MAX - 1'b1)
            time_200ms_cnt <= time_200ms_cnt + 5'd1;
        else
            time_200ms_cnt <= time_200ms_cnt;
    end
    else 
        time_200ms_cnt <= 5'd0;
end


// 计时3s:
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)
        time_3s_cnt <= 9'd0;
    else if(time_3s_cnt == TIME_3S_MAX - 1'b1)
        time_3s_cnt <= 9'd0;
    else if(btn_state_c == WAIT_3S)begin
        if(time_10ms_cnt == TIME_10MS_MAX - 1'b1)
            time_3s_cnt <= time_3s_cnt + 9'd1;
        else
            time_3s_cnt <= time_3s_cnt;
    end
    else 
        time_3s_cnt <= 9'd0;
end

//第一段：同步时序always模块，格式化描述次态寄存器迁移到现态寄存器(不需更改）
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        btn_state_c <= IDLE;
    end
    else begin
        btn_state_c <= btn_state_n;
    end
end

//第二段：组合逻辑always模块，描述状态转移条件判断
always @ (*)begin
    case(btn_state_c)
        IDLE:begin
            if(idle_to_wait_func)begin
                btn_state_n = WAIT_FUNC_BTN_DONE;
            end
            else begin
                btn_state_n = btn_state_c;
            end
        end      
        WAIT_FUNC_BTN_DONE:begin
            if(wait_func_to_20ms)begin
                btn_state_n = WAIT_FUNC_20MS;
            end
            else begin
                btn_state_n = btn_state_c;
            end
        end
        WAIT_FUNC_20MS:begin
            if(wait_20ms_to_check_func)begin
                btn_state_n = CHECK_FUNC_BTN;
            end
            else begin
                btn_state_n = btn_state_c;
            end
        end
        CHECK_FUNC_BTN:begin
            if(check_func_to_3s)begin
                btn_state_n = WAIT_3S;
            end
            else if(check_func_to_idle)begin
                btn_state_n = IDLE;
            end
            else begin
                btn_state_n = btn_state_c;
            end
        end
        WAIT_3S:begin
            if(wait_3s_to_200ms)begin
                btn_state_n = WAIT_DIR_200MS;
            end
            else if(wait_3s_to_idle)begin
                btn_state_n = IDLE;
            end
            else begin
                btn_state_n = btn_state_c;
            end
        end
        WAIT_DIR_200MS:begin
            if(wait_200ms_to_3s)begin
                btn_state_n = WAIT_3S;
            end
            else begin
                btn_state_n = btn_state_c;
            end
        end
        default:begin
            btn_state_n = IDLE;
        end
    endcase
end 

//第三段：设计转移条件      
assign idle_to_wait_func        = btn_state_c == IDLE               && 1'b1;
assign wait_func_to_20ms        = btn_state_c == WAIT_FUNC_BTN_DONE && ((func_btn == 1'b0) || (swg_wave_btn == 1'b0));
assign wait_20ms_to_check_func  = btn_state_c == WAIT_FUNC_20MS     && time_10ms_cnt == TIME_10MS_MAX - 1'b1;
assign check_func_to_3s         = btn_state_c == CHECK_FUNC_BTN     && func_btn == 1'b0;
assign check_func_to_idle       = btn_state_c == CHECK_FUNC_BTN     && func_btn == 1'b1;
assign wait_3s_to_200ms         = btn_state_c == WAIT_3S            && direction_btn == 1'b0;
assign wait_3s_to_idle          = btn_state_c == WAIT_3S            && time_3s_cnt == TIME_3S_MAX - 1'b1;
assign wait_200ms_to_3s         = btn_state_c == WAIT_DIR_200MS     && time_200ms_cnt == TIME_200MS_MAX - 1'b1;
      
//第四段：同步时序always模块，格式化描述寄存器输出（可有多个输出）
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        para_func <=4'b0;      //初始化
    end
    else if(btn_state_c == CHECK_FUNC_BTN)begin
        if(ch1_offset_btn == 1'b0)
            para_func <= 4'd1;
        else if(ch2_offset_btn == 1'b0)
            para_func <= 4'd2;
        else if(swg_freq_btn == 1'b0)
            para_func <= 4'd3;
        else if(swg_amp_btn == 1'b0)
            para_func <= 4'd4;
        else
            para_func <= para_func;
    end
    else if(btn_state_c == IDLE)
            para_func <= 4'd0;
    else 
        para_func <= para_func;
end

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)
        ch1_offset_para <= 4'd5;      //初始化
    else if(ch1_offset_para == 4'd11)
        ch1_offset_para <= 4'd0;   
    else if((btn_state_c == WAIT_DIR_200MS) && (time_200ms_cnt == 5'd2) && (time_10ms_cnt == TIME_10MS_MAX - 1'b1) && (para_func == 4'd1) && (btn_up == 1'b0))
        ch1_offset_para <= ch1_offset_para + 4'd1;
    else if((btn_state_c == WAIT_DIR_200MS) && (time_200ms_cnt == 5'd2) && (time_10ms_cnt == TIME_10MS_MAX - 1'b1) && (para_func == 4'd1) && (btn_done == 1'b0))
        ch1_offset_para <= ch1_offset_para - 4'd1;
    else 
        ch1_offset_para <= ch1_offset_para;
end

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)
        ch2_offset_para <= 4'd5;      //初始化
    else if(ch2_offset_para == 4'd11)
        ch2_offset_para <= 4'd0;  
    else if((btn_state_c == WAIT_DIR_200MS) && (time_200ms_cnt == 5'd2) && (time_10ms_cnt == TIME_10MS_MAX - 1'b1) && (para_func == 4'd2) && (btn_up == 1'b0))
        ch2_offset_para <= ch2_offset_para + 4'd1;
    else if((btn_state_c == WAIT_DIR_200MS) && (time_200ms_cnt == 5'd2) && (time_10ms_cnt == TIME_10MS_MAX - 1'b1) && (para_func == 4'd2) && (btn_done == 1'b0))
        ch2_offset_para <= ch2_offset_para - 4'd1;
    else 
        ch2_offset_para <= ch2_offset_para;
end

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)
        swg_freq_para <= 4'd5;      //初始化
    else if(swg_freq_para == 4'd11)
        swg_freq_para <= 4'd5;   
    else if((btn_state_c == WAIT_DIR_200MS) && (time_200ms_cnt == 5'd2) && (time_10ms_cnt == TIME_10MS_MAX - 1'b1) && (para_func == 4'd3) && (btn_up == 1'b0))
        swg_freq_para <= swg_freq_para + 4'd1;
    else if((btn_state_c == WAIT_DIR_200MS) && (time_200ms_cnt == 5'd2) && (time_10ms_cnt == TIME_10MS_MAX - 1'b1) && (para_func == 4'd3) && (btn_done == 1'b0))
        swg_freq_para <= swg_freq_para - 4'd1;
    else 
        swg_freq_para <= swg_freq_para;
end

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)
        swg_amp_para <= 4'd5;      //初始化
    else if(swg_amp_para == 4'd11)
        swg_amp_para <= 4'd5;  
    else if((btn_state_c == WAIT_DIR_200MS) && (time_200ms_cnt == 5'd2) && (time_10ms_cnt == TIME_10MS_MAX - 1'b1) && (para_func == 4'd4) && (btn_up == 1'b0))
        swg_amp_para <= swg_amp_para + 4'd1;
    else if((btn_state_c == WAIT_DIR_200MS) && (time_200ms_cnt == 5'd2) && (time_10ms_cnt == TIME_10MS_MAX - 1'b1) && (para_func == 4'd4) && (btn_done == 1'b0))
        swg_amp_para <= swg_amp_para - 4'd1;
    else 
        swg_amp_para <= swg_amp_para;
end

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)
        swg_wave_para <= 4'd0;      //初始化
    else if(swg_wave_para == 4'd4)
        swg_wave_para <= 4'd0;  
    else if((btn_state_c == CHECK_FUNC_BTN) && (swg_wave_btn == 1'b0))
        swg_wave_para <= swg_wave_para + 4'd1;
    else 
        swg_wave_para <= swg_wave_para;
end

//==================================================================//
//============================= 编码器 ==============================//
//==================================================================//

// 计时5ms:
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        time_5ms_cnt_flag <= 1'b0;
    end
    else if((ch1_gain_edge == 1'b1) && (ch2_gain_edge == 1'b1) && (osc_freq_edge == 1'b1) && (osc_time_edge == 1'b1))
        time_5ms_cnt_flag <= 1'b1;
    else if(time_5ms_cnt == TIME_5MS_MAX - 1'b1)
        time_5ms_cnt_flag <= 1'b0;
    else 
        time_5ms_cnt_flag <= time_5ms_cnt_flag;
end

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        time_5ms_cnt <= 24'd0;
    end
    else if(time_5ms_cnt == TIME_5MS_MAX - 1'b1)
        time_5ms_cnt <= 24'd0;
    else if(time_5ms_cnt_flag == 1'b1)
        time_5ms_cnt <= time_5ms_cnt + 24'd1;
    else
        time_5ms_cnt <= 24'd0;
end

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        r0_freq_level <= 1'b0;
        r1_freq_level <= 1'b0;
        r2_freq_level <= 1'b0;

        r0_time_level <= 1'b0;
        r1_time_level <= 1'b0;
        r2_time_level <= 1'b0;

        r0_ch1_gain_level <= 1'b0;
        r1_ch1_gain_level <= 1'b0;
        r2_ch1_gain_level <= 1'b0;

        r0_ch2_gain_level <= 1'b0;
        r1_ch2_gain_level <= 1'b0;
        r2_ch2_gain_level <= 1'b0;
    end
    else begin
        r0_freq_level <= osc_freq_btn;
        r1_freq_level <= r0_freq_level;
        r2_freq_level <= r1_freq_level;

        r0_time_level <= osc_time_btn;
        r1_time_level <= r0_time_level;
        r2_time_level <= r1_time_level;

        r0_ch1_gain_level <= ch1_gain_btn;
        r1_ch1_gain_level <= r0_ch1_gain_level;
        r2_ch1_gain_level <= r1_ch1_gain_level;

        r0_ch2_gain_level <= ch2_gain_btn;
        r1_ch2_gain_level <= r0_ch2_gain_level;
        r2_ch2_gain_level <= r1_ch2_gain_level;
    end
end

assign osc_freq_edge = ~time_5ms_cnt_flag && r1_freq_level && ~r2_freq_level;
assign osc_time_edge = ~time_5ms_cnt_flag && r1_time_level && ~r2_time_level;
assign ch1_gain_edge = ~time_5ms_cnt_flag && r1_ch1_gain_level && ~r2_ch1_gain_level;
assign ch2_gain_edge = ~time_5ms_cnt_flag && r1_ch2_gain_level && ~r2_ch2_gain_level;

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        osc_freq_para <= 4'd5;
    end
    else if(osc_freq_para == 4'd11)begin
        osc_freq_para <= 4'd5;
    end
    else if(osc_freq_edge == 1'b1)begin
        osc_freq_para <= osc_freq_para + 4'b1;
    end
    else if(osc_time_edge == 1'b1)begin
        osc_freq_para <= osc_freq_para - 4'b1;
    end
    else
        osc_freq_para <= osc_freq_para;
end

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        ch1_gain_para <= 4'd4;
    end
    else if(ch1_gain_para == 4'd11)begin
        ch1_gain_para <= 4'd0;
    end
    else if(ch1_gain_edge == 1'b1)begin
        ch1_gain_para <= ch1_gain_para + 4'b1;
    end
    else
        ch1_gain_para <= ch1_gain_para;
end

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        ch2_gain_para <= 4'd4;
    end
    else if(ch2_gain_para == 4'd11)begin
        ch2_gain_para <= 4'd0;
    end
    else if(ch2_gain_edge == 1'b1)begin
        ch2_gain_para <= ch2_gain_para + 4'b1;
    end
    else
        ch2_gain_para <= ch2_gain_para;
end

endmodule