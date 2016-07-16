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
 [pf_cfg_p.h] - PrFILE2 configuration file
 ****************************************************************************/
#ifndef __PF_CFG_P_H__
#define __PF_CFG_P_H__

/***************************************************************************
  CONFIGURATIONS
 ***************************************************************************/

/****************************************************************************
  Internal Configurations

  NOTES:
   - These configurations are *not* opened for users;
     Users do not modify them.
 ****************************************************************************/
/****************************************************************************
  Internal configuration
 ****************************************************************************/
  /*
   * If set, access lock for same file into same volume is enabled
   * that concerned sharing status of file.
   * Default : 1
   *
   * NOTES:
   *  - It has prepared for the guard that proper serialized by
   *    application is not made.
   */
  #define PF_ACTIVE_LOCK_ENABLE                     1

  /*
   * If set, access lock for the specified directory.
   * Default : 1
   */
  #define PF_ACTIVE_DIR_LOCK_ENABLE                 1

  /*
   * If set, total path length check enable, otherwise disable.
   * If not set this define, illigal at FAT32 spec.
   * But Performance will be up, and change that user request only.
   * Default : 1
   */
  #define PF_CHECK_PATH_LENGTH                      1

  /*
   * This aids in creating many similar long file names in one directory.
   * We can keep track of TRACK_BUFF_SIZE * 32 numbers.
   * Default : 1
   * NOTES:
   *  - This must be one or more.
   */
  #define TRACK_BUFF_SIZE                           1

  /*
   * This recognizes the NT reserved flag (small letter flag).
   * Windows NT, 2000, XP and etc use this flag to record the information
   * that the charactors of the base file name or the extension or both
   * are all small letters.
   * Default : 1
   */
  #define SMALL_LETTER_FLAG                         1

  /*
   * This converts a fullwords small latin capital letter to large one.
   * This behavior is almost same as Windows.
   * Default : 1
   * NOTES:
   *  - This must be one or more.
   */
  #define PF_SUPPORT_FW_CONVERT                     1

  /*
   * This is the setting of the signature for the extended short file
   * name entry.
   * Default : "\1\2"
   */
  #define PF_PATH_EXTSFN_SIGNATURE1                 0x01    /* '\1' */
  #define PF_PATH_EXTSFN_SIGNATURE2                 0x02    /* '\2' */

  /*
   * When read a cache, read all of the cache if the number of the before/after sector
   * of needless sector is smaller than this value.
   * Default : 2 (sectors)
   */
  #define CONTINUOUS_READ_THRESHOLD                 2

  /*
   * 'Readdir' of pfs interface return the long file name or not return.
   * The long file name will be returned if the value is set to 1.
   * Default : 1
   */
  #define PF_PFS_RETURN_LONG_NAME                   1

  /*
   * The maximum number of empty entries detected by pf_getempent().
   */
#if PF_BACKWARD_COMPATIBILITY_SUPPORT
  #define OVER_ENTRY                                PF_MAX_EMPTY_ENTRY_DETECT
#endif /* PF_BACKWARD_COMPATIBILITY_SUPPORT */

  /*
   * Cluster Link feature. 
   */
  #define PF_USE_CLSTLINK                           1

#if PF_FREE_CLUSTER_SEARCH_SUPPORT
  /*
   * PrFILE2 Module is supported or not.
   * Default : 1
   */
  #define PF_MODULE_SUPPORT                         1

  /*
   * PrFILE2 FAT Information API is supported or not.
   * Default : 1
   */
  #define PF_FATINFO_API_SUPPORT                    1

#else /* PF_FREE_CLUSTER_SEARCH_SUPPORT */
  #define PF_MODULE_SUPPORT                         0
  #define PF_FATINFO_API_SUPPORT                    0
#endif /* PF_FREE_CLUSTER_SEARCH_SUPPORT */

#if PF_MODULE_SUPPORT
  /*
   * PrFILE2 Extension API is supported or not.
   * Default : 1
   */
  #define PF_EXAPI_SUPPORT                          1

  /*
   * Loging call timing of PrF2 Extension API when the value is set to 1.
   * Default : 0
   */
  #define PF_EXAPI_TRACE_SUPPORT                    0

#else /* PF_MODULE_SUPPORT */
  #define PF_EXAPI_SUPPORT                          0
  #define PF_EXAPI_TRACE_SUPPORT                    0
#endif /* PF_MODULE_SUPPORT */

  /*
   * PrFILE2 debug functions for PFS are supported or not.
   * Default : 0
   */
  #define PF_DEBUG                                  0


/*---------------------------------------------------------------------------
  Configuration for backward compatibility support
 ----------------------------------------------------------------------------*/
  /*
   * This is setting for _mount API.
   */
  #define PF_BACKWARD_COMPATIBILITY_MOUNT           PF_BACKWARD_COMPATIBILITY_SUPPORT
  /*
   * This is setting for _buffering API.
   */
  #define PF_BACKWARD_COMPATIBILITY_BUFFERING       PF_BACKWARD_COMPATIBILITY_SUPPORT
  /*
   * This is setting for _chdir API.
   */
  #define PF_BACKWARD_COMPATIBILITY_CHDIR           PF_BACKWARD_COMPATIBILITY_SUPPORT
  /*
   * This is setting for _fsexec API.
   */
  #define PF_BACKWARD_COMPATIBILITY_FSEXEC          PF_BACKWARD_COMPATIBILITY_SUPPORT
  /*
   * This is setting for 13th member in file and directory entry.
   */
  #define PF_BACKWARD_COMPATIBILITY_SYSTIME         PF_BACKWARD_COMPATIBILITY_SUPPORT
  /*
   * This is setting for several structure name.
   */
  #define PF_BACKWARD_COMPATIBILITY_STRUCTURE       PF_BACKWARD_COMPATIBILITY_SUPPORT
  /*
   * This is setting for several paramater name.
   */
  #define PF_BACKWARD_COMPATIBILITY_PARAMETER       PF_BACKWARD_COMPATIBILITY_SUPPORT
  /*
   * This is setting for several error code name.
   */
  #define PF_BACKWARD_COMPATIBILITY_ERRORCODE       PF_BACKWARD_COMPATIBILITY_SUPPORT
  /*
   * This is setting for _create API.
   */
  #define PF_BACKWARD_COMPATIBILITY_CREATE          PF_BACKWARD_COMPATIBILITY_SUPPORT
  /*
   * This is setting for fopen API.
   */
  #define PF_BACKWARD_COMPATIBILITY_FOPEN           PF_BACKWARD_COMPATIBILITY_SUPPORT

#endif /* __PF_CFG_P_H__ */
