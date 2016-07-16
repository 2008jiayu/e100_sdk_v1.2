/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDSP_VIN.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for DSP VIN Control APIs
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DSP_VIN_H_
#define _AMBA_DSP_VIN_H_

#include "AmbaDSP.h"
#include "AmbaVIN.h"

typedef struct _AMBA_DSP_VIN_INFO_s_ {
    AMBA_DSP_FRAME_RATE_s           FrameRate;
    AMBA_DSP_PHASE_SHIFT_CTRL_s     DspPhaseShift;  /* DSP Phase Shift */
    AMBA_DSP_COLOR_SPACE_e          ColorSpace;     /* RGB or YUV */
    AMBA_DSP_SENSOR_PATTERN_e       BayerPattern;   /* Bayer pattern for Sensor/RGB input only */
    AMBA_DSP_YUV_ORDER_e            YuvOrder;       /* Pixel order for YUV input only */
    UINT32                          NumDataBits;    /* Bit depth of pixel data */
    UINT8                           NumSkipFrame;
} AMBA_DSP_VIN_INFO_s;

typedef enum _AMBA_DSP_VIN_MIPI_CSI2_DATA_TYPE_e_ {
    /* 0x00 to 0x07 - CSI-2 Synchronization Short Packet Data Types */
    AMBA_DSP_VIN_MIPI_SYNC_FRAME_START  = 0x00,     /* Frame Start Code */
    AMBA_DSP_VIN_MIPI_SYNC_FRAME_END,               /* Frame End Code */
    AMBA_DSP_VIN_MIPI_SYNC_LINE_START,              /* Line Start Code */
    AMBA_DSP_VIN_MIPI_SYNC_LINE_END,                /* Line End Code */

    /* 0x08 to 0x0F - CSI-2 Generic Short Packet Data Types */
    AMBA_DSP_VIN_MIPI_SHORT_PACKET1     = 0x08,     /* Generic Short Packet Code 1 */
    AMBA_DSP_VIN_MIPI_SHORT_PACKET2,                /* Generic Short Packet Code 2 */
    AMBA_DSP_VIN_MIPI_SHORT_PACKET3,                /* Generic Short Packet Code 3 */
    AMBA_DSP_VIN_MIPI_SHORT_PACKET4,                /* Generic Short Packet Code 4 */
    AMBA_DSP_VIN_MIPI_SHORT_PACKET5,                /* Generic Short Packet Code 5 */
    AMBA_DSP_VIN_MIPI_SHORT_PACKET6,                /* Generic Short Packet Code 6 */
    AMBA_DSP_VIN_MIPI_SHORT_PACKET7,                /* Generic Short Packet Code 7 */
    AMBA_DSP_VIN_MIPI_SHORT_PACKET8,                /* Generic Short Packet Code 8 */

    /* 0x10 to 0x17 - CSI-2 Generic Long Packet Data Types */
    AMBA_DSP_VIN_MIPI_LONG_NULL         = 0x10,     /* Generic 8-bit Long Null */
    AMBA_DSP_VIN_MIPI_LONG_BLANKING,                /* Generic 8-bit Long Blanking Data */
    AMBA_DSP_VIN_MIPI_LONG_EMBEDDED,                /* Generic 8-bit Long Embedded Information */

    /* 0x18 to 0x1F - CSI-2 YUV Data Types */
    AMBA_DSP_VIN_MIPI_YUV420_8BIT       = 0x18,     /* YUV420 8-bit */
    AMBA_DSP_VIN_MIPI_YUV420_10BIT      = 0x19,     /* YUV420 10-bit */
    AMBA_DSP_VIN_MIPI_YUV422_8BIT       = 0x1E,     /* YUV422 8-bit */
    AMBA_DSP_VIN_MIPI_YUV422_10BIT      = 0x1F,     /* YUV422 10-bit */

    /* 0x20 to 0x27 - CSI-2 RGB Data Types */
    AMBA_DSP_VIN_MIPI_RGB444            = 0x20,     /* RGB444 */
    AMBA_DSP_VIN_MIPI_RGB555,                       /* RGB555 */
    AMBA_DSP_VIN_MIPI_RGB565,                       /* RGB565 */
    AMBA_DSP_VIN_MIPI_RGB666,                       /* RGB666 */
    AMBA_DSP_VIN_MIPI_RGB888,                       /* RGB888 */

    /* 0x28 to 0x2F - CSI-2 RAW Data Types */
    AMBA_DSP_VIN_MIPI_RAW6              = 0x28,     /* RAW6 */
    AMBA_DSP_VIN_MIPI_RAW7,                         /* RAW7 */
    AMBA_DSP_VIN_MIPI_RAW8,                         /* RAW8 */
    AMBA_DSP_VIN_MIPI_RAW10,                        /* RAW10 */
    AMBA_DSP_VIN_MIPI_RAW12,                        /* RAW12 */
    AMBA_DSP_VIN_MIPI_RAW14,                        /* RAW14 */

    /* 0x30 to 0x37 - CSI-2 User Defined Byte-based Data Types */
    AMBA_DSP_VIN_MIPI_USER_DEFINED1     = 0x30,     /* User Defined 8-bit Data Type 1 */
    AMBA_DSP_VIN_MIPI_USER_DEFINED2,                /* User Defined 8-bit Data Type 2 */
    AMBA_DSP_VIN_MIPI_USER_DEFINED3,                /* User Defined 8-bit Data Type 3 */
    AMBA_DSP_VIN_MIPI_USER_DEFINED4,                /* User Defined 8-bit Data Type 4 */
    AMBA_DSP_VIN_MIPI_USER_DEFINED5,                /* User Defined 8-bit Data Type 5 */
    AMBA_DSP_VIN_MIPI_USER_DEFINED6,                /* User Defined 8-bit Data Type 6 */
    AMBA_DSP_VIN_MIPI_USER_DEFINED7,                /* User Defined 8-bit Data Type 7 */
    AMBA_DSP_VIN_MIPI_USER_DEFINED8                 /* User Defined 8-bit Data Type 8 */
} AMBA_DSP_VIN_MIPI_CSI2_DATA_TYPE_e;

typedef enum _AMBA_DSP_VIN_DVP_TYPE_e_ {
    AMBA_DSP_VIN_DVP_SINGLE_PEL_SDR = 0,            /* Video data over Digital Video Port (DVP) interface */
    AMBA_DSP_VIN_DVP_SINGLE_PEL_DDR,                /* Video data over Digital Video Port (DVP) interface */
    AMBA_DSP_VIN_DVP_DOUBLE_PEL_SDR,                /* Video data over Digital Video Port (DVP) interface */
    AMBA_DSP_VIN_DVP_DOUBLE_PEL_DDR                 /* Video data over Digital Video Port (DVP) interface */
} AMBA_DSP_VIN_DVP_TYPE_e;

typedef enum _AMBA_DSP_VIN_SYNC_TYPE_e_ {
    AMBA_DSP_VIN_SYNC_BT601 = 0,                    /* No embedded sync code */
    AMBA_DSP_VIN_SYNC_BT656_LOWER_PEL,              /* Embedded sync code carried on lower pixel only */
    AMBA_DSP_VIN_SYNC_BT656_UPPER_PEL,              /* Embedded sync code carried on upper pixel only */
    AMBA_DSP_VIN_SYNC_BT656_BOTH_PEL,               /* Embedded sync code carried on both pixels */
} AMBA_DSP_VIN_SYNC_TYPE_e;

typedef enum _AMBA_DSP_VIN_SIGNAL_EDGE_TYPE_e_ {
    AMBA_DSP_VIN_SIGNAL_RISING_EDGE = 0,            /* Rising edge. A signal transition from low to high (0 to 1) */
    AMBA_DSP_VIN_SIGNAL_FALLING_EDGE                /* Falling edge. A signal transition from high to low (1 to 0) */
} AMBA_DSP_VIN_SIGNAL_EDGE_TYPE_e;

typedef enum _AMBA_DSP_VIN_SLVS_PIN_PAIR_e_ {
    AMBA_DSP_VIN_PIN_SD_LVDS_0 = 0,                 /* SD_LVDS_P_0 & SD_LVDS_N_0 */
    AMBA_DSP_VIN_PIN_SD_LVDS_1,                     /* SD_LVDS_P_1 & SD_LVDS_N_1 */
    AMBA_DSP_VIN_PIN_SD_LVDS_2,                     /* SD_LVDS_P_2 & SD_LVDS_N_2 */
    AMBA_DSP_VIN_PIN_SD_LVDS_3,                     /* SD_LVDS_P_3 & SD_LVDS_N_3 */
    AMBA_DSP_VIN_PIN_SD_LVDS_4,                     /* SD_LVDS_P_4 & SD_LVDS_N_4 */
    AMBA_DSP_VIN_PIN_SD_LVDS_5,                     /* SD_LVDS_P_5 & SD_LVDS_N_5 */
    AMBA_DSP_VIN_PIN_SD_LVDS_6,                     /* SD_LVDS_P_6 & SD_LVDS_N_6 */
    AMBA_DSP_VIN_PIN_SD_LVDS_7,                     /* SD_LVDS_P_7 & SD_LVDS_N_7 */
    AMBA_DSP_VIN_PIN_SD_LVDS_8,                     /* SD_LVDS_P_8 & SD_LVDS_N_8 */
    AMBA_DSP_VIN_PIN_SD_LVDS_9,                     /* SD_LVDS_P_9 & SD_LVDS_N_9 */
    AMBA_DSP_VIN_PIN_SD_LVDS_10,                    /* SD_LVDS_P_10 & SD_LVDS_N_10 */
    AMBA_DSP_VIN_PIN_SD_LVDS_11,                    /* SD_LVDS_P_11 & SD_LVDS_N_11 */

    AMBA_DSP_NUM_VIN_PIN_PAIR,                      /* Number of vin lvds pin pair */
    AMBA_DSP_NUM_VIN_SLVS_DATA_LANE = AMBA_DSP_NUM_VIN_PIN_PAIR     /* Number of vin lvds data lanes */
} AMBA_DSP_VIN_SLVS_PIN_PAIR_e;

/*-----------------------------------------------------------------------------------------------*\
 * VIN to VOUT Synchronization Structure
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_DSP_VIN_VOUT_SYNC_e_ {
    AMBA_DSP_VIN_VOUT_SYNC0 = 0,                    /* VIN-VOUT0 synchronization */
    AMBA_DSP_VIN_VOUT_SYNC1,                        /* VIN-VOUT1 synchronization */

    AMBA_DSP_NUM_VIN_VOUT_SYNC                      /* Number of VIN-VOUT synchronization */
} AMBA_DSP_VIN_VOUT_SYNC_e;

typedef enum _AMBA_DSP_VIN_VOUT_SYNC_TYPE_e_ {
    AMBA_DSP_VIN_VOUT_SYNC_FRAME = 0,               /* Send synchronization signal on even field */
    AMBA_DSP_VIN_VOUT_SYNC_FIELD                    /* Send synchronization signal on every field */
} AMBA_DSP_VIN_VOUT_SYNC_TYPE_e;

typedef struct _AMBA_DSP_VIN_VOUT_SYNC_s_ {
    AMBA_DSP_VIN_VOUT_SYNC_TYPE_e SignalFreq;       /* Send sync signal every frame/every field */
    UINT32 SignalLine;                              /* Start active line to send synchronization signal to VOUT */
} AMBA_DSP_VIN_VOUT_SYNC_s;

typedef struct _AMBA_DSP_VIN_SYNC_PIN_SELECT_s_ {
        UINT16  FieldPinSelect:         5;          /* [4:0] 0-9: pad_p[x]; 10-19: pad_n[x]; 20: clk_n[0]; 21: clk_n[1]; 22: clk_p[1] */
        UINT16  VsyncPinSelect:         5;          /* [9:5] 0-9: pad_p[x]; 10-19: pad_n[x]; 20: clk_n[0]; 21: clk_n[1]; 22: clk_p[1] */
        UINT16  HsyncPinSelect:         5;          /* [14:10] 0-9: pad_p[x]; 10-19: pad_n[x]; 20: clk_n[0]; 21: clk_n[1]; 22: clk_p[1] */
} AMBA_DSP_VIN_SYNC_PIN_SELECT_s;

typedef struct _AMBA_DSP_VIN_SPILT_CTRL_s_ {
        UINT8   NumSplits;                          /* number of splits to make */
        UINT16  SplitWidth;                         /* Split each input line to lines of SPLIT_WIDTH pixels(including intermediate HBLANK) */
} AMBA_DSP_VIN_SPILT_CTRL_s;

typedef struct _AMBA_DSP_VIN_SLVS_SYNC_CODE_s_ {
    UINT8   SyncInterleaving;       /* 0=none; 1=2-lane interleaving; 2=4-lane interleaving */
    UINT8   ITU656Type;             /* 1= ITU-656 type, don't care CustomSyncCode structure */
    struct {
        UINT8   PatternAlign;       /* Sync code mask/patterns are: 0=LSB aligned; 1=MSB aligned */
        UINT16  SyncCodeMask;       /* Sync Code Mask */

        struct {
            UINT8   Sol:    1;      /* 1 = Enable detection of SOL sync codes */
            UINT8   Eol:    1;      /* 1 = Enable detection of EOL sync codes */
            UINT8   Sof:    1;      /* 1 = Enable detection of SOF sync codes */
            UINT8   Eof:    1;      /* 1 = Enable detection of EOF sync codes */
            UINT8   Sov:    1;      /* 1 = Enable detection of SOV sync codes */
            UINT8   Eov:    1;      /* 1 = Enable detection of EOV sync codes */
        } DetectEnable;

        UINT16  PatternSol;         /* Start of active line */
        UINT16  PatternEol;         /* End of active line */
        UINT16  PatternSof;         /* Start of frame */
        UINT16  PatternEof;         /* End of frame */
        UINT16  PatternSov;         /* Start of vertical blanking line */
        UINT16  PatternEov;         /* End of vertical blanking line */

    } CustomSyncCode;
} AMBA_DSP_VIN_SLVS_SYNC_CODE_s;

typedef struct _AMBA_DSP_VIN_RX_HV_SYNC_s_ {
    UINT32  NumActivePixels;                         /* Horizontal active region width */
    UINT32  NumActiveLines;                          /* Vertical active region height */
    UINT32  NumTotalPixels;                          /* Horizontal total width (line_length_pck) */
    UINT32  NumTotalLines;                           /* Vertical total height (frame_length_lines) */
} AMBA_DSP_VIN_RX_HV_SYNC_s;

typedef enum _AMBA_DSP_VIN_TRIGGER_PULSE_e_ {
    AMBA_DSP_VIN_TRIGGER_PULSE0 = 0,                 /* VIN Trigger Pulse 0 */
    AMBA_DSP_VIN_TRIGGER_PULSE1,                     /* VIN Trigger Pulse 1 */

    AMBA_DSP_NUM_VIN_TRIGGER_PULSE                   /* Number of trigger pulses */
} AMBA_DSP_VIN_TRIGGER_PULSE_e;

typedef enum _AMBA_DSP_VIN_PULSE_POLARITY_e_ {
    AMBA_DSP_VIN_PULSE_LOW = 0,                      /* Active pulse is low */
    AMBA_DSP_VIN_PULSE_HIGH                          /* Active pulse is high */
} AMBA_DSP_VIN_PULSE_POLARITY_e;

typedef struct _AMBA_DSP_VIN_SYNC_WAVEFORM_s_ {
    UINT32  Period;         /* for Hsync: unit=cycle, for Vsync: unit=Hsync */
    UINT32  PulseWidth;     /* unit=cycle */
    UINT32  Polarity;       /* 0: Active Low, 1: Active High */
} AMBA_DSP_VIN_SYNC_WAVEFORM_s;

typedef struct _AMBA_DSP_VIN_TRIGGER_PULSE_s_ {
    int    Enable;
    AMBA_DSP_VIN_PULSE_POLARITY_e PulsePolarity;     /* Polarity of the trigger pulse */
    UINT16 PulseStartLine;                           /* Start line of the pulse */
    UINT16 PulseEndLine;                             /* End line of the pulse */
} AMBA_DSP_VIN_TRIGGER_PULSE_s;

typedef struct _AMBA_DSP_VIN_DVP_CONFIG_s_ {
    AMBA_DSP_VIN_INFO_s                 Info;
    AMBA_DSP_VIN_DVP_TYPE_e             DvpType;        /* Type of parallel transmission (DVP) */
    AMBA_DSP_VIN_SYNC_TYPE_e            SyncType;       /* BT.601/BT.656 line sync and frame sync */
    AMBA_DSP_VIN_SIGNAL_EDGE_TYPE_e     DataClockEdge;  /* Data are valid on rising/falling clock edge */
    AMBA_DSP_VIN_SIGNAL_EDGE_TYPE_e     HsyncPolarity;  /* Leading edge of H-sync/line-sync pulse */
    AMBA_DSP_VIN_SIGNAL_EDGE_TYPE_e     VsyncPolarity;  /* Leading edge of V-sync/frame-sync pulse */
    AMBA_DSP_VIN_SIGNAL_EDGE_TYPE_e     FieldPolarity;  /* Leading edge of field pulse */
    AMBA_DSP_VIN_SYNC_PIN_SELECT_s      SyncPinSelect;
    AMBA_DSP_VIN_RX_HV_SYNC_s           RxHvSyncCtrl;   /* Input H/V sync signal format (from sensor) */
    AMBA_DSP_VIN_VOUT_SYNC_s            VinVoutSync[AMBA_DSP_NUM_VIN_VOUT_SYNC];
} AMBA_DSP_VIN_DVP_CONFIG_s;

typedef struct _AMBA_DSP_VIN_SLVS_CONFIG_s_ {
    AMBA_DSP_VIN_INFO_s                     Info;
    AMBA_DSP_VIN_SLVS_PIN_PAIR_e            *pLaneMapping;  /* LVDS logical to physical lane mapping (set NULL if non-LVDS interface) */
    UINT8                                   NumActiveLanes; /* Number of active data lanes */
    AMBA_DSP_VIN_SLVS_SYNC_CODE_s           SyncDetectCtrl; /* Sync code detection control */
    AMBA_DSP_VIN_RX_HV_SYNC_s               RxHvSyncCtrl;   /* Input H/V sync signal format (from sensor) */
    AMBA_DSP_VIN_SPILT_CTRL_s               SplitCtrl;      /* Vin split function control */
    AMBA_DSP_VIN_TRIGGER_PULSE_s            VinTrigPulse[AMBA_DSP_NUM_VIN_TRIGGER_PULSE];
    AMBA_DSP_VIN_VOUT_SYNC_s                VinVoutSync[AMBA_DSP_NUM_VIN_VOUT_SYNC];
} AMBA_DSP_VIN_SLVS_CONFIG_s;

typedef struct _AMBA_DSP_VIN_MIPI_CONFIG_s_ {
    AMBA_DSP_VIN_INFO_s                 Info;
    AMBA_DSP_VIN_MIPI_CSI2_DATA_TYPE_e  DataType;       /* Data type of MIPI packet */
    UINT8                               DataTypeMask;   /* Data type mask of MIPI packet */
    UINT8                               NumActiveLanes; /* Number of active data lanes */
    AMBA_DSP_VIN_RX_HV_SYNC_s           RxHvSyncCtrl;   /* Input H/V sync signal format (from sensor) */
    AMBA_DSP_VIN_TRIGGER_PULSE_s        VinTrigPulse[AMBA_DSP_NUM_VIN_TRIGGER_PULSE];
    AMBA_DSP_VIN_VOUT_SYNC_s            VinVoutSync[AMBA_DSP_NUM_VIN_VOUT_SYNC];

    AMBA_VIN_MPHY_CONFIG_s              MipiCtrl;       /* MIPI M-PHY configuration */
} AMBA_DSP_VIN_MIPI_CONFIG_s;

typedef struct _AMBA_DSP_VIN_MASTER_SYNC_CONFIG_s_ {
    AMBA_DSP_VIN_SYNC_WAVEFORM_s    HSync;               /* waveform of Hsync output */
    AMBA_DSP_VIN_SYNC_WAVEFORM_s    VSync;               /* waveform of Vsync output */
    UINT16                          VSyncDelayCycles;    /* offset between Hsync and Vsync edges */
    UINT8                           ToggleHsyncInVblank; /* 1: toggle Hsync during Vblank */
} AMBA_DSP_VIN_MASTER_SYNC_CONFIG_s;
/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaDSP_Main.c
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_DSP_VIN_ENC_FRAME_RATE_e_ {
    VIN_ENC_FRAME_RATE_29_97_I = 0,     /* non-integral frame rate */
    VIN_ENC_FRAME_RATE_29_97_P,
    VIN_ENC_FRAME_RATE_59_94_I,
    VIN_ENC_FRAME_RATE_59_94_P,
} AMBA_DSP_VIN_ENC_FRAME_RATE_e;

typedef enum _AMBA_DSP_FRAME_RATE_TYPE_e_ {
    VOUT_FRAME_RATE = 0,
    VIN_ENC_FRAME_RATE,
    FIX_POINT_FRAME_RATE,               /* new fixed point 16.16 format */
} AMBA_DSP_FRAME_RATE_TYPE_e;

int AmbaDSP_FrameRateMap(AMBA_DSP_FRAME_RATE_TYPE_e Type, AMBA_DSP_FRAME_RATE_s *pFrameRate);

/*---------------------------------------------------------------------------*\
 * Defined in AmbaHL_VIN.c
\*---------------------------------------------------------------------------*/
int AmbaDSP_VinCaptureConfig(int VinID, AMBA_DSP_WINDOW_s *pCaptureWindow);

int AmbaDSP_VinPhySetDVP(int VinID);
int AmbaDSP_VinPhySetSLVS(int VinID);
int AmbaDSP_VinPhySetMIPI(int VinID);

int AmbaDSP_VinConfigSLVS(int VinID, AMBA_DSP_VIN_SLVS_CONFIG_s *pVinSlvsConfig);
int AmbaDSP_VinConfigMIPI(int VinID, AMBA_DSP_VIN_MIPI_CONFIG_s *pVinMipiConfig);
int AmbaDSP_VinConfigDVP(int VinID, AMBA_DSP_VIN_DVP_CONFIG_s *pVinDvpConfig);

int AmbaDSP_VinConfigMasterSync(int VinID, AMBA_DSP_VIN_MASTER_SYNC_CONFIG_s *pVinSyncConfig);

#endif  /* _AMBA_DSP_VIN_H_ */
