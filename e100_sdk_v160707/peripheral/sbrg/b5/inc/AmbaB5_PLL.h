/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaB5_PLL.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for B5 PLL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_B5_PLL_H_
#define _AMBA_B5_PLL_H_

typedef enum _AMBA_B5_PLL_IDX_e_ {
    AMBA_B5_PLL_CORE = 0,
    AMBA_B5_PLL_PHY,
    AMBA_B5_PLL_SENSOR,
    AMBA_B5_PLL_VIDEO,

    AMBA_NUM_B5_PLL                             /* Total number of PLLs */
} AMBA_B5_PLL_IDX_e;

typedef enum _AMBA_B5_REFERENCE_CLOCK_SELECT_e_ {
    AMBA_B5_REF_CLK_PWM = 0,
    AMBA_B5_REF_CLK_EXTERNAL
} AMBA_B5_REFERENCE_CLOCK_SELECT_e;

/* Clock frequency generated from pll_out_core */
typedef struct _AMBA_B5_CORE_PLL_CLK_CTRL_s_ {
    UINT32  CoreFreq;               /* gclk_core/gclk_apb */
    UINT32  IdspFreq;               /* gclk_idsp */
    UINT32  SlowFreq;               /* gclk_slow */
    UINT32  SpiFreq;                /* gclk_ssi (generated from gclk_core) */
} AMBA_B5_CORE_PLL_CLK_CTRL_s;

/* Clock frequency generated from pll_out_phy */
typedef struct _AMBA_B5_PHY_PLL_CLK_CTRL_s_ {
    UINT32  PhyFreq;                /* gclk_phy */
    UINT32  Pwm3xFreq;              /* pwm_clk_3x/pwm_clk_ref */
} AMBA_B5_PHY_PLL_CLK_CTRL_s;

/* Clock frequency generated from pll_out_video */
typedef struct _AMBA_B5_VIDEO_PLL_CLK_CTRL_s_ {
    UINT32  VideoFreq;              /* gclk_vo */
} AMBA_B5_VIDEO_PLL_CLK_CTRL_s;

/* Clock frequency generated from pll_out_sensor */
typedef struct _AMBA_B5_SENSOR_PLL_CLK_CTRL_s_ {
    UINT32  SensorFreq;             /* gclk_si */
} AMBA_B5_SENSOR_PLL_CLK_CTRL_s;

typedef struct _AMBA_B5_CLK_DOMAIN_CTRL_s_ {
    UINT32  ClkRefFreq;                         /* CLK_REF: Clock Frequency (Hz) */
    UINT32  PllOutClkFreq[AMBA_NUM_B5_PLL];     /* gclk_core, gclk_??? Frequency (Hz) */
    float   fPllVcoFreq[AMBA_NUM_B5_PLL];       /* VCO Frequency (Hz) */

    AMBA_B5_CORE_PLL_CLK_CTRL_s     CorePllClkCtrl;      /* all clocks derived by Core PLL */
    AMBA_B5_PHY_PLL_CLK_CTRL_s      PhyPllClkCtrl;       /* all clocks derived by PHY PLL */
    AMBA_B5_VIDEO_PLL_CLK_CTRL_s    VideoPllClkCtrl;     /* all clocks derived by Video PLL */
    AMBA_B5_SENSOR_PLL_CLK_CTRL_s   SensorPllClkCtrl;    /* all clocks derived by Sensor PLL */
} AMBA_B5_CLK_DOMAIN_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Macro Definitions
\*-----------------------------------------------------------------------------------------------*/
#define AmbaB5N_PllGetClkRef()          (AmbaB5N_ClkDomainCtrl.ClkRefFreq)
#define AmbaB5F_PllGetClkRef(n)         (AmbaB5F_ClkDomainCtrl[n].ClkRefFreq)

#define AmbaB5N_PllGetCoreClk()         (AmbaB5N_ClkDomainCtrl.CorePllClkCtrl.CoreFreq)
#define AmbaB5N_PllGetIdspClk()         (AmbaB5N_ClkDomainCtrl.CorePllClkCtrl.IdspFreq)
#define AmbaB5N_PllGetSlowClk()         (AmbaB5N_ClkDomainCtrl.CorePllClkCtrl.SlowFreq)
#define AmbaB5N_PllGetSpiClk()          (AmbaB5N_ClkDomainCtrl.CorePllClkCtrl.SpiFreq)
#define AmbaB5N_PllGetPhyClk()          (AmbaB5N_ClkDomainCtrl.PhyPllClkCtrl.PhyFreq)
#define AmbaB5N_PllGetPwm3xClk()        (AmbaB5N_ClkDomainCtrl.PhyPllClkCtrl.Pwm3xFreq)
#define AmbaB5N_PllGetVideoClk()        (AmbaB5N_ClkDomainCtrl.VideoPllClkCtrl.VideoFreq)
#define AmbaB5N_PllGetSensorClk()       (AmbaB5N_ClkDomainCtrl.SensorPllClkCtrl.SensorFreq)

#define AmbaB5F_PllGetCoreClk(n)        (AmbaB5F_ClkDomainCtrl[n].CorePllClkCtrl.CoreFreq)
#define AmbaB5F_PllGetIdspClk(n)        (AmbaB5F_ClkDomainCtrl[n].CorePllClkCtrl.IdspFreq)
#define AmbaB5F_PllGetSlowClk(n)        (AmbaB5F_ClkDomainCtrl[n].CorePllClkCtrl.SlowFreq)
#define AmbaB5F_PllGetSpiClk(n)         (AmbaB5F_ClkDomainCtrl[n].CorePllClkCtrl.SpiFreq)
#define AmbaB5F_PllGetPhyClk(n)         (AmbaB5F_ClkDomainCtrl[n].PhyPllClkCtrl.PhyFreq)
#define AmbaB5F_PllGetPwm3xClk(n)       (AmbaB5F_ClkDomainCtrl[n].PhyPllClkCtrl.Pwm3xFreq)
#define AmbaB5F_PllGetVideoClk(n)       (AmbaB5F_ClkDomainCtrl[n].VideoPllClkCtrl.VideoFreq)
#define AmbaB5F_PllGetSensorClk(n)      (AmbaB5F_ClkDomainCtrl[n].SensorPllClkCtrl.SensorFreq)

#define AmbaB5_PllGetVoutClk()          (AmbaB5N_ClkDomainCtrl.VideoPllClkCtrl.VideoFreq)

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaB5_PLL.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_B5_CLK_DOMAIN_CTRL_s AmbaB5N_ClkDomainCtrl;
extern AMBA_B5_CLK_DOMAIN_CTRL_s AmbaB5F_ClkDomainCtrl[AMBA_NUM_B5_CHANNEL];

int AmbaB5_PllInit(UINT32 B5nClkRefFreq, INT32 B5f0ClkRefFreq, INT32 B5f1ClkRefFreq, INT32 B5f2ClkRefFreq, INT32 B5f3ClkRefFreq);

int AmbaB5_PllSetCoreClk(AMBA_B5_CHANNEL_s *pB5Chan, UINT32 Frequency);
int AmbaB5_PllSetIdspClk(AMBA_B5_CHANNEL_s *pB5Chan, UINT32 Frequency);
int AmbaB5_PllSetSensorClk(AMBA_B5_CHANNEL_s *pB5Chan, UINT32 Frequency);
int AmbaB5_PllSetVoutClk(UINT32 Frequency);

int AmbaB5_PllSwPllConfig(UINT32 FrameTimeInMs);
int AmbaB5_PllSwPllEnable(void);
int AmbaB5_PllSwPllDisable(void);
void AmbaB5_PllSwPllShowMsg(UINT32 Flag);
void AmbaB5_PllSwPllVinHookFunc(UINT32 EntryArg);

#endif /* _AMBA_B5_PLL_H_ */
