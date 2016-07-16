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
 [pf_exfat_entry.h] -
 ****************************************************************************/
#ifndef __PF_EXFAT_ENTRY_H__
#define __PF_EXFAT_ENTRY_H__

#include "pf_common.h"

#if PF_EXFAT_SUPPORT
/****************************************************************************
  Type Definitions
 ****************************************************************************/
/**
 * Directory entry information for exFAT.
 */
struct PF_DENT_EXFAT_TAG
{
    /*-----------------------------------------------------------------------*
      Common fields
     *-----------------------------------------------------------------------*/
    /* Long file name of the entry */
    PF_WCHAR    long_name[LONG_NAME_PATH_CHARS+1];

    /* Attribute flags of the entry */
    PF_U_LONG   attr;

    /* File size in bytes */
    PF_FSIZE_T  file_size;

    /* Creation milliseconds (actually hundredths of a sec.) */
    PF_U_CHAR   create_time_ms;

    /* Creation time */
    PF_U_SHORT  create_time;

    /* Creation date */
    PF_U_SHORT  create_date;

    /* Creation UTC Offset (exFAT Only) */
    PF_U_CHAR   create_utc_offset;

    /* Access time (exFAT Only) */
    PF_U_SHORT  access_time;

    /* Access date */
    PF_U_SHORT  access_date;

    /* Access UTC Offset (exFAT Only) */
    PF_U_CHAR   access_utc_offset;

    /* Modification milliseconds (actually hundredths of a sec.) (exFAT Only) */
    PF_U_CHAR   modify_time_ms;

    /* Modification time */
    PF_U_SHORT  modify_time;

    /* Modification date */
    PF_U_SHORT  modify_date;

    /* Modification UTC Offset (exFAT Only) */
    PF_U_CHAR   modify_utc_offset;

    /* Pointer to the volume */
    PF_VOLUME   *p_vol;

#if PF_CHECK_PATH_LENGTH
    /* Located path length */
    PF_U_LONG   path_len;
#endif /* PF_CHECK_PATH_LENGTH */

    /* Start cluster of file */
    PF_CLST     start_cluster;

    /* Sector where the entry is located on volume */
    PF_SECT     entry_sector;

    /* Position within cluster (entry number) */
    PF_U_SHORT  entry_offset;

    /* GeneralSecondaryFlags */
    PF_U_CHAR   general_flg;

    /* Start cluster of parent entry */
    PF_CLST     parent_start_cluster;

    /* GeneralSecondaryFlags of parent entry */
    PF_U_CHAR   parent_general_flg;

    /* Directory size of parent entry */
    PF_FSIZE_T  parent_dir_size;

    /*-----------------------------------------------------------------------*
      exFAT FileDirectoryEntry fields
     *-----------------------------------------------------------------------*/
    /* Length of file name */
    PF_U_CHAR   name_length;

    /* Secondary Count */
    PF_U_CHAR   secondary_cnt;

    /* Checksum field of directory entry set */
    PF_U_SHORT  check_sum;

    /* Name Hash */
    PF_U_SHORT  name_hash;

    /* Valid Data size in bytes */
    PF_FSIZE_T  valid_data_length;

    /* Sector Chain */
    PF_SECT     next_sector[2];

    /* Parent Path */
    PF_WCHAR    parent_path[LONG_NAME_PATH_CHARS + 1];
};

#define DENT_EXFAT(p_ent)   ((PF_DENT_EXFAT*)(p_ent))

/****************************************************************************
  Macro Definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  exFAT Directory Entry Offsets  -  DirectoryEntry Template
 ----------------------------------------------------------------------------*/
#define EX_DIR_ENT_TYPE                     (+ 0)   /*  1 bytes */
#define EX_DIR_FIRST_CLUSTER                (+20)   /*  4 bytes */
#define EX_DIR_DATA_LEN                     (+24)   /*  8 bytes */

/*---------------------------------------------------------------------------
  exFAT Directory Entry Offsets  - Primary DirectoryEntry Template
 ----------------------------------------------------------------------------*/
#define EX_DIR_SECONDARY_CNT                (+ 1)   /*  1 bytes */
#define EX_DIR_CHECKSUM                     (+ 2)   /*  2 bytes */
#define EX_DIR_PRIMARY_FLG                  (+ 4)   /*  2 bytes */

/*---------------------------------------------------------------------------
  exFAT Directory Entry Offsets  - Secondary DirectoryEntry Template
 ----------------------------------------------------------------------------*/
#define EX_DIR_SECONDARY_FLG                (+ 1)   /*  1 bytes */

/*---------------------------------------------------------------------------
  exFAT Directory Entry Offsets  - Allocation Bitmap
 ----------------------------------------------------------------------------*/
#define EX_DIR_AB_BITMAP_FLG                (+ 1)   /*  1 bytes */
#define EX_DIR_AB_RESERVED                  (+ 2)   /*  18 bytes */

/*---------------------------------------------------------------------------
  exFAT Directory Entry Offsets  - Up-case Table
 ----------------------------------------------------------------------------*/
#define EX_DIR_UT_RESERVED1                 (+ 1)   /*  3 bytes */
#define EX_DIR_UT_CHECKSUM                  (+ 4)   /*  4 bytes */
#define EX_DIR_UT_RESERVED2                 (+ 8)   /* 12 bytes */

/*---------------------------------------------------------------------------
  exFAT Directory Entry Offsets  - Volume Label
 ----------------------------------------------------------------------------*/
#define EX_DIR_VL_CHAR_CNT                  (+ 1)   /*  1 bytes */
#define EX_DIR_VL_VOL_LAVEL                 (+ 2)   /* 22 bytes */
#define EX_DIR_VL_RESERVED                  (+24)   /*  8 bytes */
#define EX_DIR_VL_VOL_LAVEL_SIZE            (VOLUME_LABEL_SIZE_EXFAT*2)

#define MAX_DIR_VL_CHAR_CNT                 VOLUME_LABEL_SIZE_EXFAT

/*---------------------------------------------------------------------------
  exFAT Directory Entry Offsets  - File
 ----------------------------------------------------------------------------*/
#define EX_DIR_F_ATTR                       (+ 4)   /*  2 bytes */
#define EX_DIR_F_RESERVED1                  (+ 6)   /*  2 bytes */
#define EX_DIR_F_CREATE_TIME                (+ 8)   /*  4 bytes */
#define EX_DIR_F_CREATE_DATE                (+10)   /*  4 bytes */
#define EX_DIR_F_MODIFY_TIME                (+12)   /*  4 bytes */
#define EX_DIR_F_MODIFY_DATE                (+14)   /*  4 bytes */
#define EX_DIR_F_ACCESS_TIME                (+16)   /*  4 bytes */
#define EX_DIR_F_ACCESS_DATE                (+18)   /*  4 bytes */
#define EX_DIR_F_CREATE_TIME_MS             (+20)   /*  1 bytes */
#define EX_DIR_F_MODIFY_TIME_MS             (+21)   /*  1 bytes */
#define EX_DIR_F_CREATE_UTC_OFFSET          (+22)   /*  1 bytes */
#define EX_DIR_F_MODIFY_UTC_OFFSET          (+23)   /*  1 bytes */
#define EX_DIR_F_ACCESS_UTC_OFFSET          (+24)   /*  1 bytes */
#define EX_DIR_F_RESERVED2                  (+25)   /*  7 bytes */

/*---------------------------------------------------------------------------
  exFAT Directory Entry Offsets  - Stream Extension
 ----------------------------------------------------------------------------*/
#define EX_DIR_SE_RESERVED1                 (+ 2)   /*  1 bytes */
#define EX_DIR_SE_NAME_LEN                  (+ 3)   /*  1 bytes */
#define EX_DIR_SE_NAME_HASH                 (+ 4)   /*  2 bytes */
#define EX_DIR_SE_RESERVED2                 (+ 6)   /*  2 bytes */
#define EX_DIR_SE_VALID_DATA_LEN            (+ 8)   /*  8 bytes */
#define EX_DIR_SE_RESERVED3                 (+16)   /*  4 bytes */

/*---------------------------------------------------------------------------
  exFAT Directory Entry Offsets  - File Name
 ----------------------------------------------------------------------------*/
#define EX_DIR_FN_FILE_NAME                 (+ 2)   /* 30 bytes */

#define EX_DIR_FN_FILE_NAME_SIZE            (30)    /* 30 bytes */

#define EX_DIR_FN_FILE_NAME_LEN             (EX_DIR_FN_FILE_NAME_SIZE/2)    /* FileName is Unicode */

#define MAX_FILE_NAME_ENTRY                 (17)

/*---------------------------------------------------------------------------
  exFAT Directory Entry Accessors - DirectoryEntry Template
 ----------------------------------------------------------------------------*/
#define GET_EX_DIR_ENT_TYPE(buf)            ((buf)[EX_DIR_ENT_TYPE])
#define GET_EX_DIR_FIRST_CLUSTER(buf)       GET_LE32_FIELD(buf, EX_DIR_FIRST_CLUSTER)
#define GET_EX_DIR_DATA_LEN(buf)            GET_LE64_FIELD(buf, EX_DIR_DATA_LEN)

#define SET_EX_DIR_ENT_TYPE(buf,type)       ((buf)[EX_DIR_ENT_TYPE] =(type))
#define SET_EX_DIR_FIRST_CLUSTER(buf,clst)  SET_LE32_FIELD((buf),   EX_DIR_FIRST_CLUSTER,   (clst))
#define SET_EX_DIR_DATA_LEN(buf,len)        SET_LE64_FIELD((buf),   EX_DIR_DATA_LEN,        (len))

/*---------------------------------------------------------------------------
  exFAT Directory Entry Accessors - Primary DirectoryEntry Template
 ----------------------------------------------------------------------------*/
#define GET_EX_DIR_SECONDARY_CNT(buf)       ((buf)[EX_DIR_SECONDARY_CNT])
#define GET_EX_DIR_CHECKSUM(buf)            GET_LE16_FIELD(buf, EX_DIR_CHECKSUM)
#define GET_EX_DIR_PRIMARY_FLG(buf)         GET_LE16_FIELD(buf, EX_DIR_PRIMARY_FLG)

#define SET_EX_DIR_SECONDARY_CNT(buf,cnt)   ((buf)[EX_DIR_SECONDARY_CNT]    =(cnt))
#define SET_EX_DIR_CHECKSUM(buf,cs)         SET_LE16_FIELD((buf),   EX_DIR_CHECKSUM,        (cs))
#define SET_EX_DIR_PRIMARY_FLG(buf,flg)     SET_LE16_FIELD((buf),   EX_DIR_PRIMARY_FLG,     (flg))

/*---------------------------------------------------------------------------
  exFAT Directory Entry Accessors - Secondary DirectoryEntry Template
 ----------------------------------------------------------------------------*/
#define GET_EX_DIR_SECONDARY_FLG(buf)       ((buf)[EX_DIR_SECONDARY_FLG])

#define SET_EX_DIR_SECONDARY_FLG(buf,flg)   ((buf)[EX_DIR_SECONDARY_FLG]    =(flg))

/*---------------------------------------------------------------------------
  exFAT Directory Entry Accessors - Allocation Bitmap
 ----------------------------------------------------------------------------*/
#define GET_EX_DIR_AB_BITMAP_FLG(buf)       ((buf)[EX_DIR_AB_BITMAP_FLG])

#define SET_EX_DIR_AB_BITMAP_FLG(buf,flg)   ((buf)[EX_DIR_AB_BITMAP_FLG]    =(flg))

/*---------------------------------------------------------------------------
  exFAT Directory Entry Accessors - Up-case Table
 ----------------------------------------------------------------------------*/
#define GET_EX_DIR_UT_CHECKSUM(buf)         GET_LE32_FIELD(buf, EX_DIR_UT_CHECKSUM)

#define SET_EX_DIR_UT_CHECKSUM(buf,cs)      SET_LE32_FIELD((buf),   EX_DIR_UT_CHECKSUM,     (cs))

/*---------------------------------------------------------------------------
  exFAT Directory Entry Accessors - Volume Label
 ----------------------------------------------------------------------------*/
#define GET_EX_DIR_VL_CHAR_CNT(buf)         ((buf)[EX_DIR_VL_CHAR_CNT])

#define SET_EX_DIR_VL_CHAR_CNT(buf,cnt)     ((buf)[EX_DIR_VL_CHAR_CNT]  =(cnt))

/*---------------------------------------------------------------------------
  exFAT Directory Entry Accessors - File
 ----------------------------------------------------------------------------*/
#define GET_EX_DIR_F_ATTR(buf)              GET_LE16_FIELD(buf, EX_DIR_F_ATTR)
#define GET_EX_DIR_F_CREATE_DATE(buf)       GET_LE16_FIELD(buf, EX_DIR_F_CREATE_DATE)
#define GET_EX_DIR_F_MODIFY_DATE(buf)       GET_LE16_FIELD(buf, EX_DIR_F_MODIFY_DATE)
#define GET_EX_DIR_F_ACCESS_DATE(buf)       GET_LE16_FIELD(buf, EX_DIR_F_ACCESS_DATE)
#define GET_EX_DIR_F_CREATE_TIME(buf)       GET_LE16_FIELD(buf, EX_DIR_F_CREATE_TIME)
#define GET_EX_DIR_F_MODIFY_TIME(buf)       GET_LE16_FIELD(buf, EX_DIR_F_MODIFY_TIME)
#define GET_EX_DIR_F_ACCESS_TIME(buf)       GET_LE16_FIELD(buf, EX_DIR_F_ACCESS_TIME)
#define GET_EX_DIR_F_CREATE_TIME_MS(buf)    ((buf)[EX_DIR_F_CREATE_TIME_MS])
#define GET_EX_DIR_F_MODIFY_TIME_MS(buf)    ((buf)[EX_DIR_F_MODIFY_TIME_MS])
#define GET_EX_DIR_F_CREATE_UTC(buf)        ((buf)[EX_DIR_F_CREATE_UTC_OFFSET])
#define GET_EX_DIR_F_MODIFY_UTC(buf)        ((buf)[EX_DIR_F_MODIFY_UTC_OFFSET])
#define GET_EX_DIR_F_ACCESS_UTC(buf)        ((buf)[EX_DIR_F_ACCESS_UTC_OFFSET])

#define SET_EX_DIR_F_ATTR(buf,attr)         SET_LE16_FIELD((buf),   EX_DIR_F_ATTR,          (PF_U_SHORT)(attr))
#define SET_EX_DIR_F_CREATE_DATE(buf,time)  SET_LE16_FIELD((buf),   EX_DIR_F_CREATE_DATE,   (PF_U_SHORT)(time))
#define SET_EX_DIR_F_MODIFY_DATE(buf,time)  SET_LE16_FIELD((buf),   EX_DIR_F_MODIFY_DATE,   (PF_U_SHORT)(time))
#define SET_EX_DIR_F_ACCESS_DATE(buf,time)  SET_LE16_FIELD((buf),   EX_DIR_F_ACCESS_DATE,   (PF_U_SHORT)(time))
#define SET_EX_DIR_F_CREATE_TIME(buf,time)  SET_LE16_FIELD((buf),   EX_DIR_F_CREATE_TIME,   (PF_U_SHORT)(time))
#define SET_EX_DIR_F_MODIFY_TIME(buf,time)  SET_LE16_FIELD((buf),   EX_DIR_F_MODIFY_TIME,   (PF_U_SHORT)(time))
#define SET_EX_DIR_F_ACCESS_TIME(buf,time)  SET_LE16_FIELD((buf),   EX_DIR_F_ACCESS_TIME,   (PF_U_SHORT)(time))
#define SET_EX_DIR_F_CREATE_TIME_MS(buf,ms) ((buf)[EX_DIR_F_CREATE_TIME_MS] =(ms))
#define SET_EX_DIR_F_MODIFY_TIME_MS(buf,ms) ((buf)[EX_DIR_F_MODIFY_TIME_MS] =(ms))
#define SET_EX_DIR_F_CREATE_UTC(buf,utc)    ((buf)[EX_DIR_F_CREATE_UTC_OFFSET] =(utc))
#define SET_EX_DIR_F_MODIFY_UTC(buf,utc)    ((buf)[EX_DIR_F_MODIFY_UTC_OFFSET] =(utc))
#define SET_EX_DIR_F_ACCESS_UTC(buf,utc)    ((buf)[EX_DIR_F_ACCESS_UTC_OFFSET] =(utc))

/*---------------------------------------------------------------------------
  exFAT Directory Entry Accessors - Stream Extension
 ----------------------------------------------------------------------------*/
#define GET_EX_DIR_SE_NAME_LEN(buf)             ((buf)[EX_DIR_SE_NAME_LEN])
#define GET_EX_DIR_SE_NAME_HASH(buf)            GET_LE16_FIELD(buf, EX_DIR_SE_NAME_HASH)
#define GET_EX_DIR_SE_VALID_DATA_LEN(buf)       GET_LE64_FIELD(buf, EX_DIR_SE_VALID_DATA_LEN)

#define SET_EX_DIR_SE_NAME_LEN(buf,len)         ((buf)[EX_DIR_SE_NAME_LEN]  =(len))
#define SET_EX_DIR_SE_NAME_HASH(buf,hash)       SET_LE16_FIELD((buf),   EX_DIR_SE_NAME_HASH,        (PF_U_SHORT)(hash))
#define SET_EX_DIR_SE_VALID_DATA_LEN(buf,len)   SET_LE64_FIELD((buf),   EX_DIR_SE_VALID_DATA_LEN,   (len))

/*---------------------------------------------------------------------------
  exFAT Entry Type
 ----------------------------------------------------------------------------*/
#define EX_ENT_TYPE_ALLCATINON_BMP          (0x81uL)
#define EX_ENT_TYPE_UPCASE_TBL              (0x82uL)
#define EX_ENT_TYPE_VOL_LABEL               (0x83uL)
#define EX_ENT_TYPE_FILE                    (0x85uL)
#define EX_ENT_TYPE_STREAM_EXTENSION        (0xC0uL)
#define EX_ENT_TYPE_FILE_NAME               (0xC1uL)
#define EX_ENT_TYPE_FREE_END                (0x00uL)

/* Entry Type Used BIT. */
#define EX_ENT_USED                         (0x80uL)

/* Is the entry free? */
#define IS_EX_DIR_ENTRY_FREE(buf)           (((GET_EX_DIR_ENT_TYPE(buf)&EX_ENT_USED)==0)&& \
                                             (GET_EX_DIR_ENT_TYPE(buf)&(~EX_ENT_USED)))

/* The volume lavel entry is free. */
#define EX_DIR_FB_FREE_VOL_LABEL            ((PF_U_CHAR)EX_ENT_TYPE_VOL_LABEL&(~EX_ENT_USED))

/* The file entry is free. */
#define EX_DIR_FB_FREE_FILE                 ((PF_U_CHAR)EX_ENT_TYPE_FILE&(~EX_ENT_USED))

/* The stream extension entry is free. */
#define EX_DIR_FB_FREE_STREAM_EXTENSION     ((PF_U_CHAR)EX_ENT_TYPE_STREAM_EXTENSION&(~EX_ENT_USED))

/* The file name entry is free. */
#define EX_DIR_FB_FREE_FILE_NAME            ((PF_U_CHAR)EX_ENT_TYPE_FILE_NAME&(~EX_ENT_USED))

/* Is the entry is free and no allocated entries after this one? */
#define IS_EX_DIR_ENTRY_FREE_END(buf)       (GET_EX_DIR_ENT_TYPE(buf)==EX_ENT_TYPE_FREE_END)

/* GeneralFlags - void */
#define EX_ENT_FLG_VOID                     (0xFFuL)

/* GeneralFlags - AllocationPossible */
#define EX_ENT_FLG_ALLCATE_POSSIBLE         (0x01uL)

/* GeneralFlags - NoFatChain */
#define EX_ENT_FLG_NO_FAT_CHAIN             (0x02uL)

/*---------------------------------------------------------------------------
  FAT Chain (GeneralFlags)
 ----------------------------------------------------------------------------*/
/* GeneralFlags the allocationfs cluster chain */
#define ENT_CLUSTER_CHAIN_VALID             ((PF_U_CHAR)EX_ENT_FLG_ALLCATE_POSSIBLE)

/* GeneralFlags the allocationfs cluster not chain */
#define ENT_CLUSTER_CHAIN_INVALID           ((PF_U_CHAR)(EX_ENT_FLG_ALLCATE_POSSIBLE|EX_ENT_FLG_NO_FAT_CHAIN))

/* Is the allocationfs cluster chain invalid? */
#define IS_CLUSTER_CHAIN_INVALID(flg)       ((flg&(ENT_CLUSTER_CHAIN_INVALID))==ENT_CLUSTER_CHAIN_INVALID)

#define IS_FAT_VALID(p_ent)                 (IS_EXFAT((p_ent)->p_vol) ? \
                                             (!IS_CLUSTER_CHAIN_INVALID((p_ent)->general_flg)) : \
                                             1)

/*---------------------------------------------------------------------------
  exFAT Root Directory
 ----------------------------------------------------------------------------*/
#define EX_ENT_VOLUME_LABEL_OFFSET          (0)
#define EX_ENT_ALLOCATE_BMP_OFFSET          (32)
#define EX_ENT_UP_CASE_OFFSET               (64)

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFENT_EXFAT_GetParentEnt - Get parent entry by specified parent_path.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_EXFAT_GetParentEnt(PF_DENT_EXFAT          *p_entry_parent, /* out */
                                   PF_CONST PF_DENT_EXFAT *p_entry);       /* in */

/*---------------------------------------------------------------------------
  PFENT_EXFAT_IsDirTreeUnder - Is there 'p_entry' with 'p_directory' ?
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_EXFAT_IsDirTreeUnder(PF_CONST PF_DENT_EXFAT *p_directory, /* in  */
                                     PF_CONST PF_DENT_EXFAT *p_entry,     /* in  */
                                     PF_BOOL                *p_is_found); /* out */

/*---------------------------------------------------------------------------
  PFENT_EXFAT_GetRootDirEnt - Loads the root directory of the volume.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_EXFAT_GetRootDirEnt(PF_VOLUME *p_vol, PF_DENT_EXFAT *p_ent);

/*---------------------------------------------------------------------------
  PFENT_EXFAT_compareEntryName - Compare the entry and specified pattern
 ----------------------------------------------------------------------------*/
PF_S_LONG  PFENT_EXFAT_compareEntryName(PF_CONST PF_DENT_EXFAT *p_ent,
                                        PF_CONST PF_STR        *p_up_uni_pattern);

/*---------------------------------------------------------------------------
 PFENT_EXFAT_RemoveEntry - Remove entry.
 ----------------------------------------------------------------------------*/
PF_ERROR PFENT_EXFAT_RemoveEntry(PF_DENT_EXFAT *p_ent);

/*---------------------------------------------------------------------------
  PFENT_EXFAT_InitENT
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_EXFAT_InitENT(PF_DENT_EXFAT          *p_ent,        /* out */
                              PF_CONST PF_STR        *p_filename,   /* in */
                              PF_U_CHAR              attr,          /* in */
                              PF_BOOL                is_set_time,   /* in */
                              PF_CONST PF_DENT_EXFAT *p_parent_ent, /* in */
                              PF_VOLUME              *p_vol);       /* in */

/*---------------------------------------------------------------------------
  PFENT_EXFAT_IsNameSame - Compare whether p_ent1's name and p_ent2's
                           name are same. (for exFAT)
 ----------------------------------------------------------------------------*/
PF_BOOL  PFENT_EXFAT_IsNameSame(PF_DENT_EXFAT *p_ent,
                                PF_STR        *p_name);

/*---------------------------------------------------------------------------
 PFENT_EXFAT_LoadVolumeLabelFromBuf - Load Volume Label Directory Entry field
                                      into a 'PF_DENT_EXFAT' structure.
 ----------------------------------------------------------------------------*/
void  PFENT_EXFAT_LoadVolumeLabelFromBuf(PF_DENT_EXFAT      *p_ent,
                                         PF_CONST PF_U_CHAR *buf);

/*---------------------------------------------------------------------------
 PFENT_EXFAT_LoadFileFromBuf - Load File Directory Entry field
                               into a 'PF_DENT_EXFAT' structure.
 ----------------------------------------------------------------------------*/
void  PFENT_EXFAT_LoadFileFromBuf(PF_DENT_EXFAT      *p_ent, 
                                  PF_CONST PF_U_CHAR *buf);

/*---------------------------------------------------------------------------
 PFENT_EXFAT_LoadStreamExtensionFromBuf - Load Stream Extension Directory
                                          Entry field into a 'PF_DENT_EXFAT'
                                          structure.
 ----------------------------------------------------------------------------*/
void  PFENT_EXFAT_LoadStreamExtensionFromBuf(PF_DENT_EXFAT      *p_ent,
                                             PF_CONST PF_U_CHAR *buf);

/*---------------------------------------------------------------------------
 PFENT_EXFAT_LoadFileNameFromBuf - Load File Name Directory Entry field
                                   into a 'PF_DENT_EXFAT' structure.
 ----------------------------------------------------------------------------*/
void  PFENT_EXFAT_LoadFileNameFromBuf(PF_DENT_EXFAT      *p_ent,
                                      PF_CONST PF_U_CHAR *buf);

/*---------------------------------------------------------------------------
 PFENT_EXFAT_CalcEntryChecksum - Compute checksum of File Directory Entry,
 ----------------------------------------------------------------------------*/
void  PFENT_EXFAT_CalcEntryChecksum(PF_U_SHORT         *p_checksum,
                                    PF_BOOL            is_primary_dir,
                                    PF_CONST PF_U_CHAR *buf);

/*---------------------------------------------------------------------------
 PFENT_EXFAT_UpdateFileEntry -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_EXFAT_UpdateFileEntry(PF_DENT_EXFAT *p_ent, PF_BOOL is_set_ARCH);

/*---------------------------------------------------------------------------
 PFENT_EXFAT_UpdateVolumeLabelEntry -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_EXFAT_UpdateVolumeLabelEntry(PF_DENT_EXFAT *p_ent);

/*---------------------------------------------------------------------------
  PFENT_EXFAT_CalcNameHash -
 ----------------------------------------------------------------------------*/
PF_U_SHORT  PFENT_EXFAT_CalcNameHash(PF_WCHAR *p_fileName, PF_U_LONG name_len);

/*---------------------------------------------------------------------------
 PFENT_EXFAT_UpdateEntry -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_EXFAT_UpdateEntry(PF_DENT_EXFAT *p_ent,
                                  PF_BOOL       is_set_ARCH);

/*---------------------------------------------------------------------------
  PFENT_EXFAT_findEntryType - Search directory entry for the entry
                              that matches entry type
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_EXFAT_findEntryType(PF_FFD    *p_ffd,
                                    PF_U_CHAR *p_buf,
                                    PF_U_LONG index_search_from,
                                    PF_U_CHAR entry_type,
                                    PF_U_CHAR flags,
                                    PF_U_LONG *p_lpos,
                                    PF_U_LONG *p_ppos);

/*---------------------------------------------------------------------------
  PFENT_EXFAT_CalcUtcOffset -
 ----------------------------------------------------------------------------*/
PF_U_CHAR  PFENT_EXFAT_CalcUtcOffset(PF_U_SHORT valid,
                                     PF_S_SHORT offset);

#if PF_EXFAT_FORMAT_SUPPORT
/*---------------------------------------------------------------------------
 PFENT_EXFAT_InitRootDir - Initialize root directory
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_EXFAT_InitRootDir(PF_VOLUME *p_vol);
#endif  /* PF_EXFAT_FORMAT_SUPPORT */

#endif  /* PF_EXFAT_SUPPORT */
#endif  /* __PF_EXFAT_ENTRY_H__ */
