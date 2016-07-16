#!/bin/bash

SIZE=arm-none-eabi-size
AWK=gawk
# Column number to sort
SN=4

t=0
tout=1000

print_usage()
{
	echo "$0 [1-5]"
	echo -e "\t1-5 as column number to sort. Default is 4"
	echo -e "\tNeed environment variable ${AMBA_O_LIB} or ${AMBA_LIB_FOLDER}"

}

print_title()
{
	if [ "$((t % ${tout}))" = "0" ]; then
		echo "-----------------------------------------------------------------------------------------------------------------------"
		echo -e "Text\t\tData\t\tBss\t\tText+Data\tTotal\t\tName"
		echo "-----------------------------------------------------------------------------------------------------------------------"
	fi
	t=$((t+1))
}

dump_elf_size()
{
	if [ -e "${1}" ] && [ -s "${1}" ]; then
		${SIZE} -t ${1} | grep TOTALS | \
			${AWK} '{ if ($1 < 10000000) \
							printf "%d\t\t", $1; \
						else \
							printf "%d\t", $1; \
						if ($2 < 10000000) \
							printf "%d\t\t", $2; \
						else \
							printf "%d\t", $2; \
						if ($3 < 10000000) \
							printf "%d\t\t", $3; \
						else \
							printf "%d\t", $3; \
						if (($1+$2) < 10000000) \
							printf "%d\t\t", ($1+$2); \
						else \
							printf "%d\t", ($1+$2); \
						if ($4 < 10000000) \
							printf "%d\t\t", $4; \
						else \
							printf "%d\t", $4; \
					}'
		echo -e "${1}"
	else
		echo -e "\t*** File Missing ***\t\t\t\t\t\t${1}"
	fi
}


# -----------------------------------------------------------------------------

if [ "${AMBA_O_LIB}" != "" ]; then
	F_LIB=${AMBA_O_LIB}
	F_OUT=`echo ${AMBA_O_LIB} | sed 's!/lib!!'`/out
else
	F_LIB=${AMBA_LIB_FOLDER}
	F_OUT=out
fi
if [ ! -d ${F_LIB} ] || [ ! -e ${F_LIB} ]; then
	echo "${F_LIB} does not exist!"
	exit
fi
if [ ! -d ${F_OUT} ] || [ ! -e ${F_OUT} ]; then
	echo "${F_OUT} does not exist!"
	exit
fi

if [ ! -e `which ${SIZE}` ]; then
	echo "Can not find ${SIZE}"
	exit
fi

if [ "$#" = "1" ]; then
	if [ "$1" -gt "0" ] && [ "$1" -lt "6" ]; then
		SN=$1
	else
		print_usage
		exit
	fi
fi

echo "======================================================================================================================="
echo "                   Code Size Statistics"
echo "======================================================================================================================="

echo "-----------------------------------------------------------------------------------------------------------------------"
echo -e "\t\t*.a files"
print_title
rm -f size_a.txt
for f in `ls ${F_LIB}/*.a`; do
	dump_elf_size ${f} >> size_a.txt
done
# Pre-built libraries
if [ "${F_LIB}" != "${AMBA_LIB_FOLDER}" ]; then
	for f in `ls ${AMBA_LIB_FOLDER}/*.a`; do
		l=`echo ${f} | sed -e 's!${AMBA_LIB_FOLDER}!!g'`
		if [ ! -e ${F_LIB}/${l} ]; then
			dump_elf_size ${f} >> size_a.txt
		fi
	done
fi
# text, data, bss, text+data, total
sed -i -e "s!${srctree}/!!g" size_a.txt
cat size_a.txt | sort -n -k ${SN}
rm -f size_a.txt

echo -e "\n----------------------------------------------------------------------------------------------------------------------"
echo -e "\t\t*.elf files"
t=0
print_title
rm -f size_elf.txt
for f in `ls ${F_OUT}/*.elf`; do
	dump_elf_size ${f} >> size_elf.txt
done
# text, data, bss, text+data, total
sed -i -e "s!${srctree}/!!g" size_elf.txt
cat size_elf.txt | sort -n -k ${SN}
rm -f size_elf.txt

echo "======================================================================================================================="

