#ifndef APPLIB_INDEX_H_
#define APPLIB_INDEX_H_

#include <index/Index.h>

/**
 * Storage type for index file
 */
typedef enum _APPLIB_INDEX_FILE_STORAGE_TYPE_e_ {
    APPLIB_INDEX_FILE_STROAGE_TYPE_UNKNOW = 0,
    APPLIB_INDEX_FILE_STROAGE_TYPE_MEMORY,      /**< memory index   */
    APPLIB_INDEX_FILE_STROAGE_TYPE_NAND,        /**< raw index      */
    APPLIB_INDEX_FILE_STROAGE_TYPE_SD_CARD      /**< temp index     */
} APPLIB_INDEX_FILE_STORAGE_TYPE_e;

/**
 *  @brief create index file handler
 *
 *  create index file handler
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibIndex_CreateHdlr(AMP_INDEX_HDLR_s **indexHdlr);

/**
 *  @brief delete index file handler
 *
 *  delete index file handler
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibIndex_DeleteHdlr(AMP_INDEX_HDLR_s *indexHdlr);

/**
 *  @brief Initialize the index file
 *
 *  Initialize the index file
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibIndex_Init(void);

/**
 *  @brief Set the storage type for index file
 *
 *  Set the storage type for index file
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibIndex_SetStorageType(APPLIB_INDEX_FILE_STORAGE_TYPE_e indexType);

/**
 *  @brief Get the storage type for index file
 *
 *  Get the storage type for index file
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibIndex_GetStorageType(APPLIB_INDEX_FILE_STORAGE_TYPE_e *indexType);

#endif

