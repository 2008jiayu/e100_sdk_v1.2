/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor_TI5150.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Control APIs of OmniVision TI5150 CMOS sensor with MIPI interface
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_YUV_TI5150_H_
#define _AMBA_YUV_TI5150_H_
/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define TI5150_NUM_MODE_REG     7

typedef enum _TI5150_YUV_MODE_e_ {
    TI5150_720_480_60I = 0,
    TI5150_720_576_50I,
    TI5150_NUM_YUV_MODE,
} TI5150_YUV_MODE_e;

typedef struct _TI5150_MODE_INFO_s_ {
    UINT32 InputClk;
    AMBA_YUV_OUTPUT_INFO_s OutputInfo;
} TI5150_MODE_INFO_s;

typedef struct _TI5150_REG_s_ {
    UINT16  Addr;
    UINT8   Data[TI5150_NUM_YUV_MODE];
} TI5150_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_TI5150.c
\*-----------------------------------------------------------------------------------------------*/
extern const TI5150_MODE_INFO_s TI5150ModeInfo[TI5150_NUM_YUV_MODE];

extern TI5150_REG_s TI5150RegTable[TI5150_NUM_MODE_REG];

#endif /* _AMBA_YUV_TI5150_H_ */

