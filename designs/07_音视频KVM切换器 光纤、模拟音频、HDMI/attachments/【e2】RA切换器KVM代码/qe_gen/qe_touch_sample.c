/***********************************************************************
*
* FILE : qe_sample_main.c
* DATE : 2022-02-14
* DESCRIPTION : Main Program for RA
*
* NOTE:THIS IS A TYPICAL EXAMPLE.
*
***********************************************************************/
#include "qe_touch_config.h"
#define TOUCH_SCAN_INTERVAL_EXAMPLE (20)    /* milliseconds */
#define uint16_t unsigned int
uint16_t aux_mute=1;//1-on 0-off
uint16_t opt_mute=1;//1-on 0-off
uint16_t keyreset=0;
void qe_touch_main(void);


uint64_t button_status;
#if (TOUCH_CFG_NUM_SLIDERS != 0)
uint16_t slider_position[TOUCH_CFG_NUM_SLIDERS];
#endif
#if (TOUCH_CFG_NUM_WHEELS != 0)
uint16_t wheel_position[TOUCH_CFG_NUM_WHEELS];
#endif






void qe_touch_main(void)
{
    fsp_err_t err;




    /* Open Touch middleware */
    err = RM_TOUCH_Open(g_qe_touch_instance_config01.p_ctrl, g_qe_touch_instance_config01.p_cfg);
    if (FSP_SUCCESS != err)
    {
        while (true) {}
    }



    /* Main loop */
    while (true)
    {


        /* for [CONFIG01] configuration */
        err = RM_TOUCH_ScanStart(g_qe_touch_instance_config01.p_ctrl);//执行TOUCH 扫描开启
        if (FSP_SUCCESS != err)
        {
            while (true) {}
        }
          while (0 == g_qe_touch_flag) {}
        g_qe_touch_flag = 0;

        err = RM_TOUCH_DataGet(g_qe_touch_instance_config01.p_ctrl, &button_status, NULL, NULL);
        if (FSP_SUCCESS == err)
        {
            /* TODO: Add your own code here. */

            if(button_status==0)
            {
            R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_04, BSP_IO_LEVEL_LOW);
            keyreset=0;
            }
            else
            {
            R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_04, BSP_IO_LEVEL_HIGH);
            }



            if((button_status&(0b000000001000000))&&keyreset==0)
            {
                if(aux_mute==1)
                {
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_07, BSP_IO_LEVEL_HIGH);
                aux_mute=0;
                keyreset=1;
                }
                else if(aux_mute==0)
                {
                    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_07, BSP_IO_LEVEL_LOW);
                    aux_mute=1;
                    keyreset=1;
                }
            }
            if(button_status&(0b001000000000000))
            {
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_12, BSP_IO_LEVEL_LOW);//AUX B
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_13, BSP_IO_LEVEL_LOW);//AUX A
            }
            if(button_status&(0b000100000000000))
            {
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_12, BSP_IO_LEVEL_LOW);//AUX B
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_13, BSP_IO_LEVEL_HIGH);//AUX A
            }
            if(button_status&(0b000000010000000))
            {
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_12, BSP_IO_LEVEL_HIGH);//AUX B
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_13, BSP_IO_LEVEL_LOW);//AUX A
            }
            if(button_status&(0b000000000100000))
            {
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_12, BSP_IO_LEVEL_HIGH);//AUX B
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_13, BSP_IO_LEVEL_HIGH);//AUX A
            }




            if((button_status&(0b000000000001000))&&keyreset==0)
            {
                if(opt_mute==1)
                {
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_05_PIN_00, BSP_IO_LEVEL_HIGH);
                opt_mute=0;
                keyreset=1;
                }
                else if(opt_mute==0)
                {
                    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_05_PIN_00, BSP_IO_LEVEL_LOW);
                    opt_mute=1;
                    keyreset=1;
                }
            }

            if(button_status&(0b000000000000100))
            {
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_01, BSP_IO_LEVEL_LOW);//opt B
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_00, BSP_IO_LEVEL_LOW);//opt A
            }
            if(button_status&(0b000000000000010))
            {
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_01, BSP_IO_LEVEL_HIGH);//opt B
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_00, BSP_IO_LEVEL_LOW);//opt A
            }
            if(button_status&(0b000000000000001))
            {
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_01, BSP_IO_LEVEL_LOW);//opt B
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_00, BSP_IO_LEVEL_HIGH);//opt A
            }
            if(button_status&(0b000000000010000))
            {
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_01, BSP_IO_LEVEL_HIGH);//opt B
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_00, BSP_IO_LEVEL_HIGH);//opt A
            }


            if(button_status&(0b000000100000000))
            {
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_15, BSP_IO_LEVEL_LOW);//HD1
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_14, BSP_IO_LEVEL_LOW);//HD2
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_13, BSP_IO_LEVEL_LOW);//HD3
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_08, BSP_IO_LEVEL_LOW);//HDM
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_07, BSP_IO_LEVEL_LOW);//HD4
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_06, BSP_IO_LEVEL_HIGH);//HD5
            }
            if(button_status&(0b000001000000000))
            {
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_15, BSP_IO_LEVEL_LOW);//HD1
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_14, BSP_IO_LEVEL_LOW);//HD2
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_13, BSP_IO_LEVEL_LOW);//HD3
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_08, BSP_IO_LEVEL_LOW);//HDM
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_07, BSP_IO_LEVEL_HIGH);//HD4
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_06, BSP_IO_LEVEL_LOW);//HD5
            }
            if(button_status&(0b000010000000000))
            {
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_15, BSP_IO_LEVEL_LOW);//HD1
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_14, BSP_IO_LEVEL_LOW);//HD2
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_13, BSP_IO_LEVEL_HIGH);//HD3
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_08, BSP_IO_LEVEL_HIGH);//HDM
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_07, BSP_IO_LEVEL_LOW);//HD4
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_06, BSP_IO_LEVEL_LOW);//HD5
            }
            if(button_status&(0b010000000000000))
            {
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_15, BSP_IO_LEVEL_LOW);//HD1
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_14, BSP_IO_LEVEL_HIGH);//HD2
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_13, BSP_IO_LEVEL_LOW);//HD3
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_08, BSP_IO_LEVEL_HIGH);//HDM
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_07, BSP_IO_LEVEL_LOW);//HD4
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_06, BSP_IO_LEVEL_LOW);//HD5
            }
            if(button_status&(0b100000000000000))
            {
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_15, BSP_IO_LEVEL_HIGH);//HD1
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_14, BSP_IO_LEVEL_LOW);//HD2
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_09_PIN_13, BSP_IO_LEVEL_LOW);//HD3
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_08, BSP_IO_LEVEL_HIGH);//HDM
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_07, BSP_IO_LEVEL_LOW);//HD4
                R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_06, BSP_IO_LEVEL_LOW);//HD5
            }




        }




        /* FIXME: Since this is a temporary process, so re-create a waiting process yourself. */
        R_BSP_SoftwareDelay(TOUCH_SCAN_INTERVAL_EXAMPLE, BSP_DELAY_UNITS_MILLISECONDS);
    }
}
