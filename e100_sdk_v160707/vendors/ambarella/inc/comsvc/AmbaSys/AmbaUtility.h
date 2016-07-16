/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaUtility.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella utilities.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_UTILITY_H_
#define _AMBA_UTILITY_H_

#define AMBA_CRC32_INIT_VALUE           (~0U)

/*-----------------------------------------------------------------------------------------------*\
 * Macro Definitions
\*-----------------------------------------------------------------------------------------------*/
#define AmbaUtility_Crc32Finalize(d)    ((d) ^ AMBA_CRC32_INIT_VALUE)

int    AmbaUtility_Int2Str(int n, char *s, int MaxLength, char LeadChar);
UINT32 AmbaUtility_Crc32Add(const void *pBuf, unsigned int Size, UINT32 Crc);
UINT32 AmbaUtility_Crc32(const void *pBuf, UINT32 Size);
void   AmbaUtility_Ascii2Unicode(const char *pAsciiStr, WCHAR *pUnicodeStr);
void   AmbaUtility_Unicode2Ascii(const WCHAR *pUnicodeStr, char *pAsciiStr);
int    AmbaUtility_Drive2Slot(char Drive);
char   AmbaUtility_Slot2Drive(int Slot);
int    AmbaUtility_StringSubst(char *pOrgStr, UINT32 BufLen, const char *pTgtStr, const char *pSubStr);
int    AmbaUtility_CmpData(UINT8 *pOrigin, UINT8 *pData, UINT32 Len);
int    AmbaUtility_StringToUINT32(const char *pString, UINT32 *pValue);
void   AmbaUtility_GetTimeStart(UINT32 * pStart);
UINT32 AmbaUtility_GetTimeEnd(UINT32 * pStart, UINT32 * pEnd);

#endif /* _AMBA_UTILITY_H_ */
