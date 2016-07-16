/**
 *  @file PtrList.c
 *
 *  pointer list implementation
 *
 *  **History**
 *      |Date       |Name        |Comments       |
 *      |-----------|------------|---------------|
 *      |2013/04/02 |clchan      |Created        |
 *
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#include <ptrlist.h>
#include <stdlib.h>
#include <AmbaPrintk.h>

//#define PLIST_DEBUG

#ifdef PLIST_DEBUG
void AmpPtrList_ShowAll(AMP_PTR_LIST_HDLR_s *hdlr)
{
    char Str[256];
    char Buffer[16];
    UINT32 Count = AmpPtrList_GetCount(hdlr);
    UINT32 i;
    void *Ptr;
    sprintf(Str, "PLIST[%u/%u]:", hdlr->nCount, hdlr->nLimit);
    for (i=0; i<Count; i++) {
        AmpPtrList_GetAt(hdlr, i, &Ptr);
        sprintf(Buffer, " %p", Ptr);
        strcat(Str, Buffer);
    }
    AmbaPrint(Str);
}
#else
#define AmpPtrList_ShowAll(x)
#endif

int AmpPtrList_Create(void *buffer, UINT32 size, AMP_PTR_LIST_HDLR_s **hdlr)
{
    AMP_PTR_LIST_HDLR_s * const List = (AMP_PTR_LIST_HDLR_s *)buffer;
    K_ASSERT(buffer != NULL);
    K_ASSERT(size >= sizeof(AMP_PTR_LIST_HDLR_s) + sizeof(void *));
    K_ASSERT(hdlr != NULL);
    List->Limit = (size - sizeof(AMP_PTR_LIST_HDLR_s)) / sizeof(void *);
    List->Count = 0;
    *hdlr = List;
    return 0;
}

int AmpPtrList_Delete(AMP_PTR_LIST_HDLR_s *hdlr)
{
    K_ASSERT(hdlr != NULL);
    memset(hdlr, 0, sizeof(AMP_PTR_LIST_HDLR_s));
    return 0;
}

int AmpPtrList_AddHead(AMP_PTR_LIST_HDLR_s *hdlr, void *ptr)
{
    UINT8 * const Buffer = (UINT8 *)hdlr + sizeof(AMP_PTR_LIST_HDLR_s);
    K_ASSERT(hdlr != NULL);
    K_ASSERT(ptr != NULL);
    K_ASSERT(hdlr->Count < hdlr->Limit);
    if (hdlr->Count > 0)
        memmove(Buffer + sizeof(void *), Buffer, sizeof(void *) * hdlr->Count);
    memcpy(Buffer, &ptr, sizeof(void *));
    hdlr->Count++;
    AmpPtrList_ShowAll(hdlr);
    return 0;
}

int AmpPtrList_AddTail(AMP_PTR_LIST_HDLR_s *hdlr, void *ptr)
{
    UINT8 * const Buffer = (UINT8 *)hdlr + sizeof(AMP_PTR_LIST_HDLR_s);
    K_ASSERT(hdlr != NULL);
    K_ASSERT(ptr != NULL);
    K_ASSERT(hdlr->Count < hdlr->Limit);
    memcpy(Buffer + hdlr->Count * sizeof(void *), &ptr, sizeof(void *));
    hdlr->Count++;
    AmpPtrList_ShowAll(hdlr);
    return 0;
}

int AmpPtrList_RemoveAt(AMP_PTR_LIST_HDLR_s *hdlr, UINT32 index)
{
    UINT8 * const Buffer = (UINT8 *)hdlr + sizeof(AMP_PTR_LIST_HDLR_s);
    K_ASSERT(hdlr != NULL);
    K_ASSERT(index < hdlr->Count);
    if (index != hdlr->Count - 1)
        memmove(Buffer + index * sizeof(void *), Buffer + (index + 1) * sizeof(void *), sizeof(void *) * (hdlr->Count - index - 1));
    hdlr->Count--;
    AmpPtrList_ShowAll(hdlr);
    return 0;
}

void AmpPtrList_RemoveAll(AMP_PTR_LIST_HDLR_s *hdlr)
{
    K_ASSERT(hdlr != NULL);
    hdlr->Count = 0;
    AmpPtrList_ShowAll(hdlr);
}

int AmpPtrList_GetAt(AMP_PTR_LIST_HDLR_s *hdlr, UINT32 index, void **ptr)
{
    UINT8 * const Buffer = (UINT8 *)hdlr + sizeof(AMP_PTR_LIST_HDLR_s);
    K_ASSERT(hdlr != NULL);
    K_ASSERT(ptr != NULL);
    K_ASSERT(index < hdlr->Count);
    memcpy(ptr, Buffer + index * sizeof(void *), sizeof(void *));
    return 0;
}

UINT32 AmpPtrList_GetCount(AMP_PTR_LIST_HDLR_s *hdlr)
{
    K_ASSERT(hdlr != NULL);
    return hdlr->Count;
}

UINT32 AmpPtrList_GetRequiredBufferSize(UINT32 maxElement)
{
    return (sizeof(AMP_PTR_LIST_HDLR_s) + maxElement * sizeof(void *));
}

