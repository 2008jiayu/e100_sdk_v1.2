# $1 the file of the delete list
# $2 the new release patch
# $3 the dir of old release src
# $4 the dir of new release bin
exec < $1
read Line

# Remove character /
old_release_root=`echo $3 | awk -F '/' '{print $1}'`
new_release_root=`echo $4 | awk -F '/' '{print $1}'`

cp $2 $3
cd $3
# Patch the release patch
patch -p1 < $2
rm -f $2
# Delete the files which does not exist anymore
rm -rf $Line
old_dir=$old_release_root"/"
new_dir=$new_release_root"/*"
cd ..
# Copy the binary from new release to old release
cp $new_dir $old_dir -rf