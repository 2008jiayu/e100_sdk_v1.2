
#define MYSTART		0x00000000

OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(myEntry)
SECTIONS
{
	. = MYSTART;
	.text : {
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
	.bss : {
		*(.dynbss)
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
		/* Align here to ensure that the .bss section occupies space up to
		   _end.  Align after .bss to ensure correct alignment even if the
		   .bss section disappears because there are no input sections.
		   FIXME: Why do we need it? When there is no .bss section, we don't
		   pad the .data section.  */
		. = ALIGN(. != 0 ? 32 / 8 : 1);
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

