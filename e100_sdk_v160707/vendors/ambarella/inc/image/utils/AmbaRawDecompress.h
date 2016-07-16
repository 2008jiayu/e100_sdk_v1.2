/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRawDecompress.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella RAW decompression.
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RAW_DECOMPRESS_H_
#define _AMBA_RAW_DECOMPRESS_H_

typedef struct _AMBA_RAW_COMPRESS_IMG_BUF_s_ {
    UINT8   *pBaseAddr;                 /* pointer to compressed image buffer */
    UINT32  Pitch;                      /* stride of compressed image buffer */
    UINT16  Width;                      /* image width */
    UINT16  Height;                     /* image height */
} AMBA_RAW_COMPRESS_IMG_BUF_s;

typedef struct _AMBA_RAW_DECOMPRESS_IMG_BUF_s_ {
    UINT8   *pBaseAddr;                 /* pointer to decompressed image buffer */
    UINT32  Pitch;                      /* stride of decompressed image buffer */
    UINT16  OffsetX;                    /* horizontal position to put decompressed image */
    UINT16  OffsetY;                    /* vertical position to put decompressed image */

    AMBA_DSP_WINDOW_s   CropWin;        /* crop info of the compressed image buffer */
} AMBA_RAW_DECOMPRESS_IMG_BUF_s;

int AmbaRawDecompress(AMBA_RAW_COMPRESS_IMG_BUF_s *pRawBufInfo, int NumCropWin, AMBA_RAW_DECOMPRESS_IMG_BUF_s *pCropBufInfo);

#endif /* _AMBA_RAW_DECOMPRESS_H_ */
