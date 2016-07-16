/*-----------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaFS_PrFileUnicode.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Abstraction layer of Ambarella File System which would hide thirty-party file system implemenation.
 *
 *  @History        ::
 *      Date        Name        Comments
 *      09/07/2012  Evan Chen   Created
 *
 *-----------------------------------------------------------------------------------------------*/
#ifndef _AMBA_FS_PRFILE_UNICODE_H_
#define _AMBA_FS_PRFILE_UNICODE_H_

/*-----------------------------------------------------------------------------------------------*\
 * Macro definition
 *-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaFS_PrFileUnicode.c (MW for SD)
 *-----------------------------------------------------------------------------------------------*/
PF_FILE *AmbaFS_PfUnicodeFopen(const char *filename, const char *Mode);
PF_DIR *AmbaFS_PfUnicodeOpendir(const char *pDname);
int AmbaFS_PfUnicodeChdir(const char *pDname);
int AmbaFS_PfUnicodeRemove(const char *filename);
int AmbaFS_PfUnicodeRename(const char *olpDname, const char *newname);
int AmbaFS_PfUnicodeMove(const char *srcname, const char *dstname);
int AmbaFS_PfUnicodeFsexec(AMBA_FS_DTA_UNION *u, int type, unsigned char Attr);
int AmbaFS_PfUnicodeFsfirst(const char *pFilename, unsigned char Attr, AMBA_FS_DTA_UNION *u);
int AmbaFS_PfUnicodeFsnext(AMBA_FS_DTA_UNION *u);
int AmbaFS_PfUnicodeMkdir(const char *pDname);
int AmbaFS_PfUnicodeRmdir(const char *pDname);
int AmbaFS_PfUnicodeFstat(const char *pPathname, PF_STAT *ff_stat);
int AmbaFS_PfUnicodeChmod(const char *pPathname, int pMode);
int AmbaFS_PfUnicodeChdmod(const char *pPathname, int pMode);
int AmbaFS_PfUnicodeCinsert(const char * pFileName, UINT32 Offset, UINT32 Number);
int AmbaFS_PfUnicodeCleandir(const char *pPath, const char *pattern, UINT32 Mode, UINT32 *Count);
int AmbaFS_PfUnicodeDeletedir(const char *pPath);
int AmbaFS_PfUnicodeChmoddir (const char *pPath, const char *pFilename,
                              UINT32 Mode, UINT32 Attr, UINT32 *Count);
int AmbaFS_PfUnicodeCdelete(const char *pPath, UINT32 Offset, UINT32 Num);
int AmbaFS_PfUnicodeDivide(char *pOrgPath, char *pNewPath, UINT32 Offset);
int AmbaFS_PfUnicodeCombine(char * pFileNameBase, char * pFileNameAdd);

#endif  /* _AMBA_FS_PRFILE_UNICODE_H_ */
