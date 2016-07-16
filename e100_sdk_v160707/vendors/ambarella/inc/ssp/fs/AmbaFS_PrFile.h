/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaFS_PrFile.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No papRt of this file may be reproduced, stored in a retrieVal system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Ambarella file system abstraction layer.
 *
 *  @History        ::
 *      Date        Name        Comments
 *      10/24/2012  Evan Chen   Created
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef __AMBA_FS_PRF_H__
#define __AMBA_FS_PRF_H__

#include <prfile2/pdm_driver.h>
#include <prfile2/pf_api.h>

#define     U_CHAR      unsigned char
#define     U_INT       unsigned int
#define     U_SHORT     unsigned short int
#define     U_LONG      unsigned long

#ifndef     DRV_SUCCEED
#define     DRV_SUCCEED     0
#endif
#ifndef     DRV_FAILED
#define     DRV_FAILED      -1
#endif

/* Maximum dirive to suppopRt */
#define PRF_MAX_DRIVE   PF_MAX_DISK

__BEGIN_C_PROTO__

void empty(void);

#define PRF2_FORMAT_LAYER_SELECTABLE    1
#define ENABLE_FSI                      0

#if (PRF2_FORMAT_LAYER_SELECTABLE)
#define PRF2_MEDIA_ATTR PDM_MEDIA_ATTR_NOT_EXE_LOGICAL_FORMAT
#else
#define PRF2_MEDIA_ATTR 0
#endif

#define PRF_ATTATCHED   0x1
#define PRF_MOUNTED     0x2

/* Flash media block driver interface */

/* FL1 */
PDM_ERROR AmbaFS_Fl1PrfDriveInit(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_Fl1PrfFinalize(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_Fl1PrfMount(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_Fl1PrfUnmount(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_Fl1PrfFormat(PDM_DISK *pDisk, const PDM_U_CHAR *pParam);
PDM_ERROR AmbaFS_Fl1Prf2Pread(PDM_DISK *pDisk, PDM_U_CHAR *pBuf,
                              PDM_U_LONG Sector, PDM_U_LONG Sectors,
                              PDM_U_LONG *pNumSuccess);

PDM_ERROR AmbaFS_Fl1Prf2Pwrite(PDM_DISK *pDisk, const PDM_U_CHAR *pBuf,
                               PDM_U_LONG Sector, PDM_U_LONG Sectors,
                               PDM_U_LONG *pNumSuccess);

int AmbaFS_Fl1PrfPread(U_CHAR *pBuf, long Sector, int Sectors);
int AmbaFS_Fl1PrfPwrite(U_CHAR *pBuf, long Sector, int Sectors);
int AmbaFS_Fl1Prf2IoStatSet(int StatisticEnable);
int AmbaFS_Fl1Prf2IoStatGet(UINT64 *pRb, UINT64 *pWb, UINT32 *pRt, UINT32 *pWt,
                            UINT32 *pRlvl, UINT32 *pWlvl);
int AmbaFS_Fl1Prf2Iostat2ndSet(int StatisticEnable);
int AmbaFS_Fl1Prf2IoStat2ndGet(UINT64 *pRb, UINT64 *pWb, UINT32 *pRt, UINT32 *pWt,
                               UINT32 *pRlvl, UINT32 *pWlvl);
UINT32 AmbaFS_Fl1Prf2FsInfo(void *pBs);

PDM_ERROR AmbaFS_Fl1PrfGetDiskInfo(PDM_DISK *pDisk, PDM_DISK_INFO *pDiskInfo);
PF_DRV_TBL *AmbaFS_Fl1PrfInit(void);
void AmbaFS_Fl1PrfCheckMedia(void);
int AmbaFS_Fl1Prf2FormatDrive(const char *pParam);

/* FL2 */
PDM_ERROR AmbaFS_Fl2PrfDriveInit(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_Fl2PrfFinalize(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_Fl2PrfMount(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_Fl2PrfUnmount(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_Fl2PrfFormat(PDM_DISK *pDisk, const PDM_U_CHAR *pParam);
PDM_ERROR AmbaFS_Fl2Prf2Pread(PDM_DISK *pDisk, PDM_U_CHAR *pBuf,
                              PDM_U_LONG Sector, PDM_U_LONG Sectors,
                              PDM_U_LONG *pNumSuccess);

PDM_ERROR AmbaFS_Fl2Prf2Pwrite(PDM_DISK *pDisk, const PDM_U_CHAR *pBuf,
                               PDM_U_LONG Sector, PDM_U_LONG Sectors,
                               PDM_U_LONG *pNumSuccess);

int AmbaFS_Fl2PrfPread(U_CHAR *pBuf, long Sector, int Sectors);
int AmbaFS_Fl2PrfPwrite(U_CHAR *pBuf, long Sector, int Sectors);
int AmbaFS_Fl2Prf2IoStatSet(int StatisticEnable);
int AmbaFS_Fl2Prf2IoStatGet(UINT64 *pRb, UINT64 *pWb, UINT32 *pRt, UINT32 *pWt,
                            UINT32 *pRlvl, UINT32 *pWlvl);
int AmbaFS_Fl2Prf2Iostat2ndSet(int StatisticEnable);
int AmbaFS_Fl2Prf2IoStat2ndGet(UINT64 *pRb, UINT64 *pWb, UINT32 *pRt, UINT32 *pWt,
                               UINT32 *pRlvl, UINT32 *pWlvl);
UINT32 AmbaFS_Fl2Prf2FsInfo(void *pBs);

PDM_ERROR AmbaFS_Fl2PrfGetDiskInfo(PDM_DISK *pDisk, PDM_DISK_INFO *pDiskInfo);
PF_DRV_TBL *AmbaFS_Fl2PrfInit(void);
void AmbaFS_Fl2PrfCheckMedia(void);
int AmbaFS_Fl2Prf2FormatDrive(const char *pParam);


/* SD media block driver interface */
/* PrFILE2 SD block driver interface */
int AmbaFS_SdPrf2FormatDrive(int Id, const char *pParam);
PF_ERROR AmbaFS_SdPrf2InitDriveTable(int Id, PDM_DISK_TBL* pDiskTbl, PDM_U_LONG UiExt);
PF_DRV_TBL *AmbaFS_SdPrf2init(int Id);
PDM_ERROR AmbaFS_SdPrf2DriveInit(int Id, PDM_DISK *pDisk);
PDM_ERROR AmbaFS_SdPrf2Finalize(int Id, PDM_DISK *pDisk);
PDM_ERROR AmbaFS_SdPrf2Mount(int Id, PDM_DISK *pDisk);
PDM_ERROR AmbaFS_SdPrf2Unmount(int Id, PDM_DISK *pDisk);
PDM_ERROR AmbaFS_SdPrf2Format(int Id, PDM_DISK *pDisk, const PDM_U_CHAR *pParam);
PDM_ERROR AmbaFS_SdPrf2Pread(int Id,
                             PDM_DISK *pDisk,
                             PDM_U_CHAR *pBuf,
                             PDM_U_LONG Sector,
                             PDM_U_LONG Sectors,
                             PDM_U_LONG *pNumSuccess);
PDM_ERROR AmbaFS_SdPrf2Pwrite(int Id,
                              PDM_DISK *pDisk,
                              const PDM_U_CHAR *pBuf,
                              PDM_U_LONG Sector,
                              PDM_U_LONG Sectors,
                              PDM_U_LONG *pNumSuccess);
PDM_ERROR AmbaFS_SdPrf2GetDiskInfo(int Id, PDM_DISK *pDisk, PDM_DISK_INFO *pDiskInfo);
int AmbaFS_SdPrf2IoStatSet(int Id, int StatisticEnable);
int AmbaFS_SdPrf2IoStatGet(int Id, UINT64 *pRb, UINT64 *pWb, UINT32 *pRt, UINT32 *pWt,
                           UINT32 *pRlvl, UINT32 *pWlvl);
int AmbaFS_SdPrf2IoStat2ndSet(int Id, int StatisticEnable);
int AmbaFS_SdPrf2IoStat2ndGet(int Id, UINT64 *pRb, UINT64 *pWb, UINT32 *pRt, UINT32 *pWt,
                              UINT32 *pRlvl, UINT32 *pWlvl);

/* SD1 */
PDM_ERROR AmbaFS_Sd1PrfDriveInit(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_Sd1PrfFinalize(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_Sd1PrfMount(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_Sd1PrfUnmount(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_Sd1PrfFormat(PDM_DISK *pDisk, const PDM_U_CHAR *pParam);
PDM_ERROR AmbaFS_Sd1Prf2Pread(PDM_DISK *pDisk, PDM_U_CHAR *pBuf,
                              PDM_U_LONG Sector, PDM_U_LONG Sectors,
                              PDM_U_LONG *pNumSuccess);

PDM_ERROR AmbaFS_Sd1Prf2Pwrite(PDM_DISK *pDisk, const PDM_U_CHAR *pBuf,
                               PDM_U_LONG Sector, PDM_U_LONG Sectors,
                               PDM_U_LONG *pNumSuccess);

int AmbaFS_Sd1Pread(U_CHAR *pBuf, UINT32 Sector, UINT32 Sectors);
int AmbaFS_Sd1Pwrite(U_CHAR *pBuf, UINT32 Sector, UINT32 Sectors);
int AmbaFS_Sd1Erase(long, int);
int AmbaFS_Sd1Prf2IoStatSet(int StatisticEnable);
int AmbaFS_Sd1Prf2IoStatGet(UINT64 *pRb, UINT64 *pWb, UINT32 *pRt, UINT32 *pWt,
                            UINT32 *pRlvl, UINT32 *pWlvl);
int AmbaFS_Sd1Prf2IoStat2ndSet(int StatisticEnable);
int AmbaFS_Sd1Prf2IoStat2ndGet(UINT64 *pRb, UINT64 *pWb, UINT32 *pRt, UINT32 *pWt,
                               UINT32 *pRlvl, UINT32 *pWlvl);
UINT32 AmbaFS_Sd1Prf2FsInfo(void *pBs);

PDM_ERROR AmbaFS_Sd1PrfGetDiskInfo(PDM_DISK *pDisk, PDM_DISK_INFO *pDiskInfo);
int AmbaFS_Sd1Prf2FormatDrive(const char *pParam);
PF_ERROR AmbaFS_Sd1PrfInitDriveTable(PDM_DISK_TBL* pDiskTbl, PDM_U_LONG UiExt);

/* SD2 */
PDM_ERROR AmbaFS_Sd2PrfDriveInit(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_Sd2PrfFinalize(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_Sd2PrfMount(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_Sd2PrfUnmount(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_Sd2PrfFormat(PDM_DISK *pDisk, const PDM_U_CHAR *pParam);
PDM_ERROR AmbaFS_Sd2Prf2Pread(PDM_DISK *pDisk, PDM_U_CHAR *pBuf,
                              PDM_U_LONG Sector, PDM_U_LONG Sectors,
                              PDM_U_LONG *pNumSuccess);

PDM_ERROR AmbaFS_Sd2Prf2Pwrite(PDM_DISK *pDisk, const PDM_U_CHAR *pBuf,
                               PDM_U_LONG Sector, PDM_U_LONG Sectors,
                               PDM_U_LONG *pNumSuccess);

int AmbaFS_Sd2Pread(U_CHAR *pBuf, UINT32 Sector, UINT32 Sectors);
int AmbaFS_Sd2Pwrite(U_CHAR *pBuf, UINT32 Sector, UINT32 Sectors);
int AmbaFS_Sd2PrfPread(U_CHAR *pBuf, long Sector, int Sectors);
int AmbaFS_Sd2PrfPwrite(U_CHAR *pBuf, long Sector, int Sectors);
int AmbaFS_Sd2Erase(long Sector, int Sectors);
int AmbaFS_Sd2Prf2IoStatSet(int StatisticEnable);
int AmbaFS_Sd2Prf2IoStatGet(UINT64 *pRb, UINT64 *pWb, UINT32 *pRt, UINT32 *pWt,
                            UINT32 *pRlvl, UINT32 *pWlvl);
int AmbaFS_Sd2Prf2IoStat2ndSet(int StatisticEnable);
int AmbaFS_Sd2Prf2IoStat2ndGet(UINT64 *pRb, UINT64 *pWb, UINT32 *pRt, UINT32 *pWt,
                               UINT32 *pRlvl, UINT32 *pWlvl);
UINT32 AmbaFS_Sd2Prf2FsInfo(void *pBs);

PDM_ERROR AmbaFS_Sd2PrfGetDiskInfo(PDM_DISK *pDisk, PDM_DISK_INFO *pDiskInfo);
int AmbaFS_Sd2Prf2FormatDrive(const char *pParam);
PF_ERROR AmbaFS_Sd2PrfInitDriveTable(PDM_DISK_TBL* pDiskTbl, PDM_U_LONG UiExt);

/* SDIO */
PDM_ERROR AmbaFS_SdioPrfDriveInit(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_SdioPrfFinalize(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_SdioPrfMount(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_SdioPrfUnmount(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_SdioPrfFormat(PDM_DISK *pDisk, const PDM_U_CHAR *pParam);
PDM_ERROR AmbaFS_SdioPrf2Pread(PDM_DISK *pDisk, PDM_U_CHAR *pBuf,
                               PDM_U_LONG Sector, PDM_U_LONG Sectors,
                               PDM_U_LONG *pNumSuccess);

PDM_ERROR AmbaFS_SdioPrf2Pwrite(PDM_DISK *pDisk, const PDM_U_CHAR *pBuf,
                                PDM_U_LONG Sector, PDM_U_LONG Sectors,
                                PDM_U_LONG *pNumSuccess);

int AmbaFS_SdioPread(U_CHAR *pBuf, UINT32 Sector, UINT32 Sectors);
int AmbaFS_SdioPwrite(U_CHAR *pBuf, UINT32 Sector, UINT32 Sectors);
int AmbaFS_SdioPrfErase(long, int);
int AmbaFS_SdioPrf2IoStatSet(int StatisticEnable);
int AmbaFS_SdioPrf2IoStatGet(UINT64 *pRb, UINT64 *pWb, UINT32 *pRt, UINT32 *pWt,
                             UINT32 *pRlvl, UINT32 *pWlvl);
int AmbaFS_SdioPrf2Iostat2ndSet(int StatisticEnable);
int AmbaFS_SdioPrf2IoStat2ndGet(UINT64 *pRb, UINT64 *pWb, UINT32 *pRt, UINT32 *pWt,
                                UINT32 *pRlvl, UINT32 *pWlvl);
UINT32 AmbaFS_SdioPrf2FsInfo(void *pBs);

PDM_ERROR AmbaFS_SdioPrfGetDiskInfo(PDM_DISK *pDisk, PDM_DISK_INFO *pDiskInfo);
int AmbaFS_SdioPrf2FormatDrive(const char *pParam);
PF_ERROR AmbaFS_SdioPrfInitDriveTable(PDM_DISK_TBL* pDiskTbl, PDM_U_LONG UiExt);

/* Common functions for SD1, SD2, SDIO */
PF_DRV_TBL *AmbaFS_SdPrfInit(int Id);
void AmbaFS_SdPrf2CheckMedia(int Id);

/* Ramdisk media block driver interface */
PDM_ERROR AmbaFS_RdPrfDriveInit(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_RdPrfFinalize(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_RdPrfMount(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_RdPrfUnmount(PDM_DISK *pDisk);
PDM_ERROR AmbaFS_RdPrfFormat(PDM_DISK *pDisk, const PDM_U_CHAR *pParam);
PDM_ERROR AmbaFS_RdPrf2Pread(PDM_DISK *pDisk, PDM_U_CHAR *pBuf,
                             PDM_U_LONG Sector, PDM_U_LONG Sectors,
                             PDM_U_LONG *pNumSuccess);

PDM_ERROR AmbaFS_RdPrf2Pwrite(PDM_DISK *pDisk, const PDM_U_CHAR *pBuf,
                              PDM_U_LONG Sector, PDM_U_LONG Sectors,
                              PDM_U_LONG *pNumSuccess);

int AmbaFS_RdPrfPread(U_CHAR *pBuf, long Sector, int Sectors);
int AmbaFS_RdPrfPwrite(U_CHAR *pBuf, long Sector, int Sectors);
int AmbaFS_RdPrf2IoStatSet(int StatisticEnable);
int AmbaFS_RdPrf2IoStatGet(UINT64 *pRb, UINT64 *pWb, UINT32 *pRt, UINT32 *pWt,
                           UINT32 *pRlvl, UINT32 *pWlvl);
int AmbaFS_RdPrf2Iostat2ndSet(int StatisticEnable);
int AmbaFS_RdPrf2IoStat2ndGet(UINT64 *pRb, UINT64 *pWb, UINT32 *pRt, UINT32 *pWt,
                              UINT32 *pRlvl, UINT32 *pWlvl);
PDM_ERROR AmbaFS_RdPrfGetDiskInfo(PDM_DISK *pDisk, PDM_DISK_INFO *pDiskInfo);
PF_DRV_TBL *AmbaFS_RdPrfInit(void);
void AmbaFS_RdPrfCheckMedia(void);
int AmbaFS_RdPrf2FormatDrive(const char *pParam);
int AmbaFS_Prf2Format(char Drive, const char * pParam);
void AmbaFS_PrFileInit(void);

/**
 * PapRtition entry description.
 */
typedef struct __AMBA_FS_PART_ENTRY__ {
    UINT8   BootType;           /*<* boot type 0x80: boot, 0x00: not boot */
    UINT8   StartHead;         /**< stapRt Head                           */
    UINT8   StartSector;       /**< stapRt Sector                         */
    UINT8   StartCylinder;     /**< stapRt Cylinder                       */
    UINT8   FatType;            /**< FAT TYPE                             */
    UINT8   EndHead;            /**< end Head                             */
    UINT8   EndSector;          /**< end Sector                           */
    UINT8   EndCylinder;        /**< end Cylinder                         */
    UINT32  LogicStartSector;  /**< logical start Sector                 */
    UINT32  AllSector;          /**< ALL Sectors                          */
} __attribute__((packed)) AMBA_FS_PART_ENTRY;

/**
 * PapRtition table for hard disk-like file system.
 */
typedef struct __AMBA_FS_FAT_MBR__ {
    UINT8       BootProgram[446];   /**< consistency check routine    */
    AMBA_FS_PART_ENTRY  PartTable0;    /**< partition table entry        */
    AMBA_FS_PART_ENTRY  PartTable1;    /**< papRtition table entry        */
    AMBA_FS_PART_ENTRY  PartTable2;    /**< papRtition table entry        */
    AMBA_FS_PART_ENTRY  PartTable3;    /**< papRtition table entry        */
    UINT16      Signature;  /**< system byte 0x55, 0xaa       */
} __attribute__((packed)) AMBA_FS_FAT_MBR;

typedef struct __AMBA_FS_DOS_DATETIME__ {
    unsigned    Second: 5;
    unsigned    Minut: 6;
    unsigned    Hour: 5;
    unsigned    Day: 5;
    unsigned    Month: 4;
    unsigned    Year: 7;
} __attribute__((packed)) AMBA_FS_DOS_DATETIME;

/**
 * Boot Sector configuration (EX-FAT).
 */
typedef struct __AMBA_FS_EXFAT_BS__ {
    UINT8   Jmp;        /**< 0:Jump command                       */
    UINT8   Adr;        /**< 1:IPL address                        */
    UINT8   Nop;        /**< 2:NOP(0x90)                          */
    UINT8   Maker[8];   /**< 3-A:name                             */
    UINT8   Mbz[53];    /**< B-3F:MustBeZero              */
    /* BPB */
    UINT64  Pto;        /**< 40-47:PapRtition Offset          */
    UINT64  Vol;        /**< 48-4F:Volume Length          */
    UINT32  Fto;        /**< 50-53:Fat Offset             */
    UINT32  Ftl;        /**< 54-57:Fat Length             */
    UINT32  Cho;        /**< 58-5B:Cluster Heap Offset        */
    UINT32  Cnt;        /**< 5C-5F:Cluster Count          */
    UINT32  Fcr;        /**< 60-63:FirstCluster Of Root Directory */
    UINT32  Vsn;        /**< 64-67:Volume Seria Number(UINT32)    */
    UINT16  Fsr;        /**< 68-69:File System Revision       */
    UINT16  Vfg;        /**< 6A-6B:Volume Flags           */
    UINT8   Bps;        /**< 6C:Bytes Per Sector Shift        */
    UINT8   Spc;        /**< 6D:Sector Per Cluster Shift      */
    UINT8   Fts;        /**< 6E:Number Of Fats            */
    UINT8   Dsl;        /**< 6F:Drive Select              */
    UINT8   Piu;        /**< 70:Percent In Use            */
    /* end */
    UINT8   Rsv1[7];    /**< 71-77:reserved(all 00h)          */
    UINT8   Bcode[390]; /**< 78-1FD:Boot Code                     */
    UINT16  Mrk;        /**< 1FE-1FF:extension format mark(55,AA) */
} __attribute__((packed))AMBA_FS_EXFAT_BS;

/**
 * Generic Parameters Template (EX-FAT).
 */
typedef struct __AMBA_FS_EXFAT_PARAM_TMP__ {
    UINT64  Guid;       /**< Parameter GuId                       */
    UINT8   Custom[32]; /**< Custom Defined                       */
} __attribute__((packed))AMBA_FS_EXFAT_PARAM_TMP;

/**
 * Flash Parameters (EX-FAT).
 */
typedef struct __AMBA_FS_EXFAT_FLASH_PARAM__ {
    UINT8   Guid[16];   /**< Parameter GuId                       */
    UINT32  Ebs;        /**< Erase Block Size                     */
    UINT32  Pgs;        /**< Page Size                            */
    UINT32  Sps;        /**< Spare Sectors                        */
    UINT32  Rat;        /**< Random Access Time                   */
    UINT32  Pgt;        /**< Programming Time                     */
    UINT32  Rcy;        /**< Read Cycle                           */
    UINT32  Wcy;        /**< Write Cycle                          */
    UINT32  Rsv;        /**< Reserved                             */
} __attribute__((packed)) AMBA_FS_EXFAT_FLASH_PARAM;

/**
 * OEM Parameters (EX-FAT).
 */
typedef struct __AMBA_FS_EXFAT_PARAM_OEM__ {
    AMBA_FS_EXFAT_PARAM_TMP para[9];
    UINT8                   Rsv[32];
} __attribute__((packed)) AMBA_FS_EXFAT_PARAM_OEM;

/**
 * Generic DirectoryEntry Template (EX-FAT).
 */
typedef struct __AMBA_FS_EXFAT_ENTRY__ {
    UINT8   Type;
    UINT8   Custom[19];
    UINT32  FirstCluster;
    UINT64  Length;
} __attribute__((packed)) AMBA_FS_EXFAT_ENTRY;

/**
 * Allocation Bitmap DirectoryEntry (EX-FAT).
 */
typedef struct __AMBA_FS_EXFAT_BITMAP_ENTRY__ {
#define BMP_ENTRY_TYPE  0x81
    UINT8   Type;
    UINT8   BmpFlag;
    UINT8   Rsv[18];
    UINT32  FirstCluster;
    UINT64  Length;
} __attribute__((packed)) AMBA_FS_EXFAT_BITMAP_ENTRY;

/**
 * Up-case Table DirectoryEntry (EX-FAT).
 */
typedef struct __AMBA_FS_EXFAT_UPTBL_ENTRY__ {
#define UPTBL_ENTRY_TYPE    0x82
    UINT8   Type;
    UINT8   Rsv1[3];
    UINT32  ChkSum;
    UINT8   Rsv2[12];
    UINT32  FirstCluster;
    UINT64  Length;
} __attribute__((packed)) AMBA_FS_EXFAT_UPTBL_ENTRY;

/**
 * Boot Sector configuration (FAT32).
 */
typedef struct __AMBA_FS_FAT_BS32__ {
    UINT8   Jmp;        /**< 0:Jump command                       */
    UINT8   Adr;        /**< 1:IPL address                        */
    UINT8   Nop;        /**< 2:NOP(0x90)                          */
    UINT8   Maker[8];   /**< 3-A:name                             */
    /* BPB */
    UINT16  Bps;        /**< B-C:byte count of 1Sector            */
    UINT8   Spc;        /**< D:Sector count of 1cluster           */
    UINT16  Fno;        /**< E-F:stapRt recode number of FAT       */
    UINT8   Fts;        /**< 10:FAT number                        */
    UINT16  Ent;        /**< 11-12:entry count of root directory  */
    UINT16  All;        /**< 13-14:all count of desk              */
    UINT8   Med;        /**< 15:media descriptor                  */
    UINT16  Spf;        /**< 16-17:Sector count of 1FAT           */
    UINT16  Spt;        /**< 18-19:Sector count of 1track         */
    UINT16  Hed;        /**< 1A-1B:head count                     */
    UINT32  Hid;        /**< 1C-1F:hIdden Sector count            */
    UINT32  Lal;        /**< 20-23:all Sector count of volume desk*/
    /* for FAT32 */
    UINT32  Spf32;      /**< 24-27:recode count of 1FAT           */
    UINT16  Act32;      /**< 28-29:activeFAT                      */
    UINT16  Ver32;      /**< 2A-2B:file system version            */
    UINT32  Ent32;      /**< 2C-2F:head cluster of root directory */
    UINT16  Fsi32;      /**< 30-31:recode number of FS info       */
    UINT16  Bck32;      /**< 32-33:boot recode backup recode      */
    UINT8   Rsv32[12];  /*<* 34-3F:reserved                       */

    /* end */
    UINT8   Drvno;      /**< 40:physical drive number             */
    UINT8   Rsv0;       /**< 41:reserved                          */
    UINT8   Exb;        /**< 42:extension boot recode
                 *      discrimination code(29H) */
    UINT32  Vno;        /**< 43-46:volume serial number           */
    UINT8   Vnm[11];    /**< 47-51:volume label                   */
    UINT8   Typ[8];     /**< 52-59:file system type               */
    UINT8   Rsv1[420];  /**< 5A-1FD:reserved                      */
    UINT16  Mrk;        /**< 1FE-1FF:extension format mark(55,AA) */
} __attribute__((packed)) AMBA_FS_FAT_BS32;

/**
 * Boot Sector configuration (FAT16).
 */
typedef struct __AMBA_FS_FAT_BS16__ {
    UINT8   Jmp;        /**< 0:Jump command                       */
    UINT8   Adr;        /**< 1:IPL address                        */
    UINT8   Nop;        /**< 2:NOP(0x90)                          */
    UINT8   Maker[8];   /**< 3-A:Maker                            */
    /* BPB */
    UINT16  Bps;        /**< B-C:byte count of 1Sector            */
    UINT8   Spc;        /**< D:Sector count of 1cluster           */
    UINT16  Fno;        /**< E-F:stapRt record number of FAT       */
    UINT8   Fts;        /**< 10:FAT count                         */
    UINT16  Ent;        /**< 11-12:entry count of root directory  */
    UINT16  All;        /**< 13-14:all Sector number of disk      */
    UINT8   Med;        /**< 15:media discriptor                  */
    UINT16  Spf;        /**< 16-17:Sector count of 1FAT           */
    UINT16  Spt;        /**< 18-19:Sector count of 1track         */
    UINT16  Hed;        /**< 1A-1B:head count                     */
    UINT32  Hid;        /**< 1C-1F:hIdden Sector count            */
    UINT32  Lal;        /**< 20-23:all Sector count of volume desk*/
    /* end */
    UINT8   Drvno;      /**< 24:physical drive number             */
    UINT8   Rsv0;       /**< 25:reserved                          */
    UINT8   Exb;        /**< 26:extension boot record
                                discrimination code(29H)          */
    UINT32  Vno;        /**< 27-2A:volume serial number           */
    UINT8   Vnm[11];    /**< 2B-35:volume label                   */
    UINT8   Typ[8];     /**< 36-3D:file system type               */
    UINT8   Rsv1[448];  /**< 3E-1FD:reserved                      */
    UINT16  Mrk;        /**< 1FE-1FF:extension format mark(55,AA) */
} __attribute__((packed)) AMBA_FS_FAT_BS16;

__END_C_PROTO__

#endif  /* __AMBA_FS_PRF_H__ */
