obj-y += libcomsvc_tune.a

libcomsvc_tune-objs := AmbaVer_comsvctune.o

$(obj)/AmbaVer_comsvctune.o	:	$(call amba_ver_dep,AmbaVer_comsvctune.o,$(libcomsvc_tune-objs))
CFLAGS_AmbaVer_comsvctune.o	=	-DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)		\
						-DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM)	\
						-DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)		\
						-DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)		\
						-DAMBA_CI_COUNT=$(AMBA_CI_COUNT)			\
						-DAMBA_CI_ID=$(AMBA_CI_ID)
libcomsvc_tune-objs += AmbaTUNE_HdlrManager.o
libcomsvc_tune-objs += AmbaTUNE_TextHdlr.o
libcomsvc_tune-objs += AmbaTUNE_USBHdlr.o

ccflags-y := -I$(srctree)/vendors/ambarella/inc/ssp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/link
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/misc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaSys
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/soc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/image/ituner
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/fs
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/usb
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/prfile2
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/tune
ccflags-y += -I$(srctree)/vendors/ambarella/inc/image/imgproc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/image/imgschdlr
ccflags-y += -I$(srctree)/peripheral/sensor/inc

install-files := libcomsvc_tune.a
