//==========================================================================
//  Author      ：alpha
//  Mail        ：1879136899@qq.com
//  Filename    ：
//  Created Time：
//  End Time    ：
//  Description ：
//==========================================================================
`timescale 1ns/100ps

module uart_tx(
    //system signal
    input  wire     [ 0:0]      sys_clk_50m         ,       // 系统时钟：50MHz
    input  wire     [ 0:0]      sys_rst_n           ,       // 系统复位：低电平有效
    //user signal
    input  wire     [ 2:0]      bps_select          ,       // 波特率选择
    input  wire     [ 7:0]      wait_send_data      ,       // 待发送数据
    input  wire     [ 0:0]      uart_send_en        ,       // 串口发送使能 
    output reg      [ 0:0]      out_uart_tx         ,       // 串口发送数据信号
    output reg      [ 0:0]      uart_send_finish            // 串口发送完成标志
    );

    //--------------------------------------------------------------------
    //------------------------------参数定义------------------------------
    //--------------------------------------------------------------------

    parameter       UART_START_BIT      =       1'b0        ;       // 开始位   
    parameter       UART_CHECK_BIT      =       1'b0        ;       // 校验位
    parameter       UART_STOP_BIT       =       1'b1        ;       // 停止位

    //--------------------------------------------------------------------
    //------------------------------串口发送------------------------------
    //--------------------------------------------------------------------

    // 例化串口波特率产生模块：
    wire        [ 3:0]      send_bps_clk_cnt                ;           // 波特率时钟计数（发送）
    wire        [ 0:0]      negedge_bps_clk                 ;           // 波特率下降沿
    wire        [ 0:0]      end_receive_bps_gain_cnt        ;           // 波特率下降沿

    uart_tx_bps_gain                uart_tx_bps_gain_init(
        .sys_clk_50m                (                sys_clk_50m)    ,       // 系统时钟       
        .sys_rst_n                  (                  sys_rst_n)    ,       // 系统复位
        
        .bps_select                 (                 bps_select)    ,       // 波特率选择
        .send_bps_gain_en           (               uart_send_en)    ,       // 波特率发送使能
        .send_bps_clk_cnt           (           send_bps_clk_cnt)    ,       // 波特率时钟计数（发送）
        .negedge_send_bps_clk       (       negedge_send_bps_clk)            // 波特率下降沿（发送）
    );


    // 串口数据发送：
    always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
        if(sys_rst_n == 1'b0)begin
            out_uart_tx <= 1'b1;
        end
        else begin
            case(send_bps_clk_cnt)
                0  : out_uart_tx <= 1'b1;

                // 有效数据，发送 :  wait_send_data[0 ~ 7] （最低位先发送）
                1  : out_uart_tx <= UART_START_BIT;          // 起始位
                2  : out_uart_tx <= wait_send_data[0];       // 数据位 0
                3  : out_uart_tx <= wait_send_data[1];       // 数据位 1
                4  : out_uart_tx <= wait_send_data[2];       // 数据位 2
                5  : out_uart_tx <= wait_send_data[3];       // 数据位 3
                6  : out_uart_tx <= wait_send_data[4];       // 数据位 4
                7  : out_uart_tx <= wait_send_data[5];       // 数据位 5
                8  : out_uart_tx <= wait_send_data[6];       // 数据位 6
                9  : out_uart_tx <= wait_send_data[7];       // 数据位 7
                10 : out_uart_tx <= UART_STOP_BIT;           // 停止位                               
				
                11 : out_uart_tx <= 1'b1;
			    default : out_uart_tx <= 1'b1;
            endcase
        end
    end


    // 串口发送一帧数据完成标志信号:
    always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
        if(sys_rst_n == 1'b0)begin
            uart_send_finish <= 1'b0;
        end
        else if(send_bps_clk_cnt == 4'd11 & negedge_send_bps_clk == 1'b1)
            uart_send_finish <= 1'b1;
        else
            uart_send_finish <= 1'b0;
    end



endmodule
