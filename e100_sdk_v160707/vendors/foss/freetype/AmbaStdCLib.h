/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaStdCLib.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for the standard C Library
 *
 *  @History        ::
 *      Date        Name        Comments
 *      02/25/2013  Y.H.Chen    Created
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_STD_CLIB_H_
#define _AMBA_STD_CLIB_H_

#ifndef  __cplusplus
#include "AmbaFS.h"

//#define stdin   NULL
//#define stdout  NULL
//#define stderr  NULL

//typedef AMBA_FS_FILE    FILE;

#endif /* __cplusplus */

void AmbaStdC_Init(void *pBytePool);
void AmbaStdCpp_Init(void *pBytePool);

/*-----------------------------------------------------------------------------------------------*\
 * Standard C String & Character
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * Standard C Math
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * Standard C Time & Date
\*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * Standard C Memory
\*-----------------------------------------------------------------------------------------------*/
void *AmbaStdC_calloc(size_t num, size_t size);
void  AmbaStdC_free(void *ptr);
void *AmbaStdC_malloc(unsigned int size);

/*-----------------------------------------------------------------------------------------------*\
 * Other standard C functions
\*-----------------------------------------------------------------------------------------------*/

#endif  /* _AMBA_STD_CLIB_H_ */
