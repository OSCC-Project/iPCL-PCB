//==========================================================================
//  Author       ：alpha
//  Mail         ：1879136899@qq.com
//  Filename     ：
//  Create time  ：
//  Description  ：
//==========================================================================
`timescale 1ns/100ps


module wave_preprocessing(
    //system signal
    input	wire				sys_clk_50m			,
    input	wire				sys_rst_n			,
    //user signal
    input	wire	[ 7:0]		ch1_initial_data    ,
    input	wire	[ 7:0]		ch2_initial_data    ,
    input   wire    [ 3:0]      ch1_gain_select     ,
    input   wire    [ 3:0]      ch2_gain_select     ,
    input   wire    [ 3:0]      ch1_offset_select   ,
    input   wire    [ 3:0]      ch2_offset_select   ,
    output  reg     [ 7:0]      ch1_display_data    ,
    output  reg     [ 7:0]      ch2_display_data       
);


//--------------------------------------------------------------------
//------------------------- parameter define -------------------------
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//------------------------- variable define --------------------------
//--------------------------------------------------------------------
reg     [ 7:0]      ch1_offset_data     ;
reg     [12:0]      ch2_offset_data     ;


//--------------------------------------------------------------------
//-------------------------- logic function --------------------------
//--------------------------------------------------------------------

// ch1:增益
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)
        ch1_offset_data <= 8'd0;
    else begin
        case (ch1_gain_select)
        4'd0  : ch1_offset_data <= (((ch1_initial_data << 5) < 255) ? (ch1_initial_data << 5) : 255);
        4'd1  : ch1_offset_data <= (((ch1_initial_data << 4) < 255) ? (ch1_initial_data << 4) : 255);
        4'd2  : ch1_offset_data <= (((ch1_initial_data << 3) < 255) ? (ch1_initial_data << 3) : 255);
        4'd3  : ch1_offset_data <= (((ch1_initial_data << 2) < 255) ? (ch1_initial_data << 2) : 255);
        4'd4  : ch1_offset_data <= (((ch1_initial_data << 1) < 255) ? (ch1_initial_data << 1) : 255);
        4'd5  : ch1_offset_data <= ch1_initial_data;
        4'd6  : ch1_offset_data <= ch1_initial_data >> 1;
        4'd7  : ch1_offset_data <= ch1_initial_data >> 2;
        4'd8  : ch1_offset_data <= ch1_initial_data >> 3;
        4'd9  : ch1_offset_data <= ch1_initial_data >> 4;
        4'd10 : ch1_offset_data <= ch1_initial_data >> 5;
        default : ch1_offset_data <= ch1_initial_data;
    endcase
    end
end

// ch1:偏置
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)
        ch1_display_data <= 8'd0;
    else begin
        case (ch1_offset_select)
        4'd0  : ch1_display_data <= ((ch1_offset_data < 100) ? 8'd0 : (ch1_offset_data - 100));
        4'd1  : ch1_display_data <= ((ch1_offset_data <  80) ? 8'd0 : (ch1_offset_data -  80));
        4'd2  : ch1_display_data <= ((ch1_offset_data <  60) ? 8'd0 : (ch1_offset_data -  60));
        4'd3  : ch1_display_data <= ((ch1_offset_data <  40) ? 8'd0 : (ch1_offset_data -  40));
        4'd4  : ch1_display_data <= ((ch1_offset_data <  20) ? 8'd0 : (ch1_offset_data -  20));
        4'd5  : ch1_display_data <= ch1_offset_data;       
        4'd6  : ch1_display_data <= ((ch1_offset_data > 235) ? 8'd255 : (ch1_offset_data +  20));
        4'd7  : ch1_display_data <= ((ch1_offset_data > 215) ? 8'd255 : (ch1_offset_data +  40));
        4'd8  : ch1_display_data <= ((ch1_offset_data > 195) ? 8'd255 : (ch1_offset_data +  60));
        4'd9  : ch1_display_data <= ((ch1_offset_data > 175) ? 8'd255 : (ch1_offset_data +  80));
        4'd10 : ch1_display_data <= ((ch1_offset_data > 155) ? 8'd255 : (ch1_offset_data + 100));
        default : ch1_display_data <= ch1_offset_data;
    endcase
    end
end

// ch2:
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)
        ch2_offset_data <= 8'd0;
    else begin
        case (ch2_gain_select)
        4'd0  : ch2_offset_data <= (((ch2_initial_data << 5) < 255) ? (ch2_initial_data << 5) : 8'd255);
        4'd1  : ch2_offset_data <= (((ch2_initial_data << 4) < 255) ? (ch2_initial_data << 4) : 8'd255);
        4'd2  : ch2_offset_data <= (((ch2_initial_data << 3) < 255) ? (ch2_initial_data << 3) : 8'd255);
        4'd3  : ch2_offset_data <= (((ch2_initial_data << 2) < 255) ? (ch2_initial_data << 2) : 8'd255);
        4'd4  : ch2_offset_data <= (((ch2_initial_data << 1) < 255) ? (ch2_initial_data << 1) : 8'd255);
        4'd5  : ch2_offset_data <= ch2_initial_data;
        4'd6  : ch2_offset_data <= (((ch2_initial_data >> 1) > 0) ? (ch2_initial_data >> 1) : 8'd0);
        4'd7  : ch2_offset_data <= (((ch2_initial_data >> 2) > 0) ? (ch2_initial_data >> 2) : 8'd0);
        4'd8  : ch2_offset_data <= (((ch2_initial_data >> 3) > 0) ? (ch2_initial_data >> 3) : 8'd0);
        4'd9  : ch2_offset_data <= (((ch2_initial_data >> 4) > 0) ? (ch2_initial_data >> 4) : 8'd0);
        4'd10 : ch2_offset_data <= (((ch2_initial_data >> 5) > 0) ? (ch2_initial_data >> 5) : 8'd0);
        default : ch2_offset_data <= ch2_initial_data;
    endcase
    end
end

// ch2:
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)
        ch2_display_data <= 8'd0;
    else begin
        case (ch2_offset_select)
        4'd0  : ch2_display_data <= ((ch2_offset_data < 155) ? 8'd255 : (ch2_offset_data + 100));
        4'd1  : ch2_display_data <= ((ch2_offset_data < 175) ? 8'd255 : (ch2_offset_data +  80));
        4'd2  : ch2_display_data <= ((ch2_offset_data < 195) ? 8'd255 : (ch2_offset_data +  60));
        4'd3  : ch2_display_data <= ((ch2_offset_data < 215) ? 8'd255 : (ch2_offset_data +  40));
        4'd4  : ch2_display_data <= ((ch2_offset_data < 235) ? 8'd255 : (ch2_offset_data +  20));
        4'd5  : ch2_display_data <= ch2_offset_data;       
        4'd6  : ch2_display_data <= ((ch2_offset_data <  20) ? 8'd0 : (ch2_offset_data -  20));
        4'd7  : ch2_display_data <= ((ch2_offset_data <  40) ? 8'd0 : (ch2_offset_data -  40));
        4'd8  : ch2_display_data <= ((ch2_offset_data <  60) ? 8'd0 : (ch2_offset_data -  60));
        4'd9  : ch2_display_data <= ((ch2_offset_data <  80) ? 8'd0 : (ch2_offset_data -  80));
        4'd10 : ch2_display_data <= ((ch2_offset_data < 100) ? 8'd0 : (ch2_offset_data - 100));
        default : ch2_display_data <= ch2_offset_data;
    endcase
    end
end

endmodule