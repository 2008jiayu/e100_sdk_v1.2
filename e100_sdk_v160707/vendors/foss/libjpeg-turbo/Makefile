obj-y += libjpeg-turbo.a
obj-y += libturbojpeg.a

ccflags-y := -I$(srctree)/vendors/ambarella/inc/ssp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/link
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/soc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/threadx
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/rpcprog
ccflags-y += -I$(srctree)/peripheral/inc/misc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaSys
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/soc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/fs
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/prfile
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/threadx
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/rpcprog
ccflags-y += -I$(srctree)/vendors/ambarella/inc/mw
ccflags-y += -I$(srctree)/vendors/ambarella/inc/mw/net/rpcprog
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/misc
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/peripheral/sensor/inc
ccflags-y += -I$(srctree)/peripheral/lcd/inc
ccflags-y += -I$(src)/index
ccflags-y += -I$(src)
ccflags-y += -D__ARM_NEON__

libjpeg-turbo-objs := simd/jsimd_arm_neon.o
libjpeg-turbo-objs += jcapimin.o jcapistd.o jccoefct.o jccolor.o jcdctmgr.o 
libjpeg-turbo-objs += jchuff.o jcinit.o jcmainct.o jcmarker.o jcmaster.o
libjpeg-turbo-objs += jcomapi.o jcparam.o jcphuff.o jcprepct.o jcsample.o
libjpeg-turbo-objs += jctrans.o jdapimin.o jdapistd.o jdatadst.o jdatasrc.o
libjpeg-turbo-objs += jdcoefct.o jdcolor.o jddctmgr.o jdhuff.o jdinput.o
libjpeg-turbo-objs += jdmainct.o jdmarker.o jdmaster.o jdmerge.o jdphuff.o
libjpeg-turbo-objs += jdpostct.o jdsample.o jdtrans.o jerror.o jfdctflt.o
libjpeg-turbo-objs += jfdctfst.o jfdctint.o jidctflt.o jidctfst.o jidctint.o
libjpeg-turbo-objs += jidctred.o jquant1.o jquant2.o jutils.o jmemmgr.o
libjpeg-turbo-objs += jmemnobs.o jaricom.o jcarith.o jdarith.o
libjpeg-turbo-objs += simd/jsimd_arm.o


libturbojpeg-objs := turbojpeg.o transupp.o jdatadst-tj.o jdatasrc-tj.o 

install-files := libjpeg-turbo.a libturbojpeg.a
