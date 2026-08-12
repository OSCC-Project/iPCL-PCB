//==========================================================================
//  Author       ：alpha
//  Mail         ：1879136899@qq.com
//  Filename     ：
//  Create time  ：
//  Description  ：
//==========================================================================
`timescale 1ns/100ps


module ad9288_drive(
    //system signal
    input	wire				sys_clk_50m			,
    input	wire				sys_rst_n			,
    //user signal
    input	wire	[ 7:0]		adc_data_a          ,			
    input	wire	[ 7:0]		adc_data_b          ,
    output  reg     [ 7:0]      adc_data                    	
);


//--------------------------------------------------------------------
//------------------------- parameter define -------------------------
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//------------------------- variable define --------------------------
//--------------------------------------------------------------------
reg     [ 0:0]      adc_data_cnt        ;

//--------------------------------------------------------------------
//-------------------------- logic function --------------------------
//--------------------------------------------------------------------
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)
        adc_data_cnt <= 2'd0;
    else if(adc_data_cnt == 2'd2)
        adc_data_cnt <= 2'd0;
    else
        adc_data_cnt <= adc_data_cnt + 2'd1;
end

// always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
//     if(sys_rst_n == 1'b0)begin
//         adc_data <= 8'd0;
//     end
//     else if(adc_data_cnt == 2'd0)begin
//         adc_data <= adc_data_a - 128;
//     end
//     else if(adc_data_cnt == 2'd1)begin
//         adc_data <= adc_data_b - 128;
//     end
// end

always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)
        adc_data <= 8'd0;
    else
        adc_data <= adc_data_b - 128;
end

endmodule