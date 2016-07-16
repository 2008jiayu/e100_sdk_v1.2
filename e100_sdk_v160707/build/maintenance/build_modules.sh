#!/bin/bash

build_module()
{
	make $1-clean
	make $1-install 2>&1  | tee $1.log
	grep -w "AR\|INSTALL" $1.log  2>&1 | tee lib_$1.log
	cat lib_$1.log >> lib_all.log
}

help()
{
	echo "$0 -a:		Ex: build \"all_moduels\",   where all_modules = $ALL_MODULES"
	echo "$0 module_name:	Ex: build \"single_module\", where single_module=bsp"
}

ALL_MODULES="bsp tools audio ssp comsvc peripheral mw image va app"

if [ $# == 0 ]; then
	help
	exit
fi

rm -f lib_all.log

if [ "$1" == "-a" ]; then
	for m in $ALL_MODULES; do
		echo "building $m ..."
		build_module $m
	done
else
	build_module $1
fi


echo "---- Ready INSTALL libs ----"
cat lib_all.log
