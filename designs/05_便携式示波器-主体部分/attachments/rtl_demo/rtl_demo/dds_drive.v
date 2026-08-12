//==========================================================================
//  Author       ：alpha
//  Mail         ：1879136899@qq.com
//  Filename     ：
//  Create time  ：
//  Description  ：
//==========================================================================
`timescale 1ns/100ps


module dds_drive(
    //system signal
    input	wire				pll_clk_165m		,
    input	wire				sys_rst_n			,
    //user signal
    input	wire	[31:0]		freq_ctrl           ,
    input	wire    [ 3:0]      swg_freq_para       , 
    input	wire    [ 3:0]      swg_wave_para       , 
    input	wire	[ 9:0]		phase_ctrl          ,	
    input	wire	[ 9:0]		amp_ctrl            ,			

    output  reg     [ 9:0]      da_data     
);


//--------------------------------------------------------------------
//------------------------- parameter define -------------------------
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//------------------------- variable define --------------------------
//--------------------------------------------------------------------
wire    [ 9:0]     data_addr        ;
reg     [31:0]     data_add         ;

wire    [ 9:0]     wave_data        ;
wire    [ 9:0]     tri_data         ;
wire    [ 9:0]     saw_data         ;

//--------------------------------------------------------------------
//-------------------------- logic function --------------------------
//--------------------------------------------------------------------

// rom地址信号产生（频率控制字累加）：
always @ (posedge pll_clk_165m or negedge sys_rst_n)begin
	if(sys_rst_n == 1'b0)begin
		data_add <= 32'd0;
	end
	else begin
    case (swg_freq_para)
        4'd0  : data_add <= data_add + (freq_ctrl << 14);
        4'd1  : data_add <= data_add + (freq_ctrl << 15);
        4'd2  : data_add <= data_add + (freq_ctrl << 16);
        4'd3  : data_add <= data_add + (freq_ctrl << 17);
        4'd4  : data_add <= data_add + (freq_ctrl << 18);
        4'd5  : data_add <= data_add + (freq_ctrl << 19);
        4'd6  : data_add <= data_add + (freq_ctrl << 20);
        4'd7  : data_add <= data_add + (freq_ctrl << 21);
        4'd8  : data_add <= data_add + (freq_ctrl << 22);
        4'd9  : data_add <= data_add + (freq_ctrl << 23);
        4'd10 : data_add <= data_add + (freq_ctrl << 24);
        default : data_add <= data_add + (freq_ctrl << 19);
    endcase
	end
end

assign data_addr = data_add[31:22] + phase_ctrl;

rom_wave_data       rom_wave_data_init(
   .clock   	    (pll_clk_165m	)	,
   .address   	    (data_addr		)	,
   .q   		    (wave_data		)
);

triangular_data     triangular_data_init(
   .clock   	    (pll_clk_165m	)	,
   .address   	    (data_addr		)	,
   .q   		    (tri_data		)
);

sawtooth_data       sawtooth_data_init(
   .clock   	    (pll_clk_165m	)	,
   .address   	    (data_addr		)	,
   .q   		    (saw_data		)
);

always @ (posedge pll_clk_165m)begin
    case (swg_wave_para) 
        4'd0  : begin
            if(wave_data[9] == 0)
                da_data <= (wave_data >> 2) + 384;
            else 
                da_data <= {wave_data[9], wave_data[8:0] >> 2};
        end 
        4'd1  : da_data = (wave_data > 10'd511) ? 10'd1023 : 1'b0;
        4'd2  : da_data = tri_data;
        4'd3  : da_data = saw_data;
        default : da_data = wave_data;
    endcase
end

endmodule