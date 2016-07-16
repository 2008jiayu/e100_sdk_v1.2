/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCVBS.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for composite video signal encoder
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_CVBS_H_
#define _AMBA_CVBS_H_

#include "AmbaCVBS_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * This structure is used to provide information of the current CVBS configuration
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_CVBS_INFO_s_ {
    UINT32  PixelClock;     /* Pixel clock frequency */
    UINT32  Width;          /* Horizontal display resolution of TV */
    UINT32  Height;         /* Vertical display resolution of TV */
    AMBA_DSP_FRAME_RATE_s   FrameRate;  /* Frame rate */
} AMBA_CVBS_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaCVBS.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaCVBS_Config(AMBA_CVBS_TV_SYSTEM_e TvSystem, AMBA_DSP_VOUT_DISPLAY_CVBS_CONFIG_s *pCustomConfig);
int AmbaCVBS_SetColorBar(UINT8 EnableFlag);
int AmbaCVBS_GetInfo(AMBA_CVBS_INFO_s *pCvbsInfo);

#endif /* _AMBA_CVBS_H_ */
