#!/bin/bash

help()
{
	echo "Usage: $0 old_manifest new_manifest [action]"
	echo ""
	echo "       Where action can be log or patch"
	echo "       Please run $0 where .repo is located and manifests have to be the same."
	echo "       The output is git.log under rtos"
	echo ""
	echo ""
	echo "Ex:    $0 old.xml new.xml.       To generate git log per project."
	echo "Ex:    $0 old.xml new.xml log.   To generate git log per project."
	echo "Ex:    $0 old.xml new.xml patch. To generate git log with patch per project."

}

show_variables()
{
	echo -e "old_manifest=$old_manifest\n"
	echo -e "new_manifest=$new_manifest\n"
	echo -e "revision_old=\n$revision_old\n"
	echo -e "revision_new=\n$revision_new\n"
	echo -e "project_old=\n$project_old\n"
	echo -e "project_new=\n$project_new\n"
	echo -e "path_old=\n$path_old\n"
	echo -e "path_new=\n$path_new\n"
}

# git_log_revision_range [proj_path] [old_rev] [new_rev]
git_log_revision_range()
{
	proj_name=`echo $1 | sed "s/\"//g"`
	proj_path=`echo $2 | sed "s/\"//g"`
	old_rev=`echo $3 | sed "s/\"//g"`
	new_rev=`echo $4 | sed "s/\"//g"`
	action=$5

	if [[ ! -d .repo ]]; then
		echo "Please cd to the path which .repo located ..."
		exit 1
	fi

	printf "%-*s %-*s %s    %s\n" 40 "$proj_name"  32 "$proj_path" "$old_rev" "$new_rev"
	pushd . > /dev/null

	cd $proj_path
	echo -e "\n========== $proj_name ==========\n" >> $save_file

	if [[ $action == "patch" ]]; then
		git log --no-prefix -p --decorate  $old_rev..$new_rev >> $save_file
	else
		git log --name-only --decorate  $old_rev..$new_rev >> $save_file
	fi

	popd > /dev/null
}

if [[ $# < 2 ]]; then
	help
	exit
fi

old_manifest=$1
new_manifest=$2
action=$3

revision_old=$(sed -n -e "/project name/p" $old_manifest | awk -F'revision=' '{print $2}' | sed -n -e "s/upstream=.*//p")
revision_new=$(sed -n -e "/project name/p" $new_manifest | awk -F'revision=' '{print $2}' | sed -n -e "s/upstream=.*//p")

project_old=$(sed -n -e "/project name/p" $old_manifest  | awk -F'project name=' '{print $2}' | sed -n -e "s/path=.*//p")
project_new=$(sed -n -e "/project name/p" $new_manifest  | awk -F'project name=' '{print $2}' | sed -n -e "s/path=.*//p")

path_old=$(sed -n -e "/project name/p" $old_manifest  | awk -F'path=' '{print $2}' | sed -n -e "s/revision=.*//p")
path_new=$(sed -n -e "/project name/p" $new_manifest  | awk -F'path=' '{print $2}' | sed -n -e "s/revision=.*//p")


#show_variables
projnew_array=( $project_new )
projold_array=( $project_old )
pathnew_array=( $path_new )
pathold_array=( $path_old )
revold_array=( $revision_old )
revnew_array=( $revision_new )

echo total_projects=${#projnew_array[@]}

if [[ ! -d .repo ]]; then
	echo "Please cd to the path which .repo located ..."
	exit 1
fi

save_path=`pwd`
save_file=$save_path/"git.log"
rm -f $save_file

#
# ${#array[@]} outputs array length
# ${!array[@]} outputs a list of all existing array indexes
#
for i in ${!projnew_array[@]}; do
	projnew_member=${projnew_array[i]}
	projold_member=${projold_array[i]}

	pathnew_member=${pathnew_array[i]}
	pathold_member=${pathold_array[i]}

	revold_member=${revold_array[i]}
	revnew_member=${revnew_array[i]}

	# There is a constraint here. The projects in old and new manifest should have same order and number.
	# Only the project in the same order have git log.

	if [[ $projnew_member == $projold_member ]]; then
		git_log_revision_range $projnew_member $pathnew_member $revold_member $revnew_member $action
	else
		echo -e "\n========== $projnew_member ==========\n" >> $save_file
		echo -e " git log not saved."  >> $save_file
	fi
done

echo -e "\nPlease check $save_file"

