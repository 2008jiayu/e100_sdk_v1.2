obj-y += libcomsvc_image.a
libcomsvc_image-objs := AmbaVer_comsvcimage.o


$(obj)/AmbaVer_comsvcimage.o	:	$(call amba_ver_dep,AmbaVer_comsvcimage.o,$(libcomsvc_image-objs))
CFLAGS_AmbaVer_comsvcimage.o	=	-DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)		\
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

install-files := libcomsvc_image.a
