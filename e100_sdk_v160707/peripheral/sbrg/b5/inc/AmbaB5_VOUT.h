/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaB5_VOUT.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for B5 VOUT Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_B5_VOUT_H_
#define _AMBA_B5_VOUT_H_

typedef struct _AMBA_B5_VOUT_CONFIG_s_ {
    AMBA_B5_CHANNEL_CONFIG_e    Channel[AMBA_NUM_B5_CHANNEL];
    AMBA_B5_COMPRESS_RATIO_e    CompressRatio;
    UINT16  InputLinePixel;
    UINT16  OutputFrameLine;
    UINT8   NumDataLane;
    UINT8   PixelWidth;
    UINT16  MaxHBlank;      /* 14-bit */
    UINT16  MinHBlank;      /* 10-bit */
    UINT8   MinVBlank;      /* 8-bit */
    UINT8   Reserved;
} AMBA_B5_VOUT_CONFIG_s;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB5_VOUT.c
\*---------------------------------------------------------------------------*/
int AmbaB5_VoutReset(void);
int AmbaB5_VoutClear(void);
int AmbaB5_VoutConfig(AMBA_B5_VOUT_CONFIG_s *pVoutConfig);
int AmbaB5_VoutEnable(void);
int AmbaB5_VoutDisable(void);
int AmbaB5_VoutGetLastFrameTimeStamp(UINT32 *pTimeStamp0, UINT32 *pTimeStamp1, UINT32 *pTimeStamp2, UINT32 *pTimeStamp3);

#endif  /* _AMBA_B5_VOUT_H_ */
