ifneq ($(wildcard $(srctree)/$(src)/amboot/bld),)
obj-$(CONFIG_BUILD_SSP_AMBOOT) += amboot/bld/
endif

ifneq ($(wildcard $(srctree)/$(src)/amboot/bst),)
obj-$(CONFIG_BUILD_SSP_AMBOOT) += amboot/bst/
endif

ifneq ($(wildcard $(srctree)/$(src)/dsp),)
obj-$(CONFIG_BUILD_SSP_DSP) += dsp/
endif

ifneq ($(wildcard $(srctree)/$(src)/fs),)
obj-$(CONFIG_BUILD_SSP_FS) += fs/
endif

ifneq ($(wildcard $(srctree)/$(src)/fwprog),)
obj-$(CONFIG_BUILD_SSP_FWPROG) += fwprog/
endif

ifneq ($(wildcard $(srctree)/$(src)/kal),)
obj-$(CONFIG_BUILD_SSP_KAL) += kal/
endif

ifneq ($(wildcard $(srctree)/$(src)/link),)
obj-$(CONFIG_BUILD_SSP_LINK) += link/
endif

ifneq ($(wildcard $(srctree)/$(src)/soc/Makefile),)
obj-$(CONFIG_BUILD_SSP_SOC) += soc/
endif

ifneq ($(wildcard $(srctree)/$(src)/svc),)
obj-$(CONFIG_BUILD_SSP_SVC) += svc/
endif

ifneq ($(wildcard $(srctree)/$(src)/usb),)
obj-$(CONFIG_BUILD_SSP_USB) += usb/
endif

ifneq ($(wildcard $(srctree)/$(src)/unittest),)
obj-$(CONFIG_BUILD_SSP_UNITTEST) += unittest/
endif
