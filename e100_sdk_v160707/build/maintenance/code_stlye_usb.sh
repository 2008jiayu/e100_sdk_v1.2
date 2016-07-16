#!/bin/bash

format_code_module()
{
	astyle --lineend=linux --indent=tab --style=1tbs -n "$1"
}

help()
{
	echo "usage:"
	echo "  $0 [path]:"
	echo "    Ex: $0 ssp/usb/ambarella/inc/*.h will format herader files in ssp/usb/ambarella/inc/*.h"
}

if [ $# == 0 ]; then
	help
	exit
else
	for var in "$@"
	do
		format_code_module $var
	done

fi

echo "---- Code Format Done ----"
