#
# kbuild is modified from Linux-3.14.18
#

# *DOCUMENTATION*
# To see a list of typical targets execute "make help"
# More info can be located in ./README
# Comments in this file are targeted only to the developer, do not
# expect to learn how to build the kernel reading this file.

# Do not:
# o  use make's built-in rules and variables
#    (this increases performance and avoids hard-to-debug behaviour);
# o  print "Entering directory ...";
MAKEFLAGS += -rR --no-print-directory

# Avoid funny character set dependencies
unexport LC_ALL
LC_COLLATE=C
LC_NUMERIC=C
export LC_COLLATE LC_NUMERIC

# Avoid interference with shell env settings
unexport GREP_OPTIONS

# MingGW does not support mmap()
HOST_OS = $(shell (uname -s | sed -e 's/CYGWIN.*/CYGWIN/'))
HOST_OS := $(patsubst CYGWIN%,CYGWIN,$(HOST_OS))
export HOST_OS

srctree		:= $(if $(KBUILD_SRC),$(KBUILD_SRC),$(CURDIR))

ifeq ($(HOST_OS),CYGWIN)
# Window path of drive: d:
HOST_DRV_W := $(shell cygpath -wa $(CURDIR) | awk -F ":" '{print $$1}'):/

# Mixed path of drive: /cygdrive/d
HOST_DRV_U := $(patsubst %,/cygdrive/%,$(subst :,,$(HOST_DRV_W)))

export HOST_DRV_U HOST_DRV_W

# Use the path include drive
srctree		:= $(patsubst %,/cygdrive/%,$(subst :,,$(subst \,/,$(shell cygpath -wa $(srctree)))))
objtree		:= $(patsubst %,/cygdrive/%,$(subst :,,$(subst \,/,$(shell cygpath -wa $(CURDIR)))))

else
# HOST_OS is Linux
objtree		:= $(CURDIR)

endif # HOST_OS

# We are using a recursive build, so we need to do a little thinking
# to get the ordering right.
#
# Most importantly: sub-Makefiles should only ever modify files in
# their own directory. If in some directory we have a dependency on
# a file in another dir (which doesn't happen often, but it's often
# unavoidable when linking the built-in.o targets which finally
# turn into vmlinux), we will call a sub make in that other dir, and
# after that we are sure that everything which is in that other dir
# is now up to date.
#
# The only cases where we need to modify files which have global
# effects are thus separated out and done before the recursive
# descending is started. They are now explicitly listed as the
# prepare rule.

# To put more focus on warnings, be less verbose as default
# Use 'make V=1' to see the full commands

ifeq ("$(origin V)", "command line")
  KBUILD_VERBOSE = $(V)
endif
ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE = 0
endif

# To install or not install files listed in "install-files" when "make folder-install"
# I=0 to not install. Default value is I=1
ifeq ("$(origin I)", "command line")
  KBUILD_INSTALL = $(I)
endif
ifndef KBUILD_INSTALL
  KBUILD_INSTALL = 1
endif
export KBUILD_INSTALL

# Call a source code checker (by default, "sparse") as part of the
# C compilation.
#
# Use 'make C=1' to enable checking of only re-compiled files.
# Use 'make C=2' to enable checking of *all* source files, regardless
# of whether they are re-compiled or not.
#
# See the file "Documentation/sparse.txt" for more details, including
# where to get the "sparse" utility.

ifeq ("$(origin C)", "command line")
  KBUILD_CHECKSRC = $(C)
endif
ifndef KBUILD_CHECKSRC
  KBUILD_CHECKSRC = 0
endif

# NOT used, Use make M=dir to specify directory of external module to build
KBUILD_EXTMOD :=

# kbuild supports saving output files in a separate directory.
# To locate output files in a separate directory two syntaxes are supported.
# In both cases the working directory must be the root of the kernel src.
# 1) O=
# Use "make O=dir/to/store/output/files/"
#
# 2) Set KBUILD_OUTPUT
# Set the environment variable KBUILD_OUTPUT to point to the directory
# where the output files shall be placed.
# export KBUILD_OUTPUT=dir/to/store/output/files/
# make
#
# The O= assignment takes precedence over the KBUILD_OUTPUT environment
# variable.

# KBUILD_SRC is set on invocation of make in OBJ directory
# KBUILD_SRC is not intended to be used by the regular user (for now)
ifeq ($(KBUILD_SRC),)

# OK, Make called in directory where kernel src resides
# Do we want to locate output files in a separate directory?
ifeq ("$(origin O)", "undefined")
  # Force to use O
  O := output
  export O
  KBUILD_OUTPUT := $(O)
  $(if $(findstring clean,$(MAKECMDGOALS)),,$(shell mkdir -p $(O)))
else ifeq ("$(origin O)", "command line")
  # Empty, i.e. O=, to disable out-of-source building.
  ifneq ("$(O)", "")
    KBUILD_OUTPUT := $(O)
    $(if $(findstring clean,$(MAKECMDGOALS)),,$(shell mkdir -p $(O)))
  endif
else
  $(warning $(origin O) is not handled!)
endif

ifeq ("$(origin W)", "command line")
  export KBUILD_ENABLE_EXTRA_GCC_CHECKS := $(W)
endif

# That's our default target when none is given on the command line
PHONY := _all
_all:

# Cancel implicit rules on top Makefile
$(CURDIR)/Makefile Makefile: ;

ifneq ($(KBUILD_OUTPUT),)
# Invoke a second make in the output directory, passing relevant variables
# check that the output directory actually exists
saved-output := $(KBUILD_OUTPUT)
KBUILD_OUTPUT := $(shell cd $(KBUILD_OUTPUT) && /bin/pwd)
$(if $(KBUILD_OUTPUT),, \
     $(error output directory "$(saved-output)" does not exist))

PHONY += $(MAKECMDGOALS) sub-make

$(filter-out _all sub-make $(CURDIR)/Makefile, $(MAKECMDGOALS)) _all: sub-make
	@:

sub-make: FORCE
	$(if $(KBUILD_VERBOSE:1=),@)$(MAKE) -C $(KBUILD_OUTPUT) \
	KBUILD_SRC=$(CURDIR) \
	KBUILD_EXTMOD="$(KBUILD_EXTMOD)" -f $(CURDIR)/Makefile \
	$(filter-out _all sub-make,$(MAKECMDGOALS))

# Leave processing to above invocation of make
skip-makefile := 1
endif # ifneq ($(KBUILD_OUTPUT),)
endif # ifeq ($(KBUILD_SRC),)

# We process the rest of the Makefile if this is the final invocation of make
ifeq ($(skip-makefile),)

PHONY += all
_all: all

src		:= $(srctree)
obj		:= $(objtree)

VPATH		:= $(srctree)$(if $(KBUILD_EXTMOD),:$(KBUILD_EXTMOD))

export srctree objtree VPATH


# Cross compiling and selecting different set of gcc/bin-utils
# ---------------------------------------------------------------------------
#
# When performing cross compilation for other architectures ARCH shall be set
# to the target architecture. (See arch/* for the possibilities).
# ARCH can be set during invocation of make:
# make ARCH=ia64
# Another way is to have ARCH set in the environment.
# The default ARCH is the host where make is executed.

# CROSS_COMPILE specify the prefix used for all executables used
# during compilation. Only gcc and related bin-utils executables
# are prefixed with $(CROSS_COMPILE).
# CROSS_COMPILE can be set on the command line
# make CROSS_COMPILE=ia64-linux-
# Alternatively CROSS_COMPILE can be set in the environment.
# A third alternative is to store a setting in .config so that plain
# "make" in the configured kernel build directory always uses that.
# Default value for CROSS_COMPILE is not to prefix executables
# Note: Some architectures assign CROSS_COMPILE in their arch/*/Makefile
ARCH		= arm

# Watch out: PATH=$GCC_PATH:$PATH is OK. PATH=$GCC_PATH/:$PATH is NG.
#            That is CROSS_COMPILE_DIR has to be like this:
#		OK: /gcc-arm-none-eabi-4_7-2013q3/bin/
#		NG: /gcc-arm-none-eabi-4_7-2013q3/bin//
CROSS_COMPILE	:= $(dir $(shell which arm-none-eabi-gcc))arm-none-eabi-

# Architecture as present in compile.h
UTS_MACHINE := $(ARCH)
SRCARCH 	:= $(ARCH)


KCONFIG_CONFIG	?= .config
export KCONFIG_CONFIG

# SHELL used by kbuild
CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	  else if [ -x /bin/bash ]; then echo /bin/bash; \
	  else echo sh; fi ; fi)

HOSTCC       = gcc
HOSTCXX      = g++
HOSTCFLAGS   = -Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer
HOSTCXXFLAGS = -O2

# Decide whether to build built-in, modular, or both.
# Normally, just do built-in.
KBUILD_MODULES :=
KBUILD_BUILTIN := 1

export KBUILD_MODULES KBUILD_BUILTIN
export KBUILD_CHECKSRC KBUILD_SRC KBUILD_EXTMOD

# Beautify output
# ---------------------------------------------------------------------------
#
# Normally, we echo the whole command before executing it. By making
# that echo $($(quiet)$(cmd)), we now have the possibility to set
# $(quiet) to choose other forms of output instead, e.g.
#
#         quiet_cmd_cc_o_c = Compiling $(RELDIR)/$@
#         cmd_cc_o_c       = $(CC) $(c_flags) -c -o $@ $<
#
# If $(quiet) is empty, the whole command will be printed.
# If it is set to "quiet_", only the short version will be printed.
# If it is set to "silent_", nothing will be printed at all, since
# the variable $(silent_cmd_cc_o_c) doesn't exist.
#
# A simple variant is to prefix commands with $(Q) - that's useful
# for commands that shall be hidden in non-verbose mode.
#
#	$(Q)ln $@ :<
#
# If KBUILD_VERBOSE equals 0 then the above command will be hidden.
# If KBUILD_VERBOSE equals 1 then the above command is displayed.

ifeq ($(KBUILD_VERBOSE),1)
  quiet =
  Q =
else
  quiet=quiet_
  Q = @
endif

# If the user is running make -s (silent mode), suppress echoing of
# commands

ifneq ($(filter 4.%,$(MAKE_VERSION)),)	# make-4
ifneq ($(filter %s ,$(firstword x$(MAKEFLAGS))),)
  quiet=silent_
endif
else					# make-3.8x
ifneq ($(filter s% -s%,$(MAKEFLAGS)),)
  quiet=silent_
endif
endif

export quiet Q KBUILD_VERBOSE


# Look for make include files relative to root of kernel src
MAKEFLAGS += --include-dir=$(srctree)

# We need some generic definitions (do not try to remake the file).
$(srctree)/build/scripts/Kbuild.include: ;
include $(srctree)/build/scripts/Kbuild.include

# Make variables (CC, etc...)

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
CXX		= $(CROSS_COMPILE)g++
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
AWK		= awk
GENKSYMS	= build/scripts/genksyms/genksyms
INSTALLKERNEL  := installkernel
PERL		= perl
CHECK		= sparse

CHECKFLAGS     := -D__linux__ -Dlinux -D__STDC__ -Dunix -D__unix__ \
		  -Wbitwise -Wno-return-void $(CF)
CFLAGS_MODULE   =
AFLAGS_MODULE   =
LDFLAGS_MODULE  =
CFLAGS_KERNEL	=
AFLAGS_KERNEL	=
CFLAGS_GCOV	= -fprofile-arcs -ftest-coverage

ifneq ($(wildcard $(objtree)/$(KCONFIG_CONFIG)),)

APP := $(shell grep CONFIG_APP $(objtree)/$(KCONFIG_CONFIG) | \
	grep -v '^\# CONFIG_APP' | grep CONFIG_APP_[a-zA-Z0-9_]*_APP= | sed -e s/^CONFIG_APP_// | sed -e s/_APP=y//)

APP_DIR := app/$(shell echo $(APP) | tr [:upper:] [:lower:])

BSP := $(shell grep CONFIG_BSP $(objtree)/$(KCONFIG_CONFIG) | \
	grep -v '^\# CONFIG_BSP' | sed -e s/^CONFIG_BSP_// | sed -e s/=y//)

BSP_DIR := bsp/$(shell echo $(BSP) | tr [:upper:] [:lower:])

SOC := $(shell grep CONFIG_SOC $(objtree)/$(KCONFIG_CONFIG) | \
	grep -v '\# CONFIG_SOC' | sed -e s/^CONFIG_SOC_// | sed -e s/=y//)

CC_VER := $(shell $(CC) -dumpversion)

else
APP := unknown
APP_DIR :=app/unknown
BSP := unknown
BSP_DIR := bsp/unknown
SOC := unknown
endif

AMBA_CPPFLAGS_EXTRA_DEFINES :=
AMBA_AFLAGS_EXTRA_DEFINES :=
AMBA_CFLAGS_EXTRA_DEFINES := -D"SOC=KBUILD_STR($(SOC))"


export APP APP_DIR BSP BSP_DIR SOC CC_VER

AMBA_INCLUDE	= -I$(srctree)/$(BSP_DIR)

# Use USERINCLUDE when you must reference the UAPI directories only.
USERINCLUDE    = \
                -include $(srctree)/build/include/kconfig.h \
                $(AMBA_INCLUDE)

# Use LINUXINCLUDE when you must reference the include/ directory.
# Needed to be compatible with the O= option
LINUXINCLUDE    = \
		$(if $(KBUILD_SRC), -I$(srctree)/build/include) \
		-Ibuild/include \
		$(USERINCLUDE)


KBUILD_CPPFLAGS :=

KBUILD_CFLAGS   := -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs \
		   -fno-strict-aliasing -fno-common \
		   -Wno-format-security \
		   -fno-delete-null-pointer-checks

# Use variable enum size
KBUILD_CFLAGS += -fshort-enums
# wchar_t is short ( 2byte)
KBUILD_CFLAGS += -fshort-wchar

KBUILD_AFLAGS_KERNEL :=
KBUILD_CFLAGS_KERNEL :=
KBUILD_AFLAGS   := -D__ASSEMBLY__ -D__ASM__
KBUILD_AFLAGS_MODULE  := -DMODULE
KBUILD_CFLAGS_MODULE  := -DMODULE
KBUILD_LDFLAGS_MODULE := # -T $(srctree)/build/scripts/module-common.lds

KBUILD_CPPFLAGS += $(AMBA_CPPFLAGS_EXTRA_DEFINES)
KBUILD_AFLAGS += $(AMBA_AFLAGS_EXTRA_DEFINES)
KBUILD_CFLAGS += $(AMBA_CFLAGS_EXTRA_DEFINES)


export ARCH SRCARCH CONFIG_SHELL HOSTCC HOSTCFLAGS CROSS_COMPILE AS LD CC
export CPP CXX AR NM STRIP OBJCOPY OBJDUMP
export MAKE AWK GENKSYMS INSTALLKERNEL PERL UTS_MACHINE
export HOSTCXX HOSTCXXFLAGS LDFLAGS_MODULE CHECK CHECKFLAGS

export KBUILD_CPPFLAGS NOSTDINC_FLAGS LINUXINCLUDE OBJCOPYFLAGS LDFLAGS
export KBUILD_CFLAGS CFLAGS_KERNEL CFLAGS_MODULE CFLAGS_GCOV
export KBUILD_AFLAGS AFLAGS_KERNEL AFLAGS_MODULE
export KBUILD_AFLAGS_MODULE KBUILD_CFLAGS_MODULE KBUILD_LDFLAGS_MODULE
export KBUILD_AFLAGS_KERNEL KBUILD_CFLAGS_KERNEL
export KBUILD_ARFLAGS

# When compiling out-of-tree modules, put MODVERDIR in the module
# tree rather than in the kernel tree. The kernel tree might
# even be read-only.
export MODVERDIR := .tmp_versions

# Files to ignore in find ... statements

RCS_FIND_IGNORE := \( -name SCCS -o -name BitKeeper -o -name .svn -o -name CVS \
		   -o -name .pc -o -name .hg -o -name .git \
		   -o -path ./vendors/ambarella \
		   -o -path ./vendors/bt/brcm \) -prune -o
export RCS_TAR_IGNORE := --exclude SCCS --exclude BitKeeper --exclude .svn \
			 --exclude CVS --exclude .pc --exclude .hg --exclude .git

# ===========================================================================
# Rules shared between *config targets and build targets

# Basic helpers built in build/scripts/
PHONY += scripts_basic
scripts_basic:
	$(Q)$(MAKE) $(build)=build/scripts/basic
	$(Q)rm -f .tmp_quiet_recordmcount

# To avoid any implicit rule to kick in, define an empty command.
build/scripts/basic/%: scripts_basic ;

PHONY += outputmakefile
# outputmakefile generates a Makefile in the output directory, if using a
# separate output directory. This allows convenient use of make in the
# output directory.
outputmakefile:
ifneq ($(KBUILD_SRC),)
	$(Q)#ln -fsn $(srctree) source
	$(Q)$(CONFIG_SHELL) $(srctree)/build/scripts/mkmakefile \
	    $(srctree) $(objtree) $(VERSION) $(PATCHLEVEL)
endif

# To make sure we do not include .config for any of the *config targets
# catch them early, and hand them over to build/scripts/kconfig/Makefile
# It is allowed to specify more targets when calling make, including
# mixing *config targets and build targets.
# For example 'make oldconfig all'.
# Detect when mixed targets is specified, and make a second invocation
# of make so .config is not included in this case either (for *config).

no-dot-config-targets := clean mrproper distclean \
			 cscope gtags TAGS tags help %docs check% coccicheck \
			 headers_% archheaders archscripts \
			 kernelversion %src-pkg %-clean %_clean help2 amba_targets

config-targets := 0
mixed-targets  := 0
dot-config     := 1

ifneq ($(filter $(no-dot-config-targets), $(MAKECMDGOALS)),)
	ifeq ($(filter-out $(no-dot-config-targets), $(MAKECMDGOALS)),)
		dot-config := 0
	endif
endif

ifneq ($(filter config %config,$(MAKECMDGOALS)),)
        config-targets := 1
        ifneq ($(filter-out config %config,$(MAKECMDGOALS)),)
                mixed-targets := 1
        endif
endif

ifeq ($(mixed-targets),1)
# ===========================================================================
# We're called with mixed targets (*config and build targets).
# Handle them one by one.

%:: FORCE
	$(Q)$(MAKE) -C $(srctree) KBUILD_SRC= $@

else

ifeq ($(config-targets),1)
# ===========================================================================
# *config targets only - make sure prerequisites are updated, and descend
# in build/scripts/kconfig to make the *config target

# Read arch specific Makefile to set KBUILD_DEFCONFIG as needed.
# KBUILD_DEFCONFIG may point out an alternative default configuration
# used for 'make defconfig'

include $(srctree)/build/scripts/Makefile.arm
export KBUILD_DEFCONFIG KBUILD_KCONFIG

config: scripts_basic outputmakefile FORCE
	$(Q)mkdir -p build/include/config
	$(Q)$(MAKE) $(build)=build/scripts/kconfig $@

%config: scripts_basic outputmakefile FORCE
	$(Q)mkdir -p build/include/config
	$(Q)$(MAKE) $(build)=build/scripts/kconfig $@

else
# ===========================================================================
# Build targets only - this includes vmlinux, arch specific targets, clean
# targets and others. In general all targets except *config targets.

# Additional helpers built in build/scripts/
# Carefully list dependencies so we do not try to build scripts twice
# in parallel
PHONY += build/scripts
build/scripts: scripts_basic build/include/config/auto.conf build/include/config/tristate.conf
	$(Q)$(MAKE) $(build)=$(@)

# Objects we will link into vmlinux / subdirs we need to visit
# target-y		:= folder/

ifeq ($(dot-config),1)
# Read in config

-include build/include/config/auto.conf

# Read in dependencies to all Kconfig* files, make sure to run
# oldconfig if changes are detected.
-include build/include/config/auto.conf.cmd

# To avoid any implicit rule to kick in, define an empty command
$(KCONFIG_CONFIG) build/include/config/auto.conf.cmd: ;

# If .config is newer than build/include/config/auto.conf, someone tinkered
# with it and forgot to run make oldconfig.
# if auto.conf.cmd is missing then we are probably in a cleaned tree so
# we execute the config step to be sure to catch updated Kconfig files
build/include/config/%.conf: $(KCONFIG_CONFIG) build/include/config/auto.conf.cmd
	$(Q)$(MAKE) -f $(srctree)/Makefile silentoldconfig

else
# Dummy target needed, because used as prerequisite
build/include/config/auto.conf: ;
endif # $(dot-config)

# The all: target is the default when no target is given on the command line.
all: amba_all

ifdef CONFIG_CC_OPTIMIZE_FOR_SIZE
KBUILD_CFLAGS	+= -Os $(call cc-disable-warning,maybe-uninitialized,)
else ifeq ($(CONFIG_CC_OPTIMIZE_0),y)
KBUILD_CFLAGS	+= -O0
else ifeq ($(CONFIG_CC_OPTIMIZE_1),y)
KBUILD_CFLAGS	+= -O1
else ifeq ($(CONFIG_CC_OPTIMIZE_2),y)
KBUILD_CFLAGS	+= -O2
else
KBUILD_CFLAGS	+= -O3
endif

include $(srctree)/build/scripts/Makefile.arm

ifdef CONFIG_READABLE_ASM
# Disable optimizations that make assembler listings hard to read.
# reorder blocks reorders the control in the function
# ipa clone creates specialized cloned functions
# partial inlining inlines only parts of functions
KBUILD_CFLAGS += $(call cc-option,-fno-reorder-blocks,) \
                 $(call cc-option,-fno-ipa-cp-clone,) \
                 $(call cc-option,-fno-partial-inlining)
endif

ifneq ($(CONFIG_FRAME_WARN),0)
KBUILD_CFLAGS += -Wframe-larger-than=${CONFIG_FRAME_WARN}
endif

ifeq ($(CONFIG_CC_W_MISSING_BRACES),y)
KBUILD_CFLAGS += -Wno-missing-braces
endif

# Handle stack protector mode.
ifdef CONFIG_CC_STACKPROTECTOR_REGULAR
  stackp-flag := -fstack-protector
  ifeq ($(call cc-option, $(stackp-flag)),)
    $(warning Cannot use CONFIG_CC_STACKPROTECTOR_REGULAR: \
             -fstack-protector not supported by compiler)
  endif
else
ifdef CONFIG_CC_STACKPROTECTOR_STRONG
  stackp-flag := -fstack-protector-strong
  ifeq ($(call cc-option, $(stackp-flag)),)
    $(warning Cannot use CONFIG_CC_STACKPROTECTOR_STRONG: \
	      -fstack-protector-strong not supported by compiler)
  endif
else
  # Force off for distro compilers that enable stack protector by default.
  stackp-flag := -fno-stack-protector
endif
endif
KBUILD_CFLAGS += $(stackp-flag)

# This warning generated too much noise in a regular build.
# Use make W=1 to enable this warning (see build/scripts/Makefile.build)
KBUILD_CFLAGS += -Wno-unused-but-set-variable

ifdef CONFIG_FRAME_POINTER
KBUILD_CFLAGS	+= -fno-omit-frame-pointer -fno-optimize-sibling-calls
else
# Some targets (ARM with Thumb2, for example), can't be built with frame
# pointers.  For those, we don't have FUNCTION_TRACER automatically
# select FRAME_POINTER.  However, FUNCTION_TRACER adds -pg, and this is
# incompatible with -fomit-frame-pointer with current GCC, so we don't use
# -fomit-frame-pointer with FUNCTION_TRACER.
ifndef CONFIG_FUNCTION_TRACER
KBUILD_CFLAGS	+= -fomit-frame-pointer
endif
endif

KBUILD_CFLAGS   += -fno-var-tracking-assignments

ifdef CONFIG_DEBUG_INFO
KBUILD_CFLAGS	+= -g -gdwarf-3
KBUILD_AFLAGS	+= -Wa,--gdwarf-2
endif

ifdef CONFIG_DEBUG_INFO_REDUCED
KBUILD_CFLAGS 	+= $(call cc-option, -femit-struct-debug-baseonly) \
		   $(call cc-option,-fno-var-tracking)
endif

ifdef CONFIG_FUNCTION_TRACER
ifdef CONFIG_HAVE_FENTRY
CC_USING_FENTRY	:= $(call cc-option, -mfentry -DCC_USING_FENTRY)
endif
KBUILD_CFLAGS	+= -pg $(CC_USING_FENTRY)
KBUILD_AFLAGS	+= $(CC_USING_FENTRY)
ifdef CONFIG_DYNAMIC_FTRACE
	ifdef CONFIG_HAVE_C_RECORDMCOUNT
		BUILD_C_RECORDMCOUNT := y
		export BUILD_C_RECORDMCOUNT
	endif
endif
endif

# We trigger additional mismatches with less inlining
ifdef CONFIG_DEBUG_SECTION_MISMATCH
KBUILD_CFLAGS += $(call cc-option, -fno-inline-functions-called-once)
endif

# arch Makefile may override CC so keep this after arch Makefile is included
#NOSTDINC_FLAGS += -nostdinc -isystem $(shell $(CC) -print-file-name=include)
CHECKFLAGS     += $(NOSTDINC_FLAGS)

# warn about C99 declaration after statement
KBUILD_CFLAGS += -Wdeclaration-after-statement

# disable pointer signed / unsigned warnings in gcc 4.0
KBUILD_CFLAGS += -Wpointer-sign

# disable invalid "can't wrap" optimizations for signed / pointers
KBUILD_CFLAGS	+= -fno-strict-overflow

# conserve stack if available
KBUILD_CFLAGS   += -fconserve-stack

# disallow errors like 'EXPORT_GPL(foo);' with missing header
KBUILD_CFLAGS   += -Werror=implicit-int

# require functions to have arguments in prototypes, not empty 'int foo()'
KBUILD_CFLAGS   += -Werror=strict-prototypes

# Prohibit date/time macros, which would make the build non-deterministic
#x KBUILD_CFLAGS   += $(call cc-option,-Werror=date-time)

# For ISO C99
ifeq ($(CONFIG_CC_STD_C99),y)
KBUILD_CFLAGS	+= -std=c99
else ifeq ($(CONFIG_CC_STD_GNU99),y)
KBUILD_CFLAGS	+= -std=gnu99
endif

# use the deterministic mode of AR if available
KBUILD_ARFLAGS := D

# check for 'asm goto'
#ifeq ($(shell $(CONFIG_SHELL) $(srctree)/build/scripts/gcc-goto.sh $(CC)), y)
#	KBUILD_CFLAGS += -DCC_HAVE_ASM_GOTO
#endif
KBUILD_CFLAGS += -DCC_HAVE_ASM_GOTO

# Add user supplied CPPFLAGS, AFLAGS and CFLAGS as the last assignments
KBUILD_CPPFLAGS += $(KCPPFLAGS)
KBUILD_AFLAGS += $(KAFLAGS)
KBUILD_CFLAGS += $(KCFLAGS)

#
# Flags from Ambarella
#
ifeq ($(CONFIG_SSP_THREADX_SMP),)
KBUILD_CPPFLAGS += -DAMBA_KAL_NO_SMP
KBUILD_AFLAGS += -DAMBA_KAL_NO_SMP
KBUILD_CFLAGS += -DAMBA_KAL_NO_SMP
endif

# Use --build-id when available.
LDFLAGS_BUILD_ID = $(patsubst -Wl$(comma)%,%,\
			      -Wl$(comma)--build-id)
KBUILD_LDFLAGS_MODULE += $(LDFLAGS_BUILD_ID)
LDFLAGS_vmlinux += $(LDFLAGS_BUILD_ID)

ifeq ($(CONFIG_STRIP_ASM_SYMS),y)
LDFLAGS_vmlinux	+= $(call ld-option, -X,)
endif

##########################
# LDFLAGS_ambacamera setup
##########################
CPRE  = $(patsubst %-,%,$(notdir $(CROSS_COMPILE)))
CPATH = $(patsubst %/bin/,%,$(dir $(CROSS_COMPILE)))
CVER  = $(shell $(CC) -dumpversion)

ifeq ($(HOST_OS),CYGWIN)
CPATH := $(shell cygpath -m $(CPATH))
endif

#LDFLAGS_amba_common := $(LDFLAGS_vmlinux)
#LDFLAGS_amba_common := -p --no-undefined -X --build-id
LDFLAGS_amba_common := -p --no-undefined
#LDFLAGS_amba_common     = -EL -static
# Garbage collection of unused section
LDFLAGS_amba_common              += --gc-sections

# warning: ... uses variable-size enums yet the output is to use 32-bit enums; use of enum values across objects may fail
# Use -fno-short-enums causes linking warnings.
#LDFLAGS_amba_common              += --no-enum-size-warning

# warning: ... uses 4-byte wchar_t yet the output is to use 2-byte wchar_t; use of wchar_t values across objects may fail
# Use -fshort-wchar for CC, so we can have L"text" style declaration. However libc use 4(linux)/2(cygwin) for wchar_t, there would be a warning while
# linking. So we supress it.
LDFLAGS_amba_common             += --no-wchar-size-warning

# Not good for multi-definitions, the same as "--allow-multiple-definition"
#LDFLAGS_amba_common             += -z muldefs
# Options
#LDFLAGS_amba_common             += --strip-all
LDFLAGS_amba_common              += -nostdlib -nodefaultlibs -nostartfiles
#LDFLAGS_amba_common              += -lnosys

ifeq ($(CONFIG_VFP_V3),y)
## libc
LDFLAGS_amba_common              += -L$(CPATH)/$(CPRE)/lib/armv7-ar/thumb/fpu
## libgcc
LDFLAGS_amba_common              += -L$(CPATH)/lib/gcc/$(CPRE)/$(CVER)/armv7-ar/thumb/fpu
else
# libc
LDFLAGS_amba_common              += -L$(CPATH)/$(CPRE)/lib
# libgcc
LDFLAGS_amba_common              += -L$(CPATH)/lib/gcc/$(CPRE)/$(CVER)
endif
export LDFLAGS_amba_common

#
# Targets of Amba, $(AMBA_TARGET)
#
include $(srctree)/build/scripts/Makefile.Amba

PHONY += amba_all

# Things we need to do before we recursively start building the kernel
# or the modules are listed in "prepare".
# All the preparing...
PHONY += prepare
prepare: amba_prepare ;

PHONY += amba_prepare
amba_prepare: outputmakefile build/include/config/auto.conf scripts_basic
	$(Q)$(MAKE) $(build)=. amba_O_lib
	$(Q)$(MAKE) $(build)=.

PHONY += amba_O_lib
ifneq ($(O),)
# Prepare libs
amba_O_lib:
	@if [ ! -e $(AMBA_O_LIB) ]; then mkdir -p $(AMBA_O_LIB); fi
	@mkdir -p $(objtree)/tools/exec/
	@rsync -a $(srctree)/tools/exec/ $(objtree)/tools/exec/
else
# Do nothing
amba_O_lib:
	@:
endif

# ---------------------------------------------------------------------------
#

PHONY += depend dep
depend dep:
	@echo '*** Warning: make $@ is unnecessary now.'

# ---------------------------------------------------------------------------
# Headers

exec-sh-dir = $(patsubst %/,%,$(dir $(subst -exec,/,$@)))
%-exec:
	$(Q)$(CONFIG_SHELL) $(exec-sh-dir)/exec.sh $(exec-sh-dir)

amba_install_headers:
	$(Q)$(CONFIG_SHELL) $(srctree)/build/scripts/$@.sh

# ---------------------------------------------------------------------------

###
# Cleaning is done on three levels.
# make clean     Delete most generated files
#                Leave enough to build external modules
# make mrproper  Delete the current configuration, and all generated files
# make distclean Remove editor backup files, patch leftover files and the like

# Directories & files removed with 'make clean'
CLEAN_DIRS  += $(MODVERDIR)

# Directories & files removed with 'make mrproper'
MRPROPER_DIRS  += build/include/config usr/include build/include/generated          \
                  arch/*/include/generated
MRPROPER_FILES += .config .config.old .version .old_version \
		  Module.symvers tags TAGS cscope* GPATH GTAGS GRTAGS GSYMS \
		  signing_key.priv signing_key.x509 x509.genkey		\
		  extra_certificates signing_key.x509.keyid		\
		  signing_key.x509.signer

# clean - Delete most, but leave enough to build external modules
#
clean: rm-dirs  := $(CLEAN_DIRS)
clean: rm-files := $(CLEAN_FILES)
clean-dirs      := $(addprefix _clean_, . $(AMBA_CLEAN_ALLDIRS))

PHONY += $(clean-dirs) clean
$(clean-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _clean_%,%,$@)

clean: $(AMBA_CLEAN_ALL)

# mrproper - Delete all generated files, including .config
#
mrproper: rm-dirs  := $(wildcard $(MRPROPER_DIRS))
mrproper: rm-files := $(wildcard $(MRPROPER_FILES))
mrproper-dirs      := $(addprefix _mrproper_,build/scripts)

PHONY += $(mrproper-dirs) mrproper
$(mrproper-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _mrproper_%,%,$@)

mrproper: clean $(mrproper-dirs)
	$(call cmd,rmdirs)
	$(call cmd,rmfiles)

# distclean
#
PHONY += distclean

distclean: mrproper
	@find $(srctree) $(RCS_FIND_IGNORE) \
		\( -name '*.orig' -o -name '*.rej' -o -name '*~' \
		-o -name '*.bak' -o -name '#*#' -o -name '.*.orig' \
		-o -name '.*.rej' \
		-o -name '*%' -o -name '.*.cmd' -o -name 'core' \) \
		-type f -print | xargs rm -f
		
# Packaging of the kernel to various formats
# ---------------------------------------------------------------------------
# rpm target kept for backward compatibility
package-dir	:= $(srctree)/build/scripts/package

%src-pkg: FORCE
	$(Q)$(MAKE) $(build)=$(package-dir) $@
%pkg: build/include/config/kernel.release FORCE
	$(Q)$(MAKE) $(build)=$(package-dir) $@
rpm: build/include/config/kernel.release FORCE
	$(Q)$(MAKE) $(build)=$(package-dir) $@

# Brief documentation of the typical targets used
# ---------------------------------------------------------------------------

boards := $(wildcard $(srctree)/configs/*_defconfig)
boards := $(notdir $(boards))
board-dirs := $(dir $(wildcard $(srctree)/configs/*/*_defconfig))
board-dirs := $(sort $(notdir $(board-dirs:/=)))

help:
	@echo  '----------------------------------------------------------------------'
	@echo  'Configuration targets:'
	@echo  '  menuconfig        - Update current config utilising a menu based program'
	@echo  'Generic targets:'
	@echo  '  all               - Build all targets with the order: amba_ssp_ut amba_mw_ut amba_app amba_amboot'
	@echo  ''
	@echo  '  amba_app          - Gen amba_app.elf'
	@echo  '  amba_bld          - Gen amba_bld.elf'
	@echo  '  amba_bst          - Gen amba_bst.elf'
	@echo  '  amba_mw_ut        - Gen amba_mw_ut.elf (MW Unit-Test)'
	@echo  '  amba_ssp_ut       - Gen amba_ssp_ut.elf (SSP Uint-Test)'
	@echo  '  amba_ssp_svc      - Gen amba_ssp_svc.elf'
	@echo  '  amba_fwprog       - Gen all Firmware Programmer'
	@echo  ''
	@echo  '  amboot            - Make bst bld fwprog'
	@echo  '  amba_amboot       - Make bst bld fwprog'
	@echo  '  amba_fwprog_ssput - Make ssp_ut bst bld fwprog'
	@echo  '  amba_fwprog_mwut  - Make mw_ut bst bld fwprog'
	@echo  '  amba_fwprog_app   - Make app bst bld fwprog'
	@echo  ''
	@echo  '  size [SN=1-5]     - List section size of *.a, *.elf. SN is sort number of column. Default is 4.'
	@echo  ''
	@echo  '  dir/              - Build all files in dir and below (without install)'
	@echo  '  dir/file.[oisS]   - Build specified target only'
	@echo  '  dir/file.lst      - Build specified mixed source/assembly target only'
	@echo  '  dir-install       - Install files'
	@echo  '----------------------------------------------------------------------'
	@echo  'Saved defconfig:'
	@$(if $(boards), \
		$(foreach b, $(boards), \
		printf "  %-24s - Build for %s\\n" $(b) $(subst _defconfig,,$(b));) \
		echo '')
	@$(if $(board-dirs), \
		$(foreach b, $(board-dirs), \
		printf "  %-16s - Show %s-specific targets\\n" help-$(b) $(b);) \
		printf "  %-16s - Show all of the above\\n" help-boards; \
		echo '')
	@echo  '----------------------------------------------------------------------'
	@echo  '  make V=0|1 [targets] 0 => quiet build (default), 1 => verbose build'
	@echo  ''
	@echo  'Execute "make" or "make all" to build all targets marked with [*] '
	@echo  'Execute "make help2" to get more help. '
	@echo  ''


help-board-dirs := $(addprefix help-,$(board-dirs))

help-boards: $(help-board-dirs)

boards-per-dir = $(notdir $(wildcard $(srctree)/configs/$*/*_defconfig))

$(help-board-dirs): help-%:
	@echo  'Architecture specific targets ($(SRCARCH) $*):'
	@$(if $(boards-per-dir), \
		$(foreach b, $(boards-per-dir), \
		printf "  %-24s - Build for %s\\n" $*/$(b) $(subst _defconfig,,$(b));) \
		echo '')

help2:
	@echo  '----------------------------------------------------------------------'
	@echo  'Cleaning targets:'
	@echo  '  clean           - Remove most generated files but keep the config and'
	@echo  '                    enough build support to build external modules'
	@echo  '  mrproper        - Remove all generated files + config + various backup files'
	@echo  '  distclean       - mrproper + remove editor backup and patch files'
	@echo  ''
	@echo  '  dir-clean       - Clean dir only'
	@echo  ''
	@echo  'Configuration targets:'
	@$(MAKE) -f $(srctree)/build/scripts/kconfig/Makefile help
	@echo  ''
	@echo  'Generate tag files'
	@echo  '  tags/TAGS       - Generate tags file for editors'
	@echo  '  cscope          - Generate cscope index'
	@echo  '  gtags           - Generate GNU GLOBAL index'
	@echo  'Static analysers'
	@echo  '  includecheck    - Check for duplicate included header files'
	@echo  ''
	@echo  '----------------------------------------------------------------------'
	@echo  '  make V=2   [targets] 2 => give reason for rebuild of target'
	@echo  '  make O=dir [targets] Locate all output files in "dir", including .config'
	@echo  '  make C=1   [targets] Check all c source with $$CHECK (sparse by default)'
	@echo  '  make C=2   [targets] Force check of all c source with $$CHECK'
	@echo  '  make RECORDMCOUNT_WARN=1 [targets] Warn about ignored mcount sections'
	@echo  '  make W=n   [targets] Enable extra gcc checks, n=1,2,3 where'
	@echo  '                1: warnings which may be relevant and do not occur too often'
	@echo  '                2: warnings which occur quite often but may still be relevant'
	@echo  '                3: more obscure warnings, can most likely be ignored'
	@echo  '                Multiple levels can be combined with W=12 or W=123'
	@echo  ''

# ---------------------------------------------------------------------------

clean: $(clean-dirs)
	$(call cmd,rmdirs)
	$(call cmd,rmfiles)
	@find $(if $(KBUILD_EXTMOD), $(KBUILD_EXTMOD), .) $(RCS_FIND_IGNORE) \
		\( -name '*.[oa]' -o -name '*.ko' -o -name '.*.cmd' \
		-o -name '*.lds' \
		-o -name '*.ko.*' \
		-o -name '.*.d' -o -name '.*.tmp' -o -name '*.mod.c' \
		-o -name '*.symtypes' -o -name 'modules.order' \
		-o -name modules.builtin -o -name '.tmp_*.o.*' \
		-o -name '*.gcno' \) -type f -print | xargs rm -f
	@rm -rf $(KBUILD_AMBA_OUT_DIR)

%-clean:
	$(Q)$(MAKE) $(clean)=$(patsubst %-clean,%,$@)
	@find $(if $(KBUILD_EXTMOD), $(KBUILD_EXTMOD)/$(patsubst %-clean,%,$@), ./$(patsubst %-clean,%,$@)) $(RCS_FIND_IGNORE) \
		\( -name '*.[oa]' -o -name '*.ko' -o -name '.*.cmd' \
		-o -name '*.lds' \
		-o -name '*.ko.*' \
		-o -name '.*.d' -o -name '.*.tmp' -o -name '*.mod.c' \
		-o -name '*.symtypes' -o -name 'modules.order' \
		-o -name modules.builtin -o -name '.tmp_*.o.*' \
		-o -name '*.gcno' \) -type f -print | xargs rm -f
# Generate tags for editors
# ---------------------------------------------------------------------------
quiet_cmd_tags = GEN     $@
      cmd_tags = $(CONFIG_SHELL) $(srctree)/build/scripts/tags.sh $@

tags TAGS cscope gtags: FORCE
	$(call cmd,tags)

# Scripts to check various things for consistency
# ---------------------------------------------------------------------------

PHONY += includecheck

includecheck:
	find $(srctree)/* $(RCS_FIND_IGNORE) \
		-name '*.[hcS]' -type f -print | sort \
		| xargs $(PERL) -w $(srctree)/build/scripts/checkincludes.pl

endif #ifeq ($(config-targets),1)
endif #ifeq ($(mixed-targets),1)

# Single targets
# ---------------------------------------------------------------------------
# Single targets are compatible with:
# - build with mixed source and output
# - build with separate output dir 'make O=...'
# - external modules
#
#  target-dir => where to store outputfile
#  build-dir  => directory in kernel source tree to use
build-dir  = $(patsubst %/,%,$(dir $@))
target-dir = $(dir $@)

build-install-dir = $(patsubst %/,%,$(dir $(subst -install,/,$@)))
%.s: %.c prepare build/scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
%.i: %.c prepare build/scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
%.o: %.c prepare build/scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
%.lst: %.c prepare build/scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
%.s: %.S prepare build/scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
%.o: %.S prepare build/scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
%.o: %.asm prepare build/scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
ifdef CONFIG_CC_CXX_SUPPORT
%.o: %.cpp prepare build/scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
endif
%.lds: %.lds.S prepare build/scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
%.symtypes: %.c prepare build/scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
%.a: prepare build/scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)

# Modules
/: prepare build/scripts FORCE
	$(Q)$(MAKE) \
	$(build)=$(build-dir)
%/: prepare build/scripts FORCE
	$(Q)$(MAKE) \
	$(build)=$(build-dir) KBUILD_INSTALL=0
%-install: prepare build/scripts FORCE
	$(Q)$(MAKE) \
	$(build)=$(build-install-dir) KBUILD_INSTALL=2

# FIXME Should go into a make.lib or something
# ===========================================================================
quiet_cmd_rmdirs = $(if $(wildcard $(rm-dirs)),CLEAN   $(wildcard $(rm-dirs)))
      cmd_rmdirs = rm -rf $(rm-dirs)

quiet_cmd_rmfiles = $(if $(wildcard $(rm-files)),CLEAN   $(wildcard $(rm-files)))
      cmd_rmfiles = rm -f $(rm-files)
# read all saved command lines

targets := $(wildcard $(sort $(targets)))
cmd_files := $(wildcard .*.cmd $(foreach f,$(targets),$(dir $(f)).$(notdir $(f)).cmd))

ifneq ($(cmd_files),)
  $(cmd_files): ;	# Do not try to update included dependency files
  include $(cmd_files)
endif

# Shorthand for $(Q)$(MAKE) -f build/scripts/Makefile.clean obj=dir
# Usage:
# $(Q)$(MAKE) $(clean)=dir
clean := -f $(if $(KBUILD_SRC),$(srctree)/)build/scripts/Makefile.clean obj

endif	# skip-makefile

PHONY += FORCE
FORCE:

# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)
