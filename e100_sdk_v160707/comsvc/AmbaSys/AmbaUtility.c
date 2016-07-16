/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaUtility.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Ambarella utilities.
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <w_char.h>

#include "AmbaDataType.h"
#include "AmbaRTSL_Timer.h"
#include "AmbaUtility.h"

/* FIXME: Temp function */
#define PRINT(...)

/*-----------------------------------------------------------------------------------------------*\
 *  CRC32 table.
\*-----------------------------------------------------------------------------------------------*/
const UINT32 _AmbaCrc32Table[] = {
    0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
    0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
    0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL, 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
    0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
    0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
    0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
    0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
    0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L, 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
    0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
    0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
    0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
    0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
    0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L, 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
    0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
    0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
    0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
    0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
    0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L, 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
    0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
    0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
    0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
    0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
    0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L, 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
    0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
    0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
    0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
    0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
    0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL, 0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
    0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
    0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
    0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
    0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUtility_Int2Str
 *
 *  @Description:: Convert Integer value to decimal ASCII string
 *
 *  @Input   ::
 *      n:         the value to be converted
 *      s:         pointer to the buffer of the digits string
 *      MaxLength: maximum length of the ASCII string
 *      LeadChar:  the leading character/leader
 *
 *  @Output  ::
 *      s: pointer to the buffer of the digits string
 *
 *  @Return  ::
 *      int  : the Length of the converted string
\*-----------------------------------------------------------------------------------------------*/
int AmbaUtility_Int2Str(int n, char *s, int MaxLength, char LeadChar)
{
    int StrLength = 0, t = n;
    char *pWorkChar1, *pWorkChar2 = s, Tmp;

    if (MaxLength > 0 && LeadChar != 0) {
        /* fixed length of string */
        StrLength = MaxLength;
        if (t < 0) {
            t = -t;
            *pWorkChar2++ = '-';
            MaxLength--;
        }

        pWorkChar2 += MaxLength;    /* the last byte position */
        *pWorkChar2-- = '\0';       /* attach the null terminator */
        while (MaxLength-- > 0) {
            if (t == 0) {
                *pWorkChar2-- = LeadChar;
            } else {
                *pWorkChar2-- = '0' + (t % 10);
                t /= 10;
            }
        }
    } else {
        /* variable length of string */
        if (t == 0) {
            *pWorkChar2++ = '0';
            *pWorkChar2 = '\0';         /* attach the null terminator */
            return 1;
        }

        //----------------------------------------------------------
        if (t < 0) {
            t = -t;
            *pWorkChar2++ = '-';
            *(pWorkChar2 + 1) = '\0';   /* attach the null terminator */
        }

        pWorkChar1 = pWorkChar2;
        while (t > 0) {
            *pWorkChar1++ = '0' + (t % 10);
            t /= 10;
        }
        *pWorkChar1 = '\0';             /* attach the null terminator */
        StrLength = pWorkChar1 - s;

        // change the order of the digits
        n = (pWorkChar1 - pWorkChar2) / 2;  /* n = lenght of the string / 2 */
        while (n-- > 0) {
            Tmp = *(--pWorkChar1);
            *pWorkChar1 = *pWorkChar2;
            *pWorkChar2++ = Tmp;
        }
    }

    return StrLength;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUtility_Crc32Add
 *
 *  @Description:: Add Crc32 checksum.
 *
 *  @Input   ::
 *      pBuf: Pointer to the memory to be calculated.
 *      Size: Buffer length.
 *      Crc:  Current CRC32 checksum.
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *      UINT32: Crc32 checksum.
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaUtility_Crc32Add(const void *pBuf, unsigned int Size, UINT32 Crc)
{
    const UINT8 *p;

    p = pBuf;
    while (Size > 0) {
        Crc = _AmbaCrc32Table[(Crc ^ *p++) & 0xff] ^ (Crc >> 8);
        Size--;
    }

    return Crc;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUtility_Crc32
 *
 *  @Description:: Calculate Crc32 checksum.
 *
 *  @Input   ::
 *      pBuf: Pointer to the memory to be calculated.
 *      Size: Buffer length.
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *          UINT32  : Crc32 checksum.
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaUtility_Crc32(const void *pBuf, UINT32 Size)
{
    const UINT8 *p;
    UINT32 Crc;

    p = pBuf;
    Crc = ~0U;

    while (Size > 0) {
        Crc = _AmbaCrc32Table[(Crc ^ *p++) & 0xff] ^ (Crc >> 8);
        Size--;
    }

    return Crc ^ ~0U;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUtility_Ascii2Unicode
 *
 *  @Description:: ASCII to unicode.
 *
 *  @Input   ::
 *      pAsciiStr:   Pointer to the ASCII string.
 *      pUnicodeStr: Pointer to the unicode string.
 *
 *  @Output  ::
 *          WCHAR *pUnicodeStr     : Pointer to the unicode string.
 *
 *  @Return  :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaUtility_Ascii2Unicode(const char *pAsciiStr, WCHAR *pUnicodeStr)
{
    char CharCode;

    do {
        CharCode = *pAsciiStr++;
        *pUnicodeStr++ = CharCode;
    } while (CharCode != '\0');
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUtility_Unicode2Ascii
 *
 *  @Description:: Unicode to ASCII.
 *
 *  @Input   ::
 *      pUnicodeStr: Pointer to the unicode string.
 *      pAsciiStr: Pointer to the ASCII string.
 *
 *  @Output  ::
 *      pAsciiStr: Pointer to the ASCII string.
 *
 *  @Return  :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaUtility_Unicode2Ascii(const WCHAR *pUnicodeStr, char *pAsciiStr)
{
    int Len, i;

    Len = w_strlen((const WCHAR*)pUnicodeStr);
    for (i = 0; i < Len; i++, pUnicodeStr++, pAsciiStr++) {
        if (*pUnicodeStr & 0xff00)
            *pAsciiStr = '_';
        else
            *pAsciiStr = (char) * pUnicodeStr;
    }

    *pAsciiStr = '\0';
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUtility_Drive2Slot
 *
 *  @Description:: Translate drive character to slot ID.
 *
 *  @Input   ::
 *      Drive: Drive character.
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *      int  : Slot ID
\*-----------------------------------------------------------------------------------------------*/
int AmbaUtility_Drive2Slot(char Drive)
{
    int Slot;

    Drive = tolower((int) Drive);
    Slot = Drive - 'a';

    return Slot;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUtility_Slot2Drive
 *
 *  @Description:: Translate slot ID character to drive.
 *
 *  @Input   ::
 *      Slot: Slot ID
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *     char  : Drive character.
\*-----------------------------------------------------------------------------------------------*/
char AmbaUtility_Slot2Drive(int Slot)
{
    return ('a' + Slot);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUtility_StringSubst
 *
 *  @Description:: String substitute.
 *
 *  @Input   ::
 *      pOrgStr: Pointer to the original string to be substituted.
 *      BufLen:  Buffer length of the pOrgStr.
 *      pTgtStr: String in pOrgStr to be substituted.
 *      pSubStr: String to substitute the pTgtStr in the pOrgStr.
 *
 *  @Output  ::
 *      pOrgStr: Pointer to the original string to be substituted.
 *
 *  @Return  ::
 *          int : OK(1)/NG(0)
\*-----------------------------------------------------------------------------------------------*/
int AmbaUtility_StringSubst(char *pOrgStr, UINT32 BufLen, const char *pTgtStr, const char *pSubStr)
{
    int i, RetVal = 0;
    UINT32 OrgLen, TgtLen, SubLen;
    char OutStr[256];
    char *p, *q;

    memset(OutStr, 0x0, sizeof(OutStr));

    OrgLen = strlen(pOrgStr);
    TgtLen = strlen(pTgtStr);
    SubLen = strlen(pSubStr);

    p = pOrgStr;
    q = OutStr;

    for (i = 0; i < OrgLen; i++) {

        if (strncmp(p, pTgtStr, TgtLen) == 0) {
            strncpy(q, pSubStr, SubLen);
            q += SubLen;
            RetVal = 1;
        } else {
            *q = *p;
            q++;

            if (strlen(OutStr) >= sizeof(OutStr)) {
                PRINT("Out of memory buf.");
                RetVal = -1;
                break;
            }
        }

        p++;
    }

    if (RetVal == 1) {

        if (strlen(OutStr) >= BufLen) {
            PRINT("Output string is bigger than buf length");
            RetVal = -1;
        } else {
            strcpy(pOrgStr, OutStr);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUtility_CmpData
 *
 *  @Description:: Compare memory buffer.
 *
 *  @Input   ::
 *      pOrigin: Pointer to the original memory.
 *      pData:   Pointer to the memory to be compared.
 *      Len:     Memory length.
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *          int : OK(0) / error count (>0)
\*-----------------------------------------------------------------------------------------------*/
int AmbaUtility_CmpData(UINT8 *pOrigin, UINT8 *pData, UINT32 Len)
{
    int ErrCnt = 0;
    UINT32 i;

    for (i = 0; i < Len; i++) {
        if (pOrigin[i] != pData[i]) {
            ErrCnt++;
        }
    }

    if (ErrCnt) {
        PRINT("(data error byte counts %d)", ErrCnt);
    }

    return ErrCnt;
}

int AmbaUtility_FindWild(const char *pString)
{
    int l;

    if (pString == NULL)
        return 0;

    for (l = 0 ; pString[l] != '\0'; l++)
        if (pString[l] == '*')
            return l;

    return -1;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FindRepeatSep
 *
 *  @Description::
 *
 *  @Input   ::
 *      pString: Pointer to the string.
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *      int  : String position
\*-----------------------------------------------------------------------------------------------*/
int AmbaUtility_FindRepeatSep(const char *pString)
{
    int Index;
    int Position = 0;

    if (pString == NULL)
        return 0;

    for (Index = 0, Position = 0; pString[Index] != '\0'; Index++)
        if (pString[Index] == '\\')
            Position = Index + 1;

    return Position;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUtility_StringToUINT32
 *
 *  @Description:: Convert a String to a UINT32 value
 *
 *  @Input   ::
 *      pString: Pointer to the string.
 *      pValue:  Pointer to the value.
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *          int : OK(0) / error count (>0)
\*-----------------------------------------------------------------------------------------------*/
int AmbaUtility_StringToUINT32(const char *pString, UINT32 *pValue)
{
    *pValue = strtoul(pString, NULL, 0);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUtility_GetTimeStart
 *
 *  @Description:: Get a timer tick.
 *
 *  @Input   ::
 *      pStart: Pointer to the timer tick
 *
 *  @Output  ::
 *      pStart: Pointer to the timer tick
 *
 *  @Return  :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaUtility_GetTimeStart(UINT32 *pStart)
{
    *pStart = AmbaRTSL_TimerGetTickCount();
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUtility_GetTimeEnd
 *
 *  @Description:: Convert a String to a UINT32 value
 *
 *  @Input   ::
 *      pStart: Pointer to the start timer tick.
 *      pEnd:   Pointer to the end timer tick.
 *
 *  @Output  ::
 *      pStart: Pointer to the start timer tick.
 *      pEnd:   Pointer to the end timer tick.
 *
 *  @Return  :: none
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaUtility_GetTimeEnd(UINT32 *pStart, UINT32 *pEnd)
{

    (*pEnd) = AmbaRTSL_TimerGetTickCount();

    if ((*pStart) < (*pEnd)) {
        return ((*pEnd) - (*pStart));
    } else {
        return (0xffffffff - (*pStart) + (*pEnd) + 1);
    }

}
