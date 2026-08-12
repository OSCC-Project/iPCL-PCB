//==========================================================================
//  Author       ：alpha
//  Mail         ：1879136899@qq.com
//  Filename     ：
//  Create time  ：
//  Description  ：
//==========================================================================
`timescale 1ns/100ps

module simulation_wave_gain(
    //system signal
    input	wire				sys_clk_50m			,
    input	wire				sys_rst_n			,
    //user signal		
    output	wire	[ 7:0]		test_sine_data		,		
    output	wire	[ 7:0]		test_square_data
);

//--------------------------------------------------------------------
//------------------------- parameter define -------------------------
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//------------------------- variable define --------------------------
//--------------------------------------------------------------------
reg     [11:0]		rom_ch1_addr    ;
reg     [11:0]		rom_ch2_addr    ;

wire    [ 7:0]		rom_ch1_data    ;
wire    [ 7:0]		rom_ch2_data    ;

//--------------------------------------------------------------------
//-------------------------- logic function --------------------------
//--------------------------------------------------------------------
// assign test_square_data = ({test_sine_data , 1'b0} >> 4) + 32;
// assign test_square_data = test_sine_data[7] ? 132 : 100;

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        rom_ch1_addr <= 1'b0;
    end
    else if(rom_ch1_addr == 4095)begin
        rom_ch1_addr <= 1'b0;
    end
    else begin
        rom_ch1_addr <= rom_ch1_addr + 10'd35;
    end
end

// 例化ROM波形数据模块：ch1
rom_ch1_test_wave   ch1_rom_test_wave_inst(
	.address 		(rom_ch1_addr       )   ,
	.clock 			(sys_clk_50m        )   ,
	.q 				(rom_ch1_data       )
);

assign test_sine_data = rom_ch1_data / 3 + 100;



always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        rom_ch2_addr <= 1'b0;
    end
    else if(rom_ch2_addr == 4095)begin
        rom_ch2_addr <= 1'b0;
    end
    else begin
        rom_ch2_addr <= rom_ch2_addr + 10'd45;
    end
end

// 例化ROM波形数据模块：ch2
rom_ch2_test_wave	ch2_rom_test_wave_inst(
	.address 		(rom_ch2_addr       )   ,
	.clock 			(sys_clk_50m        )   ,
	.q 				(rom_ch2_data       )
);

assign test_square_data = rom_ch2_data / 2 + 5;


endmodule