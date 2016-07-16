config BUILD_SSP_PRFILE2_EXFAT
        bool "Build PrFILE2 ExFAT"
        default y

config ENABLE_EXFAT_SUPPORT
	bool "Enable EXFAT SUPPORT"
	default y
	depends on PF_EXFAT_SUPPORT

if BUILD_SSP_PRFILE2_EXFAT
source ssp/Kconfig.prfile2_exfat
endif
