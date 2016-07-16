/**
 * @file vendor/ambarella/inc/image/imgcalib/AmbaCalib_Detect.h
 *
 * header file for Amba auto detect calibration
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef _AMBACALIB_DETECT_
#define _AMBACALIB_DETECT_

#define BOOSTBLURFACTOR 90.0

#define CAL_AVM_NOEDGE 255
#define CAL_AVM_POSSIBLE_EDGE 128
#define CAL_AVM_EDGE 0

#define CAL_AVM_DONEEDGE 254

#define CAL_RAW_WHITE 0x3fff 
#define CAL_RAW_BLACK 0x0000

//#define CLIP_W 360
//#define CLIP_H 270

#define CLIP_W 1920
#define CLIP_H 1080
#define CLIP_SIZE (CLIP_W * CLIP_H)



enum Arrow {LEFT, UP, RIGHT, DOWN};

typedef struct AVM_Point_{
    int x;
    int y;
}AVM_Point;

typedef struct AVM_Corner_{
    AVM_Point Corner[8];
}AVM_Corner;

int AmpCalib_AVMWriteShortMap(short int *Buffer, const char *Filename);
int AmpCalib_AVMWriteCharMap(unsigned char *Buffer, const char *Filename);
int AppLibCalibDetect_DetectFromRaw();
int AmpCalib_AVMMarkDetection(UINT16 *Raw, UINT32 RawWidth, UINT32 RawHeight, AVM_Corner *ResultCorner);

//extern int AmpUtil_GetAlignedPool(AMBA_KAL_BYTE_POOL_t * BytePool, void * * AlignedPool, void * * Pool, UINT32 Size, UINT32 Alignment);

typedef struct AVM_Detect_Buffer_{
    AMBA_MEM_CTRL_s Memkernel;
    AMBA_MEM_CTRL_s Memtempim;
    AMBA_MEM_CTRL_s Memsmoothedim;
    AMBA_MEM_CTRL_s MemMagnitude;
    AMBA_MEM_CTRL_s MemDeltaX;
    AMBA_MEM_CTRL_s MemDeltaY;
    AMBA_MEM_CTRL_s MemHistBuffer;
    AMBA_MEM_CTRL_s MemNms;
    AMBA_MEM_CTRL_s MemEdge;
    AMBA_MEM_CTRL_s MemTmp;
    AMBA_MEM_CTRL_s MemContourMap;
    AMBA_MEM_CTRL_s MemClipImage;   
}AVM_Detect_Buffer;

typedef struct AVM_Detect_Parameter_{
    float Sigma;
    float ThresholdLow;
    float ThresholdHigh;
    int CornerNumber;
    int ThresholdArea;
    int ThresholdMinusColor;
}AVM_Detect_Parameter;

int AmpCalib_AVMGetBuffer(AVM_Detect_Buffer *DetectBuffer, AVM_Detect_Parameter *DetectParameter);

 
#define HIST_BOUND 32768
//65536
//300000
//32768
//static int Hist[HIST_BOUND];

#endif      //_AMBACALIB_DETECT_
