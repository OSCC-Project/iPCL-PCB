//==========================================================================
//  Author      ：alpha
//  Mail        ：1879136899@qq.com
//  Filename    ：
//  Created Time：
//  End Time    ：
//  Description ：
//==========================================================================
`timescale 1ns/100ps

module uart_tx_bps_gain(
    //system signal
    input  wire     [ 0:0]      sys_clk_50m                 ,       // 系统时钟信号
    input  wire     [ 0:0]      sys_rst_n                   ,       // 系统复位信号
    //user signal
    input  wire     [ 2:0]      bps_select                  ,       // 串口波特率选择
    input  wire     [ 0:0]      send_bps_gain_en            ,       // 串口波特率时钟使能
    output reg      [ 3:0]      send_bps_clk_cnt            ,       // 串口波特率时钟计数（发送）
    output wire     [ 0:0]      negedge_send_bps_clk                // 波特率时钟的下降沿（发送）
    );

    //--------------------------------------------------------------------
    //------------------------------参数定义------------------------------
    //--------------------------------------------------------------------
    parameter       SEND_BPS_CLK_CNT_MAX        =       4'd11      ;
    parameter       RECEIVE_BPS_CLK_CNT_MAX     =       4'd10      ;

    //--------------------------------------------------------------------
    //------------------------------逻辑功能------------------------------
    //--------------------------------------------------------------------

    // 波特率选择：
    reg     [   14:0]       BPS             ;

    always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
        if(sys_rst_n == 1'b0)begin
            BPS <= 13'd433;
        end
        else
            case(bps_select)   
                // 3'd0 : BPS <= 13'd5207  ;        //   9600bps(25MHz)
                // 3'd1 : BPS <= 13'd2604  ;        //  19200bps(25MHz)
                // 3'd2 : BPS <= 13'd1301  ;        //  38400bps(25MHz)
                // 3'd3 : BPS <= 13'd867   ;        //  57600bps(25MHz)
                // 3'd4 : BPS <= 13'd218   ;        // 115200bps(25MHz)
                // default:BPS <= 13'd217  ; 
                3'd0 : BPS <= 13'd5207  ;        //   9600bps(50MHz)
                3'd1 : BPS <= 13'd2604  ;        //  19200bps(50MHz)
                3'd2 : BPS <= 13'd1301  ;        //  38400bps(50MHz)
                3'd3 : BPS <= 13'd867   ;        //  57600bps(50MHz)
                3'd4 : BPS <= 13'd433   ;        // 115200bps(50MHz)
                default:BPS <= 13'd433  ;                
            endcase
    end

    //--------------------------------------------------------------------
    //------------------------------发送波特率----------------------------
    //--------------------------------------------------------------------

    // 波特率产生计数器：
    wire                    add_send_bps_gain_cnt        ;
    wire                    end_send_bps_gain_cnt        ;
    reg     [ 13 - 1: 0]    send_bps_gain_cnt            ;

    always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
        if(sys_rst_n == 1'b0)begin
            send_bps_gain_cnt <= 0;
        end
        else if(add_send_bps_gain_cnt)begin
            if(end_send_bps_gain_cnt)
                send_bps_gain_cnt <= 0;
            else
                send_bps_gain_cnt <= send_bps_gain_cnt + 1'b1;
        end
    end

    assign add_send_bps_gain_cnt = send_bps_gain_en == 1'b1;       
    assign end_send_bps_gain_cnt = add_send_bps_gain_cnt && send_bps_gain_cnt == BPS - 1'b1;   

    // 产生波特率：
    reg     [ 0:0]      send_bps_clk;

    always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
        if(sys_rst_n == 1'b0)begin
            send_bps_clk <= 1'b0;
        end
        else if(send_bps_gain_cnt > (BPS - 1'd1) >> 1)
            send_bps_clk <= 1'b1;
        else
            send_bps_clk <= 1'b0;
    end
    
    // 对波特率进行计数：
    wire                    add_send_bps_clk_cnt        ;
    wire                    end_send_bps_clk_cnt        ;

    always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
        if(sys_rst_n == 1'b0)begin
            send_bps_clk_cnt <= 0;
        end
        else if(add_send_bps_clk_cnt)begin
            if(end_send_bps_clk_cnt)
                send_bps_clk_cnt <= 0;
            else
                send_bps_clk_cnt <= send_bps_clk_cnt + 1'b1;
        end
    end

    assign add_send_bps_clk_cnt = send_bps_gain_cnt == (BPS - 1'd1) >> 1;       
    assign end_send_bps_clk_cnt = add_send_bps_clk_cnt && send_bps_clk_cnt == SEND_BPS_CLK_CNT_MAX;   


    // 对波特率时钟寄存一个时钟周期并取下降沿：
    reg     [ 0:0]      r0_send_bps_clk      ;                      
    reg     [ 0:0]      r1_send_bps_clk      ;   
    reg     [ 0:0]      r2_send_bps_clk      ;     

    always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
        if(sys_rst_n == 1'b0)begin
            r0_send_bps_clk <= 1'b0;
            r1_send_bps_clk <= 1'b0;
            r2_send_bps_clk <= 1'b0;
        end
        else begin
            r0_send_bps_clk <= send_bps_clk;
            r1_send_bps_clk <= r0_send_bps_clk;
            r2_send_bps_clk <= r1_send_bps_clk;
        end
    end

    assign  negedge_send_bps_clk = r2_send_bps_clk && ~ r1_send_bps_clk  ;
    


endmodule


