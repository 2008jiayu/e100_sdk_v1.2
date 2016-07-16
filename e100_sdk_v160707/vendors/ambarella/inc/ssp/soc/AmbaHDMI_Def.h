/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaHDMI_Def.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for HDMI APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_HDMI_DEF_H_
#define _AMBA_HDMI_DEF_H_

#include "AmbaVideoTiming.h"

#define AMBA_HDMI_CEC_NUM_BLOCK             16      /* Total number of header and data blocks */

/*-----------------------------------------------------------------------------------------------*\
 * Definitions for HDMI Audio & Speaker Information
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_HDMI_AIC_EXTENDED_OFFSET       0x10    /* AICs after this offset are the Audio Format Code Extension */

typedef enum _AMBA_HDMI_AUDIO_FORMAT_ID_CODE_e_ {
    /* Code Short Name                        ID    Encoding Standard */
    HDMI_AIC_RESERVED                       = 0,    /* N/A */
    HDMI_AIC_LPCM                           = 1,    /* L-PCM, IEC 60958-3 */
    HDMI_AIC_AC3                            = 2,    /* AC-3, ATSC A/52B excluding Annex E */
    HDMI_AIC_MPEG1                          = 3,    /* ISO/IEC 11172-3 Layer 1 or Layer 2 */
    HDMI_AIC_MP3                            = 4,    /* ISO/IEC 11172-3 Layer 3 */
    HDMI_AIC_MPEG2                          = 5,    /* ISO/IEC 13818-3 */
    HDMI_AIC_AAC_LC                         = 6,    /* ISO/IEC 14496-3 */
    HDMI_AIC_DTS                            = 7,    /* ETSI TS 102 114 */
    HDMI_AIC_ATRAC                          = 8,    /* IEC 61909, See also ATRAC */
    HDMI_AIC_DSD                            = 9,    /* ISO/IEC 14496-3 subpart 10, See also Super Audio CD */
    HDMI_AIC_EAC3                           = 10,   /* E-AC-3, ATSC A/52B with Annex E */
    HDMI_AIC_DTSHD                          = 11,   /* DTS-HD, DVD Forum DTS-HD */
    HDMI_AIC_MLP                            = 12,   /* DVD Forum MLP */
    HDMI_AIC_DST                            = 13,   /* ISO/IEC 14496-3 subpart 10 */
    HDMI_AIC_WMA_PRO                        = 14,   /* WMA Pro Decoder Specification */

    HDMI_AIC_HEAAC                          = 17,   /* HE-AAC, ISO/IEC 14496-3:2005 */
    HDMI_AIC_HEAACV2                        = 18,   /* HE-AACv2, ISO/IEC 14496-3:2005/AMD2:2006 */
    HDMI_AIC_MPEG_SURROUND                  = 19,   /* ISO/IEC 23003-1:2007 */

    AMBA_NUM_HDMI_AIC                               /* Total Number of audio format codes */
} AMBA_HDMI_AUDIO_FORMAT_ID_CODE_e;

typedef enum _HDMI_AUDIO_CLOCK_FREQUENCY_e_ {
    HDMI_AUDIO_CLK_FREQ_128FS               = 128,  /* Audio clock freq. = 128*fs */
    HDMI_AUDIO_CLK_FREQ_256FS               = 256,  /* Audio clock freq. = 256*fs */
    HDMI_AUDIO_CLK_FREQ_384FS               = 384,  /* Audio clock freq. = 384*fs */
    HDMI_AUDIO_CLK_FREQ_512FS               = 512,  /* Audio clock freq. = 512*fs */
    HDMI_AUDIO_CLK_FREQ_768FS               = 768   /* Audio clock freq. = 768*fs */
} HDMI_AUDIO_CLOCK_FREQUENCY_e;

typedef enum _HDMI_AUDIO_SAMPLE_RATE_e_ {
    HDMI_AUDIO_FS_32K = 0,                  /* Sample rate =  32000 */
    HDMI_AUDIO_FS_44P1K,                    /* Sample rate =  44100 */
    HDMI_AUDIO_FS_48K,                      /* Sample rate =  48000 */
    HDMI_AUDIO_FS_88P2K,                    /* Sample rate =  88200 */
    HDMI_AUDIO_FS_96K,                      /* Sample rate =  96000 */
    HDMI_AUDIO_FS_176P4K,                   /* Sample rate = 176400 */
    HDMI_AUDIO_FS_192K,                     /* Sample rate = 192000 */

    AMBA_NUM_HDMI_AUDIO_FS                  /* Total Number of Audio sample rates */
} HDMI_AUDIO_SAMPLE_RATE_e;


typedef enum _HDMI_TMDS_CLOCK_e_ {
    HDMI_TMDS_CLK_25174825 = 0,             /* TMDS Clock =  25.20 / 1.001 MHz */
    HDMI_TMDS_CLK_25200000,                 /* TMDS Clock =  25.20         MHz */
    HDMI_TMDS_CLK_27000000,                 /* TMDS Clock =  27.00         MHz */
    HDMI_TMDS_CLK_27027000,                 /* TMDS Clock =  27.00 * 1.001 MHz */
    HDMI_TMDS_CLK_54000000,                 /* TMDS Clock =  54.00         MHz */
    HDMI_TMDS_CLK_54054000,                 /* TMDS Clock =  54.00 * 1.001 MHz */
    HDMI_TMDS_CLK_59340659,                 /* TMDS Clock =  59.40 / 1.001 MHz */
    HDMI_TMDS_CLK_59400000,                 /* TMDS Clock =  59.40         MHz */
    HDMI_TMDS_CLK_72000000,                 /* TMDS Clock =  72.00         MHz */
    HDMI_TMDS_CLK_74175824,                 /* TMDS Clock =  74.25 / 1.001 MHz */
    HDMI_TMDS_CLK_74250000,                 /* TMDS Clock =  74.25         MHz */
    HDMI_TMDS_CLK_108000000,                /* TMDS Clock = 108.00         MHz */
    HDMI_TMDS_CLK_108108000,                /* TMDS Clock = 108.00 * 1.001 MHz */
    HDMI_TMDS_CLK_148351648,                /* TMDS Clock = 148.50 / 1.001 MHz */
    HDMI_TMDS_CLK_148500000,                /* TMDS Clock = 148.50         MHz */
    HDMI_TMDS_CLK_296703296,                /* TMDS Clock = 297.00 / 1.001 MHz */
    HDMI_TMDS_CLK_297000000,                /* TMDS Clock = 297.00         MHz */

    AMBA_NUM_HDMI_TMDS_CLOCK                /* Total Number of TMDS Clock frequencies */
} HDMI_TMDS_CLOCK_e;

typedef enum _HDMI_AUDIO_CHANNEL_ALLOC_e_ {
    /* Code Short Name                      CA Code    CH1 CH2 CH3 CH4 CH5 CH6 CH7 CH8 */
    HDMI_CA_2CH_FL_FR                       = 0,    /* FL  FR  -   -   -   -   -   -   */
    HDMI_CA_3CH_FL_FR_LFE                   = 1,    /* FL  FR  LFE -   -   -   -   -   */
    HDMI_CA_3CH_FL_FR_FC                    = 2,    /* FL  FR  -   FC  RC  -   -   -   */
    HDMI_CA_4CH_FL_FR_LFE_FC                = 3,    /* FL  FR  LFE FC  RC  -   -   -   */
    HDMI_CA_3CH_FL_FR_RC                    = 4,    /* FL  FR  -   -   RC  -   -   -   */
    HDMI_CA_4CH_FL_FR_LFE_RC                = 5,    /* FL  FR  LFE -   RC  -   -   -   */
    HDMI_CA_4CH_FL_FR_FC_RC                 = 6,    /* FL  FR  -   FC  RC  -   -   -   */
    HDMI_CA_5CH_FL_FR_LFE_FC_RC             = 7,    /* FL  FR  LFE FC  RC  -   -   -   */
    HDMI_CA_5CH_FL_FR_RL_RR                 = 8,    /* FL  FR  -   -   RL  RR  -   -   */
    HDMI_CA_5CH_FL_FR_LFE_RL_RR             = 9,    /* FL  FR  LFE -   RL  RR  -   -   */
    HDMI_CA_5CH_FL_FR_FC_RL_RR              = 10,   /* FL  FR  -   FC  RL  RR  -   -   */
    HDMI_CA_6CH_FL_FR_LFE_FC_RL_RR          = 11,   /* FL  FR  LFE FC  RL  RR  -   -   */
    HDMI_CA_5CH_FL_FR_RL_RR_RC              = 12,   /* FL  FR  -   -   RL  RR  RC  -   */
    HDMI_CA_6CH_FL_FR_LFE_RL_RR_RC          = 13,   /* FL  FR  LFE -   RL  RR  RC  -   */
    HDMI_CA_6CH_FL_FR_FC_RL_RR_RC           = 14,   /* FL  FR  -   FC  RL  RR  RC  -   */
    HDMI_CA_7CH_FL_FR_LFE_FC_RL_RR_RC       = 15,   /* FL  FR  LFE FC  RL  RR  RC  -   */
    HDMI_CA_6CH_FL_FR_RL_RR_RLC_RRC         = 16,   /* FL  FR  -   -   RL  RR  RLC RRC */
    HDMI_CA_7CH_FL_FR_LFE_RL_RR_RLC_RRC     = 17,   /* FL  FR  LFE -   RL  RR  RLC RRC */
    HDMI_CA_7CH_FL_FR_FC_RL_RR_RLC_RRC      = 18,   /* FL  FR  -   FC  RL  RR  RLC RRC */
    HDMI_CA_8CH_FL_FR_LFE_FC_RL_RR_RLC_RRC  = 19,   /* FL  FR  LFE FC  RL  RR  RLC RRC */
    HDMI_CA_4CH_FL_FR_FLC_FRC               = 20,   /* FL  FR  -   -   -   -   FLC FRC */
    HDMI_CA_5CH_FL_FR_LFE_FLC_FRC           = 21,   /* FL  FR  LFE -   -   -   FLC FRC */
    HDMI_CA_5CH_FL_FR_FC_FLC_FRC            = 22,   /* FL  FR  -   FC  RC  -   FLC FRC */
    HDMI_CA_6CH_FL_FR_LFE_FC_FLC_FRC        = 23,   /* FL  FR  LFE FC  RC  -   FLC FRC */
    HDMI_CA_5CH_FL_FR_RC_FLC_FRC            = 24,   /* FL  FR  -   -   RC  -   FLC FRC */
    HDMI_CA_6CH_FL_FR_LFE_RC_FLC_FRC        = 25,   /* FL  FR  LFE -   RC  -   FLC FRC */
    HDMI_CA_6CH_FL_FR_FC_RC_FLC_FRC         = 26,   /* FL  FR  -   FC  RC  -   FLC FRC */
    HDMI_CA_7CH_FL_FR_LFE_FC_RC_FLC_FRC     = 27,   /* FL  FR  LFE FC  RC  -   FLC FRC */
    HDMI_CA_6CH_FL_FR_RL_RR_FLC_FRC         = 28,   /* FL  FR  -   -   RL  RR  FLC FRC */
    HDMI_CA_6CH_FL_FR_LFE_RL_RR_FLC_FRC     = 29,   /* FL  FR  LFE -   RL  RR  FLC FRC */
    HDMI_CA_6CH_FL_FR_FC_RL_RR_FLC_FRC      = 30,   /* FL  FR  -   FC  RL  RR  FLC FRC */
    HDMI_CA_7CH_FL_FR_LFE_FC_RL_RR_FLC_FRC  = 31,   /* FL  FR  LFE FC  RL  RR  FLC FRC */

    AMBA_HDMI_NUM_AUDIO_CA                          /* Number of audio/speaker channel allocations */
} HDMI_AUDIO_CHANNEL_ALLOC_e;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions for HDMI Data Island Packets
\*-----------------------------------------------------------------------------------------------*/
typedef enum _HDMI_PACKET_TYPE_e_ {
    HDMI_PACKET_NULL                        = 0x00, /* NULL packet */
    HDMI_PACKET_AUDIO_CLOCK_REGEN           = 0x01, /* Audio Clock Regeneration packet */
    HDMI_PACKET_AUDIO_SAMPLE                = 0x02, /* Audio Sample packet */
    HDMI_PACKET_GENERAL_CTRL                = 0x03, /* General Control packet */
    HDMI_PACKET_ACP                         = 0x04, /* Audio Content Protection packet */
    HDMI_PACKET_ISRC1                       = 0x05, /* ISRC packet */
    HDMI_PACKET_ISRC2                       = 0x06, /* ISRC packet */
    HDMI_PACKET_ONE_BIT_AUDIO_SAMPLE        = 0x07, /* One Bit Audio Sample packet */
    HDMI_PACKET_DST_AUDIO                   = 0x08, /* DST Audio packet */
    HDMI_PACKET_HBR_AUDIO_STREAM            = 0x09, /* High-Bitrate Audio Stream packet */
    HDMI_PACKET_GAMUT_METADATA              = 0x0A, /* Gamut Metadata packet */

    HDMI_NUM_PACKET_TYPE                            /* Number of packet types (excludes InfoFrame packets) */
} HDMI_PACKET_TYPE_e;

typedef enum _AMBA_HDMI_INFOFRAME_TYPE_e_ {
    HDMI_INFOFRAME_RESERVED = 0,
    HDMI_INFOFRAME_VENDOR_SPECIFIC,                 /* Vendor-Specific InfoFrame packet */
    HDMI_INFOFRAME_AVI,                             /* Auxiliary Video Information InfoFrame packet */
    HDMI_INFOFRAME_SPD,                             /* Source Product Descriptor InfoFrame packet */
    HDMI_INFOFRAME_AUDIO,                           /* Audio InfoFrame packet */
    HDMI_INFOFRAME_MPEG_SOURCE,                     /* MPEG Source InfoFrame packet */

    HDMI_NUM_INFOFRAME_TYPE                         /* Number of InfoFrame types (CEA-861-D) */
} AMBA_HDMI_INFOFRAME_TYPE_e;

#define HDMI_INFOFRAME_SIZE_MAX             27      /* Maximum size of InfoFrame packet payload */
#define HDMI_INFOFRAME_SIZE_AVI             13      /* Payload size of AVI InfoFrame packet */
#define HDMI_INFOFRAME_SIZE_SPD             25      /* Payload size of Source Product Description InfoFrame packet */
#define HDMI_INFOFRAME_SIZE_AUDIO           10      /* Payload size of Audio InfoFrame packet */
#define HDMI_INFOFRAME_SIZE_MPEG_SOURCE     10      /* Payload size of MPEG Source InfoFrame packet */

typedef union _AMBA_HDMI_DATA_ISLAND_PACKET_HEADER_u_ {
    UINT32  Data;                                   /* this is an 8-bit data */

    struct {
        UINT32  Type:           7;                  /* Packet/InfoFrame Type */
        UINT32  InfoFrameFlag:  1;                  /* InfoFrame Packet Type Indicator */
        UINT32  Version:        8;                  /* Version of Packet/InfoFrame format */
        UINT32  Length:         8;                  /* Number of bytes in packet payload */
        UINT32  Checksum:       8;                  /* Checksum of the packet */
    } Bits;
} AMBA_HDMI_DATA_ISLAND_PACKET_HEADER_u;

typedef struct _AMBA_HDMI_DATA_ISLAND_PACKET_s_ {
    AMBA_HDMI_DATA_ISLAND_PACKET_HEADER_u   Header; /* Packet header */
    UINT8  Payload[HDMI_INFOFRAME_SIZE_MAX];        /* Packet payload */
    UINT8  Reserved;                                /* Reserved for 32-bit alignment */
} AMBA_HDMI_DATA_ISLAND_PACKET_s;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions for Cable State
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_HDMI_CABLE_DETECT_e_ {
    AMBA_HDMI_CABLE_DETECT_NONE = 0,                    /* No cable is plugged in */
    AMBA_HDMI_CABLE_DETECT_DVI_ACTIVE,                  /* DVI cable is plugged in and in active state */
    AMBA_HDMI_CABLE_DETECT_DVI_STANDBY,                 /* DVI cable is plugged in and in standby state */
    AMBA_HDMI_CABLE_DETECT_HDMI_ACTIVE,                 /* HDMI cable is plugged in and in active state */
    AMBA_HDMI_CABLE_DETECT_HDMI_STANDBY                 /* HDMI cable is plugged in and in standby state */
} AMBA_HDMI_CABLE_DETECT_e;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions for Video Configuration
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_HDMI_VIDEO_FRAME_LAYOUT_e_ {
    AMBA_HDMI_VIDEO_2D                                  = 0,    /* 2D */
    AMBA_HDMI_VIDEO_3D_FRAME_PACKING                    = 1,    /* 3D Frame packing */
    AMBA_HDMI_VIDEO_3D_FIELD_ALTERNATIVE                = 2,    /* 3D Field alternative */
    AMBA_HDMI_VIDEO_3D_LINE_ALTERNATIVE                 = 3,    /* 3D Line alternative */
    AMBA_HDMI_VIDEO_3D_SIDE_BY_SIDE_FULL                = 4,    /* 3D Left-and-Right with full resolution */
    AMBA_HDMI_VIDEO_3D_L_DEPTH                          = 5,    /* 3D L + depth */
    AMBA_HDMI_VIDEO_3D_L_DEPTH_GFX_GFX_DEPTH            = 6,    /* 3D L + depth + Graphics + Graphics-depth */
    AMBA_HDMI_VIDEO_3D_TOP_AND_BOTTOM                   = 7,    /* 3D Top-and-Bottom with half vertical resolution */
    AMBA_HDMI_VIDEO_3D_SIDE_BY_SIDE_HALF_HORIZONTAL     = 8,    /* 3D Horizontal sub-sampling */

    AMBA_HDMI_VIDEO_3D_SIDE_BY_SIDE_HALF_QUINCUNX_LO_RO = 12,   /* 3D Quincunx: Odd/Left picture, Odd/Right picture */
    AMBA_HDMI_VIDEO_3D_SIDE_BY_SIDE_HALF_QUINCUNX_LO_RE = 13,   /* 3D Quincunx: Odd/Left picture, Even/Right picture */
    AMBA_HDMI_VIDEO_3D_SIDE_BY_SIDE_HALF_QUINCUNX_LE_RO = 14,   /* 3D Quincunx: Even/Left picture, Odd/Right picture */
    AMBA_HDMI_VIDEO_3D_SIDE_BY_SIDE_HALF_QUINCUNX_LE_RE = 15    /* 3D Quincunx: Even/Left picture, Even/Right picture */
} AMBA_HDMI_VIDEO_FRAME_LAYOUT_e;

typedef enum _AMBA_HDMI_QUANTIZATION_RANGE_e_ {
    AMBA_HDMI_QRANGE_DEFAULT = 0,                       /* RGB/YCC with a default quantization range depends on video format */
    AMBA_HDMI_QRANGE_LIMITED,                           /* RGB/YCC with a limited quantization range of 220 levels (16 to 235) */
    AMBA_HDMI_QRANGE_FULL                               /* RGB/YCC with a full quantization range of 256 levels (0 to 255) */
} AMBA_HDMI_QUANTIZATION_RANGE_e;

typedef enum _AMBA_HDMI_SINK_CURRENT_INCREMENT_e_ {
    AMBA_HDMI_SINK_CURRENT_ADD_0_MA = 0,                /* 0 mA added */
    AMBA_HDMI_SINK_CURRENT_ADD_0P675_MA,                /* 0.675 mA added */
    AMBA_HDMI_SINK_CURRENT_ADD_1P35_MA,                 /* 1.35 mA added */
    AMBA_HDMI_SINK_CURRENT_ADD_2P025_MA,                /* 2.025 mA added */
    AMBA_HDMI_SINK_CURRENT_ADD_2P7_MA,                  /* 2.7 mA added */
    AMBA_HDMI_SINK_CURRENT_ADD_3P375_MA,                /* 3.375 mA added */
    AMBA_HDMI_SINK_CURRENT_ADD_4P05_MA,                 /* 4.05 mA added */
    AMBA_HDMI_SINK_CURRENT_ADD_4P725_MA,                /* 4.725 mA added */

    AMBA_HDMI_NUM_SINK_CURRENT_INCREMENT                /* Number of sink current increment */
} AMBA_HDMI_SINK_CURRENT_INCREMENT_e;

typedef enum _AMBA_HDMI_SINK_PREEMPHASIS_MODE_e_ {
    AMBA_HDMI_SINK_PREEMPHASIS_0_PERCENT = 0,           /* No pre-emphasis */
    AMBA_HDMI_SINK_PREEMPHASIS_6P67_PERCENT,            /* 6.67% pre-emphasis */
    AMBA_HDMI_SINK_PREEMPHASIS_10P0_PERCENT,            /* 10.0% pre-emphasis */
    AMBA_HDMI_SINK_PREEMPHASIS_16P7_PERCENT,            /* 16.7% pre-emphasis */
    AMBA_HDMI_SINK_PREEMPHASIS_20P0_PERCENT,            /* 20.0% pre-emphasis */
    AMBA_HDMI_SINK_PREEMPHASIS_26P7_PERCENT,            /* 26.7% pre-emphasis */
    AMBA_HDMI_SINK_PREEMPHASIS_30P0_PERCENT,            /* 30.0% pre-emphasis */
    AMBA_HDMI_SINK_PREEMPHASIS_36P7_PERCENT,            /* 36.7% pre-emphasis */

    AMBA_HDMI_NUM_SINK_PREEMPHASIS_MODE                 /* Number of sink pre-emphasis mode */
} AMBA_HDMI_SINK_PREEMPHASIS_MODE_e;

/*-----------------------------------------------------------------------------------------------*\
 * Definitions for Audio Configuration
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_HDMI_AUDIO_CONFIG_s_ {
    HDMI_AUDIO_SAMPLE_RATE_e            SampleRate;     /* Audio sample rate */
    HDMI_AUDIO_CHANNEL_ALLOC_e          SpeakerAlloc;   /* Audio channel/speaker allocation ID */
} AMBA_HDMI_AUDIO_CONFIG_s;

typedef void (*AMBA_HDMI_ISR_f)(void);

#endif /* _AMBA_HDMI_DEF_H_ */
