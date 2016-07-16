obj-y += libapp.a
# Link script

ccflags-y := -I$(srctree)/vendors/ambarella/inc/ssp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/link
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/link/rpcprog
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/soc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/threadx
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/rpcprog
ccflags-y += -I$(srctree)/vendors/foss/gps
ccflags-y += -I$(srctree)/peripheral/misc/inc
ccflags-y += -I$(srctree)/peripheral/sensor/inc
ccflags-y += -I$(srctree)/peripheral/lcd/inc
ccflags-y += -I$(srctree)/peripheral/imu/inc
ccflags-y += -I$(srctree)/peripheral/audio/inc
ccflags-y += -I$(srctree)/peripheral/yuvinput/inc
ccflags-y += -I$(srctree)/peripheral/gps/inc
ccflags-y += -I$(srctree)/peripheral/g_sensor/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaSys
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaShell
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaTest
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/misc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/soc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/fs
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/prfile2
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/threadx
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/rpcprog
ccflags-y += -I$(srctree)/vendors/ambarella/inc/mw
ccflags-y += -I$(srctree)/vendors/ambarella/inc/mw/net/rpcprog
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/image
ccflags-y += -I$(srctree)/app/cardv/applib/inc
ccflags-y += -I$(srctree)/app/cardv/app
ccflags-y += -I$(srctree)/vendors/ecladas/

libapp-objs := AmbaVer_SetApp.o

APP_AMBA_CI_COUNT       =   $(call git_ci_count_cmd,$(srctree)/app/cardv/app,$(srctree)/app/cardv/app)
APP_AMBA_CI_ID          =   $(call git_ci_id_cmd,$(srctree)/app/cardv/app,$(srctree)/app/cardv/app)

CFLAGS_AmbaVer_SetApp.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)    \
                  -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM)    \
                  -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)      \
                  -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)      \
                  -DAMBA_CI_COUNT=$(APP_AMBA_CI_COUNT)            \
                  -DAMBA_CI_ID=$(APP_AMBA_CI_ID)

include $(srctree)/app/cardv/app/apps/Makefile
include $(srctree)/app/cardv/app/peripheral_mod/Makefile
include $(srctree)/app/cardv/app/framework/Makefile
include $(srctree)/app/cardv/app/system/Makefile
include $(srctree)/app/cardv/app/thirdparty/Makefile

install-files := libapp.a
