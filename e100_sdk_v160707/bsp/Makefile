
ifdef CONFIG_BSP_DRAGONFLY
BSP_PATH = dragonfly
endif
ifdef CONFIG_BSP_TAROKO
BSP_PATH = taroko
endif
ifdef CONFIG_BSP_LIBRA
BSP_PATH = libra
endif
ifdef CONFIG_BSP_BUB
BSP_PATH = bub
endif
ifdef CONFIG_BSP_ARIES
BSP_PATH = aries
endif
ifdef CONFIG_BSP_ORTHRUS
BSP_PATH = orthrus
endif
ifdef CONFIG_BSP_GOAT
BSP_PATH = goat
endif

obj-y += libbsp.a
libbsp-objs := $(BSP_PATH)/AmbaLinkMemoryCfg.o \
         $(BSP_PATH)/AmbaMmuMemMap.o \
         $(BSP_PATH)/bsp.o \
         $(BSP_PATH)/buttons.o

libbsp-objs += AmbaVer.o
CFLAGS_AmbaVer.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
           -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
           -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
           -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
           -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
           -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y := -I$(srctree)/vendors/ambarella/inc/ssp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/link
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/link/rpcprog
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/soc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/threadx
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/rpcprog
ccflags-y += -I$(srctree)/peripheral/misc/inc
ccflags-y += -I$(srctree)/peripheral/audio/inc
ifdef CONFIG_BSP_LIBRA
ccflags-y += -I$(srctree)/peripheral/sensor/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/dsp
endif
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaSys
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/misc

install-files := libbsp.a

