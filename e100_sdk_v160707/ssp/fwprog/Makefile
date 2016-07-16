#
# Actually, it's necessary to add $(MEMFWPROG),
# but use .stamp_output for printing out message when done.
#
obj-y	:= libfwprog.a $(DIR_amba_fwprog_out)/.stamp_output

libfwprog-objs := AmbaMEMFWPROG.o \
                  AmbaFwLoader_RegionInfo.o

libfwprog-objs += AmbaVer.o
CFLAGS_AmbaVer.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
		   -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
		   -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
		   -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
		   -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
		   -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y :=	-I$(src)						\
		-I$(srctree)/vendors/ambarella/inc/ssp			\
		-I$(srctree)/vendors/ambarella/inc/ssp/soc		\
		-I$(srctree)/vendors/ambarella/inc/ssp/soc/non_os	\
		-I$(srctree)/vendors/ambarella/inc/comsvc/misc

ifndef DATE
DATE	:=	$(shell date +%Y%m%d)
endif

YEAR	:= 	$(shell date +%Y | sed s/^0//)
MONTH	:=	$(shell date +%m | sed s/^0//)
DAY	:= 	$(shell date +%d | sed s/^0//)

MY_DATE :=	$(shell echo $(YEAR)\*65536\+$(MONTH)\*256\+$(DAY) | bc)

# Because of the quoted string format that Kconfig uses, we need to
# # destringify the following variables before processing them:
BST		:= 	$(shell echo $(CONFIG_BST))
BST_VIF		:= 	$(shell echo $(CONFIG_BST_VIF) | awk -F "." '{ver = $$1 * 16777216 + $$2 * 65536 + $$3; printf "%s\n", ver}')
BLD		:= 	$(shell echo $(CONFIG_BLD))
BLD_VIF		:= 	$(shell echo $(CONFIG_BLD_VIF) | awk -F "." '{ver = $$1 * 16777216 + $$2 * 65536 + $$3; printf "%s\n", ver}')
PBA		:= 	$(shell echo $(CONFIG_PBA))
PBA_VIF		:= 	$(shell echo $(CONFIG_PBA_VIF) | awk -F "." '{ver = $$1 * 16777216 + $$2 * 65536 + $$3; printf "%s\n", ver}')
SYS		:= 	$(shell echo $(CONFIG_SYS))
SYS_VIF		:= 	$(shell echo $(CONFIG_SYS_VIF) | awk -F "." '{ver = $$1 * 16777216 + $$2 * 65536 + $$3; printf "%s\n", ver}')
DSP		:= 	$(shell echo $(CONFIG_DSP))
DSP_VIF		:= 	$(shell echo $(CONFIG_DSP_VIF) | awk -F "." '{ver = $$1 * 16777216 + $$2 * 65536 + $$3; printf "%s\n", ver}')
DSP_LIST	:= 	$(shell echo $(CONFIG_DSP_LIST_FILE))
ROM		:= 	$(shell echo $(CONFIG_ROM))
ROM_VIF		:= 	$(shell echo $(CONFIG_ROM_VIF) | awk -F "." '{ver = $$1 * 16777216 + $$2 * 65536 + $$3; printf "%s\n", ver}')
ROM_LIST	:= 	$(shell echo $(CONFIG_ROM_LIST_FILE))

ifeq ($(CONFIG_ENABLE_AMBALINK),y)
# Note: these three use full path
LNX		:= 	$(shell echo $(CONFIG_LNX))
DTB		:= 	$(shell echo $(CONFIG_DTB))
RFS		:= 	$(shell echo $(CONFIG_RFS))
else
LNX		:=
DTB		:=
RFS		:=
endif

LNX_VIF		:= 	$(shell echo $(CONFIG_LNX_VIF) | awk -F "." '{ver = $$1 * 16777216 + $$2 * 65536 + $$3; printf "%s\n", ver}')
RFS_VIF		:= 	$(shell echo $(CONFIG_RFS_VIF) | awk -F "." '{ver = $$1 * 16777216 + $$2 * 65536 + $$3; printf "%s\n", ver}')

FWLIST := $(FWPROG_OUT_DIR)/.fwlist
ifeq ($(FWLIST),$(wildcard $(FWLIST)))
-include $(FWLIST)
FIRMWARES := $(sort $(FIRMWARES))
endif

# ------------------------------------------------------------------ #

AFLAGS := $(KBUILD_AFLAGS)
CFLAGS_FWPROG := $(KBUILD_CFLAGS)

# ------------------------------------------------------------------ #
PAYLOAD	= \
		  $(FWPROG_OUT_DIR)/bst.fw \
		  $(FWPROG_OUT_DIR)/bld.fw \
		  $(FWPROG_OUT_DIR)/pba.fw \
		  $(FWPROG_OUT_DIR)/sys.fw \
		  $(FWPROG_OUT_DIR)/dsp.fw \
		  $(FWPROG_OUT_DIR)/rom.fw \
		  $(FWPROG_OUT_DIR)/lnx.fw \
		  $(FWPROG_OUT_DIR)/dtb.fw \
		  $(FWPROG_OUT_DIR)/rfs.fw

# ------------------------------------------------------------------ #
FWPROG_LIB_PATH = -L$(srctree)/vendors/ambarella/lib

cmd_chk_lib = $(if $(wildcard $(AMBA_O_LIB)/$1), $(AMBA_O_LIB)/$1, $(AMBA_LIB_FOLDER)/$1)

FWPROG_LIBS = $(call cmd_chk_lib,libsoc_non_os.a)
FWPROG_LIBS += $(call cmd_chk_lib,libcomsvc_sys.a)
FWPROG_LIBS += $(call cmd_chk_lib,libperipheral.a)
FWPROG_LIBS += $(obj)/libfwprog.a

FWPROG	= $(addsuffix .elf,$(FIRMWARES))
FWPROG	:= $(addprefix $(objtree)/$(KBUILD_AMBA_FWPROG_OUT_DIR)/,$(FWPROG))

$(FWPROG_OUT_DIR)/.stamp_output: $(obj)/libfwprog.a $(FWPROG)
	@mkdir -p $(dir $@)
	@touch $@

BSP_OBJECT = $(call cygpath,$(objtree)/$(BSP_DIR)/bsp.o)

$(objtree)/$(KBUILD_AMBA_FWPROG_OUT_DIR)/%.elf: $(FWPROG_OUT_DIR)/header_%.fw \
		     $(PAYLOAD) $(FWPROG_LIBS) \
		     $(FWPROG_OUT_DIR)/%.lds
	@mkdir -p $(objtree)/$(KBUILD_AMBA_FWPROG_OUT_DIR)
	@echo "  Gen     $@"
	$(Q)$(CC) $(CFLAGS_FWPROG) -Wl,-T,$(FWPROG_OUT_DIR)/$(subst .elf,.lds,$(notdir $@)) \
		$(BSP_OBJECT)		\
		-nostartfiles -o $(call cygpath,$@) $(call cygpath,$<) 	\
		 $(call cygpath,$(FWPROG_LIB_PATH)) -Wl,--start-group  $(call cygpath,$(FWPROG_LIBS)) -Wl,--end-group	\
		-lc -lnosys -lm -lgcc -lrdimon	\
		-Wl,--gc-sections -Wl,--no-enum-size-warning -Wl,--no-wchar-size-warning	\
		 $(call cygpath,$(PAYLOAD))



# Linker script targets


.SECONDARY:

# ------------------------------------------------------------------ #

# Object targets


$(FWPROG_OUT_DIR)/header_%.fw: $(FWPROG_OUT_DIR)/header_%.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) -c $(call cygpath,$<) -o $@

# ------------------------------------------------------------------ #


$(FWPROG_OUT_DIR)/%.lds: $(src)/amba_fwprog.lds.S
	@mkdir -p $(FWPROG_OUT_DIR)
	@mydefs=""; \
	if [ "$(findstring bst,$@)" != "" ]; then \
		mydefs="-D__LINK_BST__"; \
	fi; \
	if [ "$(findstring bld,$@)" != "" ]; then \
		mydefs="$$mydefs -D__LINK_BLD__"; \
	fi; \
	if [ "$(findstring pba,$@)" != "" ]; then \
		mydefs="$$mydefs -D__LINK_PBA__"; \
	fi; \
	if [ "$(findstring sys,$@)" != "" ]; then \
		mydefs="$$mydefs -D__LINK_SYS__"; \
	fi; \
	if [ "$(findstring dsp,$@)" != "" ]; then \
		mydefs="$$mydefs -D__LINK_DSP__"; \
	fi; \
	if [ "$(findstring rom,$@)" != "" ]; then \
		mydefs="$$mydefs -D__LINK_ROM__"; \
	fi; \
	if [ "$(findstring lnx,$@)" != "" ]; then \
		mydefs="$$mydefs -D__LINK_LNX__ -D__LINK_DTB__"; \
	fi; \
	if [ "$(findstring rfs,$@)" != "" ]; then \
		mydefs="$$mydefs -D__LINK_RFS__"; \
	fi; \
	echo $(CPP) $(CFLAGS_FWPROG) -DFW_OUT_DIR=$(FWPROG_OUT_DIR) -D__ASM__ $$mydefs $< | sed /^#/d > $@; \
	$(CPP) $(CFLAGS_FWPROG) -DFW_OUT_DIR=$(FWPROG_OUT_DIR) -D__ASM__ $$mydefs $(call cygpath,$<) | sed /^#/d > $@

# Temp. assembler file targets

$(FWPROG_OUT_DIR)/header_%.S: $(src)/header.S
	@mkdir -p $(FWPROG_OUT_DIR)
	@mydefs=""; \
	if [ "$(findstring bst,$@)" != "" ]; then \
		mydefs="-D__LINK_BST__"; \
	fi; \
	if [ "$(findstring bld,$@)" != "" ]; then \
		mydefs="$$mydefs -D__LINK_BLD__"; \
	fi; \
	if [ "$(findstring pba,$@)" != "" ]; then \
		mydefs="$$mydefs -D__LINK_PBA__"; \
	fi; \
	if [ "$(findstring sys,$@)" != "" ]; then \
		mydefs="$$mydefs -D__LINK_SYS__"; \
	fi; \
	if [ "$(findstring dsp,$@)" != "" ]; then \
		mydefs="$$mydefs -D__LINK_DSP__"; \
	fi; \
	if [ "$(findstring rom,$@)" != "" ]; then \
		mydefs="$$mydefs -D__LINK_ROM__"; \
	fi; \
	if [ "$(findstring lnx,$@)" != "" ]; then \
		mydefs="$$mydefs -D__LINK_LNX__ -D__LINK_DTB__"; \
	fi; \
	if [ "$(findstring rfs,$@)" != "" ]; then \
		mydefs="$$mydefs -D__LINK_RFS__"; \
	fi; \
	echo $(CPP) $(CFLAGS_FWPROG) -D__ASM__ $$mydefs $< | sed /^#/d > $@; \
	$(CPP) $(CFLAGS_FWPROG) -D__ASM__ $$mydefs $(call cygpath,$<) | sed /^#/d > $@

# ------------------------------------------------------------------ #
#
# Firmware object files: FIXME: final payload path
#

ifneq ($(DSP),)
# Note: LIST file uses relative path to srctree
$(objtree)/$(DSP):
	@echo "  Gen     $@"
	@cd $(srctree) && $(AMBA_HOST_ROMFS) $(DSP_LIST) $@ 0x1000000 && cd -;
	@if [ "$(findstring gz,$@)" != "" ]; then \
		gzip -c -9 $@ > $@.gz; \
		cp $@ $@.tmp; \
		mv $@.gz $@; \
	else \
		cp $@ $@.tmp; \
	fi;
endif

ifneq ($(ROM),)
# Note: LIST file uses relative path to srctree
$(objtree)/$(ROM):
	@echo "  Gen     $@"
	@cd $(srctree) && $(AMBA_HOST_ROMFS) $(ROM_LIST) $@ 0x2000000 && cd -;
	@if [ "$(findstring gz,$@)" != "" ]; then \
		gzip -c -9 $@ > $@.gz; \
		cp $@ $@.tmp; \
		mv $@.gz $@; \
	else \
		cp $@ $@.tmp; \
	fi;
endif

ifeq ($(CONFIG_ENABLE_SPINOR_BOOT),y)
FW_FLAG :=0x2
else
FW_FLAG :=0x0
endif
# ------------------------------------------------------------------ #

ifneq ($(BST),)
$(FWPROG_OUT_DIR)/bst.fw: $(objtree)/$(BST) $(src)/piggy_bst.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) \
		-DCRC32=$(word 3,$(shell $(AMBA_HOST_CRC) $<)) \
		-DIMG_LEN=$(word 2,$(shell $(AMBA_HOST_CRC) $<)) \
		-DFIRM_VER_NUM=$(BST_VIF) \
		-DFIRM_VER_DATE=$(MY_DATE) \
		-DFIRM_MEM_ADDR=0xffff0000 \
		-DPARTHD_MAGIC=0xa324eb90 \
		-DINCBIN=$(call cygpath,$<) \
		-c $(call cygpath,$(word 2,$^)) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/bst.bin
else
$(FWPROG_OUT_DIR)/bst.fw: $(src)/piggy_bst.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) -c $(call cygpath,$<) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/bst.bin
endif

ifneq ($(BLD),)
$(FWPROG_OUT_DIR)/bld.fw: $(objtree)/$(BLD) $(src)/piggy_bld.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) \
		-DCRC32=$(word 3,$(shell $(AMBA_HOST_CRC) $<)) \
		-DIMG_LEN=$(word 2,$(shell $(AMBA_HOST_CRC) $<)) \
		-DFIRM_VER_NUM=$(BLD_VIF) \
		-DFIRM_VER_DATE=$(MY_DATE) \
		-DFIRM_MEM_ADDR=$(CONFIG_BLD_LOADADDR) \
		-DPARTHD_MAGIC=0xa324eb90 \
		-DINCBIN=$(call cygpath,$<) \
		-c $(call cygpath,$(word 2,$^)) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/bld.bin
else
$(FWPROG_OUT_DIR)/bld.fw: $(src)/piggy_bld.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) -c $(call cygpath,$<) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/bld.bin
endif

ifneq ($(PBA),)
$(FWPROG_OUT_DIR)/pba.fw: $(objtree)/$(PBA) $(src)/piggy_pba.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) \
		-DCRC32=$(word 3,$(shell $(AMBA_HOST_CRC) $<)) \
		-DIMG_LEN=$(word 2,$(shell $(AMBA_HOST_CRC) $<)) \
		-DFIRM_VER_NUM=$(PBA_VIF) \
		-DFIRM_VER_DATE=$(MY_DATE) \
		-DFIRM_MEM_ADDR=$(CONFIG_PBA_LOADADDR) \
		-DPARTHD_MAGIC=0xa324eb90 \
		-DINCBIN=$(call cygpath,$<) \
		-DOIMG_LEN=$(word 2,$(shell $(AMBA_HOST_CRC) $<.tmp)) \
		-DFIRM_FLAG=$(FW_FLAG) \
		-c $(call cygpath,$(word 2,$^)) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/pba.bin
else
$(FWPROG_OUT_DIR)/pba.fw: $(src)/piggy_pba.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) -c $(call cygpath,$<) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/pba.bin
endif

ifneq ($(SYS),)
$(FWPROG_OUT_DIR)/sys.fw: $(objtree)/$(SYS) $(src)/piggy_sys.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) \
		-DCRC32=$(word 3,$(shell $(AMBA_HOST_CRC) $<)) \
		-DIMG_LEN=$(word 2,$(shell $(AMBA_HOST_CRC) $<)) \
		-DFIRM_VER_NUM=$(SYS_VIF) \
		-DFIRM_VER_DATE=$(MY_DATE) \
		-DFIRM_MEM_ADDR=$(CONFIG_SYS_LOADADDR) \
		-DPARTHD_MAGIC=0xa324eb90 \
		-DINCBIN=$(call cygpath,$<) \
		-DOIMG_LEN=$(word 2,$(shell $(AMBA_HOST_CRC) $<.tmp)) \
		-DFIRM_FLAG=$(FW_FLAG) \
		-c $(call cygpath,$(word 2,$^)) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/sys.bin
else
$(FWPROG_OUT_DIR)/sys.fw: $(src)/piggy_sys.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) -c $(call cygpath,$<) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/sys.bin
endif

ifneq ($(DSP),)
$(FWPROG_OUT_DIR)/dsp.fw: $(objtree)/$(DSP) $(src)/piggy_dsp.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) \
		-DCRC32=$(word 3,$(shell $(AMBA_HOST_CRC) $<)) \
		-DIMG_LEN=$(word 2,$(shell $(AMBA_HOST_CRC) $<)) \
		-DFIRM_VER_NUM=$(DSP_VIF) \
		-DFIRM_VER_DATE=$(MY_DATE) \
		-DPARTHD_MAGIC=0xa324eb90 \
		-DINCBIN=$(call cygpath,$<) \
		-DOIMG_LEN=$(word 2,$(shell $(AMBA_HOST_CRC) $<.tmp)) \
		-DFIRM_FLAG=$(FW_FLAG) \
		-c $(call cygpath,$(word 2,$^)) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/dsp_fw.bin
else
$(FWPROG_OUT_DIR)/dsp.fw: $(src)/piggy_dsp.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) -c $(call cygpath,$<) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/dsp_fw.bin
endif

ifneq ($(ROM),)
$(FWPROG_OUT_DIR)/rom.fw: $(objtree)/$(ROM) $(src)/piggy_rom.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) \
		-DCRC32=$(word 3,$(shell $(AMBA_HOST_CRC) $<)) \
		-DIMG_LEN=$(word 2,$(shell $(AMBA_HOST_CRC) $<)) \
		-DFIRM_VER_NUM=$(ROM_VIF) \
		-DFIRM_VER_DATE=$(MY_DATE) \
		-DPARTHD_MAGIC=0xa324eb90 \
		-DINCBIN=$(call cygpath,$<) \
		-DOIMG_LEN=$(word 2,$(shell $(AMBA_HOST_CRC) $<.tmp)) \
		-DFIRM_FLAG=$(FW_FLAG) \
		-c $(call cygpath,$(word 2,$^)) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/rom_fw.bin
else
$(FWPROG_OUT_DIR)/rom.fw: $(src)/piggy_rom.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) -c $(call cygpath,$<) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/rom_fw.bin
endif

ifneq ($(LNX),)
# Note: $(LNX) is full path
$(FWPROG_OUT_DIR)/lnx.fw: $(LNX) $(src)/piggy_lnx.S $(FWPROG_OUT_DIR)/dtb.fw
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) \
		-DCRC32=$(word 3,$(shell $(AMBA_HOST_CRC) $<)) \
		-DIMG_LEN=$(word 2,$(shell $(AMBA_HOST_CRC) $<)) \
		-DFIRM_VER_NUM=$(LNX_VIF) \
		-DFIRM_VER_DATE=$(MY_DATE) \
		-DFIRM_MEM_ADDR=$(CONFIG_LNX_LOADADDR) \
		-DPARTHD_MAGIC=0xa324eb90 \
		-DINCBIN=$(call cygpath,$<) \
		-c $(call cygpath,$(word 2,$^)) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/lnx.bin
else
$(FWPROG_OUT_DIR)/lnx.fw: $(src)/piggy_lnx.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) -c $(call cygpath,$<) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/lnx.bin
endif

ifneq ($(LNX),)
# Note: $(DTB) is full path
$(FWPROG_OUT_DIR)/dtb.fw: $(DTB) $(src)/piggy_dtb.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) \
		-DCRC32=$(word 3,$(shell $(AMBA_HOST_CRC) $<)) \
		-DIMG_LEN=$(word 2,$(shell $(AMBA_HOST_CRC) $<)) \
		-DFIRM_VER_NUM=$(LNX_VIF) \
		-DFIRM_VER_DATE=$(MY_DATE) \
		-DFIRM_MEM_ADDR=$(CONFIG_LNX_LOADADDR) \
		-DPARTHD_MAGIC=0xa324eb90 \
		-DINCBIN=$(call cygpath,$<) \
		-c $(call cygpath,$(word 2,$^)) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/dtb.bin
else
$(FWPROG_OUT_DIR)/dtb.fw: $(src)/piggy_dtb.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) -c $(call cygpath,$<) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/dtb.bin
endif

ifneq ($(RFS),)
# Note: $(RFS) is full path
$(FWPROG_OUT_DIR)/rfs.fw: $(RFS) $(src)/piggy_rfs.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) \
		-DCRC32=$(word 3,$(shell $(AMBA_HOST_CRC) $<)) \
		-DIMG_LEN=$(word 2,$(shell $(AMBA_HOST_CRC) $<)) \
		-DFIRM_VER_NUM=$(RFS_VIF) \
		-DFIRM_VER_DATE=$(MY_DATE) \
		-DPARTHD_MAGIC=0xa324eb90 \
		-DINCBIN=$(call cygpath,$<) \
		-c $(call cygpath,$(word 2,$^)) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/rfs.bin
else
$(FWPROG_OUT_DIR)/rfs.fw: $(src)/piggy_rfs.S
	@mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) -c $(call cygpath,$<) -o $@
	$(Q)$(OBJCOPY) -j .text -O elf32-littlearm $@ $@
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $@ $(FWPROG_OUT_DIR)/rfs.bin
endif
# ------------------------------------------------------------------ #
clean-dirs	:= $(DIR_amba_fwprog_out)
install-files	:= libfwprog.a
