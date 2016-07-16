#!/bin/bash

prepare_env()
{
	file=$1
	if [ -e $file ]; then
		ln -sf build/$file $file
		echo "ln -sf build/$file $file"
	else
		echo "Please run it under rtos"
	fi

}

prepare_env Makefile
prepare_env Kconfig
prepare_env .gitignore 

