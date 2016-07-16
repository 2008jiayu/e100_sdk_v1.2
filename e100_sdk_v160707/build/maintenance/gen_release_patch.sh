# $1 The old release tarball
# $2 The new release tarball
# $3 Target sdk name
DIFF_FILE="rtos.diff"

# These files are output files for release
OVERWRITE_BIN_DIR=$3"_release_bin_`date +%Y%m%d`"
RELEASE_PATCH=$3"_release_"`date +%Y%m%d`".patch"
DELETE_LIST="delete_files_"`date +%Y%m%d`

# Remove character /
old_release_dir=`echo $1 | awk -F '/' '{print $1}'`
new_release_dir=`echo $2 | awk -F '/' '{print $1}'`

# Generate the diff between the old and new release
diff -rq $old_release_dir $new_release_dir > $DIFF_FILE

if [ -e $OVERWRITE_BIN_DIR ]; then
	rm -rf $OVERWRITE_BIN_DIR
fi

if [ -e $RELEASE_PATCH ]; then
	rm -rf $RELEASE_PATCH
fi

mkdir $OVERWRITE_BIN_DIR

exec < $DIFF_FILE

while read Line
do
	# Find the file location in old release and new release
	Only_Or_Both=`echo "$Line" | awk '{print $1}'`

	# It means that the file only exists in old or new release.
	if [ "$Only_Or_Both" = "Only" ]; then
		only_file=`echo "$Line" | awk '{print $3}' | awk -F ':' '{print $1}'`
		only_file=$only_file"/"`echo "$Line" | awk '{print $4}'`
		only_file_root=`echo "$only_file" | awk -F '/' '{print $1}'`
		only_file=`echo "$only_file" | sed -n "s/$only_file_root//p"`

		prev_file=$old_release_dir$only_file
		prev_file_root=$old_release_dir
		cur_file=$new_release_dir$only_file
		cur_file_root=$new_release_dir
	else
		prev_file=`echo "$Line" | awk '{print $2}'`
		prev_file_root=`echo "$prev_file" | awk -F '/' '{print $1}'`
		cur_file=`echo "$Line" | awk '{print $4}'`
		cur_file_root=`echo "$cur_file" | awk -F '/' '{print $1}'`
	fi

	# The file may exist only in old or new release.
	if [ -e "$cur_file" ]; then
		# Get the file type
		file_type=`file "$cur_file" | grep "text"`
		file=`echo "$cur_file" | sed -n "s/$cur_file_root//p"`
	else
		file_type=`file "$prev_file" | grep "text"`
		file=`echo "$prev_file" | sed -n "s/$prev_file_root//p"`
		# The file does not exist in new release anymore,
		# so keep it in delete list
		del_files=$del_files" "`echo "$file" | sed -n 's/\///p'`
	fi


	if [ -z "$file_type" ]; then
		# The file type is binary.
		dest_dir=$OVERWRITE_BIN_DIR`echo "$file" | sed 's#\(.*/\).*#\1#'`
		if [ -e "$cur_file" ]; then
			# Copy binary from new to old release.
			mkdir -p "$dest_dir"
			cp -rf "$cur_file" "$dest_dir"
		fi

	else
		# The file type belongs to text type, so generate the patch
		diff -Naur "$prev_file" "$cur_file" >> $RELEASE_PATCH
	fi
done

echo $del_files > $DELETE_LIST
rm -f $DIFF_FILE
tar zcvf "${OVERWRITE_BIN_DIR}.tar.gz" $OVERWRITE_BIN_DIR