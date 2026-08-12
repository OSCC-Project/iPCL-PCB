//==========================================================================
//  Author      ：alpha
//  Mail        ：1879136899@qq.com
//  Filename    ：
//  Created Time：
//  End Time    ：
//  Description ：
//==========================================================================
`timescale 1ns/100ps

module tx_data_transform(
    //system signal
    input  wire     [ 0:0]      sys_clk_50m         ,
    input  wire     [ 0:0]      sys_rst_n           ,
    //user signal
    input  wire     [26:0]      data_a              ,
    input  wire     [26:0]      data_b              ,
    input  wire     [26:0]      data_c              ,
    input  wire     [26:0]      data_d              ,
    input  wire     [26:0]      data_e              ,
    input  wire     [26:0]      data_f              ,
    input  wire     [26:0]      data_g              ,
    input  wire     [ 0:0]      uart_send_finish    ,
    output reg      [ 0:0]      uart_send_en        ,
    output reg      [ 7:0]      send_data  
    );


    //--------------------------------------------------------------------
    //------------------------------参数定义------------------------------
    //--------------------------------------------------------------------
 
    parameter       SEND_IDLE       =       4'd0        ;
    parameter       SEND_BIT0       =       4'd1        ;
    parameter       SEND_BIT1       =       4'd2        ;
    parameter       SEND_BIT2       =       4'd3        ;
    parameter       SEND_BIT3       =       4'd4        ;
    parameter       SEND_BIT4       =       4'd5        ;
    parameter       SEND_BIT5       =       4'd6        ;
    parameter       SEND_BIT6       =       4'd7        ;
    parameter       SEND_BIT7       =       4'd8        ;
    parameter       SEND_BIT8       =       4'd9        ;
    parameter       SEND_END        =       4'd10       ;

    parameter       UART_DATA_A     =       8'd97   	;
    parameter       UART_DATA_B     =       8'd98   	;
    parameter       UART_DATA_C     =       8'd99   	;
    parameter       UART_DATA_D     =       8'd100   	;
    parameter       UART_DATA_E     =       8'd101   	;
    parameter       UART_DATA_F     =       8'd102   	;
    parameter       UART_DATA_G     =       8'd103  	;    
    parameter       UART_LINE_FEED  =       8'd10   	; 


    parameter       IDLE            =       4'd0        ;
    parameter       DATA_A          =       4'd1        ;
    parameter       DATA_B          =       4'd2        ;
    parameter       DATA_C          =       4'd3        ;
    parameter       DATA_D          =       4'd4        ;
    parameter       DATA_E          =       4'd5        ;
    parameter       DATA_F          =       4'd6        ;
    parameter       DATA_G          =       4'd7        ;
    parameter       WAIT            =       4'd8        ;


    parameter       WAIT_CNT_MAX    =       26'd2_500_000	;


    //--------------------------------------------------------------------
    //------------------------------逻辑功能------------------------------
    //--------------------------------------------------------------------

    //==================================================================//
    //===========================四段式状态机===========================//
    //==================================================================//
    //=========================先后发送功能数据=========================//
    //==================================================================//

    //计数器：
    wire                    add_wait_cnt        ;
    wire                    end_wait_cnt        ;
    reg     [ 26 - 1: 0]    wait_cnt            ;

    always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
        if(sys_rst_n == 1'b0)begin
            wait_cnt <= 0;
        end
        else if(add_wait_cnt)begin
            if(end_wait_cnt)
                wait_cnt <= 0;
            else
                wait_cnt <= wait_cnt + 1;
        end
    end

    assign add_wait_cnt = (data_type_state_c == WAIT) || (data_type_state_c == IDLE);       
    assign end_wait_cnt = add_wait_cnt && wait_cnt == WAIT_CNT_MAX - 1'b1;   
    
    //串口发送使能：
    always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
        if(sys_rst_n == 1'b0)begin
            uart_send_en <= 1'b0;
        end
        else if(wait_cnt == 1'b0)begin
            uart_send_en <= 1'b1;
        end
        else 
            uart_send_en <= 1'b0;
    end


    //第一段：同步时序always模块，格式化描述次态寄存器迁移到现态寄存器(不需更改）
    reg     [ 2:0]  data_type_state_c     ;
    reg     [ 2:0]  data_type_state_n     ;

    always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
        if(sys_rst_n == 1'b0)begin
            data_type_state_c <= IDLE;
        end
        else begin
            data_type_state_c <= data_type_state_n;
        end
    end

    //第二段：组合逻辑always模块，描述状态转移条件判断
    always @ (*)begin
        case(data_type_state_c)
            IDLE:begin
                if(idle_a_start)begin
                    data_type_state_n = DATA_A;
                end
                else begin
                    data_type_state_n = data_type_state_c;
                end
            end
            DATA_A:begin
                if(a_b_start)begin
                    data_type_state_n = DATA_B;
                end
                else begin
                    data_type_state_n = data_type_state_c;
                end
            end
            DATA_B:begin
                if(b_c_start)begin
                    data_type_state_n = DATA_C;
                end
                else begin
                    data_type_state_n = data_type_state_c;
                end
            end
            DATA_C:begin
                if(c_d_start)begin
                    data_type_state_n = DATA_D;
                end
                else begin
                    data_type_state_n = data_type_state_c;
                end
            end
            DATA_D:begin
                if(d_e_start)begin
                    data_type_state_n = DATA_E;
                end
                else begin
                    data_type_state_n = data_type_state_c;
                end
            end
            DATA_E:begin
                if(e_f_start)begin
                    data_type_state_n = DATA_F;
                end
                else begin
                    data_type_state_n = data_type_state_c;
                end
            end
            DATA_F:begin
                if(f_g_start)begin
                    data_type_state_n = DATA_G;
                end
                else begin
                    data_type_state_n = data_type_state_c;
                end
            end
            DATA_G:begin
                if(g_wait_start)begin
                    data_type_state_n = WAIT;
                end
                else begin
                    data_type_state_n = data_type_state_c;
                end
            end
           
            WAIT:begin
                if(wait_idle_start)begin
                    data_type_state_n = IDLE;
                end
                else begin
                    data_type_state_n = data_type_state_c;
                end
            end
            default:begin
                data_type_state_n = IDLE;
            end
        endcase
    end

    //第三段：设计转移条件
    wire        [ 0:0]      idle_a_start        ;
    wire        [ 0:0]      a_b_start           ;
    wire        [ 0:0]      b_c_start           ;
    wire        [ 0:0]      c_d_start           ;
    wire        [ 0:0]      d_e_start           ;
    wire        [ 0:0]      e_f_start           ;
    wire        [ 0:0]      f_g_start           ;
    wire        [ 0:0]      g_wait_start        ;
    wire        [ 0:0]      wait_idle_start     ;

    assign idle_a_start     = data_type_state_c == IDLE     && wait_cnt == WAIT_CNT_MAX - 1'b1;
    assign a_b_start        = data_type_state_c == DATA_A   && bit8_idle_start == 1'b1;
    assign b_c_start        = data_type_state_c == DATA_B   && bit8_idle_start == 1'b1;
    assign c_d_start        = data_type_state_c == DATA_C   && bit8_idle_start == 1'b1;
    assign d_e_start        = data_type_state_c == DATA_D   && bit8_idle_start == 1'b1;
    assign e_f_start        = data_type_state_c == DATA_E   && bit8_idle_start == 1'b1;
    assign f_g_start        = data_type_state_c == DATA_F   && bit8_idle_start == 1'b1;
    assign g_wait_start     = data_type_state_c == DATA_G   && idle_bit0_start == 1'b1;
    assign wait_idle_start  = data_type_state_c == WAIT     && wait_cnt == WAIT_CNT_MAX - 1'b1;

    //第四段：同步时序always模块，格式化描述寄存器输出（可有多个输出）
    reg     [ 26:0]     wait_transform_data     ;                     

    always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
        if(sys_rst_n == 1'b0)begin
            wait_transform_data <=27'b0;      //初始化
        end
        else if(data_type_state_c == IDLE)begin
            wait_transform_data <= 27'b0;
        end
        else if(data_type_state_c == DATA_A)begin
            wait_transform_data <= data_a;
        end
        else if(data_type_state_c == DATA_B)begin
            wait_transform_data <= data_b;
        end
        else if(data_type_state_c == DATA_C)begin
            wait_transform_data <= data_c;
        end
        else if(data_type_state_c == DATA_D)begin
            wait_transform_data <= data_d;
        end
        else if(data_type_state_c == DATA_E)begin
            wait_transform_data <= data_e;
        end
        else if(data_type_state_c == DATA_F)begin
            wait_transform_data <= data_f;
        end
        else if(data_type_state_c == DATA_G)begin
            wait_transform_data <= data_g;
        end
        // else if(data_type_state_c == WAIT)begin
        //     wait_transform_data <= 27'd0;
        // end
        else begin
            wait_transform_data <= 27'd0;
        end
    end


    //==================================================================//
    //=========================== 状态机结束 ============================//
    //==================================================================//

    // 对待转换的数据进行寄存：
    reg     [ 26:0]     reg_wait_transform_data     ;                      

    always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
        if(sys_rst_n == 1'b0)begin
            reg_wait_transform_data <= 1'b0;
        end
        else if(uart_send_finish == 1'b1)begin
            reg_wait_transform_data <= wait_transform_data;
        end
    end


    // 取待发送数据的各个位：
    wire        [ 7:0]      uart_send_data_0        ;
    wire        [ 7:0]      uart_send_data_1        ;
    wire        [ 7:0]      uart_send_data_2        ;
    wire        [ 7:0]      uart_send_data_3        ;
    wire        [ 7:0]      uart_send_data_4        ;
    wire        [ 7:0]      uart_send_data_5        ;
    wire        [ 7:0]      uart_send_data_6        ;
    wire        [ 7:0]      uart_send_data_7        ;
    wire        [ 7:0]      uart_send_data_8        ;

    assign uart_send_data_0 = (reg_wait_transform_data / 390625) >> 8;           //取freq_calc_result的亿位
    assign uart_send_data_1 = ((reg_wait_transform_data / 78125) >> 7) % 10;     //取freq_calc_result的千万位
    assign uart_send_data_2 = ((reg_wait_transform_data / 15625) >> 6) % 10;     //取freq_calc_result的百万位
    assign uart_send_data_3 = ((reg_wait_transform_data / 3125) >> 5) % 10;      //取freq_calc_result的十万位
    assign uart_send_data_4 = ((reg_wait_transform_data / 625) >> 4) % 10;       //取freq_calc_result的万位
    assign uart_send_data_5 = ((reg_wait_transform_data / 125) >> 3) % 10;       //取freq_calc_result的千位
    assign uart_send_data_6 = ((reg_wait_transform_data / 25) >> 2) % 10;        //取freq_calc_result的百位
    assign uart_send_data_7 = ((reg_wait_transform_data / 5) >> 1) % 10;         //取freq_calc_result的十位
    assign uart_send_data_8 = reg_wait_transform_data % 10;                      //取freq_calc_result的个位


    //==================================================================//
    //===========================四段式状态机===========================//
    //==================================================================//
    //=========================先后发送数据各位=========================//
    //==================================================================//

    //第一段：同步时序always模块，格式化描述次态寄存器迁移到现态寄存器(不需更改）
    reg     [ 3:0]      data_bit_state_c     ;                      
    reg     [ 3:0]      data_bit_state_n     ;     

    always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
        if(sys_rst_n == 1'b0)begin
            data_bit_state_c <= IDLE;
        end
        else begin
            data_bit_state_c <= data_bit_state_n;
        end
    end

    //第二段：组合逻辑always模块，描述状态转移条件判断
    always @ (*)begin
        case(data_bit_state_c)
            SEND_IDLE:begin
                if(idle_bit0_start)begin
                    data_bit_state_n = SEND_BIT0;
                end
                else begin
                    data_bit_state_n = data_bit_state_c;
                end
            end
            SEND_BIT0:begin
                if(bit0_bit1_start)begin
                    data_bit_state_n = SEND_BIT1;
                end
                else begin
                    data_bit_state_n = data_bit_state_c;
                end
            end
            SEND_BIT1:begin
                if(bit1_bit2_start)begin
                    data_bit_state_n = SEND_BIT2;
                end
                else begin
                    data_bit_state_n = data_bit_state_c;
                end
            end
            SEND_BIT2:begin
                if(bit2_bit3_start)begin
                    data_bit_state_n = SEND_BIT3;
                end
                else begin
                    data_bit_state_n = data_bit_state_c;
                end
            end
            SEND_BIT3:begin
                if(bit3_bit4_start)begin
                    data_bit_state_n = SEND_BIT4;
                end
                else begin
                    data_bit_state_n = data_bit_state_c;
                end
            end
            SEND_BIT4:begin
                if(bit4_bit5_start)begin
                    data_bit_state_n = SEND_BIT5;
                end
                else begin
                    data_bit_state_n = data_bit_state_c;
                end
            end
            SEND_BIT5:begin
                if(bit5_bit6_start)begin
                    data_bit_state_n = SEND_BIT6;
                end
                else begin
                    data_bit_state_n = data_bit_state_c;
                end
            end
            SEND_BIT6:begin
                if(bit6_bit7_start)begin
                    data_bit_state_n = SEND_BIT7;
                end
                else begin
                    data_bit_state_n = data_bit_state_c;
                end
            end
            SEND_BIT7:begin
                if(bit7_bit8_start)begin
                    data_bit_state_n = SEND_BIT8;
                end
                else begin
                    data_bit_state_n = data_bit_state_c;
                end
            end
            SEND_BIT8:begin
                if(bit8_idle_start)begin
                    data_bit_state_n = SEND_IDLE;
                end
                else begin
                    data_bit_state_n = data_bit_state_c;
                end
            end            
            default:begin
                data_bit_state_n = SEND_IDLE;
            end
        endcase
    end

    //第三段：设计转移条件
    wire        [ 0:0]      idle_bit0_start      ;     
    wire        [ 0:0]      bit0_bit1_start      ;     
    wire        [ 0:0]      bit1_bit2_start      ;     
    wire        [ 0:0]      bit2_bit3_start      ;     
    wire        [ 0:0]      bit3_bit4_start      ;     
    wire        [ 0:0]      bit4_bit5_start      ;     
    wire        [ 0:0]      bit5_bit6_start      ;     
    wire        [ 0:0]      bit6_bit7_start      ;     
    wire        [ 0:0]      bit7_bit8_start      ;     
    wire        [ 0:0]      bit8_idle_start      ;       

    assign idle_bit0_start  = data_bit_state_c   ==  SEND_IDLE   && uart_send_finish == 1'b1;
    assign bit0_bit1_start  = data_bit_state_c   ==  SEND_BIT0   && uart_send_finish == 1'b1;
    assign bit1_bit2_start  = data_bit_state_c   ==  SEND_BIT1   && uart_send_finish == 1'b1;
    assign bit2_bit3_start  = data_bit_state_c   ==  SEND_BIT2   && uart_send_finish == 1'b1;
    assign bit3_bit4_start  = data_bit_state_c   ==  SEND_BIT3   && uart_send_finish == 1'b1;
    assign bit4_bit5_start  = data_bit_state_c   ==  SEND_BIT4   && uart_send_finish == 1'b1;
    assign bit5_bit6_start  = data_bit_state_c   ==  SEND_BIT5   && uart_send_finish == 1'b1;
    assign bit6_bit7_start  = data_bit_state_c   ==  SEND_BIT6   && uart_send_finish == 1'b1;
    assign bit7_bit8_start  = data_bit_state_c   ==  SEND_BIT7   && uart_send_finish == 1'b1;
    assign bit8_idle_start  = data_bit_state_c   ==  SEND_BIT8   && uart_send_finish == 1'b1;

    //第四段：同步时序always模块，格式化描述寄存器输出（可有多个输出）  
    always @ (posedge sys_clk_50m or negedge sys_rst_n)begin
        if(sys_rst_n == 1'b0)begin
            send_data <= 8'b1111_1111;      //初始化
        end
        else if(data_bit_state_c == SEND_IDLE && data_type_state_c == DATA_A)begin
            send_data <= UART_DATA_A;
        end
        else if(data_bit_state_c == SEND_IDLE && data_type_state_c == DATA_B)begin
            send_data <= UART_DATA_B;
        end
        else if(data_bit_state_c == SEND_IDLE && data_type_state_c == DATA_C)begin
            send_data <= UART_DATA_C;
        end
        else if(data_bit_state_c == SEND_IDLE && data_type_state_c == DATA_D)begin
            send_data <= UART_DATA_D;
        end
        else if(data_bit_state_c == SEND_IDLE && data_type_state_c == DATA_E)begin
            send_data <= UART_DATA_E;
        end
        else if(data_bit_state_c == SEND_IDLE && data_type_state_c == DATA_F)begin
            send_data <= UART_DATA_F;
        end
        else if(data_bit_state_c == SEND_IDLE && data_type_state_c == DATA_G)begin
            send_data <= UART_LINE_FEED;
        end
        else if(data_bit_state_c == SEND_BIT0)begin
            send_data <= uart_send_data_0 + 8'd48;
        end
        else if(data_bit_state_c == SEND_BIT1)begin
            send_data <= uart_send_data_1 + 8'd48;
        end
        else if(data_bit_state_c == SEND_BIT2)begin
            send_data <= uart_send_data_2 + 8'd48;
        end
        else if(data_bit_state_c == SEND_BIT3)begin
            send_data <= uart_send_data_3 + 8'd48;
        end
        else if(data_bit_state_c == SEND_BIT4)begin
            send_data <= uart_send_data_4 + 8'd48;
        end
        else if(data_bit_state_c == SEND_BIT5)begin
            send_data <= uart_send_data_5 + 8'd48;
        end
        else if(data_bit_state_c == SEND_BIT6)begin
            send_data <= uart_send_data_6 + 8'd48;
        end
        else if(data_bit_state_c == SEND_BIT7)begin
            send_data <= uart_send_data_7 + 8'd48;
        end
        else if(data_bit_state_c == SEND_BIT8)begin
            send_data <= uart_send_data_8 + 8'd48;
        end             
        else begin
            send_data <= 8'b0;
        end
    end


    endmodule

