host-output-files += host_romfs_gen

hostprogs-y	:= $(host-output-files)

always		:= $(hostprogs-y)

host_romfs_gen-objs := AmbaRomGen.o \
			AmbaRomGen.tab.o \
    			AmbaRomGen.yy.o

HOST_EXTRACFLAGS += -m32

HOST_EXTRACFLAGS += -I$(srctree)/vendors/ambarella/inc/ssp		\
		    -I$(srctree)/vendors/ambarella/inc/ssp/soc		\
		    -I$(srctree)/vendors/ambarella/inc/ssp/threadx

HOSTLDFLAGS += -m32

install-files := $(host-output-files)
ifeq ($(HOST_OS), CYGWIN)
	install-path  := $(call cygpath,$(objtree))/tools/exec/win
else
	install-path  := $(objtree)/tools/exec/lnx
endif
