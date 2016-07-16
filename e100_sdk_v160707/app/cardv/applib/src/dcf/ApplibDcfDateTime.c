/**
 * @file src/app/connected/applib/src/dcf/ApplibDcfDateTime.c
 *
 * Implementation of DCF Applib
 *
 * History:
 *    2015/01/30 - [Evan Ji] created file
 *
 *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (¡°Software¡±) are protected by intellectual property rights
 * including, without limitation, U.S. and/or foreign copyrights.  This Software is also the
 * confidential and proprietary information of Ambarella, Inc. and its licensors.  You may
 * not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed license
 * agreement or nondisclosure agreement with Ambarella, Inc. or its authorized
 * affiliates.	In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-
 * INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR
 * MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <wctype.h>
#include <cfs/AmpCfs.h>
#include <dcf/AmpDcf.h>
#include <applib.h>
#include <dcf/ApplibDcf_FilterDateTime.h>
#include "ApplibDcf_NamingRuleDateTime.h"

//#define DEBUG_APPLIB_DCF_DATE_TIME
#if defined(DEBUG_APPLIB_DCF_DATE_TIME)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(MAGENTA,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

static APPLIB_DCF_CFG_s InitCFSConfig = {0};


static void PerrorImpl(UINT32 nLine, char *ErrMsg)
{
    char FileName[MAX_FILENAME_LENGTH];
    strncpy(FileName, (strrchr(__FILE__, '/') + 1), strlen(strrchr(__FILE__, '/') + 1));
    AmbaPrint("[Error]%s %u: %s", FileName, nLine, ErrMsg);
}

#define Perror(ErrMsg) {\
    PerrorImpl(__LINE__, ErrMsg);\
}

static int AppLibDCF_LastObjectImpl(APPLIB_DCF_HDLR_s *Hdlr);

/**
 * Node structure for storing internal object list
 */
typedef struct APPLIB_DCF_OBJECT_LIST_NODE_s_ {
    APPLIB_DCF_HDLR_s *Hdlr;
    UINT32 ObjId;
    AMP_DCF_FILE_s ObjName;
    struct APPLIB_DCF_OBJECT_LIST_NODE_s_ *Next;
    struct APPLIB_DCF_OBJECT_LIST_NODE_s_ *Prev;
} APPLIB_DCF_OBJECT_LIST_NODE_s;

/**
 * Global variable definition
 */
typedef struct APPLIB_DCF_s_ {
    AMBA_KAL_MUTEX_t ApplibMutex;                   /**< Mutex to prevent DCF applib reentry */
    AMBA_KAL_MUTEX_t Mutex;                         /**< Mutex for global variable protection */
    APPLIB_DCF_MEDIA_LIST_s MediaTypes;             /**< List of Media types */
    APPLIB_DCF_HDLR_s *Hdlrs;                       /**< DCF Handlers */
    APPLIB_DCF_OBJECT_LIST_NODE_s *ObjAvailList;    /**< Point to availiable internal objects */
    APPLIB_DCF_OBJECT_LIST_NODE_s *ObjUsedList;     /**< Point to allocated internal objects */
    int  MaxHdlr;                                   /**< Max number of handlers */
    BOOL Init;                                      /**< To indicate if Applib DCF has been initialized */
} APPLIB_DCF_s;
static APPLIB_DCF_s g_AppLibDcf = {0};

/**
 * Internal function
 */

/**
 *  Get media type of an object
 *  @param [in] Hdlr Applib DCF Handler
 *  @param [in] ObjId Object Id
 *  @return MediaType
 */
static UINT8 AppLibDCF_GetMediaTypeById(APPLIB_DCF_HDLR_s *Hdlr, UINT32 ObjId)
{
    int Rval = APPLIB_DCF_MEDIA_DCIM;
    DBGMSGc2(MAGENTA, "==[%s-%d] -START-", __FUNCTION__, __LINE__);
    if (ObjId > 0) {
        AMP_DCF_FILE_LIST_s *List = AmpDCF_GetFileList(Hdlr->DcfHdlr, ObjId);
        if (List != NULL) {
            char *pStr;
            pStr = strrchr(List->FileList[0].Name, '.');
            for (int i=0;i<g_AppLibDcf.MediaTypes.Count;i++) {
                if (strcmp(g_AppLibDcf.MediaTypes.List[i].ExtName, pStr+1) == 0) {
                    Rval = i;
                }
            }
            AmpDCF_RelFileList(Hdlr->DcfHdlr, List);
        }
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] -END- Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Get initial value of object amount
 *  @param [in] Hdlr Applib DCF Handler
 */
static void AppLibDcf_InitObjectAmount(APPLIB_DCF_HDLR_s *Hdlr)
{
    int i, ObjId;
    UINT8 MediaId;
    DBGMSGc2(MAGENTA, "==[%s-%d] -START-", __FUNCTION__, __LINE__);
    for (i=0;i<=g_AppLibDcf.MediaTypes.Count;i++) {
        Hdlr->ObjAmount[i] = 0;
    }
    ObjId = AmpDCF_GetLastId(Hdlr->DcfHdlr);
    while (ObjId > 0) {
        Hdlr->ObjAmount[APPLIB_DCF_MEDIA_DCIM]++;
        MediaId = AppLibDCF_GetMediaTypeById(Hdlr, ObjId);
        if (MediaId < APPLIB_DCF_MEDIA_DCIM) {
            Hdlr->ObjAmount[MediaId]++;
        }
        ObjId = AmpDCF_GetPrevId(Hdlr->DcfHdlr);
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
}

/**
 *  Increase object amount
 *  @param [in] Hdlr Applib DCF Handler
 *  @param [in] MediaType media type
 */
static void AppLibDcf_IncObjectAmount(APPLIB_DCF_HDLR_s *Hdlr, UINT8 MediaType)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    Hdlr->ObjAmount[APPLIB_DCF_MEDIA_DCIM]++;
    if (MediaType < APPLIB_DCF_MEDIA_DCIM) {
        Hdlr->ObjAmount[MediaType]++;
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
}

/**
 *  Decrease object amount
 *  @param [in] Hdlr Applib DCF Handler
 *  @param [in] MediaType media type
 */
static void AppLibDcf_DecObjectAmount(APPLIB_DCF_HDLR_s *Hdlr, UINT8 MediaType)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    Hdlr->ObjAmount[APPLIB_DCF_MEDIA_DCIM]--;
    if (MediaType < APPLIB_DCF_MEDIA_DCIM) {
        Hdlr->ObjAmount[MediaType]--;
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
}

/**
 * Internal object handling functions
 */

/**
 *  Add an internal object
 *  @param [in] Hdlr Applib DCF Handler
 *  @param [in] ObjId Object Id
 *  @param [in] ObjName Object Name
 */
static int AppLibDCF_AddIntObj(APPLIB_DCF_HDLR_s *Hdlr, UINT32 ObjId, char *ObjName)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (g_AppLibDcf.ObjAvailList != NULL) {
        APPLIB_DCF_OBJECT_LIST_NODE_s *Obj;
        /**< Get available internal object */
        Obj = g_AppLibDcf.ObjAvailList;
        g_AppLibDcf.ObjAvailList = Obj->Next;
        /**< Add object to used list*/
        if (g_AppLibDcf.ObjUsedList == NULL) {
            Obj->Prev = Obj;
            Obj->Next = Obj;
        } else {
            Obj->Prev = g_AppLibDcf.ObjUsedList->Prev;
            Obj->Next = g_AppLibDcf.ObjUsedList;
            Obj->Prev->Next = Obj;
            Obj->Next->Prev = Obj;
        }
        g_AppLibDcf.ObjUsedList = Obj;
        /**< Copy data to object */
        Obj->Hdlr = Hdlr;
        Obj->ObjId = ObjId;
        strncpy(Obj->ObjName.Name, ObjName, MAX_FILENAME_LENGTH);
        Obj->ObjName.Name[MAX_FILENAME_LENGTH-1] = '\0';
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
        return 0;
    } else Perror("Internal object not available");
    return -1;
}

/**
 *  Release an internal object
 *  @param [in] Obj address of an internal object
 */
static void AppLibDcf_ReleaseIntObj(APPLIB_DCF_OBJECT_LIST_NODE_s *Obj)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    /**< Remove object from used list */
    if (Obj->Prev == Obj) {
        g_AppLibDcf.ObjUsedList = NULL;
    } else {
        Obj->Prev->Next = Obj->Next;
        Obj->Next->Prev = Obj->Prev;
        if (Obj == g_AppLibDcf.ObjUsedList) {
            g_AppLibDcf.ObjUsedList = Obj->Next;
        }
    }
    /**< Add object to available list */
    Obj->Next = g_AppLibDcf.ObjAvailList;
    g_AppLibDcf.ObjAvailList = Obj;
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
}

/**
 *  Remove an internal object by object id
 *  @param [in] Hdlr Applib DCF Handler
 *  @param [in] ObjId Object Id
 */
static void AppLibDcf_RemoveIntObjById(APPLIB_DCF_HDLR_s *Hdlr, UINT32 ObjId)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (g_AppLibDcf.ObjUsedList != NULL) {
        APPLIB_DCF_OBJECT_LIST_NODE_s *pHead, *Obj, *Prev;
        pHead = g_AppLibDcf.ObjUsedList;
        Obj = g_AppLibDcf.ObjUsedList->Prev;
        while (Obj != pHead) {
            Prev = Obj->Prev;
            if ((Obj->Hdlr == Hdlr)&&(Obj->ObjId == ObjId)) {
                AppLibDcf_ReleaseIntObj(Obj);
            }
            Obj = Prev;
        }
        if ((Obj->Hdlr == Hdlr)&&(Obj->ObjId == ObjId)) {
            AppLibDcf_ReleaseIntObj(Obj);
        }
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
}

/**
 *  Remove an internal object by object name
 *  @param [in] ObjName Object Name
 */
static void AppLibDcf_RemoveIntObjByName(char *Name)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (g_AppLibDcf.ObjUsedList != NULL) {
        APPLIB_DCF_OBJECT_LIST_NODE_s *pHead, *Obj;
        pHead = g_AppLibDcf.ObjUsedList;
        Obj = g_AppLibDcf.ObjUsedList->Prev;
        while (Obj != pHead) {
            if (strcmp(Obj->ObjName.Name, Name) == 0) {
                AppLibDcf_ReleaseIntObj(Obj);
                return;
            }
            Obj = Obj->Prev;
        }
        if (strcmp(Obj->ObjName.Name, Name) == 0) {
            AppLibDcf_ReleaseIntObj(Obj);
        }
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
}

/**
 *  Remove internal objects by handler
 *  @param [in] Hdlr Applib DCF Handler
 */
static void AppLibDcf_RemoveIntObjByHdlr(APPLIB_DCF_HDLR_s *Hdlr)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (g_AppLibDcf.ObjUsedList != NULL) {
        APPLIB_DCF_OBJECT_LIST_NODE_s *pHead, *Obj, *Prev;
        pHead = g_AppLibDcf.ObjUsedList;
        Obj = g_AppLibDcf.ObjUsedList->Prev;
        while (Obj != pHead) {
            Prev = Obj->Prev;
            if (Obj->Hdlr == Hdlr) {
                AppLibDcf_ReleaseIntObj(Obj);
            }
            Obj = Prev;
        }
        if (Obj->Hdlr == Hdlr) {
            AppLibDcf_ReleaseIntObj(Obj);
        }
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
}

/**
 * CFS Callback function and related functions
 */

/**
 *  Get the Applib DCF handler of a filename
 *  @param [in] Name filename
 *  return Applib DCF handler
 */
static APPLIB_DCF_HDLR_s *CfsCB_NameToHandler(char *Name)
{
     int i, j, Len;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
     for (i=0;i<g_AppLibDcf.MaxHdlr;i++) {
         if ((g_AppLibDcf.Hdlrs[i].DcfHdlr != NULL) && (g_AppLibDcf.Hdlrs[i].Active == TRUE)) {
             for (j=0;j<g_AppLibDcf.Hdlrs[i].RootList.Count;j++) {
                 Len = strlen(g_AppLibDcf.Hdlrs[i].RootList.List[j].Name);
                 if (strncmp(g_AppLibDcf.Hdlrs[i].RootList.List[j].Name, Name, Len) == 0) {
                    DBGMSGc2(MAGENTA, "==[%s-%d] Name:%s", __FUNCTION__, __LINE__, Name);
                    return &g_AppLibDcf.Hdlrs[i];
                 }
             }
         }
     }
     return NULL;
}

/**
 *  Add a file to DCF
 *  @param [in] Name filename
 *  return >=0 success, <0 failure
 */
static int CfsCB_AddFileImpl(char *Name)
{
    APPLIB_DCF_HDLR_s *Hdlr = CfsCB_NameToHandler(Name);
    int Rval = AMP_OK;
    DBGMSGc2(MAGENTA, "==[%s-%d] Name:%s", __FUNCTION__, __LINE__, Name);
    if (Hdlr != NULL) {
        UINT32 ObjId;
        K_ASSERT(Hdlr->DcfHdlr->Filter != NULL);
        ObjId = Hdlr->DcfHdlr->Filter->NameToId(Name);
        if (ObjId > 0) {
            if (ObjId <= Hdlr->LastInternalObjId) {
                /**< Check if the Object Id is already in DCF before add file */
                BOOL bMajorObj = AmpDCF_CheckIdValid(Hdlr->DcfHdlr, ObjId);
                if (AmpDCF_AddFile(Hdlr->DcfHdlr, Name) == AMP_OK) {
                    if (bMajorObj == FALSE) { /**< New object is added to DCF */
                        UINT8 MediaType = AppLibDCF_GetMediaTypeById(Hdlr, ObjId);
                        AppLibDcf_IncObjectAmount(Hdlr, MediaType); /**< Increase object amount */
                    }
                    /**< Remove internal object */
                    AppLibDcf_RemoveIntObjByName(Name);
                }
            } else {
                Rval = AMP_ERROR_GENERAL_ERROR;
            }
        }
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}
static int CfsCB_AddFile(char *Name)
{
    int Rval = AMP_ERROR_GENERAL_ERROR;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Name != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        Rval = CfsCB_AddFileImpl(Name);
        AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
    } else Perror("Get mutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Remove a file from DCF
 *  @param [in] Name filename
 *  return >=0 success, <0 failure
 */
static void CfsCB_RemoveFileImpl(char *Name)
{
    APPLIB_DCF_HDLR_s *Hdlr = CfsCB_NameToHandler(Name);
    DBGMSGc2(MAGENTA, "==[%s-%d] Name:%s", __FUNCTION__, __LINE__, Name);
    if (Hdlr != NULL) {
        UINT32 ObjId;
        K_ASSERT(Hdlr->DcfHdlr->Filter != NULL);
        ObjId = Hdlr->DcfHdlr->Filter->NameToId(Name);
        if (ObjId > 0) {
            if (AmpDCF_CheckIdValid(Hdlr->DcfHdlr,ObjId) == TRUE) {
                UINT8 MediaType = AppLibDCF_GetMediaTypeById(Hdlr, ObjId);
                if (AmpDCF_RemoveFile(Hdlr->DcfHdlr, Name) == AMP_OK) {
                    AppLibStorageDmf_MinusFileCountInTheDir(Name);
                    if (AmpDCF_CheckIdValid(Hdlr->DcfHdlr, ObjId) == FALSE) {
                        AppLibDcf_DecObjectAmount(Hdlr, MediaType);
                        if (Hdlr->NumberMode == APPLIB_DCF_NUMBER_RESET) {
                            if (Hdlr->LastInternalObjId == ObjId) {
                                Hdlr->LastInternalObjId = AmpDCF_GetLastId(Hdlr->DcfHdlr);
                                if (Hdlr->BrowseMode != APPLIB_DCF_MEDIA_DCIM) {
                                    AppLibDCF_LastObjectImpl(Hdlr);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
}
static int CfsCB_RemoveFile(char *Name)
{
    int Rval = AMP_ERROR_GENERAL_ERROR;
    DBGMSGc2(MAGENTA, "==[%s-%d] Name:%s", __FUNCTION__, __LINE__, Name);
    K_ASSERT(Name != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        CfsCB_RemoveFileImpl(Name);
        AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
    } else Perror("Get mutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Remove a directory from DCF
 *  @param [in] Name directory name
 */
static void CfsCB_RemoveDirectoryImpl(char *Name)
{
    APPLIB_DCF_HDLR_s *Hdlr = CfsCB_NameToHandler(Name);
    DBGMSGc2(MAGENTA, "==[%s-%d] Name:%s", __FUNCTION__, __LINE__, Name);
    if (Hdlr != NULL) {
        if (Hdlr->DcfHdlr != NULL) {
            AmpDCF_RemoveDirectory(Hdlr->DcfHdlr, Name);
        }
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
}
static int CfsCB_RemoveDirectory(char *Name)
{
    int Rval = AMP_ERROR_GENERAL_ERROR;
    DBGMSGc2(MAGENTA, "==[%s-%d] Name:%s", __FUNCTION__, __LINE__, Name);
    K_ASSERT(Name != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        CfsCB_RemoveDirectoryImpl(Name);
        AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
    } else Perror("Get mutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Add a directory to DCF
 *  @param [in] Name directory name
 */
static void CfsCB_AddDirectoryImpl(char *Name)
{
    APPLIB_DCF_HDLR_s *Hdlr = CfsCB_NameToHandler(Name);
    DBGMSGc2(MAGENTA, "==[%s-%d] Name:%s", __FUNCTION__, __LINE__, Name);
    if (Hdlr != NULL) {
        if (Hdlr->DcfHdlr != NULL) {
            AmpDCF_AddDirectory(Hdlr->DcfHdlr, Name);
        }
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
}
static int CfsCB_AddDirectory(char *Name)
{
    int Rval = AMP_ERROR_GENERAL_ERROR;
    DBGMSGc2(MAGENTA, "==[%s-%d] Name:%s", __FUNCTION__, __LINE__, Name);
    K_ASSERT(Name != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        CfsCB_AddDirectoryImpl(Name);
        AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
    } else Perror("Get mutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}
#ifdef CONFIG_APP_ARD
static int OpenedFiles = 0;
#endif

static int CfsCB_ReturnFileClose(char *Name)
{
    int Rval = AMP_ERROR_GENERAL_ERROR;
    APPLIB_DCF_HDLR_s *Hdlr = CfsCB_NameToHandler(Name);
    DBGMSGc2(MAGENTA, "==[%s-%d] Name:%s", __FUNCTION__, __LINE__, Name);
    K_ASSERT(Name != NULL);
    if  (Hdlr != NULL) {
        Rval = AppLibDCF_NameToId(Hdlr,Name);
#ifdef CONFIG_APP_ARD
        if(OpenedFiles > 0) {
            OpenedFiles--;
        }
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_DCF_FILE_CLOSE, Rval, OpenedFiles);
#else
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_DCF_FILE_CLOSE, Rval, 0);
#endif
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}
#ifdef CONFIG_APP_ARD
int AppLibDCF_GetOpenedFiles(VOID) {
    return OpenedFiles;
}
#endif

/**
 *  Call back function of CFS
 *  @param [in] opCode operation code
 *  @param [in] param parameter
 *  @return >=0 success, <0 failure
 */
static int AppLibDcf_CfsCB(int opCode, UINT32 param)
{
    int Rval = AMP_OK;
    DBGMSGc2(MAGENTA, "==[%s-%d] opCode:%d, param:%d", __FUNCTION__, __LINE__, opCode, param);
    /**< Can't waste too much time in this function. */
    switch (opCode) {
    case AMP_CFS_EVENT_FOPEN:
        {
            AMP_CFS_OP_TYPE3_s *pm3 = (AMP_CFS_OP_TYPE3_s *)param;
            if (pm3->Mode >= AMP_CFS_FILE_MODE_WRITE_ONLY) {
                Rval = CfsCB_AddFile((char *) pm3->File);
#ifdef CONFIG_APP_ARD
                if(AMP_OK == Rval) {
                    OpenedFiles++;
                }
#endif
            }
            break;
        }
    case AMP_CFS_EVENT_CREATE:
        {
            AMP_CFS_OP_TYPE1_s *pm1 = (AMP_CFS_OP_TYPE1_s *)param;
            CfsCB_AddFile((char *) pm1->File);
            break;
        }
    case AMP_CFS_EVENT_REMOVE:
        {
            AMP_CFS_OP_TYPE1_s *pm1 = (AMP_CFS_OP_TYPE1_s *)param;
            CfsCB_RemoveFile((char *) pm1->File);
            break;
        }
    case AMP_CFS_EVENT_UPDATE:
        {
            break;
        }
    case AMP_CFS_EVENT_RMDIR:
        {
            AMP_CFS_OP_TYPE1_s *pm1 = (AMP_CFS_OP_TYPE1_s *)param;
            CfsCB_RemoveDirectory((char *) pm1->File);
            break;
        }
    case AMP_CFS_EVENT_MKDIR:
        {
            AMP_CFS_OP_TYPE1_s *pm1 = (AMP_CFS_OP_TYPE1_s *)param;
            CfsCB_AddDirectory((char *) pm1->File);
            break;
        }
    case AMP_CFS_EVENT_FORMAT:
        {
            break;
        }
    case AMP_CFS_EVENT_LOWSPEED:
        {
            break;
        }
    case AMP_CFS_EVENT_FCLOSE:
        {
            AMP_CFS_OP_TYPE1_s *pm1 = (AMP_CFS_OP_TYPE1_s *)param;
            CfsCB_ReturnFileClose((char *) pm1->File);
            break;
        }
    default:
        break;
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 * DCF Applib APIs
 */

/**
 *  Get required buffer size for Applib DCF.
 *  @param [in] CfsCfg address of AppLibDCF config data
 *  @param [in] DcfInitCfg address of AppLibDCF init config data
 *  @param [in] IntObjAmount Internal object amount
 *  @return buffer size
 */
UINT32 AppLibDCF_GetRequiredBufSize(AMP_CFS_CFG_s *CfsCfg, AMP_DCF_INIT_CFG_s *DcfInitCfg, UINT32 IntObjAmount)
{
    UINT32 BufferSize;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(CfsCfg != NULL);
    K_ASSERT(DcfInitCfg != NULL);
    K_ASSERT(IntObjAmount > 0);

    DcfInitCfg->BufferSize  = AmpDCF_GetRequiredBufferSize(DcfInitCfg->MaxHdlr, \
    DcfInitCfg->TaskInfo.StackSize, DcfInitCfg->MaxDirPerDnum, DcfInitCfg->MaxFilePerId, DcfInitCfg->MaxPendingOp,\
    DcfInitCfg->EnableDefTbl, DcfInitCfg->DefTblCfg.MaxHdlr, DcfInitCfg->DefTblCfg.MaxDir, DcfInitCfg->DefTblCfg.MaxFile);

    BufferSize  = CfsCfg->BufferSize + DcfInitCfg->BufferSize;
    BufferSize += DcfInitCfg->MaxHdlr * sizeof(APPLIB_DCF_HDLR_s);
    BufferSize += IntObjAmount * sizeof(APPLIB_DCF_OBJECT_LIST_NODE_s);
    DBGMSGc2(MAGENTA, "==[%s-%d] BufferSize:%d", __FUNCTION__, __LINE__, BufferSize);
    return BufferSize;
}

/**
 *  Get default config for initialize AppLib DCF.
 *  @param [out] InitConfig address of AppLibDCF config data
 */
void AppLibDCF_GetDefaultInitCfg(APPLIB_DCF_INIT_CFG_s *InitConfig)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(InitConfig != NULL);

    AmpCFS_GetDefaultCfg(&InitConfig->CfsCfg);
#ifdef CONFIG_APP_ARD
    InitConfig->CfsCfg.SchBankSize= 4*1024*1024;
    InitConfig->CfsCfg.SchBankAmount = 6;
#else
    InitConfig->CfsCfg.SchBankSize= 512<<10;
    InitConfig->CfsCfg.SchBankAmount = 12;
#endif
    InitConfig->CfsCfg.BufferSize= AmpCFS_GetRequiredBufferSize(InitConfig->CfsCfg.SchBankSize, \
                            InitConfig->CfsCfg.SchBankAmount, InitConfig->CfsCfg.TaskInfo.StackSize,  InitConfig->CfsCfg.SchTaskAmount,  InitConfig->CfsCfg.CacheEnable, \
                            InitConfig->CfsCfg.CacheMaxFileNum);

    AmpDCF_GetInitDefaultCfg(&InitConfig->DcfInitCfg);
//    InitConfig->CfsCfg.MaxStreamNum = 8;
    InitConfig->MediaTypes.Count = 3; /**< APPLIB_DCF_MEDIA_TYPE_AMOUNT */
    InitConfig->MediaTypes.List[0].MediaType = APPLIB_DCF_MEDIA_IMAGE;
    strcpy(InitConfig->MediaTypes.List[0].ExtName, APPLIB_DCF_MEDIA_IMAGE_EXT_NAME);
    InitConfig->MediaTypes.List[1].MediaType = APPLIB_DCF_MEDIA_AUDIO;
    strcpy(InitConfig->MediaTypes.List[1].ExtName, APPLIB_DCF_MEDIA_AUDIO_EXT_NAME);
    InitConfig->MediaTypes.List[2].MediaType = APPLIB_DCF_MEDIA_VIDEO;
    strcpy(InitConfig->MediaTypes.List[2].ExtName, APPLIB_DCF_MEDIA_VIDEO_EXT_NAME);

    InitConfig->IntObjAmount = APPLIB_DCF_INTERNAL_OBJECT_AMOUNT;
    InitConfig->BufferSize = AppLibDCF_GetRequiredBufSize(&InitConfig->CfsCfg, &InitConfig->DcfInitCfg, InitConfig->IntObjAmount);
    InitConfig->Buffer = NULL;
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
}

/**
 *  Implementation of AppLib DCF initialization
 *  @param [in] Buffer buffer address
 *  @param [in] BufferSize buffer size
 *  @param [in] CfsCfg address of CFS config data
 *  @param [in] DcfInitCfg address of DCF init config data
 *  @param [in] MediaTypes address of available Media Types
 *  @param [in] IntObjAmount Internal object amount
 *  @return >=0 success, <0 failure
 */
static int AppLibDCF_InitImpl(UINT8 *Buffer, UINT32 BufferSize, AMP_CFS_CFG_s *CfsCfg, AMP_DCF_INIT_CFG_s *DcfInitCfg, APPLIB_DCF_MEDIA_LIST_s *MediaTypes, UINT32 IntObjAmount)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Buffer != NULL);
    K_ASSERT(BufferSize == AppLibDCF_GetRequiredBufSize(CfsCfg, DcfInitCfg, IntObjAmount));
    K_ASSERT(CfsCfg != NULL);
    K_ASSERT(DcfInitCfg != NULL);
    K_ASSERT(MediaTypes != NULL);
    K_ASSERT(MediaTypes->Count != 0);
    K_ASSERT(IntObjAmount > 0);
    /**< Initialize CFS */
    CfsCfg->Buffer = Buffer;
    Buffer += CfsCfg->BufferSize;
    CfsCfg->FileOperation = AppLibDcf_CfsCB;
    memcpy(&g_AppLibDcf.MediaTypes, MediaTypes, sizeof(APPLIB_DCF_MEDIA_LIST_s));
    if (AmpCFS_Init(CfsCfg) == AMP_OK) {
        /**< Initialize DCF */
        DcfInitCfg->Buffer = (void *) Buffer;
        Buffer += DcfInitCfg->BufferSize;
        if (AmpDCF_Init(DcfInitCfg) == AMP_OK) {
            int HdlrSize = DcfInitCfg->MaxHdlr * sizeof(APPLIB_DCF_HDLR_s);
            int i;
            /**< Initialize Applib_DCF Handlers */
            g_AppLibDcf.MaxHdlr = DcfInitCfg->MaxHdlr;
            g_AppLibDcf.Hdlrs = (APPLIB_DCF_HDLR_s *) Buffer;
            Buffer += HdlrSize;
            memset((void *)g_AppLibDcf.Hdlrs, 0, HdlrSize);
            /**< Initialize Applib_DCF Object list */
            g_AppLibDcf.ObjAvailList = (APPLIB_DCF_OBJECT_LIST_NODE_s *) Buffer;
            for (i=0;i<IntObjAmount-1;i++) {
                g_AppLibDcf.ObjAvailList[i].Next = &g_AppLibDcf.ObjAvailList[i+1];
            }
            g_AppLibDcf.ObjAvailList[i].Next = NULL;
            g_AppLibDcf.ObjUsedList = NULL;
            DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
            return 0;
        } else Perror("DCF Init Fail!");
    } else Perror("CFS Init Fail!");
    return -1;
}

/**
 *  Initialize AppLib DCF
 *  @param [in] InitConfig address of a Applib DCF init config data
 *  @return >=0 success, <0 failure
 */
int AppLibDCF_Init(APPLIB_DCF_INIT_CFG_s *InitConfig)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(InitConfig != NULL);
    if (g_AppLibDcf.Init == FALSE) {
        if (AmbaKAL_MutexCreate(&g_AppLibDcf.ApplibMutex) == OK) {
            if (AmbaKAL_MutexCreate(&g_AppLibDcf.Mutex) == OK) {
                if (AppLibDCF_InitImpl(InitConfig->Buffer, InitConfig->BufferSize, &InitConfig->CfsCfg, &InitConfig->DcfInitCfg, &InitConfig->MediaTypes, InitConfig->IntObjAmount) == 0) {
                    g_AppLibDcf.Init = TRUE;
                    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
                    return 0;
                }
                Perror("AppLib DCF init Fail!");
                AmbaKAL_MutexDelete(&g_AppLibDcf.Mutex);
            } else Perror("Create Mutex Fail!");
            AmbaKAL_MutexDelete(&g_AppLibDcf.ApplibMutex);
        } else Perror("Create ApplibMutex Fail!");
    } else Perror("AppLib DCF has been initialized!");
    return -1;
}


/**
 *  Set default config data for creating a DCF handler
 *  @param [in] Config address of default DCF config data
 */
void AppLibDCF_SetDefaultCfg(APPLIB_DCF_CFG_s *Config)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Config != NULL);
    InitCFSConfig.BrowseMode = Config->BrowseMode;
    InitCFSConfig.NumberMode = Config->NumberMode;
    InitCFSConfig.PhotoLastIdx = Config->PhotoLastIdx;
    InitCFSConfig.VideoLastIdx = Config->VideoLastIdx;
    InitCFSConfig.SoundLastIdx = Config->SoundLastIdx;
    InitCFSConfig.MixLastIdx = Config->MixLastIdx;
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
}

/**
 *  Get default config data for creating a DCF handler
 *  @param [in] Config address of DCF config data
 */
void AppLibDCF_GetDefaultCfg(APPLIB_DCF_CFG_s *Config)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Config != NULL);
    Config->NamingRule = 1;
    Config->BrowseMode = InitCFSConfig.BrowseMode;
    Config->NumberMode = InitCFSConfig.NumberMode;
    Config->RootList.Count = 1;
    strcpy(Config->RootList.List[0].Name, "C:\\DCIM");
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
}

/**
 *  Create a DCF handler
 *  @param [in] Config address of config data
 *  @return !=NULL Handler, == NULL failure
 */
static APPLIB_DCF_HDLR_s *AppLibDCF_CreateImpl(APPLIB_DCF_CFG_s *Config)
{
    APPLIB_DCF_HDLR_s *Hdlr = NULL;
    int i;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    for (i=0;i<g_AppLibDcf.MaxHdlr;i++) {
        if (g_AppLibDcf.Hdlrs[i].DcfHdlr == NULL) {
            Hdlr = &g_AppLibDcf.Hdlrs[i];
        }
    }
    if (Hdlr != NULL) {
        AMP_DCF_CFG_s DefCfg;
        if (AmpDCF_GetDefaultCfg(&DefCfg) == AMP_OK) {
            Hdlr->BrowseMode = Config->BrowseMode;
            Hdlr->NumberMode = Config->NumberMode;
            Hdlr->NamingRule = Config->NamingRule;
            Hdlr->CurObjId   = 0;
            Hdlr->CurDnum    = 0;
            if (Config->NamingRule == 1) {
                DefCfg.Filter = &g_AppLibDcfFlt1;
                Hdlr->Naming = &g_AppLibDcfNaming1;
                if (AmpDCF_Create(&DefCfg, &Hdlr->DcfHdlr) == AMP_OK) {
                    memcpy(&Hdlr->RootList, &Config->RootList, sizeof(APPLIB_DCF_ROOT_LIST_s));
                    Hdlr->Active = FALSE;
                    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
                    return Hdlr;
                } else Perror("Create DCF handler error!");
            } else Perror("Not supported naming rule!");
        } else Perror("Get DCF handler default config error!");
    } else Perror("Get Free Applib DCF handler error!");
    return NULL;
}
APPLIB_DCF_HDLR_s *AppLibDCF_Create(APPLIB_DCF_CFG_s *Config)
{
    APPLIB_DCF_HDLR_s *Hdlr = NULL;
    K_ASSERT(Config != NULL);
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Hdlr = AppLibDCF_CreateImpl(Config);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
    return Hdlr;
}

/**
 *  Goto first Object and return Object id
 *  @param [in] Hdlr Applib DCF Handler
 *  @return >=0 object id, <0 failure
 */
static int AppLibDCF_FirstObjectImpl(APPLIB_DCF_HDLR_s *Hdlr)
{
    int ObjId;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    ObjId = AmpDCF_GetFirstId(Hdlr->DcfHdlr);
    if (Hdlr->BrowseMode == APPLIB_DCF_MEDIA_DCIM) {
        if (ObjId > 0) {
            Hdlr->CurObjId = ObjId;
            DBGMSGc2(MAGENTA, "==[%s-%d] CurObjId;%d", __FUNCTION__, __LINE__, Hdlr->CurObjId);
        }
    } else {
        while (ObjId > 0) {
            if (AppLibDCF_GetMediaTypeById(Hdlr, ObjId) == Hdlr->BrowseMode) {
                Hdlr->CurObjId = ObjId;
                DBGMSGc2(MAGENTA, "==[%s-%d] CurObjId;%d", __FUNCTION__, __LINE__, Hdlr->CurObjId);
                break;
            }
            ObjId = AmpDCF_GetNextId(Hdlr->DcfHdlr);
        }
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] ObjId;%d", __FUNCTION__, __LINE__, ObjId);
    return ObjId;
}
int AppLibDCF_FirstObject(APPLIB_DCF_HDLR_s *Hdlr)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_FirstObjectImpl(Hdlr);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Goto last Object and return Object id
 *  @param [in] Hdlr Applib DCF Handler
 *  @return >=0 object id, <0 failure
 */
static int AppLibDCF_LastObjectImpl(APPLIB_DCF_HDLR_s *Hdlr)
{
    int ObjId;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    ObjId = AmpDCF_GetLastId(Hdlr->DcfHdlr);
    if (Hdlr->BrowseMode == APPLIB_DCF_MEDIA_DCIM) {
        if (ObjId > 0) {
            Hdlr->CurObjId = ObjId;
            DBGMSGc2(MAGENTA, "==[%s-%d] CurObjId;%d", __FUNCTION__, __LINE__, Hdlr->CurObjId);
        }
    } else {
        while (ObjId > 0) {
            if (AppLibDCF_GetMediaTypeById(Hdlr, ObjId) == Hdlr->BrowseMode) {
                Hdlr->CurObjId = ObjId;
                DBGMSGc2(MAGENTA, "==[%s-%d] CurObjId;%d", __FUNCTION__, __LINE__, Hdlr->CurObjId);
                break;
            }
            ObjId = AmpDCF_GetPrevId(Hdlr->DcfHdlr);
        }
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] ObjId;%d", __FUNCTION__, __LINE__, ObjId);
    return ObjId;
}
int AppLibDCF_LastObject(APPLIB_DCF_HDLR_s *Hdlr)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_LastObjectImpl(Hdlr);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Goto next Object and return Object id
 *  @param [in] Hdlr Applib DCF Handler
 *  @return >=0 object id, <0 failure
 */
static int AppLibDCF_NextObjectImpl(APPLIB_DCF_HDLR_s *Hdlr)
{
    int ObjId;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    ObjId = AmpDCF_GetNextId(Hdlr->DcfHdlr);
    if (Hdlr->BrowseMode == APPLIB_DCF_MEDIA_DCIM) {
        if (ObjId > 0) {
            Hdlr->CurObjId = ObjId;
            DBGMSGc2(MAGENTA, "==[%s-%d] CurObjId;%d", __FUNCTION__, __LINE__, Hdlr->CurObjId);
        }
    } else {
        while (ObjId > 0) {
            if (AppLibDCF_GetMediaTypeById(Hdlr, ObjId) == Hdlr->BrowseMode) {
                Hdlr->CurObjId = ObjId;
                DBGMSGc2(MAGENTA, "==[%s-%d] CurObjId;%d", __FUNCTION__, __LINE__, Hdlr->CurObjId);
                break;
            }
            ObjId = AmpDCF_GetNextId(Hdlr->DcfHdlr);
        }
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] ObjId;%d", __FUNCTION__, __LINE__, ObjId);
    return ObjId;
}
int AppLibDCF_NextObject(APPLIB_DCF_HDLR_s *Hdlr)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_NextObjectImpl(Hdlr);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Goto previous Object and return Object id
 *  @param [in] Hdlr Applib DCF Handler
 *  @return >=0 object id, <0 failure
 */
static int AppLibDCF_PrevObjectImpl(APPLIB_DCF_HDLR_s *Hdlr)
{
    int ObjId;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    ObjId = AmpDCF_GetPrevId(Hdlr->DcfHdlr);
    if (Hdlr->BrowseMode == APPLIB_DCF_MEDIA_DCIM) {
        if (ObjId > 0) {
            Hdlr->CurObjId = ObjId;
            DBGMSGc2(MAGENTA, "==[%s-%d] CurObjId;%d", __FUNCTION__, __LINE__, Hdlr->CurObjId);
        }
    } else {
        while (ObjId > 0) {
            if (AppLibDCF_GetMediaTypeById(Hdlr, ObjId) == Hdlr->BrowseMode) {
                Hdlr->CurObjId = ObjId;
                DBGMSGc2(MAGENTA, "==[%s-%d] CurObjId;%d", __FUNCTION__, __LINE__, Hdlr->CurObjId);
                break;
            }
            ObjId = AmpDCF_GetPrevId(Hdlr->DcfHdlr);
        }
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] ObjId:%d", __FUNCTION__, __LINE__, ObjId);
    return ObjId;
}
int AppLibDCF_PrevObject(APPLIB_DCF_HDLR_s *Hdlr)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_PrevObjectImpl(Hdlr);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Get current object Id
 *  @param [in] Hdlr Applib DCF Handler
 *  return object Id
 */
static UINT32 AppLibDCF_GetCurrentObjectIdImpl(APPLIB_DCF_HDLR_s *Hdlr)
{
    return Hdlr->CurObjId;
}
UINT32 AppLibDCF_GetCurrentObjectId(APPLIB_DCF_HDLR_s *Hdlr)
{
    UINT32 ObjId = 0;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            ObjId = AppLibDCF_GetCurrentObjectIdImpl(Hdlr);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] ObjId:%d", __FUNCTION__, __LINE__, ObjId);
    return ObjId;
}

/**
 *  Get file list of current Object
 *  @param [in] Hdlr Applib DCF Handler
 *  @param [in] ObjId Object Id
 *  @return address of file list
 */
static AMP_DCF_FILE_LIST_s *AppLibDCF_GetFileListImpl(APPLIB_DCF_HDLR_s *Hdlr, UINT32 ObjId)
{
    DBGMSGc2(MAGENTA, "\n==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    return AmpDCF_GetFileList(Hdlr->DcfHdlr, ObjId);
}
AMP_DCF_FILE_LIST_s *AppLibDCF_GetFileList(APPLIB_DCF_HDLR_s *Hdlr, UINT32 ObjId)
{
    AMP_DCF_FILE_LIST_s *List = NULL;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    K_ASSERT(ObjId > 0);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            List = AppLibDCF_GetFileListImpl(Hdlr, ObjId);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
    return List;
}

/**
 *  Release file list
 *  @param [in] Hdlr Applib DCF Handler
 *  @param [in] List address of file list
 *  @return >=0 success, <0 failure
 */
static int AppLibDCF_RelFileListImpl(APPLIB_DCF_HDLR_s *Hdlr, AMP_DCF_FILE_LIST_s *List)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    if (AmpDCF_RelFileList(Hdlr->DcfHdlr, List) == AMP_OK) {
        DBGMSGc2(MAGENTA, "==[%s-%d] rtn 0", __FUNCTION__, __LINE__);
        return 0;
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] rtn -1", __FUNCTION__, __LINE__);
    return -1;
}
int AppLibDCF_RelFileList(APPLIB_DCF_HDLR_s *Hdlr, AMP_DCF_FILE_LIST_s *List)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    K_ASSERT(List != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_RelFileListImpl(Hdlr, List);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Goto first directory and return dnum
 *  @param [in] Hdlr Applib DCF Handler
 *  @return >=0 dir number, <0 failure
 */
static int AppLibDCF_FirstDirImpl(APPLIB_DCF_HDLR_s *Hdlr)
{
    int dnum;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    dnum = AmpDCF_GetFirstDnum(Hdlr->DcfHdlr);
    if (dnum > 0) {
        Hdlr->CurDnum = dnum;
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] dnum:%d", __FUNCTION__, __LINE__, dnum);
    return dnum;
}
int AppLibDCF_FirstDir(APPLIB_DCF_HDLR_s *Hdlr)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_FirstDirImpl(Hdlr);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Goto last directory and return dnum
 *  @param [in] Hdlr Applib DCF Handler
 *  @return >=0 dir number, <0 failure
 */
static int AppLibDCF_LastDirImpl(APPLIB_DCF_HDLR_s *Hdlr)
{
    int dnum;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    dnum = AmpDCF_GetLastDnum(Hdlr->DcfHdlr);
    if (dnum > 0) {
        Hdlr->CurDnum = dnum;
    }
    DBGMSGc2(MAGENTA, "==[%s-%d], dnum:%d", __FUNCTION__, __LINE__, dnum);
    return dnum;
}
int AppLibDCF_LastDir(APPLIB_DCF_HDLR_s *Hdlr)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_LastDirImpl(Hdlr);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Goto next directory and return dnum
 *  @param [in] Hdlr Applib DCF Handler
 *  @return >=0 dir number, <0 failure
 */
static int AppLibDCF_NextDirImpl(APPLIB_DCF_HDLR_s *Hdlr)
{
    int dnum;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    dnum = AmpDCF_GetNextDnum(Hdlr->DcfHdlr);
    if (dnum > 0) {
        Hdlr->CurDnum = dnum;
    }
    DBGMSGc2(MAGENTA, "==[%s-%d], dnum:%d", __FUNCTION__, __LINE__, dnum);
    return dnum;
}
int AppLibDCF_NextDir(APPLIB_DCF_HDLR_s *Hdlr)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_NextDirImpl(Hdlr);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d], Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Goto previous directory and return dnum
 *  @param [in] Hdlr Applib DCF Handler
 *  @return >=0 dir number, <0 failure
 */
static int AppLibDCF_PrevDirImpl(APPLIB_DCF_HDLR_s *Hdlr)
{
    int dnum;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    dnum = AmpDCF_GetPrevDnum(Hdlr->DcfHdlr);
    if (dnum > 0) {
        Hdlr->CurDnum = dnum;
    }
    DBGMSGc2(MAGENTA, "==[%s-%d], dnum:%d", __FUNCTION__, __LINE__, dnum);
    return dnum;
}
int AppLibDCF_PrevDir(APPLIB_DCF_HDLR_s *Hdlr)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_PrevDirImpl(Hdlr);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d], Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Get dir list of current dnum
 *  @param [in] Hdlr Applib DCF Handler
 *  @return dir list of current dnum
 */
static AMP_DCF_DIR_LIST_s *AppLibDCF_GetDirListImpl(APPLIB_DCF_HDLR_s *Hdlr)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    return AmpDCF_GetDirectoryList(Hdlr->DcfHdlr, Hdlr->CurDnum);
}
AMP_DCF_DIR_LIST_s *AppLibDCF_GetDirList(APPLIB_DCF_HDLR_s *Hdlr)
{
    AMP_DCF_DIR_LIST_s *List = NULL;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            List = AppLibDCF_GetDirListImpl(Hdlr);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
    return List;
}

/**
 *  Release dir list
 *  @param [in] Hdlr Applib DCF Handler
 *  @param [in] List address of dir list
 *  @return >=0 success, <0 failure
 */
static int AppLibDCF_RelDirListImpl(APPLIB_DCF_HDLR_s *Hdlr, AMP_DCF_DIR_LIST_s *List)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    if (List!=NULL) {
        if (AmpDCF_RelDirectoryList(Hdlr->DcfHdlr, List) == AMP_OK) {
            DBGMSGc2(MAGENTA, "==[%s-%d] rtn 0", __FUNCTION__, __LINE__);
            return 0;
        }
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] rtn -1", __FUNCTION__, __LINE__);
    return -1;
}
int AppLibDCF_RelDirList(APPLIB_DCF_HDLR_s *Hdlr, AMP_DCF_DIR_LIST_s *List)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    K_ASSERT(List != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_RelDirListImpl(Hdlr, List);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d], Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Create an object and return a name of the new object
 *  @param [in]  Hdlr Applib DCF handler
 *  @param [in]  RootName Rootname
 *  @param [in]  ExtName extension name
 *  @param [out] ObjName address of object name
 *  @return >0 object id, <=0 failure
 */
static int AppLibDCF_CreateObjectImpl(APPLIB_DCF_HDLR_s *Hdlr, const char *RootName, const char *ExtName, char *ObjName)
{
    int i, Len;
    DBGMSGc2(MAGENTA, "==[%s -%d] -START-", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    /**< Check root name */
    for (i=0;i<Hdlr->RootList.Count;i++) {
        Len = strlen(Hdlr->RootList.List[i].Name);
        if (strncmp(Hdlr->RootList.List[i].Name, RootName, Len) == 0) {
            break;
        }
    }
    if (i < Hdlr->RootList.Count) {
        /**< Create object and add object to internal object list */
        if (g_AppLibDcf.ObjAvailList != NULL) {
            int ObjId, Rval;
            Rval = Hdlr->Naming->GetObjectName(Hdlr->DcfHdlr->Filter, Hdlr->LastInternalObjId, RootName, ExtName, ObjName);
            if (Rval < 0) {
                Perror("Get object name error!");
                return Rval;
            }
            ObjId = Hdlr->DcfHdlr->Filter->NameToId(ObjName);
            if (AppLibDCF_AddIntObj(Hdlr, ObjId, ObjName) >= 0) {
                /**< Update Last Internal Object Id */
                Hdlr->LastInternalObjId = ObjId;
                DBGMSGc2(MAGENTA, "==[%s -%d] -END-, ObjId:%d", __FUNCTION__, __LINE__, ObjId);
                return ObjId;
            } else Perror("Add internal object error!");
        } else Perror("No available internal object!");
    } else Perror("Root name error!");
    return -1;
}
int AppLibDCF_CreateObject(APPLIB_DCF_HDLR_s *Hdlr, const char *RootName, const char *ExtName, char *ObjName)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d] -START-", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    K_ASSERT(RootName != NULL);
    K_ASSERT(ExtName != NULL);
    K_ASSERT(ObjName != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_CreateObjectImpl(Hdlr, RootName, ExtName, ObjName);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-, Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Create an extended object
 *  @param [in]  Hdlr Applib DCF handler
 *  @param [in]  ObjId object id
 *  @param [in]  RootName Rootname
 *  @param [in]  ExtType Extended Object type
 *  @param [in]  SeqNum Sequence number of split file
 *  @param [in]  ExtName extension name
 *  @param [out] ObjName address of object name
 *  @return >=0 success, <0 failure
 */
static int AppLibDCF_CreateExtendedObjectImpl(APPLIB_DCF_HDLR_s *Hdlr, UINT32 ObjId, const char *RootName, UINT8 ExtType, UINT8 SeqNum, const char *ExtName, char *ObjName)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    if (g_AppLibDcf.ObjAvailList != NULL) {
        int i;
        /**< Check root name */
        for (i=0;i<Hdlr->RootList.Count;i++) {
            int Len = strlen(Hdlr->RootList.List[i].Name);
            if (strncmp(Hdlr->RootList.List[i].Name, RootName, Len) == 0)
                break;
        }
        if (i < Hdlr->RootList.Count) {
            /**< Create extended object and add object to internal object list */
            int Rval = Hdlr->Naming->GetExtObjectName(Hdlr->DcfHdlr->Filter, ObjId, RootName, ExtType, SeqNum, ExtName, ObjName);
            if (Rval >= 0) {/**< Add file to internal object list */
                if (AppLibDCF_AddIntObj(Hdlr, ObjId, ObjName) >= 0) {
                    DBGMSGc2(MAGENTA, "==[%s-%d] rtn 0", __FUNCTION__, __LINE__);
                    return 0;
                } else Perror("Add internal object error!");
            } else Perror("Get extended object error!");
        } else Perror("Root name error!");
    } else Perror("No available internal object!");
    DBGMSGc2(MAGENTA, "==[%s-%d] rtn -1", __FUNCTION__, __LINE__);
    return -1;
}
int AppLibDCF_CreateExtendedObject(APPLIB_DCF_HDLR_s *Hdlr, UINT32 ObjId, const char *RootName, UINT8 ExtType, UINT8 SeqNum, const char *ExtName, char *ObjName)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    K_ASSERT(ObjId > 0);
    K_ASSERT(RootName != NULL);
    K_ASSERT(ExtType <= APPLIB_DCF_EXT_OBJECT_SPLIT_THM);
    K_ASSERT(ExtName != NULL);
    K_ASSERT(ObjName != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_CreateExtendedObjectImpl(Hdlr, ObjId, RootName, ExtType, SeqNum, ExtName, ObjName);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Delete object files
 *  @param [in] Hdlr Applib DCF handler
 *  @param [in] ObjId object id
 *  @return >=0 success, <0 failure
 */
static int AppLibDCF_DeleteObjectImpl(APPLIB_DCF_HDLR_s *Hdlr, UINT32 ObjId)
{
    int Rval = 0;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    /**< Remove internal objects */
    AppLibDcf_RemoveIntObjById(Hdlr, ObjId);
    /**Update CurObjId*/
    if (Hdlr ->CurObjId == ObjId) {
        /**if curobj is going to remove, update curobj to next obj*/
        Hdlr ->CurObjId = AppLibDCF_NextObject(Hdlr);
    }
    /**< Remove DCF objects */
    if (AmpDCF_CheckIdValid(Hdlr->DcfHdlr, ObjId) == TRUE) {
        AMP_DCF_FILE_LIST_s *List = AmpDCF_GetFileList(Hdlr->DcfHdlr, ObjId);
        if (List != NULL) {
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
            for (int i=0;i<List->Count;i++) {
                if (AmpCFS_remove(List->FileList[i].Name) != AMP_OK) { /**< LastInternalObjId will be updated in Callback function */
                    Perror("Remove File Error!");
                    Rval = -1;
                }
            }
            if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) != OK) {
               Rval = -1;
               Perror("Get mutex error!");
            }
            if (AmpDCF_RelFileList(Hdlr->DcfHdlr, List) != AMP_OK) {
                Perror("Release file list error!");
                Rval = -1;
            }
        }
    } else {
        /**< Update LastInternalObjId */
        if (Hdlr->NumberMode == APPLIB_DCF_NUMBER_RESET) {
            APPLIB_DCF_OBJECT_LIST_NODE_s *Obj = NULL;
            if (g_AppLibDcf.ObjUsedList != NULL) {
                Obj = g_AppLibDcf.ObjUsedList;
                while (Obj != NULL) {
                    if (Obj->Hdlr == Hdlr) {
                        Hdlr->LastInternalObjId = g_AppLibDcf.ObjUsedList->ObjId;
                        break;
                    }
                    Obj = Obj->Next;
                }
            }
            if (Obj == NULL) {
                Hdlr->LastInternalObjId = AmpDCF_GetLastId(Hdlr->DcfHdlr);
                AppLibDCF_LastObjectImpl(Hdlr); /**< Goto last object */
            }
        }
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}
int AppLibDCF_DeleteObject(APPLIB_DCF_HDLR_s *Hdlr, UINT32 ObjId)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    K_ASSERT(ObjId > 0);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_DeleteObjectImpl(Hdlr, ObjId);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    
    AppLibDCF_Refresh(Hdlr);
    return Rval;
}


#ifdef CONFIG_APP_ARD
int AppLibDCF_DeleteObjectAllImpl(APPLIB_DCF_HDLR_s *Hdlr)
{
    int Rval = -1;
    UINT64 LstObjId = 0;
    UINT64 FstObjId = 0;
    INT32 ObjIdx = 0;
    UINT32 Obj = 0;

    K_ASSERT(Hdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);

    FstObjId = AppLibDCF_FirstObject(Hdlr);

    if (FstObjId > 0) {
        AMP_DCF_FILE_LIST_s * list =NULL;
        Obj = FstObjId;
        list = AppLibDCF_GetFileList(Hdlr, Obj);
        if (list != NULL) {
            ObjIdx = (list->Count - 1);
            AppLibDCF_RelFileList(Hdlr, list);
            DBGMSGc2(YELLOW,"[%s-%d] LastObjId = %d, CurObjIdIdx = %d", __FUNCTION__, __LINE__, Obj, ObjIdx);
        } else {
            AmbaPrintColor(RED,"[%s-%d] do AppLibDCF_GetFileList() Fail", __FUNCTION__, __LINE__);
        }
    } else {
        AmbaPrintColor(RED,"[%s-%d] do AppLibDCF_FirstObject() Fail", __FUNCTION__, __LINE__);
    }

    FstObjId =  (((UINT64)ObjIdx << 32) | ((UINT64)Obj));

    do {
        LstObjId = AppLibDCF_LastObject(Hdlr);

        if (LstObjId > 0) {
            AMP_DCF_FILE_LIST_s * list =NULL;

            Obj = LstObjId;
            list = AppLibDCF_GetFileList(Hdlr, Obj);
            if (list != NULL) {
                ObjIdx = (list->Count - 1);
                AppLibDCF_RelFileList(Hdlr, list);
                DBGMSGc2(YELLOW,"[%s-%d] LastObjId = %d, CurObjIdIdx = %d", __FUNCTION__, __LINE__, Obj, ObjIdx);
            } else {
                AmbaPrintColor(RED,"[%s-%d] do AppLibDCF_GetFileList() Fail", __FUNCTION__, __LINE__);
            }
        } else {
            AmbaPrintColor(RED,"[%s-%d] do AppLibDCF_LastObject() Fail", __FUNCTION__, __LINE__);
        }

        LstObjId =  (((UINT64)ObjIdx << 32) | ((UINT64)Obj));

        Rval = AppLibDCF_DeleteObject(Hdlr, (UINT32)LstObjId);
    } while(FstObjId!=LstObjId);

    // reflesh
    AppLibDCF_Refresh(Hdlr);

    return Rval;
}

int AppLibDCF_DeleteObjectAll(APPLIB_DCF_HDLR_s *Hdlr)
{
    int Rval = -1;
    K_ASSERT(Hdlr != NULL);

    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_DeleteObjectAllImpl(Hdlr);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");

    return Rval;
}

static int AppLibDCF_GetObjIdByNameImpl(char *Name)
{
    int Rval = AMP_ERROR_GENERAL_ERROR;
    APPLIB_DCF_HDLR_s *Hdlr = CfsCB_NameToHandler(Name);
    DBGMSGc2(MAGENTA, "==[%s-%d] Name:%s", __FUNCTION__, __LINE__, Name);
    K_ASSERT(Name != NULL);
    if  (Hdlr != NULL) {
        Rval = AppLibDCF_NameToId(Hdlr,Name);
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

int AppLibDCF_GetObjIdByName(char *Name)
{
    int ObjId = AMP_ERROR_GENERAL_ERROR;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Name != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            ObjId = AppLibDCF_GetObjIdByNameImpl(Name);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] ObjId:%d", __FUNCTION__, __LINE__, ObjId);
    return ObjId;
}
#endif


/**
 *  Translate object name to id
 *  @param [in] Hdlr Applib DCF handler
 *  @param [in] ObjName object name
 *  @return >0 success, =0 failure
 */
static UINT32 AppLibDCF_NameToIdImpl(APPLIB_DCF_HDLR_s *Hdlr, char *ObjName)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    K_ASSERT(Hdlr->DcfHdlr->Filter != NULL);
    return Hdlr->DcfHdlr->Filter->NameToId(ObjName);
}
UINT32 AppLibDCF_NameToId(APPLIB_DCF_HDLR_s *Hdlr, char *ObjName)
{
    UINT32 Rval = 0;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    K_ASSERT(ObjName != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_NameToIdImpl(Hdlr, ObjName);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Get dnum from object id
 *  @param [in] Hdlr Applib DCF handler
 *  @param [in] ObjectId object id
 *  @return >0 success, =0 failure
 */
static UINT32 AppLibDCF_IdToDnumImpl(APPLIB_DCF_HDLR_s *Hdlr, UINT32 ObjectId)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    K_ASSERT(Hdlr->DcfHdlr->Filter != NULL);
    return Hdlr->DcfHdlr->Filter->IdToDnum(ObjectId);
}
UINT32 AppLibDCF_IdToDnum(APPLIB_DCF_HDLR_s *Hdlr, UINT32 ObjectId)
{
    UINT32 Rval = 0;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    K_ASSERT(ObjectId > 0);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_IdToDnumImpl(Hdlr, ObjectId);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Get fnum from object id
 *  @param [in] Hdlr Applib DCF handler
 *  @param [in] ObjectId object id
 *  @return >0 success, =0 failure
 */
static UINT32 AppLibDCF_IdToFnumImpl(APPLIB_DCF_HDLR_s *Hdlr, UINT32 ObjectId)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    K_ASSERT(Hdlr->DcfHdlr->Filter != NULL);
    return Hdlr->DcfHdlr->Filter->IdToFnum(ObjectId);
}
UINT32 AppLibDCF_IdToFnum(APPLIB_DCF_HDLR_s *Hdlr, UINT32 ObjectId)
{
    UINT32 Rval = 0;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    K_ASSERT(ObjectId > 0);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_IdToFnumImpl(Hdlr, ObjectId);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}


/**
 *  Get object amount, internal objects are not included
 *  @param [in] Hdlr Applib DCF handler
 *  @return object amount
 */
static int AppLibDCF_GetObjectAmountImpl(APPLIB_DCF_HDLR_s *Hdlr)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    return Hdlr->ObjAmount[Hdlr->BrowseMode];
}
int AppLibDCF_GetObjectAmount(APPLIB_DCF_HDLR_s *Hdlr)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_GetObjectAmountImpl(Hdlr);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Get media type of an object
 *  @param [in] Name filename
 *  @return MediaType
 */
static int AppLibDCF_GetMediaTypeImpl(char *Name)
{
    int Rval = -1;
    char *pStr;
    pStr = strrchr(Name, '.');
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    for (int i=0;i<g_AppLibDcf.MediaTypes.Count;i++) {
        if (strcmp(g_AppLibDcf.MediaTypes.List[i].ExtName, pStr+1) == 0) {
            Rval = i;
        }
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}
int AppLibDCF_GetMediaType(char *Name)
{
    int MediaType = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Name != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            MediaType = AppLibDCF_GetMediaTypeImpl(Name);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] MediaType:%d", __FUNCTION__, __LINE__, MediaType);
    return MediaType;
}

/**
 *  Set browse mode
 *  @param [in] Hdlr Applib DCF Handler
 *  @param [in] MediaType Media Type
 *  return object Id
 */
static UINT32 AppLibDCF_SetBrowseModeImpl(APPLIB_DCF_HDLR_s *Hdlr, APPLIB_DCF_MEDIA_TYPE_e MediaType)
{
    UINT32 ObjId = Hdlr->CurObjId;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    if (Hdlr->CurObjId > 0) {
        if (Hdlr->BrowseMode != MediaType) {
            Hdlr->BrowseMode = MediaType;
            if (MediaType != APPLIB_DCF_MEDIA_DCIM) {
                if (AppLibDCF_GetMediaTypeById(Hdlr, Hdlr->CurObjId) != MediaType) {
                    ObjId = AppLibDCF_LastObjectImpl(Hdlr);
                }
            }
        }
    } else {
        Hdlr->BrowseMode = MediaType;
        ObjId = AppLibDCF_LastObjectImpl(Hdlr);
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] ObjId:%d", __FUNCTION__, __LINE__, ObjId);
    return ObjId;
}
UINT32 AppLibDCF_SetBrowseMode(APPLIB_DCF_HDLR_s *Hdlr, APPLIB_DCF_MEDIA_TYPE_e MediaType)
{
    UINT32 ObjId = 0;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    K_ASSERT(MediaType <= g_AppLibDcf.MediaTypes.Count);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            ObjId = AppLibDCF_SetBrowseModeImpl(Hdlr, MediaType);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] ObjId:%d", __FUNCTION__, __LINE__, ObjId);
    return ObjId;
}
/**
 *  Get browse mode
 *  @param [in] Hdlr Applib DCF Handler
 *  @return browse mode
 */
int AppLibDCF_GetBrowseMode(APPLIB_DCF_HDLR_s *Hdlr)
{
    DBGMSGc2(MAGENTA, "==[%s-%d] Hdlr->BrowseMode: %d", __FUNCTION__, __LINE__, Hdlr->BrowseMode);
    K_ASSERT(Hdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    return Hdlr->BrowseMode;
}

/**
 *  Get number mode
 *  @param [in] Hdlr Applib DCF Handler
 *  @return number mode
 */
static int AppLibDCF_GetNumberModeImpl(APPLIB_DCF_HDLR_s *Hdlr)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    return Hdlr->NumberMode;
}
int AppLibDCF_GetNumberMode(APPLIB_DCF_HDLR_s *Hdlr)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_GetNumberModeImpl(Hdlr);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Set number mode
 *  @param [in] Hdlr Applib DCF Handler
 *  @param [in] NumberMode number mode
 *  @param [in] ObjId object id of last object
 *  @return >=0 success, <0 failure
 */
static int AppLibDCF_SetNumberModeImpl(APPLIB_DCF_HDLR_s *Hdlr, APPLIB_DCF_NUMBER_MODE_e NumberMode, UINT32 ObjId)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    if (NumberMode == APPLIB_DCF_NUMBER_SERIAL) {
        if ((ObjId == 0) || (Hdlr->Naming->IsIdValid(Hdlr->DcfHdlr->Filter, ObjId) == TRUE)) {
            if (ObjId > Hdlr->LastInternalObjId) {
                Hdlr->LastInternalObjId = ObjId;
            }
        } else {
            Perror("Invalid Object Id");
            return -1;
        }
    }
    Hdlr->NumberMode = NumberMode;
    DBGMSGc2(MAGENTA, "==[%s-%d] ret 0", __FUNCTION__, __LINE__);
    return 0;
}
int AppLibDCF_SetNumberMode(APPLIB_DCF_HDLR_s *Hdlr, APPLIB_DCF_NUMBER_MODE_e NumberMode, UINT32 ObjId)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    K_ASSERT((NumberMode == APPLIB_DCF_NUMBER_SERIAL) || (NumberMode == APPLIB_DCF_NUMBER_RESET));
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_SetNumberModeImpl(Hdlr, NumberMode, ObjId);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Delete a handler
 *  @param [in] Hdlr Applib DCF handler
 *  @return >=0 success, <0 failure
 */
static int AppLibDCF_DeleteImpl(APPLIB_DCF_HDLR_s *Hdlr)
{
    int Rval;
    int i;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    AppLibDcf_RemoveIntObjByHdlr(Hdlr);
    Rval = AmpDCF_Delete(Hdlr->DcfHdlr);
    if (Rval != AMP_OK) {
        Perror("Delete Handler Fail!");
    }
    for (i=0;i<Hdlr->RootList.Count;i++) {
        AmpCFS_ClearCache(Hdlr->RootList.List[i].Name[0]);
    }
    Hdlr->DcfHdlr = NULL;
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}
int AppLibDCF_Delete(APPLIB_DCF_HDLR_s *Hdlr)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_DeleteImpl(Hdlr);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Add roots to a handler
 *  @param [in] Hdlr APPLIB DCF handler
 *  @return >=0 success, <0 failure
 */
static int AppLibDCF_AddRoot(APPLIB_DCF_HDLR_s *Hdlr)
{
    AMP_CFS_STAT Stat;
    int Rval = AMP_OK;
    DBGMSGc2(MAGENTA, "==[%s-%d] -START-", __FUNCTION__, __LINE__);
    AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
    for (int i=0;i<Hdlr->RootList.Count;i++) {
        Rval = AMP_OK;
        if (AmpCFS_Stat(Hdlr->RootList.List[i].Name, &Stat) != AMP_OK) {
            Rval = AmpCFS_Mkdir(Hdlr->RootList.List[i].Name); /**< Create root path */
        }
        if (Rval == AMP_OK) {
            if (AmpDCF_AddRoot(Hdlr->DcfHdlr, Hdlr->RootList.List[i].Name) == AMP_OK) {
                continue;
            } else Perror("Add root path error!");
        } else Perror("Create root path error!");
        Rval = -1;
        break;
    }
    if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) != OK) {
        Perror("Get Applib DCF Mutex error!");
        Rval = -1;
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] -END- Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 *  Remove roots from a handler
 *  @param [in] Hdlr APPLIB DCF handler
 *  @return >=0 success, <0 failure
 */
static int AppLibDCF_RemoveRoot(APPLIB_DCF_HDLR_s *Hdlr)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    for (int i=0;i<Hdlr->RootList.Count;i++) {
        if (AmpDCF_RemoveRoot(Hdlr->DcfHdlr, Hdlr->RootList.List[i].Name) != AMP_OK) {
            Perror("remove root path error!");
            return -1;
        }
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
    return 0;
}

/**
 *  Scan directories and files for a handler
 *  @param [in] Hdlr APPLIB DCF handler
 *  @return >=0 success, <0 failure
 */
static int AppLibDCF_Scan(APPLIB_DCF_HDLR_s *Hdlr)
{
    int Rval;
    DBGMSGc2(MAGENTA, "==[%s-%d] -START-", __FUNCTION__, __LINE__);
    AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
    Rval = AmpDCF_Scan(Hdlr->DcfHdlr, TRUE);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (Rval == AMP_OK) {
            DBGMSGc2(MAGENTA, "==[%s-%d] -END- rtn 0", __FUNCTION__, __LINE__);
            return 0;
        } else Perror("Scan file error!");
    } else Perror("Get applib DCF mutex error!");
    return -1;
}

/**
 *  Refresh DCF & CFS
 *  @param [in] Hdlr APPLIB DCF handler
 *  @return >=0 Object Id, <0 failure
 */
static int AppLibDCF_RefreshImpl(APPLIB_DCF_HDLR_s *Hdlr)
{
    DBGMSGc2(MAGENTA, "==[%s-%d] -START-", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    /**< Remove CFS cache */
    for (int i=0;i<Hdlr->RootList.Count;i++) {
        AmpCFS_ClearCache(Hdlr->RootList.List[i].Name[0]);
    }
    /**< Remove roots */
    if (Hdlr->Active == TRUE) {
        Hdlr->Active = FALSE;
        if (AppLibDCF_RemoveRoot(Hdlr) < 0) {
            Perror("Remove root: Fail!");
            return -1;
        }
        AppLibDcf_RemoveIntObjByHdlr(Hdlr);
    }
    /**< Add and scan roots */
    if (AppLibDCF_AddRoot(Hdlr) >= 0) {
        if (AppLibDCF_Scan(Hdlr) >= 0) {
            int ObjId;
            Hdlr->Active = TRUE;
            if (Hdlr->CurObjId !=0) {
                Hdlr->CurObjId = 0;
            }
            ObjId = AppLibDCF_LastObjectImpl(Hdlr);
            if (ObjId >= 0) {
                if (Hdlr->NumberMode == APPLIB_DCF_NUMBER_RESET) {
                    Hdlr->LastInternalObjId = ObjId;
                } else {
                    if (ObjId > InitCFSConfig.MixLastIdx) {
                        Hdlr->LastInternalObjId = ObjId;
                    } else {
                        Hdlr->LastInternalObjId = InitCFSConfig.MixLastIdx;
                    }
                }
                AppLibDcf_InitObjectAmount(Hdlr);
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
                return Hdlr->LastInternalObjId;
            } else Perror("Get Last Object Error!");
        } else Perror("Scan root: Fail!");
    } else Perror("Add root: Fail!");
    return -1;
}
int AppLibDCF_Refresh(APPLIB_DCF_HDLR_s *Hdlr)
{
    int Rval = -1;
    DBGMSGc2(MAGENTA, "==[%s-%d] -START-", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            Rval = AppLibDCF_RefreshImpl(Hdlr);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s -%d] -END- Rval:%d", __FUNCTION__, __LINE__, Rval);
    return Rval;
}

/**
 * Debug
 */
/**
 *  List internal objects
 */
static void AppLibDCF_ListInternalObjectImpl(void)
{
    APPLIB_DCF_OBJECT_LIST_NODE_s *pCurObj, *pTail;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (g_AppLibDcf.ObjUsedList != NULL) {

        pTail = g_AppLibDcf.ObjUsedList->Prev;
        pCurObj = pTail;
        do {
            AmbaPrint("Object Id = %d, Name = %s", pCurObj->ObjId, pCurObj->ObjName.Name);
            pCurObj = pCurObj->Prev;
        } while (pCurObj != pTail);
    }
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
}
void AppLibDCF_ListInternalObject(void)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            AppLibDCF_ListInternalObjectImpl();
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
}

/**
 *  Get dir list by dnum
 *  @param [in] Hdlr Applib DCF Handler
 *  @param [in] dnum
 *  @return dir list of specific dnum
 */
static AMP_DCF_DIR_LIST_s *AppLibDCF_GetDirListByDnumImpl(APPLIB_DCF_HDLR_s *Hdlr, UINT32 Dnum)
{
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr->DcfHdlr != NULL);
    K_ASSERT(Hdlr->Active == TRUE);
    return AmpDCF_GetDirectoryList(Hdlr->DcfHdlr, Dnum);
}
AMP_DCF_DIR_LIST_s *AppLibDCF_GetDirListByDnum(APPLIB_DCF_HDLR_s *Hdlr, UINT32 Dnum)
{
    AMP_DCF_DIR_LIST_s *List = NULL;
    DBGMSGc2(MAGENTA, "==[%s-%d]", __FUNCTION__, __LINE__);
    K_ASSERT(Hdlr != NULL);
    if (AmbaKAL_MutexTake(&g_AppLibDcf.ApplibMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
        if (AmbaKAL_MutexTake(&g_AppLibDcf.Mutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            List = AppLibDCF_GetDirListByDnumImpl(Hdlr, Dnum);
            AmbaKAL_MutexGive(&g_AppLibDcf.Mutex);
        } else Perror("Get Mutex error!");
        AmbaKAL_MutexGive(&g_AppLibDcf.ApplibMutex);
    } else Perror("Get ApplibMutex error!");
    DBGMSGc2(MAGENTA, "==[%s-%d] -END-", __FUNCTION__, __LINE__);
    return List;
}

