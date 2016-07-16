#!/bin/bash

if [[ $# == 0 || "$1" == "-h" || "$1" == "--help" ]]; then
	echo "Usage: $0: OPTION DATE [PROJECT]"
	echo "The option is:"
	echo "-h --help         Help"
	echo "-a --all          Checkout all projects by date"
	echo "-s --single       Checkout single projects by date. Need to under git project dir"
	echo "-p --print        Print out the version"
	echo ""
	echo "Ex:"
	echo "Checkout single project by date:    $0 -s \"2014-12-01 00:00\""
	echo "Checkout single project by date:    $0 -s \"2014-12-01 00:00\" bsp"
	echo "Checkout all projects by date:      $0 -a \"2014-12-01 00:00\""
	echo "Print version of project by date:   $0 -p \"2014-12-01 00:00\""
	echo "Print version of project by date:   $0 -p \"2014-12-01 00:00\" bsp"

	exit 0
fi

# git_checkout_by_date [project] [date] [co|show]
git_checkout_by_date()
{
	project="$1"
	date="$2"
	action="$3"

	if [[ ! -d build && ! -d vendors ]]; then
		echo "Please cd to rtos tree top ..."
		exit 1
	fi

	pushd . > /dev/null
	cd "$project"
	proj_rev=`git rev-list --all -n1 --before="$date"`
	printf "%-*s %s\n" 32 "$project" "${proj_rev}"

	if [ $action == "co" ]; then
		git checkout ${proj_rev}
	fi

	popd > /dev/null
}

#--- start ---

option="$1"
date="$2"
project="$3"
action="co"

if [[ "$option" == "-p" || "$option" == "--print" ]]; then
	action="show"

	if [[ "$project" != "" ]]; then
		git_checkout_by_date "$project" "$date" "$action"
	else
		projects=`repo list -p`
		for i in ${projects}; do
			git_checkout_by_date "${i}" "$date" "$action"
		done
	fi
fi

if [[ "$option" == "-s" || "$option" == "--single" ]]; then
	if [[ "$project" != "" ]]; then
		# checkout  by date under rtos dir
		git_checkout_by_date "$project" "$date" "$action"
	else
		# checkout by date under projct dir
		proj_rev=`git rev-list --all -n1 --before="$date"`
		echo "${proj_rev}"
		git checkout ${proj_rev}
	fi
fi

if [[ "$option" == "-a" || "$option" == "--all" ]]; then
	projects=`repo list -p`
	for i in ${projects}; do
		git_checkout_by_date "${i}" "$date" "$action"
	done
fi
