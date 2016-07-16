/*
     Copyright (C) 2010 By eSOL Co.,Ltd. Tokyo, Japan

     This software is protected by the law and the agreement concerning
     a Japanese country copyright method, an international agreement,
     and other intellectual property right and may be used and copied
     only in accordance with the terms of such license and with the inclusion
     of the above copyright notice.

     This software or any other copies thereof may not be provided
     or otherwise made available to any other person.  No title to
     and ownership of the software is hereby transferred.

     The information in this software is subject to change without
     notice and should not be construed as a commitment by eSOL Co.,Ltd.
 */
 /****************************************************************************
 [pf_exfat_volume.h] - Volume Module.
 ****************************************************************************/
#ifndef __PF_EXFAT_VOLUME_H__
#define __PF_EXFAT_VOLUME_H__

#include "pf_common.h"

#if PF_EXFAT_SUPPORT
/****************************************************************************
  Type Definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
 PF_DENT_INFO - exFAT Directory entry Information
 ----------------------------------------------------------------------------*/
struct  PF_DENT_INFO_TAG
{
    /* Start cluster of data */
    PF_CLST       start_cluster;

    /* Length of data */
    PF_FSIZE_T    data_length;

    /* Checksum */
    PF_U_LONG     checksum;

};

/*------------------------------------------------------------------*
  Allocation Bitmap
 *------------------------------------------------------------------*/
struct PF_ALLOCATE_BMP_TAG
{
    /* Status */
    PF_U_SHORT      stat;

    /* Number of Accessed Allocation Bitmap */
    PF_S_LONG       num_accessed;

};

/* Status for PF_ALLOCATE_BMP */
#define PF_BMP_ACCESS                       (0x0001uL)
#define PF_BMP_UPDATE                       (0x0002uL)
#define PF_BMP_ERROR                        (0x0004uL)
#define PF_BMP_DIRTY                        (0x8000uL)

/****************************************************************************
  Variables
 ****************************************************************************/
/*---------------------------------------------------------------------------
  Accessor macros
 ----------------------------------------------------------------------------*/
#define IS_BMP_ACCESS(p_vol)                (((p_vol)->allocate_info.stat & PF_BMP_ACCESS) != 0)
#define IS_BMP_UPDATE(p_vol)                (((p_vol)->allocate_info.stat & PF_BMP_UPDATE) != 0)
#define IS_BMP_ERROR(p_vol)                 (((p_vol)->allocate_info.stat & PF_BMP_ERROR) != 0)
#define IS_BMP_DIRTY(p_vol)                 (((p_vol)->allocate_info.stat & PF_BMP_DIRTY) != 0)

#define SET_BMP_ACCESS(p_vol)               ((p_vol)->allocate_info.stat |= PF_BMP_ACCESS)
#define SET_BMP_UPDATE(p_vol)               ((p_vol)->allocate_info.stat |= PF_BMP_UPDATE)
#define SET_BMP_ERROR(p_vol)                (IS_BMP_ACCESS(p_vol) ? \
                                             ((p_vol)->allocate_info.stat |= PF_BMP_ERROR) : \
                                             ((p_vol)->allocate_info.stat))
#define SET_BMP_DIRTY(p_vol)                ((p_vol)->allocate_info.stat |= PF_BMP_DIRTY)

#define CLEAR_BMP_ACCESS(p_vol)             ((p_vol)->allocate_info.stat &= ~PF_BMP_ACCESS)
#define CLEAR_BMP_UPDATE(p_vol)             ((p_vol)->allocate_info.stat &= ~PF_BMP_UPDATE)
#define CLEAR_BMP_ERROR(p_vol)              ((p_vol)->allocate_info.stat &= ~PF_BMP_ERROR)
#define CLEAR_BMP_DIRTY(p_vol)              ((p_vol)->allocate_info.stat &= ~PF_BMP_DIRTY)

#define INIT_BMP_STAT(p_vol)                ((p_vol)->allocate_info.stat &= PF_BMP_DIRTY)

/*---------------------------------------------------------------------------
  flags
 ----------------------------------------------------------------------------*/
/*
 * VolumeFlags flag of exFAT
 */
#define EXT_FLAGS_EXFAT_ACTIVE_FAT_SECOND   (0x0001uL)
#define EXT_FLAGS_EXFAT_VOLUME_DIRTY        (0x0002uL)
#define EXT_FLAGS_EXFAT_MEDIA_FAILURE       (0x0004uL)
#define EXT_FLAGS_EXFAT_CLEAR_TO_ZERO       (0x0008uL)

/*
 * Volume configuration
 */

/* Check volume dirty */
#define IS_ACTIVE_FAT_SECOND(p_vol)         ((EXT_FLAGS(p_vol) & EXT_FLAGS_EXFAT_ACTIVE_FAT_SECOND) != 0)
#define IS_VOLUME_DIRTY(p_vol)              ((EXT_FLAGS(p_vol) & EXT_FLAGS_EXFAT_VOLUME_DIRTY)      != 0)
#define IS_MEDIA_FAILURE(p_vol)             ((EXT_FLAGS(p_vol) & EXT_FLAGS_EXFAT_MEDIA_FAILURE)     != 0)
#define IS_CLEAR_TO_ZERO(p_vol)             ((EXT_FLAGS(p_vol) & EXT_FLAGS_EXFAT_CLEAR_TO_ZERO)     != 0)

/* Current FAT (0 origin) */
#define CURRENT_FAT_EXFAT(p_vol)            (IS_ACTIVE_FAT_SECOND(p_vol) ? 1 : 0)

/*---------------------------------------------------------------------------
  Up-case Table Buffer
 ----------------------------------------------------------------------------*/
#define PF_UPCASE_FLAG_SET_MASK             (PF_UPCASE_DATA_SET)

#define PF_UPCASE_DATA_ENABLE               (0x40000000)
#define PF_UPCASE_BUF_USE                   (0x80000000)

#define IS_UPCASE_BUF_USE(p_vol)            (p_vol->upcase_buf.flag & PF_UPCASE_BUF_USE)
#define IS_UPCASE_DATA_SET(p_vol)           ((p_vol->upcase_buf.flag & PF_UPCASE_BUF_USE) && \
                                             (p_vol->upcase_buf.flag & PF_UPCASE_DATA_SET))

#define IS_UPCASE_DATA_ENABLE(p_vol)        ((p_vol->upcase_buf.flag & PF_UPCASE_BUF_USE) && \
                                             (p_vol->upcase_buf.flag & PF_UPCASE_DATA_ENABLE))

#define SET_UPCASE_BUF_USE(p_vol)           (p_vol->upcase_buf.flag |= PF_UPCASE_BUF_USE)
#define SET_UPCASE_DATA_ENABLE(p_vol)       (p_vol->upcase_buf.flag |= PF_UPCASE_DATA_ENABLE)

#define CLEAR_UPCASE_DATA_ENABLE(p_vol)     (p_vol->upcase_buf.flag &= ~PF_UPCASE_DATA_ENABLE)


/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFVOL_EXFAT_ClearDirEntryInfo - Clear the Directory Entry Information
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_EXFAT_ClearDirEntryInfo(PF_VOLUME *p_vol);

/*---------------------------------------------------------------------------
  PFVOL_EXFAT_ClearAllocateBmpInfo - Clear the Allocation Bitmap information
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_EXFAT_ClearAllocateBmpInfo(PF_VOLUME *p_vol);

/*---------------------------------------------------------------------------
  PFVOL_EXFAT_ClearUpCaseBuf - Clear the Up-case Table Buffer information
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_EXFAT_ClearUpCaseBuf(PF_VOLUME *p_vol);

#if PF_EXFAT_FORMAT_SUPPORT
/*---------------------------------------------------------------------------
  PFVOL_EXFAT_InitReservedArea - Initialize Reserved Area in Data Region.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_EXFAT_InitReservedArea(PF_VOLUME *p_vol);
#endif  /* #if PF_EXFAT_FORMAT_SUPPORT */

/*---------------------------------------------------------------------------
  PFVOL_EXFAT_setvol - set the volume label into the root directory.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_EXFAT_setvol(PF_VOLUME *p_vol, PF_STR *vname);

/*---------------------------------------------------------------------------
 PFVOL_EXFAT_setupcasebuf -
----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_EXFAT_setupcasebuf(PF_DRV_CHAR   drive,
                                   PF_U_LONG     mode,
                                   PF_UPCASE_BUF *p_upcasebuf);

/*---------------------------------------------------------------------------
 PFVOL_EXFAT_AllocationBitmapAccess -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFVOL_EXFAT_AllocationBitmapAccess(PF_VOLUME *p_vol,
                                             PF_BOOL   is_start);

#endif  /* PF_EXFAT_SUPPORT */

#endif  /* __PF_EXFAT_VOLUME_H__ */
