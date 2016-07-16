obj-y += libcomsvc_misc.a
libcomsvc_misc-objs := math.o msgqueue.o ptrlist.o util.o \
                       firmwareloader.o ucodeloader.o \
		       wchar.o isr.o syncsvc.o AmbaVerImpl.o AmbaVer.o AmbaMonitor_SD.o \
		       v7_pmu.o

#ifneq ($(findstring 4.7,$(CC_VER)),)
# Patches to newlib for aeabi_memcpyXXX, aeabi_memclr, aeabi_memmove, aeabi_memset
libcomsvc_misc-objs += \
	aeabi_mem/aeabi_memclr.o \
	aeabi_mem/aeabi_memcpy-armv7a.o \
	aeabi_mem/aeabi_memcpy.o \
	aeabi_mem/aeabi_memmove.o \
	aeabi_mem/aeabi_memset.o \
	memcpy.o
#endif


$(obj)/AmbaVer.o	:	$(call amba_ver_dep,AmbaVer.o,$(libcomsvc_misc-objs))
CFLAGS_AmbaVer.o	=	-DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)		\
						-DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM)	\
						-DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)		\
						-DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)		\
						-DAMBA_CI_COUNT=$(AMBA_CI_COUNT)			\
						-DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y := -I$(srctree)/vendors/ambarella/inc/ssp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/link
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/soc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/threadx
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/fs
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/prfile2
ccflags-y += -I$(srctree)/vendors/ambarella/inc/mw
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/misc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaSys

install-files := libcomsvc_misc.a
