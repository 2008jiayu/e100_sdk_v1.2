libapplib-objs += src/AmbaVer_SetAppLib.o

APPLIB_AMBA_CI_COUNT       =    $(call git_ci_count_cmd,$(srctree)/app/cardv/applib,$(srctree)/app/cardv/applib)
APPLIB_AMBA_CI_ID          =    $(call git_ci_id_cmd,$(srctree)/app/cardv/applib,$(srctree)/app/cardv/applib)

CFLAGS_AmbaVer_SetAppLib.o =-DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)        \
                        -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM)    \
                        -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)      \
                        -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)      \
                        -DAMBA_CI_COUNT=$(APPLIB_AMBA_CI_COUNT)         \
                        -DAMBA_CI_ID=$(APPLIB_AMBA_CI_ID)

include $(srctree)/app/cardv/applib/src/3a/Makefile
include $(srctree)/app/cardv/applib/src/ambalink_privrpc/Makefile
include $(srctree)/app/cardv/applib/src/ambalink_privsvc/Makefile
include $(srctree)/app/cardv/applib/src/ambalink_pubsvc/Makefile
include $(srctree)/app/cardv/applib/src/audio/Makefile
include $(srctree)/app/cardv/applib/src/calibration/Makefile
include $(srctree)/app/cardv/applib/src/comsvc/Makefile
include $(srctree)/app/cardv/applib/src/dcf/Makefile
include $(srctree)/app/cardv/applib/src/display/Makefile
include $(srctree)/app/cardv/applib/src/editor/Makefile
include $(srctree)/app/cardv/applib/src/format/Makefile
include $(srctree)/app/cardv/applib/src/graphics/Makefile
include $(srctree)/app/cardv/applib/src/image/Makefile
include $(srctree)/app/cardv/applib/src/index/Makefile
include $(srctree)/app/cardv/applib/src/monitor/Makefile
include $(srctree)/app/cardv/applib/src/player/Makefile
include $(srctree)/app/cardv/applib/src/recorder/Makefile
include $(srctree)/app/cardv/applib/src/storage/Makefile
include $(srctree)/app/cardv/applib/src/stream/Makefile
include $(srctree)/app/cardv/applib/src/system/Makefile
include $(srctree)/app/cardv/applib/src/thirdparty/Makefile
include $(srctree)/app/cardv/applib/src/tune/Makefile
include $(srctree)/app/cardv/applib/src/usb/Makefile
include $(srctree)/app/cardv/applib/src/utility/Makefile
include $(srctree)/app/cardv/applib/src/va/Makefile

ifeq ($(CONFIG_APP_AMBA_LINK),y)
include $(srctree)/app/cardv/applib/src/net/Makefile
include $(srctree)/app/cardv/applib/src/transcoder/Makefile
endif
