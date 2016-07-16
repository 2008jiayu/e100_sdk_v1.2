#!/bin/bash

# ---- Functions declaration ---- #

cleanup()
{
	echo "cleanup app"

	if [ -s ${objtree}/${KBUILD_AMBA_OUT_DIR}/ ]; then
		find ${objtree}/${KBUILD_AMBA_OUT_DIR}/ \
			\( \
			   -name "${__LINK_FILE_STEM}.*" \
			\) -type f -print | xargs rm -f
	fi
}

show_variables()
{
	echo "================================================="
	echo "---- show_variables() ----"
	echo ""
	echo "cmd_link-amba_app: ${0} ${1} ${2} ${3} ${4}"
	echo "arg0=${0} arg1=${1} arg2=${2} arg3=${3} arg4=${4}"
	echo ""
	echo "---- Global variable ----"
	echo "LD=${LD}"
	echo "LDS_amba_app=${LDS_amba_app}"
	echo "LDFLAGS=${LDFLAGS}"
	echo "LDFLAGS_amba_app=${LDFLAGS_amba_app}"
	echo "srctree=${srctree}"
	echo "objtree=${objtree}"
	echo "DIR_amba_app=${DIR_amba_app}"
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

build_ver()
{
	if [ ${KBUILD_VERBOSE} == 0 ]; then
		echo "  GEN     ${AMBA_VER_FILE}"
		echo "  CC      ${AMBA_VER_OBJ}"
	fi

	for i in ${AMBA_LIBS}; do
		if [ ! ${AMBA_LIBS_ARRAY} ]; then
			AMBA_LIBS_ARRAY="\"${i}\""
		else
			AMBA_LIBS_ARRAY="${AMBA_LIBS_ARRAY},\"${i}\""
		fi
	done

    echo "/*"                                                                   >  ${AMBA_VER_FILE}
    echo " * Automatically generated file: don't edit"                          >> ${AMBA_VER_FILE}
    echo " */"                                                                  >> ${AMBA_VER_FILE}
    echo "const char *pAmbaVer_LinkVer_Target = \"${__LINK_FILE_STEM}.elf\";"   >> ${AMBA_VER_FILE}
    echo "const char *pAmbaVer_LinkVer_Machine = \"`whoami`@`hostname`\";"      >> ${AMBA_VER_FILE}
    echo "const char *pAmbaVer_LinkVer_Date = \"`date +"%I:%M:%S %-d/%-m/%-Y" `\";"   		>> ${AMBA_VER_FILE}
    echo "const char *pAmbaVer_LinkLibs[] = {${AMBA_LIBS_ARRAY}};"              >> ${AMBA_VER_FILE}
    echo "unsigned int AmbaVer_LinkLibsNum = ${AMBA_LIBS_NUM};"                 >> ${AMBA_VER_FILE}
    echo ""                                                                     >> ${AMBA_VER_FILE}

    LOCAL_CFLAGS="`echo ${KBUILD_CFLAGS} | sed 's/\"//g'`"
    BUILD_AMBA_VER_CMD="${CC} ${LOCAL_CFLAGS} -c ${AMBA_VER_FILE} -o ${AMBA_VER_OBJ}"
    ${BUILD_AMBA_VER_CMD}
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
	if [ ${KBUILD_VERBOSE} == 0 ]; then
		echo "  GEN     ${RPC_CRC_OUTPUT}.c"
		echo "  CC      ${RPC_CRC_OUTPUT}.o"
		echo "  GEN     ${RPC_CRC_PRV_OUTPUT}.c"
		echo "  CC      ${RPC_CRC_PRV_OUTPUT}.o"
	fi

	${RPC_CRC_CMD}
	${BUILD_RPC_CRC_CMD}
	${RPC_CRC_PRV_CMD}
	${BUILD_RPC_CRC_PRV_CMD}
}

run_compress_cmd()
{
        if [ "${BOOT_TYPE}" == "SPINOR" ]; then
                gzip -c -9 ${LINK_OUTPUT_BIN} > ${LINK_OUTPUT_COMPRESS_BIN}
                cp ${LINK_OUTPUT_BIN} ${LINK_OUTPUT_TMP_BIN}
                mv ${LINK_OUTPUT_COMPRESS_BIN} ${LINK_OUTPUT_BIN}
        else
                cp ${LINK_OUTPUT_BIN} ${LINK_OUTPUT_TMP_BIN}
        fi
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

__LINK_FILE_STEM="amba_app"

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


# ---- Definition for link target dependent ---- #

AMBA_LIBS="libbsp.a
           libthreadx.a
           libkal.a
           libprfile.a
           libfs.a
           libusb.a
           libusb_driver.a
           libusb_cdcacm.a
           libusb_dfu.a
           libusb_hid.a
           libusb_msc.a
           libusb_mtp.a
           libusb_simple.a
           libusb_stream.a
           libusb_uvc.a
           libusb_hostclasses.a
           libusb_hostcontroller.a
           libusb_pictbridge.a
           liblink.a
           libsoc.a
           libsoc_test.a
           libdsp_dspkernel.a
           libdsp_imgkernel.a
           libcomsvc_shell.a
           libcomsvc_test.a
           libcomsvc_sys.a
           libcomsvc_fwupdate.a
           libcomsvc_misc.a
		   libcomsvc_tune.a
           libaudio.a
           libaudio_sys.a
           libimg_schdlr.a
           libimg_proc.a
           libimg_calib.a
		   libimg_calibalgo.a
           libimg_ituner.a
           libimg_utils.a
           libimg_awb.a
           libimg_ae.a
           libimg_adj.a
           libimg_eis.a
		   libimg_encmonitor.a
           libva_adas.a
           libperipheral.a
           libmw_cfs.a
           libmw_dataflow.a
           libmw_dspflow.a
           libmw_net.a
		   libapplib.a
           libapp.a"

if [ -s ${AMBA_LIB_FOLDER}/libva_ivadas.a ]; then
	AMBA_LIBS="${AMBA_LIBS} libva_ivadas.a"
fi

if [ "${CONFIG_BUILD_GPL_LIB}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libgpl.a"
fi

if [ "${CONFIG_BUILD_FOSS_LIB}" = "y" ]; then
        AMBA_LIBS="${AMBA_LIBS} libfoss.a"
fi

if [ "${CONFIG_ENABLE_ECL_ADAS}" = "y" ]; then
        AMBA_LIBS="${AMBA_LIBS} libecladas.a"
fi

if [ "${CONFIG_BT}" = "y" ]; then
	# BT Wrapper from Ambarella
	if [ "${CONFIG_BT_AMBA}" = "y" ]; then
		AMBA_LIBS="${AMBA_LIBS} libbt_amba.a"
	fi
	# BT Stacks
	if [ "${CONFIG_BT_BRCM}" = "y" ]; then
		AMBA_LIBS="${AMBA_LIBS} libbt_brcm.a"
	fi
fi

if [ "${CONFIG_SBRIDGE_AMBA_B5}" = "y" ]; then  
   AMBA_LIBS="${AMBA_LIBS} libb5.a"  
fi  

AMBA_LIBS=$(echo ${AMBA_LIBS} | tr  " " "\n" | sort | tr  "\n" " ")
AMBA_LIBS_NUM=$(echo ${AMBA_LIBS} | wc -w)

__LINK_AMBA_LIBS="--whole-archive"
for i in ${AMBA_LIBS}; do
	if [ -s ${AMBA_O_LIB}/${i} ]; then
		__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} ${AMBA_O_LIB}/${i}"
	elif [ -s ${AMBA_LIB_FOLDER}/${i} ]; then
		__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} ${AMBA_LIB_FOLDER}/${i}"
	else
		echo "Can not found ${i}"
		exit 1
	fi
done
if [ ${HOST_OS} == "CYGWIN" ]; then
	__LINK_AMBA_LIBS=`echo ${__LINK_AMBA_LIBS} | sed -e 's@/cygdrive/\([a-zA-Z]\)@\1:@g'`
fi
__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} --no-whole-archive"
__LINK_AMBA_LIBS=$(echo ${__LINK_AMBA_LIBS} | tr -s [:space:])


__LINK_C_LIBS="-lc -lnosys -lm -lgcc -lrdimon"
if [ "${CONFIG_CC_CXX_SUPPORT}" = "y" ]; then
	__LINK_C_LIBS="${__LINK_C_LIBS} -lstdc++"
fi

# ld serachdir: -L option
__LINK_LIBS_INC=

# ---- AmbaVer_LinkInfo ------- #
__AMBA_VER_STEM=AmbaVer_LinkInfo
AMBA_VER_FILE="${DIR_amba_app}/${__AMBA_VER_STEM}.c"
AMBA_VER_OBJ="${DIR_amba_app}/${__AMBA_VER_STEM}.o"

# ---- For calculating rpc crc in AmbaLink ------- #
RPC_CFLAGS="`echo ${KBUILD_CFLAGS} | sed 's/\"//g'`"
RPC_CRC_EXEC_DIR="${srctree}/tools/exec"
RPC_CRC_SH="${RPC_CRC_EXEC_DIR}/rpc_crc.sh"

RPC_CRC_OUTPUT="${DIR_amba_app}/AmbaIPC_RpcHeaderCrc"
RPC_CRC_TYPE="public"
RPC_CRC_HEADERS="${srctree}/vendors/ambarella/inc/ssp/link/rpcprog"
RPC_CRC_CMD="${RPC_CRC_SH} ${RPC_CRC_OUTPUT} ${RPC_CRC_EXEC_DIR} ${RPC_CRC_TYPE} ${RPC_CRC_HEADERS}"
BUILD_RPC_CRC_CMD="${CC} ${RPC_CFLAGS} -c ${RPC_CRC_OUTPUT}.c -o ${RPC_CRC_OUTPUT}.o"

RPC_CRC_PRV_OUTPUT="${DIR_amba_app}/AmbaIPC_PrivateRpcHeaderCrc"
RPC_CRC_PRV_TYPE="private"
RPC_CRC_PRV_HEADERS="${srctree}/vendors/ambarella/inc/mw/net/rpcprog"
RPC_CRC_PRV_CMD="${RPC_CRC_SH} ${RPC_CRC_PRV_OUTPUT} ${RPC_CRC_EXEC_DIR} ${RPC_CRC_PRV_TYPE} ${RPC_CRC_PRV_HEADERS}"
BUILD_RPC_CRC_PRV_CMD="${CC} ${RPC_CFLAGS} -c ${RPC_CRC_PRV_OUTPUT}.c -o ${RPC_CRC_PRV_OUTPUT}.o"

# ---- Definition general rules for link target ---- #
if [ "$CONFIG_ENABLE_AMBALINK" = "y" ]; then
	LINK_OBJS="${RPC_CRC_OUTPUT}.o ${RPC_CRC_PRV_OUTPUT}.o ${AMBA_VER_OBJ}"
else
	LINK_OBJS="${AMBA_VER_OBJ}"
fi

LINK_LIBS="${__LINK_AMBA_LIBS} ${__LINK_C_LIBS}"
LINK_LDS="${LDS_amba_app}"
LINK_LDFLAGS="${LDFLAGS} ${LDFLAGS_amba_app} ${__LINK_LIBS_INC}"

if [ ${KBUILD_AMBA_OUT_DIR} ]; then
	LINK_AMBA_OUT_DIR=${KBUILD_AMBA_OUT_DIR}
	mkdir -p ${LINK_AMBA_OUT_DIR}
else
	LINK_AMBA_OUT_DIR=${DIR_amba_app}
fi

LINK_OUTPUT_ELF="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}.elf"
LINK_OUTPUT_BIN="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}.bin"
LINK_OUTPUT_MAP="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}.map"
LINK_OUTPUT_NM="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}.nm"
LINK_OUTPUT_COMPRESS_BIN="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}.bin.gz"
LINK_OUTPUT_TMP_BIN="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}.bin.tmp"
BOOT_TYPE=`grep "CONFIG_ENABLE*" ${KCONFIG_CONFIG} | grep "=y" | grep "BOOT" | awk -F "_" '{printf $3}'`

LINK_CMD="${LD} ${LINK_LDFLAGS} -o ${LINK_OUTPUT_ELF} -T ${LINK_LDS} --start-group ${LINK_LIBS} --end-group ${LINK_OBJS} -Map=${LINK_OUTPUT_MAP}"
NM_CMD="${NM} -n -l ${LINK_OUTPUT_ELF}"
OBJCOPY_CMD="${OBJCOPY} -O binary ${LINK_OUTPUT_ELF} ${LINK_OUTPUT_BIN}"

# ---- Run function ---- #
if [ "$CONFIG_ENABLE_AMBALINK" = "y" ]; then
	rpc_crc_calc
fi

# show_variables
build_ver
run_ld_cmd
run_nm_cmd
run_objcopy_cmd
run_compress_cmd

echo "$0 Done..."
