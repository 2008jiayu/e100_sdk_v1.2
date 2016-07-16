ifneq ($(wildcard $(srctree)/$(src)/host_romfs_gen),)
obj-$(CONFIG_BUILD_HOST_ROMFS_GEN)	+= host_romfs_gen/
endif

ifeq ($(CONFIG_BUILD_HOST_CRC),y)
host-output-files += host_crc32
endif

ifeq ($(CONFIG_BUILD_HOST_PERMUTATE),y)
host-output-files += host_permutate
endif

ifeq ($(CONFIG_BUILD_HOST_SDFW_GEN),y)
host-output-files += host_sdfw_gen
endif

hostprogs-y	:= $(host-output-files)

always		:= $(hostprogs-y)

HOST_EXTRACFLAGS += -m32 -I$(srctree)/$(BSP_DIR) -I$(srctree)/vendors/ambarella/inc/ssp -I$(objtree)/build/include/generated

install-files := $(host-output-files)
remove-installed-files := $(host-output-files)

ifeq ($(HOST_OS), CYGWIN)
install-path  := $(call cygpath,$(objtree))/tools/exec/win
clean-files += host_crc32.exe
clean-files += host_permutate.exe
clean-files += host_sdfw_gen.exe
else
install-path  := $(objtree)/tools/exec/lnx
clean-files += host_crc32
clean-files += host_permutate
clean-files += host_sdfw_gen
endif
