//==========================================================================
//  Author       ：alpha
//  Mail         ：1879136899@qq.com
//  Filename     ：
//  Create time  ：
//  Description  ：
//==========================================================================
`timescale 1ns/100ps


module adc2_data_to_display(
    //system signal
    input	wire				sys_rst_n			,
    //user signal
    input	wire	[ 3:0]		osc_freq_para       ,
    input	wire	[ 9:0]		column              ,
    input	wire	[ 0:0]		dpram_wr_clk        ,	
    input	wire	[ 0:0]		dpram_rd_clk_32m    ,
    input	wire	[ 7:0]		dpram_in_data       ,
    output	wire	[ 7:0]		dpram_out_data       		
);

//--------------------------------------------------------------------
//------------------------- parameter define -------------------------
//--------------------------------------------------------------------
parameter		WR_ADDR_MAX	    =		12'd1023    ;

//--------------------------------------------------------------------
//------------------------- variable define --------------------------
//--------------------------------------------------------------------
reg     [ 0:0]      dpram_wr_en         ;
reg     [ 9:0]      dpram_wr_addr       ;
reg     [ 0:0]      dpram_rd_en         ;
reg     [ 9:0]      dpram_rd_addr       ;

//--------------------------------------------------------------------
//-------------------------- logic function --------------------------
//--------------------------------------------------------------------

always @ (posedge dpram_wr_clk or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        dpram_rd_addr <= 9'd0;
    end
    else begin
        case (osc_freq_para)
        4'd0  : dpram_rd_addr <= column / 35;
        4'd1  : dpram_rd_addr <= column / 20;
        4'd2  : dpram_rd_addr <= column / 10;
        4'd3  : dpram_rd_addr <= column / 7;
        4'd4  : dpram_rd_addr <= column / 2;
        4'd5  : dpram_rd_addr <= column;
        4'd6  : dpram_rd_addr <= column * 2;
        4'd7  : dpram_rd_addr <= column * 7;
        4'd8  : dpram_rd_addr <= column * 10;
        4'd9  : dpram_rd_addr <= column * 35;
        4'd10 : dpram_rd_addr <= column * 70;
        default : dpram_rd_addr <= column;
    endcase
    end
end

// ram_adc_data_to_display module:
ram_adc2_data_to_display    ram_adc2_data_to_display_init(
    .data                   (dpram_in_data      )  ,
    .wraddress              (dpram_wr_addr      )  ,
    .wrclock                (dpram_wr_clk       )  ,
    .wren                   (dpram_wr_en        )  ,
    .rdaddress              (dpram_rd_addr      )  ,
    .rdclock                (dpram_rd_clk_32m   )  ,
    .rden                   (dpram_rd_en        )  ,
    .q                      (dpram_out_data     )  
);

// 写使能信号：
always @ (posedge dpram_wr_clk or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        dpram_wr_en <= 1'b0;
    end
    else if(1)begin
    // else if(dpram_rd_en == 0)begin
        if((dpram_in_data > 55) && (dpram_in_data <= 57))
            dpram_wr_en <= 1'b1;
        else if(dpram_wr_addr == WR_ADDR_MAX)
            dpram_wr_en <= 1'b0;
        else 
            dpram_wr_en <= dpram_wr_en;
    end
    else begin
        dpram_wr_en <= dpram_wr_en;
    end
end

// 写地址信号：
always @ (posedge dpram_wr_clk or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        dpram_wr_addr <= 12'd0;
    end
    else if(dpram_wr_addr == WR_ADDR_MAX)begin
        dpram_wr_addr <= 12'd0;
    end
    else if(dpram_wr_en == 1)begin
        dpram_wr_addr <= dpram_wr_addr + 10'd1;
    end
    else begin
        dpram_wr_addr <= 12'd0;
    end
end
    
// 读使能信号：
always @ (posedge dpram_rd_clk_32m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        dpram_rd_en <= 1'b0;
    end
    else if(column == 10'd0)begin
        dpram_rd_en <= 1'b1;
    end
    else if(column == 10'd700)begin
        dpram_rd_en <= 1'b0;
    end
    else begin
        dpram_rd_en <= dpram_rd_en;
    end
end		

endmodule