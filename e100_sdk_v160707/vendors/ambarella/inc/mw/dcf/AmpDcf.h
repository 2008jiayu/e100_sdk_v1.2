/**
 *  @file AmpDcf.h
 *
 *  DCF header
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef __AMPDCF_H__
#define __AMPDCF_H__

/**
 * @defgroup DCF
 * @brief DCF (Design rule for Camera File system) implementation
 *
 * The implementation of the DCF module
 * DCF module provides the API and interfaces to browse/update the mapping of media objects to the files in storage.
 *
 */

/**
 * @addtogroup DCF
 * @{
 */

#include <cfs/AmpCfs.h>

#define AMP_DCF_MAX_ROOT    4   /**< The maximum number of roots for each DCF handler */

/**
 * ITM configuration
 */
typedef struct {
    char Name[MAX_FILENAME_LENGTH];    /**< The name of the ITM file */
} AMP_DCF_ITM_CFG_s;

/**
 * The configuration of the default DCF table
 */
typedef struct {
    UINT32 MaxDir;  /**< The maximum number of directories that the default table can keep */
    UINT32 MaxFile; /**< The maximum number of files that the default table can keep */
    UINT8 MaxHdlr;  /**< The maximum number of default table handlers */
} AMP_DCF_DEF_TBL_CFG_s;

/**
 * The default configuration for initialize the DCF module
 */
typedef struct _AMP_DCF_INIT_CFG_s_ {
    AMP_TASK_INFO_s TaskInfo;           /**< DCF Task information (See AMP_TASK_INFO_s.) */
    void *Buffer;                       /**< The work buffer of the DCF module */
    UINT32 BufferSize;                  /**< The work buffer size of DCF module */
    UINT32 MaxDirPerDnum;               /**< The maximum number of directories per directory number */
    UINT32 MaxFilePerId;                /**< The maximum number of files per ID */
    UINT32 MaxPendingOp;                /**< The maximum pending operations */
    AMP_DCF_ITM_CFG_s ItmCfg;           /**< ITM configuration (See AMP_DCF_ITM_CFG_s.) */
    AMP_DCF_DEF_TBL_CFG_s DefTblCfg;    /**< The configuration of the default table (This is only worked when EnableDefTbl is TRUE. See AMP_DCF_DEF_TBL_CFG_s.) */
    UINT8 MaxHdlr;                      /**< The maximum number of DCF handlers in the DCF module */
    BOOL8 EnableITM;                    /**< The flag used to enable the ITM function */
    BOOL8 EnableDefTbl;                 /**< The flag used to enable the default table */
} AMP_DCF_INIT_CFG_s;

/**
 * DCF file entry in a file list
 */
typedef struct _AMP_DCF_FILE_s_ {
    char Name[MAX_FILENAME_LENGTH];    /**< File name */
} AMP_DCF_FILE_s;

/**
 * DCF file list
 */
typedef struct _AMP_DCF_FILE_LIST_s_ {
    UINT32 Count;               /**< File count */
    AMP_DCF_FILE_s *FileList;   /**< File list */
} AMP_DCF_FILE_LIST_s;

/**
 * DCF directory entry in a directory list
 */
typedef struct _AMP_DCF_DIR_s_ {
    char Name[MAX_FILENAME_LENGTH];    /**< Directory name */
} AMP_DCF_DIR_s;

/**
 * DCF directory list
 */
typedef struct _AMP_DCF_DIR_LIST_s_ {
    UINT32 Count;               /**< Directory count */
    AMP_DCF_DIR_s *DirList;     /**< Directory list */
} AMP_DCF_DIR_LIST_s;

struct _AMP_DCF_TABLE_s_;

/**
 * DCF table handler
 */
typedef struct {
    struct _AMP_DCF_TABLE_s_ *Func; /**< Functions of a table handler (See AMP_DCF_TABLE_s.) */
} AMP_DCF_TABLE_HDLR_s;

/**
 * The interface of DCF table functions
 */
typedef struct _AMP_DCF_TABLE_s_ {
    int (*Create)(UINT32, UINT32, AMP_DCF_TABLE_HDLR_s **);             /**< The interface to create a table handler */
    int (*Delete)(AMP_DCF_TABLE_HDLR_s *);                              /**< The interface to delete a table handler */
    int (*AddDirectory)(AMP_DCF_TABLE_HDLR_s *, UINT32, char *);       /**< The interface to add a directory */
    int (*RemoveDirectory)(AMP_DCF_TABLE_HDLR_s *, char *);            /**< The interface to remove a directory (It could remove a parent directory, so the table must be scanned.) */
    int (*AddFile)(AMP_DCF_TABLE_HDLR_s *, UINT32, UINT32, char *);    /**< The interface to add a file */
    int (*RemoveFile)(AMP_DCF_TABLE_HDLR_s *, UINT32, UINT32, char *); /**< The interface to remove a file */
    int (*GetFirstDnum)(AMP_DCF_TABLE_HDLR_s *);                        /**< The interface to get the first directory number (fnum would be set to first) */
    int (*GetLastDnum)(AMP_DCF_TABLE_HDLR_s *);                         /**< The interface to get the last directory number (fnum would be set to last) */
    int (*GetNextDnum)(AMP_DCF_TABLE_HDLR_s *, UINT32);                 /**< The interface to get the next directory number from the given dnum (dnum is just for reference, could be invalid) (fnum would be set to first) */
    int (*GetPrevDnum)(AMP_DCF_TABLE_HDLR_s *, UINT32);                 /**< The interface to get the previous directory number from the given dnum (dnum is just for reference, could be invalid) (fnum would be set to last) */
    int (*GetFirstFnum)(AMP_DCF_TABLE_HDLR_s *, UINT32);                /**< The interface to get the first file number (hdlr, dnum) */
    int (*GetLastFnum)(AMP_DCF_TABLE_HDLR_s *, UINT32);                 /**< The interface to get the last file number (hdlr, dnum) */
    int (*GetNextFnum)(AMP_DCF_TABLE_HDLR_s *, UINT32, UINT32);         /**< The interface to get the next file number from the given dnum and fnum (hdlr, dnum, fnum) (dnum and fnum are just for reference, could be invalid) */
    int (*GetPrevFnum)(AMP_DCF_TABLE_HDLR_s *, UINT32, UINT32);         /**< The interface to get the previous file number from the given dnum and fnum (hdlr, dnum, fnum) (dnum and fnum are just for reference, could be invalid) */
    int (*GetDirectoryList)(AMP_DCF_TABLE_HDLR_s *, UINT32, AMP_DCF_DIR_LIST_s *);      /**< The interface to get a list of directories with a specified number (hdlr, dnum, the address to put the directory list) */
    int (*GetFileList)(AMP_DCF_TABLE_HDLR_s *, UINT32, UINT32, AMP_DCF_FILE_LIST_s *);   /**< The interface to get a list of files with a specified number (hdlr, fnum, the address to put the file list) */
    BOOL (*CheckIdValid)(AMP_DCF_TABLE_HDLR_s *, UINT32, UINT32);       /**< The interface to check if an input ID is already in table, which is combined from a file number and a directory number (hdlr, dnum, fnum) */
} AMP_DCF_TABLE_s;

/**
 * The interface of DCF name filters
 */
typedef struct {
    UINT32 (*NameToDnum)(char *);                              /**< The interface to convert name to directory number */
    UINT32 (*NameToId)(char *);                                /**< The interface to convert name to ID */
    UINT32 (*GetId)(UINT32, UINT32);                            /**< The interface to get an ID in accordance with a directory number and a file number */
    UINT32 (*IdToDnum)(UINT32);                                 /**< The interface to convert ID to directory number */
    UINT32 (*IdToFnum)(UINT32);                                 /**< The interface to convert ID to file number */
    int (*ScanDirectory)(char *, AMP_DCF_TABLE_HDLR_s *);      /**< The interface to scan directories in a specified root into a DCF table */
    int (*ScanFile)(char *, AMP_DCF_TABLE_HDLR_s *);           /**< The interface to scan files in a specified root into a DCF table */
} AMP_DCF_FILTER_s;

/**
 * DCF handler
 */
typedef struct _AMP_DCF_HDLR_s_ {
    AMP_DCF_FILTER_s *Filter;   /**< DCF name filter */
} AMP_DCF_HDLR_s;

/**
 * The configuration for initializing a DCF handler
 */
typedef struct _AMP_DCF_CFG_s_ {
    AMP_DCF_FILTER_s *Filter;   /**< DCF name filter */
    AMP_DCF_TABLE_s *Table;     /**< DCF table */
    UINT32 MaxDir;              /**< The maximum number of directories in a DCF handler */
    UINT32 MaxFile;             /**< The maximum number of files in a DCF handler */
} AMP_DCF_CFG_s;

/**
 * Get the required buffer size for initializing the DCF module.
 * @param [in] maxHdlr The maximum number of DCF handlers
 * @param [in] stackSize Stack size
 * @param [in] maxDirPerDnum The maximum number of directories with the same number
 * @param [in] maxFilePerId The maximum number of files with the same ID
 * @param [in] maxPendingOp The maximum number of pending operations
 * @param [in] enableDefTable Enable default DCF table (need some more memory space)
 * @param [in] maxTblHdlr The maximum number of DCF table handlers (The value is only worked when EnableDefTable is TRUE.)
 * @param [in] maxTblDir The maximum number of directories in the DCF module (The number is shared between all DCF tables, and only worked when EnableDefTable is TRUE.)
 * @param [in] maxTblFile The maximum number of files in the DCF module (The number is shared between all DCF tables, and only worked when EnableDefTable is TRUE.)
 * @return The required buffer size
 */
extern UINT32 AmpDCF_GetRequiredBufferSize(UINT8 maxHdlr, UINT32 stackSize, UINT32 maxDirPerDnum, UINT32 maxFilePerId, UINT32 maxPendingOp, BOOL8 enableDefTable, UINT8 maxTblHdlr, UINT32 maxTblDir, UINT32 maxTblFile);

/**
 * Get the default configuration for initializing the DCF module.
 * @param [out] config The returned configuration of the DCF module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDCF_GetInitDefaultCfg(AMP_DCF_INIT_CFG_s *config);

/**
 * Get the default configuration for creating a DCF handler.
 * @param [out] config The returned configuration of a DCF handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDCF_GetDefaultCfg(AMP_DCF_CFG_s *config);

/**
 * Initialize the DCF module.
 * @param [in] config The configuration used to initialize the DCF module
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDCF_Init(AMP_DCF_INIT_CFG_s *config);

/**
 * Create a DCF handler. The system can have multiple DCF handlers.
 * @param [in] config The default configuration for creating a DCF handler
 * @param [out] hdlr The returned DCF handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDCF_Create(AMP_DCF_CFG_s *config, AMP_DCF_HDLR_s **hdlr);

/**
 * Delete a DCF handler.
 * @param [in] hdlr The DCF handler being deleted
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDCF_Delete(AMP_DCF_HDLR_s *hdlr);

/**
 * Add a DCF root into a DCF handler. This only scan sub-directories into the DCF table.
 * @param [in] hdlr The DCF handler
 * @param [in] path The path of a DCF root being added (e.g., c:\DCIM)
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDCF_AddRoot(AMP_DCF_HDLR_s *hdlr, char *path);

/**
 * Remove a DCF root from a DCF handler.
 * @param [in] hdlr The DCF handler
 * @param [in] path The path of a DCF root being removed (e.g., c:\DCIM)
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDCF_RemoveRoot(AMP_DCF_HDLR_s *hdlr, char *path);

/**
 * Scan all valid files into a DCF table.
 * @param [in] hdlr The DCF handler
 * @param [in] async Async mode (In sync mode, all directories are scanned after the function return. In async mode, only the last directory is scanned after the return.)
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDCF_Scan(AMP_DCF_HDLR_s *hdlr, BOOL async);

/**
 * Get the first ID.
 * @param [in] hdlr The DCF handler
 * @return The ID of the first DCF object
 */
extern int AmpDCF_GetFirstId(AMP_DCF_HDLR_s *hdlr);

/**
 * Get the last ID.
 * @param [in] hdlr The DCF handler
 * @return The ID of the last DCF object
 */
extern int AmpDCF_GetLastId(AMP_DCF_HDLR_s *hdlr);

/**
 * Get the next nth object ID from the current ID (No Cycle).
 * @param [in] hdlr The DCF handler
 * @return The ID of the next nth DCF object
 */
extern int AmpDCF_GetNextId(AMP_DCF_HDLR_s *hdlr);

/**
 * Get the previous nth object ID from the current ID (No Cycle).
 * @param [in] hdlr The DCF handler
 * @return The ID of the previous nth DCF object
 */
extern int AmpDCF_GetPrevId(AMP_DCF_HDLR_s *hdlr);

/**
 * Get the list of files with a specified ID. Should be released via AmpDCF_RelFileList() once the list is no longer used.
 * @param [in] hdlr The DCF handler
 * @param [in] id The object ID
 * @return The file list
 * @note Must release the file list once the list is no longer used
 */
extern AMP_DCF_FILE_LIST_s *AmpDCF_GetFileList(AMP_DCF_HDLR_s *hdlr, UINT32 id);

/**
 * Release the file list.
 * @param [in] hdlr The DCF handler
 * @param [in] list The file list being released
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDCF_RelFileList(AMP_DCF_HDLR_s *hdlr, AMP_DCF_FILE_LIST_s *list);

/**
 * Get the first directory number.
 * @param [in] hdlr The DCF handler
 * @return The first directory number
 */
extern int AmpDCF_GetFirstDnum(AMP_DCF_HDLR_s *hdlr);

/**
 * Get the last directory number.
 * @param [in] hdlr The DCF handler
 * @return The last directory number
 */
extern int AmpDCF_GetLastDnum(AMP_DCF_HDLR_s *hdlr);

/**
 * Get the next nth directory number from the current directory (No Cycle).
 * @param [in] hdlr The DCF handler
 * @return The number of the next nth directory
 */
extern int AmpDCF_GetNextDnum(AMP_DCF_HDLR_s *hdlr);

/**
 * Get the previous nth directory number from the current directory (No Cycle).
 * @param [in] hdlr The DCF handler
 * @return The number of the previous nth directory
 */
extern int AmpDCF_GetPrevDnum(AMP_DCF_HDLR_s *hdlr);

/**
 * Get the list of directories with a specified directory number. Should be released via AmpDCF_RelDirectoryList() once the list is no longer used.
 * @param [in] hdlr The DCF handler
 * @param [in] dnum The directory number
 * @return The directory list
 * @note Must release the directory list once the list is no longer used
 */
extern AMP_DCF_DIR_LIST_s *AmpDCF_GetDirectoryList(AMP_DCF_HDLR_s *hdlr, UINT32 dnum);

/**
 * Release the directory list.
 * @param [in] hdlr The DCF handler
 * @param [in] list The directory list being released
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDCF_RelDirectoryList(AMP_DCF_HDLR_s *hdlr, AMP_DCF_DIR_LIST_s *list);

/**
 * Add a directory to a DCF handler.
 * @param [in] hdlr The DCF handler
 * @param [in] name The directory name
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDCF_AddDirectory(AMP_DCF_HDLR_s *hdlr, char *name);

/**
 * Remove a directory from a DCF handler.
 * @param [in] hdlr The DCF handler
 * @param [in] name The directory name
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDCF_RemoveDirectory(AMP_DCF_HDLR_s *hdlr, char *name);

/**
 * Add a file to a DCF handler.
 * @param [in] hdlr The DCF handler
 * @param [in] name The file name
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDCF_AddFile(AMP_DCF_HDLR_s *hdlr, char *name);

/**
 * Remove a file from a DCF handler.
 * @param [in] hdlr The DCF handler
 * @param [in] name The file name
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDCF_RemoveFile(AMP_DCF_HDLR_s *hdlr, char *name);

/**
 * Check if an object is in a DCF handler.
 * @param [in] hdlr The DCF handler
 * @param [in] id The object ID
 * @return TRUE => the object is in the DCF handler FALSE => the object is not in the DCF handler
 */
extern BOOL AmpDCF_CheckIdValid(AMP_DCF_HDLR_s *hdlr, UINT32 id);

/**
 * Dump the ITM file.
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpDCF_DumpITM(void);

/**
 * @}
 */
#endif

