/*
    Copyright(C) 2005 eSOL Co., Ltd. All rights reserved.

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
[pdm_config.h] - Disk Manager configuration file
****************************************************************************/
#ifndef __PDM_CONFIG_H__
#define __PDM_CONFIG_H__

#include "parts.cfg"
#include "prfile2/pf_config.h"  /* PrFILE2 configuration */

/***************************************************************************
  CONFIGURATIONS
***************************************************************************/
#if USE_EBINDER
  /*
   * In eBinder environment, DiskManager's system parameters are defined
   * in configuration file 'eb_pdm_config.h', which is auto-generated by
   * eBinder Configurator.
   */
#include "prfile2/eb_pdm_config.h"

#else /* ! USE_EBINDER */
  /*
   * Set the number of drivers used.
   * Default : 1
   */
#define PDM_MAX_DISK                    PF_MAX_DISK

  /*
   * Set the number of total partition.
   *
   * NOTES:
   *  - When useing with PrFILE2, this value should be equivalent with PF_MAX_VOLUME.
   *  - When not useing with PrFILE2, it is necessary to set the number of partitions.
   */
#define PDM_MAX_PARTITION               PF_MAX_VOLUME

  /*
   * If set, using disk lock.
   * Default : 0
   */
#define PDM_DISK_LOCK_ENABLE            0

  /*
   * If set, using partition lock.
   * Default : 0
   */
#define PDM_PARTITION_LOCK_ENABLE       0

  /*
   * If set, using driver lock.
   * Default : 0
   */
#define PDM_CRITICAL_SECTION_ENABLE     PF_EXCLUSIVE_DISK_ACCESS_ENABLE

  /*
   * If set, you can expect to check the parameter error of application API.
   * Default : 0
   *
   * NOTES:
   *  - When using with PrFILE2, this value should be equivalent with PF_PARAM_CHECK_ENABLE.
   *  - When not using with PrFILE2, it is necessary to set condition.
   */
#define PDM_PARAM_CHECK_ENABLE          PF_PARAM_CHECK_ENABLE

#endif /* USE_EBINDER */

  /*
   * Don't change this definition.
   *
   * Please refer to "parts.cfg", when you change endian.
   *
   */
#if defined ESOL_BIG_ENDIAN
#define PDM_BIG_ENDIAN                   1
#else
#define PDM_BIG_ENDIAN                   0
#endif

#endif /* __PDM_CONFIG_H__ */
