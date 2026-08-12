//==========================================================================
//  Author      ：alpha
//  Mail        ：1879136899@qq.com
//  Filename    ：
//  Created Time：
//  End Time    ：
//  Description ：
//==========================================================================
`timescale 1ns/100ps

module uart(
    //system signal
    input  wire     [    0:0]      sys_clk_50m          ,       // 系统时钟：50MHz
    input  wire     [    0:0]      sys_rst_n            ,       // 系统复位：低电平有效
    //user signl       
    input  wire     [    2:0]      bps_select           ,       // 波特率选择
    input  wire     [   26:0]      data_a               ,       // 待发送数据
    input  wire     [   26:0]      data_b               ,       // 待发送数据
    input  wire     [   26:0]      data_c               ,       // 待发送数据
    input  wire     [   26:0]      data_d               ,       // 待发送数据
    input  wire     [   26:0]      data_e               ,       // 待发送数据
    input  wire     [   26:0]      data_f               ,       // 待发送数据
    input  wire     [   26:0]      data_g               ,       // 待发送数据
    output wire     [    7:0]      receive_data         ,       // 串口接收数据
    output wire     [    0:0]      receive_finish       ,       // 串口接收数据完成标志
    input  wire     [    0:0]      in_uart_rx           ,       // 串口接收数据信号 
    output wire     [    0:0]      out_uart_tx                  // 串口发送数据信号
    );


    //--------------------------------------------------------------------
    //------------------------------参数定义-------------------------------
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    //------------------------------变量定义-------------------------------
    //--------------------------------------------------------------------
    wire            [ 7:0]              send_data           ;
    wire            [ 0:0]              uart_send_en        ;
    wire            [ 0:0]              uart_send_finish    ;  


    //--------------------------------------------------------------------
    //--------------------------数据转换（发送）---------------------------
    //--------------------------------------------------------------------      

    tx_data_transform               tx_data_transform_init(
        .sys_clk_50m                (                sys_clk_50m)   ,       // 系统时钟
        .sys_rst_n                  (                  sys_rst_n)   ,       // 系统复位
        .data_a                     (                     data_a)   ,       // 待发送数据a
        .data_b                     (                     data_b)   ,       // 待发送数据b
        .data_c                     (                     data_c)   ,       // 待发送数据c
        .data_d                     (                     data_d)   ,       // 待发送数据d
        .data_e                     (                     data_e)   ,       // 待发送数据e
        .data_f                     (                     data_f)   ,       // 待发送数据f
        .data_g                     (                     data_g)   ,       // 待发送数据g
        .send_data                  (                  send_data)   ,       // 转换完成数据（单位数据）
        .uart_send_en               (               uart_send_en)   ,       // 串口发送使能
        .uart_send_finish           (           uart_send_finish)           // 串口发送一帧数据完成标志
    );  


    //--------------------------------------------------------------------
    //------------------------------串口发送-------------------------------
    //-------------------------------------------------------------------- 

    uart_tx                         uart_tx_init(
        .sys_clk_50m                (                sys_clk_50m)   ,       // 系统时钟：50MHz
        .sys_rst_n                  (                  sys_rst_n)   ,       // 系统复位：低电平有效

        .bps_select                 (                 bps_select)   ,       // 波特率选择
        .wait_send_data             (                  send_data)   ,       // 待发送数据
        .uart_send_en               (               uart_send_en)   ,       // 串口发送使能 
        .out_uart_tx                (                out_uart_tx)   ,       // 串口发送数据信号
        .uart_send_finish           (           uart_send_finish)           // 串口发送完成标志
    );


    //--------------------------------------------------------------------
    //------------------------------串口接收-------------------------------
    //--------------------------------------------------------------------

    uart_rx                         uart_rx_init(
        .sys_clk_50m                (                sys_clk_50m)   ,       // 系统时钟：50MHz
        .sys_rst_n                  (                  sys_rst_n)   ,       // 系统复位：低电平有效

        .bps_select                 (                 bps_select)   ,       // 波特率选择
        .in_uart_rx                 (                 in_uart_rx)   ,       // 串口接收数据信号 
        .receive_data               (               receive_data)   ,       // 串口接收数据信号
        .receive_finish             (             receive_finish)           // 串口接收数据完成标志
    );                                                                                                                  



endmodule