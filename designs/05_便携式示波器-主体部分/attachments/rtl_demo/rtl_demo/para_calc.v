//==========================================================================
//  Author       ：alpha
//  Mail         ：1879136899@qq.com
//  Filename     ：
//  Create time  ：
//  Description  ：
//==========================================================================
`timescale 1ns/100ps


module para_calc(
    //system signal
    input	wire				sys_clk_50m			,
    input	wire				sys_rst_n			,
    //user signal
    input	wire	[ 7:0]		ch1_wave_data       ,
    input	wire	[ 7:0]		ch2_wave_data       ,	
    output  reg     [47:0]      ch1_calc_para       ,
    output  reg     [47:0]      ch2_calc_para       	
);


//--------------------------------------------------------------------
//------------------------- parameter define -------------------------
//--------------------------------------------------------------------
parameter   TIME_CNT_MAX    =   15'd10000        ;

//--------------------------------------------------------------------
//------------------------- variable define --------------------------
//--------------------------------------------------------------------
reg     [14:0]      time_100ms_cnt      ;

reg     [15:0]      ch1_volt_data       ;
reg     [15:0]      ch2_volt_data       ;

reg     [15:0]      r_ch1_volt_vmin     ;
reg     [15:0]      r_ch1_volt_vmax     ;
reg     [15:0]      r_ch2_volt_vmin     ;
reg     [15:0]      r_ch2_volt_vmax     ;

//--------------------------------------------------------------------
//-------------------------- logic function --------------------------
//--------------------------------------------------------------------

// 计数器：
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        time_100ms_cnt <= 1'b0;
    end
    else if(time_100ms_cnt == TIME_CNT_MAX - 1'b1)begin
        time_100ms_cnt <= 1'b0;
    end
    else begin
        time_100ms_cnt <= time_100ms_cnt + 1'b1;
    end
end

// ADC数据转为电压数据：
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        ch1_volt_data <= 16'd2500;
        ch2_volt_data <= 16'd2500;
    end
    else begin
        ch1_volt_data <= ch1_wave_data * 625 >> 5;
        ch2_volt_data <= ch2_wave_data * 625 >> 5;
    end
end

// 找CH1的最大值与最小值：
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        r_ch1_volt_vmin <= 16'd2500;
        r_ch1_volt_vmax <= 16'd2500;
    end
    else if(ch1_volt_data < r_ch1_volt_vmin)
        r_ch1_volt_vmin <= ch1_volt_data;
    else if(ch1_volt_data > r_ch1_volt_vmax)
        r_ch1_volt_vmax <= ch1_volt_data;
    else begin
        r_ch1_volt_vmin <= r_ch1_volt_vmin;
        r_ch1_volt_vmax <= r_ch1_volt_vmax;
    end
end

// 找CH2的最大值与最小值：
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        r_ch2_volt_vmin <= 16'd2500;
        r_ch2_volt_vmax <= 16'd2500;
    end
    else if(ch2_volt_data < r_ch2_volt_vmin)
        r_ch2_volt_vmin <= ch2_volt_data;
    else if(ch2_volt_data > r_ch2_volt_vmax)
        r_ch2_volt_vmax <= ch2_volt_data;
    else begin
        r_ch2_volt_vmin <= r_ch2_volt_vmin;
        r_ch2_volt_vmax <= r_ch2_volt_vmax;
    end
end

// 每过一段时间将CH1、CH2的最大值与最小值赋给ch_calc_para
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        ch1_calc_para <= 48'd0;
        ch2_calc_para <= 48'd0;
    end
    else if(time_100ms_cnt == TIME_CNT_MAX - 1'b1)begin
        ch1_calc_para[ 15:0] <= r_ch1_volt_vmin;
        ch1_calc_para[31:16] <= r_ch1_volt_vmax;
        ch1_calc_para[47:32] <= (r_ch1_volt_vmax - r_ch1_volt_vmin);

        ch2_calc_para[ 15:0] <= r_ch2_volt_vmin;
        ch2_calc_para[31:16] <= r_ch2_volt_vmax;
        ch2_calc_para[47:32] <= (r_ch2_volt_vmax - r_ch2_volt_vmin);
    end
    else begin
        ch1_calc_para[ 15:0] <= ch1_calc_para[ 15:0];
        ch1_calc_para[31:16] <= ch1_calc_para[31:16];
        ch1_calc_para[47:32] <= ch1_calc_para[47:32];

        ch2_calc_para[ 15:0] <= ch2_calc_para[ 15:0];
        ch2_calc_para[31:16] <= ch2_calc_para[31:16];
        ch2_calc_para[47:32] <= ch2_calc_para[47:32];
    end
end

    

endmodule