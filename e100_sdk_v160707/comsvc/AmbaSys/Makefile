obj-y += libcomsvc_sys.a

libcomsvc_sys-objs := \
		AmbaExceptionHandler.o \
		AmbaFwLoader.o \
		AmbaMemcpy_16to32.o \
		AmbaMemcpy_32to16.o \
		AmbaMemcpy_32to32.o \
		AmbaMemcpy_32to8.o \
		AmbaMemcpy_8to32.o \
	 	AmbaPrintk.o \
		AmbaRootTask.o \
		AmbaUnZIP.o \
		AmbaUtility.o \
		decompress.o

ifdef CONFIG_CC_CXX_SUPPORT
libcomsvc_sys-objs += AmbaCtorDtor.o
endif


libcomsvc_sys-objs += AmbaVer.o
CFLAGS_AmbaVer.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE) \
		   -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
		   -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
		   -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
		   -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
		   -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y := -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaSys \
		 -I$(srctree)/vendors/ambarella/inc/ssp/soc \
		 -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaShell \
		 -I$(srctree)/vendors/ambarella/inc/ssp/dsp \
	     -I$(srctree)/vendors/ambarella/inc/ssp/	\
		 -I$(srctree)/vendors/ambarella/inc/ssp/fs \
	     -I$(srctree)/vendors/ambarella/inc/comsvc/misc

install-files := libcomsvc_sys.a
