/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaFS_Format.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Ambarella file system abstraction layer.
 *
 *  @History        ::
 *      Date        Name        Comments
 *      10/24/2012  Evan Chen    Created
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_FS_FORMAT_H_
#define _AMBA_FS_FORMAT_H_

/* #include <prfile.h> */

#define SEC_SIZE            512 /* size of sector in byte */
#define SEC_SHIFT           9   /* used to caculate sectors */
#define ROOT_DIR_ENTRY      256 /* number of root directory entries */
#define NFAT                2   /* number of fat tables */
#define FAT32_BS_SIZE       32  /* number of boot sectors for fat32 */
#define FAT16_BS_SIZE       1   /* number of boot sector of fat16 */
#define DIR_ENTRY_SIZE          (32)

#define RSV_MBR_CLUST       1   /* reserved cluster for MBR */

/* Define for flash, xD, CF or SD drive */
#define PRF_NAND                AMBA_FIO_NAND
#define PRF_SD                  AMBA_FIO_SD
#define PRF_SD2                 (PRF_SD + 1)
#define PRF_SDIO                (PRF_SD2 + 1)
#define PRF_NAND2               (PRF_SDIO + 1)

#define NAND_NHEAD              0x10
#define NAND_SPT                0x3f

/**
 * Used in creating data to be written to storage devices during formatting.
 */
typedef struct __AMBA_FS_DRIVE_INFO__ {
    UINT32 StartLba;        /**< Start of logical block address */
    UINT32 StartLbaCustom;  /* The customized StartLba value */
    UINT32 EndLba;
    UINT32 BsSizeCustom;    /** The customized boot sector size **/
    UINT32 TotalSector;     /**< Total number of sectors */
    UINT16 NumOfHead;       /**< Number of heads */
    UINT16 Spt;             /**< Sectors per track */
    UINT16 NumCyls;         /**< number of cylinders. */
    UINT16 Spc;             /**< Sectors per cluster */
    UINT32 Spf;             /**< Sectors per FAT */
    UINT16 RdeCustom;       /**< The customized Number of Root-directory Entries */
    UINT16 FatType;         /**< The valid values are: 12, 16, 32 */
    UINT16 FatId;           /**< FAT12/FAT16(S)/FAT32 */
    UINT16 Drive;           /**< Flash, xD, CF or SD Drive */
} AMBA_FS_DRIVE_INFO;

/**
 * File system information sector.
 */
typedef struct _AMBA_FS_FSI_INFO_ {
    UINT32 Title;
    UINT8  Rsv1[480];
    UINT32 Signature;
    UINT32 FsiEmpClst;      /**< Number of empty clster */
    UINT32 FsiNextClsNo;    /**< Last access cluster */
    UINT32 Rsv2[3];
    UINT16 Rsv3;
    UINT16 MagicNum;       /**< 0xaa55 */
} AMBA_FS_FSI_INFO;

/**
 * Function proto-type for 'Write Sector Function'.
 */
typedef int (*AMBA_FS_WR_SEC_FUNC)(unsigned char *, UINT32, UINT32);

/**
 * Function proto-type for 'Read Sector Function'.
 */
typedef int (*AMBA_FS_RD_SEC_FUNC)(unsigned char *, UINT32, UINT32);

/**
 * Fill in pBuffer to be written to the master boot record (MBR) sector.
 *
 * @param pDinfo - Drive info
 * @param pBuf - Output pBuffer
 */
void AmbaFS_CreateMbr(AMBA_FS_DRIVE_INFO *pDinfo, UINT8 *pBuf);

/**
 * Fill in Buffer to be written to the boot sector.
 *
 * @param pDinfo - Drive info
 * @param pBuf - Output pBuffer
 */
void AmbaFS_CreateBs(AMBA_FS_DRIVE_INFO *pDinfo, UINT8 *pBuf);

/**
 * Fill in Buffer to be written to the file system information (FSI) sector.
 *
 * @param pDinfo - Drive info
 * @param bs32 - Boot sector data
 */
void AmbaFS_CreateFsi(AMBA_FS_DRIVE_INFO *pDinfo, AMBA_FS_FAT_BS32 *bs32, UINT8 *pBuf);

/**
 * create FAT table and write to the FAT area on device.
 *
 * @param pDinfo - Drive info
 * @param bs - Boot sector data
 * @param wsf - Write sector function
 * @returns - 1 if successful, -1 if fail
 */
int AmbaFS_CreateFat(AMBA_FS_DRIVE_INFO *pDinfo, void *bs, AMBA_FS_WR_SEC_FUNC WrSecFunc);

/**
 * Write the boot area on Drive.
 *
 * @param pDinfo - Drive info
 * @param wsf - Write sector function
 * @returns - 1 if successful, -1 if fail
 */
int AmbaFS_CreateBootArea(AMBA_FS_DRIVE_INFO *pDinfo, AMBA_FS_WR_SEC_FUNC WrSecFunc);

/**
 * Write the boot area on Drive.
 *
 * @param bs - pointer to boot sector
 * @param wsf - Write sector function
 * @param rsf - Read sector function
 * @returns - 1 if successful, -1 if fail
 */
int AmbaFS_CreateExfatEntry(AMBA_FS_EXFAT_BS *pBs,
                            AMBA_FS_WR_SEC_FUNC WrSecFunc, AMBA_FS_RD_SEC_FUNC RdSecFunc,
                            UINT8 *pExtraBuf, int ExtraSize);

/**
 * Utility function for computing the number of sectors per FAT.
 *
 * @param FatType - The valid values are: 12, 16, 32
 * @param tsector - Total number of sectors
 * @param nbs - Number of boot sectors
 * @param nrs - Number of reserved sectors
 * @param spc - Sectors per track
 * @returns - Number of sectors per FAT
 */
UINT32 AmbaFS_GetSectorsPerFat(UINT8 FatType, UINT32 Tsector, UINT8 Nbs, UINT8 Nrs, UINT8 Spc);

/**
 * Utility function for computing the number of sectors per FAT (used
 * exclusively by NAND flash devices).
 *
 * @param FatType - The valid values are: 12, 16, 32
 * @param tclust - Total number of clusters
 * @param spc - Sectors per track
 * @returns - Number of sectors per FAT
 */
UINT32 AmbaFS_GetFlashSectorsPerFat(UINT8 FatType, UINT32 Tclust, UINT16 Spc);

int AmbaFS_ParseFormatParam(int DevSize, char *pParam, AMBA_FS_DRIVE_INFO *pDrvInfo);

void AmbaFS_ParseSdaFormatParam(int SecCnt, AMBA_FS_DRIVE_INFO *pDrvInfo);

/**
 * FAT type determination.
 *
 * @param pbs - Boot sector data
 * @returns - FAT type
 */
int AmbaFS_FatTypeDetermination(AMBA_FS_FAT_BS32 * pbs);

#endif  /* _AMBA_FS_FORMAT_H_ */