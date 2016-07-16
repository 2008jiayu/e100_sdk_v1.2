#!/bin/bash

# ---- Functions declaration ---- #

cleanup()
{
	echo "cleanup amba_bld"

	if [ -s ${objtree}/${KBUILD_AMBA_OUT_DIR}/ ]; then
		find ${objtree}/${KBUILD_AMBA_OUT_DIR}/ \
			\( \
			   -name "${__LINK_FILE_STEM}.*" \
			\) -type f -print | xargs rm -f
	fi
}

extract_symbol()
{
	${NM} $1 | grep $2 | awk -F" " '{ print $1 }'
}

show_variables()
{
	echo "================================================="
	echo "---- show_variables() ----"
	echo ""
	echo "cmd_link-amba_bld: ${0} ${1} ${2} ${3} ${4}"
	echo "arg0=${0} arg1=${1} arg2=${2} arg3=${3} arg4=${4}"
	echo ""
	echo "---- Global variable ----"
	echo "LD=${LD}"
	echo "LDS_amba_bld=${LDS_amba_bld}"
	echo "LDFLAGS=${LDFLAGS}"
	echo "LDFLAGS_amba_bld=${LDFLAGS_amba_bld}"
	echo "srctree=${srctree}"
	echo "objtree=${objtree}"
	echo "DIR_amba_bld=${DIR_amba_bld}"
	echo ""
	echo "---- Local variable ----"
	echo "LINK_OBJS=${LINK_OBJS}"
	echo "LINK_LIBS=${LINK_LIBS}"
	echo "LINK_LDS=${LINK_LDS}"
	echo "LINK_LDFLAGS=${LINK_LDFLAGS}"
	echo ""
	echo "LINK_OUTPUT_ELF=${LINK_OUTPUT_ELF}"
	echo "LINK_OUTPUT_BIN=${LINK_OUTPUT_BIN}"
	echo "LINK_OUTPUT_MAP=${LINK_OUTPUT_MAP}"
	echo "LINK_OUTPUT_NM=${LINK_OUTPUT_NM}"
	echo ""
	echo "LINK_CMD=${LINK_CMD}"
	echo "NM_CMD=${NM_CMD}"
	echo "OBJCOPY_CMD=${OBJCOPY_CMD}"
	echo ""
	echo "================================================="
}

run_ld_cmd()
{
	if [ ${KBUILD_VERBOSE} == 0 ]; then
		echo "  LD      ${LINK_OUTPUT_ELF}"
		echo "  LD_MAP  ${LINK_OUTPUT_MAP}"
	fi

	${LINK_CMD}
}

run_nm_cmd()
{
	if [ ${KBUILD_VERBOSE} == 0 ]; then
		echo "  NM      ${LINK_OUTPUT_NM}"
	fi

	${NM_CMD} > ${LINK_OUTPUT_NM}
}

run_objcopy_cmd()
{
	if [ ${KBUILD_VERBOSE} == 0 ]; then
		echo "  OBJCOPY ${LINK_OUTPUT_BIN}"
	fi

	${OBJCOPY_CMD}
}

rpc_crc_calc()
{
	${RPC_CRC_CMD}
	${BUILD_RPC_CRC_CMD}
}

# ---- Main start of script ---- #

# Common for link shell script

# Error out on error
set -e

# Delete output files in case of error
trap cleanup SIGHUP SIGINT SIGQUIT SIGTERM ERR

# Use "make V=1" to debug this script
case "${KBUILD_VERBOSE}" in
*1*)
	set -x
	;;
esac

__LINK_FILE_STEM="amba_bld"

if [ "$1" = "clean" ]; then
	cleanup
	exit 0
fi

# We need access to CONFIG_ symbols
case "${KCONFIG_CONFIG}" in
*/*)
	. "${KCONFIG_CONFIG}"
	;;
*)
	# Force using a file from the current directory
	. "./${KCONFIG_CONFIG}"
esac

if [ ${HOST_OS} == "CYGWIN" ]; then
	srctree=`echo ${srctree} | sed -e 's@/cygdrive/\([a-zA-Z]\)@\1:@g'`
	objtree=`echo ${objtree} | sed -e 's@/cygdrive/\([a-zA-Z]\)@\1:@g'`
fi

SYS_BIN=${CONFIG_SYS}
SYS_ELF=`echo ${SYS_BIN} | sed -e 's/.bin/.elf/'`

if [ ! -e ${SYS_ELF} ]; then
REGION_INFO="unsigned int Region0RoBase   = 0x0;\n
             unsigned int Region0RoLength = 0x0;\n
             unsigned int Region0RwBase   = 0x0;\n
             unsigned int Region0RwLength = 0x0;\n
             unsigned int Region5RoBase   = 0x0;\n
             unsigned int Region5RoLength = 0x0;\n
             unsigned int Region5RwBase   = 0x0;\n
             unsigned int Region5RwLength = 0x0;\n
             unsigned int DspBufBase      = 0x0;\n
             unsigned int DspBufLength    = 0x0;\n"
else
REGION_INFO="unsigned int Region0RoBase   = 0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region0_ro_start);\n \
             unsigned int Region0RoLength = (0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region0_ro_end) -   \
                                            0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region0_ro_start));\n \
             unsigned int Region5RoBase   = 0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region5_ro_start);\n  \
             unsigned int Region5RoLength = (0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region5_ro_end) -   \
                                            0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region5_ro_start));\n \
             unsigned int Region0RwBase   = 0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region0_rw_start);\n  \
             unsigned int Region0RwLength = (0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region0_rw_end) -   \
                                            0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region0_rw_start));\n \
             unsigned int Region5RwBase   = 0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region5_rw_start);\n  \
             unsigned int Region5RwLength = (0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region5_rw_end) -   \
                                            0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region5_rw_start));\n \
             unsigned int DspBufBase   = 0x$(extract_symbol ${objtree}/${SYS_ELF} __dsp_buf_start);\n \
             unsigned int DspBufLength = (0x$(extract_symbol ${objtree}/${SYS_ELF} __dsp_buf_end) -   \
                                            0x$(extract_symbol ${objtree}/${SYS_ELF} __dsp_buf_start));\n"
fi

# Put at srctree, because Makefile read it from srctree
__REGION_INFO_FILE__=ssp/fwprog/AmbaFwLoader_RegionInfo
REGION_INFO_FILE="${objtree}/${__REGION_INFO_FILE__}"
mkdir -p ${objtree}/ssp/fwprog

rm -f ${REGION_INFO_FILE}.c
echo -e "  GEN     ${__REGION_INFO_FILE__}.c"

echo -e "/*"						> ${REGION_INFO_FILE}.c
echo -e " * Automatically generated file: don't edit" 	>> ${REGION_INFO_FILE}.c
echo -e " */"						>> ${REGION_INFO_FILE}.c
echo -e ""						>> ${REGION_INFO_FILE}.c
echo -e ${REGION_INFO}					>> ${REGION_INFO_FILE}.c
echo -e ""						>> ${REGION_INFO_FILE}.c

# ---- Definition for link target dependent ---- #
BOOT_TYPE=`grep "CONFIG_ENABLE*" ${KCONFIG_CONFIG} | grep "=y" | grep "BOOT" | awk -F "_" '{printf $3}'`

chk_lib()
{
	if [ -s ${AMBA_O_LIB}/$1 ]; then
		echo ${AMBA_O_LIB}/$1
	elif [ -s ${AMBA_LIB_FOLDER}/$1 ]; then
		echo ${AMBA_LIB_FOLDER}/$1
	else
		echo "Can not found $1"
		exit 1
	fi
}

if [ "${BOOT_TYPE}" == "EMMC" ]; then
__LINK_AMBA_LIBS="--whole-archive		\
		  `chk_lib libcomsvc_sys.a`		\
		  `chk_lib libsoc_bld_emmc.a`	\
		  `chk_lib libsoc_non_os.a`		\
		  `chk_lib libamboot_bld.a`		\
		  `chk_lib libperipheral.a`		\
		  --no-whole-archive"

elif [ "${BOOT_TYPE}" == "SPINOR" ]; then
__LINK_AMBA_LIBS="--whole-archive		\
		  `chk_lib libcomsvc_sys.a`		\
		  `chk_lib libsoc_bld_nor.a`	\
		  `chk_lib libsoc_non_os.a`		\
		  `chk_lib libamboot_bld.a`		\
		  `chk_lib libperipheral.a`		\
		  --no-whole-archive"
else
__LINK_AMBA_LIBS="--whole-archive	\
		  `chk_lib libcomsvc_sys.a`	\
		  `chk_lib libsoc_bld.a`	\
		  `chk_lib libsoc_non_os.a`	\
		  `chk_lib libamboot_bld.a`	\
		  `chk_lib libperipheral.a`	\
		  --no-whole-archive"
fi

__LINK_AMBA_LIBS=$(echo ${__LINK_AMBA_LIBS} | tr -s [:space:])
if [ ${HOST_OS} == "CYGWIN" ]; then
	__LINK_AMBA_LIBS=`echo ${__LINK_AMBA_LIBS} | sed -e 's@/cygdrive/\([a-zA-Z]\)@\1:@g'`
fi

__LINK_C_LIBS="-lc -lnosys -lm -lgcc -lrdimon"

# ld serachdir: -L option
__LINK_LIBS_INC=

LINK_OBJS="${objtree}/${BSP_DIR}/bsp.o"
LINK_LIBS="${__LINK_AMBA_LIBS} ${__LINK_C_LIBS}"
LINK_LDS="${LDS_amba_bld}"
LINK_LDFLAGS="${LDFLAGS} ${LDFLAGS_amba_bld} ${__LINK_LIBS_INC}"

if [ ${KBUILD_AMBA_OUT_DIR} ]; then
	LINK_AMBA_OUT_DIR=${KBUILD_AMBA_OUT_DIR}
	mkdir -p ${LINK_AMBA_OUT_DIR}
else
	LINK_AMBA_OUT_DIR=${DIR_amba_bld}
fi

LINK_OUTPUT_ELF="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}.elf"
LINK_OUTPUT_BIN="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}.bin"
LINK_OUTPUT_MAP="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}.map"
LINK_OUTPUT_NM="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}.nm"

#echo ${KBUILD_CFLAGS}
#LOCAL_CFLAGS="${KBUILD_CFLAGS} -D\"KBUILD_STR(s)=#s\" "
#LOCAL_CFLAGS="${KBUILD_CFLAGS}"
#LOCAL_CFLAGS="-mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfpv3 -marm -march=armv7-a -mcpu=cortex-a9 -mhard-float -ffunction-sections -fdata-sections"
LOCAL_CFLAGS="`echo ${KBUILD_CFLAGS} | sed 's/\"//g'`"

BUILD_CMD="${CC} ${LOCAL_CFLAGS} -c ${REGION_INFO_FILE}.c -o ${REGION_INFO_FILE}.o"
LINK_CMD="${LD} ${LINK_LDFLAGS} -o ${LINK_OUTPUT_ELF} -T ${LINK_LDS} --start-group ${LINK_LIBS} --end-group ${LINK_OBJS} -Map=${LINK_OUTPUT_MAP}"
NM_CMD="${NM} -n -l ${LINK_OUTPUT_ELF}"
OBJCOPY_CMD="${OBJCOPY} -O binary ${LINK_OUTPUT_ELF} ${LINK_OUTPUT_BIN}"

SYS_BIN=${CONFIG_SYS}
SYS_ELF=`echo ${SYS_BIN} | sed -e 's/.bin/.elf/'`

rm -f ${REGION_INFO_FILE}.list
echo -e "  GEN     ${__REGION_INFO_FILE__}.list"

if [ ! -e ${SYS_ELF} ]; then
echo "REGION0_RO_BASE  0x00000000" >> ${REGION_INFO_FILE}.list
echo "REGION0_RO_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
echo "REGION0_RW_BASE 0x00000000" >> ${REGION_INFO_FILE}.list
echo "REGION0_RW_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
echo "REGION5_RO_BASE  0x00000000" >> ${REGION_INFO_FILE}.list
echo "REGION5_RO_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
echo "REGION5_RW_BASE 0x00000000" >> ${REGION_INFO_FILE}.list
echo "REGION5_RW_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
echo "DSP_BUF_BASE 0x00000000" >> ${REGION_INFO_FILE}.list
echo "DSP_BUF_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
else
REGION_INFO="REGION0_RO_BASE    0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region0_ro_start)"
echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
REGION_INFO="REGION0_RO_LENGTH    (0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region0_ro_end) -    \
                                            0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region0_ro_start))"
echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
REGION_INFO="REGION5_RO_BASE    0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region5_ro_start)"
echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
REGION_INFO="REGION5_RO_LENGTH    (0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region5_ro_end) -    \
                                            0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region5_ro_start))"
echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
REGION_INFO="REGION0_RW_BASE    0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region0_rw_start)"
echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
REGION_INFO="REGION0_RW_LENGTH    (0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region0_rw_end) -   \
                                            0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region0_rw_start))"
echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
REGION_INFO="REGION5_RW_BASE    0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region5_rw_start)"
echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
REGION_INFO="REGION5_RW_LENGTH    (0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region5_rw_end) -   \
                                            0x$(extract_symbol ${objtree}/${SYS_ELF} __ddr_region5_rw_start))"
echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
REGION_INFO="DSP_BUF_BASE    0x$(extract_symbol ${objtree}/${SYS_ELF} __dsp_buf_start)"
echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
REGION_INFO="DSP_BUF_LENGTH    (0x$(extract_symbol ${objtree}/${SYS_ELF} __dsp_buf_end) -   \
                                            0x$(extract_symbol ${objtree}/${SYS_ELF} __dsp_buf_start))"
echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
fi



# ---- Run function ---- #
#show_variables
#echo ${BUILD_CMD}
echo -e "  CC      ${__REGION_INFO_FILE__}.o"
${BUILD_CMD}
run_ld_cmd
run_nm_cmd
run_objcopy_cmd

echo "$0 Done..."
