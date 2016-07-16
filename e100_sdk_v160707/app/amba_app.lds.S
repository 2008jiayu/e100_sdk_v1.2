
#define DDR_START			0xa0000000
#define DDR_RESV_SIZE			0x00001000
#ifdef CONFIG_AMBALINK_MEM_SIZE
#define LINUX_SIZE			CONFIG_AMBALINK_MEM_SIZE
#else
#define LINUX_SIZE			0
#endif

OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(__vector)
SECTIONS
{
	. = DDR_START;
	__ddr_start = .;
	__ddr_resv_start = .;
	. = __ddr_resv_start + DDR_RESV_SIZE;
	__ddr_resv_end = .;
	.text : {
		__ddr_region0_ro_start = .;
		*AmbaBoot.o (.text*)
		*AmbaHwStack.o (.text*)
		*AmbaBoot.o (.rodata*)
		*AmbaHwStack.o (.rodata*)
		* (.text*)
		* (.rodata*)
		* (.constdata*)
		/* For libaudio.a */
		* (i.__ARM_common*)
		* (FFTCODE*)
		__ddr_region0_ro_end = .;

		. = ALIGN(32);
		__ddr_region1_ro_start = .;
		__ddr_region1_ro_end = .;

		. = ALIGN(32);
		__ddr_region2_ro_start = .;
		__ddr_region2_ro_end = .;

		. = ALIGN(32);
		__ddr_region3_ro_start = .;
		__ddr_region3_ro_end = .;

		. = ALIGN(32);
		__ddr_region4_ro_start = .;
		__ddr_region4_ro_end = .;

		. = ALIGN(32);
		__ddr_region5_ro_start = .;
		__ddr_region5_ro_end = .;
	}

	.dsp_buf : {
		. = ALIGN(0x1000);
		__dsp_buf_start = .;
		*AmbaLL_Buffers.o (*)
		__dsp_buf_end = .;
	}

	.data : {
		. = ALIGN(0x1000);
		__ddr_region0_rw_start = .;
		*AmbaBoot.o (.data*)
		*AmbaHwStack.o (.data*)
		* (.data*)
		__ddr_region0_rw_end = .;

		. = ALIGN(32);
		__ddr_region1_rw_start = .;
		__ddr_region1_rw_end = .;

		. = ALIGN(32);
		__ddr_region2_rw_start = .;
		__ddr_region2_rw_end = .;

		. = ALIGN(32);
		__ddr_region3_rw_start = .;
		__ddr_region3_rw_end = .;

		. = ALIGN(32);
		__ddr_region4_rw_start = .;
		__ddr_region4_rw_end = .;

		. = ALIGN(32);
		__ddr_region5_rw_start = .;
		__ddr_region5_rw_end = .;
	}

#ifdef CONFIG_CC_CXX_SUPPORT
	. = ALIGN(8);
	/*
	 * Stack unwinding tables and exception tables for C++
	 */
	.ARM.unwind_idx : {
		__exidx_start = .;
		*(.ARM.exidx.text.*)
		*unwind-arm.o (.ARM.exidx*)
		*pr-support.o (.ARM.exidx*)
		__exidx_end = .;
	}
	.ARM.unwind_tab : {
		__start_unwind_tab = .;
		*(.ARM.extab*)
		__stop_unwind_tab = .;
	}
	/* define a global symbol _etext just after the last code byte */
/*
	_etext = .;
*/
#endif

	/* Provided malloc heap for libc. */
	PROVIDE (end = .);
	__sbrk_start = .;
	. = __sbrk_start + 0x2000;
	__sbrk_end = .;

	.bss.noinit : {
		. = ALIGN(32);
		__noinit_start = .;
		* (.bss.noinit*)
		__noinit_end = .;
	}

	.bss : {
		. = ALIGN(32);
		__bss_start = .;
		* (.bss*)
		* (COMMON)
		__bss_end = .;
	}

	/* FIXME: There are some unused memory here ! */

	/***********************/
	/* ucode orcode region */
	/***********************/
	. = ALIGN(0x100000);
	__ucode_start = .;
	__ucode_orcode_start = .;
	. = __ucode_orcode_start + 0x480000;
	__ucode_orcode_end = .;

	. = ALIGN(32);
	__ucode_orcme_start = .;
	. = __ucode_orcme_start + 0x80000;
	__ucode_orcme_end = .;

	. = ALIGN(32);
	__ucode_orcmd_start = .;
	. = __ucode_orcmd_start + 0x80000;
	__ucode_orcmd_end = .;

	. = ALIGN(32);
	__ucode_default_data_start = .;
	. = __ucode_default_data_start + 0x80000;
	__ucode_default_data_end = .;
	__ucode_end = .;

	/***************************/
	/*  non-cached heap region */
	/***************************/
	. = ALIGN(0x1000);
	__non_cache_heap_start = .;
	. = __non_cache_heap_start + 0x400000;
	__non_cache_heap_end = .;


	/***************************/
	/*  cached heap region     */
	/***************************/
	. = ALIGN(0x1000);
	__cache_heap_start = .;
	. = __ddr_start + CONFIG_DDR_SIZE - LINUX_SIZE;
	__cache_heap_end = .;

	/*************************/
	/* linux region          */
	/*************************/
	. = ALIGN(0x100000);
	__linux_start = .;
	. = __linux_start + LINUX_SIZE;
	__linux_end = .;

	/*************************/
	/* DDR end               */
	/*************************/

	/*
	 * Provide a end symbol for newlib malloc.
	 * We don't use malloc for newlib, so it should be ok.
	 */
	PROVIDE (end = .);

	. = __ddr_start + CONFIG_DDR_SIZE;
	__ddr_end = .;
}
