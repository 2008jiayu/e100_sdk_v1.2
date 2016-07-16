/**
 * ssp/amboot/bst/amba_bst.lds
 *
 * History:
 *    2014/11/04 - [Kerson Chen] modified from amboot bst.
 *
 * Copyright (C) 2004-2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */


DRAM_START_ADDR	= 0x00000000;
MIN_DRAM_SIZE	= 0x01000000;

AMBOOT_BST_RAM_START = DRAM_START_ADDR;
#if defined(CONFIG_ENABLE_SPINOR_BOOT)
NAND_BST_START = 0xE0030000;
#else
NAND_BST_START = 0xffff0000;
#endif

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(__vector)
SECTIONS
{
	. = NAND_BST_START;

	.bst_stage1 . : {
#if defined(CONFIG_ENABLE_EMMC_BOOT)
        *AmbaCortexBootstrapEmmcPreboot.o (.text)
#else
        *AmbaCortexBootstrapPreboot.o (.text)
#endif
		*AmbaCortexBootstrap.o (.text)
#if defined(CONFIG_ENABLE_EMMC_BOOT)
        *AmbaCortexBootstrapEmmcReboot.o (.text)
#endif
		*AmbaCortexInit*.o (.text)
		*AmbaCortexBootstrap.o (.rodata)
		*AmbaCortexInit*.o (.rodata)
	}

	.bst_stage2 AMBOOT_BST_RAM_START : AT (ADDR(.bst_stage1) + SIZEOF(.bst_stage1)) {
		__postreloc_ram_start = .;
#if defined(CONFIG_ENABLE_EMMC_BOOT)
        *AmbaCortexBootstrapEMMC* (.text)
#elif defined(CONFIG_ENABLE_SPINOR_BOOT)
		*AmbaCortexBootstrapNor* (.text)
#else
		*AmbaCortexBootstrapNAND* (.text)
#endif
		__postreloc_ram_end = .;
	}

	.data (NAND_BST_START + SIZEOF(.bst_stage1) + SIZEOF(.bst_stage2)) : AT (ADDR(.bst_stage1) + SIZEOF(.bst_stage1) + SIZEOF(.bst_stage2)) {
		__table_start = .;
#if defined(CONFIG_ENABLE_SPINOR_BOOT)
		*(BstNorParam)
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
		*(BstEmmcParam)
#else
		*(BstNandParam)
#endif
		*(BstDramParam)
		__table_end = .;
		. += 0x800 - (SIZEOF(.bst_stage1) + SIZEOF(.bst_stage2) + __table_end - __table_start) ;
	}

	__postreloc_start = LOADADDR(.bst_stage2);
	__postreloc_end = __postreloc_start + __postreloc_ram_end - __postreloc_ram_start;

	/DISCARD/ : {
		*(.ARM.exidx.exit.text)
		*(.ARM.extab.exit.text)
		*(.ARM.exidx.cpuexit.text)
		*(.ARM.extab.cpuexit.text)
		*(.ARM.exidx.devexit.text)
		*(.ARM.extab.devexit.text)
		*(.ARM*)
		*(.debug*)
		*(.discard*)
	}
}
