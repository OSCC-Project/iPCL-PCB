/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* https://www.renesas.com/disclaimer
*
* Copyright (C) 2021 Renesas Electronics Corporation.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : qe_touch_define.h
* Description  : This file includes definitions.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History      : MM/DD/YYYY Version Description
*              : 09/02/2019 1.00    First Release
*              : 12/26/2019 1.10    Corresponding for FSP V0.10.0
*              : 01/16/2020 1.11    Adding "Button State Mask" macros
*              : 02/20/2020 1.20    Corresponding for FSP V0.12.0
*              : 03/04/2020 1.21    Corresponding for FSP V1.0.0 RC0
*              : 06/26/2020 1.22    Corresponding for FSP V1.1.0
*              : 09/10/2020 1.30    Corresponding for FSP V2.0.0 Beta2
*              : 05/26/2021 1.40    Adding Diagnosis Supporting
*              : 06/01/2021 1.41    Fixing a Little
*              : 11/17/2021 1.50    Adding information for Initial Offset Tuning
*              : 12/06/2021 1.51    Fixing a Little
***********************************************************************************************************************/

#ifndef QE_TOUCH_DEFINE_H
#define QE_TOUCH_DEFINE_H


/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define CTSU_CFG_NUM_SELF_ELEMENTS   (15)
#define CTSU_CFG_NUM_MUTUAL_ELEMENTS (0)
#define CTSU_CFG_NUM_CFC             (0)
#define CTSU_CFG_NUM_CFC_TX          (0)

#define TOUCH_CFG_MONITOR_ENABLE (1)
#define TOUCH_CFG_NUM_BUTTONS    (15)
#define TOUCH_CFG_NUM_SLIDERS    (0)
#define TOUCH_CFG_NUM_WHEELS     (0)
#define TOUCH_CFG_PAD_ENABLE     (0)

#define QE_TOUCH_MACRO_CTSU_IP_KIND (2)

#define CTSU_CFG_VCC_MV           (3300)
#define CTSU_CFG_LOW_VOLTAGE_MODE (0)

#define CTSU_CFG_PCLK_DIVISION (0)

#define CTSU_CFG_TSCAP_PORT (0x010C)

#define CTSU_CFG_NUM_SUMULTI   (3)
#define CTSU_CFG_SUMULTI0      (0x3F)
#define CTSU_CFG_SUMULTI1      (0x36)
#define CTSU_CFG_SUMULTI2      (0x48)

#define CTSU_CFG_CALIB_RTRIM_SUPPORT       (0)
#define CTSU_CFG_TEMP_CORRECTION_SUPPORT   (0)
#define CTSU_CFG_TEMP_CORRECTION_TS        (0)
#define CTSU_CFG_TEMP_CORRECTION_TIME      (0)



#define CTSU_CFG_TARGET_VALUE_QE_SUPPORT (1)

/***********************************************************************************************************************
Button State Mask for each configuration.
***********************************************************************************************************************/
#define CONFIG01_INDEX_BUTTON00     (14)
#define CONFIG01_MASK_BUTTON00      (1ULL << CONFIG01_INDEX_BUTTON00)
#define CONFIG01_INDEX_BUTTON01     (13)
#define CONFIG01_MASK_BUTTON01      (1ULL << CONFIG01_INDEX_BUTTON01)
#define CONFIG01_INDEX_BUTTON02     (10)
#define CONFIG01_MASK_BUTTON02      (1ULL << CONFIG01_INDEX_BUTTON02)
#define CONFIG01_INDEX_BUTTON03     (9)
#define CONFIG01_MASK_BUTTON03      (1ULL << CONFIG01_INDEX_BUTTON03)
#define CONFIG01_INDEX_BUTTON04     (8)
#define CONFIG01_MASK_BUTTON04      (1ULL << CONFIG01_INDEX_BUTTON04)
#define CONFIG01_INDEX_BUTTON05     (1)
#define CONFIG01_MASK_BUTTON05      (1ULL << CONFIG01_INDEX_BUTTON05)
#define CONFIG01_INDEX_BUTTON06     (0)
#define CONFIG01_MASK_BUTTON06      (1ULL << CONFIG01_INDEX_BUTTON06)
#define CONFIG01_INDEX_BUTTON07     (2)
#define CONFIG01_MASK_BUTTON07      (1ULL << CONFIG01_INDEX_BUTTON07)
#define CONFIG01_INDEX_BUTTON08     (3)
#define CONFIG01_MASK_BUTTON08      (1ULL << CONFIG01_INDEX_BUTTON08)
#define CONFIG01_INDEX_BUTTON09     (4)
#define CONFIG01_MASK_BUTTON09      (1ULL << CONFIG01_INDEX_BUTTON09)
#define CONFIG01_INDEX_BUTTON10     (5)
#define CONFIG01_MASK_BUTTON10      (1ULL << CONFIG01_INDEX_BUTTON10)
#define CONFIG01_INDEX_BUTTON11     (6)
#define CONFIG01_MASK_BUTTON11      (1ULL << CONFIG01_INDEX_BUTTON11)
#define CONFIG01_INDEX_BUTTON12     (7)
#define CONFIG01_MASK_BUTTON12      (1ULL << CONFIG01_INDEX_BUTTON12)
#define CONFIG01_INDEX_BUTTON13     (11)
#define CONFIG01_MASK_BUTTON13      (1ULL << CONFIG01_INDEX_BUTTON13)
#define CONFIG01_INDEX_BUTTON14     (12)
#define CONFIG01_MASK_BUTTON14      (1ULL << CONFIG01_INDEX_BUTTON14)

#endif /* QE_TOUCH_DEFINE_H */
