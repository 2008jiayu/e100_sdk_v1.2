/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaStdCLib.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Standard C library
 *
 *  @History        ::
 *      Date        Name        Comments
 *      02/25/2013  Y.H.Chen    Created
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#include "AmbaStdCLib.h"

static AMBA_KAL_BYTE_POOL_t *pStdCLibBytePool = NULL;

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaStdC_Init
 *
 *  @Description:: Initialize standard C library
 *
 *  @Input      ::
 *      pBytePool:    Pointer to the memory byte pool Control Block
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaStdC_Init(void *pBytePool)
{
    pStdCLibBytePool = (AMBA_KAL_BYTE_POOL_t *) pBytePool;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaStdC_free
 *
 *  @Description:: Deallocate memory block
 *
 *  @Input      ::
 *      pMemBase:   Pointer to a memory block previously allocated with AmbaStdC_malloc or AmbaStdC_calloc
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          void * : a pointer to the memory block allocated by the function
\*-----------------------------------------------------------------------------------------------*/
void AmbaStdC_free(void *pMemBase)
{
    AmbaKAL_BytePoolFree(pMemBase);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaStdC_malloc
 *
 *  @Description:: Allocate memory block
 *
 *  @Input      ::
 *      Size:   Size of the memory block, in bytes
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          void * : a pointer to the memory block allocated by the function
\*-----------------------------------------------------------------------------------------------*/
void *AmbaStdC_malloc(unsigned int Size)
{
    void *pMemBase;

    if (pStdCLibBytePool == NULL ||
        AmbaKAL_BytePoolAllocate(pStdCLibBytePool, &pMemBase, Size, AMBA_KAL_WAIT_FOREVER) != OK)
    {
        return NULL;
    }

    return pMemBase;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaStdC_calloc
 *
 *  @Description:: Allocate and zero-initialize array
 *
 *  @Input      ::
 *      Num:    Number of elements to allocate
 *      Size:   Size of each element
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          void * : a pointer to the memory block allocated by the function
\*-----------------------------------------------------------------------------------------------*/
void *AmbaStdC_calloc(size_t Num, size_t Size)
{
    void *pMemBase = AmbaStdC_malloc(Num * Size);

    if (pMemBase != NULL)
        memset(pMemBase, 0, Num * Size);

    return pMemBase;
}
