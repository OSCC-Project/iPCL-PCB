//==========================================================================
//  Author      ：alpha
//  Mail        ：1879136899@qq.com
//  Filename    ：
//  Created Time：
//  End Time    ：
//  Description ：
//==========================================================================
`timescale 1ns/100ps


module uart_rx(
    //system signal
    input	wire	[ 0:0]		sys_clk_50m			,
    input	wire	[ 0:0]		sys_rst_n			,
    //user signal
    input	wire	[ 2:0]		bps_select          ,
    input	wire	[ 0:0]		in_uart_rx          ,
    output	reg 	[ 7:0]		receive_data        ,		
    output	wire	[ 0:0]		receive_finish			
);


//--------------------------------------------------------------------
//------------------------------参数定义-------------------------------
//--------------------------------------------------------------------
parameter       RECEIVE_BPS_CLK_CNT_MAX     =       8'd159     ;

//--------------------------------------------------------------------
//------------------------------参数定义-------------------------------
//--------------------------------------------------------------------
reg     [ 2:0]      r_data_byte     [7:0]       ;
reg     [ 0:0]      r0_in_uart_rx               ;
reg     [ 0:0]      r1_in_uart_rx               ;
reg     [ 0:0]      r2_in_uart_rx               ;
wire    [ 0:0]      negedge_in_uart_rx          ;
reg     [ 2:0]      start_bit                   ;
reg     [ 2:0]      stop_bit                    ;
wire    [ 7:0]      receive_bps_clk_cnt         ;
reg     [ 0:0]      receive_bps_gain_en         ;


//--------------------------------------------------------------------
//------------------------------逻辑功能-------------------------------
//--------------------------------------------------------------------

// 波特率产生模块：
uart_rx_bps_gain        uart_rx_bps_gain_init(
    .sys_clk_50m			(           sys_clk_50m),
    .sys_rst_n			    (             sys_rst_n),

    .bps_select             (            bps_select),
    .receive_bps_gain_en    (   receive_bps_gain_en),
    .receive_finish         (        receive_finish),
    .receive_bps_clk_cnt    (   receive_bps_clk_cnt)
);


// 对输入信号进行寄存（消除亚稳态）：
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        // 空闲状态置1：
        r0_in_uart_rx <= 1'b1;
        r1_in_uart_rx <= 1'b1;
        r2_in_uart_rx <= 1'b1;
    end
    else begin
        r0_in_uart_rx <= in_uart_rx     ;
        r1_in_uart_rx <= r0_in_uart_rx  ;
        r2_in_uart_rx <= r1_in_uart_rx  ;
    end
end

// 取 in_uart_rx 信号的下降沿：
assign negedge_in_uart_rx = !r1_in_uart_rx & r2_in_uart_rx  ;


// 波特率产生使能信号：
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        receive_bps_gain_en <= 1'b0;
    end
    else if(negedge_in_uart_rx == 1'b1)begin
        receive_bps_gain_en <= 1'b1;
    end
    else if(receive_finish == 1'b1)begin
        receive_bps_gain_en <= 1'b0;
    end
    else begin
        receive_bps_gain_en <= receive_bps_gain_en;
    end
end


// 接收数据：
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        start_bit      <= 3'd0  ;
        r_data_byte[0] <= 3'd0  ;
        r_data_byte[1] <= 3'd0  ;
        r_data_byte[2] <= 3'd0  ;
        r_data_byte[3] <= 3'd0  ;
        r_data_byte[4] <= 3'd0  ;
        r_data_byte[5] <= 3'd0  ;
        r_data_byte[6] <= 3'd0  ;
        r_data_byte[7] <= 3'd0  ;
        stop_bit       <= 3'd0  ;
    end
    else begin
        case (receive_bps_clk_cnt)
            0 : begin
                start_bit      <= 3'd0  ;
                r_data_byte[0] <= 3'd0  ;
                r_data_byte[1] <= 3'd0  ;
                r_data_byte[2] <= 3'd0  ;
                r_data_byte[3] <= 3'd0  ;
                r_data_byte[4] <= 3'd0  ;
                r_data_byte[5] <= 3'd0  ;
                r_data_byte[6] <= 3'd0  ;
                r_data_byte[7] <= 3'd0  ;
                stop_bit       <= 3'd0  ;
            end
              6,  7,  8,  9, 10, 11 : start_bit <= start_bit + r2_in_uart_rx;
             22, 23, 24, 25, 26, 27 : r_data_byte[0] <= r_data_byte[0] + r2_in_uart_rx;
             38, 39, 40, 41, 42, 43 : r_data_byte[1] <= r_data_byte[1] + r2_in_uart_rx;
             54, 55, 56, 57, 58, 59 : r_data_byte[2] <= r_data_byte[2] + r2_in_uart_rx;
             70, 71, 72, 73, 74, 75 : r_data_byte[3] <= r_data_byte[3] + r2_in_uart_rx;
             86, 87, 88, 89, 90, 91 : r_data_byte[4] <= r_data_byte[4] + r2_in_uart_rx;
            102,103,104,105,106,107 : r_data_byte[5] <= r_data_byte[5] + r2_in_uart_rx;
            118,119,120,121,122,123 : r_data_byte[6] <= r_data_byte[6] + r2_in_uart_rx;
            134,135,136,137,138,139 : r_data_byte[7] <= r_data_byte[7] + r2_in_uart_rx;
            150,151,152,153,154,155 : stop_bit <= stop_bit + r2_in_uart_rx;
            default;
        endcase
    end
end


// 实际数据：
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        receive_data <= 1'b0;
    end
    else if(receive_finish == 1'b1)begin
        receive_data[0] <= r_data_byte[0][2];
        receive_data[1] <= r_data_byte[1][2];
        receive_data[2] <= r_data_byte[2][2];
        receive_data[3] <= r_data_byte[3][2];
        receive_data[4] <= r_data_byte[4][2];
        receive_data[5] <= r_data_byte[5][2];
        receive_data[6] <= r_data_byte[6][2];
        receive_data[7] <= r_data_byte[7][2];
    end
end


endmodule