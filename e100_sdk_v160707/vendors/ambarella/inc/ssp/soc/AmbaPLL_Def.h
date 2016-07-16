/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaPLL_Def.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for PLL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_PLL_DEF_H_
#define _AMBA_PLL_DEF_H_

typedef enum _AMBA_PLL_VA_EXT_CLK_e_ {
    AMBA_PLL_VA_EXT_XXGPIO125 = 0,          /* =iopad_ahb_spclk_c, xx_gpio125 (Hz)*/
    AMBA_PLL_VA_EXT_CLK_SI,                 /* clk_si */
    AMBA_PLL_VA_EXT_LVDS_SCLK,              /* lvds_idsp_sclk */
    AMBA_PLL_VA_EXT_MIPI_CLK,               /* idsp_rct_lvds_mipi_clock */
    AMBA_PLL_VA_EXT_MIPI_PIP_CLK,           /* idsp_rct_Pip_lvds_mipi_clock */

    AMBA_NUM_PLL_VA_EXT_CLK                 /* Total number of Source Clock Selections */
} AMBA_PLL_VA_EXT_CLK_e;

typedef enum _AMBA_PLL_UART_SRC_e_ {
    AMBA_PLL_UART_SRC_CLK_REF = 0,          /* CLK_REF */
    AMBA_PLL_UART_SRC_CORE = 1,             /* gclk_core */
    AMBA_PLL_UART_SRC_IDSP = 3              /* gclk_idsp */
} AMBA_PLL_UART_SRC_e;

typedef enum _AMBA_PLL_PWM_SRC_e_ {
    AMBA_PLL_PWM_SRC_APB = 0,               /* gclk_apb */
    AMBA_PLL_PWM_SRC_CLK_REF = 3            /* CLK_REF */
} AMBA_PLL_PWM_SRC_e;

typedef enum _AMBA_PLL_SSI3_SRC_e_ {
    AMBA_PLL_SSI3_SRC_ENT = 0,               /* gclk_apb */
    AMBA_PLL_SSI3_SRC_CORE_2X,             /* gclk_core_2x */
    AMBA_PLL_SSI3_SRC_CLK_SSI,             /* CLK_SSI */
    AMBA_PLL_SSI3_SRC_CLK_REF            /* CLK_REF */
} AMBA_PLL_SSI3_SRC_e;

typedef struct _AMBA_PLL_EXT_CLK_CONFIG_s_ {
    UINT32  VoutLcdFreq;                    /* Video2 (VoutLCD) External Clock Frequency (Hz) */
    UINT32  VoutTvFreq;                     /* Video (VoutTV) External Clock Frequency (Hz) */
    UINT32  AudioFreq;                      /* Audio External Clock Frequency (Hz) */
    UINT32  Audio1ChFreq;                   /* single-channel Audio External Clock Frequency (Hz) */

    UINT32  VaSrcClkFreq[AMBA_NUM_PLL_VA_EXT_CLK];
} AMBA_PLL_EXT_CLK_CONFIG_s;

typedef union _AMBA_PLL_VA_CLK_CONFIG_u_ {
    UINT32  Data;

    struct {
        UINT32  UseExtClk:              1;  /* [0]: Use External Clock for VideoTV/Audio Clock: 1 - enable */
        UINT32  UseVoutTvClk4VoutLcd:   1;  /* [1]: Use VoutTV Clock (glck_vo) for VideoLCD Clock: 1 - enable */

        UINT32  UseExtClk4PllRef:       1;  /* [2]: Use External Clock for Video/Audio Clock PLL Reference: 1 - enable */
        UINT32  ExtClk4PllRef:          3;  /* [5:3]: External Clock Selections for Video/Audio Clock PLL Reference */

        UINT32  Reserved:               26; /* [31:6]: Reserved */
    } Bits;
} AMBA_PLL_VA_CLK_CONFIG_u;

typedef enum _AMBA_PLL_SENSOR_CLK_DIR_e_ {
    AMBA_PLL_SENSOR_CLK_OUTPUT = 0,         /* sensor clock output mode */
    AMBA_PLL_SENSOR_CLK_INPUT               /* sensor clock input mode */
} AMBA_PLL_SENSOR_CLK_DIR_e;

/*-----------------------------------------------------------------------------------------------*\
 * Clock parameters for various Operating mode
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_OPMODE_CLK_PARAM_s_ {
    UINT32  IdspClkFreq;                    /* =gclk_idsp: iDSP Clock Frequency (Hz) */
    UINT32  CoreClkFreq;                    /* =gclk_core: Core Clock Frequency (Hz) */
    UINT32  CortexClkFreq;                  /* =gclk_cortex: Cortex Clock Frequency (Hz) */
} AMBA_OPMODE_CLK_PARAM_s;

#endif /* _AMBA_PLL_DEF_H_ */
