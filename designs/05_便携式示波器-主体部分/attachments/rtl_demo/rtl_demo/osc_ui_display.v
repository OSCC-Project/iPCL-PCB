/************************************************************************************
Author      : Alpha
Email       : 1879136899@qq.com

*************************************************************************************

module name : 
Time        : 2020.
Version     : V1.0
Chip        : EP4CE75F17C8L
OEM         : Altera(intel)
信息        :

    *********************************************************************************/
   `timescale 1ns/1ns

module osc_ui_display(
    //system signals
    input   wire            clk_40m             ,
    input   wire            rst_n               ,
    //user signals
    input   wire            vga_en              ,
    input   wire [12:0]     column              ,   // 列
    input   wire [12:0]     row                 ,   // 行
    input   wire [ 9:0]     ch1_trig_level      ,
    input   wire [ 9:0]     ch2_trig_level      ,
    input   wire [ 7:0]     ch1_wave_data       ,
    input   wire [ 7:0]     ch2_wave_data       ,
    input   wire [26:0]     ch1_freq_result     ,
    input   wire [26:0]     ch2_freq_result     ,
    input   wire [47:0]     ch1_calc_para       ,
    input   wire [47:0]     ch2_calc_para       ,
    output  reg  [23:0]     vga_rgb888
);

reg     [ 7:0]      r_ch1_wave_data         ;
reg     [ 7:0]      r_ch2_wave_data         ;

reg     [ 0:0]      char_ch1_freq_100       ;
reg     [ 0:0]      char_ch1_freq_10        ;
reg     [ 0:0]      char_ch1_freq_1         ;

reg     [ 0:0]      char_ch2_freq_100       ;
reg     [ 0:0]      char_ch2_freq_10        ;
reg     [ 0:0]      char_ch2_freq_1         ;

reg     [ 0:0]      char_ch1_vpp_1000       ;
reg     [ 0:0]      char_ch1_vpp_100        ;
reg     [ 0:0]      char_ch1_vpp_10         ;
reg     [ 0:0]      char_ch1_vpp_1          ;

reg     [ 0:0]      char_ch2_vpp_1000       ;
reg     [ 0:0]      char_ch2_vpp_100        ;
reg     [ 0:0]      char_ch2_vpp_10         ;
reg     [ 0:0]      char_ch2_vpp_1          ;

reg     [ 0:0]      char_ch1_vmin_1000      ;
reg     [ 0:0]      char_ch1_vmin_100       ;
reg     [ 0:0]      char_ch1_vmin_10        ;
reg     [ 0:0]      char_ch1_vmin_1         ;

reg     [ 0:0]      char_ch2_vmin_1000      ;
reg     [ 0:0]      char_ch2_vmin_100       ;
reg     [ 0:0]      char_ch2_vmin_10        ;
reg     [ 0:0]      char_ch2_vmin_1         ;

reg     [ 0:0]      char_ch1_vmax_1000      ;
reg     [ 0:0]      char_ch1_vmax_100       ;
reg     [ 0:0]      char_ch1_vmax_10        ;
reg     [ 0:0]      char_ch1_vmax_1         ;

reg     [ 0:0]      char_ch2_vmax_1000      ;
reg     [ 0:0]      char_ch2_vmax_100       ;
reg     [ 0:0]      char_ch2_vmax_10        ;
reg     [ 0:0]      char_ch2_vmax_1         ;

reg     [ 0:0]      char_ch1_freq_a         ;
reg     [ 0:0]      char_ch1_freq_b         ;
reg     [ 0:0]      char_ch1_freq_c         ;

reg     [ 0:0]      char_ch2_freq_a         ;
reg     [ 0:0]      char_ch2_freq_b         ;
reg     [ 0:0]      char_ch2_freq_c         ;

reg     [ 2:0]      ch1_freq_unit           ;
reg     [ 2:0]      ch2_freq_unit           ;

//----------------------- 逻辑功能 ---------------------------
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        r_ch1_wave_data <= 13'd0;
        r_ch2_wave_data <= 13'd0;
    end
    else begin
        r_ch1_wave_data <= ch1_wave_data;
        r_ch2_wave_data <= ch2_wave_data;
    end
end

always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)
        vga_rgb888 <= 24'h000000;
    else if(vga_en == 1'b1)begin
        if(column <= 700 && (row > 40 && row <= 440))begin
            // 分散格点及主轴线：
            if(((column == 50  || column == 150 || column == 250 || column == 350   || 
                 column == 450 || column == 550 || column == 650)                   &&
                (row    == 50  || row    == 100 || row    == 150 || row    == 200   ||
                 row    == 300 || row    == 350 || row    == 400))                  ||
                 column == 350 || row == 250)begin
                vga_rgb888 <= 24'hffffff;
            end
            // CH1 波形显示：
            else if(row == 256 - ch1_wave_data + 122)
                vga_rgb888 <= 24'h00ffff;
            else if(ch1_wave_data < r_ch1_wave_data && (row < 256 - ch1_wave_data + 122) && (row >= 378 - r_ch1_wave_data))
                vga_rgb888 <= 24'h00ffff;
            else if(ch1_wave_data >= r_ch1_wave_data && (row >= 256 - ch1_wave_data + 122) && (row < 378 - r_ch1_wave_data))
                vga_rgb888 <= 24'h00ffff;
            // CH2 波形显示：
            else if(row == 256 - ch2_wave_data + 122)
                vga_rgb888 <= 24'h00ff00;
            else if(ch2_wave_data < r_ch2_wave_data && (row < 256 - ch2_wave_data + 122) && (row >= 378 - r_ch2_wave_data))
                vga_rgb888 <= 24'h00ff00;
            else if(ch2_wave_data >= r_ch2_wave_data && (row >= 256 - ch2_wave_data + 122) && (row < 378 - r_ch2_wave_data))
                vga_rgb888 <= 24'h00ff00;
            // 触发线：
            else if(row == ch1_trig_level || 
            ((row > ch1_trig_level - 8) && (row <= ch1_trig_level + 8) && column <= 24))begin
                if(char_ch1_trig[row - ch1_trig_level - 8][column])
                    vga_rgb888 <= 24'h00ffff;
                else
                    vga_rgb888 <= 24'h000000;
            end
            // 测量参数显示：ch1_freq
            else if(row >= 360 && row < 376 && column >= 100 && column < 188)begin
                if(char_ch1_freq[row - 360][88 - column + 100])
                    vga_rgb888 <= 24'h00ffff;
                else 
                    vga_rgb888 <= 24'h000000;
            end
            else if(((row >= 360 && row < 376 && column >= 189 && column < 197) && char_ch1_freq_100)  ||
                    ((row >= 360 && row < 376 && column >= 197 && column < 205) && char_ch1_freq_10)   ||
                    ((row >= 360 && row < 376 && column >= 205 && column < 213) && char_ch1_freq_1))
                vga_rgb888 <= 24'h00ffff;
            else if((row >= 360 && row < 376 && column >= 213 && column < 221) && ((char_point[row - 360][8 - column + 213]) && ((ch1_freq_unit == 3'd1) || (ch1_freq_unit == 3'd2))))
                vga_rgb888 <= 24'h00ffff;
            else if((((row >= 360 && row < 376 && column >= 221 && column < 229) && char_ch1_freq_a) ||
                    ((row >= 360 && row < 376 && column >= 229 && column < 237) && char_ch1_freq_b) ||
                    ((row >= 360 && row < 376 && column >= 237 && column < 245) && char_ch1_freq_c)) && ((ch1_freq_unit == 3'd1) || (ch1_freq_unit == 3'd2)))
                vga_rgb888 <= 24'h00ffff;
            else if((row >= 360 && row < 376 && column >= 245 && column < 253) && ch1_freq_unit == 3'd1 && char_m[row - 360][8 - column + 245])
                vga_rgb888 <= 24'h00ffff;
            else if((row >= 360 && row < 376 && column >= 245 && column < 253) && ch1_freq_unit == 3'd2 && char_k[row - 360][8 - column + 245])
                vga_rgb888 <= 24'h00ffff;
            else if((row >= 360 && row < 376 && column >= 253 && column < 269) && ((ch1_freq_unit == 3'd1) || (ch1_freq_unit == 3'd2)) && (char_hz[row - 360][16 - column + 253]))
                vga_rgb888 <= 24'h00ffff;
            else if((row >= 360 && row < 376 && column >= 213 && column < 229) && (ch1_freq_unit == 3'd3) && (char_hz[row - 360][16 - column + 213]))
                vga_rgb888 <= 24'h00ffff;
            // 测量参数显示：ch2_freq
            else if((row >= 360 && row < 376 && column >= 420 && column < 508) && (char_ch2_freq[row - 360][88 - column + 420]))
                vga_rgb888 <= 24'h00ff00;
            else if(((row >= 360 && row < 376 && column >= 508 && column < 516) && char_ch2_freq_100)  ||
                    ((row >= 360 && row < 376 && column >= 516 && column < 524) && char_ch2_freq_10)   ||
                    ((row >= 360 && row < 376 && column >= 524 && column < 532) && char_ch2_freq_1))
                vga_rgb888 <= 24'h00ff00;
            else if((row >= 360 && row < 376 && column >= 532 && column < 548) && (ch2_freq_unit == 3'd3) && char_hz[row - 360][16 - column + 532])
                vga_rgb888 <= 24'h00ff00;
            else if((row >= 360 && row < 376 && column >= 532 && column < 540) && ((ch2_freq_unit == 3'd1) || (ch2_freq_unit == 3'd2)) && char_point[row - 360][8 - column + 532])
                vga_rgb888 <= 24'h00ff00;
            else if((((row >= 360 && row < 376 && column >= 540 && column < 548) && char_ch2_freq_a) ||
                    ((row >= 360 && row < 376 && column >= 548 && column < 556) && char_ch2_freq_b) ||
                    ((row >= 360 && row < 376 && column >= 556 && column < 564) && char_ch2_freq_c)) && ((ch2_freq_unit == 3'd1) || (ch2_freq_unit == 3'd2)))
                vga_rgb888 <= 24'h00ff00;
            else if((row >= 360 && row < 376 && column >= 564 && column < 572) && (ch2_freq_unit == 3'd1) && char_m[row - 360][8 - column + 564])
                vga_rgb888 <= 24'h00ff00;
            else if((row >= 360 && row < 376 && column >= 564 && column < 572) && (ch2_freq_unit == 3'd2) && char_k[row - 360][8 - column + 564])
                vga_rgb888 <= 24'h00ff00;
            else if((row >= 360 && row < 376 && column >= 572 && column < 588) && ((ch2_freq_unit == 3'd1) || (ch2_freq_unit == 3'd2)) && char_hz[row - 360][16 - column + 572])
                vga_rgb888 <= 24'h00ff00;
            // 测量参数显示：ch1_Vpp
            else if(row >= 380 && row < 396 && column >= 100 && column < 188)begin
                if(char_ch1_vpp[row - 380][88 - column + 100])
                    vga_rgb888 <= 24'h00ffff;
                else 
                    vga_rgb888 <= 24'h000000;
            end
            else if(((row >= 380 && row < 396 && column >= 189 && column < 197) && char_ch1_vpp_1000) ||
                    ((row >= 380 && row < 396 && column >= 197 && column < 205) && char_ch1_vpp_100)  ||
                    ((row >= 380 && row < 396 && column >= 205 && column < 213) && char_ch1_vpp_10)   ||
                    ((row >= 380 && row < 396 && column >= 213 && column < 221) && char_ch1_vpp_1))
                vga_rgb888 <= 24'h00ffff;
            else if(row >= 380 && row < 396 && column >= 221 && column < 237)begin
                if(char_mv[row - 380][16 - column + 221])
                    vga_rgb888 <= 24'h00ffff;
                else 
                    vga_rgb888 <= 24'h000000;
            end
            // 测量参数显示：ch2_Vpp
            else if(row >= 380 && row < 396 && column >= 420 && column < 508)begin
                if(char_ch2_vpp[row - 380][88 - column + 420])
                    vga_rgb888 <= 24'h00ff00;
                else 
                    vga_rgb888 <= 24'h000000;
            end
            else if(((row >= 380 && row < 396 && column >= 508 && column < 516) && char_ch2_vpp_1000) ||
                    ((row >= 380 && row < 396 && column >= 516 && column < 524) && char_ch2_vpp_100)  ||
                    ((row >= 380 && row < 396 && column >= 524 && column < 532) && char_ch2_vpp_10)   ||
                    ((row >= 380 && row < 396 && column >= 532 && column < 540) && char_ch2_vpp_1))
                vga_rgb888 <= 24'h00ff00;
            else if(row >= 380 && row < 396 && column >= 540 && column < 556)begin
                if(char_mv[row - 380][16 - column + 540])
                    vga_rgb888 <= 24'h00ff00;
                else 
                    vga_rgb888 <= 24'h000000;
            end
            // 测量参数显示：ch1_Vmin
            else if(row >= 400 && row < 416 && column >= 100 && column < 188)begin
                if(char_ch1_vmin[row - 400][88 - column + 100])
                    vga_rgb888 <= 24'h00ffff;
                else 
                    vga_rgb888 <= 24'h000000;
            end
            else if(((row >= 400 && row < 416 && column >= 189 && column < 197) && char_ch1_vmin_1000) ||
                    ((row >= 400 && row < 416 && column >= 197 && column < 205) && char_ch1_vmin_100)  ||
                    ((row >= 400 && row < 416 && column >= 205 && column < 213) && char_ch1_vmin_10)   ||
                    ((row >= 400 && row < 416 && column >= 213 && column < 221) && char_ch1_vmin_1))
                vga_rgb888 <= 24'h00ffff;
            else if(row >= 400 && row < 416 && column >= 221 && column < 237)begin
                if(char_mv[row - 400][16 - column + 221])
                    vga_rgb888 <= 24'h00ffff;
                else 
                    vga_rgb888 <= 24'h000000;
            end
            // 测量参数显示：ch2_Vmin
            else if(row >= 400 && row < 416 && column >= 420 && column < 508)begin
                if(char_ch2_vmin[row - 400][88 - column + 420])
                    vga_rgb888 <= 24'h00ff00;
                else 
                    vga_rgb888 <= 24'h000000;
            end
            else if(((row >= 400 && row < 416 && column >= 508 && column < 516) && char_ch2_vmin_1000) ||
                    ((row >= 400 && row < 416 && column >= 516 && column < 524) && char_ch2_vmin_100)  ||
                    ((row >= 400 && row < 416 && column >= 524 && column < 532) && char_ch2_vmin_10)   ||
                    ((row >= 400 && row < 416 && column >= 532 && column < 540) && char_ch2_vmin_1))
                vga_rgb888 <= 24'h00ff00;
            else if(row >= 400 && row < 416 && column >= 540 && column < 556)begin
                if(char_mv[row - 400][16 - column + 540])
                    vga_rgb888 <= 24'h00ff00;
                else 
                    vga_rgb888 <= 24'h000000;
            end
            // 测量参数显示：ch1_Vmax
            else if(row >= 420 && row < 436 && column >= 100 && column < 188)begin
                if(char_ch1_vmax[row - 420][88 - column + 100])
                    vga_rgb888 <= 24'h00ffff;
                else 
                    vga_rgb888 <= 24'h000000;
            end
            else if(((row >= 420 && row < 436 && column >= 189 && column < 197) && char_ch1_vmax_1000) ||
                    ((row >= 420 && row < 436 && column >= 197 && column < 205) && char_ch1_vmax_100)  ||
                    ((row >= 420 && row < 436 && column >= 205 && column < 213) && char_ch1_vmax_10)   ||
                    ((row >= 420 && row < 436 && column >= 213 && column < 221) && char_ch1_vmax_1))
                vga_rgb888 <= 24'h00ffff;
            else if(row >= 420 && row < 436 && column >= 221 && column < 237)begin
                if(char_mv[row - 420][16 - column + 221])
                    vga_rgb888 <= 24'h00ffff;
                else 
                    vga_rgb888 <= 24'h000000;
            end
            // 测量参数显示：ch2_Vmax
            else if(row >= 420 && row < 436 && column >= 420 && column < 508)begin
                if(char_ch2_vmax[row - 420][88 - column + 420])
                    vga_rgb888 <= 24'h00ff00;
                else 
                    vga_rgb888 <= 24'h000000;
            end
            else if(((row >= 420 && row < 436 && column >= 508 && column < 516) && char_ch2_vmax_1000) ||
                    ((row >= 420 && row < 436 && column >= 516 && column < 524) && char_ch2_vmax_100)  ||
                    ((row >= 420 && row < 436 && column >= 524 && column < 532) && char_ch2_vmax_10)   ||
                    ((row >= 420 && row < 436 && column >= 532 && column < 540) && char_ch2_vmax_1))
                vga_rgb888 <= 24'h00ff00;
            else if(row >= 420 && row < 436 && column >= 540 && column < 556)begin
                if(char_mv[row - 420][16 - column + 540])
                    vga_rgb888 <= 24'h00ff00;
                else 
                    vga_rgb888 <= 24'h000000;
            end
            else 
                vga_rgb888 <= 24'h000000;
        end
        // 项目名称显示：
        else if(row >= 4 && row < 36 && column >= 256 && column < 544)begin
            if(char_head[row - 4][288 - column + 256])
                vga_rgb888 <= 24'hffffff;
            else
                vga_rgb888 <= 24'h003399;
        end
        // 垂直单位指示：ch1_vid
        else if((row >= 4 && row < 20 && column >= 10 && column < 106) && (char_ch1_vid[row - 4][96 - column + 10]))
            vga_rgb888 <= 24'h00ffff;
        else if((row >= 4 && row < 20 && column >= 106 && column < 138) && (char_1000[row - 4][32 - column + 106]))
            vga_rgb888 <= 24'h00ffff;
        // 垂直单位指示：ch1_vid
        else if((row >= 22 && row < 38 && column >= 10 && column < 106) && (char_ch2_vid[row - 22][96 - column + 10]))
            vga_rgb888 <= 24'h00ff00;
        else if((row >= 22 && row < 38 && column >= 106 && column < 138) && (char_1000[row - 22][32 - column + 106]))
            vga_rgb888 <= 24'h00ff00;
        // 功能框：
        else if(row == 99  || row == 100 || row == 101 || row == 199 || row == 200 || row == 201 || 
                row == 299 || row == 300 || row == 301 || row == 399 || row == 400 || row == 401 ||
        column == 99 || column == 100 || column == 101 || column == 199 || column == 200 || column == 201 ||
        column == 299|| column == 300 || column == 301 || column == 399 || column == 400 || column == 401 ||
        column == 499|| column == 500 || column == 501 || column == 599 || column == 600 || column == 601 ||
        column == 698|| column == 699 || column == 700)begin
                vga_rgb888 <= 24'h000000;
        end
        // 水平单位指示：time_base 
        else if(row >= 45 && row < 61 && column >= 702 && column < 734)begin
            if(char_tim[row - 45][32 - column + 702])
                vga_rgb888 <= 24'hffffff;
            else 
                vga_rgb888 <= 24'h003399;
        end
        else if(row >= 45 && row < 61 && column >= 742 && column < 782)begin
            if(char_ns[row - 45][40 - column + 742])
                vga_rgb888 <= 24'hffffff;
            else 
                vga_rgb888 <= 24'h003399;
        end
        // 探头倍率选择： 
        else if(row >= 124 && row < 140 && column >= 710 && column < 782)begin
            if(char_probe1[row - 124][72 - column + 710])
                vga_rgb888 <= 24'h00ffff;
            else 
                vga_rgb888 <= 24'h003399;
        end
        else if(row >= 158 && row < 174 && column >= 710 && column < 782)begin
            if(char_probe2[row - 158][72 - column + 710])
                vga_rgb888 <= 24'h00ff00;
            else 
                vga_rgb888 <= 24'h003399;
        end
        // 通道耦合选择： 
        else if((row >= 224 && row < 240 && column >= 704 && column < 776) && (char_coupling1[row - 224][72 - column + 704]))
            vga_rgb888 <= 24'h00ffff;
        else if((row >= 224 && row < 240 && column >= 776 && column < 792) && (char_ac[row - 224][16 - column + 776]))
            vga_rgb888 <= 24'h00ffff;
        else if((row >= 258 && row < 274 && column >= 704 && column < 776) && (char_coupling2[row - 258][72 - column + 704]))
            vga_rgb888 <= 24'h00ff00;
        else if((row >= 258 && row < 274 && column >= 776 && column < 792) && (char_ac[row - 258][16 - column + 776]))
            vga_rgb888 <= 24'h00ff00;
        else 
            vga_rgb888 <= 24'h003399;
    end

    else 
        vga_rgb888 <= 24'h000000;
end

// CH1_freq:
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        ch1_freq_unit <= 3'd0;
        ch1_freq_unit <= 3'd0;
        ch1_freq_unit <= 3'd0;
    end
    else if(ch1_freq_result >= 1_000_000)
        ch1_freq_unit <= 3'd1;
    else if(ch1_freq_result >= 1_000)
        ch1_freq_unit <= 3'd2;
    else
        ch1_freq_unit <= 3'd3;
end

always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_freq_100 <= char_0[row - 360][8 - column + 189];
    end
    else if(ch1_freq_result >= 1_000_000)begin
        if(ch1_freq_result / 1_000_000 / 100 % 10 == 0)
            char_ch1_freq_100 <= char_null[row - 360][5 - column + 189];
        else begin
            case (ch1_freq_result / 1_000_000 / 100 % 10)
            0   :   char_ch1_freq_100 <= char_0[row - 360][5 - column + 189];
            1   :   char_ch1_freq_100 <= char_1[row - 360][5 - column + 189];
            2   :   char_ch1_freq_100 <= char_2[row - 360][5 - column + 189];
            3   :   char_ch1_freq_100 <= char_3[row - 360][5 - column + 189];
            4   :   char_ch1_freq_100 <= char_4[row - 360][5 - column + 189];
            5   :   char_ch1_freq_100 <= char_5[row - 360][5 - column + 189];
            6   :   char_ch1_freq_100 <= char_6[row - 360][5 - column + 189];
            7   :   char_ch1_freq_100 <= char_7[row - 360][5 - column + 189];
            8   :   char_ch1_freq_100 <= char_8[row - 360][5 - column + 189];
            9   :   char_ch1_freq_100 <= char_9[row - 360][5 - column + 189];
        default :   char_ch1_freq_100 <= char_0[row - 360][5 - column + 189];
        endcase
        end
    end
    else if(ch1_freq_result >= 1_000)begin
        if(ch1_freq_result / 1_000 / 100 % 10 == 0)
            char_ch1_freq_100 <= char_null[row - 360][5 - column + 189];
        else begin
            case (ch1_freq_result / 1_000 / 100 % 10)
            0   :   char_ch1_freq_100 <= char_0[row - 360][5 - column + 189];
            1   :   char_ch1_freq_100 <= char_1[row - 360][5 - column + 189];
            2   :   char_ch1_freq_100 <= char_2[row - 360][5 - column + 189];
            3   :   char_ch1_freq_100 <= char_3[row - 360][5 - column + 189];
            4   :   char_ch1_freq_100 <= char_4[row - 360][5 - column + 189];
            5   :   char_ch1_freq_100 <= char_5[row - 360][5 - column + 189];
            6   :   char_ch1_freq_100 <= char_6[row - 360][5 - column + 189];
            7   :   char_ch1_freq_100 <= char_7[row - 360][5 - column + 189];
            8   :   char_ch1_freq_100 <= char_8[row - 360][5 - column + 189];
            9   :   char_ch1_freq_100 <= char_9[row - 360][5 - column + 189];
        default :   char_ch1_freq_100 <= char_0[row - 360][5 - column + 189];
        endcase
        end
    end
    else if((ch1_freq_result / 1000 == 0) && ch1_freq_result / 100 % 10 == 0)
            char_ch1_freq_100 <= char_null[row - 360][5 - column + 189];
    else begin
        case (ch1_freq_result / 100 % 10)
            0   :   char_ch1_freq_100 <= char_0[row - 360][5 - column + 189];
            1   :   char_ch1_freq_100 <= char_1[row - 360][5 - column + 189];
            2   :   char_ch1_freq_100 <= char_2[row - 360][5 - column + 189];
            3   :   char_ch1_freq_100 <= char_3[row - 360][5 - column + 189];
            4   :   char_ch1_freq_100 <= char_4[row - 360][5 - column + 189];
            5   :   char_ch1_freq_100 <= char_5[row - 360][5 - column + 189];
            6   :   char_ch1_freq_100 <= char_6[row - 360][5 - column + 189];
            7   :   char_ch1_freq_100 <= char_7[row - 360][5 - column + 189];
            8   :   char_ch1_freq_100 <= char_8[row - 360][5 - column + 189];
            9   :   char_ch1_freq_100 <= char_9[row - 360][5 - column + 189];
        default :   char_ch1_freq_100 <= char_0[row - 360][5 - column + 189];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_freq_10 <= char_0[row - 360][8 - column + 197];
    end
    else if(ch1_freq_result >= 1_000_000)begin
        if((ch1_freq_result / 1_000_000 / 100 % 10 == 0) && (ch1_freq_result / 1_000_000 / 10 % 10 == 0))
            char_ch1_freq_10 <= char_null[row - 360][5 - column + 197];
        else begin
            case (ch1_freq_result / 1_000_000 / 10 % 10)
            0   :   char_ch1_freq_10 <= char_0[row - 360][5 - column + 197];
            1   :   char_ch1_freq_10 <= char_1[row - 360][5 - column + 197];
            2   :   char_ch1_freq_10 <= char_2[row - 360][5 - column + 197];
            3   :   char_ch1_freq_10 <= char_3[row - 360][5 - column + 197];
            4   :   char_ch1_freq_10 <= char_4[row - 360][5 - column + 197];
            5   :   char_ch1_freq_10 <= char_5[row - 360][5 - column + 197];
            6   :   char_ch1_freq_10 <= char_6[row - 360][5 - column + 197];
            7   :   char_ch1_freq_10 <= char_7[row - 360][5 - column + 197];
            8   :   char_ch1_freq_10 <= char_8[row - 360][5 - column + 197];
            9   :   char_ch1_freq_10 <= char_9[row - 360][5 - column + 197];
        default :   char_ch1_freq_10 <= char_0[row - 360][5 - column + 197];
        endcase
        end
    end
    else if(ch1_freq_result >= 1_000)begin
        if((ch1_freq_result / 1_000 / 100 % 10 == 0) && (ch1_freq_result / 1_000 / 10 % 10 == 0))
            char_ch1_freq_10 <= char_null[row - 360][5 - column + 197];
        else begin
            case (ch1_freq_result / 1_000 / 10 % 10)
            0   :   char_ch1_freq_10 <= char_0[row - 360][5 - column + 197];
            1   :   char_ch1_freq_10 <= char_1[row - 360][5 - column + 197];
            2   :   char_ch1_freq_10 <= char_2[row - 360][5 - column + 197];
            3   :   char_ch1_freq_10 <= char_3[row - 360][5 - column + 197];
            4   :   char_ch1_freq_10 <= char_4[row - 360][5 - column + 197];
            5   :   char_ch1_freq_10 <= char_5[row - 360][5 - column + 197];
            6   :   char_ch1_freq_10 <= char_6[row - 360][5 - column + 197];
            7   :   char_ch1_freq_10 <= char_7[row - 360][5 - column + 197];
            8   :   char_ch1_freq_10 <= char_8[row - 360][5 - column + 197];
            9   :   char_ch1_freq_10 <= char_9[row - 360][5 - column + 197];
        default :   char_ch1_freq_10 <= char_0[row - 360][5 - column + 197];
        endcase
        end
    end
    else if((ch1_freq_result / 100 % 10 == 0) && (ch1_freq_result / 10 % 10 == 0))
            char_ch1_freq_10 <= char_null[row - 360][5 - column + 197];
    else begin
            case (ch1_freq_result / 10 % 10)
            0   :   char_ch1_freq_10 <= char_0[row - 360][5 - column + 197];
            1   :   char_ch1_freq_10 <= char_1[row - 360][5 - column + 197];
            2   :   char_ch1_freq_10 <= char_2[row - 360][5 - column + 197];
            3   :   char_ch1_freq_10 <= char_3[row - 360][5 - column + 197];
            4   :   char_ch1_freq_10 <= char_4[row - 360][5 - column + 197];
            5   :   char_ch1_freq_10 <= char_5[row - 360][5 - column + 197];
            6   :   char_ch1_freq_10 <= char_6[row - 360][5 - column + 197];
            7   :   char_ch1_freq_10 <= char_7[row - 360][5 - column + 197];
            8   :   char_ch1_freq_10 <= char_8[row - 360][5 - column + 197];
            9   :   char_ch1_freq_10 <= char_9[row - 360][5 - column + 197];
        default :   char_ch1_freq_10 <= char_0[row - 360][5 - column + 197];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_freq_1 <= char_0[row - 360][8 - column + 205];
    end
    else if(ch1_freq_result >= 1_000_000)begin
        case (ch1_freq_result / 1_000_000 % 10)
            0   :   char_ch1_freq_1 <= char_0[row - 360][5 - column + 205];
            1   :   char_ch1_freq_1 <= char_1[row - 360][5 - column + 205];
            2   :   char_ch1_freq_1 <= char_2[row - 360][5 - column + 205];
            3   :   char_ch1_freq_1 <= char_3[row - 360][5 - column + 205];
            4   :   char_ch1_freq_1 <= char_4[row - 360][5 - column + 205];
            5   :   char_ch1_freq_1 <= char_5[row - 360][5 - column + 205];
            6   :   char_ch1_freq_1 <= char_6[row - 360][5 - column + 205];
            7   :   char_ch1_freq_1 <= char_7[row - 360][5 - column + 205];
            8   :   char_ch1_freq_1 <= char_8[row - 360][5 - column + 205];
            9   :   char_ch1_freq_1 <= char_9[row - 360][5 - column + 205];
        default :   char_ch1_freq_1 <= char_0[row - 360][5 - column + 205];
        endcase
    end
    else if(ch1_freq_result >= 1_000)begin
        case (ch1_freq_result / 1_000 % 10)
            0   :   char_ch1_freq_1 <= char_0[row - 360][5 - column + 205];
            1   :   char_ch1_freq_1 <= char_1[row - 360][5 - column + 205];
            2   :   char_ch1_freq_1 <= char_2[row - 360][5 - column + 205];
            3   :   char_ch1_freq_1 <= char_3[row - 360][5 - column + 205];
            4   :   char_ch1_freq_1 <= char_4[row - 360][5 - column + 205];
            5   :   char_ch1_freq_1 <= char_5[row - 360][5 - column + 205];
            6   :   char_ch1_freq_1 <= char_6[row - 360][5 - column + 205];
            7   :   char_ch1_freq_1 <= char_7[row - 360][5 - column + 205];
            8   :   char_ch1_freq_1 <= char_8[row - 360][5 - column + 205];
            9   :   char_ch1_freq_1 <= char_9[row - 360][5 - column + 205];
        default :   char_ch1_freq_1 <= char_0[row - 360][5 - column + 205];
        endcase
    end
    else begin
        case (ch1_freq_result % 10)
            0   :   char_ch1_freq_1 <= char_0[row - 360][5 - column + 205];
            1   :   char_ch1_freq_1 <= char_1[row - 360][5 - column + 205];
            2   :   char_ch1_freq_1 <= char_2[row - 360][5 - column + 205];
            3   :   char_ch1_freq_1 <= char_3[row - 360][5 - column + 205];
            4   :   char_ch1_freq_1 <= char_4[row - 360][5 - column + 205];
            5   :   char_ch1_freq_1 <= char_5[row - 360][5 - column + 205];
            6   :   char_ch1_freq_1 <= char_6[row - 360][5 - column + 205];
            7   :   char_ch1_freq_1 <= char_7[row - 360][5 - column + 205];
            8   :   char_ch1_freq_1 <= char_8[row - 360][5 - column + 205];
            9   :   char_ch1_freq_1 <= char_9[row - 360][5 - column + 205];
        default :   char_ch1_freq_1 <= char_0[row - 360][5 - column + 205];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_freq_a <= char_0[row - 360][8 - column + 221];
    end
    else if(ch1_freq_result >= 1_000_000)begin
        case (ch1_freq_result / 100_000 % 10)
            0   :   char_ch1_freq_a <= char_0[row - 360][5 - column + 221];
            1   :   char_ch1_freq_a <= char_1[row - 360][5 - column + 221];
            2   :   char_ch1_freq_a <= char_2[row - 360][5 - column + 221];
            3   :   char_ch1_freq_a <= char_3[row - 360][5 - column + 221];
            4   :   char_ch1_freq_a <= char_4[row - 360][5 - column + 221];
            5   :   char_ch1_freq_a <= char_5[row - 360][5 - column + 221];
            6   :   char_ch1_freq_a <= char_6[row - 360][5 - column + 221];
            7   :   char_ch1_freq_a <= char_7[row - 360][5 - column + 221];
            8   :   char_ch1_freq_a <= char_8[row - 360][5 - column + 221];
            9   :   char_ch1_freq_a <= char_9[row - 360][5 - column + 221];
        default :   char_ch1_freq_a <= char_0[row - 360][5 - column + 221];
        endcase
    end
    else if(ch1_freq_result >= 1_000)begin
        case (ch1_freq_result / 100 % 10)
            0   :   char_ch1_freq_a <= char_0[row - 360][5 - column + 221];
            1   :   char_ch1_freq_a <= char_1[row - 360][5 - column + 221];
            2   :   char_ch1_freq_a <= char_2[row - 360][5 - column + 221];
            3   :   char_ch1_freq_a <= char_3[row - 360][5 - column + 221];
            4   :   char_ch1_freq_a <= char_4[row - 360][5 - column + 221];
            5   :   char_ch1_freq_a <= char_5[row - 360][5 - column + 221];
            6   :   char_ch1_freq_a <= char_6[row - 360][5 - column + 221];
            7   :   char_ch1_freq_a <= char_7[row - 360][5 - column + 221];
            8   :   char_ch1_freq_a <= char_8[row - 360][5 - column + 221];
            9   :   char_ch1_freq_a <= char_9[row - 360][5 - column + 221];
        default :   char_ch1_freq_a <= char_0[row - 360][5 - column + 221];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_freq_b <= char_0[row - 360][8 - column + 229];
    end
    else if(ch1_freq_result >= 1_000_000)begin
        case (ch1_freq_result / 10_000 % 10)
            0   :   char_ch1_freq_b <= char_0[row - 360][5 - column + 229];
            1   :   char_ch1_freq_b <= char_1[row - 360][5 - column + 229];
            2   :   char_ch1_freq_b <= char_2[row - 360][5 - column + 229];
            3   :   char_ch1_freq_b <= char_3[row - 360][5 - column + 229];
            4   :   char_ch1_freq_b <= char_4[row - 360][5 - column + 229];
            5   :   char_ch1_freq_b <= char_5[row - 360][5 - column + 229];
            6   :   char_ch1_freq_b <= char_6[row - 360][5 - column + 229];
            7   :   char_ch1_freq_b <= char_7[row - 360][5 - column + 229];
            8   :   char_ch1_freq_b <= char_8[row - 360][5 - column + 229];
            9   :   char_ch1_freq_b <= char_9[row - 360][5 - column + 229];
        default :   char_ch1_freq_b <= char_0[row - 360][5 - column + 229];
        endcase
    end
    else if(ch1_freq_result >= 1_000)begin
        case (ch1_freq_result / 10 % 10)
            0   :   char_ch1_freq_b <= char_0[row - 360][5 - column + 229];
            1   :   char_ch1_freq_b <= char_1[row - 360][5 - column + 229];
            2   :   char_ch1_freq_b <= char_2[row - 360][5 - column + 229];
            3   :   char_ch1_freq_b <= char_3[row - 360][5 - column + 229];
            4   :   char_ch1_freq_b <= char_4[row - 360][5 - column + 229];
            5   :   char_ch1_freq_b <= char_5[row - 360][5 - column + 229];
            6   :   char_ch1_freq_b <= char_6[row - 360][5 - column + 229];
            7   :   char_ch1_freq_b <= char_7[row - 360][5 - column + 229];
            8   :   char_ch1_freq_b <= char_8[row - 360][5 - column + 229];
            9   :   char_ch1_freq_b <= char_9[row - 360][5 - column + 229];
        default :   char_ch1_freq_b <= char_0[row - 360][5 - column + 229];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_freq_c <= char_0[row - 360][8 - column + 237];
    end
    else if(ch1_freq_result >= 1_000_000)begin
        case (ch1_freq_result / 1000 % 10)
            0   :   char_ch1_freq_c <= char_0[row - 360][5 - column + 237];
            1   :   char_ch1_freq_c <= char_1[row - 360][5 - column + 237];
            2   :   char_ch1_freq_c <= char_2[row - 360][5 - column + 237];
            3   :   char_ch1_freq_c <= char_3[row - 360][5 - column + 237];
            4   :   char_ch1_freq_c <= char_4[row - 360][5 - column + 237];
            5   :   char_ch1_freq_c <= char_5[row - 360][5 - column + 237];
            6   :   char_ch1_freq_c <= char_6[row - 360][5 - column + 237];
            7   :   char_ch1_freq_c <= char_7[row - 360][5 - column + 237];
            8   :   char_ch1_freq_c <= char_8[row - 360][5 - column + 237];
            9   :   char_ch1_freq_c <= char_9[row - 360][5 - column + 237];
        default :   char_ch1_freq_c <= char_0[row - 360][5 - column + 237];
        endcase
    end
    else if(ch1_freq_result >= 1_000)begin
        case (ch1_freq_result % 10)
            0   :   char_ch1_freq_c <= char_0[row - 360][5 - column + 237];
            1   :   char_ch1_freq_c <= char_1[row - 360][5 - column + 237];
            2   :   char_ch1_freq_c <= char_2[row - 360][5 - column + 237];
            3   :   char_ch1_freq_c <= char_3[row - 360][5 - column + 237];
            4   :   char_ch1_freq_c <= char_4[row - 360][5 - column + 237];
            5   :   char_ch1_freq_c <= char_5[row - 360][5 - column + 237];
            6   :   char_ch1_freq_c <= char_6[row - 360][5 - column + 237];
            7   :   char_ch1_freq_c <= char_7[row - 360][5 - column + 237];
            8   :   char_ch1_freq_c <= char_8[row - 360][5 - column + 237];
            9   :   char_ch1_freq_c <= char_9[row - 360][5 - column + 237];
        default :   char_ch1_freq_c <= char_0[row - 360][5 - column + 237];
        endcase
    end
end

// CH2_freq:
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        ch2_freq_unit <= 3'd0;
        ch2_freq_unit <= 3'd0;
        ch2_freq_unit <= 3'd0;
    end
    else if(ch2_freq_result >= 1_000_000)
        ch2_freq_unit <= 3'd1;
    else if(ch2_freq_result >= 1_000)
        ch2_freq_unit <= 3'd2;
    else
        ch2_freq_unit <= 3'd3;
end

always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_freq_100 <= char_0[row - 360][8 - column + 508];
    end
    else if(ch2_freq_result >= 1_000_000)begin
        if(ch2_freq_result / 1_000_000 / 100 % 10 == 0)
            char_ch2_freq_100 <= char_null[row - 360][5 - column + 508];
        else begin
        case (ch2_freq_result / 1_000_000 / 100 % 10)
            0   :   char_ch2_freq_100 <= char_0[row - 360][5 - column + 508];
            1   :   char_ch2_freq_100 <= char_1[row - 360][5 - column + 508];
            2   :   char_ch2_freq_100 <= char_2[row - 360][5 - column + 508];
            3   :   char_ch2_freq_100 <= char_3[row - 360][5 - column + 508];
            4   :   char_ch2_freq_100 <= char_4[row - 360][5 - column + 508];
            5   :   char_ch2_freq_100 <= char_5[row - 360][5 - column + 508];
            6   :   char_ch2_freq_100 <= char_6[row - 360][5 - column + 508];
            7   :   char_ch2_freq_100 <= char_7[row - 360][5 - column + 508];
            8   :   char_ch2_freq_100 <= char_8[row - 360][5 - column + 508];
            9   :   char_ch2_freq_100 <= char_9[row - 360][5 - column + 508];
        default :   char_ch2_freq_100 <= char_0[row - 360][5 - column + 508];
        endcase
        end
    end
    else if(ch2_freq_result >= 1_000)begin
        if(ch2_freq_result / 1_000 / 100 % 10 == 0)
            char_ch2_freq_100 <= char_null[row - 360][5 - column + 508];
        else begin
        case (ch2_freq_result / 1_000 / 100 % 10)
            0   :   char_ch2_freq_100 <= char_0[row - 360][5 - column + 508];
            1   :   char_ch2_freq_100 <= char_1[row - 360][5 - column + 508];
            2   :   char_ch2_freq_100 <= char_2[row - 360][5 - column + 508];
            3   :   char_ch2_freq_100 <= char_3[row - 360][5 - column + 508];
            4   :   char_ch2_freq_100 <= char_4[row - 360][5 - column + 508];
            5   :   char_ch2_freq_100 <= char_5[row - 360][5 - column + 508];
            6   :   char_ch2_freq_100 <= char_6[row - 360][5 - column + 508];
            7   :   char_ch2_freq_100 <= char_7[row - 360][5 - column + 508];
            8   :   char_ch2_freq_100 <= char_8[row - 360][5 - column + 508];
            9   :   char_ch2_freq_100 <= char_9[row - 360][5 - column + 508];
        default :   char_ch2_freq_100 <= char_0[row - 360][5 - column + 508];
        endcase
        end
    end
    else if(ch2_freq_result / 100 % 10 == 0)
            char_ch2_freq_100 <= char_null[row - 360][5 - column + 508];
    else begin
        case (ch2_freq_result / 100 % 10)
            0   :   char_ch2_freq_100 <= char_0[row - 360][5 - column + 508];
            1   :   char_ch2_freq_100 <= char_1[row - 360][5 - column + 508];
            2   :   char_ch2_freq_100 <= char_2[row - 360][5 - column + 508];
            3   :   char_ch2_freq_100 <= char_3[row - 360][5 - column + 508];
            4   :   char_ch2_freq_100 <= char_4[row - 360][5 - column + 508];
            5   :   char_ch2_freq_100 <= char_5[row - 360][5 - column + 508];
            6   :   char_ch2_freq_100 <= char_6[row - 360][5 - column + 508];
            7   :   char_ch2_freq_100 <= char_7[row - 360][5 - column + 508];
            8   :   char_ch2_freq_100 <= char_8[row - 360][5 - column + 508];
            9   :   char_ch2_freq_100 <= char_9[row - 360][5 - column + 508];
        default :   char_ch2_freq_100 <= char_0[row - 360][5 - column + 508];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_freq_10 <= char_0[row - 360][8 - column + 516];
    end
    else if(ch2_freq_result >= 1_000_000)begin
        if((ch2_freq_result / 1_000_000 / 100 % 10 == 0) && (ch2_freq_result / 1_000_000 / 10 % 10 == 0))
            char_ch2_freq_10 <= char_null[row - 360][5 - column + 516];
        else begin
        case (ch2_freq_result / 1_000_000 / 10 % 10)
            0   :   char_ch2_freq_10 <= char_0[row - 360][5 - column + 516];
            1   :   char_ch2_freq_10 <= char_1[row - 360][5 - column + 516];
            2   :   char_ch2_freq_10 <= char_2[row - 360][5 - column + 516];
            3   :   char_ch2_freq_10 <= char_3[row - 360][5 - column + 516];
            4   :   char_ch2_freq_10 <= char_4[row - 360][5 - column + 516];
            5   :   char_ch2_freq_10 <= char_5[row - 360][5 - column + 516];
            6   :   char_ch2_freq_10 <= char_6[row - 360][5 - column + 516];
            7   :   char_ch2_freq_10 <= char_7[row - 360][5 - column + 516];
            8   :   char_ch2_freq_10 <= char_8[row - 360][5 - column + 516];
            9   :   char_ch2_freq_10 <= char_9[row - 360][5 - column + 516];
        default :   char_ch2_freq_10 <= char_0[row - 360][5 - column + 516];
        endcase
        end
    end
    else if(ch2_freq_result >= 1_000)begin
        if((ch2_freq_result / 1_000 / 100 % 10 == 0) && (ch2_freq_result / 1_000 / 10 % 10 == 0))
            char_ch2_freq_10 <= char_null[row - 360][5 - column + 516];
        else begin
        case (ch2_freq_result / 1_000 / 10 % 10)
            0   :   char_ch2_freq_10 <= char_0[row - 360][5 - column + 516];
            1   :   char_ch2_freq_10 <= char_1[row - 360][5 - column + 516];
            2   :   char_ch2_freq_10 <= char_2[row - 360][5 - column + 516];
            3   :   char_ch2_freq_10 <= char_3[row - 360][5 - column + 516];
            4   :   char_ch2_freq_10 <= char_4[row - 360][5 - column + 516];
            5   :   char_ch2_freq_10 <= char_5[row - 360][5 - column + 516];
            6   :   char_ch2_freq_10 <= char_6[row - 360][5 - column + 516];
            7   :   char_ch2_freq_10 <= char_7[row - 360][5 - column + 516];
            8   :   char_ch2_freq_10 <= char_8[row - 360][5 - column + 516];
            9   :   char_ch2_freq_10 <= char_9[row - 360][5 - column + 516];
        default :   char_ch2_freq_10 <= char_0[row - 360][5 - column + 516];
        endcase
        end
    end
    else if((ch2_freq_result / 100 % 10 == 0) && (ch2_freq_result / 10 % 10 == 0))
            char_ch2_freq_10 <= char_null[row - 360][5 - column + 516];
    else begin
        case (ch2_freq_result / 10 % 10)
            0   :   char_ch2_freq_10 <= char_0[row - 360][5 - column + 516];
            1   :   char_ch2_freq_10 <= char_1[row - 360][5 - column + 516];
            2   :   char_ch2_freq_10 <= char_2[row - 360][5 - column + 516];
            3   :   char_ch2_freq_10 <= char_3[row - 360][5 - column + 516];
            4   :   char_ch2_freq_10 <= char_4[row - 360][5 - column + 516];
            5   :   char_ch2_freq_10 <= char_5[row - 360][5 - column + 516];
            6   :   char_ch2_freq_10 <= char_6[row - 360][5 - column + 516];
            7   :   char_ch2_freq_10 <= char_7[row - 360][5 - column + 516];
            8   :   char_ch2_freq_10 <= char_8[row - 360][5 - column + 516];
            9   :   char_ch2_freq_10 <= char_9[row - 360][5 - column + 516];
        default :   char_ch2_freq_10 <= char_0[row - 360][5 - column + 516];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_freq_1 <= char_0[row - 360][8 - column + 524];
    end
    else if(ch2_freq_result >= 1_000_000)begin
        case (ch2_freq_result / 1_000_000 % 10)
            0   :   char_ch2_freq_1 <= char_0[row - 360][5 - column + 524];
            1   :   char_ch2_freq_1 <= char_1[row - 360][5 - column + 524];
            2   :   char_ch2_freq_1 <= char_2[row - 360][5 - column + 524];
            3   :   char_ch2_freq_1 <= char_3[row - 360][5 - column + 524];
            4   :   char_ch2_freq_1 <= char_4[row - 360][5 - column + 524];
            5   :   char_ch2_freq_1 <= char_5[row - 360][5 - column + 524];
            6   :   char_ch2_freq_1 <= char_6[row - 360][5 - column + 524];
            7   :   char_ch2_freq_1 <= char_7[row - 360][5 - column + 524];
            8   :   char_ch2_freq_1 <= char_8[row - 360][5 - column + 524];
            9   :   char_ch2_freq_1 <= char_9[row - 360][5 - column + 524];
        default :   char_ch2_freq_1 <= char_0[row - 360][5 - column + 524];
        endcase
    end
    else if(ch2_freq_result >= 1_000)begin
        case (ch2_freq_result / 1_000 % 10)
            0   :   char_ch2_freq_1 <= char_0[row - 360][5 - column + 524];
            1   :   char_ch2_freq_1 <= char_1[row - 360][5 - column + 524];
            2   :   char_ch2_freq_1 <= char_2[row - 360][5 - column + 524];
            3   :   char_ch2_freq_1 <= char_3[row - 360][5 - column + 524];
            4   :   char_ch2_freq_1 <= char_4[row - 360][5 - column + 524];
            5   :   char_ch2_freq_1 <= char_5[row - 360][5 - column + 524];
            6   :   char_ch2_freq_1 <= char_6[row - 360][5 - column + 524];
            7   :   char_ch2_freq_1 <= char_7[row - 360][5 - column + 524];
            8   :   char_ch2_freq_1 <= char_8[row - 360][5 - column + 524];
            9   :   char_ch2_freq_1 <= char_9[row - 360][5 - column + 524];
        default :   char_ch2_freq_1 <= char_0[row - 360][5 - column + 524];
        endcase
    end
    else begin
        case (ch2_freq_result % 10)
            0   :   char_ch2_freq_1 <= char_0[row - 360][5 - column + 524];
            1   :   char_ch2_freq_1 <= char_1[row - 360][5 - column + 524];
            2   :   char_ch2_freq_1 <= char_2[row - 360][5 - column + 524];
            3   :   char_ch2_freq_1 <= char_3[row - 360][5 - column + 524];
            4   :   char_ch2_freq_1 <= char_4[row - 360][5 - column + 524];
            5   :   char_ch2_freq_1 <= char_5[row - 360][5 - column + 524];
            6   :   char_ch2_freq_1 <= char_6[row - 360][5 - column + 524];
            7   :   char_ch2_freq_1 <= char_7[row - 360][5 - column + 524];
            8   :   char_ch2_freq_1 <= char_8[row - 360][5 - column + 524];
            9   :   char_ch2_freq_1 <= char_9[row - 360][5 - column + 524];
        default :   char_ch2_freq_1 <= char_0[row - 360][5 - column + 524];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_freq_a <= char_0[row - 360][8 - column + 540];
    end
    else if(ch2_freq_result >= 1_000_000)begin
        case (ch2_freq_result / 100_000 % 10)
            0   :   char_ch2_freq_a <= char_0[row - 360][5 - column + 540];
            1   :   char_ch2_freq_a <= char_1[row - 360][5 - column + 540];
            2   :   char_ch2_freq_a <= char_2[row - 360][5 - column + 540];
            3   :   char_ch2_freq_a <= char_3[row - 360][5 - column + 540];
            4   :   char_ch2_freq_a <= char_4[row - 360][5 - column + 540];
            5   :   char_ch2_freq_a <= char_5[row - 360][5 - column + 540];
            6   :   char_ch2_freq_a <= char_6[row - 360][5 - column + 540];
            7   :   char_ch2_freq_a <= char_7[row - 360][5 - column + 540];
            8   :   char_ch2_freq_a <= char_8[row - 360][5 - column + 540];
            9   :   char_ch2_freq_a <= char_9[row - 360][5 - column + 540];
        default :   char_ch2_freq_a <= char_0[row - 360][5 - column + 540];
        endcase
    end
    else if(ch2_freq_result >= 1_000)begin
        case (ch2_freq_result / 100 % 10)
            0   :   char_ch2_freq_a <= char_0[row - 360][5 - column + 540];
            1   :   char_ch2_freq_a <= char_1[row - 360][5 - column + 540];
            2   :   char_ch2_freq_a <= char_2[row - 360][5 - column + 540];
            3   :   char_ch2_freq_a <= char_3[row - 360][5 - column + 540];
            4   :   char_ch2_freq_a <= char_4[row - 360][5 - column + 540];
            5   :   char_ch2_freq_a <= char_5[row - 360][5 - column + 540];
            6   :   char_ch2_freq_a <= char_6[row - 360][5 - column + 540];
            7   :   char_ch2_freq_a <= char_7[row - 360][5 - column + 540];
            8   :   char_ch2_freq_a <= char_8[row - 360][5 - column + 540];
            9   :   char_ch2_freq_a <= char_9[row - 360][5 - column + 540];
        default :   char_ch2_freq_a <= char_0[row - 360][5 - column + 540];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_freq_b <= char_0[row - 360][8 - column + 548];
    end
    else if(ch2_freq_result >= 1_000_000)begin
        case (ch2_freq_result / 10_000 % 10)
            0   :   char_ch2_freq_b <= char_0[row - 360][5 - column + 548];
            1   :   char_ch2_freq_b <= char_1[row - 360][5 - column + 548];
            2   :   char_ch2_freq_b <= char_2[row - 360][5 - column + 548];
            3   :   char_ch2_freq_b <= char_3[row - 360][5 - column + 548];
            4   :   char_ch2_freq_b <= char_4[row - 360][5 - column + 548];
            5   :   char_ch2_freq_b <= char_5[row - 360][5 - column + 548];
            6   :   char_ch2_freq_b <= char_6[row - 360][5 - column + 548];
            7   :   char_ch2_freq_b <= char_7[row - 360][5 - column + 548];
            8   :   char_ch2_freq_b <= char_8[row - 360][5 - column + 548];
            9   :   char_ch2_freq_b <= char_9[row - 360][5 - column + 548];
        default :   char_ch2_freq_b <= char_0[row - 360][5 - column + 548];
        endcase
    end
    else if(ch2_freq_result >= 1_000)begin
        case (ch2_freq_result / 10 % 10)
            0   :   char_ch2_freq_b <= char_0[row - 360][5 - column + 548];
            1   :   char_ch2_freq_b <= char_1[row - 360][5 - column + 548];
            2   :   char_ch2_freq_b <= char_2[row - 360][5 - column + 548];
            3   :   char_ch2_freq_b <= char_3[row - 360][5 - column + 548];
            4   :   char_ch2_freq_b <= char_4[row - 360][5 - column + 548];
            5   :   char_ch2_freq_b <= char_5[row - 360][5 - column + 548];
            6   :   char_ch2_freq_b <= char_6[row - 360][5 - column + 548];
            7   :   char_ch2_freq_b <= char_7[row - 360][5 - column + 548];
            8   :   char_ch2_freq_b <= char_8[row - 360][5 - column + 548];
            9   :   char_ch2_freq_b <= char_9[row - 360][5 - column + 548];
        default :   char_ch2_freq_b <= char_0[row - 360][5 - column + 548];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_freq_c <= char_0[row - 360][8 - column + 5556];
    end
    else if(ch2_freq_result >= 1_000_000)begin
        case (ch2_freq_result / 10_000 % 10)
            0   :   char_ch2_freq_c <= char_0[row - 360][5 - column + 556];
            1   :   char_ch2_freq_c <= char_1[row - 360][5 - column + 556];
            2   :   char_ch2_freq_c <= char_2[row - 360][5 - column + 556];
            3   :   char_ch2_freq_c <= char_3[row - 360][5 - column + 556];
            4   :   char_ch2_freq_c <= char_4[row - 360][5 - column + 556];
            5   :   char_ch2_freq_c <= char_5[row - 360][5 - column + 556];
            6   :   char_ch2_freq_c <= char_6[row - 360][5 - column + 556];
            7   :   char_ch2_freq_c <= char_7[row - 360][5 - column + 556];
            8   :   char_ch2_freq_c <= char_8[row - 360][5 - column + 556];
            9   :   char_ch2_freq_c <= char_9[row - 360][5 - column + 556];
        default :   char_ch2_freq_c <= char_0[row - 360][5 - column + 556];
        endcase
    end
    else if(ch2_freq_result >= 1_000)begin
        case (ch2_freq_result % 10)
            0   :   char_ch2_freq_c <= char_0[row - 360][5 - column + 556];
            1   :   char_ch2_freq_c <= char_1[row - 360][5 - column + 556];
            2   :   char_ch2_freq_c <= char_2[row - 360][5 - column + 556];
            3   :   char_ch2_freq_c <= char_3[row - 360][5 - column + 556];
            4   :   char_ch2_freq_c <= char_4[row - 360][5 - column + 556];
            5   :   char_ch2_freq_c <= char_5[row - 360][5 - column + 556];
            6   :   char_ch2_freq_c <= char_6[row - 360][5 - column + 556];
            7   :   char_ch2_freq_c <= char_7[row - 360][5 - column + 556];
            8   :   char_ch2_freq_c <= char_8[row - 360][5 - column + 556];
            9   :   char_ch2_freq_c <= char_9[row - 360][5 - column + 556];
        default :   char_ch2_freq_c <= char_0[row - 360][5 - column + 556];
        endcase
    end
end

// CH1_vpp:
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_vpp_1000 <= char_0[row - 380][8 - column + 189];
    end
    else if(ch1_calc_para[47:32] / 1000 == 0)
        char_ch1_vpp_1000 <= char_null[row - 380][5 - column + 189];
    else begin
        case (ch1_calc_para[47:32] / 1000)
            0   :   char_ch1_vpp_1000 <= char_0[row - 380][5 - column + 189];
            1   :   char_ch1_vpp_1000 <= char_1[row - 380][5 - column + 189];
            2   :   char_ch1_vpp_1000 <= char_2[row - 380][5 - column + 189];
            3   :   char_ch1_vpp_1000 <= char_3[row - 380][5 - column + 189];
            4   :   char_ch1_vpp_1000 <= char_4[row - 380][5 - column + 189];
            5   :   char_ch1_vpp_1000 <= char_5[row - 380][5 - column + 189];
            6   :   char_ch1_vpp_1000 <= char_6[row - 380][5 - column + 189];
            7   :   char_ch1_vpp_1000 <= char_7[row - 380][5 - column + 189];
            8   :   char_ch1_vpp_1000 <= char_8[row - 380][5 - column + 189];
            9   :   char_ch1_vpp_1000 <= char_9[row - 380][5 - column + 189];
        default :   char_ch1_vpp_1000 <= char_0[row - 380][5 - column + 189];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_vpp_100 <= char_0[row - 380][8 - column + 189];
    end
    else if((ch1_calc_para[47:32] / 1000 == 0) && (ch1_calc_para[47:32] / 100 % 10 == 0))
        char_ch1_vpp_100 <= char_null[row - 380][5 - column + 197];
    else begin
        case (ch1_calc_para[47:32] / 100 % 10)
            0   :   char_ch1_vpp_100 <= char_0[row - 380][5 - column + 197];
            1   :   char_ch1_vpp_100 <= char_1[row - 380][5 - column + 197];
            2   :   char_ch1_vpp_100 <= char_2[row - 380][5 - column + 197];
            3   :   char_ch1_vpp_100 <= char_3[row - 380][5 - column + 197];
            4   :   char_ch1_vpp_100 <= char_4[row - 380][5 - column + 197];
            5   :   char_ch1_vpp_100 <= char_5[row - 380][5 - column + 197];
            6   :   char_ch1_vpp_100 <= char_6[row - 380][5 - column + 197];
            7   :   char_ch1_vpp_100 <= char_7[row - 380][5 - column + 197];
            8   :   char_ch1_vpp_100 <= char_8[row - 380][5 - column + 197];
            9   :   char_ch1_vpp_100 <= char_9[row - 380][5 - column + 197];
        default :   char_ch1_vpp_100 <= char_0[row - 380][5 - column + 197];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_vpp_10 <= char_0[row - 380][5 - column + 205];
    end
    else if((ch1_calc_para[47:32] / 1000 == 0) && (ch1_calc_para[47:32] / 100 % 10 == 0) && ch1_calc_para[47:32] / 10 % 10 == 0)
        char_ch1_vpp_10 <= char_null[row - 380][5 - column + 205];
    else begin
        case (ch1_calc_para[47:32] / 10 % 10)
            0   :   char_ch1_vpp_10 <= char_0[row - 380][5 - column + 205];
            1   :   char_ch1_vpp_10 <= char_1[row - 380][5 - column + 205];
            2   :   char_ch1_vpp_10 <= char_2[row - 380][5 - column + 205];
            3   :   char_ch1_vpp_10 <= char_3[row - 380][5 - column + 205];
            4   :   char_ch1_vpp_10 <= char_4[row - 380][5 - column + 205];
            5   :   char_ch1_vpp_10 <= char_5[row - 380][5 - column + 205];
            6   :   char_ch1_vpp_10 <= char_6[row - 380][5 - column + 205];
            7   :   char_ch1_vpp_10 <= char_7[row - 380][5 - column + 205];
            8   :   char_ch1_vpp_10 <= char_8[row - 380][5 - column + 205];
            9   :   char_ch1_vpp_10 <= char_9[row - 380][5 - column + 205];
        default :   char_ch1_vpp_10 <= char_0[row - 380][5 - column + 205];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_vpp_1 <= char_0[row - 380][8 - column + 213];
    end
    else begin
        case (ch1_calc_para[47:32] % 10)
            0   :   char_ch1_vpp_1 <= char_0[row - 380][5 - column + 213];
            1   :   char_ch1_vpp_1 <= char_1[row - 380][5 - column + 213];
            2   :   char_ch1_vpp_1 <= char_2[row - 380][5 - column + 213];
            3   :   char_ch1_vpp_1 <= char_3[row - 380][5 - column + 213];
            4   :   char_ch1_vpp_1 <= char_4[row - 380][5 - column + 213];
            5   :   char_ch1_vpp_1 <= char_5[row - 380][5 - column + 213];
            6   :   char_ch1_vpp_1 <= char_6[row - 380][5 - column + 213];
            7   :   char_ch1_vpp_1 <= char_7[row - 380][5 - column + 213];
            8   :   char_ch1_vpp_1 <= char_8[row - 380][5 - column + 213];
            9   :   char_ch1_vpp_1 <= char_9[row - 380][5 - column + 213];
        default :   char_ch1_vpp_1 <= char_0[row - 380][5 - column + 213];
        endcase
    end
end

// CH2_vpp:
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_vpp_1000 <= char_0[row - 380][8 - column + 508];
    end
    else if(ch2_calc_para[47:32] / 1000 == 0)
        char_ch2_vpp_1000 <= char_null[row - 380][5 - column + 508];
    else begin
        case (ch2_calc_para[47:32] / 1000)
            0   :   char_ch2_vpp_1000 <= char_0[row - 380][5 - column + 508];
            1   :   char_ch2_vpp_1000 <= char_1[row - 380][5 - column + 508];
            2   :   char_ch2_vpp_1000 <= char_2[row - 380][5 - column + 508];
            3   :   char_ch2_vpp_1000 <= char_3[row - 380][5 - column + 508];
            4   :   char_ch2_vpp_1000 <= char_4[row - 380][5 - column + 508];
            5   :   char_ch2_vpp_1000 <= char_5[row - 380][5 - column + 508];
            6   :   char_ch2_vpp_1000 <= char_6[row - 380][5 - column + 508];
            7   :   char_ch2_vpp_1000 <= char_7[row - 380][5 - column + 508];
            8   :   char_ch2_vpp_1000 <= char_8[row - 380][5 - column + 508];
            9   :   char_ch2_vpp_1000 <= char_9[row - 380][5 - column + 508];
        default :   char_ch2_vpp_1000 <= char_0[row - 380][5 - column + 508];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_vpp_100 <= char_0[row - 380][8 - column + 516];
    end
    else if((ch2_calc_para[47:32] / 1000 == 0) && (ch2_calc_para[47:32] / 100 % 10 == 0))
        char_ch2_vpp_100 <= char_null[row - 380][5 - column + 516];
    else begin
        case (ch2_calc_para[47:32] / 100 % 10)
            0   :   char_ch2_vpp_100 <= char_0[row - 380][5 - column + 516];
            1   :   char_ch2_vpp_100 <= char_1[row - 380][5 - column + 516];
            2   :   char_ch2_vpp_100 <= char_2[row - 380][5 - column + 516];
            3   :   char_ch2_vpp_100 <= char_3[row - 380][5 - column + 516];
            4   :   char_ch2_vpp_100 <= char_4[row - 380][5 - column + 516];
            5   :   char_ch2_vpp_100 <= char_5[row - 380][5 - column + 516];
            6   :   char_ch2_vpp_100 <= char_6[row - 380][5 - column + 516];
            7   :   char_ch2_vpp_100 <= char_7[row - 380][5 - column + 516];
            8   :   char_ch2_vpp_100 <= char_8[row - 380][5 - column + 516];
            9   :   char_ch2_vpp_100 <= char_9[row - 380][5 - column + 516];
        default :   char_ch2_vpp_100 <= char_0[row - 380][5 - column + 516];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_vpp_10 <= char_0[row - 380][8 - column + 524];
    end
    else if((ch2_calc_para[47:32] / 1000 == 0) && (ch2_calc_para[47:32] / 100 % 10 == 0) && (ch2_calc_para[47:32] / 10 % 10 == 0))
        char_ch2_vpp_10 <= char_null[row - 380][5 - column + 524];
    else begin
        case (ch2_calc_para[47:32] / 10 % 10)
            0   :   char_ch2_vpp_10 <= char_0[row - 380][5 - column + 524];
            1   :   char_ch2_vpp_10 <= char_1[row - 380][5 - column + 524];
            2   :   char_ch2_vpp_10 <= char_2[row - 380][5 - column + 524];
            3   :   char_ch2_vpp_10 <= char_3[row - 380][5 - column + 524];
            4   :   char_ch2_vpp_10 <= char_4[row - 380][5 - column + 524];
            5   :   char_ch2_vpp_10 <= char_5[row - 380][5 - column + 524];
            6   :   char_ch2_vpp_10 <= char_6[row - 380][5 - column + 524];
            7   :   char_ch2_vpp_10 <= char_7[row - 380][5 - column + 524];
            8   :   char_ch2_vpp_10 <= char_8[row - 380][5 - column + 524];
            9   :   char_ch2_vpp_10 <= char_9[row - 380][5 - column + 524];
        default :   char_ch2_vpp_10 <= char_0[row - 380][5 - column + 524];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_vpp_1 <= char_0[row - 380][8 - column + 532];
    end
    else begin
        case (ch2_calc_para[47:32] % 10)
            0   :   char_ch2_vpp_1 <= char_0[row - 380][5 - column + 532];
            1   :   char_ch2_vpp_1 <= char_1[row - 380][5 - column + 532];
            2   :   char_ch2_vpp_1 <= char_2[row - 380][5 - column + 532];
            3   :   char_ch2_vpp_1 <= char_3[row - 380][5 - column + 532];
            4   :   char_ch2_vpp_1 <= char_4[row - 380][5 - column + 532];
            5   :   char_ch2_vpp_1 <= char_5[row - 380][5 - column + 532];
            6   :   char_ch2_vpp_1 <= char_6[row - 380][5 - column + 532];
            7   :   char_ch2_vpp_1 <= char_7[row - 380][5 - column + 532];
            8   :   char_ch2_vpp_1 <= char_8[row - 380][5 - column + 532];
            9   :   char_ch2_vpp_1 <= char_9[row - 380][5 - column + 532];
        default :   char_ch2_vpp_1 <= char_0[row - 380][5 - column + 532];
        endcase
    end
end

// CH1_min:
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_vmin_1000 <= char_0[row - 400][8 - column + 189];
    end
    else if(ch1_calc_para[15:0] / 1000 == 0)
        char_ch1_vmin_1000 <= char_null[row - 400][5 - column + 189];
    else begin
        case (ch1_calc_para[15:0] / 1000)
            0   :   char_ch1_vmin_1000 <= char_0[row - 400][5 - column + 189];
            1   :   char_ch1_vmin_1000 <= char_1[row - 400][5 - column + 189];
            2   :   char_ch1_vmin_1000 <= char_2[row - 400][5 - column + 189];
            3   :   char_ch1_vmin_1000 <= char_3[row - 400][5 - column + 189];
            4   :   char_ch1_vmin_1000 <= char_4[row - 400][5 - column + 189];
            5   :   char_ch1_vmin_1000 <= char_5[row - 400][5 - column + 189];
            6   :   char_ch1_vmin_1000 <= char_6[row - 400][5 - column + 189];
            7   :   char_ch1_vmin_1000 <= char_7[row - 400][5 - column + 189];
            8   :   char_ch1_vmin_1000 <= char_8[row - 400][5 - column + 189];
            9   :   char_ch1_vmin_1000 <= char_9[row - 400][5 - column + 189];
        default :   char_ch1_vmin_1000 <= char_0[row - 400][5 - column + 189];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_vmin_100 <= char_0[row - 400][8 - column + 189];
    end
    else if((ch1_calc_para[15:0] / 1000 == 0) && (ch1_calc_para[15:0] / 100 % 10 == 0))
        char_ch1_vmin_100 <= char_null[row - 400][5 - column + 197];
    else begin
        case (ch1_calc_para[15:0] / 100 % 10)
            0   :   char_ch1_vmin_100 <= char_0[row - 400][5 - column + 197];
            1   :   char_ch1_vmin_100 <= char_1[row - 400][5 - column + 197];
            2   :   char_ch1_vmin_100 <= char_2[row - 400][5 - column + 197];
            3   :   char_ch1_vmin_100 <= char_3[row - 400][5 - column + 197];
            4   :   char_ch1_vmin_100 <= char_4[row - 400][5 - column + 197];
            5   :   char_ch1_vmin_100 <= char_5[row - 400][5 - column + 197];
            6   :   char_ch1_vmin_100 <= char_6[row - 400][5 - column + 197];
            7   :   char_ch1_vmin_100 <= char_7[row - 400][5 - column + 197];
            8   :   char_ch1_vmin_100 <= char_8[row - 400][5 - column + 197];
            9   :   char_ch1_vmin_100 <= char_9[row - 400][5 - column + 197];
        default :   char_ch1_vmin_100 <= char_0[row - 400][5 - column + 197];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_vmin_10 <= char_0[row - 400][8 - column + 205];
    end
    else if((ch1_calc_para[15:0] / 1000 == 0) && (ch1_calc_para[15:0] / 100 % 10 == 0) && (ch1_calc_para[15:0] / 10 % 10 == 0))
        char_ch1_vmin_10 <= char_null[row - 400][5 - column + 205];
    else begin
        case (ch1_calc_para[15:0] / 10 % 10)
            0   :   char_ch1_vmin_10 <= char_0[row - 400][5 - column + 205];
            1   :   char_ch1_vmin_10 <= char_1[row - 400][5 - column + 205];
            2   :   char_ch1_vmin_10 <= char_2[row - 400][5 - column + 205];
            3   :   char_ch1_vmin_10 <= char_3[row - 400][5 - column + 205];
            4   :   char_ch1_vmin_10 <= char_4[row - 400][5 - column + 205];
            5   :   char_ch1_vmin_10 <= char_5[row - 400][5 - column + 205];
            6   :   char_ch1_vmin_10 <= char_6[row - 400][5 - column + 205];
            7   :   char_ch1_vmin_10 <= char_7[row - 400][5 - column + 205];
            8   :   char_ch1_vmin_10 <= char_8[row - 400][5 - column + 205];
            9   :   char_ch1_vmin_10 <= char_9[row - 400][5 - column + 205];
        default :   char_ch1_vmin_10 <= char_0[row - 400][5 - column + 205];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_vmin_1 <= char_0[row - 400][8 - column + 213];
    end
    else begin
        case (ch1_calc_para[15:0] % 10)
            0   :   char_ch1_vmin_1 <= char_0[row - 400][5 - column + 213];
            1   :   char_ch1_vmin_1 <= char_1[row - 400][5 - column + 213];
            2   :   char_ch1_vmin_1 <= char_2[row - 400][5 - column + 213];
            3   :   char_ch1_vmin_1 <= char_3[row - 400][5 - column + 213];
            4   :   char_ch1_vmin_1 <= char_4[row - 400][5 - column + 213];
            5   :   char_ch1_vmin_1 <= char_5[row - 400][5 - column + 213];
            6   :   char_ch1_vmin_1 <= char_6[row - 400][5 - column + 213];
            7   :   char_ch1_vmin_1 <= char_7[row - 400][5 - column + 213];
            8   :   char_ch1_vmin_1 <= char_8[row - 400][5 - column + 213];
            9   :   char_ch1_vmin_1 <= char_9[row - 400][5 - column + 213];
        default :   char_ch1_vmin_1 <= char_0[row - 400][5 - column + 213];
        endcase
    end
end

// CH2_vmin:
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_vmin_1000 <= char_0[row - 400][8 - column + 508];
    end
    else if(ch2_calc_para[15:0] / 1000 == 0)
        char_ch2_vmin_1000 <= char_null[row - 400][5 - column + 508];
    else begin
        case (ch2_calc_para[15:0] / 1000)
            0   :   char_ch2_vmin_1000 <= char_0[row - 400][5 - column + 508];
            1   :   char_ch2_vmin_1000 <= char_1[row - 400][5 - column + 508];
            2   :   char_ch2_vmin_1000 <= char_2[row - 400][5 - column + 508];
            3   :   char_ch2_vmin_1000 <= char_3[row - 400][5 - column + 508];
            4   :   char_ch2_vmin_1000 <= char_4[row - 400][5 - column + 508];
            5   :   char_ch2_vmin_1000 <= char_5[row - 400][5 - column + 508];
            6   :   char_ch2_vmin_1000 <= char_6[row - 400][5 - column + 508];
            7   :   char_ch2_vmin_1000 <= char_7[row - 400][5 - column + 508];
            8   :   char_ch2_vmin_1000 <= char_8[row - 400][5 - column + 508];
            9   :   char_ch2_vmin_1000 <= char_9[row - 400][5 - column + 508];
        default :   char_ch2_vmin_1000 <= char_0[row - 400][5 - column + 508];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_vmin_100 <= char_0[row - 400][8 - column + 516];
    end
    else if((ch2_calc_para[15:0] / 1000 == 0) && (ch2_calc_para[15:0] / 100 % 10 == 0))
        char_ch2_vmin_100 <= char_null[row - 400][5 - column + 516];
    else begin
        case (ch2_calc_para[15:0] / 100 % 10)
            0   :   char_ch2_vmin_100 <= char_0[row - 400][5 - column + 516];
            1   :   char_ch2_vmin_100 <= char_1[row - 400][5 - column + 516];
            2   :   char_ch2_vmin_100 <= char_2[row - 400][5 - column + 516];
            3   :   char_ch2_vmin_100 <= char_3[row - 400][5 - column + 516];
            4   :   char_ch2_vmin_100 <= char_4[row - 400][5 - column + 516];
            5   :   char_ch2_vmin_100 <= char_5[row - 400][5 - column + 516];
            6   :   char_ch2_vmin_100 <= char_6[row - 400][5 - column + 516];
            7   :   char_ch2_vmin_100 <= char_7[row - 400][5 - column + 516];
            8   :   char_ch2_vmin_100 <= char_8[row - 400][5 - column + 516];
            9   :   char_ch2_vmin_100 <= char_9[row - 400][5 - column + 516];
        default :   char_ch2_vmin_100 <= char_0[row - 400][5 - column + 516];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_vmin_10 <= char_0[row -400][8 - column + 524];
    end
    else if((ch2_calc_para[15:0] / 1000 == 0) && (ch2_calc_para[15:0] / 100 % 10 == 0) && (ch2_calc_para[15:0] / 10 % 10 == 0))
        char_ch2_vmin_10 <= char_null[row - 400][5 - column + 524];
    else begin
        case (ch2_calc_para[15:0] / 10 % 10)
            0   :   char_ch2_vmin_10 <= char_0[row - 400][5 - column + 524];
            1   :   char_ch2_vmin_10 <= char_1[row - 400][5 - column + 524];
            2   :   char_ch2_vmin_10 <= char_2[row - 400][5 - column + 524];
            3   :   char_ch2_vmin_10 <= char_3[row - 400][5 - column + 524];
            4   :   char_ch2_vmin_10 <= char_4[row - 400][5 - column + 524];
            5   :   char_ch2_vmin_10 <= char_5[row - 400][5 - column + 524];
            6   :   char_ch2_vmin_10 <= char_6[row - 400][5 - column + 524];
            7   :   char_ch2_vmin_10 <= char_7[row - 400][5 - column + 524];
            8   :   char_ch2_vmin_10 <= char_8[row - 400][5 - column + 524];
            9   :   char_ch2_vmin_10 <= char_9[row - 400][5 - column + 524];
        default :   char_ch2_vmin_10 <= char_0[row - 400][5 - column + 524];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_vmin_1 <= char_0[row - 400][8 - column + 532];
    end
    else begin
        case (ch2_calc_para[15:0] % 10)
            0   :   char_ch2_vmin_1 <= char_0[row - 400][5 - column + 532];
            1   :   char_ch2_vmin_1 <= char_1[row - 400][5 - column + 532];
            2   :   char_ch2_vmin_1 <= char_2[row - 400][5 - column + 532];
            3   :   char_ch2_vmin_1 <= char_3[row - 400][5 - column + 532];
            4   :   char_ch2_vmin_1 <= char_4[row - 400][5 - column + 532];
            5   :   char_ch2_vmin_1 <= char_5[row - 400][5 - column + 532];
            6   :   char_ch2_vmin_1 <= char_6[row - 400][5 - column + 532];
            7   :   char_ch2_vmin_1 <= char_7[row - 400][5 - column + 532];
            8   :   char_ch2_vmin_1 <= char_8[row - 400][5 - column + 532];
            9   :   char_ch2_vmin_1 <= char_9[row - 400][5 - column + 532];
        default :   char_ch2_vmin_1 <= char_0[row - 400][5 - column + 532];
        endcase
    end
end

// CH1_Vmax:
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_vmax_1000 <= char_0[row - 420][8 - column + 189];
    end
    else if(ch1_calc_para[31:16] / 1000 == 0)
        char_ch1_vmax_1000 <= char_null[row - 420][5 - column + 189];
    else begin
        case (ch1_calc_para[31:16] / 1000)
            0   :   char_ch1_vmax_1000 <= char_0[row - 420][5 - column + 189];
            1   :   char_ch1_vmax_1000 <= char_1[row - 420][5 - column + 189];
            2   :   char_ch1_vmax_1000 <= char_2[row - 420][5 - column + 189];
            3   :   char_ch1_vmax_1000 <= char_3[row - 420][5 - column + 189];
            4   :   char_ch1_vmax_1000 <= char_4[row - 420][5 - column + 189];
            5   :   char_ch1_vmax_1000 <= char_5[row - 420][5 - column + 189];
            6   :   char_ch1_vmax_1000 <= char_6[row - 420][5 - column + 189];
            7   :   char_ch1_vmax_1000 <= char_7[row - 420][5 - column + 189];
            8   :   char_ch1_vmax_1000 <= char_8[row - 420][5 - column + 189];
            9   :   char_ch1_vmax_1000 <= char_9[row - 420][5 - column + 189];
        default :   char_ch1_vmax_1000 <= char_0[row - 420][5 - column + 189];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_vmax_100 <= char_0[row - 420][8 - column + 197];
    end
    else if((ch1_calc_para[31:16] / 1000 == 0) && (ch1_calc_para[31:16] / 100 % 10 == 0))
        char_ch1_vmax_100 <= char_null[row - 420][5 - column + 197];
    else begin
        case (ch1_calc_para[31:16] / 100 % 10)
            0   :   char_ch1_vmax_100 <= char_0[row - 420][5 - column + 197];
            1   :   char_ch1_vmax_100 <= char_1[row - 420][5 - column + 197];
            2   :   char_ch1_vmax_100 <= char_2[row - 420][5 - column + 197];
            3   :   char_ch1_vmax_100 <= char_3[row - 420][5 - column + 197];
            4   :   char_ch1_vmax_100 <= char_4[row - 420][5 - column + 197];
            5   :   char_ch1_vmax_100 <= char_5[row - 420][5 - column + 197];
            6   :   char_ch1_vmax_100 <= char_6[row - 420][5 - column + 197];
            7   :   char_ch1_vmax_100 <= char_7[row - 420][5 - column + 197];
            8   :   char_ch1_vmax_100 <= char_8[row - 420][5 - column + 197];
            9   :   char_ch1_vmax_100 <= char_9[row - 420][5 - column + 197];
        default :   char_ch1_vmax_100 <= char_0[row - 420][5 - column + 197];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_vmax_10 <= char_0[row - 420][8 - column + 205];
    end
    else if((ch1_calc_para[31:16] / 1000 == 0) && (ch1_calc_para[31:16] / 100 % 10 == 0) && (ch1_calc_para[31:16] / 10 % 10 == 0))
        char_ch1_vmax_10 <= char_null[row - 420][5 - column + 205];
    else begin
        case (ch1_calc_para[31:16] / 10 % 10)
            0   :   char_ch1_vmax_10 <= char_0[row - 420][5 - column + 205];
            1   :   char_ch1_vmax_10 <= char_1[row - 420][5 - column + 205];
            2   :   char_ch1_vmax_10 <= char_2[row - 420][5 - column + 205];
            3   :   char_ch1_vmax_10 <= char_3[row - 420][5 - column + 205];
            4   :   char_ch1_vmax_10 <= char_4[row - 420][5 - column + 205];
            5   :   char_ch1_vmax_10 <= char_5[row - 420][5 - column + 205];
            6   :   char_ch1_vmax_10 <= char_6[row - 420][5 - column + 205];
            7   :   char_ch1_vmax_10 <= char_7[row - 420][5 - column + 205];
            8   :   char_ch1_vmax_10 <= char_8[row - 420][5 - column + 205];
            9   :   char_ch1_vmax_10 <= char_9[row - 420][5 - column + 205];
        default :   char_ch1_vmax_10 <= char_0[row - 420][5 - column + 205];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch1_vmax_1 <= char_0[row - 420][8 - column + 213];
    end
    else begin
        case (ch1_calc_para[31:16] % 10)
            0   :   char_ch1_vmax_1 <= char_0[row - 420][5 - column + 213];
            1   :   char_ch1_vmax_1 <= char_1[row - 420][5 - column + 213];
            2   :   char_ch1_vmax_1 <= char_2[row - 420][5 - column + 213];
            3   :   char_ch1_vmax_1 <= char_3[row - 420][5 - column + 213];
            4   :   char_ch1_vmax_1 <= char_4[row - 420][5 - column + 213];
            5   :   char_ch1_vmax_1 <= char_5[row - 420][5 - column + 213];
            6   :   char_ch1_vmax_1 <= char_6[row - 420][5 - column + 213];
            7   :   char_ch1_vmax_1 <= char_7[row - 420][5 - column + 213];
            8   :   char_ch1_vmax_1 <= char_8[row - 420][5 - column + 213];
            9   :   char_ch1_vmax_1 <= char_9[row - 420][5 - column + 213];
        default :   char_ch1_vmax_1 <= char_0[row - 420][5 - column + 213];
        endcase
    end
end

// CH2_Vmax:
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_vmax_1000 <= char_0[row - 420][8 - column + 508];
    end
    else if(ch2_calc_para[31:16] / 1000 == 0)
        char_ch2_vmax_1000 <= char_null[row - 420][5 - column + 508];
    else begin
        case (ch2_calc_para[31:16] / 1000)
            1   :   char_ch2_vmax_1000 <= char_1[row - 420][5 - column + 508];
            2   :   char_ch2_vmax_1000 <= char_2[row - 420][5 - column + 508];
            3   :   char_ch2_vmax_1000 <= char_3[row - 420][5 - column + 508];
            4   :   char_ch2_vmax_1000 <= char_4[row - 420][5 - column + 508];
            5   :   char_ch2_vmax_1000 <= char_5[row - 420][5 - column + 508];
            6   :   char_ch2_vmax_1000 <= char_6[row - 420][5 - column + 508];
            7   :   char_ch2_vmax_1000 <= char_7[row - 420][5 - column + 508];
            8   :   char_ch2_vmax_1000 <= char_8[row - 420][5 - column + 508];
            9   :   char_ch2_vmax_1000 <= char_9[row - 420][5 - column + 508];
        default :   char_ch2_vmax_1000 <= char_null[row - 420][5 - column + 508];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_vmax_100 <= char_0[row - 420][8 - column + 516];
    end
    else if((ch2_calc_para[31:16] / 1000 == 0) && (ch2_calc_para[31:16] / 100 % 10 == 0))
        char_ch2_vmax_100 <= char_null[row - 420][5 - column + 516];
    else begin
        case (ch2_calc_para[31:16] / 100 % 10)
            0   :   char_ch2_vmax_100 <= char_0[row - 420][5 - column + 516];
            1   :   char_ch2_vmax_100 <= char_1[row - 420][5 - column + 516];
            2   :   char_ch2_vmax_100 <= char_2[row - 420][5 - column + 516];
            3   :   char_ch2_vmax_100 <= char_3[row - 420][5 - column + 516];
            4   :   char_ch2_vmax_100 <= char_4[row - 420][5 - column + 516];
            5   :   char_ch2_vmax_100 <= char_5[row - 420][5 - column + 516];
            6   :   char_ch2_vmax_100 <= char_6[row - 420][5 - column + 516];
            7   :   char_ch2_vmax_100 <= char_7[row - 420][5 - column + 516];
            8   :   char_ch2_vmax_100 <= char_8[row - 420][5 - column + 516];
            9   :   char_ch2_vmax_100 <= char_9[row - 420][5 - column + 516];
        default :   char_ch2_vmax_100 <= char_0[row - 420][5 - column + 516];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_vmax_10 <= char_0[row - 420][8 - column + 524];
    end
    else if((ch2_calc_para[31:16] / 1000 == 0) && (ch2_calc_para[31:16] / 100 % 10 == 0) && (ch2_calc_para[31:16] / 10 % 10 == 0))
        char_ch2_vmax_10 <= char_null[row - 420][5 - column + 524];
    else begin
        case (ch2_calc_para[31:16] / 10 % 10)
            0   :   char_ch2_vmax_10 <= char_0[row - 420][5 - column + 524];
            1   :   char_ch2_vmax_10 <= char_1[row - 420][5 - column + 524];
            2   :   char_ch2_vmax_10 <= char_2[row - 420][5 - column + 524];
            3   :   char_ch2_vmax_10 <= char_3[row - 420][5 - column + 524];
            4   :   char_ch2_vmax_10 <= char_4[row - 420][5 - column + 524];
            5   :   char_ch2_vmax_10 <= char_5[row - 420][5 - column + 524];
            6   :   char_ch2_vmax_10 <= char_6[row - 420][5 - column + 524];
            7   :   char_ch2_vmax_10 <= char_7[row - 420][5 - column + 524];
            8   :   char_ch2_vmax_10 <= char_8[row - 420][5 - column + 524];
            9   :   char_ch2_vmax_10 <= char_9[row - 420][5 - column + 524];
        default :   char_ch2_vmax_10 <= char_0[row - 420][5 - column + 524];
        endcase
    end
end
always @ (posedge clk_40m or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        char_ch2_vmax_1 <= char_0[row - 420][8 - column + 213];
    end
    else begin
        case (ch2_calc_para[31:16] % 10)
            0   :   char_ch2_vmax_1 <= char_0[row - 420][5 - column + 532];
            1   :   char_ch2_vmax_1 <= char_1[row - 420][5 - column + 532];
            2   :   char_ch2_vmax_1 <= char_2[row - 420][5 - column + 532];
            3   :   char_ch2_vmax_1 <= char_3[row - 420][5 - column + 532];
            4   :   char_ch2_vmax_1 <= char_4[row - 420][5 - column + 532];
            5   :   char_ch2_vmax_1 <= char_5[row - 420][5 - column + 532];
            6   :   char_ch2_vmax_1 <= char_6[row - 420][5 - column + 532];
            7   :   char_ch2_vmax_1 <= char_7[row - 420][5 - column + 532];
            8   :   char_ch2_vmax_1 <= char_8[row - 420][5 - column + 532];
            9   :   char_ch2_vmax_1 <= char_9[row - 420][5 - column + 532];
        default :   char_ch2_vmax_1 <= char_0[row - 420][5 - column + 532];
        endcase
    end
end

//----------------------- 字符编码 ---------------------------

reg     [ 23:0]     char_ch1_trig   [15:0]  ;
reg     [287:0]     char_head       [31:0]  ;
reg     [ 87:0]     char_ch1_freq   [15:0]  ;
reg     [ 87:0]     char_ch2_freq   [15:0]  ;
reg     [ 87:0]     char_ch1_vpp    [15:0]  ;
reg     [ 87:0]     char_ch2_vpp    [15:0]  ;
reg     [ 87:0]     char_ch1_vmin   [15:0]  ;
reg     [ 87:0]     char_ch2_vmin   [15:0]  ;
reg     [ 87:0]     char_ch1_vmax   [15:0]  ;
reg     [ 87:0]     char_ch2_vmax   [15:0]  ;
reg     [ 95:0]     char_ch1_vid    [15:0]  ;
reg     [ 95:0]     char_ch2_vid    [15:0]  ;
reg     [ 31:0]     char_tim        [15:0]  ;
reg     [ 47:0]     char_ns         [15:0]  ;
reg     [ 79:0]     char_probe1     [15:0]  ;
reg     [ 79:0]     char_probe2     [15:0]  ;
reg     [ 71:0]     char_coupling1  [15:0]  ;
reg     [ 71:0]     char_coupling2  [15:0]  ;
reg     [  7:0]     char_0          [15:0]  ;
reg     [  7:0]     char_1          [15:0]  ;
reg     [  7:0]     char_2          [15:0]  ;
reg     [  7:0]     char_3          [15:0]  ;
reg     [  7:0]     char_4          [15:0]  ;
reg     [  7:0]     char_5          [15:0]  ;
reg     [  7:0]     char_6          [15:0]  ;
reg     [  7:0]     char_7          [15:0]  ;
reg     [  7:0]     char_8          [15:0]  ;
reg     [  7:0]     char_9          [15:0]  ;
reg     [ 15:0]     char_mv         [15:0]  ;
reg     [  7:0]     char_m          [15:0]  ;
reg     [  7:0]     char_k          [15:0]  ;
reg     [  7:0]     char_point      [15:0]  ;
reg     [  7:0]     char_null       [15:0]  ;
reg     [ 15:0]     char_hz         [15:0]  ;
reg     [ 15:0]     char_ac         [15:0]  ;
reg     [ 31:0]     char_1000       [15:0]  ;

// CH1:Trig_line
always @ (posedge clk_40m)begin
    char_ch1_trig[ 0] <= 24'h000000;
    char_ch1_trig[ 1] <= 24'h000000;
    char_ch1_trig[ 2] <= 24'h000000;
    char_ch1_trig[ 3] <= 24'h100000;
    char_ch1_trig[ 4] <= 24'h1C0300;
    char_ch1_trig[ 5] <= 24'h100200;
    char_ch1_trig[ 6] <= 24'h100200;
    char_ch1_trig[ 7] <= 24'h103A38;
    char_ch1_trig[ 8] <= 24'h104644;
    char_ch1_trig[ 9] <= 24'h104202;
    char_ch1_trig[10] <= 24'h104202;
    char_ch1_trig[11] <= 24'h104202;
    char_ch1_trig[12] <= 24'h104244;
    char_ch1_trig[13] <= 24'h7CE738;
    char_ch1_trig[14] <= 24'h000000;
    char_ch1_trig[15] <= 24'h000000;
end

// 多功能便携式示波器：
always @ (posedge clk_40m)begin
    char_head[ 0] <= 288'h000000000000000000000000000000000000000000000000000000000000000000000000;
    char_head[ 1] <= 288'h000018000000000000000000000000000000000000000000000000000000000000000000;
    char_head[ 2] <= 288'h00003E00000000000000000000000000000400000000F000000000000000000000000000;
    char_head[ 3] <= 288'h00007FE000000000000200000000000000070730000060000000000000000E0000000000;
    char_head[ 4] <= 288'h0000FFFC0000002000070F00001001E0000E0638000060000000000000000F0000000000;
    char_head[ 5] <= 288'h0001F9F800000038000E0E20003DFFF0000C0C300000602000000FE000000E00000007F0;
    char_head[ 6] <= 288'h0003E3F000000078001C0C700038FF00000C186000006060000FFFC000400E00003FFFF8;
    char_head[ 7] <= 288'h000FFFE000000070003A0DE00070FE00000C1860000030780007F80000C00E000079DC70;
    char_head[ 8] <= 288'h003FFFC0000000E000738F0000F00FFE000C37FE0000307C0003000000E00FF000719CF0;
    char_head[ 9] <= 288'h007E7F00007E00E000E1DC0801E03FFE001F3FE000003E000000000000F01FBC00779FE0;
    char_head[10] <= 288'h00787FF007FE03FE01FFFC0801C7FE1E07FF70FC0001FF000000000000F8FE1E003F8FEC;
    char_head[11] <= 288'h0000FFF807FC7FFF07FCDC180387CE9C07FEF7E003FFFE00000001FF0001CE1E001CE01E;
    char_head[12] <= 288'h0003EFFC07F87FCF0FE00EFC07077F9C03DCE0E603FFF8000007FFFF00018EFE0000C16E;
    char_head[13] <= 288'h001FCFFF00703787070707FC0F037E3C010D67FC01E0380007FFFFC303818E000001FFE7;
    char_head[14] <= 288'h000FBF9F00730F07007F80001FC3BE38001E60E30000180007FE780003918FE0007FFFE0;
    char_head[15] <= 288'h00007F3F007E0E0700E398003FC18FB8003C33FF000798000300380003118FF000FFBC00;
    char_head[16] <= 288'h0001FC7E007C1C0F00FB98707380FFF000783E00007F9C0000103840061390E000460E00;
    char_head[17] <= 288'h0007FFFC00F0380E01F318F0E3987CF000F830F803FE0C00007038780633C0C0001C0380;
    char_head[18] <= 288'h003FDFF003E0701C01C31BC0039E1C0003D81FFC03FC0E0000E0183E0423F0C0003001C0;
    char_head[19] <= 288'h003E0FE01FC0E01C01FB1F02038FF8000799FF380138870001E0381E04661F8000FFBFF0;
    char_head[20] <= 288'h00003FE01F01E03801F31C030381F8000F19EC30003FC30003C0381E07C607C003FFFFFC;
    char_head[21] <= 288'h0000FFE00E07D07801C318030380FE001E18187C007F038003C0380E07CE07E01FE3F1EF;
    char_head[22] <= 288'h0001FBE0000F8FF0018318030381C7801C18300E03F801C001CFF80C039C0E78006331C7;
    char_head[23] <= 288'h0007E3C0001E07E003471803038F00E00018200E1FC000E00007F80003383C3E007F3FC3;
    char_head[24] <= 288'h003F8000006003E0033F1C0703FE003E01F8E00C070000780000F80000E7F01F003F3F80;
    char_head[25] <= 288'h01FF0000000001C0000F0FFF0380001F0079C61C0400001F000000000100001F00381800;
    char_head[26] <= 288'h07FE000000000000000107FF0300001F001A03F800000007000000000000000000000000;
    char_head[27] <= 288'h3FF80000000000000000000000000000000001F000000001000000000000000000000000;
    char_head[28] <= 288'h000000000000000000000000000000000000006000000000000000000000000000000000;
    char_head[29] <= 288'h000000000000000000000000000000000000000000000000000000000000000000000000;
    char_head[30] <= 288'h000000000000000000000000000000000000000000000000000000000000000000000000;
    char_head[31] <= 288'h000000000000000000000000000000000000000000000000000000000000000000000000;
end

// CH1:Freq = 
always @ (posedge clk_40m)begin
    char_ch1_freq[ 0] <= 176'h0000000000000000000000;
    char_ch1_freq[ 1] <= 176'h0000000000000000000000;
    char_ch1_freq[ 2] <= 176'h0000000000000000000000;
    char_ch1_freq[ 3] <= 176'h3EE70800FC000000000000;
    char_ch1_freq[ 4] <= 176'h4242380042000000000000;
    char_ch1_freq[ 5] <= 176'h4242080048000000000000;
    char_ch1_freq[ 6] <= 176'h8042081848000000007E00;
    char_ch1_freq[ 7] <= 176'h8042081878EE3C1A000000;
    char_ch1_freq[ 8] <= 176'h807E080048324226000000;
    char_ch1_freq[ 9] <= 176'h8042080048204242007E00;
    char_ch1_freq[10] <= 176'h8042080040207E42000000;
    char_ch1_freq[11] <= 176'h4242080040204042000000;
    char_ch1_freq[12] <= 176'h4442081840204226000000;
    char_ch1_freq[13] <= 176'h38E73E18E0F83C1A000000;
    char_ch1_freq[14] <= 176'h0000000000000002000000;
    char_ch1_freq[15] <= 176'h0000000000000007000000;
end

// CH2:Freq =
always @ (posedge clk_40m)begin
    char_ch2_freq[ 0] <= 176'h0000000000000000000000;
    char_ch2_freq[ 1] <= 176'h0000000000000000000000;
    char_ch2_freq[ 2] <= 176'h0000000000000000000000;
    char_ch2_freq[ 3] <= 176'h3EE73C00FC000000000000;
    char_ch2_freq[ 4] <= 176'h4242420042000000000000;
    char_ch2_freq[ 5] <= 176'h4242420048000000000000;
    char_ch2_freq[ 6] <= 176'h8042421848000000007E00;
    char_ch2_freq[ 7] <= 176'h8042021878EE3C1A000000;
    char_ch2_freq[ 8] <= 176'h807E040048324226000000;
    char_ch2_freq[ 9] <= 176'h8042080048204242007E00;
    char_ch2_freq[10] <= 176'h8042100040207E42000000;
    char_ch2_freq[11] <= 176'h4242200040204042000000;
    char_ch2_freq[12] <= 176'h4442421840204226000000;
    char_ch2_freq[13] <= 176'h38E77E18E0F83C1A000000;
    char_ch2_freq[14] <= 176'h0000000000000002000000;
    char_ch2_freq[15] <= 176'h0000000000000007000000;
end

// CH1:Vpp =
always @ (posedge clk_40m)begin
    char_ch1_vpp[ 0] <= 88'h0000000000000000000000;
    char_ch1_vpp[ 1] <= 88'h0000000000000000000000;
    char_ch1_vpp[ 2] <= 88'h0000000000000000000000;
    char_ch1_vpp[ 3] <= 88'h3EE70800E7000000000000;
    char_ch1_vpp[ 4] <= 88'h4242380042000000000000;
    char_ch1_vpp[ 5] <= 88'h4242080042000000000000;
    char_ch1_vpp[ 6] <= 88'h8042081844000000007E00;
    char_ch1_vpp[ 7] <= 88'h8042081824D8D800000000;
    char_ch1_vpp[ 8] <= 88'h807E080024646400000000;
    char_ch1_vpp[ 9] <= 88'h8042080028424200007E00;
    char_ch1_vpp[10] <= 88'h8042080028424200000000;
    char_ch1_vpp[11] <= 88'h4242080018424200000000;
    char_ch1_vpp[12] <= 88'h4442081810646400000000;
    char_ch1_vpp[13] <= 88'h38E73E1810585800000000;
    char_ch1_vpp[14] <= 88'h0000000000404000000000;
    char_ch1_vpp[15] <= 88'h0000000000E0E000000000;
end

// CH2:Vpp =
always @ (posedge clk_40m)begin
    char_ch2_vpp[ 0] <= 88'h0000000000000000000000;
    char_ch2_vpp[ 1] <= 88'h0000000000000000000000;
    char_ch2_vpp[ 2] <= 88'h0000000000000000000000;
    char_ch2_vpp[ 3] <= 88'h3EE73C00E7000000000000;
    char_ch2_vpp[ 4] <= 88'h4242420042000000000000;
    char_ch2_vpp[ 5] <= 88'h4242420042000000000000;
    char_ch2_vpp[ 6] <= 88'h8042421844000000007E00;
    char_ch2_vpp[ 7] <= 88'h8042021824D8D800000000;
    char_ch2_vpp[ 8] <= 88'h807E040024646400000000;
    char_ch2_vpp[ 9] <= 88'h8042080028424200007E00;
    char_ch2_vpp[10] <= 88'h8042100028424200000000;
    char_ch2_vpp[11] <= 88'h4242200018424200000000;
    char_ch2_vpp[12] <= 88'h4442421810646400000000;
    char_ch2_vpp[13] <= 88'h38E77E1810585800000000;
    char_ch2_vpp[14] <= 88'h0000000000404000000000;
    char_ch2_vpp[15] <= 88'h0000000000E0E000000000;
end


// CH1:Vmin =
always @ (posedge clk_40m)begin
    char_ch1_vmin[ 0] <= 88'h0000000000000000000000;
    char_ch1_vmin[ 1] <= 88'h0000000000000000000000;
    char_ch1_vmin[ 2] <= 88'h0000000000000000000000;
    char_ch1_vmin[ 3] <= 88'h3EE70800E7003000000000;
    char_ch1_vmin[ 4] <= 88'h4242380042003000000000;
    char_ch1_vmin[ 5] <= 88'h4242080042000000000000;
    char_ch1_vmin[ 6] <= 88'h8042081844000000007E00;
    char_ch1_vmin[ 7] <= 88'h8042081824FE70DC000000;
    char_ch1_vmin[ 8] <= 88'h807E080024491062000000;
    char_ch1_vmin[ 9] <= 88'h8042080028491042007E00;
    char_ch1_vmin[10] <= 88'h8042080028491042000000;
    char_ch1_vmin[11] <= 88'h4242080018491042000000;
    char_ch1_vmin[12] <= 88'h4442081810491042000000;
    char_ch1_vmin[13] <= 88'h38E73E1810ED7CE7000000;
    char_ch1_vmin[14] <= 88'h0000000000000000000000;
    char_ch1_vmin[15] <= 88'h0000000000000000000000;
end

// CH2:Vmin =
always @ (posedge clk_40m)begin
    char_ch2_vmin[ 0] <= 88'h0000000000000000000000;
    char_ch2_vmin[ 1] <= 88'h0000000000000000000000;
    char_ch2_vmin[ 2] <= 88'h0000000000000000000000;
    char_ch2_vmin[ 3] <= 88'h3EE73C00E7003000000000;
    char_ch2_vmin[ 4] <= 88'h4242420042003000000000;
    char_ch2_vmin[ 5] <= 88'h4242420042000000000000;
    char_ch2_vmin[ 6] <= 88'h8042421844000000007E00;
    char_ch2_vmin[ 7] <= 88'h8042021824FE70DC000000;
    char_ch2_vmin[ 8] <= 88'h807E040024491062000000;
    char_ch2_vmin[ 9] <= 88'h8042080028491042007E00;
    char_ch2_vmin[10] <= 88'h8042100028491042000000;
    char_ch2_vmin[11] <= 88'h4242200018491042000000;
    char_ch2_vmin[12] <= 88'h4442421810491042000000;
    char_ch2_vmin[13] <= 88'h38E77E1810ED7CE7000000;
    char_ch2_vmin[14] <= 88'h0000000000000000000000;
    char_ch2_vmin[15] <= 88'h0000000000000000000000;
end

// CH1:Vmax =
always @ (posedge clk_40m)begin
    char_ch1_vmax[ 0] <= 88'h0000000000000000000000;
    char_ch1_vmax[ 1] <= 88'h0000000000000000000000;
    char_ch1_vmax[ 2] <= 88'h0000000000000000000000;
    char_ch1_vmax[ 3] <= 88'h3EE70800E7000000000000;
    char_ch1_vmax[ 4] <= 88'h4242380042000000000000;
    char_ch1_vmax[ 5] <= 88'h4242080042000000000000;
    char_ch1_vmax[ 6] <= 88'h8042081844000000007E00;
    char_ch1_vmax[ 7] <= 88'h8042081824FE3876000000;
    char_ch1_vmax[ 8] <= 88'h807E080024494424000000;
    char_ch1_vmax[ 9] <= 88'h8042080028490C18007E00;
    char_ch1_vmax[10] <= 88'h8042080028493418000000;
    char_ch1_vmax[11] <= 88'h4242080018494418000000;
    char_ch1_vmax[12] <= 88'h4442081810494C24000000;
    char_ch1_vmax[13] <= 88'h38E73E1810ED366E000000;
    char_ch1_vmax[14] <= 88'h0000000000000000000000;
    char_ch1_vmax[15] <= 88'h0000000000000000000000;
end

// CH2:Vmax =
always @ (posedge clk_40m)begin
    char_ch2_vmax[ 0] <= 88'h0000000000000000000000;
    char_ch2_vmax[ 1] <= 88'h0000000000000000000000;
    char_ch2_vmax[ 2] <= 88'h0000000000000000000000;
    char_ch2_vmax[ 3] <= 88'h3EE73C00E7000000000000;
    char_ch2_vmax[ 4] <= 88'h4242420042000000000000;
    char_ch2_vmax[ 5] <= 88'h4242420042000000000000;
    char_ch2_vmax[ 6] <= 88'h8042421844000000007E00;
    char_ch2_vmax[ 7] <= 88'h8042021824FE3876000000;
    char_ch2_vmax[ 8] <= 88'h807E040024494424000000;
    char_ch2_vmax[ 9] <= 88'h8042080028490C18007E00;
    char_ch2_vmax[10] <= 88'h8042100028493418000000;
    char_ch2_vmax[11] <= 88'h4242200018494418000000;
    char_ch2_vmax[12] <= 88'h4442421810494C24000000;
    char_ch2_vmax[13] <= 88'h38E77E1810ED366E000000;
    char_ch2_vmax[14] <= 88'h0000000000000000000000;
    char_ch2_vmax[15] <= 88'h0000000000000000000000;
end

// CH1:mv/Vid 
always @ (posedge clk_40m)begin
    char_ch1_vid[ 0] <= 96'h000000000000000000000000;
    char_ch1_vid[ 1] <= 96'h000000020000000000004000;
    char_ch1_vid[ 2] <= 96'h000000040000020000002000;
    char_ch1_vid[ 3] <= 96'h3EE708080000040030001000;
    char_ch1_vid[ 4] <= 96'h424238080000040030061000;
    char_ch1_vid[ 5] <= 96'h424208100000040000020800;
    char_ch1_vid[ 6] <= 96'h804208100000080000020818;
    char_ch1_vid[ 7] <= 96'h80420810FEEE08EE703E0818;
    char_ch1_vid[ 8] <= 96'h807E08104944104410420800;
    char_ch1_vid[ 9] <= 96'h804208104944104410420800;
    char_ch1_vid[10] <= 96'h804208104928102810420800;
    char_ch1_vid[11] <= 96'h424208084928202810421000;
    char_ch1_vid[12] <= 96'h444208084910201010461018;
    char_ch1_vid[13] <= 96'h38E73E04ED1040107C3B2018;
    char_ch1_vid[14] <= 96'h000000020000400000004000;
    char_ch1_vid[15] <= 96'h000000000000000000000000;
end

// CH2:mv/Vid 
always @ (posedge clk_40m)begin
    char_ch2_vid[ 0] <= 96'h000000000000000000000000;
    char_ch2_vid[ 1] <= 96'h000000020000000000004000;
    char_ch2_vid[ 2] <= 96'h000000040000020000002000;
    char_ch2_vid[ 3] <= 96'h3EE73C080000040030001000;
    char_ch2_vid[ 4] <= 96'h424242080000040030061000;
    char_ch2_vid[ 5] <= 96'h424242100000040000020800;
    char_ch2_vid[ 6] <= 96'h804242100000080000020818;
    char_ch2_vid[ 7] <= 96'h80420210FEEE08EE703E0818;
    char_ch2_vid[ 8] <= 96'h807E04104944104410420800;
    char_ch2_vid[ 9] <= 96'h804208104944104410420800;
    char_ch2_vid[10] <= 96'h804210104928102810420800;
    char_ch2_vid[11] <= 96'h424220084928202810421000;
    char_ch2_vid[12] <= 96'h444242084910201010461018;
    char_ch2_vid[13] <= 96'h38E77E04ED1040107C3B2018;
    char_ch2_vid[14] <= 96'h000000020000400000004000;
    char_ch2_vid[15] <= 96'h000000000000000000000000;
end

// time_base: 
always @ (posedge clk_40m)begin
    char_tim[ 0] <= 64'h00000000;
    char_tim[ 1] <= 64'h00000002;
    char_tim[ 2] <= 64'h00000004;
    char_tim[ 3] <= 64'h00300008;
    char_tim[ 4] <= 64'h00300008;
    char_tim[ 5] <= 64'h10000010;
    char_tim[ 6] <= 64'h10000010;
    char_tim[ 7] <= 64'h7C70FE10;
    char_tim[ 8] <= 64'h10104910;
    char_tim[ 9] <= 64'h10104910;
    char_tim[10] <= 64'h10104910;
    char_tim[11] <= 64'h10104908;
    char_tim[12] <= 64'h12104908;
    char_tim[13] <= 64'h0C7CED04;
    char_tim[14] <= 64'h00000002;
    char_tim[15] <= 64'h00000000;
end

// s/vid): 
always @ (posedge clk_40m)begin
    char_ns[ 0] <= 48'h000000000000;
    char_ns[ 1] <= 48'h000000000040;
    char_ns[ 2] <= 48'h000200000020;
    char_ns[ 3] <= 48'h000400300010;
    char_ns[ 4] <= 48'h000400300610;
    char_ns[ 5] <= 48'h000400000208;
    char_ns[ 6] <= 48'h000800000208;
    char_ns[ 7] <= 48'h3E08EE703E08;
    char_ns[ 8] <= 48'h421044104208;
    char_ns[ 9] <= 48'h401044104208;
    char_ns[10] <= 48'h3C1028104208;
    char_ns[11] <= 48'h022028104210;
    char_ns[12] <= 48'h422010104610;
    char_ns[13] <= 48'h7C40107C3B20;
    char_ns[14] <= 48'h004000000040;
    char_ns[15] <= 48'h000000000000;
end

// probe1 
always @ (posedge clk_40m)begin
    char_probe1[ 0] <= 80'h000000000000000000;
    char_probe1[ 1] <= 80'h000000000000000000;
    char_probe1[ 2] <= 80'h000000000000000000;
    char_probe1[ 3] <= 80'h000000000008000008;
    char_probe1[ 4] <= 80'h000000C00038000038;
    char_probe1[ 5] <= 80'h000000400008000008;
    char_probe1[ 6] <= 80'h000000400008180008;
    char_probe1[ 7] <= 80'hD8EE3C583C08187608;
    char_probe1[ 8] <= 80'h643242644208002408;
    char_probe1[ 9] <= 80'h422042424208001808;
    char_probe1[10] <= 80'h422042427E08001808;
    char_probe1[11] <= 80'h422042424008001808;
    char_probe1[12] <= 80'h642042644208182408;
    char_probe1[13] <= 80'h58F83C583C3E186E3E;
    char_probe1[14] <= 80'h400000000000000000;
    char_probe1[15] <= 80'hE00000000000000000;
end

// probe2 
always @ (posedge clk_40m)begin
    char_probe2[ 0] <= 80'h000000000000000000;
    char_probe2[ 1] <= 80'h000000000000000000;
    char_probe2[ 2] <= 80'h000000000000000000;
    char_probe2[ 3] <= 80'h00000000003C000008;
    char_probe2[ 4] <= 80'h000000C00042000038;
    char_probe2[ 5] <= 80'h000000400042000008;
    char_probe2[ 6] <= 80'h000000400042180008;
    char_probe2[ 7] <= 80'hD8EE3C583C02187608;
    char_probe2[ 8] <= 80'h643242644204002408;
    char_probe2[ 9] <= 80'h422042424208001808;
    char_probe2[10] <= 80'h422042427E10001808;
    char_probe2[11] <= 80'h422042424020001808;
    char_probe2[12] <= 80'h642042644242182408;
    char_probe2[13] <= 80'h58F83C583C7E186E3E;
    char_probe2[14] <= 80'h400000000000000000;
    char_probe2[15] <= 80'hE00000000000000000;
end

// coupling1
always @ (posedge clk_40m)begin
    char_coupling1[ 0] <= 72'h000000000000000000;
    char_coupling1[ 1] <= 72'h000000000000000000;
    char_coupling1[ 2] <= 72'h000000000000000000;
    char_coupling1[ 3] <= 72'h000000001030000000;
    char_coupling1[ 4] <= 72'h000000007030000000;
    char_coupling1[ 5] <= 72'h000000001000000000;
    char_coupling1[ 6] <= 72'h000000001000000018;
    char_coupling1[ 7] <= 72'h1C3CC6D81070DC3E18;
    char_coupling1[ 8] <= 72'h224242641010624400;
    char_coupling1[ 9] <= 72'h404242421010424400;
    char_coupling1[10] <= 72'h404242421010423800;
    char_coupling1[11] <= 72'h404242421010424000;
    char_coupling1[12] <= 72'h224246641010423C18;
    char_coupling1[13] <= 72'h1C3C3B587C7CE74218;
    char_coupling1[14] <= 72'h000000400000004200;
    char_coupling1[15] <= 72'h000000E00000003C00;
end

// coupling2
always @ (posedge clk_40m)begin
    char_coupling2[ 0] <= 72'h000000000000000000;
    char_coupling2[ 1] <= 72'h000000000000000000;
    char_coupling2[ 2] <= 72'h000000000000000000;
    char_coupling2[ 3] <= 72'h000000001030000000;
    char_coupling2[ 4] <= 72'h000000007030000000;
    char_coupling2[ 5] <= 72'h000000001000000000;
    char_coupling2[ 6] <= 72'h000000001000000018;
    char_coupling2[ 7] <= 72'h1C3CC6D81070DC3E18;
    char_coupling2[ 8] <= 72'h224242641010624400;
    char_coupling2[ 9] <= 72'h404242421010424400;
    char_coupling2[10] <= 72'h404242421010423800;
    char_coupling2[11] <= 72'h404242421010424000;
    char_coupling2[12] <= 72'h224246641010423C18;
    char_coupling2[13] <= 72'h1C3C3B587C7CE74218;
    char_coupling2[14] <= 72'h000000400000004200;
    char_coupling2[15] <= 72'h000000E00000003C00;
end

// num 0
always @ (posedge clk_40m)begin
    char_0[ 0] <= 8'h00;
    char_0[ 1] <= 8'h00;
    char_0[ 2] <= 8'h00;
    char_0[ 3] <= 8'h18;
    char_0[ 4] <= 8'h24;
    char_0[ 5] <= 8'h42;
    char_0[ 6] <= 8'h42;
    char_0[ 7] <= 8'h42;
    char_0[ 8] <= 8'h42;
    char_0[ 9] <= 8'h42;
    char_0[10] <= 8'h42;
    char_0[11] <= 8'h42;
    char_0[12] <= 8'h24;
    char_0[13] <= 8'h18;
    char_0[14] <= 8'h00;
    char_0[15] <= 8'h00;
end

// num 1
always @ (posedge clk_40m)begin
    char_1[ 0] <= 8'h00;
    char_1[ 1] <= 8'h00;
    char_1[ 2] <= 8'h00;
    char_1[ 3] <= 8'h08;
    char_1[ 4] <= 8'h38;
    char_1[ 5] <= 8'h08;
    char_1[ 6] <= 8'h08;
    char_1[ 7] <= 8'h08;
    char_1[ 8] <= 8'h08;
    char_1[ 9] <= 8'h08;
    char_1[10] <= 8'h08;
    char_1[11] <= 8'h08;
    char_1[12] <= 8'h08;
    char_1[13] <= 8'h3E;
    char_1[14] <= 8'h00;
    char_1[15] <= 8'h00;
end

// num 2
always @ (posedge clk_40m)begin
    char_2[ 0] <= 8'h00;
    char_2[ 1] <= 8'h00;
    char_2[ 2] <= 8'h00;
    char_2[ 3] <= 8'h3C;
    char_2[ 4] <= 8'h42;
    char_2[ 5] <= 8'h42;
    char_2[ 6] <= 8'h42;
    char_2[ 7] <= 8'h02;
    char_2[ 8] <= 8'h04;
    char_2[ 9] <= 8'h08;
    char_2[10] <= 8'h10;
    char_2[11] <= 8'h20;
    char_2[12] <= 8'h42;
    char_2[13] <= 8'h7E;
    char_2[14] <= 8'h00;
    char_2[15] <= 8'h00;
end

// num 3
always @ (posedge clk_40m)begin
    char_3[ 0] <= 8'h00;
    char_3[ 1] <= 8'h00;
    char_3[ 2] <= 8'h00;
    char_3[ 3] <= 8'h3C;
    char_3[ 4] <= 8'h42;
    char_3[ 5] <= 8'h42;
    char_3[ 6] <= 8'h02;
    char_3[ 7] <= 8'h04;
    char_3[ 8] <= 8'h18;
    char_3[ 9] <= 8'h04;
    char_3[10] <= 8'h02;
    char_3[11] <= 8'h42;
    char_3[12] <= 8'h42;
    char_3[13] <= 8'h3C;
    char_3[14] <= 8'h00;
    char_3[15] <= 8'h00;
end

// num 4
always @ (posedge clk_40m)begin
    char_4[ 0] <= 8'h00;
    char_4[ 1] <= 8'h00;
    char_4[ 2] <= 8'h00;
    char_4[ 3] <= 8'h04;
    char_4[ 4] <= 8'h0C;
    char_4[ 5] <= 8'h0C;
    char_4[ 6] <= 8'h14;
    char_4[ 7] <= 8'h24;
    char_4[ 8] <= 8'h24;
    char_4[ 9] <= 8'h44;
    char_4[10] <= 8'h7F;
    char_4[11] <= 8'h04;
    char_4[12] <= 8'h04;
    char_4[13] <= 8'h1F;
    char_4[14] <= 8'h00;
    char_4[15] <= 8'h00;
end

// num 5
always @ (posedge clk_40m)begin
    char_5[ 0] <= 8'h00;
    char_5[ 1] <= 8'h00;
    char_5[ 2] <= 8'h00;
    char_5[ 3] <= 8'h7E;
    char_5[ 4] <= 8'h40;
    char_5[ 5] <= 8'h40;
    char_5[ 6] <= 8'h40;
    char_5[ 7] <= 8'h78;
    char_5[ 8] <= 8'h44;
    char_5[ 9] <= 8'h02;
    char_5[10] <= 8'h02;
    char_5[11] <= 8'h42;
    char_5[12] <= 8'h44;
    char_5[13] <= 8'h38;
    char_5[14] <= 8'h00;
    char_5[15] <= 8'h00;
end

// num 6
always @ (posedge clk_40m)begin
    char_6[ 0] <= 8'h00;
    char_6[ 1] <= 8'h00;
    char_6[ 2] <= 8'h00;
    char_6[ 3] <= 8'h18;
    char_6[ 4] <= 8'h24;
    char_6[ 5] <= 8'h40;
    char_6[ 6] <= 8'h40;
    char_6[ 7] <= 8'h5C;
    char_6[ 8] <= 8'h62;
    char_6[ 9] <= 8'h42;
    char_6[10] <= 8'h42;
    char_6[11] <= 8'h42;
    char_6[12] <= 8'h22;
    char_6[13] <= 8'h1C;
    char_6[14] <= 8'h00;
    char_6[15] <= 8'h00;
end

// num 7
always @ (posedge clk_40m)begin
    char_7[ 0] <= 8'h00;
    char_7[ 1] <= 8'h00;
    char_7[ 2] <= 8'h00;
    char_7[ 3] <= 8'h7E;
    char_7[ 4] <= 8'h42;
    char_7[ 5] <= 8'h04;
    char_7[ 6] <= 8'h04;
    char_7[ 7] <= 8'h08;
    char_7[ 8] <= 8'h08;
    char_7[ 9] <= 8'h10;
    char_7[10] <= 8'h10;
    char_7[11] <= 8'h10;
    char_7[12] <= 8'h10;
    char_7[13] <= 8'h10;
    char_7[14] <= 8'h00;
    char_7[15] <= 8'h00;
end

// num 8
always @ (posedge clk_40m)begin
    char_8[ 0] <= 8'h00;
    char_8[ 1] <= 8'h00;
    char_8[ 2] <= 8'h00;
    char_8[ 3] <= 8'h3C;
    char_8[ 4] <= 8'h42;
    char_8[ 5] <= 8'h42;
    char_8[ 6] <= 8'h42;
    char_8[ 7] <= 8'h24;
    char_8[ 8] <= 8'h18;
    char_8[ 9] <= 8'h24;
    char_8[10] <= 8'h42;
    char_8[11] <= 8'h42;
    char_8[12] <= 8'h42;
    char_8[13] <= 8'h3C;
    char_8[14] <= 8'h00;
    char_8[15] <= 8'h00;
end

// num 9
always @ (posedge clk_40m)begin
    char_9[ 0] <= 8'h00;
    char_9[ 1] <= 8'h00;
    char_9[ 2] <= 8'h00;
    char_9[ 3] <= 8'h38;
    char_9[ 4] <= 8'h44;
    char_9[ 5] <= 8'h42;
    char_9[ 6] <= 8'h42;
    char_9[ 7] <= 8'h42;
    char_9[ 8] <= 8'h46;
    char_9[ 9] <= 8'h3A;
    char_9[10] <= 8'h02;
    char_9[11] <= 8'h02;
    char_9[12] <= 8'h24;
    char_9[13] <= 8'h18;
    char_9[14] <= 8'h00;
    char_9[15] <= 8'h00;
end

// mv
always @ (posedge clk_40m)begin
    char_mv[ 0] <= 16'h0000;
    char_mv[ 1] <= 16'h0000;
    char_mv[ 2] <= 16'h0000;
    char_mv[ 3] <= 16'h0000;
    char_mv[ 4] <= 16'h0000;
    char_mv[ 5] <= 16'h0000;
    char_mv[ 6] <= 16'h0000;
    char_mv[ 7] <= 16'hFEEE;
    char_mv[ 8] <= 16'h4944;
    char_mv[ 9] <= 16'h4944;
    char_mv[10] <= 16'h4928;
    char_mv[11] <= 16'h4928;
    char_mv[12] <= 16'h4910;
    char_mv[13] <= 16'hED10;
    char_mv[14] <= 16'h0000;
    char_mv[15] <= 16'h0000;
end

// m
always @ (posedge clk_40m)begin
    char_m[ 0] <= 8'h00;
    char_m[ 1] <= 8'h00;
    char_m[ 2] <= 8'h00;
    char_m[ 3] <= 8'hEE;
    char_m[ 4] <= 8'h6C;
    char_m[ 5] <= 8'h6C;
    char_m[ 6] <= 8'h6C;
    char_m[ 7] <= 8'h6C;
    char_m[ 8] <= 8'h6C;
    char_m[ 9] <= 8'h54;
    char_m[10] <= 8'h54;
    char_m[11] <= 8'h54;
    char_m[12] <= 8'h54;
    char_m[13] <= 8'hD6;
    char_m[14] <= 8'h00;
    char_m[15] <= 8'h00;
end

// k
always @ (posedge clk_40m)begin
    char_k[ 0] <= 8'h00;
    char_k[ 1] <= 8'h00;
    char_k[ 2] <= 8'h00;
    char_k[ 3] <= 8'hEE;
    char_k[ 4] <= 8'h44;
    char_k[ 5] <= 8'h48;
    char_k[ 6] <= 8'h50;
    char_k[ 7] <= 8'h70;
    char_k[ 8] <= 8'h50;
    char_k[ 9] <= 8'h48;
    char_k[10] <= 8'h48;
    char_k[11] <= 8'h44;
    char_k[12] <= 8'h44;
    char_k[13] <= 8'hEE;
    char_k[14] <= 8'h00;
    char_k[15] <= 8'h00;
end

// point
always @ (posedge clk_40m)begin
    char_point[ 0] <= 8'h00;
    char_point[ 1] <= 8'h00;
    char_point[ 2] <= 8'h00;
    char_point[ 3] <= 8'h00;
    char_point[ 4] <= 8'h00;
    char_point[ 5] <= 8'h00;
    char_point[ 6] <= 8'h00;
    char_point[ 7] <= 8'h00;
    char_point[ 8] <= 8'h00;
    char_point[ 9] <= 8'h00;
    char_point[10] <= 8'h00;
    char_point[11] <= 8'h00;
    char_point[12] <= 8'h60;
    char_point[13] <= 8'h60;
    char_point[14] <= 8'h00;
    char_point[15] <= 8'h00;
end

// null
always @ (posedge clk_40m)begin
    char_null[ 0] <= 8'h00;
    char_null[ 1] <= 8'h00;
    char_null[ 2] <= 8'h00;
    char_null[ 3] <= 8'h00;
    char_null[ 4] <= 8'h00;
    char_null[ 5] <= 8'h00;
    char_null[ 6] <= 8'h00;
    char_null[ 7] <= 8'h00;
    char_null[ 8] <= 8'h00;
    char_null[ 9] <= 8'h00;
    char_null[10] <= 8'h00;
    char_null[11] <= 8'h00;
    char_null[12] <= 8'h00;
    char_null[13] <= 8'h00;
    char_null[14] <= 8'h00;
    char_null[15] <= 8'h00;
end

// hz
always @ (posedge clk_40m)begin
    char_hz[ 0] <= 16'h0000;
    char_hz[ 1] <= 16'h0000;
    char_hz[ 2] <= 16'h0000;
    char_hz[ 3] <= 16'hE700;
    char_hz[ 4] <= 16'h4200;
    char_hz[ 5] <= 16'h4200;
    char_hz[ 6] <= 16'h4200;
    char_hz[ 7] <= 16'h427E;
    char_hz[ 8] <= 16'h7E44;
    char_hz[ 9] <= 16'h4208;
    char_hz[10] <= 16'h4210;
    char_hz[11] <= 16'h4210;
    char_hz[12] <= 16'h4222;
    char_hz[13] <= 16'hE77E;
    char_hz[14] <= 16'h0000;
    char_hz[15] <= 16'h0000;
end

// ac
always @ (posedge clk_40m)begin
    char_ac[ 0] <= 16'h0000;
    char_ac[ 1] <= 16'h0000;
    char_ac[ 2] <= 16'h0000;
    char_ac[ 3] <= 16'h103E;
    char_ac[ 4] <= 16'h1042;
    char_ac[ 5] <= 16'h1842;
    char_ac[ 6] <= 16'h2880;
    char_ac[ 7] <= 16'h2880;
    char_ac[ 8] <= 16'h2480;
    char_ac[ 9] <= 16'h3C80;
    char_ac[10] <= 16'h4480;
    char_ac[11] <= 16'h4242;
    char_ac[12] <= 16'h4244;
    char_ac[13] <= 16'hE738;
    char_ac[14] <= 16'h0000;
    char_ac[15] <= 16'h0000;
end

// 1000
always @ (posedge clk_40m)begin
    char_1000[ 0] <= 32'h00000000;
    char_1000[ 1] <= 32'h00000000;
    char_1000[ 2] <= 32'h00000000;
    char_1000[ 3] <= 32'h08181818;
    char_1000[ 4] <= 32'h38242424;
    char_1000[ 5] <= 32'h08424242;
    char_1000[ 6] <= 32'h08424242;
    char_1000[ 7] <= 32'h08424242;
    char_1000[ 8] <= 32'h08424242;
    char_1000[ 9] <= 32'h08424242;
    char_1000[10] <= 32'h08424242;
    char_1000[11] <= 32'h08424242;
    char_1000[12] <= 32'h08242424;
    char_1000[13] <= 32'h3E181818;
    char_1000[14] <= 32'h00000000;
    char_1000[15] <= 32'h00000000;
end

endmodule




