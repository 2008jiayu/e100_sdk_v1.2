obj-y += libcomsvc_fwupdate.a

# Belows are AmbaCSL
libcomsvc_fwupdate-objs := \
	AmbaFwUpdaterSD.o	

libcomsvc_fwupdate-objs += AmbaVer.o
CFLAGS_AmbaVer.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
		   -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
		   -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
		   -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
		   -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
		   -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y := -I$(srctree)/vendors/ambarella/inc/ssp \
             -I$(srctree)/vendors/ambarella/inc/ssp/soc \
             -I$(srctree)/vendors/ambarella/inc/ssp/kal \
             -I$(srctree)/vendors/ambarella/inc/ssp/dsp \
             -I$(srctree)/vendors/ambarella/inc/ssp/fs \
             -I$(srctree)/vendors/ambarella/inc/ssp/dsp \
	     -I$(srctree)/vendors/ambarella/inc/comsvc/misc \
	     -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaSys \

install-files := libcomsvc_fwupdate.a
