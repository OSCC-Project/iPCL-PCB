//==========================================================================
//  Author      ：alpha
//  Mail        ：1879136899@qq.com
//  Filename    ：
//  Created Time：
//  End Time    ：
//  Description ：
//==========================================================================
`timescale 1ns/100ps

module dac0_ad5304_drive_tb();

reg		[ 0:0]		sys_clk_50m		    ;
reg		[ 0:0]		sys_rst_n		    ;
reg		[ 0:0]		start		        ;
reg     [ 1:0]      channal_num         ;
reg     [ 7:0]      dac0_data           ;

wire    [ 0:0]      done                ;
wire    [ 0:0]      sclk                ;
wire    [ 0:0]      cs_n                ;
wire    [ 0:0]      d_out               ;
    
//==============================待测试模块例化==============================
dac0_ad5304_drive				dac0_ad5304_drive_init(
    .sys_clk_50m                (sys_clk_50m        )   ,
    .sys_rst_n			        (sys_rst_n          )   ,
    .start                      (start              )   ,
    .channal_num                (channal_num        )   ,
    .dac0_data                  (dac0_data          )   ,
    .done                       (done               )   ,         
    .sclk                       (sclk               )   ,
    .cs_n                       (cs_n               )   ,		
    .d_out                      (d_out              )
);


//================================iverilog=================================
initial begin
    $dumpfile("wave.vcd");
    $dumpvars(0,dac0_ad5304_drive_tb);
end


//=================================时钟生成=================================
initial sys_clk_50m = 1'b0;
always begin
    #5 sys_clk_50m = ~sys_clk_50m;
end


//=================================激励产生=================================
initial begin
    sys_rst_n = 1'b0;
    channal_num = 2'b00;
    dac0_data = 8'b0000_0011;
    start = 1;
    #101;
    sys_rst_n = 1;
    #101;
    start = 0;
    #20000;
    $finish;
end

endmodule