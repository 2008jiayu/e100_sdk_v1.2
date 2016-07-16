/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaImg_Proc.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Sample AE(Auto Exposure) and AWB(Auto White Balance) algorithms
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_IMG_PROC_
#define _AMBA_IMG_PROC_

#include "AmbaKAL.h"

#define DISABLE            0
#define ENABLE             1

#define MAX_AEB_NUM 9
#define MAX_CHAN_NUM 12

/* Image Pipe Mode */
#define IP_EXPERSS_MODE      0
#define IP_HYBRID_MODE       1

/* Image Related Mode */
#define IP_MODE_VIDEO      0
#define IP_MODE_STILL      1

#define IP_MODE_LISO_STILL 2
#define IP_MODE_HISO_STILL 3
#define IP_MODE_LISO_VIDEO 4
#define IP_MODE_HISO_VIDEO 5

#define IP_MODE_LISO_VIDEO 4
#define IP_MODE_HISO_VIDEO 5

/* Flow Related Mode */
#define IP_PREVIEW_MODE    0
#define IP_FASTFOCUS_MODE  1
#define IP_CAPTURE_MODE    2
#define IP_PREFLASH_MODE   3

/** vin_changed type */
#define VIN_CHG_NORMAL        0
#define VIN_CHG_FAST        1
#define VIN_CHG_FLOCKED        2
#define VIN_CHG_BG_DONE        3
/********************/

#define IP_CAP_NO_AEB           0x0000
#define IP_CAP_AEB              0x0008

#define AMBA_IMG_AE_TSK_PRIORITY               20
#define AMBA_IMG_AE_TSK_STACK_SIZE             16384
#define AMBA_IMG_AE_TSK_CORE_EXCLUSIVE_BIT_MAP 0xFFFE  
#define AMBA_IMG_AWB_TSK_PRIORITY               30
#define AMBA_IMG_AWB_TSK_STACK_SIZE             2048
#define AMBA_IMG_AWB_TSK_CORE_EXCLUSIVE_BIT_MAP 0xFFFE
#define AMBA_IMG_ADJ_TSK_PRIORITY               50
#define AMBA_IMG_ADJ_TSK_STACK_SIZE             8192
#define AMBA_IMG_ADJ_TSK_CORE_EXCLUSIVE_BIT_MAP 0xFFFE
#define AMBA_IMG_OB_TSK_PRIORITY                45
#define AMBA_IMG_OB_TSK_STACK_SIZE              8192    
#define AMBA_IMG_OB_TSK_CORE_EXCLUSIVE_BIT_MAP  0xFFFE 

#define IMG_NORMAL_MODE        0   //Without HDR
#define IMG_DSP_HDR_MODE_0     1   //DSP HDR, 2 exposures
#define IMG_DSP_HDR_MODE_1     2   //DSP HDR, 3 exposures

#define IMG_SENSOR_HDR_MODE_0  10  //Sensor HDR

/*-----------------------------------------------------------------------------------------------*\
 * AE Detail Info
 \*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_AE_INFO_s_ {
    UINT16     EvIndex;
    UINT16     NfIndex;
    INT16      ShutterIndex;
    INT16      AgcIndex;
    INT16      IrisIndex;
    INT32      Dgain;
    UINT16     IsoValue;
    UINT16     Flash;
    UINT16     Mode;
    float      ShutterTime;
    float      AgcGain;
    UINT16     Target;
    UINT16     LumaStat;
    INT16      LimitStatus;
    UINT16     Reserved[2];
    float      SensorDgain;
} AMBA_AE_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * AEB Detail Info
 \*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_AEB_INFO_s {
    UINT8    Num;        // 1~5
    INT8    EvBias[5];    // unit ev is 32, +4 31/32 EV ~ -4 31/32 EV
} AMBA_AEB_INFO_s;


/*-----------------------------------------------------------------------------------------------*\
 * luma, R, G and B values of each tile
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_AWB_TILES_VALUE_s_ {
    UINT16 R;
    UINT16 G;
    UINT16 B;
    UINT16 Y;
} AMBA_AWB_TILES_VALUE_s;

typedef struct _AMBA_AWB_TILES_INFO_s_ {
    UINT16 Rows;
    UINT16 Cols;
    AMBA_AWB_TILES_VALUE_s *pTilesValue;
} AMBA_AWB_TILES_INFO_s;

typedef struct _AMBA_AE_TILES_INFO_s_ {
    UINT16 Rows;
    UINT16 Cols;
    UINT16 TilesValue[1024];
} AMBA_AE_TILES_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * 3A Status Info
 \*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_3A_STATUS_s_ {
   UINT8 Ae;
   UINT8 Awb;
   UINT8 Af;
} AMBA_3A_STATUS_s;

/*-----------------------------------------------------------------------------------------------*\
 * 3A Operation Info
 \*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_3A_OP_INFO_s_ {
   UINT8  AeOp;
    UINT8  AwbOp;
    UINT8  AfOp;
    UINT8  AdjOp;
    UINT8  Reserved;
    UINT8  Reserved1;
    UINT8  Reserved2;
    UINT8  Reserved3;
} AMBA_3A_OP_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * WB simulation Info
\*-----------------------------------------------------------------------------------------------*/
typedef struct _WB_SIM_INFO_s_ {
    UINT16  LumaIdx;
    UINT16  OutDoorIdx;
    UINT16  HighLightIdx;
    UINT16  LowLightIdx;
    UINT32  AwbRatio[2];
    UINT32  Reserved;
    UINT32  Reserved1;
    UINT32  Reserved2;
    UINT32  Reserved3;
} WB_SIM_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * RAW Info
\*-----------------------------------------------------------------------------------------------*/
typedef struct AMBA_IMG_RAW_WINDOW_s_ {
    UINT16  OffsetX;                    /* Horizontal offset of the window */
    UINT16  OffsetY;                    /* Vertical offset of the window */
    UINT16  Width;                      /* Number of pixels per line in the window */
    UINT16  Height;                     /* Number of lines in the window */
} AMBA_IMG_RAW_WINDOW_s;

typedef struct AMBA_IMG_RAW_INFO_s_ {
    UINT8   Compressed;                 /* 1 - compressed raw data, 0 - uncompressed raw data */
    UINT8   *pBaseAddr;                 /* pointer to raw buffer */
    UINT16  Pitch;                      /* raw buffer pitch */
    UINT8   BayerPattern;   /* Bayer pattern for Sensor/RGB input only, 0:RG 1:BG 2:GR 3:GB */
    UINT32  NumDataBits;    /* Bit depth of pixel data */
    AMBA_IMG_RAW_WINDOW_s   Window;         /* Window position and size */
    AMBA_IMG_RAW_WINDOW_s   OBWindow;         /* OBWindow position and size */
} AMBA_IMG_RAW_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 * Image Proc callback function pointers
\*-----------------------------------------------------------------------------------------------*/
typedef struct _IMG_PROC_FUNC_s_ {
    void    (* AeAwbAdj_Init)(UINT32 chNo,UINT8 initFlg,AMBA_KAL_BYTE_POOL_t *pMMPL);
    void    (* AeAwbAdj_Control)(UINT32 chNo);
    void    (* Ae_Ctrl)(UINT32 chNo);
    void    (* Awb_Ctrl)(UINT32 chNo);
    void    (* Adj_Ctrl)(UINT32 chNo);
} IMG_PROC_FUNC_s;

/*-----------------------------------------------------------------------------------------------*\
 * ImgProc Memory Info
 \*-----------------------------------------------------------------------------------------------*/
typedef struct _IMG_PROC_MEM_INFO_s_ {
    void   *pMemAddr;
    UINT32 MemSize;
} IMG_PROC_MEM_INFO_s;

typedef struct _AMBA_IMG_TSK_Info_s_ {
    UINT32 Priority; ///< task priority
    UINT32 StackSize; ///< task stack size
    UINT32 CoreExclusiveBitMap; ///< exclusive bitmap of core. Bit[0] = 1 means core #0 is excluded
    UINT32  Reserved;    
} AMBA_IMG_TSK_Info_s;

#endif  /* _AMBA_AE_AWB_ */
