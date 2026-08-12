//==========================================================================
//  Author      ：alpha
//  Mail        ：1879136899@qq.com
//  Filename    ：
//  Created Time：
//  End Time    ：
//  Description ：
//==========================================================================
`timescale 1ns/100ps

module led(
    //system signal
    input	wire	[ 0:0]		sys_clk_50m			,
    input	wire	[ 0:0]		sys_rst_n			,
    //user signal
    input	wire	[ 0:0]		led_flag            ,
    input	wire	[ 2:0]		key_flag            ,
    output  reg	    [ 0:0]		c401_led            ,
    output  reg     [ 2:0]      osc_led 			
);


//--------------------------------------------------------------------
//------------------------------参数定义-------------------------------
//--------------------------------------------------------------------
parameter WAIT_CNT_MAX = 26'd12_500_000;

//--------------------------------------------------------------------
//------------------------------参数定义-------------------------------
//--------------------------------------------------------------------
reg     [    0:0]      flag                     ;

wire    [    0:0]      add_wait_cnt             ;
wire    [    0:0]      end_wait_cnt             ;
reg     [   25:0]      wait_cnt                 ;

//--------------------------------------------------------------------
//------------------------------逻辑功能-------------------------------
//--------------------------------------------------------------------
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        c401_led <= 1'b0;
    end
    else begin
        c401_led = led_flag;
    end
end

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        osc_led <= 3'b000;
    end
    else begin
        case (key_flag)
            3'b000 : osc_led <= 3'b111  ;
            3'b001 : osc_led <= 3'b110  ;
            3'b010 : osc_led <= 3'b101  ;
            3'b100 : osc_led <= 3'b011  ;
            default: osc_led <= 3'b111  ;
        endcase
    end
end



endmodule