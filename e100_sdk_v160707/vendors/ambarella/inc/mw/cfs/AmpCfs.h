/**
 * @file AmpCfs.h
 *
 * Copyright (C) 2004-2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef __AMPCFS_H__
#define __AMPCFS_H__

#include <stdlib.h>
#include <mw.h>
#include <AmbaFS.h>
#include <ctype.h>

/**
 * @defgroup CFS
 * @brief Cached file system
 *
 * The CFS module includes the following functions:
 * 1. CFS read/write function
 * 2. CFS seek function
 * 3. CFS sync function
 * 4. Other file system related function
 */

/**
 * @addtogroup CFS
 * @{
 */

#define MAX_FILENAME_LENGTH (64)                /**< The maximum length of file names */

#define AMP_CFS_FIO_ALIGNED_SIZE    (128 << 10) /**< The alignment size of FIO (128K bytes) */

/*
 * Seek Origin (the position used as reference for the offset)
 */
#define AMP_CFS_SEEK_CUR    AMBA_FS_SEEK_CUR    /**< The current position of a file (seek to the position relative to current file position) */
#define AMP_CFS_SEEK_START  AMBA_FS_SEEK_START  /**< The beginning of a file (seek to the position relative to start position of file) */
#define AMP_CFS_SEEK_END    AMBA_FS_SEEK_END    /**< The end of a file (seek to the position relative to end position of file) */

/*
 * File attributes
 */
#define AMP_CFS_ATTR_RDONLY ATTR_RDONLY         /**< Read only */
#define AMP_CFS_ATTR_HIDDEN ATTR_HIDDEN         /**< Hidden */
#define AMP_CFS_ATTR_DIR    ATTR_DIR            /**< Subdirectory */
#define AMP_CFS_ATTR_ARCH   ATTR_ARCH           /**< Archives */
#define AMP_CFS_ATTR_NONE   ATTR_NONE           /**< No attributes */
#define AMP_CFS_ATTR_ALL    ATTR_ALL            /**< All attributes (for fsfirst function) */

/*
 * Change attribute mode
 */
#define AMP_CFS_ATTR_ADD    ATTR_ADD            /**< Add mode */
#define AMP_CFS_ATTR_SUB    ATTR_SUB            /**< Delete mode */

/*
 * Sync mode for AmpCFS_Sync
 */
#define AMP_CFS_SYNC_FLUSH      AMBA_FS_SYNC_FLUSH      /**< Flush mode (to flush but not to invalidate caches) */
#define AMP_CFS_SYNC_INVALIDATE AMBA_FS_SYNC_INVALIDATE /**< Invalidate mode (to flush and invalidate caches) */

/**
 * Asynchronous mode parameters
 */
#define AMP_CFS_MAX_TASK_AMOUNT             2   /**< The maximum number of tasks */
#define AMP_CFS_MAX_DRIVE_AMOUNT            26  /**< The maximum number of drives */

/*
 * Debug
 */
//#define AMP_CFS_DEBUG_INIT

/*
 * FIO definition
 */
typedef AMBA_FS_STAT        AMP_CFS_STAT;       /**< The data type of file state  */
typedef AMBA_FS_DTA         AMP_CFS_DTA;        /**< The data type of directory entry */
typedef AMBA_FS_DEVINF      AMP_CFS_DEVINF;     /**< The data type of device information */

/**
 * File Mode
 */
typedef enum AMP_CFS_FILE_MODE_e_ {
    AMP_CFS_FILE_MODE_READ_ONLY = 0,    /**< "r" */
    AMP_CFS_FILE_MODE_READ_WRITE,       /**< "r+" */
    AMP_CFS_FILE_MODE_WRITE_ONLY,       /**< "w" */
    AMP_CFS_FILE_MODE_WRITE_READ,       /**< "w+" */
    AMP_CFS_FILE_MODE_APPEND_ONLY,      /**< "a" */
    AMP_CFS_FILE_MODE_APPEND_READ,      /**< "a+" */
    AMP_CFS_FILE_MODE_MAX               /**< max */
} AMP_CFS_FILE_MODE_e;

/**
 * Status of file streams
 */
typedef enum AMP_CFS_FILE_STATUS_e_ {
    AMP_CFS_STATUS_UNUSED = 0,          /**< The status indicating that a file is unused */
    AMP_CFS_STATUS_OPENED_READ,         /**< The status indicating that a file is opened to read */
    AMP_CFS_STATUS_OPENED_WRITE,        /**< The status indicating that a file is opened to write */
    AMP_CFS_STATUS_CLOSING_READ,        /**< The status indicating that a file is closing (async read mode only) */
    AMP_CFS_STATUS_CLOSING_WRITE,       /**< The status indicating that a file is closing (async write mode only) */
    AMP_CFS_STATUS_ERROR                /**< The status indicating that an error occurs in operating a file (async mode only, to block subsequent read/write) */
} AMP_CFS_FILE_STATUS_e;

/**
 * CFS event
 */
typedef enum AMP_CFS_EVENT_e_ {
    AMP_CFS_EVENT_FOPEN = 1,        /**< The event raised in opening a file */
    AMP_CFS_EVENT_FCLOSE,           /**< The event raised in closing a file */
    AMP_CFS_EVENT_CREATE,           /**< The event raised in creating a file */
    AMP_CFS_EVENT_REMOVE,           /**< The event raised in removing a file */
    AMP_CFS_EVENT_UPDATE,           /**< The event raised in updating a file */
    AMP_CFS_EVENT_RMDIR,            /**< The event raised in removing a directory */
    AMP_CFS_EVENT_MKDIR,            /**< The event raised in creating a directory */
    AMP_CFS_EVENT_FORMAT,           /**< The event raised in formating a drive */
    AMP_CFS_EVENT_IO_ERROR,         /**< The event raised when an I/O error occurs */
    AMP_CFS_EVENT_LOWSPEED,         /**< The event raised when an I/O operation is in low speed */
    AMP_CFS_EVENT_TOO_FRAGMENTED    /**< The event raised when the storage is too fragmented  */
} AMP_CFS_EVENT_e;

typedef int (*AMP_CFS_EVENT_HDLR_FP)(int opCode, UINT32 param);

/**
 * Types of Parameters for CFS callback functions
 *
 * Type1
 *  1. fclose: file name
 *  2. create: file name
 *  3. remove: file name
 *  4. update: file name
 *  5. Rmdir: dir name
 *  6. Mkdir: dir name
 */
typedef struct AMP_CFS_OP_TYPE1_s_ {
    char File[MAX_FILENAME_LENGTH];    /**< File name */
} AMP_CFS_OP_TYPE1_s;

/**
 * Type2
 *  1. Format
 *  2. LOW_RATE
 *  3. Too fragmented
 */
typedef struct AMP_CFS_OP_TYPE2_s_ {
    char Drive;    /**< Drive name */
} AMP_CFS_OP_TYPE2_s;

/**
 * Type3
 *  1. fopen: file name and file open mode
 */
typedef struct AMP_CFS_OP_TYPE3_s_ {
    char File[MAX_FILENAME_LENGTH];    /**< File name */
    UINT8 Mode;                         /**< File open mode (see AMP_CFS_FILE_MODE_e.) */
} AMP_CFS_OP_TYPE3_s;

/**
 * The configuration of the CFS module
 */
typedef struct AMP_CFS_CFG_s_ {
    UINT8 *Buffer;                                          /**< The work buffer of the CFS module */
    UINT32 BufferSize;                                      /**< The size of the work buffer */
    int (*FileOperation)(int, UINT32);                      /**< The callback function to report all events of file operations in the CFS module */
    UINT32 CacheMaxFileNum;                                 /**< The maximum number of cached files */
    UINT32 SchBankSize;                                     /**< The size of a bank */
    AMP_TASK_INFO_s TaskInfo;                               /**< The information of CFS background task */
    UINT32 SchLowTxRate[AMP_CFS_MAX_TASK_AMOUNT];           /**< The minimum acceptable transmission rate (Its unit is KBps. It is for AsyncMode.) */
    UINT32 SchLowSpeedSize[AMP_CFS_MAX_TASK_AMOUNT];        /**< The tolerance size of writing data continually in low speed */
    UINT8  SchTaskDriveTable[AMP_CFS_MAX_DRIVE_AMOUNT];     /**< The mapping table used to convert drive name to task ID */
    BOOL8  CacheEnable;                                     /**< The flag used to enable or disable the cache function of the CFS module (If the value is TRUE, the cache function will be enabled.) */
    UINT8  SchTaskAmount;                                   /**< The number of tasks */
    UINT8  SchBankAmount;                                   /**< The number of banks */
} AMP_CFS_CFG_s;

/**
 * Parameters for asyncing a file
 */
typedef struct AMP_CFS_FILE_PARAM_ASYNC_s_ {
    UINT8 MaxNumBank;                                       /**< The maximum number of banks used by a stream */
} AMP_CFS_FILE_PARAM_ASYNC_s;

/**
 * Parameters for opening a file
 */
typedef struct AMP_CFS_FILE_PARAM_s_ {
    char Filename[MAX_FILENAME_LENGTH];                    /**< File name */
    AMP_CFS_FILE_PARAM_ASYNC_s AsyncData;                   /**< The parameters related to asynchronous mode */
    UINT32 Alignment;                                       /**< File alignment (The value should be a multiple of a cluster size.) */
    UINT32 BytesToSync;                                     /**< The number of bytes that the CFS stream would invoke fsync() one time (A file should be syncd after BytesToSync bytes have been written.) */
    UINT8 Mode;                                             /**< File open mode (see AMP_CFS_FILE_MODE_e.) */
    BOOL8 AsyncMode;                                        /**< The flag to enable async mode (Indicate if a stream is in asynchronous mode.) */
    BOOL8 LowPriority;                                      /**< The flag to enable low priority (Indicate if a stream runs in low priority.) */
} AMP_CFS_FILE_PARAM_s;

/**
 * CFS File descriptor
 */
typedef struct AMP_CFS_FILE_s_ {
    char Filename[MAX_FILENAME_LENGTH];                    /**< Full path of a file */
} AMP_CFS_FILE_s;

/*
 * CFS APIs
 */

/**
 *  Get the required buffer size for initializing the CFS module.
 *  @param [in] schBankSize Bank size
 *  @param [in] schBankAmount The number of banks
 *  @param [in] schStackSize The stack size of scheduling tasks
 *  @param [in] schTaskAmount The number of scheduling tasks
 *  @param [in] cacheEnable Indicate if the cache is enabled
 *  @param [in] cacheMaxFileNum The maximum number of cached files
 *  @return The required buffer size
 */
extern UINT32 AmpCFS_GetRequiredBufferSize(UINT32 schBankSize, UINT8 schBankAmount, UINT32 schStackSize, UINT8 schTaskAmount, BOOL8 cacheEnable, UINT32 cacheMaxFileNum);

/**
 *  Get the default configuration for initializing the CFS module.
 *  @param [out] config The returned configuration of the CFS module
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_GetDefaultCfg(AMP_CFS_CFG_s *config);

/**
 *  Initialize the CFS module.
 *  @param [in] config The configuration used to initialize the CFS module
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_Init(AMP_CFS_CFG_s *config);

/*
 * Stream level
 */
/**
 *  Get the max cached data size of a file
 *  @param [in] file The file descriptor
 *  @param [out] dataSize The returned data size (bytes)
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AmpCFS_GetFileMaxCachedDataSize(AMP_CFS_FILE_s *file, UINT64 *dataSize);

/**
 *  Get default file parameters for opening a file descriptor.
 *  @param [out] fileParam The returned file parameters
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_GetFileParam(AMP_CFS_FILE_PARAM_s *fileParam);

/**
 *  Open a CFS file.
 *  @param [in] fileParam The parameters used to open a file descriptor
 *  @return File descriptor
 */
extern AMP_CFS_FILE_s* AmpCFS_fopen(AMP_CFS_FILE_PARAM_s *fileParam);

/**
 *  Close a file.
 *  @param [in] file The CFS file descriptor
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_fclose(AMP_CFS_FILE_s *file);

/**
 *  Read data elements from a file, and stores them in a buffer.
 *  @param [in] buffer The buffer in which data elements are stored
 *  @param [in] size The size of each data element being read
 *  @param [in] count The number of the data elements
 *  @param [in] file The file descriptor
 *  @return The number of elements read
 */
extern UINT64 AmpCFS_fread(void *buffer, UINT64 size, UINT64 count, AMP_CFS_FILE_s *file);

/**
 *  Write data elements from a buffer to a file.
 *  @param [in] buffer The buffer from which data elements are read
 *  @param [in] size The size of each data element being written
 *  @param [in] count The number of the data elements
 *  @param [in] file The file descriptor
 *  @return The number of elements written
 */
extern UINT64 AmpCFS_fwrite(const void *buffer, UINT64 size, UINT64 count, AMP_CFS_FILE_s *file);

/**
 *  Move file I/O pointer.
 *  @param [in] file The file descriptor
 *  @param [in] offset The number of bytes to offset from the origin position
 *  @param [in] origin The position used as reference for the offset
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_fseek(AMP_CFS_FILE_s *file, INT64 offset, int origin);

/**
 *  Get current file I/O pointer.
 *  @param [in] file The file descriptor
 *  @return Position of the file
 */
extern INT64  AmpCFS_ftell(AMP_CFS_FILE_s *file);
/**
 *  Flush all data in the cache for the specified file to media.
 *  @param [in] file The file descriptor
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_FSync(AMP_CFS_FILE_s *file);

/**
 *  Get the length of a file.
 *  @param [in] file The file descriptor
 *  @return >= 0 file length, -1 failure
 */
extern INT64  AmpCFS_FGetLen(AMP_CFS_FILE_s *file);

/**
 *  Adds consecutive clusters of a specified size to the end of a file.
 *  @param [in] file The file descriptor
 *  @param [in] size The size of the area to be added (bytes)
 *  @return The size (bytes) of the added area
 */
extern UINT64 AmpCFS_fappend(AMP_CFS_FILE_s *file, UINT64 size);

/**
 * Check the end of file.
 * @param [in] file The file descriptor
 * @return 1 - The EOF has been reached, 0 - The EOF has not been reached, or an internal error occurred, others - AMP_ER_CODE_e
 */
extern int AmpCFS_feof(AMP_CFS_FILE_s *file);

/*
 * File level
 * Most of the APIs require full paths of directory and file names.
 */
/**
 *  Get file or directory information.
 *  @param [in] fileName The file path used to get the file status
 *  @return The status of the file
 */
extern int AmpCFS_FStatus(const char *fileName);

/**
 *  Delete a file.
 *  @param [in] fileName The path of a file being removed.
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_remove(const char *fileName);

/**
 *  Move a source file to a destination location.
 *  @param [in] srcFileName The path of a source file
 *  @param [in] dstFileName The path of a destination file
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_Move(const char *srcFileName, const char *dstFileName);

/**
 *  Get the status of a file.
 *  @param [in]  fileName The file path used to obtain its state in the file system
 *  @param [out] status The returned state of the file
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_Stat(const char *fileName, AMP_CFS_STAT *status);

/**
 *  Change the attributes of a file.
 *  @param [in]  fileName File path
 *  @param [out] attribute File attributes
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_Chmod(const char *fileName, int attribute);

/**
 *  Search files and return the first matched result.
 *  @param [in]  dirName The directory path being searched
 *  @param [in]  attribute Directory attributes
 *  @param [out] dirEntry The returned directory entry
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_FirstDirEnt(const char *dirName, unsigned char attribute, AMP_CFS_DTA *dirEntry);

/**
 *  Search files and return the next matched result.
 *  @param [out] dirEntry The returned directory entry
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_NextDirEnt(AMP_CFS_DTA *dirEntry);

/**
 *  Combine two files into one file.
 *  @param [in] baseFilename The path of the base file
 *  @param [in] addFilename The path of the add file
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_Combine(const char *baseFilename, const char *addFilename);

/**
 *  Divide a file into two files.
 *  @param [in] orgFilename The path of a file being divided
 *  @param [in] newFilename The path of a file being created after division
 *  @param [in] offset Byte offset from the beginning of the original file to a division location
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_Divide(const char *orgFilename, const char *newFilename, UINT32 offset);

/**
 *  Allocate and insert clusters to a file.
 *  @param [in] fileName File path
 *  @param [in] offset The offset from the beginning of the file to a specified location to insert clusters
 *  @param [in] number The number of clusters being inserted
 *  @return The number of clusters that are actually inserted
 */
extern int AmpCFS_Cinsert(const char *fileName, UINT32 offset, UINT32 number);

/**
 *  Delete clusters from a file.
 *  @param [in] fileName File path
 *  @param [in] offset The offset from the beginning of the file to a specified location to delete clusters
 *  @param [in] number The number of clusters being deleted
 *  @return The number of clusters that are actually deleted
 */
extern int AmpCFS_Cdelete(const char *fileName, UINT32 offset, UINT32 number);

/**
 *  Create a new directory.
 *  @param [in] dirName The path of a directory being created
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_Mkdir(const char *dirName);

/**
 *  Delete a directory.
 *  @param [in] dirName The path of a directory being deleted
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_Rmdir(const char *dirName);

/**
 *  Get the last error number
 *  @param [out] errNum The returned error number
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AmpCFS_GetError(int *errNum);

/*
 * Drive level
 */

/**
 *  Get device capacity (i.e., the information of a drive).
 *  @param [in]  driveName Drive name (from A to Z)
 *  @param [out] devInfo The returned information of device capacity
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_GetDev(char driveName, AMP_CFS_DEVINF *devInfo);

/**
 *  Get the cached data size of a drive.
 *  @param [in]  driveName Drive name (from A to Z)
 *  @param [out] sizeByte The returned size of cached data
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_GetCachedDataSize(char driveName, UINT64 *sizeByte);

/**
 *  Format a drive.
 *  @param [in] driveName Drive name (from A to Z)
 *  @param [in] param The parameter string used to notify the driver of the format type
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_Format(char driveName, const char *param);

/**
 *  Write all cached data for a specified drive back to the underlying device.
 *  @param [in] driveName Drive name (from A to Z)
 *  @param [in] mode Invalidation mode (In AMBA_FS_NINVALIDATE mode, caches will not be invalidated; however, in AMBA_FS_INVALIDATE mode, caches will be invalidated.)
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_Sync(char driveName, int mode);

/**
 *  Mount a drive
 *  @param [in] driveName Drive name (from A to Z)
 *  @return 0 - AMP_OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_Mount(char driveName);

/**
 *  Unmount a drive
 *  @param [in] driveName Drive name (from A to Z)
 *  @return 0 - AMP_OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_Unmount(char driveName);

/*
 * Cache Operations
 */

/**
 *  Clear drive caches which include file states, directory entries, and drive information.
 *  @param [in] driveName Drive name (from A to Z)
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AmpCFS_ClearCache(char driveName);

/**
 * @}
 */
#endif
