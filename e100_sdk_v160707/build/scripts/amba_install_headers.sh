#!/bin/bash

exec_install_files()
{
	src=$1
	if [ -e $src/install-files.sh ]; then
		${CONFIG_SHELL} $src/install-files.sh $src
	fi
}

exec_install_files comsvc/AmbaShell
exec_install_files comsvc/AmbaSys
exec_install_files comsvc/AmbaTest
exec_install_files comsvc/FwUpdate
exec_install_files comsvc/misc
exec_install_files image/imgcalib
exec_install_files image/imgproc
exec_install_files image/imgschdlr
exec_install_files image/ituner
exec_install_files image/utils
exec_install_files mw/cfs
exec_install_files mw/dataflow
exec_install_files mw/dspflow
exec_install_files mw/net
exec_install_files ssp/dsp/dspkernel
exec_install_files ssp/dsp/imgkernel
exec_install_files ssp/fs
exec_install_files ssp/kal
exec_install_files ssp/link
exec_install_files ssp/soc/B5
exec_install_files ssp/soc
exec_install_files ssp/usb
exec_install_files va/adas
exec_install_files va

