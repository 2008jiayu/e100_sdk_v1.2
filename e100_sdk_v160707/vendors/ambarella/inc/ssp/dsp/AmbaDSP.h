/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDSP.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella DSP Support Package
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DSP_H_
#define _AMBA_DSP_H_

#define AMBA_DSP_MAX_OSD_BLEND_AREA_NUMBER      (3)

typedef struct _AMBA_DSP_UCODE_VERSION_INFO_s_ {
    UINT8   Month;                      /* Month */
    UINT8   Day;                        /* Day */
    UINT16  Year;                       /* Year */
    UINT32  UCodeVer;                   /* the Version of uCode */
    UINT32  ApiVer;                     /* the Version of API */
    UINT32  SiliconVer;                 /* Silicon Version */
} AMBA_DSP_UCODE_VERSION_INFO_s;

typedef union _AMBA_DSP_CHANNEL_ID_u_ {
    UINT16   Data;                      /* this is an 8-bit data */

    struct {
        UINT16  VinID:      2;          /* VIN Channel ID */
        UINT16  SensorID:   6;          /* Sensor ID/Source ID */
        UINT16  HdrID:      4;          /* [11:8]: Hdr Exposure frame ID. 1 bit for each exposure channel */
        UINT16  Reserved:   4;          /* [15:12]: */
    } Bits;
} AMBA_DSP_CHANNEL_ID_u;

typedef struct _AMBA_DSP_WINDOW_s_ {
    UINT16  OffsetX;                    /* Horizontal offset of the window */
    UINT16  OffsetY;                    /* Vertical offset of the window */
    UINT16  Width;                      /* Number of pixels per line in the window */
    UINT16  Height;                     /* Number of lines in the window */
} AMBA_DSP_WINDOW_s;

typedef enum _AMBA_DSP_YUV_FORMAT_e_ {
    AMBA_DSP_YUV420 = 0,                /* YUV 420 format */
    AMBA_DSP_YUV422                     /* YUV 422 format */
} AMBA_DSP_YUV_FORMAT_e;

typedef struct _AMBA_DSP_RAW_BUF_s_ {
    UINT8   Compressed;                 /* 1 - compressed raw data, 0 - uncompressed raw data */
    UINT8   *pBaseAddr;                 /* pointer to raw buffer */

    UINT16  Pitch;                      /* raw buffer pitch */
    AMBA_DSP_WINDOW_s   Window;         /* Window position and size */
} AMBA_DSP_RAW_BUF_s;

typedef struct _AMBA_DSP_YUV_IMG_BUF_s {
    AMBA_DSP_YUV_FORMAT_e   DataFmt;    /* YUV Data format: 420 or 422 */
    UINT8   *pBaseAddrY;                /* pointer to Luma (Y) data area */
    UINT8   *pBaseAddrUV;               /* pointer to Chroma (UV) data area */

    UINT32  Pitch;                      /* YUV data buffer pitch */
    AMBA_DSP_WINDOW_s   Window;         /* Window position and size */
    UINT32  ChannelID;                      /* YUV data buffer channel  */
} AMBA_DSP_YUV_IMG_BUF_s;

typedef struct _AMBA_DSP_YUV_DATA_RDY_s {
    AMBA_DSP_YUV_IMG_BUF_s  Buff;       /* Yuv buffer */
    UINT32                  PtsTick;    /* PtsTick in audioTick unit */
    UINT32                  AudioClk;   /* audioTick freq */
} AMBA_DSP_YUV_DATA_RDY_s;

typedef struct _AMBA_DSP_QUICKVIEW_YUV_IMG_BUF_s {
    AMBA_DSP_YUV_FORMAT_e   DataFmt;    /* YUV Data format: 420 or 422 */
    UINT8   *pBaseAddrY;                /* pointer to Luma (Y) data area */
    UINT8   *pBaseAddrUV;               /* pointer to Chroma (UV) data area */

    UINT32  Pitch;                      /* YUV data buffer pitch */
    UINT16  ActiveWidth;                /* Active width */
    UINT16  ActiveHeight;               /* Active height */
    AMBA_DSP_WINDOW_s   Window;         /* Window position and size */
} AMBA_DSP_QUICKVIEW_YUV_IMG_BUF_s;

typedef struct _AMBA_DSP_BUF_s_ {
    UINT8   *pBaseAddr;                 /* pointer to a DRAM image buffer */
    UINT16  Pitch;                      /* buffer pitch */
    AMBA_DSP_WINDOW_s   Window;         /* Window position and size */
} AMBA_DSP_BUF_s;

typedef struct _AMBA_DSP_FRAME_RATE_s_ {
    UINT8   Interlace;                  /* 1 - Interlace; 0 - Progressive */
    UINT32  TimeScale;                  /* time scale */
    UINT32  NumUnitsInTick;             /* Frames per Second = TimeScale / (NumUnitsInTick * (1 + Interlace)) */
} AMBA_DSP_FRAME_RATE_s;

typedef struct _AMBA_DSP_JPEG_BIT_RATE_CTRL_s_ {
    UINT16  QualityLevel;               /* initial quality level */
    UINT16  TargetBitRate;              /* target bit per pixel */
    UINT32  Tolerance;                  /* bitrate tolerance */
    UINT32  RateCurvPoints;             /* number of rate curve points */
    UINT8   *pRateCurv;                 /* pointer to rate curve points buffer */
    UINT32  MaxEncLoop;                 /* maximum encode loop */
} AMBA_DSP_JPEG_BIT_RATE_CTRL_s;

typedef enum _AMBA_DSP_ROTATE_FLIP_e_ {
    AMBA_DSP_ROTATE_0 = 0,              /* No rotation */
    AMBA_DSP_ROTATE_0_HORZ_FLIP,        /* No rotation and horizontal flip */
    AMBA_DSP_ROTATE_90,                 /* Clockwise 90 degree */
    AMBA_DSP_ROTATE_90_VERT_FLIP,       /* Clockwise 90 degree and vertical flip*/
    AMBA_DSP_ROTATE_180,                /* Clockwise 180 degree */
    AMBA_DSP_ROTATE_180_HORZ_FLIP,      /* Clockwise 180 degree and horizontal flip */
    AMBA_DSP_ROTATE_270,                /* Clockwise 270 degree */
    AMBA_DSP_ROTATE_270_VERT_FLIP       /* Clockwise 270 degree and vertical flip */
} AMBA_DSP_ROTATE_FLIP_e;

typedef struct _AMBA_DSP_VIDEO_BLEND_CONFIG_s_ {
    UINT8                   BlendArea;              /* 0 ~ 31, user specific blending area */
    UINT8                   Enable;                 /* 1 - enable, 0 - disable */
    AMBA_DSP_YUV_IMG_BUF_s  BlendYuvBuf;            /* information of yuv 420 blending buffer */
    AMBA_DSP_YUV_IMG_BUF_s  AlphaBuf;               /* information of alpha matrix */
} AMBA_DSP_VIDEO_BLEND_CONFIG_s;

typedef struct _AMBA_DSP_CAL_IDSP_FREQ_INFO_s_ {
    UINT16                  VinCapWidth;                /* AmbaDSP_VinCaptureConfig */
    UINT16                  VinCapHeight;
    AMBA_DSP_FRAME_RATE_s   VinFramerate;
    UINT32                  VBlanking;
    UINT16                  VinOutWidth;
    UINT16                  MainYuvWidth;               /* AMBA_DSP_LIVEVIEW_CONFIG_s */
    UINT16                  MainYuvHeight;
    UINT16                  SecYuvWidth;                /* AMBA_DSP_LIVEVIEW_CONFIG_s */
    UINT16                  SecYuvHeight;
    UINT32                  MainEncFrameSize;
    UINT32                  MainEncFrameRate;
    UINT32                  SecEncFrameSize;
    UINT32                  SecEncFrameRate;
    UINT16                  LcdLiveviewWidth;           /* AMBA_DSP_LIVEVIEW_DISP_CONFIG_s */
    UINT16                  LcdLiveviewHeight;
    AMBA_DSP_FRAME_RATE_s   LcdLiveviewFrateRate;
    UINT16                  TvLiveviewWidth;
    UINT16                  TvLiveviewHeight;
    AMBA_DSP_FRAME_RATE_s   TvLiveviewFrateRate;
    AMBA_DSP_FRAME_RATE_s   SecYuvFrateRate;            /* AMBA_DSP_LIVEVIEW_CONFIG_s */
    UINT8                   VideoProcMode:2;            /* AMBA_DSP_LIVEVIEW_CTRL_FLAG_u */
    UINT8                   VideoAlgoMode:3;            /* AMBA_DSP_LIVEVIEW_CTRL_FLAG_u */
    UINT8                   VideoOSMode:2;              /* AMBA_DSP_LIVEVIEW_CTRL_FLAG_u */
    UINT8                   VideoHdrMode:1;             /* AMBA_DSP_LIVEVIEW_CTRL_FLAG_u */
    UINT32                  HWLimitation;               /* Report HW min. required freq */
} AMBA_DSP_CAL_IDSP_FREQ_INFO_s;

typedef struct _AMBA_DSP_CAL_CORE_FREQ_INFO_s_ {
    UINT32  NumEncStream;
    UINT32  *FrameSize;
    UINT32  *FrameRate;
    UINT32  LcdFrameWidth;
    UINT32  LcdFrameHeight;
    UINT32  LcdFrameRate;
    UINT32  TvFrameWidth;
    UINT32  TvFrameHeight;
    UINT32  TvFrameRate;
    UINT8   VideoProcMode:2;            /* AMBA_DSP_LIVEVIEW_CTRL_FLAG_u */
    UINT8   VideoAlgoMode:3;            /* AMBA_DSP_LIVEVIEW_CTRL_FLAG_u */
    UINT8   VideoOSMode:2;              /* AMBA_DSP_LIVEVIEW_CTRL_FLAG_u */
    UINT8   LiveViewOnly:1;             /* AMBA_DSP_LIVEVIEW_CTRL_FLAG_u */
} AMBA_DSP_CAL_CORE_FREQ_INFO_s;

/*---------------------------------------------------------------------------*\
 * DSP VIN related
\*---------------------------------------------------------------------------*/
typedef enum _AMBA_DSP_SENSOR_PATTERN_e_ {
    AMBA_DSP_BAYER_RG = 0,              /* RG */
    AMBA_DSP_BAYER_BG,                  /* BG */
    AMBA_DSP_BAYER_GR,                  /* GR */
    AMBA_DSP_BAYER_GB                   /* GB */
} AMBA_DSP_SENSOR_PATTERN_e;

typedef enum _AMBA_DSP_YUV_ORDER_e_ {
    AMBA_DSP_CR_Y0_CB_Y1 = 0,
    AMBA_DSP_CB_Y0_CR_Y1,
    AMBA_DSP_Y0_CR_Y1_CB,
    AMBA_DSP_Y0_CB_Y1_CR
} AMBA_DSP_YUV_ORDER_e;

typedef enum _AMBA_DSP_COLOR_SPACE_e_ {
    AMBA_DSP_COLOR_SPACE_RGB = 0,
    AMBA_DSP_COLOR_SPACE_YUV
} AMBA_DSP_COLOR_SPACE_e;

typedef enum _AMBA_DSP_PHASE_SHIFT_e_ {
    AMBA_DSP_PHASE_SHIFT_MODE_0 = 0,
    AMBA_DSP_PHASE_SHIFT_MODE_1,
    AMBA_DSP_PHASE_SHIFT_MODE_2,
    AMBA_DSP_PHASE_SHIFT_MODE_3,
    AMBA_DSP_PHASE_SHIFT_MODE_4,
    AMBA_DSP_PHASE_SHIFT_MODE_5,
    AMBA_DSP_PHASE_SHIFT_MODE_6,

    AMBA_DSP_NUM_PHASE_SHIFT_MODE,
} AMBA_DSP_PHASE_SHIFT_e;

typedef struct _AMBA_DSP_PHASE_SHIFT_CTRL_s_ {
    AMBA_DSP_PHASE_SHIFT_e  Horizontal;
    AMBA_DSP_PHASE_SHIFT_e  Vertical;
} AMBA_DSP_PHASE_SHIFT_CTRL_s;

/*---------------------------------------------------------------------------*\
 * DSP VOUT related
\*---------------------------------------------------------------------------*/
typedef enum _AMBA_DSP_VOUT_IDX_e_ {
    AMBA_DSP_VOUT_LCD = 0,              /* VOUT channel LCD */
    AMBA_DSP_VOUT_TV,                   /* VOUT channel TV */

    AMBA_NUM_DSP_VOUT                   /* Total number of VOUT channels */
} AMBA_DSP_VOUT_IDX_e;

typedef struct _AMBA_DSP_VOUT_VIDEO_CONFIG_s_ {
    AMBA_DSP_ROTATE_FLIP_e  RotateFlip; /* Rotate and flip setting */
    AMBA_DSP_WINDOW_s       Window;     /* VOUT video window */
} AMBA_DSP_VOUT_VIDEO_CONFIG_s;

typedef struct _AMBA_DSP_VOUT_DEFAULT_IMG_CONFIG_s_ {
    UINT8   FieldRepeat;                /* Repeat the default image for top field and bottom field in interlaced mode */
    UINT16  Pitch;                      /* The DRAM pitch (number of bytes) between two contiguous lines */
    UINT8   *pBaseAddrY;                /* pointer to Luma (Y) data area */
    UINT8   *pBaseAddrUV;               /* pointer to Chroma (UV) data area */
} AMBA_DSP_VOUT_DEFAULT_IMG_CONFIG_s;

typedef struct _AMBA_DSP_VOUT_OSD_BUF_CONFIG_s_ {
    UINT8   FieldRepeat;                /* Repeat the default image for top field and bottom field in interlaced mode */
    UINT16  Pitch;                      /* The DRAM pitch (number of bytes) between two contiguous lines */
    void    *pBaseAddr;                 /* pointer to the base address of OSD data */
} AMBA_DSP_VOUT_OSD_BUF_CONFIG_s;

#define AMBA_DSP_VOUT_DIGITAL_GAMMA_SIZE    (1024*3)

typedef enum _AMBA_DSP_VOUT_MIXER_COLOR_FORMAT_e_ {
    MIXER_IN_YUV_422 = 0,               /* YUV 422 */
    MIXER_IN_YUV_444_RGB                /* YUV 444 or RGB */
} AMBA_DSP_VOUT_MIXER_COLOR_FORMAT_e;

typedef struct _AMBA_DSP_VOUT_MIXER_CONFIG_s_ {
    UINT16  ActiveWidth;                /* Number of pixels per line */
    UINT16  ActiveHeight;               /* Number of lines */
    UINT8   VideoHorReverseEnable;      /* 1 - Video Data is horizontally reversed */
    AMBA_DSP_FRAME_RATE_s               FrameRate;          /* frame rate */
    AMBA_DSP_VOUT_MIXER_COLOR_FORMAT_e  MixerColorFormat;   /* mixer color format */
} AMBA_DSP_VOUT_MIXER_CONFIG_s;

typedef enum _AMBA_DSP_VOUT_MIXER_CSC_CTRL_e_ {
    MIXER_CSC_DISABLE = 0,              /* Mixer CSC Disabled */
    MIXER_CSC_FOR_VIDEO,                /* Mixer CSC Enabled for Video layer */
    MIXER_CSC_FOR_OSD                   /* Mixer CSC Enabled for OSD */
} AMBA_DSP_VOUT_MIXER_CSC_CTRL_e;

typedef enum _AMBA_DSP_VOUT_OSD_DATA_FORMAT_e_ {
    OSD_8BIT_CLUT_MODE = 0,             /* 8-bit color look-up table mode */

    /* 16-bit direct mode */
    OSD_16BIT_VYU_RGB_565 = 1,          /* VYU_RGB_565            */
    OSD_16BIT_UYV_BGR_565,              /* UYV_BGR_565            */
    OSD_16BIT_AYUV_4444,                /* AYUV_4444              */
    OSD_16BIT_RGBA_4444,                /* RGBA_4444              */
    OSD_16BIT_BGRA_4444,                /* BGRA_4444              */
    OSD_16BIT_ABGR_4444,                /* ABGR_4444              */
    OSD_16BIT_ARGB_4444,                /* ARGB_4444              */
    OSD_16BIT_AYUV_1555,                /* AYUV_1555              */
    OSD_16BIT_YUV_1555,                 /* YUV_1555,  MSB ignored */
    OSD_16BIT_RGBA_5551,                /* RGBA_5551              */
    OSD_16BIT_BGRA_5551,                /* BGRA_5551              */
    OSD_16BIT_ABGR_1555,                /* ABGR_1555              */
    OSD_16BIT_ARGB_1555,                /* ARGB_1555              */

    /* 32-bit direct mode */
    OSD_32BIT_AYUV_8888 = 27,           /* AYUV_8888 */
    OSD_32BIT_RGBA_8888,                /* RGBA_8888 */
    OSD_32BIT_BGRA_8888,                /* BGRA_8888 */
    OSD_32BIT_ABGR_8888,                /* ABGR_8888 */
    OSD_32BIT_ARGB_8888                 /* ARGB_8888 */
} AMBA_DSP_VOUT_OSD_DATA_FORMAT_e;

typedef enum _AMBA_DSP_VOUT_TYPE_e_ {
    AMBA_DSP_VOUT_DIGITAL = 0,          /* digital vout */
    AMBA_DSP_VOUT_CVBS,                 /* analog cvbs vout */
    AMBA_DSP_VOUT_HDMI                  /* HDMI vout */
} AMBA_DSP_VOUT_TYPE_e;

/*---------------------------------------------------------------------------*/
typedef enum _AMBA_DSP_SYS_STATE_e_ {
    AMBA_DSP_SYS_STATE_LIVEVIEW = 0,    /* liveview state */
    AMBA_DSP_SYS_STATE_PLAYBACK,        /* still playback state */
    AMBA_DSP_SYS_STATE_SENSORLESS,      /* sensorless encode state */
    AMBA_DSP_SYS_STATE_VOUT,            /* vout only state */
    AMBA_DSP_SYS_STATE_VIDEOTUNING,     /* video tuning state */

    AMBA_NUM_DSP_SYS_STATE              /* number of DSP state */
} AMBA_DSP_SYS_STATE_e;

typedef struct _AMBA_DSP_TASK_CFG_s_ {
    UINT32 Priority;
    UINT32 CoreSelectBitMap;
} AMBA_DSP_TASK_CFG_s;

typedef struct _AMBA_DSP_SYS_CONFIG_s_ {
    AMBA_DSP_SYS_STATE_e SysState;           /* DSP System state */
    UINT8   *pDebugLogDataArea;              /* pointer to DSP Debug Log Data area */
    UINT32  DebugLogDataAreaSize;            /* DSP Debug Log Data area in Bytes */
    UINT8   *pWorkArea;                      /* pointer to DSP Work Data area */
    UINT32  WorkAreaSize;                    /* DSP Work Data area in Bytes */
    AMBA_DSP_TASK_CFG_s IntTask;             /* Dsp interrupt task config */
    AMBA_DSP_TASK_CFG_s ArmCommTask;         /* Arm communication task config */
    AMBA_DSP_TASK_CFG_s CmdPrepareTask;      /* Arm command prepare task config */
    AMBA_DSP_TASK_CFG_s MsgTask;             /* Message dispatcher task config */
    UINT32  VoutSeamlessEnable;              /* 0 for disable, 1 for enable with no reset, 2 for enable with reset. */
    UINT32  VoutSeamlessBufferNum;           /* seamless buffer number */
    UINT32  VoutSeamlessBufferSize;          /* seamless buffer size, (LCD width) * (LCD height) * (Luma + Chroma Buffer) */
} AMBA_DSP_SYS_CONFIG_s;

typedef enum _AMBA_DSP_FILTER_ID_e_ {
    AMBA_DSP_VIDEO_FILTER = 0,          /* Liveview filter id */
    AMBA_DSP_STILL_LISO_FILTER,         /* Still Low-ISO filter id */
    AMBA_DSP_STILL_HISO_FILTER,         /* Still High-ISO filter id */
    AMBA_DSP_RAW_TO_RAW_FILTER,         /* Raw2Raw filter id */

    AMBA_DSP_NUM_FILTER,                 /* Number of filter id */
    AMBA_DSP_NO_BATCH_FILTER = 0xF,
} AMBA_DSP_FILTER_ID_e;

/* AMBA_DSP_FILTER_CVT(BatchID, ChannelID): ChannelID(4 bits) | Reserved(2 bits) | BatchID (4 bits) */
#define AMBA_DSP_FILTER_CVT(x, z)    (((z) << 6) | (x))

typedef struct _AMBA_DSP_LOG_CTRL_s_ {
    UINT8  SysCmdShow;
    UINT8  VoutCmdShow;
    UINT8  EncCmdShow;
    UINT8  DecCmdShow;
    UINT8  IDspCmdShow;
    UINT8  VcapCmdShow;
    UINT8  VdspShow;
    UINT8  VcapShow;
    UINT8  MsgLogOn;
    UINT8  VinSofShow;
    UINT8  VinEofShow;
    UINT8  BitsInfoShowJpeg;
    UINT8  BitsInfoShowH264;
    UINT8  PtsShow;
    UINT8  Video3aInfo;
    UINT8  Still3aInfo;
    UINT8  DefaultCmdBufInfo;
    UINT8  VdspCmdBufInfo;
    UINT8  VcapCmdBufInfo;
    UINT8  AsyncCmdBufInfo;
    UINT8  CapInfo;
} AMBA_DSP_LOG_CTRL_s;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaDSP_Main.c
\*---------------------------------------------------------------------------*/
AMBA_DSP_UCODE_VERSION_INFO_s *AmbaGetDspUCodeVerInfo(void);

int AmbaDSP_GetDefaultSysCfg(AMBA_DSP_SYS_CONFIG_s *pDspSysConfig);
int AmbaDSP_Init(AMBA_DSP_SYS_CONFIG_s *pDspSysConfig);
int AmbaDSP_Suspend(void);
int AmbaDSP_Resume(void);
int AmbaDSP_SetWorkArea(UINT8 *pWorkArea, UINT32 WorkAreaSize);

void AmbaDSP_GetUcodeBaseAddr(UINT32 *pCodeAddr, UINT32 *pMeAddr, UINT32 *pMdxfAddr, UINT32 *pDefaultDataAddr);
void AmbaDSP_GetUcodeTextAddr(UINT32 *pCodeAddr, UINT32 *pMeAddr, UINT32 *pMdxfAddr);

UINT32 AmbaDSP_CalCoreFreq(AMBA_DSP_CAL_CORE_FREQ_INFO_s *pInfo);
UINT32 AmbaDSP_CalIdspFreq(AMBA_DSP_CAL_IDSP_FREQ_INFO_s *pInfo);

/**
 * Set DSP command print On/Off
 * @param [in] StrCmdCode The DSP command code string you want to set(e.g. 0x3001)
 * @param [in] On 1 - set on, 0 - set off
 */
extern void AmbaDSP_CmdShow(char *StrCmdCode, UINT8 On);

/**
 * Set DSP command print On/Off by whole category
 * @param [in] CmdCat The DSP command category group you want to set\n
                      1~9 dsp command, 91~96 idsp command
 * @param [in] On 1 - set on, 0 - set off
 */
extern void AmbaDSP_CmdShowCat(UINT32 CmdCat, UINT8 On);

/**
 * Set all DSP command print On/Off
 * @param [in] On 1 - set on, 0 - set off
 */
extern void AmbaDSP_CmdShowAll(UINT8 On);

#endif  /* _AMBA_DSP_H_ */
