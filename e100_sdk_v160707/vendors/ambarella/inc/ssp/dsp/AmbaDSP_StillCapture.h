/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDSP_StillCapture.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella DSP Still Capture APIs
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DSP_STILL_CAPTURE_H_
#define _AMBA_DSP_STILL_CAPTURE_H_

#define A12_STILL_SSP

#include "AmbaDSP.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaDSP_StillCapture.c
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_JPEG_ENC_CONFIG_s_ {
    int     MaxNumQTable;                       /* maximum number of Q-tables */
    UINT8   (*pQTable)[128];                    /* pointer to Q-table array, size of each Q-table is 128 bytes */
    UINT8   *pBitsBufAddr;                      /* pointer to bitstream buffer */
    UINT32  BitsBufSize;                        /* bitstream buffer size */
} AMBA_JPEG_ENC_CONFIG_s;

int AmbaDSP_JpegEncConfig(AMBA_JPEG_ENC_CONFIG_s *pJpegEncConfig);
int AmbaDSP_JpegEncBitRateCtrl(AMBA_DSP_JPEG_BIT_RATE_CTRL_s *pBitRateCtrl);

typedef enum _AMBA_DSP_STILL_OP_MODE_e_ {
    AMBA_DSP_STILL_HIGH_ISO = 0,    /* high ISO */
    AMBA_DSP_STILL_LOW_ISO,         /* low ISO */
    AMBA_DSP_STILL_FAST_MODE,       /* fast mode */
    AMBA_DSP_STILL_MFHISO_MODE,     /* MultiFrame HISO mode */
    AMBA_DSP_STILL_NP_MODE,         /* NightPortrait mode */
    AMBA_DSP_STILL_MID_ISO_MODE,    /* mid ISO mode */
} AMBA_DSP_STILL_OP_MODE_e;

#define AMBA_DSP_STILL_CAPTURE_MAX_RAW_BUF_DEPTH    60

typedef struct _AMBA_DSP_STILL_CAPTURE_CONFIG_s_ {
    UINT8                       VcapSkipNum;           /* Number of skip frame before capture */
    AMBA_DSP_STILL_OP_MODE_e    OpMode;                /* capture operation mode */
    int                         RawBufDepth;           /* raw buffer depth */
    AMBA_DSP_RAW_BUF_s          *pRawBufAddr;          /* pointer to Raw buffers */
    UINT8                       CaptureVideoThumbnail; /* capture a thumbnail for the recorded video */
    AMBA_DSP_FRAME_RATE_s       FrameRate;             /* frame rate */
} AMBA_DSP_STILL_CAPTURE_CONFIG_s;

typedef struct _AMBA_DSP_QUICKVIEW_CONFIG_s_ {
    UINT8   Generate;                                       /* generate quickview */
    UINT8   FastQuickView;                                  /* generate small raw for fast quickview */
    AMBA_DSP_QUICKVIEW_YUV_IMG_BUF_s  *pYuvQuickViewLCD;    /* pointer to YUV buffer for LCD quickview */
    AMBA_DSP_QUICKVIEW_YUV_IMG_BUF_s  *pYuvQuickViewTV;     /* pointer to YUV buffer for TV quickview */
    AMBA_DSP_RAW_BUF_s      *pFastQuickRawBufAddr;          /* pointer to Raw buffer for Fast Quickview */
} AMBA_DSP_QUICKVIEW_CONFIG_s;

int AmbaDSP_StillCaptureConfig(AMBA_DSP_STILL_CAPTURE_CONFIG_s *pConfig);
int AmbaDSP_StillQuickviewConfig(AMBA_DSP_QUICKVIEW_CONFIG_s *pConfig);

int AmbaDSP_StillCaptureRaw(int NumRawPicture); /* capture RAW data */
int AmbaDSP_StillFreeOneRawPicture(void);

int AmbaDSP_StillGenerateRaw3A(AMBA_DSP_RAW_BUF_s *pRawBufAddr, AMBA_DSP_WINDOW_s *pRegionAddr);
int AmbaDSP_StillGenerateCleanRaw(AMBA_DSP_RAW_BUF_s *pRawBufAddr, AMBA_DSP_RAW_BUF_s *pCleanRawBufAddr);

typedef enum _AMBA_DSP_STILL_PIC_TYPE_e_ {
    AMBA_DSP_STILL_PIC_MAIN = 0,    /* main picture */
    AMBA_DSP_STILL_PIC_SCREENNAIL,  /* screennail */
    AMBA_DSP_STILL_PIC_THUMBNAIL,   /* thumbnail */
    
    AMBA_DSP_NUM_STILL_PIC_TYPE
} AMBA_DSP_STILL_PIC_TYPE_e;

typedef struct _AMBA_DSP_RAW2YUV_TILE_CONFIG_s_ {
    UINT8   NumTilesX;              /* number of tiles in horizontal direction */
    UINT8   NumTilesY;              /* number of tiles in vertical direction */
    UINT8   NumFramesSkip;          /* number of frames to skip */
} AMBA_DSP_RAW2YUV_TILE_CONFIG_s;

/*
   Configure Still Raw to Yuv Tile settings, ONLY for PIV at BackgroundProc = 0 Liveview mode.
*/
int AmbaDSP_StillRaw2YuvTileConfig(AMBA_DSP_RAW2YUV_TILE_CONFIG_s *pRaw2YuvTileConfig);

/*
   Generate 3 YUVs: main, QuickViewLCD and QuickViewTV
*/
int AmbaDSP_StillRaw2Yuv(AMBA_DSP_RAW_BUF_s *pRawBufAddr,
                         AMBA_DSP_YUV_IMG_BUF_s *pYuvBufAddr,
                         AMBA_DSP_QUICKVIEW_YUV_IMG_BUF_s *pWorkYuvBufAddr);

typedef struct _AMBA_DSP_STILL_SCRN_NAIL_BUF_s_ {
    UINT16  Width;                  /* screennail width */
    UINT16  Height;                 /* screennail height */
    UINT16  ActiveWidth;            /* screennail active width */
    UINT16  ActiveHeight;           /* screennail active height */
    UINT8   *pBufAddr;              /* pointer to the Screen Nail YUV buffer */
} AMBA_DSP_STILL_SCRN_NAIL_BUF_s;

int AmbaDSP_StillGenerateScrnNails(AMBA_DSP_YUV_IMG_BUF_s *pYuvBufAddr,
                                   int NumScrnNail,
                                   AMBA_DSP_STILL_SCRN_NAIL_BUF_s *pScrnNailBufAddr);

typedef struct _AMBA_DSP_JPEG_ENC_CTRL_s_ {
    AMBA_DSP_YUV_IMG_BUF_s *pYuvBuf;    /* pointer to source YUV buffer*/
    UINT8   QTableIdx;                  /* Q-table index */
    UINT16  EncWidth;                   /* Main JPEG width */
    UINT16  EncHeight;                  /* Main JPEG height */
    AMBA_DSP_ROTATE_FLIP_e RotateFlip;  /* rotate and flip setting */
} AMBA_DSP_JPEG_ENC_CTRL_s;

/* blocked API, generate AMBA_DSP_EVENT_JPEG_DATA_READY event for each JPEG */
int AmbaDSP_JpegEncode(int NumImage, AMBA_DSP_JPEG_ENC_CTRL_s *pJpegEncCtrl);

int AmbaDSP_StillYuvBlend(AMBA_DSP_YUV_IMG_BUF_s *pSrc1YuvBuf,
                          AMBA_DSP_YUV_IMG_BUF_s *pSrc2YuvBuf,
                          AMBA_DSP_BUF_s *pAlphaBuf,
                          AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBuf);

/* New APIs for A12 SSP */
/* Belowing are APIs which should only use in A12 or later,
 * To compensate what A9 SSP lacks of
 */
#ifdef A12_STILL_SSP
/* Definition */

/* Enumation */

/* Structure */
typedef struct _AMBA_DSP_STILL_RAW_CAPTURE_CONFIG_s_ {
    UINT8                       RawCaptureCntl; /* 0: SSP continue capture until reach RawCaptureNum, 1: User control raw capture timing */
    UINT32                      RawCaptureNum;  /* Total raw capture frames */
    UINT16                      YuvDepth;       /* Input Yuv buffer depth */
    AMBA_DSP_YUV_IMG_BUF_s      *pYuvBufAddr;   /* pointer to Yuv buffers */
    UINT16                      ScrnDepth;      /* Input Yuv buffer depth */
    AMBA_DSP_YUV_IMG_BUF_s      *pScrnBufAddr;  /* pointer to Screennail Yuv buffers */
    UINT16                      ThmbDepth;      /* Input Yuv buffer depth */
    AMBA_DSP_YUV_IMG_BUF_s      *pThmbBufAddr;  /* pointer to Thumbnail Yuv buffers */

    UINT16                      MainWidth;      /* Main width */
    UINT16                      MainHeight;     /* Main height */

    UINT16                      ScrnWidth;      /* Screennail width */
    UINT16                      ScrnHeight;     /* Screennail height */
    UINT16                      ScrnActWidth;   /* Screennail active width */
    UINT16                      ScrnActHeight;  /* Screennail active height */
    UINT16                      ThmbWidth;      /* Thumbnail width */
    UINT16                      ThmbHeight;     /* Thumbnail height */
    UINT16                      ThmbActWidth;   /* Thumbnail active width */
    UINT16                      ThmbActHeight;  /* Thumbnail active height */
    AMBA_DSP_ROTATE_FLIP_e      RotateFlip;     /* rotate and flip setting in advance */
} AMBA_DSP_STILL_RAW_CAPTURE_CONFIG_s;


typedef struct _AMBA_DSP_STILL_PROCESS_CONFIG_s_ {
    AMBA_DSP_STILL_OP_MODE_e      OpMode;         /* capture operation mode */
    UINT8                         InputFormat;    /* 0:CFA raw, 1: Yuv */
    UINT16                        InputDepth;     /* Input buffer depth */
    union {
        AMBA_DSP_RAW_BUF_s        *pRawBufAddr;   /* pointer to input Raw buffers */
        AMBA_DSP_YUV_IMG_BUF_s    *pYuvBufAddr;   /* pointer to input Yuv buffers */
    } Input;

    UINT8                         RawDataBits;    /* Raw data bits if input format = CFA */
    UINT8                         RawBayerPattern;/* Raw bayer pattern if input format = CFA */
    UINT8                         NumOfPivMainBlending; /* Number of OSD blending for Main */
    UINT8                         NumOfPivScrnBlending; /* Number of OSD blending for Scrn */
    UINT8                         NumOfPivThmBlending;  /* Number of OSD blending for Thm */

    UINT16                        MainWidth;      /* Main Yuv width */
    UINT16                        MainHeight;     /* Main Yuv height */
    UINT16                        ScrnActWidth;   /* Screennail active width */
    UINT16                        ScrnActHeight;  /* Screennail active height */
    UINT16                        ThmbActWidth;   /* Thumbnail active width */
    UINT16                        ThmbActHeight;  /* Thumbnail active height */
    UINT16                        ScrnDepth;      /* Screennail Yuv buffer depth */
    UINT16                        ThmbDepth;      /* Thumbnail Yuv buffer depth */
    AMBA_DSP_YUV_IMG_BUF_s        *pScrnBufAddr;  /* pointer to Screennail Yuv buffers */
    AMBA_DSP_YUV_IMG_BUF_s        *pThmbBufAddr;  /* pointer to Thumbnail Yuv buffers */
    AMBA_DSP_ROTATE_FLIP_e        RotateFlip;     /* rotate and flip setting in advance */
    AMBA_DSP_VIDEO_BLEND_CONFIG_s PivMainBlendConfig[AMBA_DSP_MAX_OSD_BLEND_AREA_NUMBER]; /* OSD blending config for Main */
    AMBA_DSP_VIDEO_BLEND_CONFIG_s PivScrnBlendConfig[AMBA_DSP_MAX_OSD_BLEND_AREA_NUMBER]; /* OSD blending config for Scrn */
    AMBA_DSP_VIDEO_BLEND_CONFIG_s PivThmBlendConfig[AMBA_DSP_MAX_OSD_BLEND_AREA_NUMBER]; /* OSD blending config for Thm */
} AMBA_DSP_STILL_PROCESS_CONFIG_s;

/* API */
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaDSP_StillRawCaptureConfig
 *
 *  @Description:: config still raw capture
 *
 *  @Input      ::
 *      pConfig : pointer to still raw capture configuration
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaDSP_StillRawCaptureConfig(AMBA_DSP_STILL_RAW_CAPTURE_CONFIG_s *pConfig);


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaDSP_StillProcessConfig
 *
 *  @Description:: config still process, use for conversion file comes from memeory instead of VIN
 *
 *  @Input      ::
 *      pConfig : pointer to still process configuration
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int AmbaDSP_StillProcessConfig(AMBA_DSP_STILL_PROCESS_CONFIG_s *pConfig);

typedef struct _AMBA_DSP_EXPRESS_IDSP_DEBUG_INFO_s_ {
    UINT8 DumpMode;         /* 0: video, 1: piv */
    UINT8 IdspSec2Enable;   /* Enable Section 2 dump */
    UINT8 IdspSec3Enable;   /* Enable Section 3 dump */
    UINT32 Sec2CfgBase;     /* Section 2 config dump pool base address */
    UINT32 Sec2CfgSize;     /* Dumped Section 2 single config size */
    UINT32 Sec3CfgBase;     /* Section 3 config dump pool base address */
    UINT32 Sec3CfgSize;     /* Dumped Section 3 single config size */
} AMBA_DSP_EXPRESS_IDSP_DEBUG_INFO_s;

typedef struct _AMBA_DSP_EXPRESS_IDSP_CFG_INFO_s_ {
    UINT32 Reserved0;
    UINT8 TileNbrX;
    UINT8 TileNbrY;
    UINT8 TileNbrMaxX;
    UINT8 TileNbrMaxY;
    UINT16 InDmaStartX;
    UINT16 InDmaStartY;
    UINT16 InDmaX;
    UINT16 InDmaY;
    UINT16 OutDmaStartX;
    UINT16 OutDmaStartY;
    UINT16 OutDmaX;
    UINT16 OutDmaY;
    UINT8 IdspMode;         // 0: section1(VIN), 1: section2(Main), 2: section3(Vwarp), 3: section4(PreBlend), 4: section5(PreviewA)...
    UINT8 Reserved1;
    UINT16 IdspCfgSize;
    UINT32 IdspCfgAddr;
} AMBA_DSP_EXPRESS_IDSP_CFG_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaDSP_ExpressIDSPDbg
 *
 *  @Description:: set IDSP dump info for express mode video or PIV
 *
 *  @Input      ::
 *      pIdspDbgInfo : pointer to debug info command
 *
 *  @Output     ::
 *      pIdspCfg : pointer to output array
 *
 *  @Return     ::
 *          int : NG(-1)/Number of used AMBA_DSP_EXPRESS_IDSP_CFG_INFO_s
\*-----------------------------------------------------------------------------------------------*/
extern int AmbaDSP_ExpressIDSPDbg(AMBA_DSP_EXPRESS_IDSP_DEBUG_INFO_s *pIdspDbgInfo, AMBA_DSP_EXPRESS_IDSP_CFG_INFO_s *pIdspCfg);

/*-----------------------------------------------------------------------------------------------*\
*  @RoutineName:: AmbaDSP_StillRawCaptureMonitorGetCurrentInfo 265
*  266
*  @Description:: Get current raw info based on capture raw count  267
*  268
*  @Input      ::  269
*      rawCount : query which raw's info   270
*      rawAddr  : return raw address based on raw count    271
*      rawPitch : return raw pitch based on raw count  272
*  273
*  @Output     :: none 274
*  275
*  @Return     ::  276
*          int : OK(0)/NG(-1)  277
\*-----------------------------------------------------------------------------------------------*/
extern int AmbaDSP_StillRawCaptureMonitorGetCurrentInfo(UINT32 rawCount, UINT32 *rawAddr, UINT32 *rawPitch, UINT32 *currRow);

#endif

#endif  /* _AMBA_DSP_STILL_CAPTURE_H_ */
