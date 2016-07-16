
OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(__vector)
SECTIONS
{
	. = CONFIG_BLD_LOADADDR;
	.text : {
		*AmbaBootlooder.o (.text)
		* (.text*)
		* (.rodata*)
#ifdef CONFIG_MMU
		*(.fixup)
#endif
	}

	. = ALIGN(4);
	.data : {
		* (.data*)
	}

	. = ALIGN(4);
	.bss.noinit : {
                __noinit_start = .;
                * (.bss.noinit*)
                __noinit_end = .;
        }

	. = ALIGN(4);
	.bss : {
                . = ALIGN(4);
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

#ifdef CONFIG_ARM_UNWIND
	/*
	 * Stack unwinding tables
	 */
	. = ALIGN(8);
	.ARM.unwind_idx : {
		__start_unwind_idx = .;
		*(.ARM.exidx*)
		__stop_unwind_idx = .;
	}
	.ARM.unwind_tab : {
		__start_unwind_tab = .;
		*(.ARM.extab*)
		__stop_unwind_tab = .;
	}
#endif

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
		*(.ARM.exidx.exit.text)
		*(.ARM.extab.exit.text)
		*(.ARM.exidx.cpuexit.text)
		*(.ARM.extab.cpuexit.text)
		*(.ARM.exidx.devexit.text)
		*(.ARM.extab.devexit.text)
		*(.discard*)
	}
}

