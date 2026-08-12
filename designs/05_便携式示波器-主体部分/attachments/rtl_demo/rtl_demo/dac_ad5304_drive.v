//==========================================================================
//  Author       ：alpha
//  Mail         ：1879136899@qq.com
//  Filename     ：
//  Create time  ：
//  Description  ：
//==========================================================================
`timescale 1ns/100ps


module dac_ad5304_drive(
    //system signal
    input	wire				sys_clk_50m			,
    input	wire				sys_rst_n			,
    //user signal
    input   wire                start               ,
    input   wire    [ 1:0]      channal_num         ,
    input   wire    [ 7:0]      dac_data            ,
    output  reg                 done                , 
    //user signal(spi_line_signal)          
    output  reg                 sclk                ,
    output  reg                 cs_n                ,		
    output  reg                 d_out
);


//--------------------------------------------------------------------
//------------------------- parameter define -------------------------
//--------------------------------------------------------------------
parameter		SCLK_GAIN_CNT_MAX   =   4'd12		;
parameter		SCLK_CNT_MAX        =   8'd66		;

//--------------------------------------------------------------------
//------------------------- variable define --------------------------
//--------------------------------------------------------------------
reg         [ 0:0]      en              ;
reg         [ 3:0]      sclk_gain_cnt   ;
reg         [ 7:0]      sclk_cnt        ;

//--------------------------------------------------------------------
//-------------------------- logic function --------------------------
//--------------------------------------------------------------------

// en signal define:
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        en <= 1'b0;
    end
    else if(start == 1'b1)begin
        en <= 1'b1;
    end
    else if(done == 1'b1)begin
        en <= 1'b0;
    end
    else begin
        en <= en;
    end
end


// sclk_gain_cnt signal define:
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        sclk_gain_cnt <= 1'b0;
    end
    else if(en == 1'b1)begin
        if(sclk_gain_cnt == SCLK_GAIN_CNT_MAX - 1'b1)begin
            sclk_gain_cnt <= 1'b0;
        end
        else begin
            sclk_gain_cnt <= sclk_gain_cnt + 1'b1;
        end
    end
    else begin
        sclk_gain_cnt <= 1'b0;    
    end
end

// sclk signal define:
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        sclk <= 1'b0;
    end
    else if(en == 1'b1)begin
        if(sclk_gain_cnt == 1'b1)begin
            sclk <= 1'b0;
        end
        else if(sclk_gain_cnt == (SCLK_GAIN_CNT_MAX - 1'b1))begin
            sclk <= 1'b1;
        end
        else begin
            sclk <= sclk;
        end
    end
    else begin
        sclk <= 1'b0;
    end
end

// sclk_cnt signal define:
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        sclk_cnt <= 1'b0;
    end
    else if(en == 1'b1)begin
        if(sclk_cnt == SCLK_CNT_MAX - 1'b1)begin
            sclk_cnt <= 1'b0;
        end
        else if(sclk_gain_cnt == SCLK_GAIN_CNT_MAX >> 2)begin
            sclk_cnt <= sclk_cnt + 1'b1;
        end
    end
    else begin
        sclk_cnt <= 1'b0;
    end
end

// done signal define:
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        done <= 1'b0;
    end
    else if(sclk_cnt == 8'd65)begin
        done <= 1'b1;
    end
    else begin
        done <= 1'b0;
    end
end

// LSM logic:
always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
    if(sys_rst_n == 1'b0)begin
        cs_n <= 1'b1;
        d_out <= 1'b1;
    end
    else if(en == 1'b1)begin
        case(sclk_cnt)
            8'd0 :  begin cs_n <= 1'b1;                         end

            8'd1 :  begin d_out <= channal_num[1];cs_n <= 1'b0; end     // channal_A1
            8'd2 :  begin d_out <= channal_num[0];              end     // channal_A0
            8'd3 :  begin d_out <= 1'b1;                        end     // PD_N
            8'd4 :  begin d_out <= 1'b0;                        end     // LDAC_N
            8'd5 :  begin d_out <= dac_data[7];                 end     // data[7]
            8'd6 :  begin d_out <= dac_data[6];                 end     // data[6]
            8'd7 :  begin d_out <= dac_data[5];                 end     // data[5]
            8'd8 :  begin d_out <= dac_data[4];                 end     // data[4]
            8'd9 :  begin d_out <= dac_data[3];                 end     // data[3]
            8'd10 : begin d_out <= dac_data[2];                 end     // data[2]
            8'd11 : begin d_out <= dac_data[1];                 end     // data[1]
            8'd12 : begin d_out <= dac_data[0];                 end     // data[0]
            8'd13 : begin d_out <= 1'b0;                        end     // 1'b0
            8'd14 : begin d_out <= 1'b0;                        end     // 1'b0
            8'd15 : begin d_out <= 1'b0;                        end     // x
            8'd16 : begin d_out <= 1'b0;                        end     // x

            8'd17 : begin cs_n <= 1'b1;                         end
            default : begin cs_n <= 1'b1;                       end
        endcase
    end
    else begin
        cs_n <= 1'b1;
        d_out <= 1'b1;
    end
end

endmodule