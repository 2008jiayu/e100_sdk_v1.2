obj-y += libcomsvc_shell.a

# Belows are AmbaCSL
libcomsvc_shell-objs := \
	AmbaShell_Cardmgr.o \
	AmbaShell_Cat.o \
	AmbaShell_Chdir.o \
	AmbaShell_Chmod.o \
	AmbaShell_Cleandir.o \
	AmbaShell_Copy.o \
	AmbaShell_Date.o \
	AmbaShell_Dmesg.o \
	AmbaShell_Echo.o \
	AmbaShell_Format.o \
	AmbaShell_FwProg.o \
	AmbaShell_Help.o \
	AmbaShell_Hotboot.o \
	AmbaShell_Lexer.o \
	AmbaShell_List.o \
	AmbaShell_Ls.o \
	AmbaShell_Mkdir.o \
	AmbaShell_Move.o \
	AmbaShell_Nftl.o \
	AmbaShell_Priv.o \
	AmbaShell_Ps.o \
	AmbaShell_Pwd.o \
	AmbaShell_Reboot.o \
	AmbaShell_Poweroff.o \
	AmbaShell_Rm.o \
	AmbaShell_Rw.o \
	AmbaShell_Savebin.o \
	AmbaShell_Sleep.o \
	AmbaShell_Suspend.o \
	AmbaShell_Tab.o \
	AmbaShell_True.o \
	AmbaShell_Utility.o \
	AmbaShell.o \
	AmbaShell_Vol.o	\
	AmbaShell_Ver.o

libcomsvc_shell-objs += AmbaVer.o
CFLAGS_AmbaVer.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
		   -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
		   -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
		   -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
		   -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
		   -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y := -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaShell \
		 -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaSys \
	     -I$(srctree)/vendors/ambarella/inc/comsvc/misc \
	     -I$(srctree)/vendors/ambarella/inc/ssp/prfile \
	     -I$(srctree)/vendors/ambarella/inc/ssp/fs \
	     -I$(srctree)/vendors/ambarella/inc/ssp/soc \
	     -I$(srctree)/vendors/ambarella/inc/ssp/

install-files := libcomsvc_shell.a
