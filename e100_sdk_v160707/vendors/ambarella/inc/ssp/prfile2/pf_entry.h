/*
     Copyright (C) 2004 By eSOL Co.,Ltd. Tokyo, Japan
 
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
 [pf_entry.h] - 
 ****************************************************************************/
#ifndef __PF_ENTRY_H__
#define __PF_ENTRY_H__

#include "pf_common.h"

/****************************************************************************
  Type Definitions
 ****************************************************************************/
/**
 * Directory entry information for files/directories. Holds the information
 * associated with an entry in a directory structure on the disk (name,
 * attributes, creation/access/write dates/times, start cluster).
 */
/**
 * Directory entry information for FAT12/16/32.
 */
struct PF_DENT_FAT_TAG
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

#if PF_EXFAT_SUPPORT
    /* Creation UTC Offset (exFAT Only) */
    PF_U_CHAR   create_utc_offset;

    /* Access time (exFAT Only) */
    PF_U_SHORT  access_time;
#endif /* PF_EXFAT_SUPPORT */

    /* Access date */
    PF_U_SHORT  access_date;

#if PF_EXFAT_SUPPORT
    /* Access UTC Offset (exFAT Only) */
    PF_U_CHAR   access_utc_offset;

    /* Modification milliseconds (actually hundredths of a sec.) (exFAT Only) */
    PF_U_CHAR   modify_time_ms;
#endif /* PF_EXFAT_SUPPORT */

    /* Modification time */
    PF_U_SHORT  modify_time;

    /* Modification date */
    PF_U_SHORT  modify_date;

#if PF_EXFAT_SUPPORT
    /* Modification UTC Offset (exFAT Only) */
    PF_U_CHAR   modify_utc_offset;
#endif /* PF_EXFAT_SUPPORT */

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

#if PF_EXFAT_SUPPORT
    /* GeneralSecondaryFlags */
    PF_U_CHAR   general_flg;
#endif /* PF_EXFAT_SUPPORT */

    /* Start cluster of parent entry */
    PF_CLST     parent_start_cluster;

#if PF_EXFAT_SUPPORT
    /* GeneralSecondaryFlags of parent entry */
    PF_U_CHAR   parent_general_flg;

    /* Directory size of parent entry */
    PF_FSIZE_T  parent_dir_size;
#endif /* PF_EXFAT_SUPPORT */

    /*-----------------------------------------------------------------------*
      FAT DirectoryEntry fields
     *-----------------------------------------------------------------------*/
    /* Number of LFNs; If it is zero, string in 'long_name' field is not valid */
    PF_U_CHAR   num_entry_LFNs;

    /* Ordinal field of LFN entry, without LAST_LONG_ENTRY flag */
    PF_U_CHAR   ordinal;

    /* Checksum field of LFN entry */
    PF_U_CHAR   check_sum;

    /* Short file name of the entry */
    PF_CHAR     short_name[SHORT_NAME_BUF_SIZE];

    /* Small letter flag */
    PF_U_CHAR   small_letter_flag;

};

#define DENT(p_ent)         ((PF_DENT*)(p_ent))
#define DENT_FAT(p_ent)     ((PF_DENT_FAT*)(p_ent))


/*---------------------------------------------------------------------------
  SFN number with tail
 ----------------------------------------------------------------------------*/
struct PF_DIR_TAIL
{
    PF_U_LONG   tracker_size;
    PF_TAIL_BUF tracker_buff[TRACK_BUFF_SIZE];
    PF_U_LONG*  tracker_bits;
};


/****************************************************************************
  Macro Definitions
 ****************************************************************************/
/**
 * Maximum number of entries in a directory (the FAT specification requires
 * FAT implementations not to create or accept directories with more than 65536
 * entries).
 */
#define MAX_ENTRIES_FAT                     (0x10000uL)

/**
 * Maximum number of entries in a directory (the exFAT specification requires
 * FAT implementations not to create or accept directories with more than
 * 8,388,608 entries).
 */
#define MAX_ENTRIES_EXFAT                   (0x800000uL)

#if PF_EXFAT_SUPPORT
#define MAX_ENTRIES(p_vol)                  (IS_EXFAT(p_vol) ? MAX_ENTRIES_EXFAT : MAX_ENTRIES_FAT)
#else   /* PF_EXFAT_SUPPORT */
#define MAX_ENTRIES(p_vol)                  MAX_ENTRIES_FAT
#endif  /* PF_EXFAT_SUPPORT */


/*---------------------------------------------------------------------------
  SFN (8.3) Directory Entry Offsets
 ----------------------------------------------------------------------------*/
#define DIR_NAME                            (+ 0)   /*  8 bytes */
#define DIR_EXT                             (+ 8)   /*  3 bytes */
#define DIR_ATTR                            (+11)   /*  1 byte  */
#define DIR_NT_RESERVED                     (+12)   /*  1 byte  */
#define DIR_CREATE_TIME_MS                  (+13)   /*  1 byte  */
#define DIR_CREATE_TIME                     (+14)   /*  2 bytes */
#define DIR_CREATE_DATE                     (+16)   /*  2 bytes */
#define DIR_ACCESS_DATE                     (+18)   /*  2 bytes */
#define DIR_START_CLUSTER_H                 (+20)   /*  2 bytes */
#define DIR_MODIFY_TIME                     (+22)   /*  2 bytes */
#define DIR_MODIFY_DATE                     (+24)   /*  2 bytes */
#define DIR_START_CLUSTER_L                 (+26)   /*  2 bytes */
#define DIR_FILE_SIZE                       (+28)   /*  4 bytes */


#define DIR_NAME_SIZE                       (8)     /*  8 bytes */
#define DIR_EXT_SIZE                        (3)     /*  3 bytes */
#define DIR_SHORT_NAME_SIZE                 (11)    /* 11 bytes */
#define DIR_EXT_SIZE                        (3)     /*  3 bytes */
#define DIR_ATTR_SIZE                       (1)     /*  1 byte  */
#define DIR_NT_RESERVED_SIZE                (1)     /*  1 byte  */
#define DIR_CREATE_TIME_MS_SIZE             (2)     /*  1 byte  */
#define DIR_CREATE_TIME_SIZE                (2)     /*  2 bytes */
#define DIR_CREATE_DATE_SIZE                (2)     /*  2 bytes */
#define DIR_ACCESS_DATE_SIZE                (2)     /*  2 bytes */
#define DIR_START_CLUSTER_H_SIZE            (2)     /*  2 bytes */
#define DIR_MODIFY_TIME_SIZE                (2)     /*  2 bytes */
#define DIR_MODIFY_DATE_SIZE                (2)     /*  2 bytes */
#define DIR_START_CLUSTER_L_SIZE            (2)     /*  2 bytes */
#define DIR_FILE_SIZE_SIZE                  (4)     /*  4 bytes */


/*---------------------------------------------------------------------------
  SFN (8.3) Extra Directory Entry 
 ----------------------------------------------------------------------------*/
#define DIR_EXTRA_ENTRY_NUM                 (2)

#define DIR_DOT_ENTRY_OFFSET                (0x00ul)
#define DIR_DOTDOT_ENTRY_OFFSET             (0x20ul)

#define DIR_DOT_ENTRY_INDEX                 (0x00ul)
#define DIR_DOTDOT_ENTRY_INDEX              (0x01ul)

#define DIR_DOT_ENTRY_IMAGE                 ".          "
#define DIR_DOTDOT_ENTRY_IMAGE              "..         "


/*---------------------------------------------------------------------------
  SFN (8.3) Directory Entry Accessors
 ----------------------------------------------------------------------------*/
#define GET_LE16_FIELD(buf,offset)          LE16_TO_U16(*(PF_U_SHORT*)&(buf)[offset])
#define GET_LE32_FIELD(buf,offset)          LE32_TO_U32(*(PF_U_LONG*)&(buf)[offset])
#define GET_LE64_FIELD(buf,offset)          LE64_TO_U64(*(PF_UINT64*)&(buf)[offset])

#define GET_DIR_ATTR(buf)                   ((buf)[DIR_ATTR])
#define GET_DIR_NT_RESERVED(buf)            ((buf)[DIR_NT_RESERVED])
#define GET_DIR_CREATE_TIME_MS(buf)         ((buf)[DIR_CREATE_TIME_MS])
#define GET_DIR_CREATE_TIME(buf)            GET_LE16_FIELD(buf, DIR_CREATE_TIME)
#define GET_DIR_CREATE_DATE(buf)            GET_LE16_FIELD(buf, DIR_CREATE_DATE)
#define GET_DIR_ACCESS_DATE(buf)            GET_LE16_FIELD(buf, DIR_ACCESS_DATE)
#define GET_DIR_MODIFY_TIME(buf)            GET_LE16_FIELD(buf, DIR_MODIFY_TIME)
#define GET_DIR_MODIFY_DATE(buf)            GET_LE16_FIELD(buf, DIR_MODIFY_DATE)
#define GET_DIR_START_CLUSTER(buf)          ((PF_U_LONG)(((PF_U_LONG)GET_LE16_FIELD(buf, DIR_START_CLUSTER_H) << 16) | \
                                                          (PF_U_LONG)GET_LE16_FIELD(buf, DIR_START_CLUSTER_L)))
#define GET_DIR_FILE_SIZE(buf)              GET_LE32_FIELD(buf, DIR_FILE_SIZE)


#define SET_LE16_FIELD(buf,offset,val)      (*(PF_U_SHORT*)&(buf)[offset] = U16_TO_LE16(val))
#define SET_LE32_FIELD(buf,offset,val)      (*(PF_U_LONG*)&(buf)[offset] = U32_TO_LE32(val))
#define SET_LE64_FIELD(buf,offset,val)      (*(PF_UINT64*)&(buf)[offset] = U64_TO_LE64(val))

#define SET_DIR_ATTR(buf,attr)              ((buf)[DIR_ATTR]          =((PF_U_CHAR)((attr)&0xFF)))
#define SET_DIR_NT_RESERVED(buf,val)        ((buf)[DIR_NT_RESERVED]   =(val) )
#define SET_DIR_CREATE_TIME_MS(buf,ms)      ((buf)[DIR_CREATE_TIME_MS]=(ms)  )
#define SET_DIR_CREATE_TIME(buf,time)       SET_LE16_FIELD((buf), DIR_CREATE_TIME,     (time))
#define SET_DIR_CREATE_DATE(buf,date)       SET_LE16_FIELD((buf), DIR_CREATE_DATE,     (date))
#define SET_DIR_ACCESS_DATE(buf,date)       SET_LE16_FIELD((buf), DIR_ACCESS_DATE,     (date))
#define SET_DIR_MODIFY_TIME(buf,time)       SET_LE16_FIELD((buf), DIR_MODIFY_TIME,     (time))
#define SET_DIR_MODIFY_DATE(buf,date)       SET_LE16_FIELD((buf), DIR_MODIFY_DATE,     (date))
#define SET_DIR_START_CLUSTER_H(buf,clst)   SET_LE16_FIELD((buf), DIR_START_CLUSTER_H, (PF_U_SHORT)((clst)>>16))
#define SET_DIR_START_CLUSTER_L(buf,clst)   SET_LE16_FIELD((buf), DIR_START_CLUSTER_L, (PF_U_SHORT) (clst))
#define SET_DIR_FILE_SIZE(buf,size)         SET_LE32_FIELD((buf), DIR_FILE_SIZE,       (size))


/*---------------------------------------------------------------------------
  LFN Directory Entry Offsets
 ----------------------------------------------------------------------------*/
#define DIR_ORD                             (+ 0)   /*  1 byte  */
#define DIR_LNAME1                          (+ 1)   /* 10 bytes */
/*#define DIR_ATTR                          (+11)       1 byte  */
#define DIR_FLAGS                           (+12)   /*  1 byte  */
#define DIR_SUM                             (+13)   /*  1 byte  */
#define DIR_LNAME2                          (+14)   /* 12 bytes */
/*#define DIR_START_CLUSTER_L               (+26)       2 bytes */
#define DIR_LNAME3                          (+28)   /*  4 bytes */


#define DIR_ORD_SIZE                        (1)     /*  1 byte  */
#define DIR_LNAME1_SIZE                     (10)    /* 10 bytes */
/*#define DIR_ATTR_SIZE                     (1)         1 byte  */
#define DIR_FLAGS_SIZE                      (1)     /*  1 byte  */
#define DIR_SUM_SIZE                        (1)     /*  1 byte  */
#define DIR_LNAME2_SIZE                     (12)    /* 12 bytes */
/*#define DIR_START_CLUSTER_L_SIZE          (2)         2 bytes */
#define DIR_LNAME3_SIZE                     (4)     /*  4 bytes */


#define DIR_LNAME1_SIZE                     (10)    /* 10 bytes */
#define DIR_LNAME2_SIZE                     (12)    /* 12 bytes */
#define DIR_LNAME3_SIZE                     (4)     /*  4 bytes */

#define DIR_LNAME1_OFFSET                   (0)
#define DIR_LNAME2_OFFSET                   DIR_LNAME1_SIZE
#define DIR_LNAME3_OFFSET                   (DIR_LNAME1_SIZE + \
                                             DIR_LNAME2_SIZE)

#define DIR_LNAME_SIZE                      (DIR_LNAME1_SIZE + \
                                             DIR_LNAME2_SIZE + \
                                             DIR_LNAME3_SIZE)

#define LAST_LONG_ENTRY                     (0x40ul)
#define MIN_LONG_ENTRY                      (1)
#define MAX_LONG_ENTRY                      (20)

/*---------------------------------------------------------------------------
  LFN Directory Entry Accessors
 ----------------------------------------------------------------------------*/
#define GET_DIR_ORD(buf)                    ((buf)[DIR_ORD])
#define GET_DIR_FLAGS(buf)                  ((buf)[DIR_FLAGS])
#define GET_DIR_SUM(buf)                    ((buf)[DIR_SUM])
#define GET_DIR_START_CLUSTER_L(buf)        ((PF_U_SHORT)GET_LE16_FIELD(buf, DIR_START_CLUSTER_L))

#define SET_DIR_ORD(buf,ord)                ((buf)[DIR_ORD]  =(ord))
#define SET_DIR_FLAGS(buf,flags)            ((buf)[DIR_FLAGS]=(flags))
#define SET_DIR_SUM(buf,sum)                ((buf)[DIR_SUM]  =(sum))


/*---------------------------------------------------------------------------
  First Byte of Directory Entry
 ----------------------------------------------------------------------------*/
/* The entry is free. */
#define DIR_FB_FREE                         ((PF_U_CHAR)0xE5ul)

/* The entry is free, and no allocated entries after this one. */
#define DIR_FB_FREE_END                     ((PF_U_CHAR)0x00)

/* The entry is in use, and the actual character for this byte is
 * DIR_FB_FREE (0xE5).
 */
#define DIR_FB_KANJI_ESC                    ((PF_U_CHAR)0x05ul)

/* Is the entry free? */
#define IS_DIR_ENTRY_FREE(buf)              ((buf)[0]==DIR_FB_FREE)

/* Is the entry is free and no allocated entries after this one? */
#define IS_DIR_ENTRY_FREE_END(buf)          ((buf)[0]==DIR_FB_FREE_END)

/* Is the entry is used, and is its first byte DIR_FB_KANJI_ESC? */
#define IS_DIR_ENTRY_KANJI_ESC(buf)         ((buf)[0]==DIR_FB_KANJI_ESC)

#if PF_EXFAT_SUPPORT
/* Is the entry free? */
#define IS_ENTRY_FREE(p_vol, buf)           (IS_EXFAT(p_vol) ? \
                                             IS_EX_DIR_ENTRY_FREE(buf) : IS_DIR_ENTRY_FREE(buf))
                                           
/* Is the entry is free and no allocated entries after this one? */
#define IS_ENTRY_FREE_END(p_vol, buf)       (IS_EXFAT(p_vol) ? \
                                             IS_EX_DIR_ENTRY_FREE_END(buf) : IS_DIR_ENTRY_FREE_END(buf))
#else /* !PF_EXFAT_SUPPORT */              
/* Is the entry free? */                   
#define IS_ENTRY_FREE(p_vol, buf)           IS_DIR_ENTRY_FREE(buf)
                                           
/* Is the entry is free and no allocated entries after this one? */
#define IS_ENTRY_FREE_END(p_vol, buf)       IS_DIR_ENTRY_FREE_END(buf)
#endif /* !PF_EXFAT_SUPPORT */

/* Are the directory entries same? */
#define IS_DENT_EQ(p_ent1, p_ent2)                                      \
                (((p_ent1)->p_vol           == (p_ent2)->p_vol)         \
                 && ((p_ent1)->entry_sector == (p_ent2)->entry_sector)  \
                 && ((p_ent1)->entry_offset == (p_ent2)->entry_offset))

/*---------------------------------------------------------------------------
  NT reserved flag (small letter  flag)
 ----------------------------------------------------------------------------*/
/* The all character of the base file name are the large letter. */
#define LFN_BASENAME_FLAG                   ((PF_U_CHAR)0x08ul)

/* The all character of the extension are the large letter. */
#define LFN_EXTENSION_FLAG                  ((PF_U_CHAR)0x10ul)

/* The all character of the base file name or extension are the large letter. */
#define LFN_FILENAME_FLAG                   (LFN_BASENAME_FLAG | LFN_EXTENSION_FLAG)

/* Is the all character of the base file name are the large letter. */
#define IS_LFN_BASENAME_FLAG(buf)           ((buf)[DIR_NT_RESERVED] & LFN_BASENAME_FLAG)

/* Is the all character of the extension are the large letter. */
#define IS_LFN_EXTENSION_FLAG(buf)          ((buf)[DIR_NT_RESERVED] & LFN_EXTENSION_FLAG)

/* Is the all character of the base file name or extension are the large letter. */
#define IS_LFN_FILENAME_FLAG(buf)           (((buf)[DIR_NT_RESERVED] & LFN_FILENAME_FLAG) != 0)

/*---------------------------------------------------------------------------
  SFN number with tail
 ----------------------------------------------------------------------------*/
/* Tracker size of one unit */
#define TAIL_TRACK_SIZE                     (32)
/* Tracker mask of one unit */
#define TAIL_TRACK_MASK                     (0xFFFFFFFFul)


/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFENT_setDotEntry - place dir "."
 ----------------------------------------------------------------------------*/
void  PFENT_SetDotEntry(PF_U_CHAR *entry);

/*---------------------------------------------------------------------------
  PFENT_SetDotDotEntry - place dir ".."
 ----------------------------------------------------------------------------*/
void  PFENT_SetDotDotEntry(PF_U_CHAR *entry);

/*---------------------------------------------------------------------------
  PFENT_CalcCheckSum - Calculate Checksum of Short Filename.
 ----------------------------------------------------------------------------*/
PF_U_CHAR  PFENT_CalcCheckSum(PF_CONST PF_DENT_FAT *p_ent);

/*---------------------------------------------------------------------------
 PFENT_LoadShortNameFromBuf - Load the short name field from the buffer.
 ----------------------------------------------------------------------------*/
void  PFENT_LoadShortNameFromBuf(PF_DENT_FAT *p_ent, PF_CONST PF_U_CHAR *buf);

/*---------------------------------------------------------------------------
 PFENT_loadEntryNumericFieldsFromBuf - Load all numeric fields from a buffer
                                       into a 'PF_DENT_FAT' structure.
 ----------------------------------------------------------------------------*/
void PFENT_loadEntryNumericFieldsFromBuf(PF_DENT_FAT *p_ent, PF_CONST PF_U_CHAR *buf);

/*---------------------------------------------------------------------------
 PFENT_StoreEntryNumericFieldsToBuf - Store all fields (including the short
                                      name) from a 'PF_DENT_FAT' structure
                                      into a buffer.
 ----------------------------------------------------------------------------*/
void  PFENT_StoreEntryNumericFieldsToBuf(PF_U_CHAR *buf, PF_CONST PF_DENT_FAT *p_ent);

/*---------------------------------------------------------------------------
 PFENT_LoadLFNEntryFieldsFromBuf - Load all fields from a LFN entry buffer.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_LoadLFNEntryFieldsFromBuf(PF_DENT_FAT        *p_ent, 
                                          PF_CONST PF_U_CHAR *buf,
                                          PF_BOOL            is_reverse);

/*---------------------------------------------------------------------------
 PFENT_storeLFNEntryFieldsToBuf - Store all fields of LFN entry into a
                                  buffer.
 ----------------------------------------------------------------------------*/
void  PFENT_storeLFNEntryFieldsToBuf(PF_U_CHAR   *buf,
                                     PF_DENT_FAT *p_ent,
                                     PF_U_CHAR   ord,
                                     PF_U_CHAR   sum,
                                     PF_BOOL     is_last);

/*---------------------------------------------------------------------------
  PFENT_CompareAttr - Compare the entry and specified attribute
 ----------------------------------------------------------------------------*/
PF_BOOL  PFENT_CompareAttr(PF_U_LONG attr,
                           PF_U_LONG attr_required);

/*---------------------------------------------------------------------------
  PFENT_compareEntryName - Compare the entry and specified pattern
 ----------------------------------------------------------------------------*/
PF_S_LONG  PFENT_compareEntryName(PF_CONST PF_DENT_FAT *p_ent,
                                  PF_CONST PF_STR      *p_pattern,
                                  PF_U_CHAR            attr);

/*---------------------------------------------------------------------------
  PFENT_CompareEntry - Compare the entry and specified pattern
 ----------------------------------------------------------------------------*/
PF_S_LONG  PFENT_CompareEntry(PF_CONST PF_DENT *p_ent,
                              PF_CONST PF_STR  *p_pattern,
                              PF_U_LONG        attr_required);

/*---------------------------------------------------------------------------
  PFENT_GetEntryOfPath - Get the entry of a file or directory informations
                         associated with 'p_path'.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_GetEntryOfPath(PF_DENT         *p_ent,
                               PF_VOLUME       *p_vol,
                               PF_CONST PF_STR *p_path);

/*---------------------------------------------------------------------------
  PFENT_GetParentEntryOfPath - Get the parent entry of a directory associated
                               with 'p_path'.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_GetParentEntryOfPath(PF_DENT         *p_ent,
                                     PF_VOLUME       *p_vol,
                                     PF_CONST PF_STR *p_path);

/*---------------------------------------------------------------------------
  PFENT_findEntry - Search directory for the entry that maches both pattern
                    and attributes.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_findEntry(PF_FFD    *p_ffd,
                          PF_DENT   *p_ent,
                          PF_U_LONG index_search_from,
                          PF_STR    *p_pattern,
                          PF_U_CHAR attr_required,
                          PF_U_LONG *p_lpos,
                          PF_U_LONG *p_ppos);

/*---------------------------------------------------------------------------
  PFENT_allocateEntry - Find 'num_entries' successive unused entries in the
                        directory specified by 'p_ffd' and make 'p_ent'
                        point to the first of them.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_allocateEntry(PF_DENT   *p_ent,
                              PF_U_LONG num_entries,
                              PF_FFD    *p_ffd,
                              PF_U_LONG *p_prev_chain,
                              PF_STR    *p_filename,
                              PF_U_LONG attr_required,
                              PF_U_LONG *p_pos);

/*---------------------------------------------------------------------------
  PFENT_GetRootDir - Loads the root directory of the volume.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_GetRootDir(PF_VOLUME *p_vol, PF_DENT *p_ent);

/*---------------------------------------------------------------------------
  PFENT_MakeRootDir - Make the root directory of the volume.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_MakeRootDir(PF_VOLUME *p_vol);

/*---------------------------------------------------------------------------
 PFENT_UpdateSFNEntry - Write back the SFN directory entry specified by 'p_ent'.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_UpdateSFNEntry(PF_DENT_FAT *p_ent, PF_BOOL flag);

/*---------------------------------------------------------------------------
 PFENT_UpdateEntry - Write back the directory entry specified by 'p_ent'.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_UpdateEntry(PF_DENT_FAT *p_ent, 
                            PF_U_LONG   *p_prev_chain,
                            PF_BOOL     is_set_ARCH);

/*---------------------------------------------------------------------------
 PFENT_AdjustSFN - check the specified short name incluses tail number,
                   and search an empty SFN with tail.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_AdjustSFN(PF_DENT_FAT *p_ent, PF_CHAR *p_short_name);

/*---------------------------------------------------------------------------
 PFENT_RemoveEntry - Remove entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_RemoveEntry(PF_DENT_FAT *p_ent, PF_ENT_ITER *p_iter);

/*---------------------------------------------------------------------------
 PFENT_getcurrentDateTimeForEnt - Get current date and time with proper
                                  format for directory entries.
 ----------------------------------------------------------------------------*/
PF_U_CHAR  PFENT_getcurrentDateTimeForEnt(PF_U_SHORT *p_date,
                                          PF_U_SHORT *p_time,
                                          PF_U_CHAR  *p_utc_offset);

/*---------------------------------------------------------------------------
 PFENT_checkDateStamp - Check date information in timestamp.
 ----------------------------------------------------------------------------*/
PF_BOOL  PFENT_checkDateStamp(PF_SYS_DATE *p_date);

/*---------------------------------------------------------------------------
 PFENT_checkTimeStamp - Check time information in timestamp.
 ----------------------------------------------------------------------------*/
PF_BOOL  PFENT_checkTimeStamp(PF_VOLUME *p_vol, PF_SYS_TIME *p_time);

/*---------------------------------------------------------------------------
 PFENT_getTimeStamp - Get timestamp information.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_getTimeStamp(PF_DENT *p_ent, PF_TIMESTMP *p_timestamp);

/*---------------------------------------------------------------------------
 PFENT_setTimeStamp - Set timestamp information.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_setTimeStamp(PF_TIMESTMP *p_timestamp, PF_DENT *p_ent);

/*---------------------------------------------------------------------------
  PFENT_InitENT
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_InitENT(PF_DENT_FAT *p_ent,         /* out */
                        PF_STR      *p_filename,    /* in */
                        PF_U_CHAR   attr,           /* in */
                        PF_BOOL     is_set_time,    /* in */
                        PF_DENT_FAT *p_dir_ent,     /* in */
                        PF_VOLUME   *p_vol);        /* in */

/*---------------------------------------------------------------------------
  PFENT_FillVoidEntryToSectors - Some sector(s) is filled with void-entry(zero).
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_FillVoidEntryToSectors(PF_VOLUME *p_vol,
                                       PF_U_LONG start_sector,
                                       PF_U_LONG num_sectors,
                                       PF_BOOL   is_make_new_directory,
                                       PF_DENT   *p_ent,
                                       PF_DENT   *p_parent_ent);

/*---------------------------------------------------------------------------
  PFENT_IsDirectoryTreeUnder
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_IsDirectoryTreeUnder(PF_CONST PF_DENT *p_directory, /* in */
                                     PF_CONST PF_DENT *p_entry,     /* in */
                                     PF_BOOL          *p_is_found); /* in */

/*---------------------------------------------------------------------------
  PFENT_GetParentEntry
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_GetParentEntry(PF_DENT          *p_entry_parent,        /* out */
                               PF_CONST PF_DENT *p_entry,               /* in */
                               PF_BOOL          is_only_start_cluster); /* in */

/*---------------------------------------------------------------------------
  PFENT_IsSame - Compare that whether src is the same as dst. 
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_IsSame(PF_BOOL *p_is_same_file, /* out */
                       PF_BOOL *p_is_same_dir,  /* out */
                       PF_DENT *p_ent1,         /* in */
                       PF_DENT *p_parent_ent1,  /* in */
                       PF_DENT *p_ent2,         /* in */
                       PF_DENT *p_parent_ent2); /* in */

/*---------------------------------------------------------------------------
  PFENT_CmpName - Compare whether p_ent1's name and p_ent2's name are same.
 ----------------------------------------------------------------------------*/
PF_BOOL  PFENT_IsNameSame(PF_DENT_FAT *p_ent,
                          PF_STR      *p_name,
                          PF_BOOL     *is_short_name,
                          PF_BOOL     *is_TailSFN);

#endif  /* __PF_ENTRY_H__ */
