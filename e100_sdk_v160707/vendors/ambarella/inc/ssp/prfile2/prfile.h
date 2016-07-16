/**
 * system/include/ebcfg/prfile.h
 *
 * History:
 *    2005/03/22 - [Charles Chiou] created file
 *
 * Copyright (C) 2004-2005, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef __EBCFG_PRFILE_H__
#define __EBCFG_PRFILE_H__

/********************************************/
/* From 'parts/config/prfile/eb_pf_cfg.cfg' */
/********************************************/

/* The following variables are taken care of by Kconfig */

#ifdef CONFIG_PF_INIDATA
	#define PF_INIDATA 1
#else
	#define PF_INIDATA 0
#endif
#ifdef CONFIG_LONG_FILE
	#define LONG_FILE 1
#else
	#define LONG_FILE 0
#endif
#ifdef CONFIG_FAT32_DSK
	#define FAT32_DSK 1
#else
	#define FAT32_DSK 0
#endif
#ifdef CONFIG_DRV_ERR
	#define DRV_ERR 1
#else
	#define DRV_ERR 0
#endif
#ifdef CONFIG_RD_ACCESSDATE
	#define RD_ACCESSDATE 1
#else
	#define RD_ACCESSDATE 0
#endif
#ifdef CONFIG_USE_STDLIB
	#define USE_STDLIB 1
#else
	#define USE_STDLIB 0
#endif
// #define SECCNT 512
// #define FRTSZT 5
// #define FATAREA_FLAG 1
// #define CLSTLINK_MAX 0
#ifdef CONFIG_FORMAT_LAYER_SELECTABLE
	#define FORMAT_LAYER_SELECTABLE 1
#else
	#define FORMAT_LAYER_SELECTABLE 0
#endif
// #define MAX_UNIT 8
#ifdef CONFIG_STUB_PF
	#define	STUB_PF	1
#else
	#define STUB_PF 0
#endif
#ifdef CONFIG_STUB_DRV
	#define STUB_DRV 0	/* This is always disabled: we have our   */
				/* own implementation in system/src/prf.c */
#else
	#define	STUB_DRV 0
#endif
#ifdef CONFIG_PF_EMPCLST_FOR_FSI
	#define PF_EMPCLST_FOR_FSI 1
#else
	#define PF_EMPCLST_FOR_FSI 0
#endif
#define PF_USE_NOENC 0
#define PF_USE_SJIS 1
#define PF_USE_BIG5 2
#define PF_USE_GB 3
#if defined(CONFIG_PF_USE_NOENC)
#define PF_CORRESPONDENCE_LANGUAGE PF_USE_NOENC
#elif defined(CONFIG_PF_USE_SJIS)
#define PF_CORRESPONDENCE_LANGUAGE PF_USE_SJIS
#elif defined(CONFIG_PF_USE_BIG5)
#define PF_CORRESPONDENCE_LANGUAGE PF_USE_BIG5
#elif defined(CONFIG_PF_USE_GB)
#define PF_CORRESPONDENCE_LANGUAGE PF_USE_GB
#endif


/********************************************/
/* From 'parts/config/prfile/eb_pf_dep.cfg' */
/********************************************/

#define	PF_KERNEL_NOPRODUCT	0
#define	PF_ESOL_PRKERNEL	1	/* Not supported. */
#define	PF_ESOL_PRKERNELV4	2
#define	PF_NEC_RX4000		3
#define	PF_KERNEL_TKERNEL	4
#define	PF_CUSTOM1		101	/* reserved. */
#define	PF_CUSTOM2		102	/* reserved. */
#define	PF_CUSTOM3		103	/* reserved. */
#define	PF_KERNEL_PRODUCT	PF_ESOL_PRKERNELV4

/**********************************************/
/* From 'parts/include/prfile/dbg_PrFILE.cfg' */
/**********************************************/

/* The following variables are taken care of by Kconfig */

#ifdef CONFIG_PF_TRACE
#define __TGT_TRACE__
#endif

#endif

