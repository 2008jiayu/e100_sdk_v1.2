#!/bin/sh

if [ $# -lt 3 ]; then
        echo Usage: ${0} destination source a9/a12/...
else
        dst00=`echo ${1} | tr '[A-Z]' '[a-z]'`      # lower case only
        dst10=`echo ${dst00} | sed 's/\(.\)/\U\1/'` # upper case + lower case
        dst11=`echo ${1} | tr '[a-z]' '[A-Z]'`      # upper case only

        src00=`echo ${2} | tr '[A-Z]' '[a-z]'`
        src10=`echo ${src00} | sed 's/\(.\)/\U\1/'`
        src11=`echo ${2} | tr '[a-z]' '[A-Z]'`

        dst_folder="${dst00}_${3}"
        src_folder="${src00}_${3}"

        if [ -d ${dst_folder} ]; then
            echo "The destination folder '${dst_folder}' has already existed!!"
            echo "Please remove it before executing this script!!"
            exit 1
        fi

        echo Duplicating ${dst_folder} from ${src_folder} ...

        cp -rf ${src_folder} ${dst_folder}

        echo Change working dir to ${dst_folder}; cd ${dst_folder}

        echo Removing .svn folders ...
        find . -name ".svn" -type d | xargs --no-run-if-empty rm -r

        echo Renaming '*'${src00}'*' to '*'${dst00}'*' ...
        find . -type f | grep -v '.svn' | xargs --no-run-if-empty rename -f "s/${src00}/${dst00}/"

        echo Renaming '*'${src10}'*' to '*'${dst10}'*' ...
        find . -type f | grep -v '.svn' | xargs --no-run-if-empty rename -f "s/${src10}/${dst10}/"

        echo Renaming '*'${src11}'*' to '*'${dst11}'*' ...
        find . -type f | grep -v '.svn' | xargs --no-run-if-empty rename -f "s/${src11}/${dst11}/"

        echo Replacing string ${src00} with ${dst00} ...
        find . -type f -exec sed -i "s/${src00}/${dst00}/g" '{}' \;

        echo Replacing string ${src10} with ${dst10} ...
        find . -type f -exec sed -i "s/${src10}/${dst10}/g" '{}' \;

        echo Replacing string ${src11} with ${dst11} ...
        find . -type f -exec sed -i "s/${src11}/${dst11}/g" '{}' \;

        echo "Done!"
fi
