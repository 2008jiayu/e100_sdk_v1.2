/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaPLL.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for PLL Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_PLL_H_
#define _AMBA_PLL_H_

#include "AmbaPLL_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaPLL.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaPLL_Init(void);

int AmbaPLL_SetClkRef(UINT32 Frequency);
int AmbaPLL_SetExtClkConfig(AMBA_PLL_EXT_CLK_CONFIG_s *pExtClkConfig);

int AmbaPLL_SetVoutTvClkConfig(AMBA_PLL_VA_CLK_CONFIG_u *pVoutTvClkConfig);
int AmbaPLL_SetVoutLcdClkConfig(AMBA_PLL_VA_CLK_CONFIG_u *pVoutLcdClkConfig);
int AmbaPLL_SetAudioClkConfig(AMBA_PLL_VA_CLK_CONFIG_u *pAudioClkConfig);

int AmbaPLL_SetUartClkConfig(AMBA_PLL_UART_SRC_e UartSrcClkSel);
int AmbaPLL_SetSpiClkConfig(int UseExtSrcClk4Spi);
int AmbaPLL_SetPwmClkConfig(int UseExtSrcClk4Pwm);
int AmbaPLL_SetEnetClkConfig(int UseEnet2ndRefClk);

/*---------------------------------------------------------------------------*/
UINT32 AmbaPLL_GetVoutLcdClk(void);
UINT32 AmbaPLL_GetVoutTvClk(void);
UINT32 AmbaPLL_GetSdClk(void);
UINT32 AmbaPLL_GetSensorClk(void);
UINT32 AmbaPLL_GetIdspClk(void);
UINT32 AmbaPLL_GetDdrioClk(void);
UINT32 AmbaPLL_GetDdrClk(void);
UINT32 AmbaPLL_GetCoreClk(void);
UINT32 AmbaPLL_GetAudioClk(void);
UINT32 AmbaPLL_GetHdmiClk(void);
UINT32 AmbaPLL_GetCortexClk(void);
UINT32 AmbaPLL_GetClkRef(void);

UINT32 AmbaPLL_GetAhbClk(void);
UINT32 AmbaPLL_GetApbClk(void);
UINT32 AmbaPLL_GetUartClk(void);
UINT32 AmbaPLL_GetSpi0Clk(void);
UINT32 AmbaPLL_GetMotorClk(void);
UINT32 AmbaPLL_GetIrClk(void);
UINT32 AmbaPLL_GetSpi1Clk(void);
UINT32 AmbaPLL_GetPwmClk(void);
UINT32 AmbaPLL_GetSd48Clk(void);
UINT32 AmbaPLL_GetSdioClk(void);
UINT32 AmbaPLL_GetUsbPhyClk(void);
UINT32 AmbaPLL_GetAdcClk(void);
UINT32 AmbaPLL_GetDebounceClk(void);
UINT32 AmbaPLL_GetSoVinClk(void);
UINT32 AmbaPLL_GetSoPipClk(void);
UINT32 AmbaPLL_GetAxiClk(void);
UINT32 AmbaPLL_GetEnetClk(void);
UINT32 AmbaPLL_GetHdmiPhyClk(void);

/*---------------------------------------------------------------------------*/
int AmbaPLL_SetSensorClkDir(AMBA_PLL_SENSOR_CLK_DIR_e ClkSiDir);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaPLL_SetClkFreq.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaPLL_SetVoutLcdClk(UINT32 Frequency);
int AmbaPLL_SetVoutTvClk(UINT32 Frequency);
int AmbaPLL_SetSensorClk(UINT32 Frequency);
int AmbaPLL_SetIdspClk(UINT32 Frequency);
int AmbaPLL_SetDdrioClk(UINT32 Frequency);
int AmbaPLL_SetDdrClk(UINT32 Frequency);
int AmbaPLL_SetCoreClk(UINT32 Frequency);
int AmbaPLL_SetAudioClk(UINT32 Frequency);
int AmbaPLL_SetCortexClk(UINT32 Frequency);

int AmbaPLL_SetUartClk(UINT32 Frequency);
int AmbaPLL_SetSpi0Clk(UINT32 Frequency);
int AmbaPLL_SetMotorClk(UINT32 Frequency);
int AmbaPLL_SetIrClk(UINT32 Frequency);
int AmbaPLL_SetSpi1Clk(UINT32 Frequency);
int AmbaPLL_SetPwmClk(UINT32 Frequency);
int AmbaPLL_SetSd48Clk(UINT32 Frequency);
int AmbaPLL_SetSdioClk(UINT32 Frequency);
int AmbaPLL_SetAdcClk(UINT32 Frequency);
int AmbaPLL_SetDebounceClk(UINT32 Frequency);
int AmbaPLL_SetSoVinClk(UINT32 Frequency);
int AmbaPLL_SetSoPipClk(UINT32 Frequency);
int AmbaPLL_SetEnetClk(UINT32 Frequency);

int AmbaPLL_SetOpMode(AMBA_OPMODE_CLK_PARAM_s *pClkParam);
int AmbaPLL_SetStandbyMode(int PowerDown, void (*PreProc)(void), void (*PostProc)(void));

#endif /* _AMBA_PLL_H_ */
