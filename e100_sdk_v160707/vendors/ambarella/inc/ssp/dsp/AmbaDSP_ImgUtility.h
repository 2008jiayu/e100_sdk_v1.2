/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDSP_ImgUtility.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella DSP Utility APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DSP_IMG_UTIL_H_
#define _AMBA_DSP_IMG_UTIL_H_

#include "AmbaDSP_ImgDef.h"
#include "AmbaDataType.h"
#define AMBA_DSP_IMG_MAX_PIPE_NUM         8

typedef enum _AMBA_DSP_IMG_LISO_FEATURE_e_ {
    AMBA_DSP_IMG_NO_ADV_FEATURE = 0,
    AMBA_DSP_IMG_ADV_FEATURE_3_PASS_LISO = 1,
    AMBA_DSP_IMG_ADV_FEATURE_2_PASS_LISO = 2,
    AMBA_DSP_IMG_ADV_FEATURE_LISO_MAX
} AMBA_DSP_IMG_LISO_FEATURE_e;

typedef struct _AMBA_DSP_IMG_PIPE_INFO_s_ {
    AMBA_DSP_IMG_PIPE_e Pipe;
    UINT8               CtxBufNum;
    UINT8               CfgBufNum;
    UINT8               Reserved;
} AMBA_DSP_IMG_PIPE_INFO_s;

typedef struct _AMBA_DSP_IMG_ARCH_INFO_s_ {
    UINT8                    *pWorkBuf;
    UINT32                   BufSize;
    UINT32                   BufNum;
    UINT32                   PipeNum;
    AMBA_DSP_IMG_PIPE_INFO_s *pPipeInfo[AMBA_DSP_IMG_MAX_PIPE_NUM];
} AMBA_DSP_IMG_ARCH_INFO_s;

typedef struct _AMBA_DSP_IMG_CTX_INFO_s_ {
    AMBA_DSP_IMG_PIPE_e Pipe;
    UINT8               CtxId;    
    UINT8               ChanId;
    UINT8               Reserved;
} AMBA_DSP_IMG_CTX_INFO_s;

typedef struct _AMBA_DSP_IMG_CFG_INFO_s_ {
    AMBA_DSP_IMG_PIPE_e Pipe;
    UINT8               CfgId;
    UINT8               Reserved;
    UINT8               Reserved1;
} AMBA_DSP_IMG_CFG_INFO_s;

typedef enum _AMBA_DSP_IMG_CFG_STATE_e_ {
    AMBA_DSP_IMG_CFG_STATE_IDLE = 0x00,
    AMBA_DSP_IMG_CFG_STATE_INIT,
    AMBA_DSP_IMG_CFG_STATE_PREEXE,
    AMBA_DSP_IMG_CFG_STATE_POSTEXE,
} AMBA_DSP_IMG_CFG_STATE_e;

typedef enum _AMBA_DSP_IMG_CONFIG_EXECUTE_MODE_e_{
    AMBA_DSP_IMG_CFG_EXE_QUICK = 0x00,
    AMBA_DSP_IMG_CFG_EXE_FULLCOPY,
    AMBA_DSP_IMG_CFG_EXE_PARTIALCOPY,
} AMBA_DSP_IMG_CONFIG_EXECUTE_MODE_e;


typedef struct {
    UINT32 HisoCfgCommonUpdate : 1;
    UINT32 HisoCfgColorUpdate : 1;
    UINT32 HisoCfgMctfUpdate : 1;
    UINT32 HisoCfgStep01Update : 1;
    UINT32 HisoCfgStep02Update : 1;
    UINT32 HisoCfgStep03Update : 1;
    UINT32 HisoCfgStep04Update : 1;
    UINT32 HisoCfgStep05Update : 1;
    UINT32 HisoCfgStep06Update : 1;
    UINT32 HisoCfgStep07Update : 1;
    UINT32 HisoCfgStep08Update : 1;
    UINT32 HisoCfgStep09Update : 1;
    UINT32 HisoCfgStep10Update : 1;
    UINT32 HisoCfgStep11Update : 1;
    UINT32 HisoCfgStep12Update : 1;
    UINT32 HisoCfgStep15Update : 1;
    UINT32 HisoCfgStep16Update : 1;
    UINT32 HisoCfgVwarpUpdate : 1;
    UINT32 HisoCfgFlowUpdate : 1;
    UINT32 HisoCfgAaaUpdate : 1;
    UINT32 HisoCfgVideoMctfUpdate : 1;
    UINT32 Reserved : 11;
} AMBA_DSP_IMG_HISO_CFG_UPDATE_INFO_s;

typedef struct {
    UINT32 LisoCfgCommonUpdate : 1;
    UINT32 LisoCfgColorUpdate : 1;
    UINT32 LisoCfgMctfUpdate : 1;
    UINT32 LisoCfgStep01Update : 1;
    UINT32 LisoCfgReserved02Update : 1;
    UINT32 LisoCfgStep03Update : 1;
    UINT32 LisoCfgStep04Update : 1;
    UINT32 LisoCfgReserved05Update : 1;
    UINT32 LisoCfgReserved06Update : 1;
    UINT32 LisoCfgReserved07Update : 1;
    UINT32 LisoCfgReserved08Update : 1;
    UINT32 LisoCfgReserved09Update : 1;
    UINT32 LisoCfgReserved10Update : 1;
    UINT32 LisoCfgReserved11Update : 1;
    UINT32 LisoCfgReserved12Update : 1;
    UINT32 LisoCfgReserved15Update : 1;
    UINT32 LisoCfgReserved16Update : 1;
    UINT32 LisoCfgVwarpUpdate : 1;
    UINT32 LisoCfgFlowUpdate : 1;
    UINT32 LisoCfgAaaUpdate : 1;
    UINT32 LisoCfgVideoMctfUpdate : 1;
    UINT32 Reserved : 11;
} AMBA_DSP_IMG_LISO_CFG_UPDATE_INFO_s;

typedef struct {
    UINT8 Reserved;
} AMBA_DSP_IMG_HISO_CFG_INFO_s;

typedef struct {
    AMBA_DSP_IMG_LISO_FEATURE_e AdvMode;
} AMBA_DSP_IMG_LISO_CFG_INFO_s;

typedef struct _AMBA_DSP_IMG_CFG_STATUS_s_ {
    UINT32                   Addr;           /* Config buffer address. */
    union {
        AMBA_DSP_IMG_HISO_CFG_INFO_s Hiso;
        AMBA_DSP_IMG_LISO_CFG_INFO_s Liso;
        UINT32 Reserved;
    } CfgInfo;
    union {
        AMBA_DSP_IMG_HISO_CFG_UPDATE_INFO_s *pHiso;
        AMBA_DSP_IMG_LISO_CFG_UPDATE_INFO_s *pLiso;
    } CfgUpdateInfo;
    AMBA_DSP_IMG_CFG_STATE_e State;
    AMBA_DSP_IMG_ALGO_MODE_e AlgoMode;
    UINT8                    Reserved;
    UINT8                    Reserved1;
} AMBA_DSP_IMG_CFG_STATUS_s;

typedef struct _AMBA_DSP_IMG_HDR_CFG_STATUS_s_ {
    UINT32 Addr; /* Config buffer address */
} AMBA_DSP_IMG_HDR_CFG_STATUS_s;

typedef struct _AMBA_DSP_IMG_SIZE_INFO_s_ {
    UINT16  WidthIn;
    UINT16  HeightIn;
    UINT16  WidthMain;
    UINT16  HeightMain;
    UINT16  WidthPrevA;
    UINT16  HeightPrevA;
    UINT16  WidthPrevB;
    UINT16  HeightPrevB;
    UINT16  WidthScrn;
    UINT16  HeightScrn;
    UINT16  WidthQvRaw;
    UINT16  HeightQvRaw;
    UINT16  Reserved;
    UINT16  Reserved1;
} AMBA_DSP_IMG_SIZE_INFO_s;

typedef struct _AMBA_DSP_IMG_LISO_DEBUG_MODE_s_ {
    UINT8 Step;
    UINT8 Mode;
    UINT8 BreakMode;
    UINT8 ChannelID;
    UINT8 TileX;
    UINT8 TileY;
    UINT32 PicNum;
} AMBA_DSP_IMG_DEBUG_MODE_s;

typedef struct _AMBA_DSP_IMG_LIB_VERSION_s_ {
        int    major;
        int    minor;
        UINT32  reserved0;
        UINT32  reserved1;
        char description[64];
} AMBA_DSP_IMG_LIB_VERSION_s;


/*---------------------------------------------------------------------------*\
 * Defined in AmbaDSP_ImgUtility.c
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 * Pipeline related api 
\*---------------------------------------------------------------------------*/
int AmbaDSP_ImgQueryArchMemSize(AMBA_DSP_IMG_ARCH_INFO_s *pArchInfo);
int AmbaDSP_ImgInitArch(AMBA_DSP_IMG_ARCH_INFO_s *pArchInfo);
int AmbaDSP_ImgIsSupportVirtualMemory(void);
int AmbaDSP_ImgHdrCfgUsePhysBatchBufferAddr(AMBA_DSP_IMG_MODE_CFG_s *pMode);
int AmbaDSP_ImgHdrCfgUsePhysTblAddr(AMBA_DSP_IMG_MODE_CFG_s *pMode);
int AmbaDSP_ImgHdrCfgUseVertTblAddr(AMBA_DSP_IMG_MODE_CFG_s *pMode);
int AmbaDSP_ImgIsoCfgUseVirtTblAddr(AMBA_DSP_IMG_MODE_CFG_s *pMode);
int AmbaDSP_ImgIsoCfgUsePhysTblAddr(AMBA_DSP_IMG_MODE_CFG_s *pMode);
/*---------------------------------------------------------------------------*\
 * Context related api
\*---------------------------------------------------------------------------*/
/* To initialize the context. */
int AmbaDSP_ImgInitCtx(
                  UINT8                       InitMode,    
                  /* 0: reset destination context to default settings. */ 
                  /* 1: clone from source context to destination context. */
                  UINT8                       DefblcEnb,    
                  /* Available in reset mode only.            */
                  /* 0: reset with deferred black level off.  */
                  /* 1: reset with deferred black level on.   */
                  AMBA_DSP_IMG_CTX_INFO_s     *pDestCtx,
                  AMBA_DSP_IMG_CTX_INFO_s     *pSrcCtx);
        
/*---------------------------------------------------------------------------*\
 * Config related api 
\*---------------------------------------------------------------------------*/
/* To initialize the config buffer layout to certain algo mode settings. */
int AmbaDSP_ImgInitCfg(AMBA_DSP_IMG_CFG_INFO_s *pCfgInfo, AMBA_DSP_IMG_ALGO_MODE_e AlgoMode);

/* To pre-execute the config from CtxId using AlgoMode and FuncMode method. */
int AmbaDSP_ImgPreExeCfg(
                  AMBA_DSP_IMG_MODE_CFG_s     *pMode,
                  AMBA_DSP_IMG_CONFIG_EXECUTE_MODE_e ExeMode);   
                    /* 0: fast execute. Point to content in context memory buffer. */
                    /* 1: full copy execute. Copy content from context memory to config memory. */



/* To post-execute the config from CtxId using AlgoMode and FuncMode method. */
int AmbaDSP_ImgPostExeCfg(
                  AMBA_DSP_IMG_MODE_CFG_s     *pMode,
                  AMBA_DSP_IMG_CONFIG_EXECUTE_MODE_e ExeMode);   
                    /* 0: fast execute. Point to content in context memory buffer. */
                    /* 1: full copy execute. Copy content from context memory to config memory. */

int AmbaDSP_ImgGetDspWriteBackVideoIsoCfgId(UINT8 *pWriteBackCfgId);

int AmbaDSP_ImgSetIsoCfgTag(AMBA_DSP_IMG_MODE_CFG_s *pMode, UINT32 IsoCfgTag);

int AmbaDSP_ImgSetDiagCaseId(AMBA_DSP_IMG_MODE_CFG_s *pMode, UINT8 DiagCaseId);

int AmbaDSP_ImgSetSizeInfo(
                    AMBA_DSP_IMG_MODE_CFG_s     *pMode,
                    AMBA_DSP_IMG_SIZE_INFO_s    *pSizeInfo);

/* To Get the config status, including address and size. */
int AmbaDSP_ImgGetCfgStatus(AMBA_DSP_IMG_CFG_INFO_s *pCfgInfo, AMBA_DSP_IMG_CFG_STATUS_s *pStatus);

int AmbaDSP_ImgGetLibVersion(AMBA_DSP_IMG_LIB_VERSION_s* pAmbaDSPlibVersion);
#endif  /* _AMBA_DSP_IMG_UTIL_H_ */
