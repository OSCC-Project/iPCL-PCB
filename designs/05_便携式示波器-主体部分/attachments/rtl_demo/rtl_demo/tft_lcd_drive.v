/************************************************************************************
Author      : Alpha
Email       : 1879136899@qq.com

*************************************************************************************

module name : VGA_drive
Time        : 2020.
Version     : V1.0
Chip        : EP4CE75F17C8L
OEM         : Altera(intel)
Describe    : 分辨率：800x480
              pixel clock : 40MHz
              

    *********************************************************************************/

module tft_lcd_drive(
    //system signals,
    input   wire            clk_40m         ,
    input   wire            rst_n           ,
    //user signals
    output  wire [12:0]     column          ,
    output  wire [12:0]     row             ,
    output  wire            vga_vsync       ,
    output  wire            vga_hsync       ,
    output  wire            vga_en          
    );

    //----------------------- 参数定义 ---------------------------
    parameter   H_TOTAL       = 11'd1056  ;
    parameter   H_DATA        = 10'd800   ;
    parameter   H_SYNC        = 8'd128    ;
    parameter   H_BACK        = 7'd88     ;

    parameter   V_TOTAL       = 10'd525   ;
    parameter   V_DATA        = 10'd480   ;
    parameter   V_SYNC        = 3'd2      ;
    parameter   V_BACK        = 5'd25     ;

    //----------------------- 逻辑功能 ---------------------------

    //时序逻辑，行信号计数器：
    reg [10:0] cnt_h;      

    always @ (posedge clk_40m or negedge rst_n)begin
        if(rst_n == 1'b0)
            cnt_h <= 1'b0;
        else if(cnt_h == H_TOTAL)
            cnt_h <= 1'b0;
        else
            cnt_h <= cnt_h + 1'b1;
    end

    //时序逻辑，场信号计数器：
    reg [9:0] cnt_v;

    always @ (posedge clk_40m or negedge rst_n)begin
        if(rst_n == 1'b0)
            cnt_v <= 1'b0;
        else if((cnt_v == V_TOTAL) && (cnt_h == H_TOTAL))
            cnt_v <= 1'b0;
        else if(cnt_h == H_TOTAL)
            cnt_v <= cnt_v + 1'b1;
    end

    //组合逻辑,实际显示区域使能
    assign vga_en = ((cnt_h > (H_SYNC + H_BACK) && cnt_h <= (H_SYNC + H_BACK + H_DATA)) 
    &&               (cnt_v > (V_SYNC + V_BACK) && cnt_v <= (V_SYNC + V_BACK + V_DATA))) ? 1'b1 : 1'b0;

    //组合逻辑
    assign  vga_hsync = (cnt_h > H_SYNC) ? 1'b1:1'b0;
    assign  vga_vsync = (cnt_v > V_SYNC) ? 1'b1:1'b0;

    // wire    [10:0]  column  ;
    // wire    [10:0]  row     ;

    assign  column = vga_en ? cnt_h - H_SYNC - H_BACK : 1'b0;
    assign  row    = vga_en ? cnt_v - V_SYNC - V_BACK : 1'b0;

    endmodule 
