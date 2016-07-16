obj-y += libamboot_bld.a

# Link script
obj-y += amba_bld.lds

ifeq ($(CONFIG_ENABLE_EMMC_BOOT), y) 
libamboot_bld-objs := AmbaNonOS_EmmcShell.o
else ifeq ($(CONFIG_ENABLE_SPINOR_BOOT), y) 
libamboot_bld-objs := AmbaNonOS_NorShell.o
else
libamboot_bld-objs := AmbaNonOS_Shell.o
endif
libamboot_bld-objs += AmbaVer.o
CFLAGS_AmbaVer.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
		   -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
		   -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
		   -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
		   -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
		   -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y :=    -I$(srctree)/vendors/ambarella/inc/ssp
ccflags-y +=    -I$(srctree)/vendors/ambarella/inc/ssp/soc
ccflags-y +=    -I$(srctree)/vendors/ambarella/inc/ssp/soc/non_os
ccflags-y +=    -I$(srctree)/vendors/ambarella/inc/comsvc/misc

install-files := libamboot_bld.a
