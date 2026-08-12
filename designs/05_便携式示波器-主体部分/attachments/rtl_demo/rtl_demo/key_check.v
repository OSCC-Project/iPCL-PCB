//==========================================================================
//  Author      ：alpha
//  Mail        ：1879136899@qq.com
//  Filename    ：
//  Created Time：
//  End Time    ：
//  Description ：
//==========================================================================
`timescale 1ns/100ps


module key_check(
    //system signal
    input	wire	[ 0:0]		sys_clk_50m			,
    input	wire	[ 0:0]		sys_rst_n			,
    //user signal
    input	wire	[ 2:0]		key_in              ,
    output  reg     [ 2:0]      key_flag				
);


//--------------------------------------------------------------------
//------------------------------参数定义-------------------------------
//--------------------------------------------------------------------
parameter   WAIT_CNT_MAX		=   19'd500_000		;
// parameter   WAIT_CNT_MAX		=   19'd2_000		;

parameter   IDLE                =       4'd1   	;
parameter   WAIT_CHECK          =       4'd2   	;
parameter   DELAY_TIME          =       4'd3   	;
parameter   CHECK               =       4'd4   	;


//--------------------------------------------------------------------
//------------------------------参数定义-------------------------------
//--------------------------------------------------------------------
reg     [    2:0]      r0_key_in                ;
reg     [    2:0]      r1_key_in                ;
reg     [    2:0]      r2_key_in                ;
wire    [    0:0]      negedge_key0             ;
wire    [    0:0]      negedge_key1             ;
wire    [    0:0]      negedge_key2             ;

wire    [    0:0]      add_wait_cnt             ;
wire    [    0:0]      end_wait_cnt             ;
reg     [   18:0]      wait_cnt                 ;

reg     [    3:0]      key_check_state_c        ;
reg     [    3:0]      key_check_state_n        ;

wire    [    0:0]      idle_wait_check_start    ;
wire    [    0:0]      wait_check_time_start    ;
wire    [    0:0]      time_check_start         ;
wire    [    0:0]      check_idle_start         ;


//--------------------------------------------------------------------
//------------------------------逻辑功能-------------------------------
//--------------------------------------------------------------------
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        r0_key_in <= 3'b0;
        r1_key_in <= 3'b0;
        r2_key_in <= 3'b0;
    end
    else begin
        r0_key_in <= key_in;
        r1_key_in <= r0_key_in;
        r2_key_in <= r1_key_in;
    end
end

assign negedge_key0 = ~r0_key_in[0] && r1_key_in[0];
assign negedge_key1 = ~r0_key_in[1] && r1_key_in[1];
assign negedge_key2 = ~r0_key_in[2] && r1_key_in[2];

//计数器：
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        wait_cnt <= 0;
    end
    else if(end_wait_cnt)begin
        wait_cnt <= 0;
    end
    else if(add_wait_cnt)begin
        wait_cnt <= wait_cnt + 1;
    end
    else begin
        wait_cnt <= wait_cnt;
    end
end

assign add_wait_cnt = key_check_state_c == DELAY_TIME;       
assign end_wait_cnt = wait_cnt == WAIT_CNT_MAX - 1'b1;   


//第一段：同步时序always模块，格式化描述次态寄存器迁移到现态寄存器(不需更改）
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        key_check_state_c <= IDLE;
    end
    else begin
        key_check_state_c <= key_check_state_n;
    end
end

//第二段：组合逻辑always模块，描述状态转移条件判断
always @ (*)begin
    case(key_check_state_c)
        IDLE:begin
            if(idle_wait_check_start)begin
                key_check_state_n = WAIT_CHECK;
            end
            else begin
                key_check_state_n = key_check_state_c;
            end
        end      
        WAIT_CHECK:begin
            if(wait_check_time_start)begin
                key_check_state_n = DELAY_TIME;
            end
            else begin
                key_check_state_n = key_check_state_c;
            end
        end
        DELAY_TIME:begin
            if(time_check_start)begin
                key_check_state_n = CHECK;
            end
            else begin
                key_check_state_n = key_check_state_c;
            end
        end
        CHECK:begin
            if(check_idle_start)begin
                key_check_state_n = IDLE;
            end
            else begin
                key_check_state_n = key_check_state_c;
            end
        end
        default:begin
            key_check_state_n = IDLE;
        end
    endcase
end

//第三段：设计转移条件
assign idle_wait_check_start = key_check_state_c == IDLE       && 1'b1;
assign wait_check_time_start = key_check_state_c == WAIT_CHECK && (negedge_key0 == 1'b1 | 
                                                                   negedge_key1 == 1'b1 | 
                                                                   negedge_key2 == 1'b1 );
assign time_check_start      = key_check_state_c == DELAY_TIME && end_wait_cnt == 1'b1;
assign check_idle_start      = key_check_state_c == CHECK      && 1'b1;


//第四段：同步时序always模块，格式化描述寄存器输出（可有多个输出）
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        key_flag <=3'b0;      //初始化
    end
    else if(key_check_state_c == CHECK && r2_key_in[0] == 1'b0)begin
        key_flag[0] <= ~key_flag[0];
    end
    else if(key_check_state_c == CHECK && r2_key_in[1] == 1'b0)begin
        key_flag[1] <= ~key_flag[1];
    end
    else if(key_check_state_c == CHECK && r2_key_in[2] == 1'b0)begin
        key_flag[2] <= ~key_flag[2];
    end
    else begin
        key_flag <= key_flag;
    end
end

//==================================================================//
//=========================== 状态机结束 ============================//
//==================================================================//


endmodule