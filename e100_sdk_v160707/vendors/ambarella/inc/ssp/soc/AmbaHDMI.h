/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaHDMI.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for HDMI Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_HDMI_H_
#define _AMBA_HDMI_H_

#include "AmbaHDMI_Def.h"
#include "AmbaEDID.h"
#include "AmbaDSP_VOUT.h"

/*-----------------------------------------------------------------------------------------------*\
 * HDMI Vendor-Specific Data Block (HDMI VSDB)
\*-----------------------------------------------------------------------------------------------*/
typedef enum _HDMI_VSDB_3D_STRUCTURE_e_ {
    HDMI_VSDB_3D_STRUCT_FRAME_PACKING                   = 0x0001,   /* Frame packing */
    HDMI_VSDB_3D_STRUCT_FIELD_ALTERNATIVE               = 0x0002,   /* Field alternative */
    HDMI_VSDB_3D_STRUCT_LINE_ALTERNATIVE                = 0x0004,   /* Line alternative */
    HDMI_VSDB_3D_STRUCT_SIDE_BY_SIDE_FULL               = 0x0008,   /* Left-and-Right with full resolution */
    HDMI_VSDB_3D_STRUCT_L_DEPTH                         = 0x0010,   /* L + depth */
    HDMI_VSDB_3D_STRUCT_L_DEPTH_GFX_GFX_DEPTH           = 0x0020,   /* L + depth + Graphics + Graphics-depth */
    HDMI_VSDB_3D_STRUCT_TOP_AND_BOTTOM                  = 0x0040,   /* Top-and-Bottom with half vertical resolution */
    HDMI_VSDB_3D_STRUCT_SIDE_BY_SIDE_HALF_HORIZONTAL    = 0x0100,   /* Left-and-Right with horizontal sub-sampling */
    HDMI_VSDB_3D_STRUCT_SIDE_BY_SIDE_HALF_QUINCUNX      = 0x8000,   /* Side-by-Side with any quincunx sub-sampling matrix */
    HDMI_VSDB_3D_STRUCT_SIDE_BY_SIDE_HALF_DETAIL        = 0xFF00,   /* Left-and-Right with half resolution by 3D_Detail */
} HDMI_VSDB_3D_STRUCTURE_e;

typedef enum _HDMI_VSDB_3D_DETAIL_e_ {
    HDMI_VSDB_3D_DETAIL_ALL                             = 0x0,      /* All of the horizontal and qunicunx sub-sampling matrix */
    HDMI_VSDB_3D_DETAIL_HORIZONTAL                      = 0x1,      /* Horizontal sub-sampling */
    HDMI_VSDB_3D_DETAIL_QUINCUNX_ALL                    = 0x6,      /* Quincunx: all combination of sub-sampling position */
    HDMI_VSDB_3D_DETAIL_QUINCUNX_LO_RO                  = 0x7,      /* Quincunx: Odd/Left picture, Odd/Right picture */
    HDMI_VSDB_3D_DETAIL_QUINCUNX_LO_RE                  = 0x8,      /* Quincunx: Odd/Left picture, Even/Right picture */
    HDMI_VSDB_3D_DETAIL_QUINCUNX_LE_RO                  = 0x9,      /* Quincunx: Even/Left picture, Odd/Right picture */
    HDMI_VSDB_3D_DETAIL_QUINCUNX_LE_RE                  = 0xA       /* Quincunx: Even/Left picture, Even/Right picture */
} HDMI_VSDB_3D_DETAIL_e;

/*-----------------------------------------------------------------------------------------------*\
 * HDMI Audio Sampling Rate Support
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_HDMI_AUDIO_SAMPLE_RATE_e_ {
    AMBA_HDMI_AUDIO_FS_32K                              = 0x01,     /* Sample rate =  32000 */
    AMBA_HDMI_AUDIO_FS_44P1K                            = 0x02,     /* Sample rate =  44100 */
    AMBA_HDMI_AUDIO_FS_48K                              = 0x04,     /* Sample rate =  48000 */
    AMBA_HDMI_AUDIO_FS_88P2K                            = 0x08,     /* Sample rate =  88200 */
    AMBA_HDMI_AUDIO_FS_96K                              = 0x10,     /* Sample rate =  96000 */
    AMBA_HDMI_AUDIO_FS_176P4K                           = 0x20,     /* Sample rate = 176400 */
    AMBA_HDMI_AUDIO_FS_192K                             = 0x40      /* Sample rate = 192000 */
} AMBA_HDMI_AUDIO_SAMPLE_RATE_e;

typedef enum _AMBA_HDMI_LPCM_DETAIL_e_ {
    AMBA_HDMI_LPCM_16BIT                                = 0x01,     /* 16-bit L-PCM */
    AMBA_HDMI_LPCM_20BIT                                = 0x02,     /* 20-bit L-PCM */
    AMBA_HDMI_LPCM_24BIT                                = 0x04      /* 24-bit L-PCM */
} AMBA_HDMI_LPCM_DETAIL_e;

typedef struct _AMBA_HDMI_AUDIO_DESCRIPTOR_s_ {
    UINT8  MaxNumChan;                                  /* Max Number of channels */
    UINT8  SampleRate;                                  /* Sample rate support */
    UINT8  Detail;                                      /* Detailed information associated to each Audio Format Code */
    UINT8  Reserved;                                    /* Reserved */
} AMBA_HDMI_AUDIO_DESCRIPTOR_s;

/*-----------------------------------------------------------------------------------------------*\
 * Product Description Structure
\*-----------------------------------------------------------------------------------------------*/
typedef enum _CEA861_SOURCE_INFORMATION_e_ {
    CEA861_SOURCE_UNKNOWN = 0x00,                       /* Unknown */
    CEA861_SOURCE_DIGITAL_STB,                          /* Digital STB */
    CEA861_SOURCE_DVD,                                  /* DVD player */
    CEA861_SOURCE_DVHS,                                 /* D-VHS */
    CEA861_SOURCE_HDD_RECORDER,                         /* HDD Video Recorder */
    CEA861_SOURCE_DVC,                                  /* DVC */
    CEA861_SOURCE_DSC,                                  /* DSC */
    CEA861_SOURCE_VCD,                                  /* Video CD */
    CEA861_SOURCE_GAME,                                 /* Game */
    CEA861_SOURCE_PC,                                   /* PC general */
    CEA861_SOURCE_BD,                                   /* Blu-Ray Disc */
    CEA861_SOURCE_SACD,                                 /* Super Audio CD */
    CEA861_SOURCE_HD_DVD,                               /* HD DVD */
    CEA861_SOURCE_PMP                                   /* PMP */
} CEA861_SOURCE_INFORMATION_e;

typedef struct _CEA861_SOURCE_PRODUCT_DESCRIPTION_s_ {
    UINT8   VendorName[8];
    UINT8   ProductDescription[16];
    CEA861_SOURCE_INFORMATION_e SrcInfo;                /* Source Information */
} CEA861_SOURCE_PRODUCT_DESCRIPTION_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions & Constants for CEA861 Video Capability Data
\*-----------------------------------------------------------------------------------------------*/
typedef union _CEA861_VIDEO_CAPABILITY_u_ {
    UINT8  Data;

    struct {
        UINT8 S_CE:     2;  /* [1:0] CE video format. 1 = Overscanned, 2 = Underscanned, 3 = Supports both over-&under-scan */
        UINT8 S_IT:     2;  /* [3:2] IT video format. 1 = Overscanned, 2 = Underscanned, 3 = Supports both over-&under-scan */
        UINT8 S_PT:     2;  /* [5:4] Preferred video format. 1 = Overscanned, 2 = Underscanned, 3 = Supports both over-&under-scan */
        UINT8 QS:       1;  /* [6] 1 = Selectable Quantization Range (RGB only) */
        UINT8 QY:       1;  /* [7] 1 = Selectable Quantization Range (YCC only) */
    } Bits;
} CEA861_VIDEO_CAPABILITY_u;

/*-----------------------------------------------------------------------------------------------*\
 * HDMI Management Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_HDMI_SINK_INFO_s_ {
    AMBA_HDMI_CABLE_DETECT_e        CableState;                 /* The current DVI/HDMI cable configuration */
    UINT8                           SelectableRgbQuantRange;    /* RGB quantization range is selectable or not */
    UINT8                           SelectableYccQuantRange;    /* YCbCr quantization range is selectable or not */
    UINT16                          *pVideoInfo;                /* Supported video formats (union of (1 << AMBA_HDMI_VIDEO_FRAME_LAYOUT_e) for each VIC) */
    AMBA_HDMI_AUDIO_DESCRIPTOR_s    *pAudioInfo;                /* Capabilities of each Audio Coding Type */
} AMBA_HDMI_SINK_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * HDMI MODE INFO
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_HDMI_VIDEO_CONFIG_s_ {
    AMBA_CEA861_VIDEO_ID_CODE_e         FormatID;       /* Base video format ID code */
    AMBA_HDMI_VIDEO_FRAME_LAYOUT_e      FrameLayout;    /* 2D/3D video trnsamission */
    AMBA_DSP_VOUT_HDMI_OUTPUT_MODE_e    PixelFormat;    /* RGB/YCbCr color space */
    AMBA_HDMI_QUANTIZATION_RANGE_e      QuantRange;     /* Quantization rage of RGB/YCC */
    AMBA_VIDEO_TIMING_s                 VideoTiming;    /* Custom timings. Valid for HDMI_VIC_RESERVED */
} AMBA_HDMI_VIDEO_CONFIG_s;

typedef struct _AMBA_HDMI_MODE_INFO_s_ {
    float  FrameRate;       /* Frame rate (Hz) */

    AMBA_DSP_VOUT_DISPLAY_TIMING_CONFIG_s   *pDisplayTiming;
    AMBA_DSP_VOUT_DISPLAY_HDMI_CONFIG_s     *pHdmiConfig;
    UINT8 PixelRepeat;
} AMBA_HDMI_MODE_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * Macros: HDMI 2D/3D Video Format Support
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_HDMI_2D                                    (1 << AMBA_HDMI_VIDEO_2D)
#define AMBA_HDMI_3D_FRAME_PACKING                      (1 << AMBA_HDMI_VIDEO_3D_FRAME_PACKING)
#define AMBA_HDMI_3D_FIELD_ALTERNATIVE                  (1 << AMBA_HDMI_VIDEO_3D_FIELD_ALTERNATIVE)
#define AMBA_HDMI_3D_LINE_ALTERNATIVE                   (1 << AMBA_HDMI_VIDEO_3D_LINE_ALTERNATIVE)
#define AMBA_HDMI_3D_SIDE_BY_SIDE_FULL                  (1 << AMBA_HDMI_VIDEO_3D_SIDE_BY_SIDE_FULL)
#define AMBA_HDMI_3D_L_DEPTH                            (1 << AMBA_HDMI_VIDEO_3D_L_DEPTH)
#define AMBA_HDMI_3D_L_DEPTH_GFX_GFX_DEPTH              (1 << AMBA_HDMI_VIDEO_3D_L_DEPTH_GFX_GFX_DEPTH)
#define AMBA_HDMI_3D_TOP_AND_BOTTOM                     (1 << AMBA_HDMI_VIDEO_3D_TOP_AND_BOTTOM)
#define AMBA_HDMI_3D_SIDE_BY_SIDE_HALF_HORIZONTAL       (1 << AMBA_HDMI_VIDEO_3D_SIDE_BY_SIDE_HALF_HORIZONTAL)
#define AMBA_HDMI_3D_SIDE_BY_SIDE_HALF_QUINCUNX_LO_RO   (1 << AMBA_HDMI_VIDEO_3D_SIDE_BY_SIDE_HALF_QUINCUNX_LO_RO)
#define AMBA_HDMI_3D_SIDE_BY_SIDE_HALF_QUINCUNX_LO_RE   (1 << AMBA_HDMI_VIDEO_3D_SIDE_BY_SIDE_HALF_QUINCUNX_LO_RE)
#define AMBA_HDMI_3D_SIDE_BY_SIDE_HALF_QUINCUNX_LE_RO   (1 << AMBA_HDMI_VIDEO_3D_SIDE_BY_SIDE_HALF_QUINCUNX_LE_RO)
#define AMBA_HDMI_3D_SIDE_BY_SIDE_HALF_QUINCUNX_LE_RE   (1 << AMBA_HDMI_VIDEO_3D_SIDE_BY_SIDE_HALF_QUINCUNX_LE_RE)

#define AMBA_HDMI_3D_SIDE_BY_SIDE_HALF_QUINCUNX         \
    (AMBA_HDMI_3D_SIDE_BY_SIDE_HALF_QUINCUNX_LO_RO  |   \
     AMBA_HDMI_3D_SIDE_BY_SIDE_HALF_QUINCUNX_LO_RE  |   \
     AMBA_HDMI_3D_SIDE_BY_SIDE_HALF_QUINCUNX_LE_RO  |   \
     AMBA_HDMI_3D_SIDE_BY_SIDE_HALF_QUINCUNX_LE_RE)

#define AMBA_HDMI_3D_SIDE_BY_SIDE_HALF                  \
    (AMBA_HDMI_3D_SIDE_BY_SIDE_HALF_HORIZONTAL      |   \
     AMBA_HDMI_3D_SIDE_BY_SIDE_HALF_QUINCUNX)

#define AMBA_HDMI_3D                                    \
    (AMBA_HDMI_3D_FRAME_PACKING                     |   \
     AMBA_HDMI_3D_FIELD_ALTERNATIVE                 |   \
     AMBA_HDMI_3D_LINE_ALTERNATIVE                  |   \
     AMBA_HDMI_3D_SIDE_BY_SIDE_FULL                 |   \
     AMBA_HDMI_3D_L_DEPTH                           |   \
     AMBA_HDMI_3D_L_DEPTH_GFX_GFX_DEPTH             |   \
     AMBA_HDMI_3D_TOP_AND_BOTTOM                    |   \
     AMBA_HDMI_3D_SIDE_BY_SIDE_HALF)

/*-----------------------------------------------------------------------------------------------*\
 * HDMI Control Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_HDMI_CTRL_s_ {
    UINT16                          PhysicalAddress;
    UINT8                           LogicalAddress;
    AMBA_HDMI_CABLE_DETECT_e        CableState;                         /* The current DVI/HDMI cable configuration */
    CEA861_VIDEO_CAPABILITY_u       VideoCapability;                    /* Video capability */
    UINT16                          SinkVideoInfo[AMBA_NUM_VIDEO_ID];   /* Unions of AMBA_HDMI_VIDEO_LAYOUT_e */
    AMBA_HDMI_AUDIO_DESCRIPTOR_s    SinkAudioInfo[AMBA_NUM_HDMI_AIC];   /* Capabilities of each Audio Coding Type */

    AMBA_HDMI_VIDEO_CONFIG_s                VideoCfg;
    AMBA_HDMI_AUDIO_CONFIG_s                AudioCfg;
    CEA861_SOURCE_PRODUCT_DESCRIPTION_s     DeviceCfg;

    AMBA_DSP_VOUT_DISPLAY_HDMI_CONFIG_s     DspHdmiConfig;
    AMBA_DSP_VOUT_DISPLAY_TIMING_CONFIG_s   DspDisplayTiming;
} AMBA_HDMI_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaHDMI.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_KAL_EVENT_FLAG_t AmbaHdmiEventFlag;

int AmbaHDMI_Init(CEA861_SOURCE_PRODUCT_DESCRIPTION_s *pSourceProduct);

int AmbaHDMI_Enable(void);
int AmbaHDMI_Disable(void);
int AmbaHDMI_SetMute(UINT32 MuteFlag);

int AmbaHDMI_SetMode(AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig);
int AmbaHDMI_GetModeInfo(AMBA_HDMI_MODE_INFO_s *pModeInfo);
int AmbaHDMI_Start(void);
int AmbaHDMI_Stop(void);

int AmbaHDMI_SetPhyCtrl(AMBA_HDMI_SINK_CURRENT_INCREMENT_e SinkCurrentInc, AMBA_HDMI_SINK_PREEMPHASIS_MODE_e SinkPreEmphasisMode);

int AmbaHDMI_GetSinkInfo(AMBA_HDMI_SINK_INFO_s *pSinkInfo); /* Designed for the case when pSinkStateCb is NULL */
UINT16 AmbaHDMI_GetPhysicalAddress(void);

/*-----------------------------------------------------------------------------------------------*\
 * HDMI CEC module
\*-----------------------------------------------------------------------------------------------*/
int AmbaHDMI_CecEnable(void);
int AmbaHDMI_CecDisable(void);
int AmbaHDMI_CecSetDeviceAddress(UINT8 LogicalAddress);
int AmbaHDMI_CecWrite(UINT8 *pMessage, UINT32 MsgSize);
int AmbaHDMI_CecRead(UINT8 *pMessage, UINT32 *pMsgSize);

#endif /* _AMBA_HDMI_H_ */
