/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaShell_Utility.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Ambarella shell utilities.
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#include "AmbaFS.h"
#include "AmbaUtility.h"

#include "AmbaShell.h"
#include "AmbaShell_Priv.h"

int AmbaShell_MemAlloc(void **pBuf, UINT32 Size)
{
    if (pBuf == NULL)
        return NG;

    if (AmbaKAL_BytePoolAllocate(AmbaShell_GetHeap(), pBuf, Size, 0) == OK) {
        return OK;
    } else {
        *pBuf = NULL;
        return NG;
    }
}

int AmbaShell_MemFree(void *pMemBase)
{
    return AmbaKAL_BytePoolFree(pMemBase);
}

void AmbaShell_Ascii2Unicode(char *AsciiString, char *UnicodeString)
{
    if (AmbaFS_GetCodeMode() == AMBA_FS_UNICODE) {
        AmbaUtility_Ascii2Unicode(AsciiString, (UINT16 *)UnicodeString);
    } else {
        strcpy(UnicodeString, AsciiString);
    }
}

void AmbaShell_Unicode2Ascii(char *UnicodeString, char *AsciiString)
{
    if (AmbaFS_GetCodeMode() == AMBA_FS_UNICODE) {
        AmbaUtility_Unicode2Ascii((const UINT16 *)UnicodeString, AsciiString);
    } else {
        strcpy(AsciiString, UnicodeString);
    }
}

