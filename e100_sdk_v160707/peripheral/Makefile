obj-y += libperipheral.a

libperipheral-objs += AmbaVer.o
CFLAGS_AmbaVer.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
		   -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
		   -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
		   -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
		   -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
		   -DAMBA_CI_ID=$(AMBA_CI_ID)

include $(src)/dram/Makefile.dram
include $(src)/sbrg/Makefile.sbrg
include $(src)/sensor/Makefile.sensor
include $(src)/yuvinput/Makefile.yuvinput
include $(src)/audio/Makefile.audio
include $(src)/lcd/Makefile.lcd
include $(src)/imu/Makefile.imu
include $(src)/nand/Makefile.nand
include $(src)/nor/Makefile.nor
include $(src)/emmc/Makefile.emmc
include $(src)/misc/Makefile.misc
include $(src)/gps/Makefile.gps
include $(src)/g_sensor/Makefile.g_sensor

ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/soc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/threadx
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaSys
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/misc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaTest
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaShell

install-files := libperipheral.a

