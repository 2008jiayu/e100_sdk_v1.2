#!/bin/bash

if [ $# != 1 ]; then
	echo "Usage:"
	echo -e "\t$0 libs-folder"
	echo -e "\t$0 lib"
	exit
fi

files=

p=$1

if [ -e ${p} ]; then
	if [ -d ${p} ]; then
		files=`find ${p} -type f | grep "\.a$" | sort`
	else
		files=${p}
	fi
fi

for f in ${files}; do
	# GCC
	info=`arm-none-eabi-strings ${f} | grep GCC | head -1`
	if [ "${info}" == "" ]; then
		# ARM CC
		info=`arm-none-eabi-strings ${f} | grep Compiler | head -1`
	fi
	if [ "${info}" == "" ]; then
		# GNU AS
		info=`arm-none-eabi-strings ${f} | grep GNU | head -1`
	fi

	if [ "${info}" == "" ]; then
		echo -e "${f}:\n\t(unknown)"
	else
		echo -e "${f}:\n\t${info}"
	fi
done

