obj-y += libcomsvc_test.a


libcomsvc_test-objs := AmbaTest.o
libcomsvc_test-objs += AmbaTest_Fio.o
libcomsvc_test-objs += AmbaTest_PIO.o

ifdef CONFIG_CC_CXX_SUPPORT
# For __dso_handle define and test case
libcomsvc_test-objs += AmbaTest_cpp.o
# For CPP test case
libcomsvc_test-objs += AmbaTest_cpp_case.o
endif

libcomsvc_test-objs += AmbaVer.o
CFLAGS_AmbaVer.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
		   -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
		   -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
		   -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
		   -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
		   -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y := -I${srctree}/vendors/ambarella/inc/comsvc/AmbaTest \
	     -I$(srctree)/vendors/ambarella/inc/ssp \
	     -I$(srctree)/vendors/ambarella/inc/ssp/soc \
	     -I$(srctree)/vendors/ambarella/inc/ssp/soc/MW \
	     -I$(srctree)/vendors/ambarella/inc/ssp/soc/RTSL \
	     -I$(srctree)/vendors/ambarella/inc/ssp/fs \
	     -I$(srctree)/vendors/ambarella/inc/ssp/dsp \
	     -I$(srctree)/vendors/ambarella/inc/ssp/kal \
	     -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaShell \
	     -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaSys \
	     -I$(srctree)/vendors/ambarella/inc/comsvc/misc

install-files := libcomsvc_test.a
