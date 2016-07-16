
ifneq ($(wildcard $(srctree)/$(src)/cfs),)
obj-$(CONFIG_BUILD_MW_CFS) += cfs/
endif

ifneq ($(wildcard $(srctree)/$(src)/dataflow),)
obj-$(CONFIG_BUILD_MW_SERVICE) += dataflow/
endif

ifneq ($(wildcard $(srctree)/$(src)/dspflow),)
obj-$(CONFIG_BUILD_MW_FLOW) += dspflow/
endif

ifneq ($(wildcard $(srctree)/$(src)/net),)
obj-$(CONFIG_BUILD_MW_NET) += net/
endif

ifneq ($(wildcard $(srctree)/$(src)/unittest),)
obj-$(CONFIG_BUILD_MW_UNITTEST) += unittest/
endif
