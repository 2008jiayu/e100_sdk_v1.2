ifneq ($(wildcard $(srctree)/$(src)/imgschdlr),)
obj-$(CONFIG_IMG_SCHEDULER) += imgschdlr/
endif

ifneq ($(wildcard $(srctree)/$(src)/imgcalib),)
obj-$(CONFIG_IMG_CALIBRATION) += imgcalib/
endif

ifneq ($(wildcard $(srctree)/$(src)/imgproc),)
obj-$(CONFIG_IMG_PROC) += imgproc/
endif

ifneq ($(wildcard $(srctree)/$(src)/ituner),)
obj-$(CONFIG_IMG_ITUNER) += ituner/
endif

ifneq ($(wildcard $(srctree)/$(src)/utils),)
obj-$(CONFIG_IMG_UTILS) += utils/
endif

ifneq ($(wildcard $(srctree)/$(src)/encmonitor),)
obj-$(CONFIG_IMG_ENCMONITOR) += encmonitor/
endif