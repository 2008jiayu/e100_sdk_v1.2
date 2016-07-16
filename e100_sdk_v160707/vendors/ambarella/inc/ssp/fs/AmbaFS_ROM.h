/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaFS_ROM.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Read Only Memory File System.
 *
 *  @History        ::
 *      Date        Name        Comments
 *      09/10/2012  Kerson      Created
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_ROMFS_H_
#define _AMBA_ROMFS_H_

/*-----------------------------------------------------------------------------------------------*\
 * AmbaFS ROM data structure definition
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FS_DIR_DEPTH           10
#define AMBA_FS_LEN_FILE            32

#define AMBA_FS_N_DIRS              8
#define AMBA_FS_LEN_DIRS            (AMBA_FS_CWD_LEN / AMBA_FS_N_DIRS)

typedef struct RFS_FILE {
    struct {
        char Cwd[AMBA_FS_CWD_LEN];      /* current working path */
        char Abs[AMBA_FS_CWD_LEN];      /* absolute path */
        char Rel[AMBA_FS_CWD_LEN];      /* path relative to cwd */
        char Dirs[AMBA_FS_N_DIRS][AMBA_FS_LEN_DIRS];    /* dirs name */
        char FileName[AMBA_FS_LEN_FILE];/* filename */
        int  NumberDirs;                /* number of dirs to be open */
        int  Slot;                      /* slot number */
    } Path;

    UINT32 OpenMode;                    /* file open mode */
    int Index;                          /* file node index */
    UINT64 FilePos;                     /* file position in byte */
} RFS_FILE;

/* Unicode wrapper functions. */
int AmbaFS_RomUnicodeFopen(const char *pFilename, const char *pMode, RFS_FILE *pStream);
int AmbaFS_RomUnicodeFSfirst(const char *pFilename, UINT32 Attr, AMBA_FS_DTA *pFFDta);
int AmbaFS_RomUnicodeChdir(const char *pDname);
int AmbaFS_RomUnicodeFstat(const char *pPname, AMBA_FS_STAT *pFFStat);

/* ROMFS functions. */
int AmbaFS_RomFopen(const char *pFilename, const char *pMode, RFS_FILE *pStream);
int AmbaFS_RomFclose(RFS_FILE *pStream);
UINT32 AmbaFS_RomFread(void *pBuf, UINT32 Size, UINT32 Nobj, RFS_FILE *pStream);
int AmbaFS_RomFseek(RFS_FILE *pStream, int Offset, int Origin);
int AmbaFS_RomFsearch(int Mode, const char *pFilename, UINT32 Attr, AMBA_FS_DTA *pFFDta);
int AmbaFS_RomFSfirst(const char *pFullname, UINT32 Attr, AMBA_FS_DTA *pFFDta);
int AmbaFS_RomFSnext(AMBA_FS_DTA *pFFDta);
int AmbaFS_RomChdir(const char *pDname);
int AmbaFS_RomFstat(const char *pPname, AMBA_FS_STAT *pFFStat);
int AmbaFS_RomGetvol(char Drv, AMBA_FS_VOLTAB *pVtbl);
int AmbaFS_RomGetDev(char Drv, AMBA_FS_DEVINF *pDev);
INT64 AmbaFS_RomFtell(RFS_FILE *pStream);
int AmbaFS_RomFeof(RFS_FILE *pStream);
const char *AmbaFS_RomGetError(int Error);
int AmbaFS_RomInit(void);

#endif /* _AMBA_ROMFS_H_ */
