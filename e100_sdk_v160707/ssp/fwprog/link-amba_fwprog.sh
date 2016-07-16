#!/bin/bash

# Error out on error
set -e

# Delete output files in case of error
trap cleanup SIGHUP SIGINT SIGQUIT SIGTERM ERR
cleanup()
{
	echo "cleanup FwProg"

	rm -rf ${objtree}/${KBUILD_AMBA_FWPROG_OUT_DIR}
	rm -rf ${objtree}/${DIR_amba_fwprog}/${DIR_amba_fwprog_out}
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

rm -rf ${objtree}/${KBUILD_AMBA_FWPROG_OUT_DIR}/devfw/
mkdir -p ${objtree}/${KBUILD_AMBA_FWPROG_OUT_DIR}/devfw/

if [ ${HOST_OS} == "CYGWIN" ]; then
	srctree=`echo ${srctree} | sed -e 's@/cygdrive/\([a-zA-Z]\)@\1:@g'`
	objtree=`echo ${objtree} | sed -e 's@/cygdrive/\([a-zA-Z]\)@\1:@g'`
	${objtree}/tools/exec/win/host_sdfw_gen ${objtree}/${DIR_amba_fwprog}/.temp ${objtree}/${KBUILD_AMBA_FWPROG_OUT_DIR}/devfw/ ${objtree}/${DIR_amba_fwprog}/
else
	${objtree}/tools/exec/lnx/host_sdfw_gen ${objtree}/${DIR_amba_fwprog}/.temp ${objtree}/${KBUILD_AMBA_FWPROG_OUT_DIR}/devfw/ ${objtree}/${DIR_amba_fwprog}/
fi

echo "Build fwprog done!"
echo "===> All *.elf files are put at ${objtree}/${KBUILD_AMBA_FWPROG_OUT_DIR}"

