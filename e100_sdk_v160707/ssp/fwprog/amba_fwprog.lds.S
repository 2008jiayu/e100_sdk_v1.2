/**
 * fwprog.lds.S
 */

#define PROG_RAM_START	0x100000

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(__vector)
SECTIONS
{
	. = PROG_RAM_START;
	.text : {
		__BEGIN_FIRMWARE_IMAGE__ = ABSOLUTE(.);

		*AmbaBootlooder.o (.text)
		*.o (.text*)
		* (.rodata*)
	}

	.data : {
		. = ALIGN(4);
		* (.data*)
	}

	.fwdata : {
#if defined(__LINK_BST__)
		. = ALIGN(2048);
		__BEGIN_BST_IMAGE__ = .;
		KEEP (FW_OUT_DIR/bst.fw (*))
		. = ALIGN(2048);
                . += 0x100;             /* there is a 0x100 header in the fw. */
		__END_BST_IMAGE__ = .;
#endif

#if defined(__LINK_BLD__)
		. = ALIGN(2048);
		__BEGIN_BLD_IMAGE__ = .;
		KEEP (FW_OUT_DIR/bld.fw (*))
		. = ALIGN(2048);
                . += 0x100;             /* there is a 0x100 header in the fw. */
		__END_BLD_IMAGE__ = . ;
#endif

#if defined(__LINK_PBA__)
		. = ALIGN(2048);
		__BEGIN_PBA_IMAGE__ = .;
		KEEP (FW_OUT_DIR/pba.fw (*))
		. = ALIGN(2048);
                . += 0x100;             /* there is a 0x100 header in the fw. */
		__END_PBA_IMAGE__ = .;
#endif

#if defined(__LINK_SYS__)
		. = ALIGN(2048);
		__BEGIN_SYS_IMAGE__ = .;
		KEEP (FW_OUT_DIR/sys.fw (*))
		. = ALIGN(2048);
                . += 0x100;             /* there is a 0x100 header in the fw. */
		__END_SYS_IMAGE__ = .;
#endif

#if defined(__LINK_DSP__)
		. = ALIGN(2048);
		__BEGIN_DSP_IMAGE__ = .;
		KEEP (FW_OUT_DIR/dsp.fw (*))
		. = ALIGN(2048);
                . += 0x100;             /* there is a 0x100 header in the fw. */
		__END_DSP_IMAGE__ = .;
#endif

#if defined(__LINK_ROM__)
		. = ALIGN(2048);
		__BEGIN_ROM_IMAGE__ = .;
		KEEP (FW_OUT_DIR/rom.fw (*))
		. = ALIGN(2048);
                . += 0x100;             /* there is a 0x100 header in the fw. */
		__END_ROM_IMAGE__ = .;
#endif

#if defined(__LINK_LNX__)
		. = ALIGN(2048);
		__BEGIN_LNX_IMAGE__ = .;
		KEEP (FW_OUT_DIR/lnx.fw (*))
		. = ALIGN(2048);
                . += 0x100;             /* there is a 0x100 header in the fw. */
		__END_LNX_IMAGE__ = .;
#endif

#if defined(__LINK_DTB__)
		. = ALIGN(2048);
		__BEGIN_DTB_IMAGE__ = .;
		KEEP (FW_OUT_DIR/dtb.fw (*))
		. = ALIGN(2048);
                . += 0x100;             /* there is a 0x100 header in the fw. */
		__END_DTB_IMAGE__ = .;
#endif

#if defined(__LINK_RFS__)
		. = ALIGN(2048);
		__BEGIN_RFS_IMAGE__ = .;
		KEEP (FW_OUT_DIR/rfs.fw (*))
		. = ALIGN(2048);
                . += 0x100;             /* there is a 0x100 header in the fw. */
		__END_RFS_IMAGE__ = .;
#endif

		. = ALIGN(2048);
		__END_FIRMWARE_IMAGE__ = ABSOLUTE(.);
	}

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


		/* Stack for SVC mode */
		. = ALIGN(32);
		__svc_stack_start = .;
		. = __svc_stack_start + 0x8000;
		__svc_stack_end = .;


		/* Stack for IRQ mode */
		. = ALIGN(32);
		__irq_stack_start = .;
		. = __irq_stack_start + 0x100;
		__irq_stack_end = .;


		/* Stack for FIQ mode */
		. = ALIGN(32);
		__fiq_stack_start = .;
		. = __fiq_stack_start + 0x100;
		__fiq_stack_end = .;


		/* Stack for UND mode */
		. = ALIGN(32);
		__und_stack_start = .;
		. = __und_stack_start + 0x100;
		__und_stack_end = .;


		/* Stack for ABT mode */
		. = ALIGN(32);
		__abt_stack_start = .;
		. = __abt_stack_start + 0x100;
		__abt_stack_end = .;


		/* Stack for SYS mode */
		. = ALIGN(32);
		__sys_stack_start = .;
		. = __sys_stack_start + 0x8000;
		__sys_stack_end = .;
	}

 	PROVIDE (end = .);

	.memfwprog . (NOLOAD) : {
		. = ALIGN(0x10000);
		__memfwprog_result = .;
		. = __memfwprog_result  + (0x10000);
		. = ALIGN(0x10000);
		__memfwprog_command = .;
		. = __memfwprog_command + (0x10000);
	}

	/DISCARD/ : {
	/* Discard .ARM.exidx for directUSB parsing. */
	* (.ARM.exidx)

#if !defined(__LINK_BST__)
		FW_OUT_DIR/bst.fw (*)
#endif
#if !defined(__LINK_BLD__)
		FW_OUT_DIR/bld.fw (*)
#endif
#if !defined(__LINK_PBA__)
		FW_OUT_DIR/pba.fw (*)
#endif
#if !defined(__LINK_SYS__)
		FW_OUT_DIR/sys.fw (*)
#endif
#if !defined(__LINK_DSP__)
		FW_OUT_DIR/dsp.fw (*)
#endif
#if !defined(__LINK_ROM__)
		FW_OUT_DIR/rom.fw (*)
#endif
#if !defined(__LINK_LNX__)
		FW_OUT_DIR/lnx.fw (*)
#endif
#if !defined(__LINK_DTB__)
		FW_OUT_DIR/dtb.fw (*)
#endif
#if !defined(__LINK_RFS__)
		FW_OUT_DIR/rfs.fw (*)
#endif
	}
}
