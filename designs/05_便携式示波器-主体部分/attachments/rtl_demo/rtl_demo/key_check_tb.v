//==========================================================================
//  Author      ：alpha
//  Mail        ：1879136899@qq.com
//  Filename    ：
//  Created Time：
//  End Time    ：
//  Description ：
//==========================================================================
`timescale 1ns/100ps


module key_check_tb();

reg		[ 0:0]		sys_clk_50m		;
reg		[ 0:0]		sys_rst_n		;
reg		[ 0:0]		key_in   		;
wire	[ 0:0]		key_flag   		;

//==============================待测试模块例化==============================
key_check				key_check_init(
    .sys_clk_50m	    (    sys_clk_50m)		,
    .sys_rst_n			(      sys_rst_n)       ,
    .key_in             (         key_in)       ,
    .key_flag		    (       key_flag)			
);


//================================iverilog=================================
initial begin
    $dumpfile("wave.vcd");
    $dumpvars(0,key_check_tb);
end


//=================================时钟生成=================================
initial sys_clk_50m = 1'b0;
always begin
    #10 sys_clk_50m = ~sys_clk_50m;
end


//=================================激励产生=================================
initial begin
    sys_rst_n = 0;
    key_in = 1;
    #101;
    sys_rst_n = 1;
    #101;
    #1001;
    key_in = 0;
    #50001;
    key_in = 1;
    #1001;
    $finish;
end


endmodule