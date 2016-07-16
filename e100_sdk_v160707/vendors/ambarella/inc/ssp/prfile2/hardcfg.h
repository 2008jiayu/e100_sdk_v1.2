/**
 * system/include/ebcfg/hardcfg.h
 *
 * This file contains a set of hard-coded configuration macros used by the
 * eSOL MW libraries (PrKERNELv4, PrFILE, PrFILE2, PrUSB, etc). Since the
 * libraries are distributed in binary format, the Ambarella code and those
 * libraries must be matched exactly according to the defs here.
 *
 * History:
 *    2007/03/01 - [Charles Chiou] created file
 *
 * Copyright (C) 2004-2007, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef __EBCFG_HARDCFG_H__
#define __EBCFG_HARDCFG_H__

#define ENABLE_COREDUMP 1
#define ENABLE_PROFILING 1

#ifndef _AMBA_KAL_
#define _AMBA_KAL_
#endif

#ifdef CONFIG_ENABLE_EXFAT_SUPPORT
#define _AMBA_EXFAT_SUPPORT_
#endif
/*
 * PRFILE2 Options
 */

#define PF_BACKWARD_COMPATIBILITY_SUPPORT	0	//#undef
#define PF_STDLIB_ALIAS_SUPPORT			0	//#undef
#define CONFIG_PF_CHARACTER_CODE_SET_CP932	0	//#undef

#define PF_CHARACTER_CODE_SET ASCII
#define PF_MAX_DISK 13
#define PF_MAX_VOLUME 13
#define PF_MAX_FILE_NUM 10
#define PF_MAX_DIR_NUM 3
#define PF_MAX_CONTEXT_NUM 0
#define PF_USE_API_STUB 0
#undef PRFILE2_TASK_PRIORITY_LOW
#define PRFILE2_TASK_PRIORITY_MIDDLE 1
#undef PRFILE2_TASK_PRIORITY_HIGH
#define PRFILE2_TASK_STACK_SIZE 0x4000
#define PF_CRITICAL_SECTION_ENABLE		0	//#undef
#define PF_PARAM_CHECK_ENABLE 1
#define PF_EXCLUSIVE_DISK_ACCESS_ENABLE		0	//#undef

/***************************************************/
/* From 'parts/config/prfile2/eb_pf_config_ext.h'  */
/***************************************************/

#define PF_UNICODE_INTERFACE_SUPPORT    (1)
#define PF_FREE_CLUSTER_SEARCH_SUPPORT  (1)

/**********************************************/
/* From 'parts/config/prfile2/pf_config.h'    */
/**********************************************/

#define PF_AUTO_REGCTX_SUPPORT              (0)
#define PF_AUTO_CONTEXT_REGISTRATION        (0)
#define PF_MAX_EMPTY_ENTRY_DETECT           (0x1000L)
#define PF_SEQUENTIAL_FILE_SEARCH_SUPPORT   (1)
#define PF_DIRCACHE_SUPPORT                 (0)

/********************************************************/
/* From 'parts/config/prfile2/eb_pf_exfat_config_ext.h' */
/********************************************************/

#define PF_EXFAT_SUPPORT                (1)
#define PF_EXFAT_SET_PERCENT_IN_USE     (0)
#define PF_EXFAT_UPDATE_VOLUME_DITRY    (0)
#define PF_EXFAT_CHECK_VOLUME_DITRY     (0)
#define PF_EXFAT_FORMAT_SUPPORT         (1)
#define PF_EXFAT_USE_FAT_CHAIN          (1)

#if !((PF_MAX_EMPTY_ENTRY_DETECT >= (-1)) && \
      (PF_MAX_EMPTY_ENTRY_DETECT <= (0x7FFFFFFF)))
#error "PF_MAX_EMPTY_ENTRY_DETECT" is invalid value.
#endif

#if ((PF_AUTO_REGCTX_SUPPORT != 0) && (PF_MAX_CONTEXT_NUM == 0))
#error "If PF_AUTO_REGCTX_SUPPORT is '1'," \
" PF_MAX_CONTEXT_NUM should be '1' or more."
#endif /* ((PF_AUTO_REGCTX_SUPPORT == 1) && (PF_MAX_CONTEXT_NUM == 0)) */

#if (PF_SEQUENTIAL_FILE_SEARCH_SUPPORT != 0) && (PF_DIRCACHE_SUPPORT != 0)
#error "Please set either PF_SEQUENTIAL_FILE_SEARCH_SUPPORT or " \
"PF_DIRCACHE_SUPPORT"
#endif


#endif

