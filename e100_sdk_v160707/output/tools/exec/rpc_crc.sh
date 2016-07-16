#This shell script is used to check crc value of rpc headers and svae them to a crc file.
#$1: The generated crc.c.
#$2: The dir for crc_check
#$3: The rpc header is private or public.
#$4: The directory we'll looking for rpc headers.
#There may be serveral directories, so you can put them in $4 $5, $6 and so on.


# remove the existing crc.c
CRC_FILE=$1".c"
if [ -e $CRC_FILE ]; then
	rm -f $CRC_FILE
fi


# find the rpc headers for one directory
function find_rpc_header(){
	LIST=`ls $1 | grep "\.h"`
	echo $LIST
}


# the variables for public and private rpc are different
if [ "$3" = "public" ]; then
	CRC_COLLECTION="unsigned int crc_result[] = {"
	PROG_ID="int rpc_prog_id[] = {"
	RPCNUM="int rpc_num = "
else
	CRC_COLLECTION="unsigned int priv_crc_result[] = {"
	PROG_ID="int priv_rpc_prog_id[] = {"
	RPCNUM="int priv_rpc_num = "
fi

NEED_COMMA=0
COUNT=0

# remove the first three input arguements
shift 3
RPC_DIR=$#
# rpc headers may be distributed in the serveral directories
for (( cnt=1; cnt<=$RPC_DIR; cnt=cnt+1 ))
do
	DIR=$1
	FILE_LIST=$(find_rpc_header ${DIR})
	for i in $FILE_LIST
	do
		if [ "$i" = "AmbaIPC_Rpc_Def.h" ]; then
			continue
		fi

		if [ $NEED_COMMA -eq 1 ]; then
			CRC_COLLECTION=$CRC_COLLECTION", "
			PROG_ID=$PROG_ID", "
		fi
		HEADER=$DIR"/"$i
		CRC=`$AMBA_HOST_CRC $HEADER | awk '{print $3}'| awk -F '\r' '{print $1}'`
		CRC_COLLECTION=$CRC_COLLECTION$CRC"U"
		ID=`cat $HEADER | grep "PROG_ID" | grep "#define" | awk '{print $3}'| awk -F '\r' '{print $1}'`
		COUNT=`expr $COUNT + 1`
		PROG_ID=$PROG_ID$ID
		NEED_COMMA=1
	done
    shift 1
done
CRC_COLLECTION=$CRC_COLLECTION"};"
PROG_ID=$PROG_ID"};"
RPCNUM=$RPCNUM$COUNT";"

echo $CRC_COLLECTION > $CRC_FILE
echo $PROG_ID >> $CRC_FILE
echo $RPCNUM >> $CRC_FILE