/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_PLL.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for PLL RTSL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_PLL_H_
#define _AMBA_RTSL_PLL_H_

#include "AmbaPLL_Def.h"

typedef enum _AMBA_PLL_IDX_e_ {
    AMBA_PLL_VOUT_LCD = 0,
    AMBA_PLL_SD,
    AMBA_PLL_SENSOR,
    AMBA_PLL_IDSP,
    AMBA_PLL_DDRIO,
    AMBA_PLL_CORE,
    AMBA_PLL_AUDIO,
    AMBA_PLL_HDMI,
    AMBA_PLL_CORTEX,
    AMBA_PLL_ENET,

    AMBA_NUM_PLL                                /* Total number of PLLs */
} AMBA_PLL_IDX_e;

typedef struct _AMBA_CLK_DOMAIN_CTRL_s_ {
    UINT32  ClkRefFreq;                         /* CLK_REF: Clock Frequency (Hz) */
    UINT32  PllRefFreq[AMBA_NUM_PLL];           /* Reference Clock Frequency (Hz) */
    UINT32  PllOutClkFreq[AMBA_NUM_PLL];        /* gclk_core, gclk_??? Frequency (Hz) */
    AMBA_PLL_EXT_CLK_CONFIG_s ExtClkConfig;     /* External Clock Configurations */

    float   fPllVcoFreq[AMBA_NUM_PLL];          /* VCO Frequency (Hz) */
} AMBA_CLK_DOMAIN_CTRL_s;

typedef struct _AMBA_CORTEX_PLL_CLK_CTRL_s_ {
    UINT32  PrivateTimerFreq;       /* Private Timer frequency */
    UINT32  GlobalTimerFreq;        /* Global Timer frequency */
} AMBA_CORTEX_PLL_CLK_CTRL_s;

typedef struct _AMBA_CORE_PLL_CLK_CTRL_s_ {
    UINT32  SdioFreq;               /* gclk_sdio */
    UINT32  Sd48Freq;               /* gclk_sd48 */
    UINT32  Spi0Freq;               /* gclk_spi */
    UINT32  Spi1Freq;               /* gclk_spi2 */
} AMBA_CORE_PLL_CLK_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_PLL.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_CLK_DOMAIN_CTRL_s     AmbaClkDomainCtrl;
extern AMBA_CORTEX_PLL_CLK_CTRL_s AmbaCortexPllClkCtrl;
extern AMBA_CORE_PLL_CLK_CTRL_s   AmbaCorePllClkCtrl;

int AmbaRTSL_PllInit(UINT32 ClkRefFreq, AMBA_PLL_EXT_CLK_CONFIG_s *pExtClkConfig);

int AmbaRTSL_PllSetClkRef(UINT32 Frequency);
int AmbaRTSL_PllSetExtClkConfig(AMBA_PLL_EXT_CLK_CONFIG_s *pExtClkConfig);

int AmbaRTSL_PllSetVoutTvClkConfig(AMBA_PLL_VA_CLK_CONFIG_u *pVoutTvClkConfig);
int AmbaRTSL_PllSetVoutLcdClkConfig(AMBA_PLL_VA_CLK_CONFIG_u *pVoutLcdClkConfig);
int AmbaRTSL_PllSetAudioClkConfig(AMBA_PLL_VA_CLK_CONFIG_u *pAudioClkConfig);

int AmbaRTSL_PllSetUartClkConfig(AMBA_PLL_UART_SRC_e UartSrcClkSel);
int AmbaRTSL_PllSetSpiClkConfig(int UseExtSrcClk4Spi);
int AmbaRTSL_PllSetPwmClkConfig(int UseExtSrcClk4Pwm);
int AmbaRTSL_PllSetEnetClkConfig(int UseEnet2ndRefClk);

int AmbaRTSL_PllSetHdmiClkConfig(int UseXX_XIN4PllRef);
int AmbaRTSL_PllSetSensorClkDir(AMBA_PLL_SENSOR_CLK_DIR_e ClkSiDir);

/*---------------------------------------------------------------------------*/
#define AmbaRTSL_PllGetClkRef()         (AmbaClkDomainCtrl.ClkRefFreq)

#define AmbaRTSL_PllGetVoutLcdClk()     (AmbaClkDomainCtrl.PllOutClkFreq[AMBA_PLL_VOUT_LCD])
#define AmbaRTSL_PllGetSdClk()          (AmbaClkDomainCtrl.PllOutClkFreq[AMBA_PLL_SD])
#define AmbaRTSL_PllGetSensorClk()      (AmbaClkDomainCtrl.PllOutClkFreq[AMBA_PLL_SENSOR])
#define AmbaRTSL_PllGetIdspClk()        (AmbaClkDomainCtrl.PllOutClkFreq[AMBA_PLL_IDSP])
#define AmbaRTSL_PllGetDdrioClk()       (AmbaClkDomainCtrl.PllOutClkFreq[AMBA_PLL_DDRIO])
#define AmbaRTSL_PllGetCore2xClk()      (AmbaClkDomainCtrl.PllOutClkFreq[AMBA_PLL_CORE])
#define AmbaRTSL_PllGetCoreClk()        (AmbaClkDomainCtrl.PllOutClkFreq[AMBA_PLL_CORE] >> 1)
#define AmbaRTSL_PllGetAudioClk()       (AmbaClkDomainCtrl.PllOutClkFreq[AMBA_PLL_AUDIO])
#define AmbaRTSL_PllGetHdmiClk()        (AmbaClkDomainCtrl.PllOutClkFreq[AMBA_PLL_HDMI])
#define AmbaRTSL_PllGetCortexClk()      (AmbaClkDomainCtrl.PllOutClkFreq[AMBA_PLL_CORTEX])

UINT32 AmbaRTSL_PllGetAhbClk(void);
UINT32 AmbaRTSL_PllGetApbClk(void);
UINT32 AmbaRTSL_PllGetUartClk(void);
UINT32 AmbaRTSL_PllGetSpi0Clk(void);
UINT32 AmbaRTSL_PllGetMotorClk(void);
UINT32 AmbaRTSL_PllGetIrClk(void);
UINT32 AmbaRTSL_PllGetSpi1Clk(void);
UINT32 AmbaRTSL_PllGetPwmClk(void);
UINT32 AmbaRTSL_PllGetSd48Clk(void);
UINT32 AmbaRTSL_PllGetSdioClk(void);
UINT32 AmbaRTSL_PllGetSdxcClk(void);
UINT32 AmbaRTSL_PllGetUsbPhyClk(void);
UINT32 AmbaRTSL_PllGetAdcClk(void);
UINT32 AmbaRTSL_PllGetDebounceClk(void);
UINT32 AmbaRTSL_PllGetSoVinClk(void);
UINT32 AmbaRTSL_PllGetSoPipClk(void);
UINT32 AmbaRTSL_PllGetFaceDetectClk(void);
UINT32 AmbaRTSL_PllGetAxiClk(void);
UINT32 AmbaRTSL_PllGetEnetClk(void);
UINT32 AmbaRTSL_PllGetHdmiPhyClk(void);

UINT32 AmbaRTSL_PllGetDdrClk(void);
UINT32 AmbaRTSL_PllGetDdrCalibClk(void);
UINT32 AmbaRTSL_PllGetAudio1ChClk(void);    /* for Single Channel Audio Clock */

/*---------------------------------------------------------------------------*/
UINT32 PLL_GetPllFreq(AMBA_PLL_IDX_e PllIdx);

UINT32 PLL_GetVoutTvClkFreq(void);
UINT32 PLL_GetVoutLcdClkFreq(void);
UINT32 PLL_GetSdClkFreq(void);
UINT32 PLL_GetHdmiClkFreq(void);
UINT32 PLL_GetAudioClkFreq(void);

UINT32 PLL_GetSensorClkFreq(void);
UINT32 PLL_GetEnetClkFreq(void);

UINT32 PLL_GetIdspClkFreq(void);
UINT32 PLL_GetDdrioClkFreq(void);
UINT32 PLL_GetCoreClkFreq(void);
UINT32 PLL_GetCortexClkFreq(void);

UINT32 PLL_GetUartRefFreq(void);
UINT32 PLL_GetSpiRefFreq(void);
UINT32 PLL_GetSpi3RefFreq(void);
UINT32 PLL_GetPwmRefFreq(void);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_PLL_SetClkFreq.c
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_CLK_LIMIT_s_ {
    UINT32  CoreClkFreq;    /* =gclk_core: Core Clock Frequency (Hz) */
    UINT32  IdspClkFreq;    /* =gclk_idsp: iDSP Clock Frequency (Hz) */
    UINT32  DdrClkFreq;     /* =gclk_dram: DDR Clock Frequency (Hz) */
    UINT32  CortexClkFreq;  /* =gclk_cortex: Cortex Clock Frequency (Hz) */
} AMBA_CLK_LIMIT_s;

int AmbaRTSL_PllSetFreq(AMBA_PLL_IDX_e PllIdx, UINT32 Frequency);

int AmbaRTSL_PllSetVoutLcdClk(UINT32 Frequency);
int AmbaRTSL_PllSetVoutTvClk(UINT32 Frequency);
int AmbaRTSL_PllSetSensorClk(UINT32 Frequency);
int AmbaRTSL_PllSetIdspClk(UINT32 Frequency);
int AmbaRTSL_PllSetDdrioClk(UINT32 Frequency);
int AmbaRTSL_PllSetDdrClk(UINT32 Frequency);
int AmbaRTSL_PllSetCoreClk(UINT32 Frequency);
int AmbaRTSL_PllSetAudioClk(UINT32 Frequency);
int AmbaRTSL_PllSetCortexClk(UINT32 Frequency);

int AmbaRTSL_PllSetArmClk(UINT32 Frequency);
int AmbaRTSL_PllSetUartClk(UINT32 Frequency);
int AmbaRTSL_PllSetSpi0Clk(UINT32 Frequency);
int AmbaRTSL_PllSetSpi3Clk(UINT32 Frequency);
int AmbaRTSL_PllSetMotorClk(UINT32 Frequency);
int AmbaRTSL_PllSetIrClk(UINT32 Frequency);
int AmbaRTSL_PllSetSpi1Clk(UINT32 Frequency);
int AmbaRTSL_PllSetPwmClk(UINT32 Frequency);
int AmbaRTSL_PllSetSd48Clk(UINT32 Frequency);
int AmbaRTSL_PllSetSdioClk(UINT32 Frequency);
int AmbaRTSL_PllSetSdxcClk(UINT32 Frequency);
int AmbaRTSL_PllSetAdcClk(UINT32 Frequency);
int AmbaRTSL_PllSetDebounceClk(UINT32 Frequency);
int AmbaRTSL_PllSetSoVinClk(UINT32 Frequency);
int AmbaRTSL_PllSetSoPipClk(UINT32 Frequency);
int AmbaRTSL_PllSetEnetClk(UINT32 Frequency);

int AmbaRTSL_PllSetDdrCalibClk(UINT32 Frequency);
int AmbaRTSL_PllSetAudio1ChClk(UINT32 Frequency);       /* for Single Channel Audio Clock */

int AmbaRTSL_PllResetAllCorePllDividedClks(void);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_PLL_CoefTable.c
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_PLL_COEFF_s_ {
    double  Multiplier;

    struct {
        UINT32  IntProg:    7;
        UINT32  Sdiv:       4;
        UINT32  Sout:       4;
        UINT32  Postscaler: 16;
    } Settings;
} AMBA_PLL_COEFF_s;

int AmbaRTSL_CalculatePllCoeff(UINT32 RefFreq, UINT32 PllOutFreq, AMBA_PLL_COEFF_s *pPllCoeff);
int AmbaRTSL_CalculateHdmiPllCoeff(UINT32 RefFreq, UINT32 PllOutFreq, AMBA_PLL_COEFF_s *pPllCoeff);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_PllOpModeCtrl.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaRTSL_PllSetOpMode(AMBA_OPMODE_CLK_PARAM_s *pClkParam);
int AmbaRTSL_PllSetStandbyMode(int PowerDown, void (*PreProc)(void), void (*PostProc)(void));

void AmbaRTSL_CorePark(void);
void AmbaRTSL_CoreUnpark(void);

int AmbaRTSL_RctTimerWait(UINT32 TimeOut);

#endif /* _AMBA_RTSL_PLL_H_ */
