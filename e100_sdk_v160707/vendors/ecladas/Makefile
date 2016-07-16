obj-y += libecladas.a

ifeq ($(CONFIG_BUILD_ECL_ADAS),y)
ccflags-y := \
    -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaSys \
    -I$(srctree)/vendors/ambarella/inc/comsvc/misc \
    -I$(srctree)/vendors/ambarella/inc/mw \
    -I$(srctree)/vendors/ambarella/inc/ssp/prfile2 \
    -I$(srctree)/vendors/ambarella/inc/ssp/fs \
    -I$(srctree)/vendors/ambarella/inc/ssp/dsp \
    -I$(srctree)/vendors/ambarella/inc/ssp/soc \
    -I$(srctree)/vendors/ambarella/inc/ssp	\
    -I$(srctree)/peripheral/sensor/inc	\
    -I$(srctree)/vendors/ecladas \
    -I$(srctree)/vendors/ecladas/include \
    -I$(srctree)/vendors/ecladas/adas_source/math \
    -I$(srctree)/vendors/ecladas/adas_source \
    -I$(srctree)/vendors/ecladas/adas_source/include \
    -I$(srctree)/vendors/ecladas/adas_source/show \
    -I$(srctree)/vendors/ecladas/adas_source/math/adas \
    -I$(srctree)/vendors/lib \

libecladas-objs += \
	XS508PZYKJA.o \
	X508_IicTestDrv.o \
	ecl_adas_proc.o \
	adas_source/data/k_comm.o \
	adas_source/show/k_draw.o \
	adas_source/show/k_dc.o \
	adas_source/show/k_font.o \
	adas_source/math/k_math.o \
	adas_source/math/k_matrix.o \
	adas_source/math/k_geometry.o \
	adas_source/math/k_fuzzy.o \
	adas_source/math/adas/k_edge.o \
	adas_source/math/adas/k_haar.o \
	adas_source/math/adas/k_lane.o \
	adas_source/math/adas/k_object.o \
	adas_source/math/adas/k_adas.o\

CFLAGS_AmbaVer.o = \
	-DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE) \
	-DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
	-DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM) \
	-DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE) \
	-DAMBA_CI_COUNT=$(AMBA_CI_COUNT) \
	-DAMBA_CI_ID=$(AMBA_CI_ID)
	
install-files := libecladas.a
install-path :=$(srctree)/vendors/ambarella/lib
endif
