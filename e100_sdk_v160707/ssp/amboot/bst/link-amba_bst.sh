#!/bin/bash

# Error out on error
set -e

# Delete output files in case of error
trap cleanup SIGHUP SIGINT SIGQUIT SIGTERM ERR
cleanup()
{
	echo "cleanup bst"

	rm -f ${objtree}/ssp/amboot/bst/.*.cmd
	rm -f ${objtree}/ssp/amboot/bst/*ddr3*
	rm -f ${objtree}/ssp/amboot/bst/*lpddr2*
	rm -f ${objtree}/ssp/amboot/bst/*lpddr3*
	rm -f ${KBUILD_AMBA_OUT_DIR}/amba_bst.*
	rm -f ${objtree}/ssp/amboot/bst/amba_bst.lds
}

#
#
# Use "make V=1" to debug this script
case "${KBUILD_VERBOSE}" in
*1*)
	set -x
	;;
esac

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

DRAM_TYPE=`grep CONFIG_DRAM ${KCONFIG_CONFIG} | grep "=y" | awk -F "_" '{printf $3}'`
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

if [ "${DRAM_TYPE}" == "DDR3" ]; then
	if [ "${BOOT_TYPE}" == "EMMC" ]; then
		LIBS="`chk_lib libsoc_bst_emmc_ddr3.a` \
		      ${objtree}/peripheral/libperipheral.a"
	elif [ "${BOOT_TYPE}" == "SPINOR" ]; then
		LIBS="`chk_lib libsoc_bst_nor_ddr3.a` \
		      ${objtree}/peripheral/libperipheral.a"
	else
		LIBS="`chk_lib libsoc_bst_ddr3.a` \
		      ${objtree}/peripheral/libperipheral.a"
	fi
	OUTPUT=${objtree}/ssp/amboot/bst/amba_bst_ddr3.elf
	MAP=${objtree}/ssp/amboot/bst/amba_bst_ddr3.map

elif [ "${DRAM_TYPE}" == "LPDDR2" ]; then
	if [ "${BOOT_TYPE}" == "EMMC" ]; then
		LIBS="`chk_lib libsoc_bst_emmc_lpddr2.a` \
		      ${objtree}/peripheral/libperipheral.a"
	elif [ "${BOOT_TYPE}" == "SPINOR" ]; then
		LIBS="`chk_lib libsoc_bst_nor_lpddr2.a` \
		      ${objtree}/peripheral/libperipheral.a"
	else
		LIBS="`chk_lib libsoc_bst_lpddr2.a` \
		      ${objtree}/peripheral/libperipheral.a"
	fi
	OUTPUT=${objtree}/ssp/amboot/bst/amba_bst_lpddr2.elf
	MAP=${objtree}/ssp/amboot/bst/amba_bst_lpddr2.map

else
	if [ "${BOOT_TYPE}" == "EMMC" ]; then
		LIBS="`chk_lib libsoc_bst_emmc_lpddr3.a` \
		      ${objtree}/peripheral/libperipheral.a"
	elif [ "${BOOT_TYPE}" == "SPINOR" ]; then
		LIBS="`chk_lib libsoc_bst_nor_lpddr3.a` \
		      ${objtree}/peripheral/libperipheral.a"
	else
		LIBS="`chk_lib libsoc_bst_lpddr3.a` \
		      ${objtree}/peripheral/libperipheral.a"
	fi
	OUTPUT=${objtree}/ssp/amboot/bst/amba_bst_lpddr3.elf
	MAP=${objtree}/ssp/amboot/bst/amba_bst_lpddr3.map
fi

if [ ${HOST_OS} == "CYGWIN" ]; then
	LIBS=`echo ${LIBS} | sed -e 's@/cygdrive/\([a-zA-Z]\)@\1:@g'`
fi

if [ ${KBUILD_AMBA_OUT_DIR} ]; then
	LINK_AMBA_OUT_DIR=${KBUILD_AMBA_OUT_DIR}
	mkdir -p ${LINK_AMBA_OUT_DIR}
else
	LINK_AMBA_OUT_DIR=${DIR_amba_bst}
fi

OUTPUT_BIN=${LINK_AMBA_OUT_DIR}/amba_bst.bin

LOCAL_LDFLAGS="-nostartfiles -mthumb-interwork -L${AMBA_O_LIB} -L${AMBA_LIB_FOLDER} -L${objtree}/peripheral"

BUILD_CMD="${CC} ${LOCAL_LDFLAGS} -Wl,-T${LDS_amba_bst} -Wl,--start-group ${LIBS} -Wl,--end-group -Wl,-Map=${MAP} -o ${OUTPUT}"

OBJCOPY_CMD="${OBJCOPY} -j .bst_stage1 -j .bst_stage2 -j .data -S -g -O binary ${OUTPUT} ${OUTPUT_BIN}"

#echo "================================================="
#echo ""
#echo "LD=${LD}"
#echo "LOCAL_LDFLAGS=${LOCAL_LDFLAGS}"
#echo "srctree=${srctree}"
#echo "objtree=${objtree}"
#echo "LINK_LIBS=${LIBS}"
#echo ""
#echo "lds=${LDS_amba_bst}"
#echo "LINK_OUTPUT=${OUTPUT}"
#echo "BUILD_CMD=${BUILD_CMD}"
#echo "OBJCOPY_CMD=${OBJCOPY_CMD}"
#echo ""
#echo "KCONFIG=${KCONFIG_CONFIG}"
#echo "DRAM_TYPE=${DRAM_TYPE}"
#echo "================================================="

${BUILD_CMD}
${OBJCOPY_CMD}
