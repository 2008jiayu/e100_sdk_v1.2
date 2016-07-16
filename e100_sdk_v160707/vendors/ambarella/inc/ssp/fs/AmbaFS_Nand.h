/*-----------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaFS_Nand.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: NAND flash bad block table APIs.
 *
 *  @History        ::
 *      Date        Name        Comments
 *      11/23/2012  Kerson      Created
 *
\*-----------------------------------------------------------------------------------------------*/

#ifndef _AMBA_FS_NAND_H_
#define _AMBA_FS_NAND_H_

/* Definitions for filesystem storage1/storage2 in NAND flash. */
#define NAND_FS_ID_STG1             0
#define NAND_FS_ID_STG2             1

#define NAND_FS_ID_MAX              2

#ifndef NAND_FAT_PAGES
#define NAND_FAT_PAGES              32
#endif
#ifndef NAND_DATA_PAGES
#define NAND_DATA_PAGES             32
#endif
#ifndef NAND_FAT_BUF_SIZE
#define NAND_FAT_BUF_SIZE           4
#endif
#ifndef NAND_DATA_BUF_SIZE
#define NAND_DATA_BUF_SIZE          8
#endif

/*-----------------------------------------------------------------------------------------------*\
 * Runtime variables required by PrFILE.
\*-----------------------------------------------------------------------------------------------*/
typedef struct _AMBA_NAND_FS_OBJ_s_ {
    PF_SECTOR_BUF           *pCacheBuf;
    PF_SECTOR_BUF           *pRawCacheBuf;
    PF_CACHE_PAGE           *pCachePage;
    PF_CACHE_PAGE           *pRawCachePage;
    AMBA_FS_FAT_MBR         Mbr;                    /* MBR */
    AMBA_FS_FAT_BS16        Bs;                     /* fat16 Boot sector */
    PDM_FUNCTBL             DrvFuncTbl;
    PDM_INIT_DISK           DrvInitTbl;
    PDM_DISK                *pDiskTbl;
    PDM_PARTITION           *pPartTbl;
    PF_DRV_TBL              DrvTblEntry;
    PF_DRV_TBL              *pDrvTbl[2];            /* Drive table */
    PF_CACHE_SETTING        CacheSet;
    AMBA_FS_IO_STATISTICS   IoStat;                 /* IO statistics data */
    AMBA_FS_IO_STATISTICS   IoStat2nd;              /* 2nd IO statistics data */
    UINT8                   IoStat2ndEnable;        /* Enable 2nd IO statistics */
    int                     Slot;                   /* Scardmgr slot */
    int                     NftlID;                 /* ID for nftl instance */
} AMBA_NAND_FS_OBJ_s;

int AmbaFS_NandFormatDrive(int ID, const char *pParam);

#if 0
PDM_ERROR AmbaFS_NandDrvInit(int ID, PDM_DISK *pDisk);
PDM_ERROR AmbaFS_NandFinalize(int ID, PDM_DISK *pDisk);
PDM_ERROR AmbaFS_NandMount(int ID, PDM_DISK *pDisk);
PDM_ERROR AmbaFS_NandUnmount(int ID, PDM_DISK *pDisk);
PDM_ERROR AmbaFS_NandFormat(int ID, PDM_DISK *pDisk, const PDM_U_CHAR *pParam);
PDM_ERROR AmbaFS_NandPhyRead(int ID, PDM_DISK *pDisk, PDM_U_CHAR *pBuf,
                             PDM_U_LONG Sector, PDM_U_LONG Sectors, PDM_U_LONG *pNumSuccess);
PDM_ERROR AmbaFS_NandPhyWrite(int ID, PDM_DISK *pDisk, const PDM_U_CHAR *pBuf,
                              PDM_U_LONG Sector, PDM_U_LONG Sectors, PDM_U_LONG *pNumSuccess);
PDM_ERROR AmbaFS_NandGetDiskInfo(int ID, PDM_DISK *pDisk, PDM_DISK_INFO *pDiskInfo);
PF_ERROR AmbaFS_NandDrvTblInit(int ID, PDM_DISK_TBL *pDiskTbl, PDM_U_LONG Ext);
#endif

PF_DRV_TBL *AmbaFS_Nand1Init(void);
void AmbsFS_Nand1SetScm(void);
void AmbaFS_Nand1CheckMedia(void);
void AmbaFS_Nand1CheckFormat(void);

PF_DRV_TBL *AmbaFS_Nand2Init(void);
void AmbsFS_Nand2SetScm(void);
void AmbaFS_Nand2CheckMedia(void);
void AmbaFS_Nand2CheckFormat(void);

#endif  /* _AMBA_FS_NAND_H_ */

