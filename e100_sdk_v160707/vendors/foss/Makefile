ifeq ($(CONFIG_BUILD_FOSS_LIB),y)

obj-y += libfoss.a

libfoss-objs := AmbaVer.o

ifeq ($(CONFIG_BUILD_FOSS_FREETYPE),y)
# Belows are FreeType
libfoss-objs += \
	freetype/freetype-2.4.11/src/psnames/psnames.o \
	freetype/freetype-2.4.11/src/sfnt/sfnt.o \
	freetype/freetype-2.4.11/src/base/ftbase.o \
	freetype/freetype-2.4.11/src/base/ftdebug.o \
	freetype/freetype-2.4.11/src/base/ftinit.o \
	freetype/freetype-2.4.11/src/base/ftsynth.o \
	freetype/freetype-2.4.11/src/base/ftsystem.o \
	freetype/freetype-2.4.11/src/raster/raster.o \
	freetype/freetype-2.4.11/src/smooth/smooth.o \
	freetype/freetype-2.4.11/src/truetype/truetype.o \
    freetype/AmbaStdCLib.o

endif

ifeq ($(CONFIG_BUILD_FOSS_GPSD),y)
# Belows are GPS Daemon
libfoss-objs += \
	gps/gps_lib.o \
	gps/gps_parser.o

endif

ifeq ($(CONFIG_BUILD_FOSS_OPENCV),y)
obj-y += opencv/
endif

ifeq ($(CONFIG_BUILD_FOSS_LIBJPEG_TURBO),y)
obj-y += libjpeg-turbo/
endif

CFLAGS_AmbaVer.o = \
	-DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE) \
	-DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
	-DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM) \
	-DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE) \
	-DAMBA_CI_COUNT=$(AMBA_CI_COUNT) \
	-DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y := \
    -I$(kbuild-dir)/freetype \
    -I$(kbuild-dir)/freetype/freetype-2.4.11/include \
    -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaSys \
    -I$(srctree)/vendors/ambarella/inc/comsvc/misc \
    -I$(srctree)/vendors/ambarella/inc/ssp/prfile2 \
    -I$(srctree)/vendors/ambarella/inc/ssp/fs \
    -I$(srctree)/vendors/ambarella/inc/ssp/soc \
    -I$(srctree)/vendors/ambarella/inc/ssp	\
    -I$(srctree)/vendors/ambarella/vendors/foss/gps

install-files := libfoss.a

endif
