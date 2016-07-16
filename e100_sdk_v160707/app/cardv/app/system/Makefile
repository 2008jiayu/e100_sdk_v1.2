
libapp-objs +=system/app_pref.o
libapp-objs +=system/app_util.o
libapp-objs +=system/handler.o
libapp-objs +=system/t_app.o

ifeq ($(CONFIG_APP_ARD),y)
AMBA_APP_BUILD_DATE_NUM = $(shell date '+%y%W%u')
CFLAGS_app_pref.o = -DAMBA_APP_BUILD_DATE_NUM=$(AMBA_APP_BUILD_DATE_NUM)
endif

ifeq ($(CONFIG_APP_AMBA_LINK),y)
libapp-objs +=system/onlinePlayback.o
endif
