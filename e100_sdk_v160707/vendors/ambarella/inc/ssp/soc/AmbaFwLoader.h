/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaFwLoader.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: System initializations: H/W and S/W
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_FW_LOADER_H_
#define _AMBA_FW_LOADER_H_

/*
 * zero for success,
 * positive code for fallback
 * negative code for errors
 */
#define AMBA_FW_LOADER_NO_ERR       (0)
#define AMBA_FW_LOADER_DISABLED     (1)
#define AMBA_FW_LOADER_UNSUPPORT    (2)
#define AMBA_FW_LOADER_IO_ERR       (-1)

typedef struct _AMBA_FW_REGION_s_ {
    char * pName;
    UINT32 ImageOffset;
    UINT32 RoSectionBase;
    UINT32 RoSectionEnd;
    UINT32 RoSectionLength;
    UINT32 RwSectionBase;
    UINT32 RwSectionEnd;
    UINT32 RwSectionLength;
} AMBA_FW_REGION_s;

int AmbaFwLoader_Init(void);
int AmbaFwLoader_Load(UINT32 region);
int AmbaFwLoader_Finish(void);
int AmbaFwLoader_FlushCache(void);

#endif /* _AMBA_FW_LOADER_H_ */

