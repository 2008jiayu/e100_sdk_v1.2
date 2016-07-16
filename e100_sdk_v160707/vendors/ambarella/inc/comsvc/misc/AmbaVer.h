/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaVer.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: the module of Ambarella Network Support
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_VER_H_
#define _AMBA_VER_H_

typedef struct _AMBA_VerInfo_s {
    const char  *Key;           /* Module name as key */
    const char  *MachStr;       /* Compiled by whoami@machine */
    const char  *DateStr;       /* Compiled date string */
    UINT32      DateNum;        /* Compiled date number */
    UINT32      SecNum;         /* Compiled second number */
    UINT32      CiCount;       /* Commit count (Commit sequence number)*/
    const char  *CiIdStr;       /* Commit ID */
} AMBA_VerInfo_s;


extern void AmbaVer_Init(void);
extern UINT32 AmbaVer_GetCount(void);
extern const char *AmbaVer_GetKey(int idx);
extern AMBA_VerInfo_s *AmbaVer_GetVerInfo(const char *ModuleName);
extern void AmbaVer_ShowVerInfo(const char *ModuleName);
extern void AmbaVer_ShowVerCompactInfo(const char *ModuleName);
extern void AmbaVer_ShowAllVerInfo(void);
extern void AmbaVer_ShowAllVerCompactInfo(void);
extern void AmbaVer_ShowLinkVerInfo(void);
void AmbaVer_GetLinkVerInfo(const char **pLinkTarget, const char **pLinkMachine, const char **pLinkDate);


#endif	/* _AMBA_VER_H_ */

