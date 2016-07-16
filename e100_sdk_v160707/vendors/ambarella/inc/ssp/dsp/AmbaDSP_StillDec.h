/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDSP_StillDec.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella DSP Still Picture Decoder APIs
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DSP_STILL_DEC_H_
#define _AMBA_DSP_STILL_DEC_H_

#include "AmbaDSP.h"

typedef enum _AMBA_DSP_STILL_DEC_STATUS_e_ {
    AMBA_DSP_STILL_DEC_DONE = 0,        /* decode done */
    AMBA_DSP_STILL_DEC_PENDING_BITS,    /* pending on input bits */
    AMBA_DSP_STILL_DEC_JPEG_HEADER_ERR, /* jpeg header error */
    AMBA_DSP_STILL_DEC_JPEG_MCU_ERR,    /* jpeg mcu error */
    AMBA_DSP_STILL_DEC_H264_DEC_ERR,    /* h264 picture error */
} AMBA_DSP_STILL_DEC_STATUS_e;

typedef enum _AMBA_DSP_STILL_DEC_RGB_FORMAT_e_ {
    AMBA_DSP_STILL_DEC_ABGR8888 = 0,    /* Alpha is the MSB */
    AMBA_DSP_STILL_DEC_ARGB8888 = 1,
} AMBA_DSP_STILL_DEC_RGB_FORMAT_e;

typedef struct _AMBA_DSP_STILL_DEC_BUF_CONFIG_s_ {
    UINT32  ImgDataFmt;                 /* Image data format: 0 - JPEG; 1 - H.264 I-Frame */
    UINT32  BitsDataSize;               /* Size of the bitstream */
    UINT8   *pBitsDataAddr;             /* Start address of the bitstream */

    UINT32  MaxYuvBufSize;              /* Maximum size of YUV buffer */
    UINT8   *pYuvBufAddr;               /* Start address of YUV buffer */

    struct {
        UINT16  Width;                  /* output from DSP/uCode: Image width */
        UINT16  Height;                 /* output from DSP/uCode: Image Height */
        UINT16  YuvPitch;               /* output from DSP/uCode: YUV data Pitch */
        UINT16  YuvFormat;              /* output from DSP/uCode: YUV data format */
        UINT32  Status;                 /* output from DSP/uCode: OK/NG/Error Code. Please see AMBA_DSP_STILL_DEC_STATUS_e */
        UINT32  YAddr;                  /* output from DSP/uCode: decode result Y */
        UINT32  UVAddr;                 /* output from DSP/uCode: decode result UV */        
    } DecInfo;
} AMBA_DSP_STILL_DEC_BUF_CONFIG_s;

typedef struct _AMBA_DSP_YUV2YUV_OPERATION_s_ {
    UINT8   RotateFlip;                 /* rotate and flip setting  */
    UINT8   LumaGain;                   /* luma gain: 1 ~ 255 */
} AMBA_DSP_YUV2YUV_OPERATION_s;

typedef struct _AMBA_DSP_YUV2RGB_CONFIG_s_ {
    UINT16  Matrix[9];
    INT16   ROffset;
    INT16   GOffset;
    INT16   BOffset;
    AMBA_DSP_STILL_DEC_RGB_FORMAT_e   RgbDataFormat; /* RGB output format */
} AMBA_DSP_YUV2RGB_CONFIG_s;

typedef struct _AMBA_DSP_BLEND_OPERATION_s_ {
    UINT8   *pAlphaMap;                 /* alpha map */
    UINT8   GlobalAlpha;                /* Use Global alpha value if pAlphaMap = NULL */
} AMBA_DSP_BLEND_OPERATION_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaDSP_StillDec.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaDSP_StillDecConfig(int ImgDataFmt, UINT32 MaxYuvBufSize, UINT8 *pYuvBufAddr);
int AmbaDSP_StillDecSetBitsDataBlk(int LastBlkFlag, UINT32 BlkDataSize, UINT8 *pBlkDataAddr);
int AmbaDSP_StillDecAbort(void);

int AmbaDSP_StillDecode(int NumImg, AMBA_DSP_STILL_DEC_BUF_CONFIG_s *pDecBufConfig);

int AmbaDSP_StillDecClearYuvBuf(AMBA_DSP_YUV_IMG_BUF_s *pYuvBufAddr, UINT32 FillYuvColor);
int AmbaDSP_StillDecYuv2Yuv(int NumImg, AMBA_DSP_YUV_IMG_BUF_s *pSrcYuvBufAddr,
                            AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                            AMBA_DSP_YUV2YUV_OPERATION_s *pOperation);

int AmbaDSP_StillDecYuvBlend(AMBA_DSP_YUV_IMG_BUF_s *pSrc1YuvBufAddr,
                             AMBA_DSP_YUV_IMG_BUF_s *pSrc2YuvBufAddr,
                             AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                             AMBA_DSP_BLEND_OPERATION_s *pOperation);

int AmbaDSP_StillDecYuv2RgbConfig(AMBA_DSP_YUV2RGB_CONFIG_s *pYuv2RgbConfig);
int AmbaDSP_StillDecYuv2Rgb(AMBA_DSP_YUV_IMG_BUF_s *pSrcYuvBufAddr, AMBA_DSP_BUF_s *pDestRgbBufAddr);

int AmbaDSP_StillDecDispYuvImg(AMBA_DSP_VOUT_IDX_e VoutIdx,
                               AMBA_DSP_YUV_IMG_BUF_s *pYuvBufAddr,
                               AMBA_DSP_VOUT_VIDEO_CONFIG_s *pVoutConfig);

#endif  /* _AMBA_DSP_STILL_DEC_H_ */
