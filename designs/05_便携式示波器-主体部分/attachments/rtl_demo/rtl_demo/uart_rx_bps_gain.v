//==========================================================================
//  Author      ：alpha
//  Mail        ：1879136899@qq.com
//  Filename    ：
//  Created Time：
//  End Time    ：
//  Description ：
//==========================================================================
`timescale 1ns/100ps


module uart_rx_bps_gain(
    //system signal
    input	wire	[ 0:0]		sys_clk_50m			,
    input	wire	[ 0:0]		sys_rst_n			,
    //user signal
    input	wire	[ 2:0]		bps_select          ,
    input	wire	[ 0:0]		receive_bps_gain_en ,
    output  reg     [ 0:0]		receive_finish      ,
    output  reg     [ 7:0]      receive_bps_clk_cnt 
);


//--------------------------------------------------------------------
//------------------------------参数定义-------------------------------
//--------------------------------------------------------------------
parameter       RECEIVE_BPS_CLK_CNT_MAX     =       8'd159      ;


//--------------------------------------------------------------------
//------------------------------逻辑功能-------------------------------
//--------------------------------------------------------------------

// 波特率选择：
reg     [   14:0]           BPS                     ;

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        BPS <= 13'd433;
    end
    else
        case(bps_select)   
            3'd0 : BPS <= 13'd324   ;        //   9600bps(50MHz)
            3'd1 : BPS <= 13'd162   ;        //  19200bps(50MHz)
            3'd2 : BPS <= 13'd80    ;        //  38400bps(50MHz)
            3'd3 : BPS <= 13'd53    ;        //  57600bps(50MHz)
            3'd4 : BPS <= 13'd26    ;        // 115200bps(50MHz)
            default:BPS <= 13'd26   ;                
        endcase
end


// 波特率产生计数器：
wire                    add_receive_bps_gain_cnt        ;
wire                    end_receive_bps_gain_cnt        ;
reg     [ 13 - 1: 0]    receive_bps_gain_cnt            ;

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        receive_bps_gain_cnt <= 0;
    end
    else if(add_receive_bps_gain_cnt)begin
        if(end_receive_bps_gain_cnt)
            receive_bps_gain_cnt <= 0;
        else
            receive_bps_gain_cnt <= receive_bps_gain_cnt + 1'b1;
    end
end

assign add_receive_bps_gain_cnt = receive_bps_gain_en == 1'b1;       
assign end_receive_bps_gain_cnt = add_receive_bps_gain_cnt && receive_bps_gain_cnt == BPS - 1'b1;   


// 产生波特率：
reg     [ 0:0]      reveive_bps_clk;

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        reveive_bps_clk <= 1'b0;
    end
    else if(receive_bps_gain_cnt > (BPS - 1'd1) >> 1)
        reveive_bps_clk <= 1'b1;
    else
        reveive_bps_clk <= 1'b0;
end
    

// 对波特率进行计数：
wire                    add_receive_bps_clk_cnt     ;
wire                    end_receive_bps_clk_cnt     ;

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        receive_bps_clk_cnt <= 0;
    end
    else if(end_receive_bps_clk_cnt)begin
        receive_bps_clk_cnt <= 0;
    end
    else if(add_receive_bps_clk_cnt)begin
        receive_bps_clk_cnt <= receive_bps_clk_cnt + 1'b1;
    end
end

assign add_receive_bps_clk_cnt = receive_bps_gain_cnt == (BPS - 1'd1) >> 1;       
assign end_receive_bps_clk_cnt = receive_bps_clk_cnt == RECEIVE_BPS_CLK_CNT_MAX;   


// 产生接收数据完成标志位：
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        receive_finish <= 1'b0;
    end
    else if(receive_bps_clk_cnt == RECEIVE_BPS_CLK_CNT_MAX)
        receive_finish <= 1'b1;
    else
        receive_finish <= 1'b0;
end


endmodule