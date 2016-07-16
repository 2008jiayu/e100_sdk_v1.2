/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaGDMA_Def.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for GDMA APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_GDMA_DEF_H_
#define _AMBA_GDMA_DEF_H_

#define AMBA_GDMA_NUM_INSTANCE      8
#define AMBA_GDMA_MAX_WIDTH         8192
#define AMBA_GDMA_MAX_HEIGHT        4096

typedef enum _AMBA_GDMA_PIXEL_FORMAT_e_ {
    AMBA_GDMA_8_BIT = 0,                    /* Lookup */
    AMBA_GDMA_4_BIT,                        /* Lookup */
    AMBA_GDMA_16_BIT_1555,
    AMBA_GDMA_16_BIT_565,
    AMBA_GDMA_24_BIT,
    AMBA_GDMA_32_BIT,
    
    AMBA_NUM_GDMA_PIXEL_FORMAT              /* Total Number of GDMA Pixel Formats */
} AMBA_GDMA_PIXEL_FORMAT_e;

typedef struct _AMBA_GDMA_LINEAR_CTRL_s_ {
    AMBA_GDMA_PIXEL_FORMAT_e PixelFormat;
    int     NumPixels;                      /* Nnumber of pixels */
    void    *pSrc;                          /* Source address */
    void    *pDest;                         /* Destination address */
} AMBA_GDMA_LINEAR_CTRL_s;

typedef struct _AMBA_GDMA_BLOCK_CTRL_s_ {
    AMBA_GDMA_PIXEL_FORMAT_e PixelFormat;
    UINT16  Width;                          /* Width of the Block in pixels */
    UINT16  Height;                         /* Height of the Block in rows */
    UINT16  PitchSrc;                       /* Pitch in bytes of Source */
    UINT16  PitchDest;                      /* Pitch in bytes of Destination */
    void    *pSrc;                          /* Source address */
    void    *pDest;                         /* Destination address */
} AMBA_GDMA_BLOCK_CTRL_s;

#endif /* _AMBA_GDMA_DEF_H_ */
