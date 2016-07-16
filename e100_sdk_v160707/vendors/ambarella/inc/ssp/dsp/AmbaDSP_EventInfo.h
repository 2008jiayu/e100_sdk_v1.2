/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDSP_EventInfo.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella DSP Event info
 *
\*-------------------------------------------------------------------------------------------------------------------*/


#ifndef _AMBA_DSP_EVENT_INFO_H_
#define _AMBA_DSP_EVENT_INFO_H_

/*-----------------------------------------------------------------------------------------------*\
 * Event info structure for AMBA_DSP_EVENT_CFA_3A_DATA_READY and
 *                          AMBA_DSP_EVENT_RAW_CFA_3A_DATA_READY event
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_DSP_3A_AWB_TILE_COLUMN_COUNT       32
#define AMBA_DSP_3A_AWB_TILE_ROW_COUNT          32
#define AMBA_DSP_3A_AE_TILE_COLUMN_COUNT        12
#define AMBA_DSP_3A_AE_TILE_ROW_COUNT           8
#define AMBA_DSP_3A_AF_TILE_COLUMN_COUNT        12
#define AMBA_DSP_3A_AF_TILE_ROW_COUNT           8
#define AMBA_DSP_3A_FLOAT_TILE_COUNT            32

#define AMBA_DSP_STILL_3A_AWB_TILE_COLUMN_COUNT 32
#define AMBA_DSP_STILL_3A_AWB_TILE_ROW_COUNT    32
#define AMBA_DSP_STILL_3A_AE_TILE_COLUMN_COUNT  12
#define AMBA_DSP_STILL_3A_AE_TILE_ROW_COUNT     8
#define AMBA_DSP_STILL_3A_AF_TILE_COLUMN_COUNT  12
#define AMBA_DSP_STILL_3A_AF_TILE_ROW_COUNT     8

#ifdef __GNUC__
#undef __packed
#define __packed
#define __PACKED __attribute__((packed))
#else
#define __PACKED
#endif

typedef __packed struct _AMBA_DSP_3A_HEADER_s_ {
    UINT16  AwbTileColStart;
    UINT16  AwbTileRowStart;
    UINT16  AwbTileWidth;
    UINT16  AwbTileHeight;
    UINT16  AwbTileActiveWidth;
    UINT16  AwbTileActiveHeight;
    UINT16  AwbRgbShift;
    UINT16  AwbYShift;
    UINT16  AwbMinMaxShift;
    UINT16  AeTileColStart;
    UINT16  AeTileRowStart;
    UINT16  AeTileWidth;
    UINT16  AeTileHeight;
    UINT16  AeYShift;
    UINT16  AeLinearYShift;
    UINT16  AeMinMaxShift;
    UINT16  AfTileColStart;
    UINT16  AfTileRowStart;
    UINT16  AfTileWidth;
    UINT16  AfTileHeight;
    UINT16  AfTileActiveWidth;
    UINT16  AfTileActiveHeight;
    UINT16  AfYShift;
    UINT16  AfCfaYShift;
    UINT8   AwbTileNumCol;
    UINT8   AwbTileNumRow;
    UINT8   AeTileNumCol;
    UINT8   AeTileNumRow;
    UINT8   AfTileNumCol;
    UINT8   AfTileNumRow;
    UINT16  Rederved0;
    UINT32  Reserved1[5];
    UINT32  RawPicSeqNum;       //20
    UINT32  IsoCfgTag;          //21
    UINT32  ChannelId:2;        //22 sync with new cmd_msg
    UINT32  Reserved:30;        //22
    UINT32  Reserved2[10];      //Padding to 128 bytes
}__PACKED AMBA_DSP_3A_HEADER_s;

typedef __packed struct _AMBA_DSP_CFA_AWB_s_ {
    UINT16  SumR;
    UINT16  SumG;
    UINT16  SumB;
    UINT16  CountMin;
    UINT16  CountMax;
}__PACKED AMBA_DSP_CFA_AWB_s;

typedef __packed struct _AMBA_DSP_CFA_AE_s_ {
    UINT16  LinY;
    UINT16  CountMin;
    UINT16  CountMax;
}__PACKED AMBA_DSP_CFA_AE_s;

typedef __packed struct _AMBA_DSP_CFA_AF_s_ {
    UINT16  SumY;
    UINT16  SumFV1;
    UINT16  SumFV2;
}__PACKED AMBA_DSP_CFA_AF_s;

typedef __packed struct _AMBA_DSP_CFA_HISTO_s_ {
    UINT32  HisBinR[64];
    UINT32  HisBinG[64];
    UINT32  HisBinB[64];
    UINT32  HisBinY[64];
}__PACKED AMBA_DSP_CFA_HISTO_s;

typedef __packed struct _AMBA_DSP_CFA_FLOAT_AWB_s_ {
    UINT32  SumR;
    UINT32  SumG;
    UINT32  SumB;
    UINT32  CountMin;
    UINT32  CountMax;
}__PACKED AMBA_DSP_CFA_FLOAT_AWB_s;

typedef __packed struct _AMBA_DSP_CFA_FLOAT_AE_s_ {
    UINT32  LinY;
    UINT32  CountMin;
    UINT32  CountMax;
}__PACKED AMBA_DSP_CFA_FLOAT_AE_s;

typedef __packed struct _AMBA_DSP_CFA_FLOAT_AF_s_ {
    UINT32  SumFV1;
    UINT32  SumFV2;
}__PACKED AMBA_DSP_CFA_FLOAT_AF_s;

typedef __packed struct _AMBA_DSP_EVENT_CFA_3A_DATA_s_ {
    AMBA_DSP_3A_HEADER_s      Header;
    UINT16                    FrameId;
    AMBA_DSP_CFA_AWB_s        Awb[AMBA_DSP_3A_AWB_TILE_ROW_COUNT * AMBA_DSP_3A_AWB_TILE_COLUMN_COUNT];
    AMBA_DSP_CFA_AE_s         Ae[AMBA_DSP_3A_AE_TILE_ROW_COUNT * AMBA_DSP_3A_AE_TILE_COLUMN_COUNT];
    AMBA_DSP_CFA_AF_s         Af[AMBA_DSP_3A_AF_TILE_ROW_COUNT * AMBA_DSP_3A_AF_TILE_COLUMN_COUNT];
    AMBA_DSP_CFA_HISTO_s      Histo;
    UINT8                     Reserved[126];    /* Padding to 128x bytes */
}__PACKED AMBA_DSP_EVENT_CFA_3A_DATA_s;

typedef __packed struct _AMBA_DSP_EVENT_STILL_CFA_3A_DATA_s_ {
    AMBA_DSP_3A_HEADER_s      Header;
    UINT16                    FrameId;
    AMBA_DSP_CFA_AWB_s        Awb[AMBA_DSP_STILL_3A_AWB_TILE_ROW_COUNT * AMBA_DSP_STILL_3A_AWB_TILE_COLUMN_COUNT];
    AMBA_DSP_CFA_AE_s         Ae[AMBA_DSP_STILL_3A_AE_TILE_ROW_COUNT * AMBA_DSP_STILL_3A_AE_TILE_COLUMN_COUNT];
    AMBA_DSP_CFA_AF_s         Af[AMBA_DSP_STILL_3A_AF_TILE_ROW_COUNT * AMBA_DSP_STILL_3A_AF_TILE_COLUMN_COUNT];
    AMBA_DSP_CFA_HISTO_s      Histo;
    UINT8                     Reserved[126];    /* Padding to 128x bytes */
}__PACKED AMBA_DSP_EVENT_STILL_CFA_3A_DATA_s;

/*-----------------------------------------------------------------------------------------------*\
 * Event info structure for AMBA_DSP_EVENT_RGB_3A_DATA_READY and
 *                          AMBA_DSP_EVENT_RAW_RGB_3A_DATA_READY event
\*-----------------------------------------------------------------------------------------------*/
typedef __packed struct _AMBA_DSP_RGB_AE_s_ {
    UINT16  SumY;
}__PACKED AMBA_DSP_RGB_AE_s;

typedef __packed struct _AMBA_DSP_RGB_FLOAT_AE_s_ {
    UINT32  SumY;
}__PACKED AMBA_DSP_RGB_FLOAT_AE_s;

typedef __packed struct _AMBA_DSP_RGB_HISTO_s_ {
    UINT32  HisBinY[64];
    UINT32  HisBinR[64];
    UINT32  HisBinG[64];
    UINT32  HisBinB[64];
}__PACKED AMBA_DSP_RGB_HISTO_s;

typedef __packed struct _AMBA_DSP_RGB_FLOAT_AF_s_ {
    UINT32  SumFV1H;
    UINT32  SumFV2H;
    UINT32  SumFV1V;
    UINT32  SumFV2V;
}__PACKED AMBA_DSP_RGB_FLOAT_AF_s;

typedef __packed struct _AMBA_DSP_RGB_AF_s_ {
    UINT16  SumFY;
    UINT16  SumFV1;
    UINT16  SumFV2;
}__PACKED AMBA_DSP_RGB_AF_s;

typedef __packed struct _AMBA_DSP_EVENT_RGB_3A_DATA_s_ {
    AMBA_DSP_3A_HEADER_s     Header;
    UINT16                   FrameId;
    AMBA_DSP_RGB_AF_s        Af[AMBA_DSP_3A_AF_TILE_ROW_COUNT * AMBA_DSP_3A_AF_TILE_COLUMN_COUNT];
    AMBA_DSP_RGB_AE_s        Ae[AMBA_DSP_3A_AE_TILE_ROW_COUNT * AMBA_DSP_3A_AE_TILE_COLUMN_COUNT];
    AMBA_DSP_RGB_HISTO_s     Histo;
    UINT8                    Reserved[254];     /* Padding to 128x bytes */
}__PACKED AMBA_DSP_EVENT_RGB_3A_DATA_s;

typedef __packed struct _AMBA_DSP_EVENT_STILL_RGB_3A_DATA_s_ {
    AMBA_DSP_3A_HEADER_s     Header;
    UINT16                   FrameId;
    AMBA_DSP_RGB_AF_s        Af[AMBA_DSP_STILL_3A_AF_TILE_ROW_COUNT * AMBA_DSP_STILL_3A_AF_TILE_COLUMN_COUNT];
    AMBA_DSP_RGB_AE_s        Ae[AMBA_DSP_STILL_3A_AE_TILE_ROW_COUNT * AMBA_DSP_STILL_3A_AE_TILE_COLUMN_COUNT];
    AMBA_DSP_RGB_HISTO_s     Histo;
    AMBA_DSP_RGB_FLOAT_AF_s  FloatAf[AMBA_DSP_3A_FLOAT_TILE_COUNT];
    AMBA_DSP_RGB_FLOAT_AE_s  FloatAe[AMBA_DSP_3A_FLOAT_TILE_COUNT];
    UINT8                    Reserved[206];     /* Padding to 128x bytes */
}__PACKED AMBA_DSP_EVENT_STILL_RGB_3A_DATA_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Event info structure for AMBA_DSP_EVENT_VIDEO_DATA_READY and
 *                           AMBA_DSP_EVENT_JPEG_DATA_READY event
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_DSP_PIC_FRAME_TYPE_e_ {
    AMBA_DSP_PIC_FRAME_TYPE_MJPEG = 0,
    AMBA_DSP_PIC_FRAME_TYPE_IDR = 1,
    AMBA_DSP_PIC_FRAME_TYPE_I = 2,
    AMBA_DSP_PIC_FRAME_TYPE_P = 3,
    AMBA_DSP_PIC_FRAME_TYPE_B = 4,
    AMBA_DSP_PIC_FRAME_TYPE_JPG_MAIN = 5,
    AMBA_DSP_PIC_FRAME_TYPE_JPG_THMB = 6,
    AMBA_DSP_PIC_FRAME_TYPE_JPG_SCRN = 7,
    AMBA_DSP_PIC_FRAME_TYPE_NUMBER = 0xFFFFFFFF,
} AMBA_DSP_PIC_FRAME_TYPE_e;

typedef struct _AMBA_DSP_EVENT_ENC_PIC_READY_s_ {
    UINT32  ChannelId;
    UINT32  StreamId;
    UINT32  FrmNo;
    UINT64  PTS;
    UINT32  StartAddr;
    UINT32  FrameType;
    UINT32  PicStruct;
    UINT32  PicSize;
}__PACKED AMBA_DSP_EVENT_ENC_PIC_READY_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Event info structure for AMBA_DSP_EVENT_VIDEO_ENC_START
 *                           AMBA_DSP_EVENT_VIDEO_ENC_PAUSE
 *                           AMBA_DSP_EVENT_VIDEO_ENC_RESUME
 *                           AMBA_DSP_EVENT_VIDEO_ENC_STOP event
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s_ {
    UINT32  ChannelId;
    UINT32  StreamId;
}__PACKED AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Event info structure for AMBA_DSP_EVENT_LIVEVIEW_LCD_YUV_DATA_READY and
 *                           AMBA_DSP_EVENT_LIVEVIEW_TV_YUV_DATA_READY and
 *                           AMBA_DSP_EVENT_VCAP_YUV_DATA_READY
 *                           AMBA_DSP_EVENT_SCAP_YUV_DATA_READY
 *
 *  AMBA_DSP_YUV_IMG_BUF_s (Defined in AmbaDSP.h )
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  Event info structure for AMBA_DSP_EVENT_SCAP_RAW_DATA_READY event
 *
 *  AMBA_DSP_RAW_BUF_s (Defined in AmbaDSP.h )
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  Event info structure for AMBA_DSP_EVENT_JPEG_DEC_STATUS_REPORT event
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_DSP_EVENT_JPEG_DEC_STATUS_REPORT_s_ {
    UINT16  Width;
    UINT16  Height;
    UINT16  YuvPitch;
    UINT16  YuvFormat;
    UINT32 Status;
}__PACKED AMBA_DSP_EVENT_JPEG_DEC_STATUS_REPORT_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Event info structure for AMBA_DSP_EVENT_JPEG_DEC_YUV_DISP_REPORT event
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_DSP_EVENT_JPEG_DEC_YUV_DISP_REPORT_s_ {
    struct {
        UINT32  VoutUpdated;
        UINT32  VoutYAddr;
        UINT32  VoutUVAddr;
    } DispInfo[2];
}__PACKED AMBA_DSP_EVENT_JPEG_DEC_YUV_DISP_REPORT_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Event info structure for AMBA_DSP_EVENT_H264_DEC_STATUS_REPORT event
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_DSP_DEC_STATE_e_ {
    AMBA_DSP_DEC_STATE_INVALID = 0,
    AMBA_DSP_DEC_STATE_IDLE = 1,
    AMBA_DSP_DEC_STATE_RUN = 2,
    AMBA_DSP_DEC_STATE_IDLE_WITH_LAST_PIC = 3,
}__PACKED AMBA_DSP_DEC_STATE_e;

typedef struct _AMBA_DSP_EVENT_H264_DEC_STATUS_UPDATE_s_ {
    UINT32                DecoderId;
    AMBA_DSP_DEC_STATE_e  DecodeState;
    UINT32                ErrorStatus;
    UINT32                NumOfDecodedPic;
    UINT32                BitsNextReadAddr;
    UINT32                DisplayFramePTS;
    UINT8                 EndOfStream;
}__PACKED AMBA_DSP_EVENT_H264_DEC_STATUS_UPDATE_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Event info structure for AMBA_DSP_EVENT_ISO_CFG_COMPLETE/AMBA_DSP_EVENT_HDR_PROC_COMPLETE event
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_CFG_UPDATE_INFO_s_ {
    UINT32  ChannleID;
    UINT32  LockedCfgAddr;
} AMBA_CFG_UPDATE_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Event info structure for AMBA_DSP_EVENT_ERROR
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_DSP_ERROR_INFO_s_ {
    UINT32 ChannelID;
    UINT32 ErrorCode;
} AMBA_DSP_ERROR_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Event info structure for AMBA_DSP_VIN_STATUS_TILE_CONFIG_INFO_s
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_DSP_VIN_STATUS_TILE_CONFIG_INFO_s_ {
    UINT8  VinStatsType;    // 0: main; 1: hdr
    UINT8  Reserved0;
    UINT8  TotalExposures;
    UINT8  BlendIndex;     // exposure no.

    UINT32 Reserved1;

    UINT16 StatsLeft;
    UINT16 StatsWidth;

    UINT16 StatsTop;
    UINT16 StatsHeight;

    UINT32 Reserved2[28];
} AMBA_DSP_VIN_STATUS_TILE_CONFIG_INFO_s;

typedef struct _AMBA_DSP_HDR_HIST_STAT_s_ {
    UINT32 HistoBinR[128];
    UINT32 HistoBinG[128];
    UINT32 HistoBinB[128];
} AMBA_DSP_HDR_HIST_STAT_s;

typedef struct _AMBA_DSP_EVENT_VIN_STATS_INFO_s_ {
    AMBA_DSP_VIN_STATUS_TILE_CONFIG_INFO_s Header;
    AMBA_DSP_HDR_HIST_STAT_s Data;
} AMBA_DSP_EVENT_VIN_STATS_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Event info structure for AMBA_LL_EVENT_JPEG_DEC_STATUS_REPORT
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_DSP_EVENT_JPEG_DEC_COMMON_BUFFER_REPORT_s_ {
    UINT32  FbpAddr;
    UINT32  BufferNum;
    UINT32  BufferSize;
    UINT16  ImgPitch;
    UINT16  ImgWidth;
    UINT16  ImgHeight;
    UINT8   CurrFbId;
} AMBA_DSP_EVENT_JPEG_DEC_COMMON_BUFFER_REPORT_s;


#endif  /* _AMBA_DSP_EVENT_INFO_H_ */
