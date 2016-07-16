/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaFwGen.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Ambarella firmware generator.
 *
 *  @History        ::
 *      Date        Name        Comments
 *      11/15/2012  Kerson      Created
 *
\*-------------------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <fts.h>
#include <sys/stat.h>
#include <unistd.h>

typedef unsigned int    UINT32;
typedef unsigned short  UINT16;
typedef unsigned char   UINT8;

#include "autoconf.h"
#include "bsp.h"

const unsigned int crc32_tab[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

static char *PartName[] = {
    "bst",
    "ptb",
    "bld",
    "pba",
    "sys",
    "dsp",
    "rom",
    "lnx",
    "rfs",
    "img"
};

static char PloadRegionInfoPath[256];

typedef enum _AMBA_NAND_PARTITION_ID_e_ {
    AMBA_NAND_PARTITION_BOOTSTRAP = 0,              /* for Bootstrap */
    AMBA_NAND_PARTITION_TABLE,                      /* for Partition Table */
    AMBA_NAND_PARTITION_BOOTLOADER,                 /* for Bootloader */
    AMBA_NAND_PARTITION_SD_FIRMWARE_UPDATE,         /* for SD Firmware Update s/w */
    AMBA_NAND_PARTITION_SYS_SOFTWARE,               /* for System Software */
    AMBA_NAND_PARTITION_DSP_uCODE,                  /* for DSP uCode */
    AMBA_NAND_PARTITION_SYS_ROM_DATA,               /* for System ROM Data */

    /* optional */
    AMBA_NAND_PARTITION_LINUX_KERNEL,               /* optional for Linux Kernel */
    AMBA_NAND_PARTITION_LINUX_ROOT_FS,              /* optional for Linux Root File System */
    AMBA_NAND_PARTITION_LINUX_HIBERNATION_IMAGE,    /* optional for Linux Hibernation Image */

    /* without pre-built image */
    AMBA_NAND_PARTITION_STORAGE0,                   /* for FAT: Drive 'A' */
    AMBA_NAND_PARTITION_STORAGE1,                   /* for FAT: Drive 'B'; Slot-0: Drive 'C'; Slot-1: Drive 'D' */
    AMBA_NAND_PARTITION_VIDEO_REC_INDEX,            /* for Video Recording Index */
    AMBA_NAND_PARTITION_USER_SETTING,               /* for User Settings */
    AMBA_NAND_PARTITION_CALIBRATION_DATA,           /* for Calibration Data */

    AMBA_NUM_NAND_PARTITION,                        /* Total Number of NAND Partitions */

    /* Total Number of NAND Media Partitions */
    AMBA_NUM_NAND_MEDIA_PARTITION = AMBA_NUM_NAND_PARTITION - AMBA_NAND_PARTITION_STORAGE0,
} AMBA_NAND_PARTITION_ID_e;

#define HAS_IMG_PARTS                   (AMBA_NAND_PARTITION_LINUX_ROOT_FS + 1)

/*-----------------------------------------------------------------------------------------------*\
 * This is the header for a flash image partition.
 *-----------------------------------------------------------------------------------------------*/

typedef enum _AMBA_FIRMWARE_IMAGE_e_ {
    AMBA_FIRMWARE_SYS_SOFTWARE = 0,
    AMBA_FIRMWARE_DSP_uCODE,
    AMBA_FIRMWARE_SYS_ROM_DATA,
    AMBA_FIRMWARE_LINUX_KERNEL,     /* optional for Linux Kernel */
    AMBA_FIRMWARE_LINUX_ROOT_FS,

    AMBA_NUM_FIRMWARE_IMAGE
} AMBA_FIRMWARE_IMAGE_e;

#define PLOAD_REGION_NUM  6
typedef struct _AMBA_PLOAD_PARTITION_s_ {
    UINT32  RegionRoStart[PLOAD_REGION_NUM];
    UINT32  RegionRwStart[PLOAD_REGION_NUM];
    UINT32  RegionRoSize[PLOAD_REGION_NUM];
    UINT32  RegionRwSize[PLOAD_REGION_NUM];
    UINT32  DspBufStart;
    UINT32  DspBufSize;
} AMBA_PLOAD_PARTITION_s;

typedef struct _AMBA_FIRMWARE_HEADER_s_ {
    char    ModelName[32];   /* model name */

    struct _VER_INFO_s_ {
        UINT8   Major;      /* Major number */
        UINT8   Minor;      /* Minor number */
        UINT16  Svn;        /* SVN serial number */
    } VerInfo;

    AMBA_PLOAD_PARTITION_s PloadInfo; /* Pload region info */

    UINT32  CRC32;          /* CRC32 of entire Binary File: AmbaCamera.bin */

    struct {
        UINT32   Size;
        UINT32   Crc32;
    } AmbaFwInfo[AMBA_NUM_FIRMWARE_IMAGE];

    UINT32  PartitionSize[AMBA_NUM_NAND_PARTITION];
} AMBA_FIRMWARE_HEADER_s;

/* ---------------------------------------------------------------------------*/

typedef enum _AMBA_BOOT_PARTITION_FIRMWARE_IMAGE_e_ {
    AMBA_FIRMWARE_BST_BOOTSTRAP = 0,
    AMBA_FIRMWARE_PTB_PARTITION_TABLE,
    AMBA_FIRMWARE_BLD_BOOTLOADER,
    AMBA_FIRMWARE_PBA_FIRMWARE_UPDATER,

    AMBA_NUM_BOOT_PARTITION_FIRMWARE_IMAGE
} AMBA_BOOT_PARTITION_FIRMWARE_IMAGE_e;

typedef struct _AMBA_BOOT_PARTITION_FIRMWARE_HEADER_s_ {
    char    ModelName[32];   /* model name */

    struct _VER_INFO_BOOT_PARTITION_s_ {
        UINT8   Major;      /* Major number */
        UINT8   Minor;      /* Minor number */
        UINT16  Svn;        /* SVN serial number */
    } VerInfo;

    UINT32  CRC32;          /* CRC32 of entire Binary File: AmbaCamera.bin */

    struct {
        UINT32   Size;
        UINT32   Crc32;
    } AmbaFwInfo[AMBA_NUM_BOOT_PARTITION_FIRMWARE_IMAGE];

    UINT32  PartitionSize[AMBA_NUM_NAND_PARTITION];
} AMBA_BOOT_PARTITION_FIRMWARE_HEADER_s;

/* ---------------------------------------------------------------------------*/

typedef struct _AMBA_FW_FILE_s_ {
    char    FileName[32];
    UINT32  ImgLen;
} AMBA_FW_FILE_s;

static int GetPartNum(char *FileName)
{
    if (!strncmp(FileName,      "bst.bin", 7))          return AMBA_NAND_PARTITION_BOOTSTRAP;
    else if (!strncmp(FileName, "bld.bin", 7))          return AMBA_NAND_PARTITION_BOOTLOADER;
    else if (!strncmp(FileName, "pba.bin", 7))          return AMBA_NAND_PARTITION_SD_FIRMWARE_UPDATE;
    else if (!strncmp(FileName, "sys.bin", 7))          return AMBA_NAND_PARTITION_SYS_SOFTWARE;
    else if (!strncmp(FileName, "dsp_fw.bin", 10))      return AMBA_NAND_PARTITION_DSP_uCODE;
    else if (!strncmp(FileName, "rom_fw.bin", 10))      return AMBA_NAND_PARTITION_SYS_ROM_DATA;
    else if (!strncmp(FileName, "lnx.bin", 6))          return AMBA_NAND_PARTITION_LINUX_KERNEL;
    else if (!strncmp(FileName, "rfs.bin", 6))          return AMBA_NAND_PARTITION_LINUX_ROOT_FS;
    return -1;
}
#if 0
void DumpHeader(AMBA_FIRMWARE_HEADER_s *pFwHeader)
{
    int i;

    if (pFwHeader->ModelName != NULL && (*pFwHeader->ModelName) != '\0')
        printf("\r\nModelName %s\r\n", pFwHeader->ModelName);

    printf("CRC32 0x%x\r\n",       pFwHeader->CRC32);

    for (i = 0; i < AMBA_NUM_FIRMWARE_IMAGE; i++) {
        printf("Part %d Crc32 = 0x%x\r\n",   i, pFwHeader->AmbaFwInfo[i].Crc32);
        printf("Part %d Size =  %d\r\n\r\n", i, pFwHeader->AmbaFwInfo[i].Size);
    }

    for (i = 0; i < HAS_IMG_PARTS; i++) {
        printf("PartitionSize %d = 0x%x\r\n", i, pFwHeader->PartitionSize[i]);
    }
}
#endif
const int AmbaFW_PartitionSize[AMBA_NUM_NAND_PARTITION] = {
    /* 2KB fixed size. Can not be changed. */
    [AMBA_NAND_PARTITION_BOOTSTRAP]                    = AMBA_BOOTSTRAPE_SIZE,

    /* Firmware (System code) partitions */
    [AMBA_NAND_PARTITION_TABLE]                        = AMBA_PARTITION_TABLE_SIZE,
    [AMBA_NAND_PARTITION_BOOTLOADER]                   = AMBA_BOOTLOADER_SIZE,
    [AMBA_NAND_PARTITION_SD_FIRMWARE_UPDATE]           = AMBA_SD_FWUPDATE_SIZE,
    [AMBA_NAND_PARTITION_SYS_SOFTWARE]                 = AMBA_SYSTEM_SOFTWARE_SIZE,

    /* Firmware (Read-Only, uCode and ROM) partitions */
    [AMBA_NAND_PARTITION_DSP_uCODE]                    = AMBA_DSP_SIZE,
    [AMBA_NAND_PARTITION_SYS_ROM_DATA]                 = AMBA_ROM_SIZE,

    [AMBA_NAND_PARTITION_LINUX_KERNEL]                 = AMBA_LINUX_SIZE,
    [AMBA_NAND_PARTITION_LINUX_ROOT_FS]                = AMBA_ROOTFS_SIZE,
    [AMBA_NAND_PARTITION_LINUX_HIBERNATION_IMAGE]      = AMBA_HIBERNATION_SIZE,

    /* Media partitions */
    [AMBA_NAND_PARTITION_STORAGE0]                     = MP_STORAGE1_SIZE,
    [AMBA_NAND_PARTITION_STORAGE1]                     = MP_STORAGE2_SIZE,
    [AMBA_NAND_PARTITION_VIDEO_REC_INDEX]              = MP_INDEX_SIZE,
    [AMBA_NAND_PARTITION_USER_SETTING]                 = MP_PREFERENCE_SIZE,
    [AMBA_NAND_PARTITION_CALIBRATION_DATA]             = MP_CALIBRATION_SIZE,
};

static AMBA_PLOAD_PARTITION_s g_PloadInfo;

static int GenerateSdFirmware(AMBA_FW_FILE_s *pFwFile, char *OutfileName,
                              int StartPartNum, int TotalPartNum, int IsBootPart)
{
    AMBA_FIRMWARE_HEADER_s FwHeader;
    AMBA_BOOT_PARTITION_FIRMWARE_HEADER_s BootFwHeader;
    unsigned int crc = ~0U;
    int i;

    /* Generate multiple Firmware File. */
    FILE *fout = fopen(OutfileName, "wb");
    if (fout == NULL) {
        printf("\nunable to open '%s' for output!\n", OutfileName);
        return 0;
    }

    if (IsBootPart) {
        memset((void *)&BootFwHeader, 0x0, sizeof(AMBA_BOOT_PARTITION_FIRMWARE_HEADER_s));
        fseek(fout, sizeof(BootFwHeader), SEEK_SET);
    } else {
        memset((void *)&FwHeader, 0x0, sizeof(AMBA_FIRMWARE_HEADER_s));
        fseek(fout, sizeof(FwHeader), SEEK_SET);
    }

    crc = ~0U;
    for (i = StartPartNum; i < (StartPartNum + TotalPartNum); i++) {
        static char buf[0x40000];
        char *p;
        size_t len, flen = 0;
        FILE *fin;

        if (pFwFile[i].ImgLen == 0)
            continue;

        if ((fin = fopen(pFwFile[i].FileName, "rb")) == NULL) {
            continue;
        }

        while ((len = fread(buf, 1, sizeof(buf), fin)) > 0) {
            fwrite(buf, 1, len, fout);
            flen += len;

            p = buf;
            while (len--) {
                crc = crc32_tab[(crc ^ *p++) & 0xff] ^ (crc >> 8);
            }
        }
        fclose(fin);
        if (IsBootPart) {
            BootFwHeader.AmbaFwInfo[i].Size = flen;
            BootFwHeader.AmbaFwInfo[i].Crc32 = crc;
        } else {
            FwHeader.AmbaFwInfo[i - AMBA_NAND_PARTITION_SYS_SOFTWARE].Size = flen;
            FwHeader.AmbaFwInfo[i - AMBA_NAND_PARTITION_SYS_SOFTWARE].Crc32 = crc;
        }
#if 1
        if (i == AMBA_NAND_PARTITION_SYS_SOFTWARE)
            FwHeader.PloadInfo = g_PloadInfo;
#endif
    }
    crc ^= ~0U;

    fseek(fout, 0, SEEK_SET);

    if (IsBootPart) {
#ifdef FW_MODEL_NAME
        strcpy(BootFwHeader.ModelName, FW_MODEL_NAME);
#endif
        BootFwHeader.CRC32 = crc;
        memcpy(BootFwHeader.PartitionSize, AmbaFW_PartitionSize, sizeof(AmbaFW_PartitionSize));
        fwrite(&BootFwHeader, sizeof(FwHeader), 1, fout);
    } else {
#ifdef FW_MODEL_NAME
        strcpy(FwHeader.ModelName, FW_MODEL_NAME);
#endif
        FwHeader.CRC32 = crc;
        memcpy(FwHeader.PartitionSize, AmbaFW_PartitionSize, sizeof(AmbaFW_PartitionSize));
        fwrite(&FwHeader, sizeof(FwHeader), 1, fout);
        //DumpHeader(&FwHeader);
    }

    fclose(fout);
    return 0;
}

static int GetOutputFileName(AMBA_FW_FILE_s *pFwFile, char *OutfileName, int StartPartNum, int EndPartNum)
{
    int i, count = 0;
    for (i = StartPartNum; i <= EndPartNum; i++) {

        if ((pFwFile[i].ImgLen == 0) || (i == AMBA_NAND_PARTITION_TABLE))
            continue;

        if (i != StartPartNum) {
            strcat(OutfileName, "_");
        }
        strcat(OutfileName, PartName[i]);
        count += 1;
    }
    strcat(OutfileName, ".devfw");
#if 0
    if (count > 0)
     fprintf(stderr, "  GEN     %s\r\n", OutfileName);
#endif
    return count;
}

static int GetFwFileName(int argc, char **argv, AMBA_FW_FILE_s *pFwFile)
{
    char *dot[] = {".", 0};
    char **paths = dot;
    FTSENT *node;
    FTS *tree = fts_open(paths, FTS_NOCHDIR, 0);
    if (!tree) {
        perror("fts_open");
        return -1;
    }

    while ((node = fts_read(tree))) {
        if (node->fts_level > 0 && node->fts_name[0] == '.') {
            fts_set(tree, node, FTS_SKIP);
        } else if (node->fts_info & FTS_F) {
            /* Find file */
            int PartNum = GetPartNum(node->fts_name);
            if (PartNum < 0)
                continue;
            strcpy(pFwFile[PartNum].FileName, node->fts_name);
            pFwFile[PartNum].ImgLen = node->fts_statp->st_size;
            /* fprintf(stderr, "Found %s\r\n", pFwFile[PartNum].FileName); */
        }
    }

    sprintf(PloadRegionInfoPath, "%s%s", argv[3], "AmbaFwLoader_RegionInfo.list");

    if (fts_close(tree)) {
        perror("fts_close");
        return 1;
    }
    return 0;
}

static void GetPloadRegionInfo(char *buf)
{
   char RegionName[16];
   UINT32 Value, Value2;

   if (buf == NULL)
      return;

   sscanf(buf, "%s 0x%x", RegionName, &Value);
   if (!strcmp(RegionName, "REGION0_RO_BASE")) {
         g_PloadInfo.RegionRoStart[0] = Value;
       //fprintf(stderr,"#### g_PloadInfo.RegionRotart[0] = 0x%x\n", Value);
      return;
   } else if (!strcmp(RegionName, "REGION0_RW_BASE")) {
       g_PloadInfo.RegionRwStart[0] = Value;
       //fprintf(stderr,"#### g_PloadInfo.RegionRwtart[0] = 0x%x\n", Value);
       return;
   } else if (!strcmp(RegionName, "REGION5_RO_BASE")) {
       g_PloadInfo.RegionRoStart[5] = Value;
       //fprintf(stderr,"#### g_PloadInfo.RegionRotart[5] = 0x%x\n", Value);
       return;
   } else if (!strcmp(RegionName, "REGION5_RW_BASE")) {
       g_PloadInfo.RegionRwStart[5] = Value;
       //fprintf(stderr,"#### g_PloadInfo.RegionRwtart[5] = 0x%x\n", Value);
       return;
   } else if (!strcmp(RegionName, "DSP_BUF_BASE")) {
       g_PloadInfo.DspBufStart = Value;
       //fprintf(stderr,"#### g_PloadInfo.DspBufStart = 0x%x\n", Value);
       return;
   }

   sscanf(buf, "%s (0x%x - 0x%x)", RegionName, &Value, &Value2);
   if (!strcmp(RegionName, "REGION0_RO_LENGTH")) {
       g_PloadInfo.RegionRoSize[0] = (Value - Value2);
       //fprintf(stderr,"#### g_PloadInfo.RegionRoSize[0] = 0x%x\n", g_PloadInfo.RegionRoSize[0]);
       return;
   } else if (!strcmp(RegionName, "REGION0_RW_LENGTH")) {
       g_PloadInfo.RegionRwSize[0] = (Value - Value2);
       //fprintf(stderr,"#### g_PloadInfo.RegionRwSize[0] = 0x%x\n", g_PloadInfo.RegionRwSize[0]);
       return;
   } else if (!strcmp(RegionName, "REGION5_RO_LENGTH")) {
       g_PloadInfo.RegionRoSize[5] = (Value - Value2);
       //fprintf(stderr,"#### g_PloadInfo.RegionRoSize[5] = 0x%x\n", g_PloadInfo.RegionRwSize[5]);
       return;
   } else if (!strcmp(RegionName, "REGION5_RW_LENGTH")) {
       g_PloadInfo.RegionRwSize[5] = (Value - Value2);
       //fprintf(stderr,"#### g_PloadInfo.RegionRwSize[5] = 0x%x\n", g_PloadInfo.RegionRwSize[5]);
       return;
   } else if (!strcmp(RegionName, "DSP_BUF_LENGTH")) {
       g_PloadInfo.DspBufSize = (Value - Value2);
       //fprintf(stderr,"#### g_PloadInfo.DspBufSize = 0x%x\n", g_PloadInfo.DspBufSize);
      return;
   }

   return;

}

AMBA_FW_FILE_s FwFile[HAS_IMG_PARTS];

int main(int argc, char **argv)
{
    int i, Rval;

    memset(FwFile, 0x0, sizeof(FwFile));

    if (argc < 2) {
        fprintf(stderr, "useage: host_sdfw_gen [input path]  [output path]\r\n");
        return -1;
    }

    if (chdir(argv[1]) < 0)
        return -1;

    GetFwFileName(argc, argv, FwFile);

    // Get PloadInfo from list
    if (PloadRegionInfoPath != NULL) {
        FILE *fout = NULL;
        static char buf[64];

        //fprintf(stderr, " ********* %s\r\n", PloadRegionInfoPath);
        fout = fopen(PloadRegionInfoPath, "r");
      if (fout != NULL) {
            while(fgets(buf, 64, fout) != NULL){
                  //fprintf(stderr, "%s", buf);
                  GetPloadRegionInfo(buf);
            }
          }
        fclose(fout);
    }

    /* Single IMAGE file */
    for (i = AMBA_NAND_PARTITION_SYS_SOFTWARE; i < AMBA_NAND_PARTITION_LINUX_HIBERNATION_IMAGE; i++) {
        char OutfileName[256] = {0}, OutfileNameFullPath[256] = {0};
        if (0 == GetOutputFileName(FwFile, OutfileName, i, i))
            continue;

        sprintf(OutfileNameFullPath, "%s%s", argv[2], OutfileName);
        fprintf(stderr, "  Gen     %s\r\n", OutfileNameFullPath);
        Rval = GenerateSdFirmware(FwFile, OutfileNameFullPath, i, 1, 0);
        if (Rval < 0)
            return -1;
    }

    /* Multiple IMAGE file */
    for (i = AMBA_NAND_PARTITION_SYS_SOFTWARE; i < AMBA_NAND_PARTITION_LINUX_ROOT_FS; i++) {
        char OutfileName[256] = {0}, OutfileNameFullPath[256] = {0};
        if (0 == GetOutputFileName(FwFile, OutfileName, i, AMBA_NAND_PARTITION_LINUX_ROOT_FS))
            continue;

        sprintf(OutfileNameFullPath, "%s%s", argv[2], OutfileName);
        fprintf(stderr, "  Gen     %s\r\n", OutfileNameFullPath);
        Rval = GenerateSdFirmware(FwFile, OutfileNameFullPath, i, AMBA_NUM_FIRMWARE_IMAGE, 0);
        if (Rval < 0)
            return -1;
    }

    /* Boot partition */
    for (i = AMBA_NAND_PARTITION_BOOTSTRAP; i < AMBA_NAND_PARTITION_SD_FIRMWARE_UPDATE; i++) {
        char OutfileName[256] = {0}, OutfileNameFullPath[256] = {0};
        if (i == AMBA_NAND_PARTITION_TABLE)
            continue;
        if (0 == GetOutputFileName(FwFile, OutfileName, i, AMBA_NAND_PARTITION_SD_FIRMWARE_UPDATE))
            continue;

        sprintf(OutfileNameFullPath, "%s%s", argv[2], OutfileName);
        fprintf(stderr, "  Gen     %s\r\n", OutfileNameFullPath);
        Rval = GenerateSdFirmware(FwFile, OutfileName, i, AMBA_NUM_FIRMWARE_IMAGE, 1);
        if (Rval < 0)
            return -1;
    }
    return 0;
}

