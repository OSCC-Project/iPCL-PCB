//==========================================================================
//  Author       ：alpha
//  Mail         ：1879136899@qq.com
//  Filename     ：
//  Create time  ：
//  Description  ：
//==========================================================================
`timescale 1ns/100ps


module adc_sample_rate_set(
    //system signal
    input	wire				pll_clk_200m		,
    input	wire				sys_rst_n			,
    //user signal
    input	wire	[26:0]		ch1_freq_result     ,	
    output	reg	    [ 0:0]		adc1_clka           ,			
    output	wire	[ 0:0]		adc1_clkb           ,	
    input	wire	[26:0]		ch2_freq_result     ,	
    output	reg		[ 0:0]		adc2_clka           ,			
    output	wire	[ 0:0]		adc2_clkb           	
);


//--------------------------------------------------------------------
//------------------------- parameter define -------------------------
//--------------------------------------------------------------------
parameter		CLK_100M_MAX    =		27'd1000	    ;
parameter		CLK_50M_MAX     =		27'd1000	    ;
parameter		CLK_10M_MAX     =		27'd1000	    ;
parameter		CLK_5M_MAX      =		27'd1000	    ;
parameter		CLK_1M_MAX      =		27'd1000	    ;

//--------------------------------------------------------------------
//------------------------- variable define --------------------------
//--------------------------------------------------------------------
reg     [26:0]      clk_100m_cnt    ;
reg     [26:0]      clk_50m_cnt     ;
reg     [26:0]      clk_10m_cnt     ;
reg     [26:0]      clk_5m_cnt      ;
reg     [26:0]      clk_1m_cnt      ;

reg     [ 0:0]      clk_100m        ;
reg     [ 0:0]      clk_50m         ;
reg     [ 0:0]      clk_10m         ;
reg     [ 0:0]      clk_5m          ;
reg     [ 0:0]      clk_1m          ;

//--------------------------------------------------------------------
//-------------------------- logic function --------------------------
//--------------------------------------------------------------------
assign adc1_clkb = ~adc1_clka   ;
assign adc2_clkb = ~adc2_clka   ;

always @ (posedge pll_clk_200m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)
        adc1_clka <= 1'b0;
    else if(ch1_freq_result > 10_000_000)
        adc1_clka <= clk_100m;
    else if(ch1_freq_result > 1_000_000)
        adc1_clka <= clk_50m;
    else if(ch1_freq_result > 500_000)
        adc1_clka <= clk_10m;
    else if(ch1_freq_result > 100_000)
        adc1_clka <= clk_5m;
    else if(ch1_freq_result > 50_000)
        adc1_clka <= clk_1m;
    else
        adc1_clka <= clk_1m;
end

always @ (posedge pll_clk_200m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        clk_100m_cnt <= 1'b0;
        clk_50m_cnt  <= 1'b0; 
        clk_10m_cnt  <= 1'b0;
        clk_5m_cnt   <= 1'b0; 
        clk_1m_cnt   <= 1'b0;
    end
    else if(ch1_freq_result > 10_000_000)begin
        if(clk_100m_cnt == CLK_100M_MAX - 1)
            clk_100m_cnt <= 1'b0;
        else 
            clk_100m_cnt <= clk_100m_cnt + 1'b1;
    end
    else if(ch1_freq_result > 1_000_000)begin
        if(clk_50m_cnt == CLK_50M_MAX - 1)
            clk_50m_cnt <= 1'b0;
        else 
            clk_50m_cnt <= clk_50m_cnt + 1'b1;
    end
    else if(ch1_freq_result > 500_000)begin
        if(clk_10m_cnt == CLK_10M_MAX - 1)
            clk_10m_cnt <= 1'b0;
        else 
            clk_10m_cnt <= clk_10m_cnt + 1'b1;
    end
    else if(ch1_freq_result > 100_000)begin
        if(clk_5m_cnt == CLK_5M_MAX - 1)
            clk_5m_cnt <= 1'b0;
        else 
            clk_5m_cnt <= clk_5m_cnt + 1'b1;
    end
    else if(ch1_freq_result > 50_000)begin
        if(clk_1m_cnt == CLK_1M_MAX - 1)
            clk_1m_cnt <= 1'b0;
        else 
            clk_1m_cnt <= clk_1m_cnt + 1'b1;
    end
    else begin 
        clk_100m_cnt <= 1'b0;
        clk_50m_cnt  <= 1'b0;
        clk_10m_cnt  <= 1'b0;
        clk_5m_cnt   <= 1'b0;
        clk_1m_cnt   <= 1'b0;
    end
end

endmodule