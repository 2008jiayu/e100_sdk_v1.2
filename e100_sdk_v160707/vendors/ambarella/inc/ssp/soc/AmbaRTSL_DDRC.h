/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_DDRC.h
 *
 *  @Copyright      :: Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for DDRC RTSL APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_DDRC_H_
#define _AMBA_RTSL_DDRC_H_

typedef enum _AMBA_DDRC_DLL_BYTE_e_ {
    AMBA_DDRC_DLL_BYTE_0 = 0,           /* DDRC DLL Byte-0 */
    AMBA_DDRC_DLL_BYTE_1,               /* DDRC DLL Byte-1 */
    AMBA_DDRC_DLL_BYTE_2                /* DDRC DLL Byte-2 */
} AMBA_DDRC_DLL_BYTE_e;

typedef enum _AMBA_DDRC_DRAM_TYPE_e_ {
    AMBA_DDRC_DRAM_TYPE_DDR2 = 0,       /* DDR2 SDRAM */
    AMBA_DDRC_DRAM_TYPE_DDR3,           /* DDR3 SDRAM */
    AMBA_DDRC_DRAM_TYPE_LPDDR,          /* LPDDR SDRAM */
    AMBA_DDRC_DRAM_TYPE_LPDDR2,         /* LPDDR2 SDRAM */
    AMBA_DDRC_DRAM_TYPE_LPDDR3          /* LPDDR3 SDRAM */
} AMBA_DDRC_DRAM_TYPE_e;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_DDRC.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaRTSL_DdrcSetDLL(AMBA_DDRC_DLL_BYTE_e DllByte, UINT32 Value);

int AmbaRTSL_DdrcSetOCD(int Value);
int AmbaRTSL_DdrcSetODT(int Value);     /* PADS_TERM/PADS_NOTERM, if Value < 0, no Termination */
int AmbaRTSL_DdrcSetDramModeRegister(UINT32 ModeRegister, UINT32 Value);

UINT32 AmbaRTSL_DdrcGetDLL(AMBA_DDRC_DLL_BYTE_e DllByte);
int AmbaRTSL_DdrcGetOCD(void);
int AmbaRTSL_DdrcGetODT(void);
AMBA_DDRC_DRAM_TYPE_e AmbaRTSL_DdrcGetDramType(void);

#endif /* _AMBA_RTSL_DDRC_H_ */
