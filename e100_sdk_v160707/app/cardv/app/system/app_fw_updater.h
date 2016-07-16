/**
 *  @FileName       :: app_fw_updater.h
 *
 *  @Description    :: Definitions & Constants for User Firmware Updater through SD
 *
 *  @History        ::
 *
 *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (¡°Software¡±) are protected by intellectual property rights
 * including, without limitation, U.S. and/or foreign copyrights.  This Software is also the
 * confidential and proprietary information of Ambarella, Inc. and its licensors.  You may
 * not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed license
 * agreement or nondisclosure agreement with Ambarella, Inc. or its authorized
 * affiliates.	In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-
 * INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR
 * MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _AMBA_USER_FW_UPDATER_SD_H_
#define _AMBA_USER_FW_UPDATER_SD_H_

#include "AmbaNAND_Def.h"
#include "AmbaNAND_PartitionTable.h"

typedef enum _AMBA_FIRMWARE_TYPE_e_ {
    AMBA_FIRMWARE_SYS_SOFTWARE = 0,
    AMBA_FIRMWARE_DSP_uCODE,
    AMBA_FIRMWARE_SYS_ROM_DATA,
    AMBA_FIRMWARE_LINUX_KERNEL,     /* optional for Linux Kernel */
    AMBA_FIRMWARE_LINUX_ROOT_FS,

    AMBA_NUM_FIRMWARE_TYPE
} AMBA_FIRMWARE_TYPE_e;

typedef struct _AMBA_FIRMWARE_IMAGE_HEADER_s_ {
    UINT32  Crc32;                  /* CRC32 Checksum */
    UINT32  Version;                /* Version number */
    UINT32  Date;                   /* Date */
    UINT32  Length;                 /* Image length */
    UINT32  MemAddr;                /* Location to be loaded into memory */
    UINT32  Flag;                   /* Flag of partition    */
    UINT32  Magic;                  /* The magic number     */
    UINT32  Reserved[57];
} AMBA_FIRMWARE_IMAGE_HEADER_s;

typedef struct _AMBA_FIRMWARE_HEADER_s_ {
    char    ModelName[32];          /* model name */

    struct {
        UINT8   Major;              /* Major number */
        UINT8   Minor;              /* Minor number */
        UINT16  Svn;                /* SVN serial number */
    } VER_INFO_s;

    AMBA_PLOAD_PARTITION_s PloadInfo;

    UINT32  Crc32;                  /* CRC32 of entire Binary File: AmbaCamera.bin */

    struct {
        UINT32   Size;
        UINT32   Crc32;
    } __POST_ATTRIB_PACKED__ FwInfo[AMBA_NUM_FIRMWARE_TYPE];

    UINT32  PartitionSize[AMBA_NUM_NAND_PARTITION];
} __POST_ATTRIB_PACKED__ AMBA_FIRMWARE_HEADER_s;

/*-----------------------------------------------------------------------------------------------*\
 * Error codes
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FW_UPDATER_ERR_MAGIC             (-1)
#define AMBA_FW_UPDATER_ERR_LENGTH            (-2)
#define AMBA_FW_UPDATER_ERR_CRC               (-3)
#define AMBA_FW_UPDATER_ERR_VER_NUM           (-4)
#define AMBA_FW_UPDATER_ERR_VER_DATE          (-5)
#define AMBA_FW_UPDATER_ERR_PROG_IMAGE        (-6)
#define AMBA_FW_UPDATER_ERR_FIRM_FILE         (-7)
#define AMBA_FW_UPDATER_ERR_NO_MEM            (-8)
#define AMBA_FW_UPDATER_ERR_ILLEGAL_HEADER    (-9)
#define AMBA_FW_UPDATER_ERR_META_SET          (-10)
#define AMBA_FW_UPDATER_ERR_META_GET          (-11)
#define AMBA_FW_UPDATER_ERR_LOG_SET           (-12)
#define AMBA_FW_UPDATER_ERR_LOG_GET           (-13)
#define AMBA_FW_UPDATER_ERR_MP_RAW            (-14)
#define AMBA_FW_UPDATER_ERR_MP_PREFERENCE     (-15)
#define AMBA_FW_UPDATER_ERR_MP_CALIB          (-16)
#endif /* _AMBA_USER_FW_UPDATER_SD_H_ */
