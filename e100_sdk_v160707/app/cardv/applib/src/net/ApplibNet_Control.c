#include <net/ApplibNet_Control.h>
#include <net/ApplibNet.h>
#include <net/NetCtrl.h>
#include <net/Json_Utility.h>
#include <net/NetUtility.h>
#include "../AppLibTask_Priority.h"

#define NETCTRL_DBG_EN
#define NETCTRL_ERR_EN

#undef NETCTRL_DBG
#ifdef NETCTRL_DBG_EN
#define NETCTRL_DBG(fmt,args...) AmbaPrint(fmt,##args);
#else
#define NETCTRL_DBG(fmt,args...)
#endif

#undef NETCTRL_ERR
#ifdef NETCTRL_ERR_EN
#define NETCTRL_ERR(fmt,args...) AmbaPrintColor(RED,fmt,##args);
#else
#define NETCTRL_ERR(fmt,args...)
#endif



#define NETCTRL_MGR_STACK_SIZE    (0x3800) ///need to be updated
#define NETCTRL_MGR_NAME          "AppLib_Net_Control_Manager"
#define NETCTRL_MGR_MSGQUEUE_SIZE (8)

#define NETCTRL_INVALID_SESSION (0)

#define MAX_CMD_INSTANCE (2)

typedef struct _NETCTRL_MGR_s_ {
    UINT8 Stack[NETCTRL_MGR_STACK_SIZE];  /**< Stack */
    UINT8 MsgPool[sizeof(APP_NETCTRL_MESSAGE_s)*NETCTRL_MGR_MSGQUEUE_SIZE];   /**< Message memory pool. */
    AMBA_KAL_TASK_t Task;               /**< Task ID */
    AMBA_KAL_MSG_QUEUE_t MsgQueue;      /**< Message queue ID */
    AMBA_KAL_MUTEX_t MutexActiveSession;
    AMBA_KAL_MUTEX_t MutexArgBuf;
} NETCTRL_MGR_s;

typedef struct _NETCTRL_SESSION_INFO_s_ {
    volatile int TokenNumber; // 0: invalid token number
    volatile UINT32 ClientId;
}NETCTRL_SESSION_INFO_s;

typedef struct _NETCTRL_DATA_SVC_MGR_s_ {
    char Filename[64];
    UINT8 Transferring;
    UINT8 Canceled;
}NETCTRL_DATA_SVC_MGR_s;


static UINT32 gCurCmdInstanceIdx = 0;
static AMP_NETCTRL_HDLR_INFO_s gCmdInstance[MAX_CMD_INSTANCE] = {0};
static AMP_NETCTRL_DATASVC_HDLR_INFO_s gDataInstance[1] = {0};


static UINT8 gNetCtrlInited = 0;
static NETCTRL_MGR_s gNetCtrl = {0};
static NETCTRL_SESSION_INFO_s gActiveSession = {0};
static char gCmdArgBuf[1024] = {0};
static unsigned char gActiveClientInfo[128] = {0};
static char gActiveTransportType[16] = {0};

static char *gJsonStringBuf = NULL;
static char *gJsonStringBufRaw = NULL;
static int JsonStringBufSize = 32;

static char ComposeCmdBuf[1024] = {0};
static char RequestThumbType[12] = {0};

#define FLIE_XFER_GET   (0)
#define FLIE_XFER_PUT   (1)
#define FLIE_XFER_CHANNEL_NUM   (2) // boundary

#define DATA_SVC_EVENT_GET_DONE (1<<0)
#define DATA_SVC_EVENT_PUT_DONE (1<<1)
#define SESSION_QUERY_EVENT_HOLDER_KEEP  (1<<2)
#define SESSION_QUERY_EVENT_HOLDER_GIVEUP  (1<<3)

static NETCTRL_DATA_SVC_MGR_s gDataSvcMgr[FLIE_XFER_CHANNEL_NUM] = {0};
static AMBA_KAL_MUTEX_t gMutexDataSvcMgr = {0};
static AMBA_KAL_EVENT_FLAG_t gDataSvcEvent = {0};

//-------------------------------------------------------------------
// Active session  management
//-------------------------------------------------------------------
static inline void SetActiveSession(int TokenNumber, UINT32 ClientId)
{
    AmbaKAL_MutexTake(&gNetCtrl.MutexActiveSession, AMBA_KAL_WAIT_FOREVER);
    gActiveSession.TokenNumber = TokenNumber;
    gActiveSession.ClientId = ClientId;
    AmbaKAL_MutexGive(&gNetCtrl.MutexActiveSession);
}

static inline void GetActiveSession(int *pTokenNumber, UINT32 *pClientId)
{
    AmbaKAL_MutexTake(&gNetCtrl.MutexActiveSession, AMBA_KAL_WAIT_FOREVER);
    *pTokenNumber = gActiveSession.TokenNumber;
    *pClientId = gActiveSession.ClientId;
    AmbaKAL_MutexGive(&gNetCtrl.MutexActiveSession);
}

static inline void GetActiveSessionToken(int *pTokenNumber)
{
    AmbaKAL_MutexTake(&gNetCtrl.MutexActiveSession, AMBA_KAL_WAIT_FOREVER);
    *pTokenNumber = gActiveSession.TokenNumber;
    AmbaKAL_MutexGive(&gNetCtrl.MutexActiveSession);
}

static inline void ResetActiveSession(void)
{
    AmbaKAL_MutexTake(&gNetCtrl.MutexActiveSession, AMBA_KAL_WAIT_FOREVER);
    gActiveSession.TokenNumber = NETCTRL_INVALID_SESSION;
    AmbaKAL_MutexGive(&gNetCtrl.MutexActiveSession);
}

static int FindCmdInstance(AMP_NETCTRL_HDLR_INFO_s *pstCmdHlr, UINT32 *RetIndex)
{
    int i = 0;

    if ((!pstCmdHlr) || (!RetIndex)) {
        return -1;
    }

    for (i=0;i<MAX_CMD_INSTANCE;i++) {
        if ((pstCmdHlr->InstTypeId ==  gCmdInstance[i].InstTypeId) &&
            ((UINT32)pstCmdHlr->Hdlr == (UINT32)gCmdInstance[i].Hdlr)) {
            *RetIndex = i;
            return 0;
        }
    }

    return -1;
}


//-------------------------------------------------------------------
// Session token management
//-------------------------------------------------------------------
static int GenNewToken(int *pTokenNumber)
{
    /* token number '0' is reserved for invalid session*/
    static int NewTokenNum = 1;

    if (pTokenNumber == NULL) {
        NETCTRL_ERR("[AppLib - NetCtrl] <GetNewTokenNumber> pTokenNumber is NULL");
        return NG;
    }

    *pTokenNumber = NewTokenNum;

    NewTokenNum++;
    if (NewTokenNum <= 0) {
        NewTokenNum = 1;
    }

    //NETCTRL_DBG("[AppLib - NetControl] <GetNewTokenNumber> Get token number %d",*pTokenNumber);

    return OK;
}

static int GenNewSessionToken(int *pTokenNum)
{
    int ReturnValue = 0;
    //int TokenNumber = NETCTRL_INVALID_SESSION;
    //UINT32 ClientId = 0;

    if (pTokenNum == NULL) {
         NETCTRL_ERR("[AppLib - NetControl] <RecvCmd> pTokenNum is NULL.");
        return NG;
    }

#if 0
    GetActiveSession(&TokenNumber, &ClientId);
    if (TokenNumber != NETCTRL_INVALID_SESSION) {
        NETCTRL_ERR("[AppLib - NetControl] <RecvCmd> There's a session existed already");
        return NG;
    }
#endif

    ReturnValue = GenNewToken(pTokenNum);
    if (ReturnValue != OK) {
        NETCTRL_ERR("[AppLib - NetControl] <RecvCmd> Get token number fail");
        return NG;
    }

    return OK;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
static int MapErrorCode(AMP_NETCTRL_ERROR_e Error)
{
    int MappedErrorCode = 0;

    switch (Error) {
    case AMP_NETCTRL_UNKNOWN_ERROR:
    default:
        MappedErrorCode = ERROR_NETCTRL_UNKNOWN_ERROR;
        break;
    case AMP_NETCTRL_INVALID_PATH:
        MappedErrorCode = ERROR_NETCTRL_INVALID_PATH;
        break;
    case AMP_NETCTRL_INVALID_PARAM:
        MappedErrorCode = ERROR_NETCTRL_INVALID_PARAM;
        break;
    case AMP_NETCTRL_INVALID_OPERATION:
        MappedErrorCode = ERROR_NETCTRL_INVALID_OPERATION;
        break;
    case AMP_NETCTRL_NO_MORE_MEMORY:
        MappedErrorCode = ERROR_NETCTRL_NO_MORE_MEMORY;
        break;
    }

    return MappedErrorCode;
}

static int GetWifiStatus(void)
{
#if 0
    return 1;
#else
    char *pParam = NULL;
    char *pParamRaw = NULL;
    int ParamMaxLen = 1024;
    AMP_NETCTRL_LNXCMD_RESULT_s LnxResult;
    int WifiIsOn = 0;
    int ReturnValue = 0;

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&pParam, (void**)&pParamRaw, ParamMaxLen, 32);
    if (ReturnValue < 0) {
        NETCTRL_ERR("[AppLib - NetControl] <GetWifiStatus> Allocate buffer for param fail");
        return -1;
    }

    memset(pParam, 0, ParamMaxLen);

    AmpNetCtrl_GetWifiStatus(NULL, pParam, ParamMaxLen, &LnxResult);
    AmbaPrintColor(CYAN,"[AppLib - NetControl] <GetWifiStatus> %s",pParam);

    if (strstr(pParam, "\"status\":\"enabled\"") != NULL) {
        WifiIsOn = 1;
    } else if (strstr(pParam, "\"status\":\"disabled\"") != NULL){
        WifiIsOn = 0;
    } else {
        WifiIsOn = -1;
        NETCTRL_ERR("[AppLib - NetControl] <GetWifiStatus> unknown wifi status");
    }

    AmbaKAL_BytePoolFree(pParamRaw);

    NETCTRL_DBG("[AppLib - NetControl] <GetWifiStatus> WifiIsOn = %d",WifiIsOn);
    return WifiIsOn;
#endif
}

static int NetCtrlSend(AMP_NETCTRL_HDLR_INFO_s *HdlrInfo, UINT32 ClientId, char *String, int StringLen)
{
    AMP_NETCTRL_SEND_INFO_s SendInfo = {0};
    int ReturnValue = 0;

    NETCTRL_DBG("[AppLib - NetControl] <NetCtrlSend> %s (len: %d)", String, StringLen);

    SendInfo.ClientId = ClientId;
    SendInfo.MemAddr = String;
    SendInfo.Size = StringLen;
    ReturnValue = AmpNetCtrl_Send(HdlrInfo, &SendInfo);
    if (ReturnValue != 0) {
        return -1;
    }

    return 0;
}

static int ReplyError(int MsgId, int CmdInstanceIdx, int ClientId, int ErrorCode)
{
    AMP_JSON_OBJECT_s *spJsonObjReply = NULL;
    AMP_JSON_STRING_s *spJsonStrReply = NULL;

    if (CmdInstanceIdx >= MAX_CMD_INSTANCE) {
        NETCTRL_ERR("[AppLib - NetControl] <ReplyError> invalid CmdInstanceIdx");
        return -1;
    }

    spJsonObjReply = AmpJson_CreateObject();
    if (!spJsonObjReply) {
        NETCTRL_ERR("[AppLib - NetControl] <ReplyError> spJsonObj is NULL");
        return -1;
    }

    AmpJson_AddObject(spJsonObjReply, "rval", AmpJson_CreateIntegerObject(ErrorCode));
    AmpJson_AddObject(spJsonObjReply, "msg_id", AmpJson_CreateIntegerObject(MsgId));
    spJsonStrReply = AmpJson_JsonObjectToJsonString(spJsonObjReply);
    if (!spJsonStrReply) {
        NETCTRL_ERR("[AppLib - NetControl] <ReplyError> spReplyJsonStr is NULL");
        AmpJson_FreeJsonObject(spJsonObjReply);
        return -1;
    }

    NetCtrlSend(&(gCmdInstance[CmdInstanceIdx]), ClientId, spJsonStrReply->JsonString, strlen(spJsonStrReply->JsonString));
    AmpJson_FreeJsonString(spJsonStrReply);
    AmpJson_FreeJsonObject(spJsonObjReply);

    return 0;
}

/* convert file path format from linux style to DCF style */
static int ConvertFilePathFormat(char *pFilename, char *pRetFilename, int RetFilenameBufSize)
{
    char *pLnxMountPoint = "/tmp/SD0";
    char *pStrFound = NULL;
    int Len = 0;
    char *pTemp = NULL;

    if ((!pFilename) || (!pRetFilename) || (RetFilenameBufSize == 0)) {
        return-1;
    }

    pStrFound = strstr(pFilename, pLnxMountPoint);
    if (pStrFound) {
        snprintf(pRetFilename, RetFilenameBufSize, "C:%s", pStrFound+strlen(pLnxMountPoint));
    } else {
        snprintf(pRetFilename, RetFilenameBufSize, "%s", pFilename);
    }

    Len = strlen(pRetFilename);
    pTemp = pRetFilename;
    while (Len--) {
        if(*pTemp == '/'){
            *pTemp = '\\';
        }

        pTemp++;
    }

    AmbaPrint("<ConvertFilePathPrefix> pFilename = %s, pRetFilename = %s", pFilename, pRetFilename);
    return 0;
}

static int ConvertToFullPath(char *Filename, char *FullnameBuf, UINT32 FullnameBufSize)
{
    AMP_NETCTRL_LNXCMD_RESULT_s LnxResult = {0};
    int CmdRetBufSize = 64;
    char *pCmdRetBuf = NULL;
    char *pCmdRetBufRaw = NULL;
    int ReturnValue = 0;

    if ((!Filename) || (!FullnameBuf) || (FullnameBufSize == 0)) {
        NETCTRL_ERR("[AppLib - NetControl] <ConvertToFullPath> invalid param");
        return -1;
    }

     /* generate full path filename */
    if (Filename[0] != '/') {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&pCmdRetBuf, (void**)&pCmdRetBufRaw, CmdRetBufSize, 32);
        if (ReturnValue < 0) {
            NETCTRL_ERR("[AppLib - NetControl] <CancelFileXferCheck> Allocate buffer fail");
            return -1;
        }

        ReturnValue = AmpNetCtrl_LnxPwd(pCmdRetBuf, CmdRetBufSize, &LnxResult);
        if ((ReturnValue != 0) || (LnxResult.Rval != 0)) {
            NETCTRL_ERR("[AppLib - NetControl] <CancelFileXferCheck> Allocate buffer fail");
            return -1;
        }

        snprintf(FullnameBuf, FullnameBufSize, "%s/%s", pCmdRetBuf, Filename);

        AmbaKAL_BytePoolFree(pCmdRetBufRaw);
    } else {
        snprintf(FullnameBuf, FullnameBufSize, "%s", Filename);
    }

    return 0;
}

/*
* pfilename should be full path
* return 1: file exist
* return 0: file does not exist
*/
static int CheckFileExist(char *pFilename)
{
    AMP_CFS_STAT Status = {0};
    int ReturnValue = 0;

    if (!pFilename) {
        return 0;
    }

    ReturnValue = AmpCFS_Stat(pFilename, &Status);
    if (ReturnValue != AMP_OK) {
        return 0;
    }

    AmbaPrint("file: %s, Attr: 0x%x", pFilename, Status.Attr);

    return 1;
}

static int SetMediaAttribute(APP_NETCTRL_MESSAGE_s *pMsg)
{
    UINT32 ClientId = 0;
    UINT32 CmdInstanceIdx = 0;
    AMP_JSON_OBJECT_s *spJsonObjParam = NULL;
    int Type = 0;
    char Filename[64] = {0};
    char FileFullname[64] = {0};
    AMP_CFS_STAT Status = {0};
    int CurAttribute = 0;
    int ReturnValue = 0;

    #define WRITEABLE   (0x0000)
    #define RDONLY      (0x0001)

    ClientId = pMsg->MessageData[0];
    CmdInstanceIdx = pMsg->MessageData[1];

    // parsing Json string to retrieve arguments
    AmbaKAL_MutexTake(&gNetCtrl.MutexArgBuf, AMBA_KAL_WAIT_FOREVER);
    spJsonObjParam = AmpJson_JsonStringToJsonObject(gCmdArgBuf);
    AmpJson_GetIntegerByKey(spJsonObjParam, "type", &Type);
    AmpJson_GetStringByKey(spJsonObjParam, "param", Filename, sizeof(Filename));
    AmpJson_FreeJsonObject(spJsonObjParam);
    AmbaKAL_MutexGive(&gNetCtrl.MutexArgBuf);


    ReturnValue = ConvertToFullPath(Filename, FileFullname, sizeof(FileFullname));
    if (ReturnValue != 0) {
        ReplyError(AMBA_SET_MEDIA_ATTRIBUTE, CmdInstanceIdx, ClientId, ERROR_NETCTRL_UNKNOWN_ERROR);
        return -1;
    }
    AmbaPrint("<%s> L%d FileFullname = %s",__FUNCTION__,__LINE__,FileFullname);

    ConvertFilePathFormat(FileFullname, Filename, sizeof(Filename));

    /* get current attribute */
    ReturnValue = AmpCFS_Stat(Filename, &Status);
    if (ReturnValue != AMP_OK) {
        /* file does not exist */
        NETCTRL_ERR("[AppLib - NetControl] <SetMediaAttribute> File doesn't exist (%s)",Filename);
        ReplyError(AMBA_SET_MEDIA_ATTRIBUTE, CmdInstanceIdx, ClientId, ERROR_NETCTRL_INVALID_PATH);
        return -1;
    }

    CurAttribute = Status.Attr;
    switch (Type) {
    case WRITEABLE:
        CurAttribute &= (~ATTR_RDONLY);
        break;
    case RDONLY:
        CurAttribute |= (ATTR_RDONLY);
        break;
    default:
        NETCTRL_ERR("[AppLib - NetControl] <SetMediaAttribute> unknown type (%d)", Type);
        ReplyError(AMBA_SET_MEDIA_ATTRIBUTE, CmdInstanceIdx, ClientId, ERROR_NETCTRL_INVALID_TYPE);
        return -1;
    }

    /* set file attribute */
    ReturnValue = AmpCFS_Chmod(Filename, CurAttribute);
    if (ReturnValue != AMP_OK) {
        NETCTRL_ERR("[AppLib - NetControl] <SetMediaAttribute> AmpCFS_Chmod() fail ");
        ReplyError(AMBA_SET_MEDIA_ATTRIBUTE, CmdInstanceIdx, ClientId, ERROR_NETCTRL_UNKNOWN_ERROR);
        return -1;
    }

    ReplyError(AMBA_SET_MEDIA_ATTRIBUTE, CmdInstanceIdx, ClientId, 0);

    return 0;
}

static int DeleteFile(APP_NETCTRL_MESSAGE_s *pMsg)
{
    UINT32 ClientId = 0;
    UINT32 CmdInstanceIdx = 0;
    AMP_JSON_OBJECT_s *spJsonObjParam = NULL;
    char Filename[64] = {0};
    char FileFullname[64] = {0};
    int ReturnValue = 0;

    ClientId = pMsg->MessageData[0];
    CmdInstanceIdx = pMsg->MessageData[1];

    // parsing Json string to retrieve arguments
    AmbaKAL_MutexTake(&gNetCtrl.MutexArgBuf, AMBA_KAL_WAIT_FOREVER);
    spJsonObjParam = AmpJson_JsonStringToJsonObject(gCmdArgBuf);
    AmpJson_GetStringByKey(spJsonObjParam, "param", Filename, sizeof(Filename));
    AmpJson_FreeJsonObject(spJsonObjParam);
    AmbaKAL_MutexGive(&gNetCtrl.MutexArgBuf);

    ReturnValue = ConvertToFullPath(Filename, FileFullname, sizeof(FileFullname));
    if (ReturnValue != 0) {
        ReplyError(AMBA_DEL_FILE, CmdInstanceIdx, ClientId, ERROR_NETCTRL_UNKNOWN_ERROR);
        return -1;
    }
    AmbaPrint("<%s> L%d FileFullname = %s",__FUNCTION__,__LINE__,FileFullname);

    ConvertFilePathFormat(FileFullname, Filename, sizeof(Filename));

    ReturnValue = CheckFileExist(Filename);
    if (ReturnValue == 0) {
        /* file does not exist */
        NETCTRL_ERR("[AppLib - NetControl] <DeleteFile> File doesn't exist (%s)",Filename);
        ReplyError(AMBA_DEL_FILE, CmdInstanceIdx, ClientId, ERROR_NETCTRL_INVALID_PATH);
        return -1;
    }

    ReturnValue = AmpCFS_remove(Filename);
    if (ReturnValue != AMP_OK) {
        NETCTRL_ERR("[AppLib - NetControl] <DeleteFile> AmpCFS_remove() fail (ReturnValue = %d)", ReturnValue);
        ReplyError(AMBA_DEL_FILE, CmdInstanceIdx, ClientId, ERROR_NETCTRL_UNKNOWN_ERROR);
        return -1;
    }

    ReplyError(AMBA_DEL_FILE, CmdInstanceIdx, ClientId, 0);

    return 0;
}

static int LinuxCmd_CD(APP_NETCTRL_MESSAGE_s *pMsg)
{
    UINT32 ClientId = 0;
    UINT32 CmdInstanceIdx = 0;
    AMP_JSON_OBJECT_s *spJsonObjParam = NULL;
    AMP_JSON_OBJECT_s *spJsonObjReply = NULL;
    AMP_JSON_STRING_s *spJsonStrReply = NULL;
    char *pDirPath = NULL;
    char *pDirPathRaw = NULL;
    int DirPathMaxLen = 256;
    char *pRetBuf = NULL;
    char *pRetBufRaw = NULL;
    int RetBuMaxLen = 256;
    AMP_NETCTRL_LNXCMD_RESULT_s LnxResult = {0};
    int ErrorCode = 0;
    int ReturnValue = 0;

    ClientId = pMsg->MessageData[0];
    CmdInstanceIdx = pMsg->MessageData[1];

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&pDirPath, (void**)&pDirPathRaw, DirPathMaxLen, 32);
    if (ReturnValue < 0) {
        ReplyError(AMBA_CD, CmdInstanceIdx, ClientId, ERROR_NETCTRL_UNKNOWN_ERROR);
        NETCTRL_ERR("[AppLib - NetControl] <LinuxCmd_CD> Allocate buffer for dir path fail");
        return -1;
    }

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&pRetBuf, (void**)&pRetBufRaw, RetBuMaxLen, 32);
    if (ReturnValue < 0) {
        ReplyError(AMBA_CD, CmdInstanceIdx, ClientId, ERROR_NETCTRL_UNKNOWN_ERROR);
        NETCTRL_ERR("[AppLib - NetControl] <LinuxCmd_CD> Allocate buffer for ret buf fail");
        AmbaKAL_BytePoolFree(pDirPathRaw);
        return -1;
    }

    // parsing Json string to retrieve arguments
    AmbaKAL_MutexTake(&gNetCtrl.MutexArgBuf, AMBA_KAL_WAIT_FOREVER);
    spJsonObjParam = AmpJson_JsonStringToJsonObject(gCmdArgBuf);
    AmpJson_GetStringByKey(spJsonObjParam, "param", pDirPath, DirPathMaxLen);
    AmpJson_FreeJsonObject(spJsonObjParam);
    AmbaKAL_MutexGive(&gNetCtrl.MutexArgBuf);

    NETCTRL_DBG("[AppLib - NetControl] <LinuxCmd_CD> pDirPath = %s", pDirPath);

    spJsonObjReply = AmpJson_CreateObject();

    ReturnValue = AmpNetCtrl_LnxCd(pDirPath, pRetBuf, RetBuMaxLen, &LnxResult);
    NETCTRL_DBG("[AppLib - NetControl] <LinuxCmd_CD> ReturnValue = %d, LnxResult.Rval = %d", ReturnValue, LnxResult.Rval);
    if (ReturnValue == 0) {
        if (LnxResult.Rval == 0) {
            ErrorCode = (sizeof(spJsonObjReply->ObjValue.String) >= LnxResult.ResultSize) ? 0: ERROR_NETCTRL_UNKNOWN_ERROR;
        } else {
            ErrorCode = MapErrorCode(LnxResult.Rval);
        }
    } else {
        /* IPC fail. */
        ErrorCode = ERROR_NETCTRL_UNKNOWN_ERROR;
    }

    AmpJson_AddObject(spJsonObjReply, "rval", AmpJson_CreateIntegerObject(ErrorCode));
    AmpJson_AddObject(spJsonObjReply, "msg_id", AmpJson_CreateIntegerObject(AMBA_CD));

    if (ErrorCode == 0) {
        AmpJson_AddObject(spJsonObjReply, "pwd", AmpJson_CreateStringObject(pRetBuf));
    }

    spJsonStrReply = AmpJson_JsonObjectToJsonString(spJsonObjReply);
    NetCtrlSend(&(gCmdInstance[CmdInstanceIdx]), ClientId, spJsonStrReply->JsonString, strlen(spJsonStrReply->JsonString));

    AmpJson_FreeJsonString(spJsonStrReply);
    AmpJson_FreeJsonObject(spJsonObjReply);
    AmbaKAL_BytePoolFree(pRetBufRaw);
    AmbaKAL_BytePoolFree(pDirPathRaw);

    return 0;
}

static int LinuxCmd_LS(APP_NETCTRL_MESSAGE_s *pMsg)
{
    UINT32 ClientId = 0;
    UINT32 CmdInstanceIdx = 0;
    AMP_JSON_OBJECT_s *spJsonObjParam = NULL;
    char *pParam = NULL;
    char *pParamRaw = NULL;
    int ParamMaxLen = 256;
    char *pRetBuf = NULL;
    char *pRetBufRaw = NULL;
    int RetBuMaxLen = 64*1024; // temporarily used this size
    int ShiftAddr = 0;
    int HasOption = 0;
    AMP_NETCTRL_LNXCMD_RESULT_s LnxResult = {0};
    int ErrorCode = 0;
    int ReturnValue = 0;

    ClientId = pMsg->MessageData[0];
    CmdInstanceIdx = pMsg->MessageData[1];

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&pParam, (void**)&pParamRaw, ParamMaxLen, 32);
    if (ReturnValue < 0) {
        ReplyError(AMBA_LS, CmdInstanceIdx, ClientId, ERROR_NETCTRL_UNKNOWN_ERROR);
        NETCTRL_ERR("[AppLib - NetControl] <LinuxCmd_LS> Allocate buffer for param fail");
        return -1;
    }

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&pRetBuf, (void**)&pRetBufRaw, RetBuMaxLen, 32);
    if (ReturnValue < 0) {
        ReplyError(AMBA_LS, CmdInstanceIdx, ClientId, ERROR_NETCTRL_UNKNOWN_ERROR);
        NETCTRL_ERR("[AppLib - NetControl] <LinuxCmd_LS> Allocate buffer for ret buf fail");
        AmbaKAL_BytePoolFree(pParamRaw);
        return -1;
    }

    // parsing Json string to retrieve arguments
    AmbaKAL_MutexTake(&gNetCtrl.MutexArgBuf, AMBA_KAL_WAIT_FOREVER);
    spJsonObjParam = AmpJson_JsonStringToJsonObject(gCmdArgBuf);
    HasOption = AmpJson_GetStringByKey(spJsonObjParam, "param", pParam, ParamMaxLen);
    /* For LS command, param is an optional field. If there is no param field in the request,
         the first argument of AmpNetCtrl_LnxLs() should be NULL string. */
    if (HasOption != OK) {
        *pParam = '\0';
    }

    AmpJson_FreeJsonObject(spJsonObjParam);
    AmbaKAL_MutexGive(&gNetCtrl.MutexArgBuf);

    //NETCTRL_DBG("[AppLib - NetControl] <LinuxCmd_LS> pParam = -%s-", pParam);
    //NETCTRL_DBG("[AppLib - NetControl] <LinuxCmd_LS> pRetBuf: 0x%08x",(UINT32)pRetBuf);
    snprintf(pRetBuf, RetBuMaxLen, "{\"rval\":0,\"msg_id\":%d,\"listing\":", AMBA_LS);
    ShiftAddr = strlen(pRetBuf);

    ReturnValue = AmpNetCtrl_LnxLs(pParam, pRetBuf+ShiftAddr, RetBuMaxLen-ShiftAddr-2, &LnxResult); // -2: for "}" and '\0'
    NETCTRL_DBG("[AppLib - NetControl] <LinuxCmd_LS> ReturnValue = %d, LnxResult.Rval = %d", ReturnValue, LnxResult.Rval);
    if (ReturnValue == 0) {
        if (LnxResult.Rval == 0) {
            ErrorCode = 0;
        } else {
            ErrorCode = MapErrorCode(LnxResult.Rval);
        }
    } else {
        /* IPC fail. */
        ErrorCode = ERROR_NETCTRL_UNKNOWN_ERROR;
    }

    if (ErrorCode == 0) {
        strcat(pRetBuf, "}");
        NetCtrlSend(&(gCmdInstance[CmdInstanceIdx]), ClientId, pRetBuf, strlen(pRetBuf));
    } else {
        snprintf(pRetBuf, RetBuMaxLen, "{\"rval\":%d,\"msg_id\":%d}",ErrorCode, AMBA_LS);
        NetCtrlSend(&(gCmdInstance[CmdInstanceIdx]), ClientId, pRetBuf, strlen(pRetBuf));
    }

    AmbaKAL_BytePoolFree(pRetBufRaw);
    AmbaKAL_BytePoolFree(pParamRaw);

    return 0;
}

static int LinuxCmd_PWD(APP_NETCTRL_MESSAGE_s *pMsg)
{
    UINT32 ClientId = 0;
    UINT32 CmdInstanceIdx = 0;
    int CmdRetBufSize = 1024;
    char *pCmdRetBuf = NULL;
    char *pCmdRetBufRaw = NULL;
    AMP_JSON_OBJECT_s *spJsonObjReply = NULL;
    AMP_JSON_STRING_s *spJsonStrReply = NULL;
    AMP_NETCTRL_LNXCMD_RESULT_s LnxResult = {0};
    int ErrorCode = 0;
    int ReturnValue = 0;

    ClientId = pMsg->MessageData[0];
    CmdInstanceIdx = pMsg->MessageData[1];

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&pCmdRetBuf, (void**)&pCmdRetBufRaw, CmdRetBufSize, 32);
    if (ReturnValue < 0) {
        ReplyError(AMBA_PWD, CmdInstanceIdx, ClientId, ERROR_NETCTRL_UNKNOWN_ERROR);
        NETCTRL_ERR("[AppLib - NetControl] <LinuxCmd_PWD> Allocate buffer for command return buffer fail");
        return -1;
    }

    spJsonObjReply = AmpJson_CreateObject();

    ReturnValue = AmpNetCtrl_LnxPwd(pCmdRetBuf, CmdRetBufSize, &LnxResult);
     if (ReturnValue == 0) {
        if (LnxResult.Rval == 0) {
            ErrorCode = (sizeof(spJsonObjReply->ObjValue.String) >= LnxResult.ResultSize) ? 0: ERROR_NETCTRL_UNKNOWN_ERROR;
        } else {
            ErrorCode = MapErrorCode(LnxResult.Rval);
        }
    } else {
        /* IPC fail. */
        ErrorCode = ERROR_NETCTRL_UNKNOWN_ERROR;
    }

    AmpJson_AddObject(spJsonObjReply, "rval", AmpJson_CreateIntegerObject(ErrorCode));
    AmpJson_AddObject(spJsonObjReply, "msg_id", AmpJson_CreateIntegerObject(AMBA_PWD));

    if (ErrorCode == 0) {
        AmpJson_AddObject(spJsonObjReply, "pwd", AmpJson_CreateStringObject(pCmdRetBuf));
    }

    spJsonStrReply = AmpJson_JsonObjectToJsonString(spJsonObjReply);
    NetCtrlSend(&(gCmdInstance[CmdInstanceIdx]), ClientId, spJsonStrReply->JsonString, strlen((char*)spJsonStrReply->JsonString));

    AmpJson_FreeJsonString(spJsonStrReply);
    AmpJson_FreeJsonObject(spJsonObjReply);
    AmbaKAL_BytePoolFree(pCmdRetBufRaw);

    return 0;
}

static int LinuxCmd_GetWifiSetting(APP_NETCTRL_MESSAGE_s *pMsg)
{
    UINT32 ClientId = 0;
    UINT32 CmdInstanceIdx = 0;
    int CmdRetBufSize = 1024;
    char *pCmdRetBuf = NULL;
    char *pCmdRetBufRaw = NULL;
    int ShiftAddr = 0;
    AMP_NETCTRL_LNXCMD_RESULT_s LnxResult = {0};
    int ErrorCode = 0;
    int ReturnValue = 0;

    ClientId = pMsg->MessageData[0];
    CmdInstanceIdx = pMsg->MessageData[1];

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&pCmdRetBuf, (void**)&pCmdRetBufRaw, CmdRetBufSize, 32);
    if (ReturnValue < 0) {
        ReplyError(AMBA_GET_WIFI_SETTING, CmdInstanceIdx, ClientId, ERROR_NETCTRL_UNKNOWN_ERROR);
        NETCTRL_ERR("[AppLib - NetControl] <LinuxCmd_GetWifiSetting> Allocate buffer for command return buffer fail");
        return -1;
    }



    sprintf(pCmdRetBuf, "{\"rval\":0,\"msg_id\":%d,\"param\":", AMBA_GET_WIFI_SETTING);
    ShiftAddr = strlen(pCmdRetBuf);

    ReturnValue = AmpNetCtrl_GetWifiConfig(NULL, pCmdRetBuf+ShiftAddr, CmdRetBufSize-ShiftAddr-2, &LnxResult);
    if (ReturnValue == 0) {
        if (LnxResult.Rval == 0) {
            ErrorCode = 0;
        } else {
            ErrorCode = MapErrorCode(LnxResult.Rval);
        }
    } else {
        /* IPC fail. */
        ErrorCode = ERROR_NETCTRL_UNKNOWN_ERROR;
    }

    if (ErrorCode == 0) {
        strcat(pCmdRetBuf, "}");
        NetCtrlSend(&(gCmdInstance[CmdInstanceIdx]), ClientId, pCmdRetBuf, strlen(pCmdRetBuf));
    } else {
        snprintf(pCmdRetBuf, CmdRetBufSize, "{\"rval\":%d,\"msg_id\":%d",ErrorCode, AMBA_GET_WIFI_SETTING);
        NetCtrlSend(&(gCmdInstance[CmdInstanceIdx]), ClientId, pCmdRetBuf, strlen(pCmdRetBuf));
    }

    AmbaKAL_BytePoolFree(pCmdRetBufRaw);

    return 0;
}

static int LinuxCmd_SetWifiSetting(APP_NETCTRL_MESSAGE_s *pMsg)
{
    UINT32 ClientId = 0;
    UINT32 CmdInstanceIdx = 0;
    char *pParam = NULL;
    char *pParamRaw = NULL;
    int ParamMaxLen = 1024;
    AMP_JSON_OBJECT_s *spJsonObjParam = NULL;
    AMP_JSON_OBJECT_s *spJsonObjReply = NULL;
    AMP_JSON_STRING_s *spJsonStrReply = NULL;
    AMP_NETCTRL_LNXCMD_RESULT_s LnxResult = {0};
    int ErrorCode = 0;
    int ReturnValue = 0;

    ClientId = pMsg->MessageData[0];
    CmdInstanceIdx = pMsg->MessageData[1];

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&pParam, (void**)&pParamRaw, ParamMaxLen, 32);
    if (ReturnValue < 0) {
        ReplyError(AMBA_SET_WIFI_SETTING, CmdInstanceIdx, ClientId, ERROR_NETCTRL_UNKNOWN_ERROR);
        NETCTRL_ERR("[AppLib - NetControl] <LinuxCmd_SetWifiSetting> Allocate buffer for param fail");
        AmbaKAL_BytePoolFree(pParamRaw);
        return -1;
    }

   // parsing Json string to retrieve arguments
    AmbaKAL_MutexTake(&gNetCtrl.MutexArgBuf, AMBA_KAL_WAIT_FOREVER);
    spJsonObjParam = AmpJson_JsonStringToJsonObject(gCmdArgBuf);
    AmpJson_GetStringByKey(spJsonObjParam, "param", pParam, ParamMaxLen);
    AmpJson_FreeJsonObject(spJsonObjParam);
    AmbaKAL_MutexGive(&gNetCtrl.MutexArgBuf);

    spJsonObjReply = AmpJson_CreateObject();
    if (!spJsonObjReply) {
        ReplyError(AMBA_SET_WIFI_SETTING, CmdInstanceIdx, ClientId, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        NETCTRL_ERR("[AppLib - NetControl] <LinuxCmd_SetWifiSetting> Allocate buffer for param fail");
        return -1;
    }

    ReturnValue = AmpNetCtrl_SetWifiConfig(pParam, NULL, &LnxResult);
    if (ReturnValue == 0) {
        if (LnxResult.Rval == 0) {
            ErrorCode = 0;
        } else {
            ErrorCode = MapErrorCode(LnxResult.Rval);
        }
    } else {
        /* IPC fail. */
        ErrorCode = ERROR_NETCTRL_UNKNOWN_ERROR;
    }

    AmpJson_AddObject(spJsonObjReply, "rval", AmpJson_CreateIntegerObject(ErrorCode));
    AmpJson_AddObject(spJsonObjReply, "msg_id", AmpJson_CreateIntegerObject(AMBA_SET_WIFI_SETTING));
    spJsonStrReply = AmpJson_JsonObjectToJsonString(spJsonObjReply);
    NetCtrlSend(&(gCmdInstance[CmdInstanceIdx]), ClientId, spJsonStrReply->JsonString, strlen((char*)spJsonStrReply->JsonString));

    AmpJson_FreeJsonString(spJsonStrReply);
    AmpJson_FreeJsonObject(spJsonObjReply);
    AmbaKAL_BytePoolFree(pParamRaw);

    return 0;
}

/* return 1: wifi on, 0: wifi off */

static int LinuxCmd_WifiControl(APP_NETCTRL_MESSAGE_s *pMsg)
{
    UINT32 ClientId = 0;
    UINT32 CmdInstanceIdx = 0;
    int ErrorCode = 0;
    AMP_JSON_OBJECT_s *spJsonObjReply = NULL;
    AMP_JSON_STRING_s *spJsonStrReply = NULL;
    int WifiConnStatus = -1;
    int ReturnValue = 0;

    ClientId = pMsg->MessageData[0];
    CmdInstanceIdx = pMsg->MessageData[1];

    switch (pMsg->MessageID) {
    case AMBA_WIFI_START:
        WifiConnStatus = GetWifiStatus();
        if (WifiConnStatus == 0){
            ReturnValue = AmpNetCtrl_WifiStart(NULL);
        } else if (WifiConnStatus == 1) {
            /* wifi is already on. Do nothing. */
            ReturnValue = 0;
        } else {
            /* error occur */
            ReturnValue = -1;
        }
        break;
    case AMBA_WIFI_STOP:
        ReturnValue = AmpNetCtrl_WifiStop(NULL);
        break;
    case AMBA_WIFI_RESTART:
        ReturnValue = AmpNetCtrl_WifiRestart(NULL);
        break;
    default:
        ReturnValue = -1;
        break;
    }

    ErrorCode = (ReturnValue == 0) ? 0 : ERROR_NETCTRL_UNKNOWN_ERROR;

    spJsonObjReply = AmpJson_CreateObject();
    AmpJson_AddObject(spJsonObjReply, "rval", AmpJson_CreateIntegerObject(ErrorCode));
    AmpJson_AddObject(spJsonObjReply, "msg_id", AmpJson_CreateIntegerObject(pMsg->MessageID));
    spJsonStrReply = AmpJson_JsonObjectToJsonString(spJsonObjReply);
    NetCtrlSend(&(gCmdInstance[CmdInstanceIdx]), ClientId, spJsonStrReply->JsonString, strlen((char*)spJsonStrReply->JsonString));

    AmpJson_FreeJsonString(spJsonStrReply);
    AmpJson_FreeJsonObject(spJsonObjReply);

    return 0;
}

static int LinuxCmd_WifiStop(APP_NETCTRL_MESSAGE_s *pMsg)
{
    UINT32 ClientId = 0;
    UINT32 CmdInstanceIdx = 0;
    AMP_JSON_OBJECT_s *spJsonObjReply = NULL;
    AMP_JSON_STRING_s *spJsonStrReply = NULL;
    int ReturnValue = 0;

    ClientId = pMsg->MessageData[0];
    CmdInstanceIdx = pMsg->MessageData[1];

    spJsonObjReply = AmpJson_CreateObject();
    AmpJson_AddObject(spJsonObjReply, "rval", AmpJson_CreateIntegerObject(0));
    AmpJson_AddObject(spJsonObjReply, "msg_id", AmpJson_CreateIntegerObject(pMsg->MessageID));
    spJsonStrReply = AmpJson_JsonObjectToJsonString(spJsonObjReply);
    NetCtrlSend(&(gCmdInstance[CmdInstanceIdx]), ClientId, spJsonStrReply->JsonString, strlen((char*)spJsonStrReply->JsonString));

    ReturnValue = AmpNetCtrl_WifiStop(NULL);

    AmpJson_FreeJsonString(spJsonStrReply);
    AmpJson_FreeJsonObject(spJsonObjReply);

    return 0;
}


static int LinuxCmd_GetWifiStatus(APP_NETCTRL_MESSAGE_s *pMsg)
{
    UINT32 ClientId = 0;
    UINT32 CmdInstanceIdx = 0;
    char *pParam = NULL;
    char *pParamRaw = NULL;
    int ParamMaxLen = 1024;
    int ShiftAddr = 0;
    AMP_NETCTRL_LNXCMD_RESULT_s LnxResult = {0};
    int ErrorCode =0 ;
    int ReturnValue = 0;

    ClientId = pMsg->MessageData[0];
    CmdInstanceIdx = pMsg->MessageData[1];

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&pParam, (void**)&pParamRaw, ParamMaxLen, 32);
    if (ReturnValue < 0) {
        ReplyError(AMBA_GET_WIFI_STATUS, CmdInstanceIdx, ClientId, ERROR_NETCTRL_UNKNOWN_ERROR);
        NETCTRL_ERR("[AppLib - NetControl] <LinuxCmd_GetWifiStatus> Allocate buffer for param fail");
        AmbaKAL_BytePoolFree(pParamRaw);
        return -1;
    }

    memset(pParam, 0, ParamMaxLen);
    snprintf(pParam, ParamMaxLen, "{\"rval\":0,\"msg_id\":%d,\"param\":", AMBA_GET_WIFI_STATUS);
    ShiftAddr = strlen(pParam);

    AmpNetCtrl_GetWifiStatus(NULL, pParam+ShiftAddr, ParamMaxLen-ShiftAddr-2, &LnxResult);
     if (ReturnValue == 0) {
        if (LnxResult.Rval == 0) {
            ErrorCode = 0;
        } else {
            ErrorCode = MapErrorCode(LnxResult.Rval);
        }
    } else {
        /* IPC fail. */
        ErrorCode = ERROR_NETCTRL_UNKNOWN_ERROR;
    }

    if (ErrorCode == 0) {
        strcat(pParam, "}");
        NetCtrlSend(&(gCmdInstance[CmdInstanceIdx]), ClientId, pParam, strlen(pParam));
    } else {
        snprintf(pParam, ParamMaxLen, "{\"rval\":%d,\"msg_id\":%d",ErrorCode, AMBA_GET_WIFI_STATUS);
        NetCtrlSend(&(gCmdInstance[CmdInstanceIdx]), ClientId, pParam, strlen(pParam));
    }

    AmbaKAL_BytePoolFree(pParamRaw);

    return 0;
}

static int LinuxCmd(APP_NETCTRL_MESSAGE_s *pMsg)
{
    int ReturnValue = OK;
    int TokenNumber = 0;
    UINT32 ClientId = 0;

    GetActiveSession(&TokenNumber, &ClientId);

    switch (pMsg->MessageID) {
    case AMBA_CD:
        LinuxCmd_CD(pMsg);
        break;
    case AMBA_LS:
        LinuxCmd_LS(pMsg);
        break;
    case AMBA_PWD:
        LinuxCmd_PWD(pMsg);
        break;
    case AMBA_GET_WIFI_SETTING:
        LinuxCmd_GetWifiSetting(pMsg);
        break;
    case AMBA_WIFI_RESTART:
    case AMBA_WIFI_START:
        LinuxCmd_WifiControl(pMsg);
        break;
    case AMBA_WIFI_STOP:
        LinuxCmd_WifiStop(pMsg);
        break;
    case AMBA_SET_WIFI_SETTING:
        LinuxCmd_SetWifiSetting(pMsg);
        break;
    case AMBA_GET_WIFI_STATUS:
        LinuxCmd_GetWifiStatus(pMsg);
        break;
    default:
        ReturnValue = NG;
        NETCTRL_ERR("[AppLib - NetControl] <LinuxCmd> OPERATION_UNSUPPORTED!");
        break;
    }

    //-- The following code are temporarily used. Should be removed after complete this function
    if (ReturnValue == NG) {
        ReplyError(pMsg->MessageID, pMsg->MessageData[1], ClientId, ERROR_NETCTRL_OPERATION_UNSUPPORTED);
    }

    return ReturnValue;
}

static int ParseClientInfo(APP_NETCTRL_MESSAGE_s *pMsg)
{
    int ClientId = 0;
    UINT32 CmdInstanceIdx = 0;
    AMP_JSON_OBJECT_s *spJsonObjParam = NULL;
    AMP_JSON_OBJECT_s *spJsonObjReply = NULL;
    AMP_JSON_STRING_s *spJsonStrReply = NULL;
    char Param[32] = {0};
    int ErrorCode = 0;

    memset(gActiveTransportType, 0, sizeof(gActiveTransportType));
    memset(gActiveClientInfo, 0, sizeof(gActiveClientInfo));

    ClientId = pMsg->MessageData[0];
    CmdInstanceIdx = pMsg->MessageData[1];

    // parsing Json string to retrieve arguments
    AmbaKAL_MutexTake(&gNetCtrl.MutexArgBuf, AMBA_KAL_WAIT_FOREVER);
    spJsonObjParam = AmpJson_JsonStringToJsonObject(gCmdArgBuf);
    AmpJson_GetStringByKey(spJsonObjParam, "type", gActiveTransportType, sizeof(gActiveTransportType));
    AmpJson_GetStringByKey(spJsonObjParam, "param", Param, sizeof(Param));
    AmpJson_FreeJsonObject(spJsonObjParam);
    AmbaKAL_MutexGive(&gNetCtrl.MutexArgBuf);

    if (strcmp(gActiveTransportType, "TCP") == 0) {
        sscanf(Param, "%hhu.%hhu.%hhu.%hhu", &gActiveClientInfo[0], &gActiveClientInfo[1],
                                             &gActiveClientInfo[2], &gActiveClientInfo[3]);
        AmbaPrint("%hhu.%hhu.%hhu.%hhu", gActiveClientInfo[0], gActiveClientInfo[1], gActiveClientInfo[2], gActiveClientInfo[3]);
    } else if (strcmp(gActiveTransportType, "RFCOMM") == 0) {
        sscanf(Param, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &gActiveClientInfo[0], &gActiveClientInfo[1], &gActiveClientInfo[2],
                                                        &gActiveClientInfo[3], &gActiveClientInfo[4], &gActiveClientInfo[5]);
        AmbaPrint("%hhx:%hhx:%hhx:%hhx:%hhx:hhx", gActiveClientInfo[0], gActiveClientInfo[1], gActiveClientInfo[2],
                                                  gActiveClientInfo[3], gActiveClientInfo[4], gActiveClientInfo[5]);
    } else {
        NETCTRL_ERR("[AppLib - NetControl] <ParseClientInfo> No support this transport protocol type");
        ErrorCode = ERROR_NETCTRL_UNKNOWN_ERROR;
    }

    spJsonObjReply = AmpJson_CreateObject();
    if (!spJsonObjReply) {
        NETCTRL_ERR("[AppLib - NetControl] <ParseClientInfo> AmpJson_CreateObject() fail!");
        ReplyError(AMBA_SET_CLNT_INFO, CmdInstanceIdx, ClientId, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return-1;
    }

    AmpJson_AddObject(spJsonObjReply, "rval", AmpJson_CreateIntegerObject(ErrorCode));
    AmpJson_AddObject(spJsonObjReply, "msg_id", AmpJson_CreateIntegerObject(AMBA_SET_CLNT_INFO));

    spJsonStrReply = AmpJson_JsonObjectToJsonString(spJsonObjReply);
    if (!spJsonStrReply) {
        NETCTRL_ERR("[AppLib - NetControl] <ParseClientInfo> AmpJson_JsonObjectToJsonString() fail!");
        AmpJson_FreeJsonObject(spJsonObjReply);
        return-1;
    }

    NetCtrlSend(&(gCmdInstance[CmdInstanceIdx]), ClientId, spJsonStrReply->JsonString, strlen((char*)spJsonStrReply->JsonString));

    AmpJson_FreeJsonString(spJsonStrReply);
    AmpJson_FreeJsonObject(spJsonObjReply);

    return 0;
}

static int ResetClientInfo(void)
{
    memset(gActiveClientInfo, 0, sizeof(gActiveClientInfo));
    memset(gActiveTransportType, 0, sizeof(gActiveTransportType));
    return 0;
}

static int GetFile(APP_NETCTRL_MESSAGE_s *pMsg)
{
    UINT32 ClientId = 0;
    UINT32 CmdInstanceIdx = 0;
    AMP_JSON_OBJECT_s *spJsonObjParam = NULL;
    AMP_JSON_OBJECT_s *spJsonObjReply = NULL;
    AMP_JSON_STRING_s *spJsonStrReply = NULL;
    char *pFilename = NULL;
    char *pFilenameRaw = NULL;
    int FilenameMaxLen = 128;
    int Offset = 0;
    int FetchSize = 0;
    int Status = 0;
    AMP_NETCTRL_DATASVC_DATA_s DataReq = {0};
    AMP_NETCTRL_DATASVC_RESULT_s DataResult = {0};
    AMP_NETCTRL_DATASVC_DEST_INFO_s DestInfo = {0};
    char Fullpath[128];
    int ErrorCode = 0;
    int ReturnValue = 0;

    ClientId = pMsg->MessageData[0];
    CmdInstanceIdx = pMsg->MessageData[1];

    AmbaKAL_EventFlagClear(&gDataSvcEvent, DATA_SVC_EVENT_GET_DONE);

    memcpy(DestInfo.ClientInfo, gActiveClientInfo, sizeof(DestInfo.ClientInfo));
    strcpy(DestInfo.TransportType, gActiveTransportType);
    ReturnValue = AmpNetCtrl_DataSvc_GetStatus(&(gDataInstance[0]), &DestInfo, &Status);
    if (ReturnValue != 0) {
        NETCTRL_ERR("[AppLib - NetControl] <GetFile> AmpNetCtrl_DataSvc_GetStatus fail (%d)", ReturnValue);
        ReplyError(AMBA_GET_FILE, CmdInstanceIdx, ClientId, MapErrorCode(ReturnValue));
        return -2;
    } else if (Status & (DATA_SERVER_SEND_BUSY | DATA_SERVER_CLOSE_CONNECT)) {
        NETCTRL_ERR("[AppLib - NetControl] <GetFile> data server busy (Status = 0x%x)", Status);
        ReplyError(AMBA_GET_FILE, CmdInstanceIdx, ClientId, ERROR_NETCTRL_SYSTEM_BUSY);
        return -3;
    }

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&pFilename, (void**)&pFilenameRaw, FilenameMaxLen, 32);
    if (ReturnValue < 0) {
        ReplyError(AMBA_GET_FILE, CmdInstanceIdx, ClientId,ERROR_NETCTRL_UNKNOWN_ERROR);
        NETCTRL_ERR("[AppLib - NetControl] <GetFile> Allocate buffer for filename fail");
        return -1;
    }

    // parsing Json string to retrieve arguments
    AmbaKAL_MutexTake(&gNetCtrl.MutexArgBuf, AMBA_KAL_WAIT_FOREVER);
    spJsonObjParam = AmpJson_JsonStringToJsonObject(gCmdArgBuf);
    AmpJson_GetStringByKey(spJsonObjParam, "param", pFilename, FilenameMaxLen);
    AmpJson_GetIntegerByKey(spJsonObjParam, "offset", &Offset);
    AmpJson_GetIntegerByKey(spJsonObjParam, "fetch_size", &FetchSize);
    AmpJson_FreeJsonObject(spJsonObjParam);
    AmbaKAL_MutexGive(&gNetCtrl.MutexArgBuf);

    #if 0
    NETCTRL_DBG("[AppLib - NetControl] <GetFile> Filename = %s", pFilename);
    NETCTRL_DBG("[AppLib - NetControl] <GetFile> Offset = %d", Offset);
    NETCTRL_DBG("[AppLib - NetControl] <GetFile> FetchSize = %d", FetchSize);
    #endif

    ConvertToFullPath(pFilename, Fullpath, sizeof(Fullpath));

    AmbaKAL_MutexTake(&gMutexDataSvcMgr, AMBA_KAL_WAIT_FOREVER);
    snprintf(gDataSvcMgr[FLIE_XFER_GET].Filename, sizeof(gDataSvcMgr[FLIE_XFER_GET].Filename), "%s", Fullpath);
    gDataSvcMgr[FLIE_XFER_GET].Transferring = 1;
    AmbaKAL_MutexGive(&gMutexDataSvcMgr);
    //AmbaPrint("=============== gDataSvcMgr[FLIE_XFER_GET].Filename = -%s-",gDataSvcMgr[FLIE_XFER_GET].Filename);

    DataReq.MsgId = DATASVC_GET_FILE;
    DataReq.Offset = Offset;
    DataReq.FetchFilesize = FetchSize;
    DataReq.BufferAddr = NULL;
    strcpy(DataReq.Filepath, Fullpath);
    memcpy(DataReq.ClientInfo, gActiveClientInfo, sizeof(DataReq.ClientInfo)); // need to add protection
    strcpy(DataReq.TransportType, gActiveTransportType);    // need to add protection
    ReturnValue = AmpNetCtrl_DataSvc_Send(&(gDataInstance[0]), &DataReq, &DataResult);


    spJsonObjReply = AmpJson_CreateObject();
    if (!spJsonObjReply) {
        NETCTRL_ERR("[AppLib - NetControl] <GetFile> AmpJson_CreateObject() fail!");
        ReplyError(AMBA_GET_FILE, CmdInstanceIdx, ClientId, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmbaKAL_BytePoolFree(pFilenameRaw);
        return-1;
    }

    if (DataResult.Rval != 0) {
        NETCTRL_ERR("[AppLib - NetControl] <PutFile> AmpNetCtrl_DataSvc_Send() fail - DataResult.Rval != 0");
        ErrorCode = MapErrorCode(DataResult.Rval);
    } else if (ReturnValue != 0) {
        NETCTRL_ERR("[AppLib - NetControl] <PutFile> AmpNetCtrl_DataSvc_Send() fail - ReturnValue != 0");
        ErrorCode = MapErrorCode(ReturnValue);
    }

    AmpJson_AddObject(spJsonObjReply, "rval", AmpJson_CreateIntegerObject(ErrorCode));
    AmpJson_AddObject(spJsonObjReply, "msg_id", AmpJson_CreateIntegerObject(AMBA_GET_FILE));
    AmpJson_AddObject(spJsonObjReply, "rem_size", AmpJson_CreateIntegerObject(DataResult.RemSize));
    AmpJson_AddObject(spJsonObjReply, "size", AmpJson_CreateIntegerObject(DataResult.TotalFilesize));

    spJsonStrReply = AmpJson_JsonObjectToJsonString(spJsonObjReply);
    if (!spJsonStrReply) {
        NETCTRL_ERR("[AppLib - NetControl] <GetFile> AmpJson_JsonObjectToJsonString() fail!");
        AmpJson_FreeJsonObject(spJsonObjReply);
        AmbaKAL_BytePoolFree(pFilenameRaw);
        return-1;
    }

    NetCtrlSend(&(gCmdInstance[CmdInstanceIdx]), ClientId, spJsonStrReply->JsonString, strlen((char*)spJsonStrReply->JsonString));

    AmpJson_FreeJsonString(spJsonStrReply);
    AmpJson_FreeJsonObject(spJsonObjReply);
    AmbaKAL_BytePoolFree(pFilenameRaw);
    return 0;
}

static int PutFile(APP_NETCTRL_MESSAGE_s *pMsg)
{
    UINT32 ClientId = 0;
    UINT32 CmdInstanceIdx = 0;
    int Status = 0;
    AMP_JSON_OBJECT_s *spJsonObjParam = NULL;
    AMP_JSON_OBJECT_s *spJsonObjReply = NULL;
    AMP_JSON_STRING_s *spJsonStrReply = NULL;
    char *pFilename = NULL;
    char *pFilenameRaw = NULL;
    int FilenameMaxLen = 128;
    char MD5sum[36] = {0};	// md5 checksum: 32 byte
    int Offset = 0;
    int Size = 0;
    AMP_NETCTRL_DATASVC_DATA_s DataReq = {0};
    AMP_NETCTRL_DATASVC_RESULT_s DataResult = {0};
    AMP_NETCTRL_DATASVC_DEST_INFO_s DestInfo = {0};
    char Fullpath[128];
    int ErrorCode = 0;
    int ReturnValue = 0;

    ClientId = pMsg->MessageData[0];
    CmdInstanceIdx = pMsg->MessageData[1];

    AmbaKAL_EventFlagClear(&gDataSvcEvent, DATA_SVC_EVENT_PUT_DONE);

    memcpy(DestInfo.ClientInfo, gActiveClientInfo, sizeof(DestInfo.ClientInfo));
    strcpy(DestInfo.TransportType, gActiveTransportType);
    ReturnValue = AmpNetCtrl_DataSvc_GetStatus(&(gDataInstance[0]), &DestInfo, &Status);
    if (ReturnValue != 0) {
        NETCTRL_ERR("[AppLib - NetControl] <PutFile> AmpNetCtrl_DataSvc_GetStatus fail (%d)", ReturnValue);
        ReplyError(AMBA_PUT_FILE, CmdInstanceIdx, ClientId, MapErrorCode(ReturnValue));
        return -2;
    } else if (Status & (DATA_SERVER_RECV_BUSY | DATA_SERVER_CLOSE_CONNECT)) {
        NETCTRL_ERR("[AppLib - NetControl] <PutFile> data server busy (Status = 0x%x)", Status);
        ReplyError(AMBA_PUT_FILE, CmdInstanceIdx, ClientId, ERROR_NETCTRL_SYSTEM_BUSY);
        return -3;
    }

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&pFilename, (void**)&pFilenameRaw, FilenameMaxLen, 32);
    if (ReturnValue < 0) {
        ReplyError(AMBA_PUT_FILE, CmdInstanceIdx, ClientId, ERROR_NETCTRL_UNKNOWN_ERROR);
        NETCTRL_ERR("[AppLib - NetControl] <PutFile> Allocate buffer for filename fail");
        return -1;
    }

    //-- parsing Json string to retrieve arguments
    AmbaKAL_MutexTake(&gNetCtrl.MutexArgBuf, AMBA_KAL_WAIT_FOREVER);
    spJsonObjParam = AmpJson_JsonStringToJsonObject(gCmdArgBuf);
    AmpJson_GetStringByKey(spJsonObjParam, "param", pFilename, FilenameMaxLen);
    AmpJson_GetIntegerByKey(spJsonObjParam, "size", &Size);
    AmpJson_GetStringByKey(spJsonObjParam, "md5sum", MD5sum, sizeof(MD5sum));
    AmpJson_GetIntegerByKey(spJsonObjParam, "offset", &Offset);
    AmpJson_FreeJsonObject(spJsonObjParam);
    AmbaKAL_MutexGive(&gNetCtrl.MutexArgBuf);

    #if 0
    NETCTRL_DBG("[AppLib - NetControl] <PutFile> Filename = %s", pFilename);
    NETCTRL_DBG("[AppLib - NetControl] <PutFile> Size = %d", Size);
    NETCTRL_DBG("[AppLib - NetControl] <PutFile> MD5sum = %X", MD5sum);
    NETCTRL_DBG("[AppLib - NetControl] <PutFile>Offset = %d", Offset);
    #endif

    if (strcmp(MD5sum,"") == 0) {
        NETCTRL_ERR("[AppLib - NetControl] <PutFile> invalid checksum");
        ReplyError(AMBA_PUT_FILE, CmdInstanceIdx, ClientId, ERROR_NETCTRL_INVALID_PARAM);
        AmbaKAL_BytePoolFree(pFilenameRaw);
        return -1;
    }

    ConvertToFullPath(pFilename, Fullpath, sizeof(Fullpath));

    AmbaKAL_MutexTake(&gMutexDataSvcMgr, AMBA_KAL_WAIT_FOREVER);
    snprintf(gDataSvcMgr[FLIE_XFER_PUT].Filename, sizeof(gDataSvcMgr[FLIE_XFER_GET].Filename), "%s", Fullpath);
    gDataSvcMgr[FLIE_XFER_PUT].Transferring = 1;
    AmbaKAL_MutexGive(&gMutexDataSvcMgr);
    //AmbaPrint("=============== gDataSvcMgr[FLIE_XFER_PUT].Filename = -%s-",gDataSvcMgr[FLIE_XFER_PUT].Filename);

    DataReq.MsgId = DATASVC_PUT_FILE;
    DataReq.Offset = Offset;
    DataReq.FetchFilesize = Size;
    DataReq.BufferAddr = NULL;
    strcpy(DataReq.Filepath, Fullpath);
    memcpy(DataReq.Md5sum, MD5sum, sizeof(DataReq.Md5sum));
    memcpy(DataReq.ClientInfo, gActiveClientInfo, sizeof(DataReq.ClientInfo));  // need to add protection
    strcpy(DataReq.TransportType, gActiveTransportType);    // need to add protection

    ReturnValue = AmpNetCtrl_DataSvc_Send(&(gDataInstance[0]), &DataReq, &DataResult);

    spJsonObjReply = AmpJson_CreateObject();
    if (!spJsonObjReply) {
        NETCTRL_ERR("[AppLib - NetControl] <PutFile> AmpJson_CreateObject() fail");
        ReplyError(AMBA_PUT_FILE, CmdInstanceIdx, ClientId, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmbaKAL_BytePoolFree(pFilenameRaw);
        return -1;
    }

    if (DataResult.Rval != 0) {
        ErrorCode = MapErrorCode(DataResult.Rval);
        NETCTRL_ERR("[AppLib - NetControl] <PutFile> AmpNetCtrl_DataSvc_Send() fail - DataResult.Rval != 0");
    } else if (ReturnValue != 0) {
        NETCTRL_ERR("[AppLib - NetControl] <PutFile> AmpNetCtrl_DataSvc_Send() fail - ReturnValue != 0");
        ErrorCode = MapErrorCode(ReturnValue);
    }

    AmpJson_AddObject(spJsonObjReply, "rval", AmpJson_CreateIntegerObject(ErrorCode));
    AmpJson_AddObject(spJsonObjReply, "msg_id", AmpJson_CreateIntegerObject(AMBA_PUT_FILE));

    spJsonStrReply = AmpJson_JsonObjectToJsonString(spJsonObjReply);
    if (!spJsonStrReply) {
        NETCTRL_ERR("[AppLib - NetControl] <PutFile> AmpJson_JsonObjectToJsonString() fail");
        ReplyError(AMBA_PUT_FILE, CmdInstanceIdx, ClientId, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmpJson_FreeJsonObject(spJsonObjReply);
        AmbaKAL_BytePoolFree(pFilenameRaw);
        return -1;
    }

    NetCtrlSend(&(gCmdInstance[CmdInstanceIdx]), ClientId, spJsonStrReply->JsonString, strlen((char*)spJsonStrReply->JsonString));

    AmpJson_FreeJsonString(spJsonStrReply);
    AmpJson_FreeJsonObject(spJsonObjReply);
    AmbaKAL_BytePoolFree(pFilenameRaw);

    return 0;
}

static int CancelFileXferCheck(char *Filename, UINT32 *CancelRequestType)
{
    AMP_NETCTRL_DATASVC_DEST_INFO_s DestInfo = {0};
    int Status = 0;
    int CheckStatus = 0;
    char Fullname[96];
    int i = 0;
    int ReturnValue = 0;

    if ((!Filename) || (!CancelRequestType)) {
        NETCTRL_ERR("[AppLib - NetControl] <CancelFileXferCheck> invalid param");
        return ERROR_NETCTRL_UNKNOWN_ERROR;
    }

    ReturnValue = ConvertToFullPath(Filename, Fullname, sizeof(Fullname));
    if (ReturnValue != 0) {
        return ERROR_NETCTRL_UNKNOWN_ERROR;
    }

    AmbaKAL_MutexTake(&gMutexDataSvcMgr, AMBA_KAL_WAIT_FOREVER);
    for (i=0;i<FLIE_XFER_CHANNEL_NUM;i++) {
        if ((gDataSvcMgr[i].Transferring == 1) &&
            (strcmp(Fullname, gDataSvcMgr[i].Filename) == 0)) {
            break;
        }
    }
    AmbaKAL_MutexGive(&gMutexDataSvcMgr);

    if (i >= FLIE_XFER_CHANNEL_NUM) {
        NETCTRL_ERR("[AppLib - NetControl] <CancelFileXferCheck> no matched transferring file");
        return ERROR_NETCTRL_INVALID_OPTION_VALUE;
    }

    if (i == FLIE_XFER_GET) {
        *CancelRequestType = DATASVC_GET_FILE;
        CheckStatus = DATA_SERVER_SEND_BUSY;
    } else {
        *CancelRequestType = DATASVC_PUT_FILE;
        CheckStatus = DATA_SERVER_RECV_BUSY;
    }

    memcpy(DestInfo.ClientInfo, gActiveClientInfo, sizeof(DestInfo.ClientInfo));
    strcpy(DestInfo.TransportType, gActiveTransportType);
    ReturnValue = AmpNetCtrl_DataSvc_GetStatus(&(gDataInstance[0]), &DestInfo, &Status);
    if (!((ReturnValue == 0) && (Status & CheckStatus))) {
        // file is not transfering
        return ERROR_NETCTRL_INVALID_OPERATION;
    }

    //-- file is transfering
    return 0;
}


static int CancelFileXfer(APP_NETCTRL_MESSAGE_s *pMsg)
{
    UINT32 ClientId = 0;
    UINT32 CmdInstanceIdx = 0;
    AMP_JSON_OBJECT_s *spJsonObjParam = NULL;
    AMP_JSON_OBJECT_s *spJsonObjReply = NULL;
    AMP_JSON_STRING_s *spJsonStrReply = NULL;
    char *pFilename = NULL;
    char *pFilenameRaw = NULL;
    int FilenameMaxLen = 256;
    int SentSize = 0;
    AMP_NETCTRL_DATASVC_CANCEL_TRANS_s CancelTrans = {0};
    AMP_NETCTRL_DATASVC_CANCEL_RESULT_s CancelResult = {0};
    UINT32 CancelRequestType = 0;
    int ErrorCode = 0;
    UINT32 WaitFlag = 0;
    UINT32 ActualFlag = 0;
    int ReturnValue = 0;

    ClientId = pMsg->MessageData[0];
    CmdInstanceIdx = pMsg->MessageData[1];

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&pFilename, (void**)&pFilenameRaw, FilenameMaxLen, 32);
    if (ReturnValue < 0) {
        ReplyError(AMBA_CANCEL_FILE_XFER, CmdInstanceIdx, ClientId, ERROR_NETCTRL_UNKNOWN_ERROR);
        NETCTRL_ERR("[AppLib - NetControl] <CancelFileXfer> Allocate buffer for filename fail");
        return -1;
    }

    spJsonObjReply = AmpJson_CreateObject();
    if (!spJsonObjReply) {
        NETCTRL_ERR("[AppLib - NetControl] <CancelFileXfer> AmpJson_CreateObject() fail");
        ReplyError(AMBA_CANCEL_FILE_XFER, CmdInstanceIdx, ClientId, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmbaKAL_BytePoolFree(pFilenameRaw);
        return -1;
    }

    //-- parsing Json string to retrieve arguments
    AmbaKAL_MutexTake(&gNetCtrl.MutexArgBuf, AMBA_KAL_WAIT_FOREVER);
    spJsonObjParam = AmpJson_JsonStringToJsonObject(gCmdArgBuf);
    AmpJson_GetStringByKey(spJsonObjParam, "param", pFilename, FilenameMaxLen);
    if (strstr(gCmdArgBuf, "sent_size")) {
        AmpJson_GetIntegerByKey(spJsonObjParam, "sent_size", &SentSize);
    } else {
        SentSize = 0;
    }
    AmpJson_FreeJsonObject(spJsonObjParam);
    AmbaKAL_MutexGive(&gNetCtrl.MutexArgBuf);

    ReturnValue = CancelFileXferCheck(pFilename, &CancelRequestType);
    if (ReturnValue != 0) {
        ReplyError(AMBA_CANCEL_FILE_XFER, CmdInstanceIdx, ClientId, ReturnValue);
        AmbaKAL_BytePoolFree(pFilenameRaw);
        return -1;
    }

    AmbaKAL_MutexTake(&gMutexDataSvcMgr, AMBA_KAL_WAIT_FOREVER);
    if (CancelRequestType == DATASVC_GET_FILE) {
        gDataSvcMgr[FLIE_XFER_GET].Canceled = 1;
        WaitFlag = DATA_SVC_EVENT_GET_DONE;
    } else {
        gDataSvcMgr[FLIE_XFER_PUT].Canceled = 1;
        WaitFlag = DATA_SVC_EVENT_PUT_DONE;
    }
    AmbaKAL_MutexGive(&gMutexDataSvcMgr);

    AmbaPrint("[AppLib - NetControl] <CancelFileXfer> L%d CancelRequestType = %d",__LINE__, CancelRequestType);
    CancelTrans.MsgId = CancelRequestType;
    CancelTrans.SentSize = SentSize;
    memcpy(CancelTrans.ClientInfo, gActiveClientInfo, sizeof(CancelTrans.ClientInfo));
    strcpy(CancelTrans.TransportType, gActiveTransportType);

    /* AmpNetCtrl_DataSvc_CancelDataTrans() return does not mean that cancelation is done. MW will send
          notification when cancelation is done. */
    AmpNetCtrl_DataSvc_CancelDataTrans(&(gDataInstance[0]), &CancelTrans,  &CancelResult);
    //AmbaPrintColor(CYAN,"[AppLib - NetControl] <CancelFileXfer> CancelResult.Rval = %d",CancelResult.Rval);
    if (CancelResult.Rval != 0) {
        ErrorCode = MapErrorCode(CancelResult.Rval);
        ReplyError(AMBA_CANCEL_FILE_XFER, CmdInstanceIdx, ClientId, ErrorCode);
        return -1;
    }

    ReturnValue = AmbaKAL_EventFlagTake(&gDataSvcEvent, WaitFlag, TX_AND_CLEAR, &ActualFlag, AMBA_KAL_WAIT_FOREVER);
    if (ReturnValue != OK) {
        NETCTRL_ERR("[AppLib - NetControl] <CancelFileXfer> file cancellation timeout");
        ReplyError(AMBA_CANCEL_FILE_XFER, CmdInstanceIdx, ClientId, ERROR_NETCTRL_UNKNOWN_ERROR);
        return -1;
    }

    AmpJson_AddObject(spJsonObjReply, "rval", AmpJson_CreateIntegerObject(ErrorCode));
    AmpJson_AddObject(spJsonObjReply, "msg_id", AmpJson_CreateIntegerObject(AMBA_CANCEL_FILE_XFER));

    if (CancelRequestType == DATASVC_GET_FILE) {
        AmpJson_AddObject(spJsonObjReply, "transferred_size", AmpJson_CreateIntegerObject(CancelResult.TransSize));
    }

    spJsonStrReply= AmpJson_JsonObjectToJsonString(spJsonObjReply);
     if (!spJsonStrReply) {
        NETCTRL_ERR("[AppLib - NetControl] <CancelFileXfer> AmpJson_JsonObjectToJsonString() fail");
        ReplyError(AMBA_CANCEL_FILE_XFER, CmdInstanceIdx, ClientId, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmpJson_FreeJsonObject(spJsonObjReply);
        AmbaKAL_BytePoolFree(pFilenameRaw);
        return -1;
    }

    NetCtrlSend(&(gCmdInstance[CmdInstanceIdx]), ClientId, spJsonStrReply->JsonString, strlen((char*)spJsonStrReply->JsonString));

    AmpJson_FreeJsonString(spJsonStrReply);
    AmpJson_FreeJsonObject(spJsonObjReply);
    AmbaKAL_BytePoolFree(pFilenameRaw);

    return 0;
}

#if 0
static int FileTransferCancelDone(AMP_NETCTRL_DATASVC_STATUS_s *spStatusFromDataSvc)
{
    int Token = 0;
    UINT32 ClientId = 0;
    AMP_JSON_OBJECT_s *spJsonObjReply = NULL;
    AMP_JSON_STRING_s *spJsonStrReply = NULL;

    GetActiveSession(&Token, &ClientId);

    spJsonObjReply = AmpJson_CreateObject();
    if (!spJsonObjReply) {
        NETCTRL_ERR("[AppLib - NetControl] <FileTransferCancelDone> AmpJson_CreateObject() fail");
        ReplyError(AMBA_CANCEL_FILE_XFER, gCurCmdInstanceIdx, ClientId, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    AmpJson_AddObject(spJsonObjReply, "rval", AmpJson_CreateIntegerObject(0));
    AmpJson_AddObject(spJsonObjReply, "msg_id", AmpJson_CreateIntegerObject(AMBA_CANCEL_FILE_XFER));
    if (spStatusFromDataSvc->Type == DATASVC_GET_FILE) {
        AmpJson_AddObject(spJsonObjReply, "transferred_size", AmpJson_CreateIntegerObject(spStatusFromDataSvc->Bytes));
    }

    spJsonStrReply= AmpJson_JsonObjectToJsonString(spJsonObjReply);
     if (!spJsonStrReply) {
        NETCTRL_ERR("[AppLib - NetControl] <FileTransferCancelDone> AmpJson_JsonObjectToJsonString() fail");
        ReplyError(AMBA_CANCEL_FILE_XFER, gCurCmdInstanceIdx, ClientId, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        AmpJson_FreeJsonObject(spJsonObjReply);
        return -1;
    }

    NetCtrlSend(&(gCmdInstance[gCurCmdInstanceIdx]), ClientId, spJsonStrReply->JsonString, strlen((char*)spJsonStrReply->JsonString));

    AmpJson_FreeJsonString(spJsonStrReply);
    AmpJson_FreeJsonObject(spJsonObjReply);

    return 0;
}
#endif

static int CloseDataConnection(void)
{
    AMP_NETCTRL_DATASVC_DEST_INFO_s DestInfo = {0};
    AMP_NETCTRL_DATASVC_CANCEL_TRANS_s CancelTrans = {0};
    AMP_NETCTRL_DATASVC_CANCEL_RESULT_s CancelResult = {0};
    int Status = 0;
    int RetResult = 0;
    UINT32 ActualFlag =0 ;
    int WaitFlag = 0;
    int ReturnValue = 0;

    memcpy(DestInfo.ClientInfo, gActiveClientInfo, sizeof(DestInfo.ClientInfo));
    strcpy(DestInfo.TransportType, gActiveTransportType);
    ReturnValue = AmpNetCtrl_DataSvc_GetStatus(&(gDataInstance[0]), &DestInfo, &Status);
    if (ReturnValue == 0) {
        if (Status & DATA_SERVER_SEND_BUSY) {
            NETCTRL_DBG("[AppLib - NetControl] <CloseDataConnection> DATA_SERVER_SEND_BUSY");
            CancelTrans.MsgId = DATASVC_GET_FILE;
            CancelTrans.SentSize = 0;
            memcpy(CancelTrans.ClientInfo, gActiveClientInfo, sizeof(CancelTrans.ClientInfo));
            strcpy(CancelTrans.TransportType, gActiveTransportType);
            ReturnValue = AmpNetCtrl_DataSvc_CancelDataTrans(&(gDataInstance[0]), &CancelTrans,  &CancelResult);
            if ((ReturnValue == 0) && (CancelResult.Rval == 0)) {
                WaitFlag |= DATA_SVC_EVENT_GET_DONE;
                AmbaKAL_MutexTake(&gMutexDataSvcMgr, AMBA_KAL_WAIT_FOREVER);
                gDataSvcMgr[FLIE_XFER_GET].Canceled = 1;
                AmbaKAL_MutexGive(&gMutexDataSvcMgr);
            }
        }

        if (Status & DATA_SERVER_RECV_BUSY) {
            NETCTRL_DBG("[AppLib - NetControl] <CloseDataConnection> DATA_SERVER_RECV_BUSY");
            CancelTrans.MsgId = DATASVC_PUT_FILE;
            CancelTrans.SentSize = 0;
            memcpy(CancelTrans.ClientInfo, gActiveClientInfo, sizeof(CancelTrans.ClientInfo));
            strcpy(CancelTrans.TransportType, gActiveTransportType);
            ReturnValue = AmpNetCtrl_DataSvc_CancelDataTrans(&(gDataInstance[0]), &CancelTrans,  &CancelResult);
            if ((ReturnValue == 0) && (CancelResult.Rval == 0)) {
                WaitFlag |= DATA_SVC_EVENT_PUT_DONE;
                AmbaKAL_MutexTake(&gMutexDataSvcMgr, AMBA_KAL_WAIT_FOREVER);
                gDataSvcMgr[FLIE_XFER_PUT].Canceled = 1;
                AmbaKAL_MutexGive(&gMutexDataSvcMgr);
            }
        }
    }

    if (WaitFlag) {
        ReturnValue = AmbaKAL_EventFlagTake(&gDataSvcEvent, WaitFlag, TX_AND_CLEAR, &ActualFlag, 5000);
        if (ReturnValue != 0) {
            NETCTRL_ERR("[AppLib - NetControl] <CloseDataConnection> AmbaKAL_EventFlagTake() timeout");
        }
        AmbaPrint("<%s> L%d event flag get: ActualFlag = 0x%x",__FUNCTION__, __LINE__, ActualFlag);
    }

    ReturnValue = AmpNetCtrl_DataSvc_CloseConnection(&(gDataInstance[0]),&DestInfo, &RetResult);
    if (ReturnValue != 0) {
        NETCTRL_ERR("[AppLib - NetControl] <CloseDataConnection> AmpNetCtrl_DataSvc_CloseConnection() fail");
    }

    ResetClientInfo();

    return 0;
}

static int QuerySessionHolder(APP_NETCTRL_MESSAGE_s *pMsg)
{
    UINT32 NewClientId = 0;
    UINT32 CurClientId = 0;
    int CurClientToken = 0;
    UINT32 CmdInstanceIdx = 0;
    int NewToken = 0;
    char *JsonStrBuf = NULL;
    char *JsonStrBufRaw = NULL;
    int JsonStrBufSize = 64;
    UINT32 ActualFlag = 0;
    int ReturnValue = 0;

    NewClientId = pMsg->MessageData[0];
    CmdInstanceIdx = pMsg->MessageData[1];
    GetActiveSession(&CurClientToken, &CurClientId);

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&JsonStrBuf, (void**)&JsonStrBufRaw, JsonStrBufSize, 32);
    if (ReturnValue < 0) {
        NETCTRL_ERR("[AppLib - NetControl] <QuerySessionHolder> Allocate buffer for json string fail");
        return -1;
    }

    AmbaKAL_EventFlagClear(&gDataSvcEvent, SESSION_QUERY_EVENT_HOLDER_KEEP | SESSION_QUERY_EVENT_HOLDER_GIVEUP);

    snprintf(JsonStrBuf, JsonStrBufSize, "{\"msg_id\":%d}", AMBA_QUERY_SESSION_HOLDER);
    AppLibNetControl_ReplyToLnx(JsonStrBuf, strlen(JsonStrBuf));

    /* [Spec] The camera will close the existing session and allow the creation of a new session once the
            timeout period has elapsed. The default timeout period is 800 ms. */
    ReturnValue = AmbaKAL_EventFlagTake(&gDataSvcEvent, (SESSION_QUERY_EVENT_HOLDER_KEEP | SESSION_QUERY_EVENT_HOLDER_GIVEUP),
                                        TX_OR_CLEAR, &ActualFlag, 800);
    NETCTRL_DBG("[AppLib - NetControl] <QuerySessionHolder> ReturnValue = %d, flag taken : 0x%x", ReturnValue, ActualFlag);

    if ((ReturnValue != 0) || ((ReturnValue == 0) && (ActualFlag & SESSION_QUERY_EVENT_HOLDER_GIVEUP))) {
        /* timeout or current session holder give up */
        NETCTRL_DBG("[AppLib - NetControl] <QuerySessionHolder> terminate current session");

        /* cancel current data transfer before stop session */
        CloseDataConnection();

        GenNewSessionToken(&NewToken);
        SetActiveSession(NewToken, NewClientId);
        snprintf(JsonStrBuf, JsonStrBufSize, "{\"rval\":0,\"msg_id\":%d,\"param\":%d}", AMBA_START_SESSION, NewToken);
        NetCtrlSend(&(gCmdInstance[gCurCmdInstanceIdx]), NewClientId, JsonStrBuf, strlen(JsonStrBuf));
    } else if (ActualFlag & SESSION_QUERY_EVENT_HOLDER_KEEP) {
        NETCTRL_DBG("[AppLib - NetControl] <QuerySessionHolder> keep current session");
        snprintf(JsonStrBuf, JsonStrBufSize, "{\"rval\":%d,\"msg_id\":%d}", ERROR_NETCTRL_SESSION_START_FAIL, AMBA_START_SESSION);
        NetCtrlSend(&(gCmdInstance[gCurCmdInstanceIdx]), NewClientId, JsonStrBuf, strlen(JsonStrBuf));
    } else {
        NETCTRL_ERR("[AppLib - NetControl] <QuerySessionHolder> unexpected case (ReturnValue = %d, ActualFlag = 0x%x)", ReturnValue, ActualFlag);
        snprintf(JsonStrBuf, JsonStrBufSize, "{\"rval\":%d,\"msg_id\":%d}", ERROR_NETCTRL_UNKNOWN_ERROR, AMBA_START_SESSION);
        NetCtrlSend(&(gCmdInstance[gCurCmdInstanceIdx]), NewClientId, JsonStrBuf, strlen(JsonStrBuf));
    }

    AmbaKAL_BytePoolFree(JsonStrBufRaw);
    return 0;
}

static int SessionHolderReply(char *CmdBuf)
{
    AMP_JSON_OBJECT_s *spJsonObjParam = NULL;
    UINT32 EventFlag = 0;
    int Rval = 0;

    if (CmdBuf) {
        spJsonObjParam = AmpJson_JsonStringToJsonObject(CmdBuf);
        AmpJson_GetIntegerByKey(spJsonObjParam, "rval", &Rval);
        NETCTRL_DBG("[AppLib - NetControl] <SessionHolderReply> rval=%d", Rval);
        AmpJson_FreeJsonObject(spJsonObjParam);

        EventFlag = (Rval == 0) ? SESSION_QUERY_EVENT_HOLDER_KEEP : SESSION_QUERY_EVENT_HOLDER_GIVEUP;
        AmbaKAL_EventFlagGive(&gDataSvcEvent, EventFlag);
    }

    return 0;
}

static int FileTransferDone(AMP_NETCTRL_DATASVC_STATUS_s *spStatusFromDataSvc)
{
    AMP_JSON_OBJECT_s *spJsonResObj = NULL;
    AMP_JSON_OBJECT_s *spJsonObjType = NULL;
    AMP_JSON_STRING_s *spReplyStrObj = NULL;
    AMP_JSON_OBJECT_s *Array = NULL;
    int Token = 0;
    UINT32 ClientId = 0;
    char Md5sum[36] = {0};
    int ReturnValue = 0;

    /* Status is non-zero value when get/put file fail. And you could figure out what error is happened by error type and no */
    NETCTRL_DBG("[AppLib - NetControl] <FileTransferDone> status=%d", spStatusFromDataSvc->Status);
    NETCTRL_DBG("[AppLib - NetControl] <FileTransferDone> Type=%u", spStatusFromDataSvc->Type);

    memcpy(Md5sum, spStatusFromDataSvc->Md5sum, sizeof(spStatusFromDataSvc->Md5sum));
    Md5sum[sizeof(spStatusFromDataSvc->Md5sum)] = 0x00;

    GetActiveSession(&Token, &ClientId);

    if(spStatusFromDataSvc->Type == DATASVC_GET_FILE) {
        if (spStatusFromDataSvc->Status == 0) {
            spJsonObjType = AmpJson_CreateStringObject("get_file_complete");
        } else {
            spJsonObjType = AmpJson_CreateStringObject("get_file_fail");
        }
    } else {
        /* DATASVC_PUT_FILE */
        if (spStatusFromDataSvc->Status == 0) {
            spJsonObjType = AmpJson_CreateStringObject("put_file_complete");
        } else {
            spJsonObjType = AmpJson_CreateStringObject("put_file_fail");
        }
    }

    spJsonResObj = AmpJson_CreateObject();
    if (!spJsonResObj) {
        NETCTRL_ERR("[AppLib - NetControl] <FileTransferDone> spJsonResObj is NULL");
        AmpJson_FreeJsonObject(spJsonObjType);
        return -1;
    }

    AmpJson_AddObject(spJsonResObj, "token", AmpJson_CreateIntegerObject(Token));
    AmpJson_AddObject(spJsonResObj, "msg_id", AmpJson_CreateIntegerObject(AMBA_NOTIFICATION));
    AmpJson_AddObject(spJsonResObj, "type", spJsonObjType);
    Array = AmpJson_CreateArrayObject();
    if (spStatusFromDataSvc->Type == DATASVC_GET_FILE) {
        AmpJson_AddObject(Array, "bytes sent", AmpJson_CreateIntegerObject(spStatusFromDataSvc->Bytes));
    } else {
        AmpJson_AddObject(Array, "bytes received", AmpJson_CreateIntegerObject(spStatusFromDataSvc->Bytes));
    }
    AmpJson_AddObject(Array, "md5sum", AmpJson_CreateStringObject(Md5sum));
    AmpJson_AddObject(spJsonResObj, "param", Array);

    spReplyStrObj = AmpJson_JsonObjectToJsonString(spJsonResObj);
    if (spReplyStrObj != NULL) {
        NetCtrlSend(&(gCmdInstance[gCurCmdInstanceIdx]), ClientId, spReplyStrObj->JsonString, strlen(spReplyStrObj->JsonString));
    } else {
        NETCTRL_ERR("[AppLib - NetControl] <FileTransferDone> spReplyStrObj is NULL");
        AmpJson_FreeJsonObject(spJsonResObj);
        return -1;
    }

    //NETCTRL_DBG("[AppLib - NetControl] <FileTransferDone> send status to RTOS App handler"); // check if needed

    if (Array) {
        AmpJson_FreeJsonObject(Array->ObjValue.Array);
    }

    AmpJson_FreeJsonString(spReplyStrObj);
    AmpJson_FreeJsonObject(spJsonResObj);

    return ReturnValue;

}

static int ThumbTransferDone(AMP_NETCTRL_DATASVC_STATUS_s *spStatusFromDataSvc)
{
    AMP_JSON_OBJECT_s *spJsonResObj = NULL;
    AMP_JSON_OBJECT_s *spJsonObjType = NULL;
    AMP_JSON_STRING_s *spReplyStrObj = NULL;
    int Token = 0;
    UINT32 ClientId = 0;
    char Md5sum[36] = {0};
    int ReturnValue = 0;

    /* Status is non-zero value when get/put file fail. And you could figure out what error is happened by error type and no */
    NETCTRL_DBG("[AppLib - NetControl] <ThumbTransferDone> status=%d", spStatusFromDataSvc->Status);
    NETCTRL_DBG("[AppLib - NetControl] <ThumbTransferDone> Type=%u", spStatusFromDataSvc->Type);

    memcpy(Md5sum, spStatusFromDataSvc->Md5sum, sizeof(spStatusFromDataSvc->Md5sum));
    Md5sum[sizeof(spStatusFromDataSvc->Md5sum)] = 0x00;

    GetActiveSession(&Token, &ClientId);

    spJsonResObj = AmpJson_CreateObject();
    if (!spJsonResObj) {
        NETCTRL_ERR("[AppLib - NetControl] <RecvDataSvcStatus> spJsonResObj is NULL");
        AmpJson_FreeJsonObject(spJsonObjType);
        ReplyError(AMBA_GET_THUMB, gCurCmdInstanceIdx, ClientId, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    AmpJson_AddObject(spJsonResObj, "rval", AmpJson_CreateIntegerObject(spStatusFromDataSvc->Status));
    AmpJson_AddObject(spJsonResObj, "msg_id", AmpJson_CreateIntegerObject(AMBA_GET_THUMB));
    if (spStatusFromDataSvc->Status == 0) {
        AmpJson_AddObject(spJsonResObj, "size", AmpJson_CreateIntegerObject(spStatusFromDataSvc->Bytes));
        AmpJson_AddObject(spJsonResObj, "type", AmpJson_CreateStringObject(RequestThumbType));
        AmpJson_AddObject(spJsonResObj, "md5sum", AmpJson_CreateStringObject(Md5sum));
    }

    spReplyStrObj = AmpJson_JsonObjectToJsonString(spJsonResObj);
    if (spReplyStrObj != NULL) {
        NETCTRL_DBG("[AppLib - NetControl] <RecvDataSvcStatus> spReplyStrObj->szJsonStr = %s", spReplyStrObj->JsonString);
        NetCtrlSend(&(gCmdInstance[gCurCmdInstanceIdx]), ClientId, spReplyStrObj->JsonString, strlen(spReplyStrObj->JsonString));
    } else {
        NETCTRL_ERR("[AppLib - NetControl] <RecvDataSvcStatus> spReplyStrObj is NULL");
        AmpJson_FreeJsonObject(spJsonResObj);
        ReplyError(AMBA_GET_THUMB, gCurCmdInstanceIdx, ClientId, ERROR_NETCTRL_JSON_PACKAGE_ERROR);
        return -1;
    }

    NETCTRL_DBG("[AppLib - NetControl] <RecvDataSvcStatus> send status to RTOS App handler"); // check if needed

    AmpJson_FreeJsonString(spReplyStrObj);
    AmpJson_FreeJsonObject(spJsonResObj);

    return ReturnValue;
}

static int PrepareCmdArg(APP_NETCTRL_MESSAGE_s *pMsg, UINT32 *pParam1, UINT32 *pParam2)
{
    int ClientId = pMsg->MessageData[0];
    UINT32 CmdInstancIdx = pMsg->MessageData[1];
    int Len = 0;
    char *pBuf = NULL;
    char *pBufRaw = NULL;
    int ReturnValue = 0;

    *pParam1 = 0;
    *pParam2 = 0;

    AmbaKAL_MutexTake(&gNetCtrl.MutexArgBuf, AMBA_KAL_WAIT_FOREVER);

    Len = strlen(gCmdArgBuf) + 1;
    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&pBuf, (void**)&pBufRaw, Len, 32);
    if (ReturnValue < 0) {
        ReplyError(pMsg->MessageID, CmdInstancIdx, ClientId, ERROR_NETCTRL_UNKNOWN_ERROR);
        NETCTRL_ERR("[AppLib - NetControl] <PrepareCmdArg> Allocate buffer for cmd arguments fail");
        ReturnValue = -1;
    } else {
        strcpy(pBuf, gCmdArgBuf);
        *pParam1 = (UINT32)pBuf;
        *pParam2 = (UINT32)pBufRaw;
    }

    AmbaKAL_MutexGive(&gNetCtrl.MutexArgBuf);

    return ReturnValue;
}

//-- return 1: has completed cmd
static int ComposeCmdFromLinx(AMP_NETCTRL_CMD_s *spDataFromLnx)
{
    char *pTemp = NULL;
	UINT32 StartOffset = 0;
    UINT32 StringLen = 0;
    static INT32 BraceCount = 0;
    static UINT32 Len = 0;

    #if 0
    AmbaPrint("[AppLib - NetControl] <ComposeCmdFromLinx> param=-%s- size=%d",
                                                            spDataFromLnx->Param,
                                                            spDataFromLnx->ParamSize);
    #endif

    pTemp = spDataFromLnx->Param;
    StringLen = spDataFromLnx->ParamSize;

    if (BraceCount == 0) {
        memset(ComposeCmdBuf, 0, sizeof(ComposeCmdBuf));
        Len = 0;
    }

    if ((sizeof(ComposeCmdBuf) - Len) < StringLen) {
        NETCTRL_ERR("[AppLib - NetControl] <ComposeCmdFromLinx> ComposeCmdBuf is not enough");
        return -2;
    }

    while (StringLen) {
        if (*pTemp == '{') {
			if (BraceCount == 0) {
                StartOffset = (UINT32)(pTemp - spDataFromLnx->Param);
            }
            BraceCount++;
        } else if (*pTemp == '}') {
            BraceCount--;
			if (BraceCount == 0) {
                *(pTemp+1) = '\0';
                break;
            }
        } else {
            ;
        }

        pTemp++;
        StringLen--;
    }

    snprintf(ComposeCmdBuf+Len, sizeof(ComposeCmdBuf)-Len,"%s", spDataFromLnx->Param+StartOffset);
    Len += spDataFromLnx->ParamSize - StartOffset - 1;

    if ((BraceCount == 0) && (*ComposeCmdBuf == '{')) {
        AmbaPrint("[AppLib - NetControl] <ComposeCmdFromLinx> ComposeCmdBuf = -%s-", ComposeCmdBuf);
        return 1;
    }

    return 0;
}

static void AppLibNetControl_MgrTask(UINT32 info)
{
    APP_NETCTRL_MESSAGE_s Msg = {0};
    UINT32 MsgToApp = 0;
    UINT32 Data1 = 0, Data2 = 0;
    UINT32 Param1 = 0, Param2 = 0;
    UINT8 SendMsgToApp = 1;
    int TokenNum = 0;
    UINT32 ClientId = 0;
    int ReturnValue = 0;

    //NETCTRL_DBG("[AppLib - NetControl] <MgrTask> NetCtrl manager ready");

    while (1) {
        SendMsgToApp = 1;
        AmbaKAL_MsgQueueReceive(&gNetCtrl.MsgQueue, (void *)&Msg, AMBA_KAL_WAIT_FOREVER);
        Data1 = Msg.MessageData[0];
        Data2 = Msg.MessageData[1];
        //NETCTRL_DBG("[AppLib - NetControl] <MgrTask> Received msg: 0x%X (Param1 = 0x%X, Param2 = 0x%X)",
        //                                                   Msg.MessageID, Data1, Data2);

        switch (Msg.MessageID) {
        case AMBA_START_SESSION:
            SetActiveSession(Msg.Token, Data1);
            Param1 = Msg.Token;
            Param2 = Data1;
            MsgToApp = AMSG_NETCTRL_SESSION_START;
            break;
        case AMBA_STOP_SESSION:
            CloseDataConnection();
            ResetActiveSession();
            MsgToApp = AMSG_NETCTRL_SESSION_STOP;
            break;
        case AMBA_RESET_VF:
            MsgToApp = AMSG_NETCTRL_VF_RESET;
            break;
        case AMBA_STOP_VF:
            MsgToApp = AMSG_NETCTRL_VF_STOP;
            break;
        case AMBA_RECORD_START:
            MsgToApp = AMSG_NETCTRL_VIDEO_RECORD_START;
            break;
        case AMBA_RECORD_STOP:
            MsgToApp = AMSG_NETCTRL_VIDEO_RECORD_STOP;
            break;
        case AMBA_GET_RECORD_TIME:
            MsgToApp = AMSG_NETCTRL_VIDEO_GET_RECORD_TIME;
            break;
        case AMBA_GET_ALL_CURRENT_SETTINGS:
            MsgToApp = AMSG_NETCTRL_SYS_GET_SETTING_ALL;
            break;
        case AMBA_GET_SINGLE_SETTING_OPTIONS:
            MsgToApp = AMSG_NETCTRL_SYS_GET_SINGLE_SETTING_OPTION;
            ReturnValue = PrepareCmdArg(&Msg, &Param1, &Param2);
            SendMsgToApp = (ReturnValue == 0) ? 1 : 0;
            break;
        case AMBA_GET_SETTING:
            MsgToApp = AMSG_NETCTRL_SYS_GET_SETTING;
            ReturnValue = PrepareCmdArg(&Msg, &Param1, &Param2);
            SendMsgToApp = (ReturnValue == 0) ? 1 : 0;
            break;
        case AMBA_SET_SETTING:
            MsgToApp = AMSG_NETCTRL_SYS_SET_SETTING;
            ReturnValue = PrepareCmdArg(&Msg, &Param1, &Param2);
            SendMsgToApp = (ReturnValue == 0) ? 1 : 0;
            break;
        case AMBA_GET_NUMB_FILES:
            MsgToApp = AMSG_NETCTRL_SYS_GET_NUMB_FILES;
            ReturnValue = PrepareCmdArg(&Msg, &Param1, &Param2);
            SendMsgToApp = (ReturnValue == 0) ? 1 : 0;
            break;
        case AMBA_GET_DEVICEINFO:
            MsgToApp = AMSG_NETCTRL_SYS_GET_DEVICE_INFO;
            break;
        case AMBA_TAKE_PHOTO:
            MsgToApp = AMSG_NETCTRL_PHOTO_TAKE_PHOTO;
            break;
        case AMBA_CONTINUE_CAPTURE_STOP:
            MsgToApp = AMSG_NETCTRL_PHOTO_CONTINUE_CAPTURE_STOP;
            break;
        case AMBA_GET_THUMB:
            MsgToApp = AMSG_NETCTRL_MEDIA_GET_THUMB;
            ReturnValue = PrepareCmdArg(&Msg, &Param1, &Param2);
            SendMsgToApp = (ReturnValue == 0) ? 1 : 0;
            break;
        case AMBA_GET_MEDIAINFO:
            MsgToApp = AMSG_NETCTRL_MEDIA_GET_MEDIAINFO;
            ReturnValue = PrepareCmdArg(&Msg, &Param1, &Param2);
            SendMsgToApp = (ReturnValue == 0) ? 1 : 0;
            break;
        case AMBA_SET_MEDIA_ATTRIBUTE:
            SendMsgToApp = 0;
            SetMediaAttribute(&Msg);
            break;
        case AMBA_DEL_FILE:
            SendMsgToApp = 0;
            DeleteFile(&Msg);
            break;
        case AMBA_LS:
        case AMBA_CD:
        case AMBA_PWD:
        case AMBA_WIFI_RESTART:
        case AMBA_WIFI_START:
        case AMBA_WIFI_STOP:
        case AMBA_SET_WIFI_SETTING:
        case AMBA_GET_WIFI_SETTING:
        case AMBA_GET_WIFI_STATUS:
            SendMsgToApp = 0;
            LinuxCmd(&Msg);
            break;
        case AMBA_SET_CLNT_INFO:
            SendMsgToApp = 0;
            ParseClientInfo(&Msg);
            break;
        case AMBA_GET_FILE:
            SendMsgToApp = 0;
            GetFile(&Msg);
            break;
        case AMBA_PUT_FILE:
            SendMsgToApp = 0;
            PutFile(&Msg);
            break;
        case AMBA_FORMAT:
            MsgToApp = AMSG_NETCTRL_SYS_FORMAT;
            ReturnValue = PrepareCmdArg(&Msg, &Param1, &Param2);
            SendMsgToApp = (ReturnValue == 0) ? 1 : 0;
            break;
        case AMBA_GET_SPACE:
            MsgToApp = AMSG_NETCTRL_SYS_GET_SPACE;
            ReturnValue = PrepareCmdArg(&Msg, &Param1, &Param2);
            SendMsgToApp = (ReturnValue == 0) ? 1 : 0;
            break;
        case AMBA_CANCEL_FILE_XFER:
            SendMsgToApp = 0;
            CancelFileXfer(&Msg);
            break;
        case AMBA_QUERY_SESSION_HOLDER:
            SendMsgToApp = 0;
            QuerySessionHolder(&Msg);
            break;
        default:
            if (Msg.MessageID >= AMBA_CUSTOM_CMD_BASE) {
                /* custom commands */
                MsgToApp = AMSG_NETCTRL_CUSTOM_CMD;
                ReturnValue = PrepareCmdArg(&Msg, &Param1, &Param2);
                SendMsgToApp = (ReturnValue == 0) ? 1 : 0;
            } else {
                /* unsupported commands */
                SendMsgToApp = 0;
                NETCTRL_ERR("[AppLib - NetControl] <MgrTask> OPERATION_UNSUPPORTED! (Msg.MessageID = %d)",Msg.MessageID);
                GetActiveSession(&TokenNum, &ClientId);
                ReplyError(Msg.MessageID, Data2, ClientId, ERROR_NETCTRL_OPERATION_UNSUPPORTED);
            }

            break;
        }

        if (SendMsgToApp) {
            AppLibComSvcHcmgr_SendMsgNoWait(MsgToApp, Param1, Param2);
        }

    }

}


/**
 *  @brief Callback function for receiving net control command from linux
 *
 *  @param [in] spDataFromLnx net control data
 *
 *  @return 0 success, <0 failure
 *  @see AMP_NETCTRL_DATA_s
 */
int AppLibNetControl_RecvCmd(AMP_NETCTRL_HDLR_INFO_s *HdlrInfo, AMP_NETCTRL_CMD_s *spDataFromLnx)
{
    int Token = 0;
    int MsgId = 0;
    APP_NETCTRL_MESSAGE_s Msg = {0};
    AMP_JSON_OBJECT_s *spJsonObjParam = NULL;
    int ActiveToken = NETCTRL_INVALID_SESSION;
    UINT32 ActiveClientId = 0;
    static UINT8 FlagSkipCmd = 0;
    int ReturnValue = 0;

    if (!gNetCtrlInited) {
        NETCTRL_ERR("[AppLib - NetControl] <RecvCmd> Please init net control applib first");
        return ERROR_NETCTRL_UNKNOWN_ERROR;
    }

    if (spDataFromLnx != NULL) {
        //NETCTRL_DBG("[AppLib - NetControl] <RecvCmd> param=%s size=%d ClientId=%d", spDataFromLnx->Param,
        //                                                    spDataFromLnx->ParamSize, spDataFromLnx->ClientId);
        NETCTRL_DBG("\n[AppLib - NetControl] <RecvCmd> ClientId=%d", spDataFromLnx->ClientId);
    } else {
        NETCTRL_ERR("[AppLib - NetControl] <RecvCmd> spDataFromLnx is NULL.");
        return ERROR_NETCTRL_UNKNOWN_ERROR;
    }

    ReturnValue = ComposeCmdFromLinx(spDataFromLnx);
    if (ReturnValue == -2) {
        return -1;
    } else if (ReturnValue != 1) {
        NETCTRL_DBG("[AppLib - NetControl] <RecvCmd> Cmd from Linx is not complete...");
        return 0;
    }

    ReturnValue = FindCmdInstance(HdlrInfo, &gCurCmdInstanceIdx);
    if (ReturnValue != 0) {
        ReplyError(MsgId, 0, spDataFromLnx->ClientId, ERROR_NETCTRL_SESSION_START_FAIL);    // TODO: update
        return ERROR_NETCTRL_SESSION_START_FAIL;
    }

    #if 0 // json lib doesn't not work when using workaround netCtrl lib
    sscanf(spDataFromLnx->Param, "%*[^:]:%d %*[^:]:%d", &Token, &MsgId);
    NETCTRL_DBG("[AppLib - NetControl] <RecvCmd> Token=%d(%X) MsgId=%d", Token, Token, MsgId);
    #else
    spJsonObjParam = AmpJson_JsonStringToJsonObject(ComposeCmdBuf);
    AmpJson_GetIntegerByKey(spJsonObjParam, "token", &Token);
    AmpJson_GetIntegerByKey(spJsonObjParam, "msg_id", &MsgId);
    //NETCTRL_DBG("[AppLib - NetControl] <RecvCmd> Token=%d MsgId=%d", Token, MsgId);
    AmpJson_FreeJsonObject(spJsonObjParam);
    #endif

    if (MsgId == AMBA_START_SESSION) {
        #if 1
        GetActiveSession(&ActiveToken , &ActiveClientId);
        if (ActiveToken != NETCTRL_INVALID_SESSION) {
            NETCTRL_DBG("[AppLib - NetControl] <RecvCmd> There's a session existed already");
            Msg.Token = ActiveToken;
            Msg.MessageID = AMBA_QUERY_SESSION_HOLDER;
            Msg.MessageData[0] = spDataFromLnx->ClientId;
            Msg.MessageData[1] = gCurCmdInstanceIdx;
        } else {
            ReturnValue = GenNewSessionToken(&ActiveToken);
            if (ReturnValue != OK) {
                ReplyError(MsgId, gCurCmdInstanceIdx, spDataFromLnx->ClientId, ERROR_NETCTRL_SESSION_START_FAIL);
                return ERROR_NETCTRL_SESSION_START_FAIL;
            }

            Msg.Token = ActiveToken;
            Msg.MessageID = MsgId;
            Msg.MessageData[0] = spDataFromLnx->ClientId;
            Msg.MessageData[1] = gCurCmdInstanceIdx;

            FlagSkipCmd = 0;
        }
        #else
        ReturnValue = GenNewSessionToken(&ActiveToken);
        if (ReturnValue != OK) {
            ReplyError(MsgId, gCurCmdInstanceIdx, spDataFromLnx->ClientId, ERROR_NETCTRL_SESSION_START_FAIL);

            return ERROR_NETCTRL_SESSION_START_FAIL;
        }

        Msg.Token = ActiveToken;
        Msg.MessageID = MsgId;
        Msg.MessageData[0] = spDataFromLnx->ClientId;
        Msg.MessageData[1] = gCurCmdInstanceIdx;

        FlagSkipCmd = 0;
        #endif
    } else if (MsgId == AMBA_QUERY_SESSION_HOLDER) {
        SessionHolderReply(ComposeCmdBuf);
        return 0;
    } else {
        if (FlagSkipCmd) {
            NETCTRL_ERR("[AppLib - NetControl] <RecvCmd> Cmd Skipped. Please start session first");
            ReplyError(MsgId, gCurCmdInstanceIdx, spDataFromLnx->ClientId, ERROR_NETCTRL_INVALID_OPERATION);
            return ERROR_NETCTRL_INVALID_OPERATION; //update
        }

        GetActiveSession(&ActiveToken , &ActiveClientId);
        if ((ActiveToken == NETCTRL_INVALID_SESSION) ||
            (ActiveToken != Token)) {
            NETCTRL_ERR("[AppLib - NetControl] <RecvCmd> invalid token '%d'",Token);
            ReplyError(MsgId, gCurCmdInstanceIdx, spDataFromLnx->ClientId, ERROR_NETCTRL_INVALID_TOKEN);
            return ERROR_NETCTRL_INVALID_TOKEN;
        }

        if (MsgId == AMBA_STOP_SESSION) {
            FlagSkipCmd = 1;
        }

        AmbaKAL_MutexTake(&gNetCtrl.MutexArgBuf, AMBA_KAL_WAIT_FOREVER);
        snprintf(gCmdArgBuf, sizeof(gCmdArgBuf), "%s", ComposeCmdBuf);
        AmbaKAL_MutexGive(&gNetCtrl.MutexArgBuf);

        Msg.Token = ActiveToken;
        Msg.MessageID = MsgId;
        Msg.MessageData[0] = spDataFromLnx->ClientId;
        Msg.MessageData[1] = gCurCmdInstanceIdx;
    }

    //-- send cmd to netCtrl task
    AmbaKAL_MsgQueueSend(&gNetCtrl.MsgQueue, &Msg, AMBA_KAL_NO_WAIT);

    return ReturnValue;
}


/**
 *  @brief Callback function for receiving status from data server
 *
 *  @param [in] spStatusFromDataSvc data service status
 *
 *  @return 0 success, <0 failure
 */
int AppLibNetControl_RecvDataSvcStatus(AMP_NETCTRL_DATASVC_HDLR_INFO_s *DataHdlrInfo, AMP_NETCTRL_DATASVC_STATUS_s *spStatusFromDataSvc)
{
    UINT8 Canceled = 0;

    if (!gNetCtrlInited) {
        NETCTRL_ERR("[AppLib - NetControl] <RecvDataSvcStatus> Please init net control applib first");
        return -1;
    }

    if(spStatusFromDataSvc == NULL) {
        NETCTRL_ERR("[AppLib - NetControl] <RecvDataSvcStatus> spStatusFromDataSvc is NULL");
        return -1;
    }

    /* Status is non-zero value when get/put file fail. And you could figure out what error is happened by error type and no */
    //NETCTRL_DBG("[AppLib - NetControl] <RecvDataSvcStatus> status=%d (0x%x)", spStatusFromDataSvc->Status, spStatusFromDataSvc->Status);
    //NETCTRL_DBG("[AppLib - NetControl] <RecvDataSvcStatus> Type=%u", spStatusFromDataSvc->Type);

    switch (spStatusFromDataSvc->Type) {
    case DATASVC_GET_FILE:
        AmbaKAL_MutexTake(&gMutexDataSvcMgr, AMBA_KAL_WAIT_FOREVER);
        Canceled = gDataSvcMgr[FLIE_XFER_GET].Canceled;
        memset(&(gDataSvcMgr[FLIE_XFER_GET]), 0 , sizeof(NETCTRL_DATA_SVC_MGR_s));
        AmbaKAL_MutexGive(&gMutexDataSvcMgr);

        if (Canceled == 0) {
            FileTransferDone(spStatusFromDataSvc);
        }

        AmbaKAL_EventFlagGive(&gDataSvcEvent, DATA_SVC_EVENT_GET_DONE);
        break;
    case DATASVC_PUT_FILE:
        AmbaKAL_MutexTake(&gMutexDataSvcMgr, AMBA_KAL_WAIT_FOREVER);
        Canceled = gDataSvcMgr[FLIE_XFER_PUT].Canceled;
        memset(&(gDataSvcMgr[FLIE_XFER_PUT]), 0 , sizeof(NETCTRL_DATA_SVC_MGR_s));
        AmbaKAL_MutexGive(&gMutexDataSvcMgr);

        if (Canceled == 0) {
            FileTransferDone(spStatusFromDataSvc);
        }

        AmbaKAL_EventFlagGive(&gDataSvcEvent, DATA_SVC_EVENT_PUT_DONE);
        break;
    case DATASVC_GET_THUMB:
        ThumbTransferDone(spStatusFromDataSvc);
        break;
    default:
        break;
    }

    return 0;
}


/**
 *  @brief Reply the result to linux after executing net control command that issuing from linux
 *
 *  @param [in] pStr pointer to json string
 *  @param [in] StringLength the length of json string 'pStr'
 *
 *  @return 0 success, <0 failure
 */
int AppLibNetControl_ReplyToLnx(char *pStr, UINT32 StringLength)
{
    int TokenNumber = 0;
    UINT32 ClientId = 0;

    if (!gNetCtrlInited) {
        NETCTRL_ERR("[AppLib - NetControl] <ReplyToLnx> Please init net control applib first");
        return NG;
    }

    if (pStr == NULL) {
        NETCTRL_ERR("[AppLib - NetControl] <ReplyToLnx> invalid param (pStr is NULL)");
        return NG;
    }
    //NETCTRL_DBG("[AppLib - NetControl] <ReplyToLnx> json str: %s (len: %d)", pStr, StringLength);

    GetActiveSession(&TokenNumber, &ClientId);
    NetCtrlSend(&(gCmdInstance[gCurCmdInstanceIdx]), ClientId, pStr, StringLength);

    return OK;
}

int AppLibNetControl_ReplyErrorCode(int MsgId, int ErrorCode)
{
    int TokenNumber = 0;
    UINT32 ClientId = 0;

    memset(gJsonStringBuf, 0, JsonStringBufSize);
    GetActiveSession(&TokenNumber, &ClientId);

    snprintf(gJsonStringBuf, JsonStringBufSize,"{\"rval\":%d,\"msg_id\":%d}", ErrorCode, MsgId);
    NetCtrlSend(&(gCmdInstance[gCurCmdInstanceIdx]), ClientId, gJsonStringBuf, strlen(gJsonStringBuf));

    return 0;
}

int AppLibNetControl_SendThumb(UINT8 *DataBuf, UINT32 DataSize, char *ThumbType)
{
    AMP_NETCTRL_DATASVC_DEST_INFO_s DestInfo = {0};
    AMP_NETCTRL_DATASVC_DATA_s DataReq = {0};
    AMP_NETCTRL_DATASVC_RESULT_s DataResult = {0};
    int Status = 0;
    int ReturnValue = 0;

    if ((!DataBuf) || (DataSize == 0) || (!ThumbType)) {
        NETCTRL_ERR("[AppLib - NetControl] <SendThumb> invalid param");
        return -2;
    }

    memcpy(DestInfo.ClientInfo, gActiveClientInfo, sizeof(DestInfo.ClientInfo));
    strcpy(DestInfo.TransportType, gActiveTransportType);

    ReturnValue = AmpNetCtrl_DataSvc_GetStatus(&(gDataInstance[0]), &DestInfo, &Status);
    if (ReturnValue != 0) {
        NETCTRL_ERR("[AppLib - NetControl] <SendThumb> AmpNetCtrl_DataSvc_GetStatus fail (%d)", ReturnValue);
        return -2;
    } else if (Status & (DATA_SERVER_SEND_BUSY | DATA_SERVER_CLOSE_CONNECT)) {
        NETCTRL_ERR("[AppLib - NetControl] <SendThumb> data server busy (Status = 0x%x)", Status);
        return -3;
    }

    snprintf(RequestThumbType, sizeof(RequestThumbType), "%s", ThumbType);

    DataReq.MsgId = DATASVC_GET_THUMB;
    DataReq.Offset = 0;
    DataReq.FetchFilesize = DataSize;
    DataReq.BufferAddr = (char*)DataBuf;
    memcpy(DataReq.ClientInfo, gActiveClientInfo, sizeof(DataReq.ClientInfo));
    strcpy(DataReq.TransportType, gActiveTransportType);
    ReturnValue = AmpNetCtrl_DataSvc_Send(&(gDataInstance[0]), &DataReq, &DataResult);
    if (ReturnValue != 0) {
        NETCTRL_ERR("[AppLib - NetControl] <SendThumb> AmpNetCtrl_DataSvc_Send fail (ReturnValue = %d)", ReturnValue);
        return -1;
    }

    return 0;
}

int AppLibNetControl_GetCurrentWorkDir(char *Buf, UINT32 BufSize)
{
    AMP_NETCTRL_LNXCMD_RESULT_s LnxResult = {0};
    int CmdRetBufSize = 64;
    char *pCmdRetBuf = NULL;
    char *pCmdRetBufRaw = NULL;
    int ReturnValue = 0;

    if ((!Buf) || (BufSize == 0)) {
        NETCTRL_ERR("[AppLib - NetControl] <GetCurrentWorkDir> invalid param");
        return -1;
    }

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&pCmdRetBuf, (void**)&pCmdRetBufRaw, CmdRetBufSize, 32);
    if (ReturnValue < 0) {
        NETCTRL_ERR("[AppLib - NetControl] <GetCurrentWorkDir> Allocate buffer fail");
        return -1;
    }

    ReturnValue = AmpNetCtrl_LnxPwd(pCmdRetBuf, CmdRetBufSize, &LnxResult);
    if ((ReturnValue != 0) || (LnxResult.Rval != 0)) {
        NETCTRL_ERR("[AppLib - NetControl] <GetCurrentWorkDir> AmpNetCtrl_LnxPwd fail");
        return -1;
    }

    snprintf(Buf, BufSize, "%s", pCmdRetBuf);
    AmbaKAL_BytePoolFree(pCmdRetBufRaw);

    return 0;
}

static int InitNetCtrl(void *pMemoryPool)
{
    AMP_NETCTRL_INIT_CFG_s InitCfg;
    AMP_NETCTRL_CFG_s InstanceDefaultCfg;
    AMP_NETCTRL_HDLR_INFO_s CmdHdlrInfo;
    AMP_NETCTRL_DATASVC_HDLR_INFO_s DataHdlrInfo;
    AMBA_KAL_BYTE_POOL_t *pMemPool = (AMBA_KAL_BYTE_POOL_t *)pMemoryPool;
    int ReturnValue = 0;

    if (!pMemoryPool) {
        NETCTRL_ERR("[AppLib - NetControl] <InitNetCtrl> pMemoryPool is NULL");
        return -1;
    }

    ReturnValue = AmpNetCtrl_GetInitDefaultCfg(&InitCfg);
    InitCfg.MemPoolAddr = (UINT8 *)pMemoryPool;
    InitCfg.MemPoolSize = pMemPool->tx_byte_pool_size;
    ReturnValue = AmpNetCtrl_Init(&InitCfg);
    if (ReturnValue != AMP_OK) {
        NETCTRL_ERR("[AppLib - NetControl] <InitNetCtrl> AmpNetCtrl_Init() fail.(error code: %d)",ReturnValue);
        return -1;
    }

    /* Cretae instance of command server*/
    ReturnValue = AmpNetCtrl_GetInstanceDefaultCfg(&InstanceDefaultCfg);
    if (ReturnValue != AMP_OK) {
        NETCTRL_ERR("[AppLib - NetControl] <InitNetCtrl> AmpNetCtrl_GetInstanceDefaultCfg() fail.(error code: %d)",ReturnValue);
        return -1;
    }

    InstanceDefaultCfg.InstTypeId = INST_TYPE_LNX_WIFI_BT; //TODO: update
    ReturnValue = AmpNetCtrl_CreateInstance(&InstanceDefaultCfg, &CmdHdlrInfo);
    if (ReturnValue != AMP_OK) {
        NETCTRL_ERR("[AppLib - NetControl] <InitNetCtrl> AmpNetCtrl_CreateInstance() fail.(error code: %d)",ReturnValue);
        return -1;
    }

    gCmdInstance[0].InstTypeId = CmdHdlrInfo.InstTypeId;
    gCmdInstance[0].Hdlr = CmdHdlrInfo.Hdlr;

    ReturnValue = AmpNetCtrl_RegCmdRcvCB(&CmdHdlrInfo, AppLibNetControl_RecvCmd);
    if (ReturnValue != AMP_OK) {
        NETCTRL_ERR("[AppLib - NetControl] <InitNetCtrl> AmpNetCtrl_RegCmdRcvCB() fail.(error code: %d)",ReturnValue);
        return -1;
    }


    /* Cretae instance of data server*/
    ReturnValue = AmpNetCtrl_DataSvc_GetInstanceDefaultCfg(&InstanceDefaultCfg);
    if (ReturnValue != AMP_OK) {
        NETCTRL_ERR("[AppLib - NetControl] <InitNetCtrl> AmpNetCtrl_DataSvc_GetInstanceDefaultCfg() fail.(error code: %d)",ReturnValue);
        return -1;
    }

    ReturnValue = AmpNetCtrl_DataSvc_CreateInstance(&InstanceDefaultCfg, &DataHdlrInfo);
    if (ReturnValue != AMP_OK) {
        NETCTRL_ERR("[AppLib - NetControl] <InitNetCtrl> AmpNetCtrl_DataSvc_CreateInstance() fail.(error code: %d)",ReturnValue);
        return -1;
    }

    ReturnValue = AmpNetCtrl_DataSvc_RegRecvCb(&DataHdlrInfo, AppLibNetControl_RecvDataSvcStatus);
    if (ReturnValue != AMP_OK) {
        NETCTRL_ERR("[AppLib - NetControl] <InitNetCtrl> AmpNetCtrl_DataSvc_RegRecvCb() fail.(error code: %d)",ReturnValue);
        return -1;
    }

    gDataInstance[0].InstTypeId = DataHdlrInfo.InstTypeId;
    gDataInstance[0].Hdlr = DataHdlrInfo.Hdlr;

    return 0;
}

static int InitJsonUtility(void *pMemoryPool)
{
    AMP_JSON_INIT_CFG_s InitJsonCfg;
    int ReturnValue = 0;

    /* initialize AmpJson lib */
    AmpJson_GetInitDefaultCfg(&InitJsonCfg);
    InitJsonCfg.MemPoolAddr = pMemoryPool;
    ReturnValue = AmpJson_Init(&InitJsonCfg);
    if (ReturnValue != AMP_OK) {
        NETCTRL_ERR("[AppLib - NetControl] <InitJsonUtility> AmpJson_Init() fail.(error code: %d)",ReturnValue);
        return -1;
    }

    return 0;
}

static int InitLinuxCommand(void)
{
    int ReturnValue = 0;

    ReturnValue = AmpNetCtrl_LnxCmdInit();
    if (ReturnValue < 0) {
        NETCTRL_ERR("[AppLib - NetControl] <InitLinuxCommand> AmpNetCtrl_LnxCmdInit() fail");
        return -1;
    }

    return 0;
}


/**
 *  @brief Initialize net control module
 *
 *  @return 0 success, <0 failure
 */
int AppLibNetControl_Init(void *pMemoryPool)
{
    int ReturnValue = 0;

    if (gNetCtrlInited) {
        NETCTRL_DBG("[AppLib - NetControl] <Init> init already.");
        return ReturnValue;
    }

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&gJsonStringBuf, (void**)&gJsonStringBufRaw, JsonStringBufSize, 32);
    if (ReturnValue < 0) {
        NETCTRL_ERR("[AppLib - NetControl] <Init> Allocate buffer for json string fail");
        return -1;
    }

    /* init net control module */
    ReturnValue = InitNetCtrl(pMemoryPool);
    if (ReturnValue != 0) {
        return -1;
    }

    /* Create net control cmd handler message queue */
    memset(&gNetCtrl, 0, sizeof(NETCTRL_MGR_s));
    ReturnValue = AmbaKAL_MsgQueueCreate(&gNetCtrl.MsgQueue, gNetCtrl.MsgPool, sizeof(APP_NETCTRL_MESSAGE_s), NETCTRL_MGR_MSGQUEUE_SIZE);
    if (ReturnValue != OK) {
        NETCTRL_ERR("[AppLib - NetControl] <Init> Create Queue fail. (error code: %d)",ReturnValue);
        return ReturnValue;
    }

    /* Create net control cmd handler task */
    ReturnValue = AmbaKAL_TaskCreate(&gNetCtrl.Task, /* pTask */
        NETCTRL_MGR_NAME, /* pTaskName */
        APPLIB_NET_CTRL_TASK_PRIORITY, /* Priority */
        AppLibNetControl_MgrTask, /* void (*EntryFunction)(UINT32) */
        0x0, /* EntryArg */
        (void *) gNetCtrl.Stack, /* pStackBase */
        NETCTRL_MGR_STACK_SIZE, /* StackByteSize */
        AMBA_KAL_AUTO_START); /* Do NOT Start */

    if (ReturnValue != OK) {
        NETCTRL_ERR("[AppLib - NetControl] <Init> Create '%s; task fail (error code: %d)",NETCTRL_MGR_NAME, ReturnValue);
        return ReturnValue;
    }

    /* Create mutex */
    ReturnValue = AmbaKAL_MutexCreate(&gNetCtrl.MutexActiveSession);
    if (ReturnValue != OK) {
        NETCTRL_ERR("[AppLib - NetControl] <Init> Create Mutex MutexActiveSession fail = %d", ReturnValue);
        return ReturnValue;
    }

    /* Create mutex for command argument buffer*/
    ReturnValue = AmbaKAL_MutexCreate(&gNetCtrl.MutexArgBuf);
    if (ReturnValue != OK) {
        NETCTRL_ERR("[AppLib - NetControl] <Init> Create Mutex MutexArgBuf fail = %d", ReturnValue);
        return ReturnValue;
    }

    /* Create mutex for data server manager*/
    ReturnValue = AmbaKAL_MutexCreate(&gMutexDataSvcMgr);
    if (ReturnValue != OK) {
        NETCTRL_ERR("[AppLib - NetControl] <Init> Create Mutex gMutexDataSvcMgr fail = %d", ReturnValue);
        return ReturnValue;
    }

    if (AmbaKAL_EventFlagCreate(&gDataSvcEvent) != OK) {
        NETCTRL_ERR("[AppLib - NetControl] <Init> Create event flag for data svc fail");
        return -1;
    }

    ReturnValue = InitJsonUtility(pMemoryPool);
    if (ReturnValue != 0) {
        return -1;
    }

    ReturnValue = InitLinuxCommand();
    if (ReturnValue != 0) {
        return -1;
    }

    ResetActiveSession();
    gNetCtrlInited = 1;

    return ReturnValue;
}


