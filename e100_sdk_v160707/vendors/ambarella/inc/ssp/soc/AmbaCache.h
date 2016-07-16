/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCache.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Cache Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_CACHE_H_
#define _AMBA_CACHE_H_

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaCache.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaCache_Clean(void *pAddr, UINT32 Size);
void AmbaCache_Invalidate(void *pAddr, UINT32 Size);
void AmbaCache_Flush(void *pAddr, UINT32 Size);

#endif /* _AMBA_CACHE_H_ */
