#!/bin/bash

ptn="HELLO hello Hello"

# Force sort and uniq     
ptn=`echo ${ptn} | tr " " "\n" | sort -f | uniq  |  tr "\n" " "`
echo -e "Grep patterns: ${ptn}\n"

#exclude_list="--exclude-dir=esol --exclude-dir=gpl --exclude-dir=ExpressLogic --exclude=*.pdf"
exclude_list="--exclude=*.pdf"
log_file=log_pattern.txt

rm -f $log_file

for p in ${ptn}; do
	echo -e "\n========== $p ==========\n" >> $log_file

	# skip binary, while word, case-sensitive
	grep -Iwnr ${exclude_list} ${p} rtos >> $log_file
done

