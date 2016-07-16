/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDSP_VideoDec.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella DSP Decoder APIs
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DSP_VIDEO_DEC_H_
#define _AMBA_DSP_VIDEO_DEC_H_

#include "AmbaDSP.h"

#define AMBA_DSP_MAX_NUM_VIDEO_DEC_STREAM      4   /* maximum number of Video stream */

typedef enum _AMBA_DSP_H264DEC_STOP_OPTION_e_ {
    AMBA_DSP_H264DEC_STOP_OPTION_NORMAL = 0,
    AMBA_DSP_H264DEC_STOP_OPTION_KEEP_LAST_DISP
} AMBA_DSP_H264DEC_STOP_OPTION_e;

typedef enum _AMBA_DSP_H264DEC_DIR_e_ {
    AMBA_DSP_H264DEC_DIR_FORWARD = 0,
    AMBA_DSP_H264DEC_DIR_BACKWARD
} AMBA_DSP_H264DEC_DIR_e;

typedef enum _AMBA_DSP_H264DEC_TRICK_PLAY_e_ {
    AMBA_DSP_H264DEC_TRICK_PLAY_PAUSE = 0,
    AMBA_DSP_H264DEC_TRICK_PLAY_RESUME,
    AMBA_DSP_H264DEC_TRICK_PLAY_STEP
} AMBA_DSP_H264DEC_TRICK_PLAY_e;

typedef enum _AMBA_DSP_DEC_TYPE_e_ {
    AMBA_DSP_DEC_TYPE_NORMAL = 0,       /* For ambarella clips */
    AMBA_DSP_DEC_TYPE_LOW_DELAY,
    AMBA_DSP_DEC_TYPE_COMPLIANT
} AMBA_DSP_DEC_TYPE_e;

typedef struct _AMBA_DSP_H264DEC_STREAM_CONFIG_s_ {
    UINT8                StreamID;
    AMBA_DSP_DEC_TYPE_e  VideoDecType;
    UINT8                ErrConcealMode;
    UINT8                *pBitsBufAddr;
    UINT32               BitsBufSize;
    UINT16               MaxFrameWidth;     /* 0 - use deafult value */
    UINT16               MaxFrameHeight;    /* 0 - use deafult value */
    UINT8                MaxRatioOfGopNM;   /* 0 - use deafult value */
    UINT8                IPFrameOnly;
    UINT8                BackwardTrickPlay;

    UINT8                EnableEncode;      /* 1 - enable encoding */
    UINT16               EncodeWidth;
    UINT16               EncodeHeight;

    UINT32               FrameRateDivisor;  /* use when transcode or play HFR clip at normal speed */
} AMBA_DSP_H264DEC_STREAM_CONFIG_s;

typedef struct _AMBA_DSP_H264DEC_START_CONFIG_s_ {
    UINT32  PreloadDataSize;
    UINT64  FirstDisplayPTS;
} AMBA_DSP_H264DEC_START_CONFIG_s;

typedef struct _AMBA_DSP_H264DEC_BITS_FIFO_s_ {
    UINT8   *pStartAddr;                /* was UINT8 *pDataAddr */
    UINT8   *pEndAddr;                  /* was UINT32 DataSize */
} AMBA_DSP_H264DEC_BITS_FIFO_s;

typedef struct _AMBA_DSP_DEC_POST_CTRL_s_ {
    AMBA_DSP_WINDOW_s      InputWindow;          /* cropping of decode YUV */
    AMBA_DSP_WINDOW_s      TargetWindow;         /* Target display size */
    AMBA_DSP_ROTATE_FLIP_e  TargetRotateFlip;    /* Target window rotate and flip setting */

    AMBA_DSP_VOUT_IDX_e    OutputSelect;    /* Vout index */
    AMBA_DSP_ROTATE_FLIP_e RotateFlip;      /* Vout rotate and flip setting */
    AMBA_DSP_WINDOW_s      VoutWindow;      /* Vout video window */
} AMBA_DSP_DEC_POST_CTRL_s;

typedef struct _AMBA_DSP_H264DEC_FADE_EFFECT_s_
{
    UINT32 StartPTS;
    UINT32 Duration;                        /* ms */
    UINT16 StartMatrix[9];
    INT16  StartYOffset;
    INT16  StartUOffset;
    INT16  StartVOffset;
    UINT16 EndMatrix[9];
    INT16  EndYOffset;
    INT16  EndUOffset;
    INT16  EndVOffset;
} AMBA_DSP_H264DEC_FADE_EFFECT_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaDSP_VideoDec.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaDSP_VideoDecConfig(int MaxNumStream, AMBA_DSP_H264DEC_STREAM_CONFIG_s *pStreamConfig);

int AmbaDSP_VideoDecStart(int NumStream, int *pStreamIdx, AMBA_DSP_H264DEC_START_CONFIG_s *pStartConfig);
int AmbaDSP_VideoDecStop(int NumStream, int *pStreamIdx, AMBA_DSP_H264DEC_STOP_OPTION_e *pStopOption);
int AmbaDSP_VideoDecSpeedDirSet(int NumStream, int *pStreamIdx, UINT16 *pSpeed, AMBA_DSP_H264DEC_DIR_e *pDir);
int AmbaDSP_VideoDecTrickPlay(int NumStream, int *pStreamIdx, AMBA_DSP_H264DEC_TRICK_PLAY_e *pTrickPlay);
int AmbaDSP_VideoDecBitsFifoUpdate(int NumStream, int *pStreamIdx, AMBA_DSP_H264DEC_BITS_FIFO_s *pBitsFIFO);
int AmbaDSP_VideoDecPostCtrl(int StreamIdx, int NumPostCtrl, AMBA_DSP_DEC_POST_CTRL_s *pPostCtrl);   /* Zoom In/Out */
int AmbaDSP_VideoDecFadeEffect(int StreamIdx, int NumEffect, AMBA_DSP_H264DEC_FADE_EFFECT_s *pFadeEffect);

int AmbaDSP_VideoDecVideo2Yuv(int StreamIdx, AMBA_DSP_YUV_IMG_BUF_s *pYuvBufAddr);

#endif  /* _AMBA_DSP_VIDEO_DEC_H_ */
