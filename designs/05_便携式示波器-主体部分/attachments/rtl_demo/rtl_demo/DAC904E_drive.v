//==========================================================================
//  Author      ：alpha
//  Mail        ：1879136899@qq.com
//  Filename    ：
//  Created Time：
//  End Time    ：
//  Description ：
//==========================================================================
`timescale 1ns/100ps


module DAC904E_drive(
    //system signal
    input	wire	[ 0:0]		pll_clk_165m		,
    input	wire	[ 0:0]		sys_rst_n			,
    //user signal(dac1)
    input	wire	[31:0]		dac2_freq_ctrl      ,
    input	wire	[11:0]		dac2_phase_ctrl     ,		
    input	wire	[11:0]		dac2_amp_ctrl       ,
    output  wire    [ 9:0]      dac2_data                     
);

wire     [11:0]  data_addr;
reg      [31:0]  data_add;

// rom地址信号产生（频率控制字累加）：
always @ (posedge pll_clk_165m or negedge sys_rst_n)begin
	if(sys_rst_n == 1'b0)begin
		data_add <= 32'd0;
	end
	else begin
		data_add <= data_add + dac2_freq_ctrl;
	end
end

assign   data_addr = data_add[31:20] + dac2_phase_ctrl;

rom_sin 		rom_sin_init(
   .clock   	(pll_clk_165m	)	,
   .address   	(data_addr		)	,
   .q   		(dac2_data		)
);

endmodule
