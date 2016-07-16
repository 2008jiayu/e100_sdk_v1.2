/**
 * @file src/app/Policecam/applib/src/dcf/ApplibDCF_NamingRuleCarCam.c
 *
 * Implementation of DCF Applib naming rule
 *
 * History:
 *    2015/02/05 - [Jim Meng] created file
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
#include <dcf/ApplibDcf.h>
#include <AmbaRTC.h>
#include "ApplibDcf_FilterPoliceCam.h"
#include "ApplibDcf_NamingRulePoliceCam.h"


static  char FilePFX[DCF_FILE_PFX_LEN];/**<< for file prefix */

static void PerrorImpl(UINT32 nLine, char *ErrMsg)
{
    char FileName[MAX_FILENAME_LENGTH];
    strncpy(FileName, (strrchr(__FILE__, '\\') + 1), (strlen(__FILE__) - strlen((strrchr(__FILE__, '\\') + 1)) + 1));
    AmbaPrint("[Error]%s %u: %s", FileName, nLine, ErrMsg);
}

#define Perror(ErrMsg) {\
    PerrorImpl(__LINE__, ErrMsg);\
}

/**
 * Internal functions
 */
/**
 *  Get Directory Name
 *  @param [in]  Dnum directory number
 *  @param [out] Dname directory name
 */
static void Naming_GetDirName(int Dnum, char *Dname)
{
    int i;
    K_ASSERT((DCF_DIR_PFX_LEN+DCF_DIR_NUM_LEN+DCF_DIR_NUM_LEN) <= MAX_FILENAME_LENGTH);
#if DCF_DIR_PFX_LEN
    strncpy(Dname, DCF_DIR_PFX_STR, DCF_DIR_PFX_LEN);
#endif
    for (i=DCF_DIR_NUM_LEN-1;i>=0;i--) {
        Dname[DCF_DIR_PFX_LEN+i] = '0'+(Dnum%10);
        Dnum /= 10;
    }
#if DCF_DIR_SFX_LEN
    strncpy(&Dname[DCF_DIR_PFX_LEN+DCF_DIR_NUM_LEN], DCF_DIR_SFX_STR, DCF_DIR_SFX_LEN);
#endif
    Dname[DCF_DIR_PFX_LEN+DCF_DIR_NUM_LEN+DCF_DIR_SFX_LEN] = '\0';
	AmbaPrint("Naming_GetDirName dir name = %s",Dname);

}
/**
 *  Get filename prefix
 * Get file prefix when create new object
 *  @param [out] FilePFX file prefix
 */
static void Naming_GetFileNamePFX(WCHAR *FilePFX)
{
    int ReturnValue;
    char temp[DCF_FILE_PFX_LEN];
    char temp1[DCF_FILE_PFX_LEN];	
    AMBA_RTC_TIME_SPEC_u TimeSpec = {0};

    ReturnValue = AmbaRTC_GetSystemTime(AMBA_TIME_STD_TAI, &TimeSpec);
    if (ReturnValue < 0) {
            AmbaPrintColor(RED, "[Get File Name] AmbaRTC_GetSystemTime failure. ReturnValue = %d", ReturnValue);
    }
    ReturnValue = TimeSpec.Calendar.Year*10000+TimeSpec.Calendar.Month*100+TimeSpec.Calendar.Day;
    sprintf(temp,"%08d",ReturnValue);	
    ReturnValue = TimeSpec.Calendar.Hour*10000+TimeSpec.Calendar.Minute*100+TimeSpec.Calendar.Second;
    sprintf(temp1,"%06d",ReturnValue);
    strcat(temp, temp1);	
    temp[DCF_FILE_PFX_LEN-1] = '_';
    strncpy(FilePFX, temp, sizeof(CHAR)*DCF_FILE_PFX_LEN);

}

/**
 *  Get filename of an object
 *  @param [in]  Fnum file number
 *  @param [out] Fname file name
 */
static void Naming_GetFileName(int Fnum, char *Fname)
{
    int i;

    K_ASSERT((DCF_FILE_PFX_LEN+DCF_FILE_NUM_LEN+DCF_FILE_SFX_LEN) <= MAX_FILENAME_LENGTH);
#if DCF_FILE_PFX_LEN
    Naming_GetFileNamePFX(FilePFX);
    strncpy(Fname, FilePFX, DCF_FILE_PFX_LEN);
#endif
    for (i=DCF_FILE_NUM_LEN-1;i>=0;i--) {
        Fname[DCF_FILE_PFX_LEN+i] = '0'+(Fnum%10);
        Fnum /= 10;
    }
#if DCF_FILE_SFX_LEN
    strncpy(&Fname[DCF_FILE_PFX_LEN+DCF_FILE_NUM_LEN], DCF_FILE_SFX_STR, DCF_FILE_SFX_LEN);
#endif
    Fname[DCF_FILE_PFX_LEN+DCF_FILE_NUM_LEN+DCF_FILE_SFX_LEN] = '\0';
    AmbaPrint("Naming_GetFileName file name = %s",Fname);
}

/**
 *  Get thumbnail name (image/video)
 *  @param [in]  Fnum file number
 *  @param [out] Fname file name
 */
static void Naming_GetThmFileName(int Fnum, char *Fname)
{
    int i;
    K_ASSERT((DCF_FILE_PFX_LEN+DCF_FILE_NUM_LEN+DCF_FILE_SFX_LEN+DCF_FILE_THM_LEN) <= MAX_FILENAME_LENGTH);
#if DCF_FILE_PFX_LEN
    strncpy(Fname, FilePFX, DCF_FILE_PFX_LEN);
#endif
    for (i=DCF_FILE_NUM_LEN-1;i>=0;i--) {
        Fname[DCF_FILE_PFX_LEN + i] = '0' + (Fnum%10);
        Fnum /= 10;
    }
#if DCF_FILE_SFX_LEN
    strncpy(&Fname[DCF_FILE_PFX_LEN+DCF_FILE_NUM_LEN], DCF_FILE_SFX_STR, DCF_FILE_SFX_LEN);
#endif
#if DCF_FILE_THM_LEN
    strncpy(&Fname[DCF_FILE_PFX_LEN+DCF_FILE_NUM_LEN+DCF_FILE_SFX_LEN], DCF_FILE_THM_STR, DCF_FILE_THM_LEN);
#endif
    Fname[DCF_FILE_PFX_LEN+DCF_FILE_NUM_LEN+DCF_FILE_SFX_LEN+DCF_FILE_THM_LEN] = '\0';
}

/**
 *  Get filename of a split file
 *  @param [in]  SeqNum sequence number
 *  @param [in]  Fnum file number
 *  @param [out] Fname file name
 */
static void Naming_GetSplitFileName(UINT SeqNum, int Fnum, char *Fname, char *ThmName)
{
    int i;
    K_ASSERT(SeqNum <= DCF_SEQ_NUM_MAX);
    K_ASSERT((DCF_FILE_PFX_LEN + DCF_FILE_NUM_LEN + DCF_FILE_SFX_LEN + strlen(ThmName)) <= MAX_FILENAME_LENGTH);
#if DCF_FILE_PFX_LEN
    K_ASSERT(DCF_FILE_PFX_LEN >= DCF_SEQ_NUM_LEN);
    strncpy(Fname, FilePFX, DCF_FILE_PFX_LEN - DCF_SEQ_NUM_LEN);
    for (i=DCF_SEQ_NUM_LEN-1;i>=0;i--) {
        Fname[DCF_FILE_PFX_LEN-DCF_SEQ_NUM_LEN+i] = '0'+(SeqNum%10);
        SeqNum /= 10;
    }
    K_ASSERT(SeqNum == 0); /**< DCF_SEQ_NUM_LEN is not enough */
#endif
    for (i=DCF_FILE_NUM_LEN-1;i>=0;i--) {
        Fname[DCF_FILE_PFX_LEN+i] = '0'+(Fnum%10);
        Fnum /= 10;
    }
#if DCF_FILE_SFX_LEN
    strncpy(Fname + DCF_FILE_PFX_LEN + DCF_FILE_NUM_LEN, DCF_FILE_SFX_STR, DCF_FILE_SFX_LEN);
#endif
    Fname[DCF_FILE_PFX_LEN + DCF_FILE_NUM_LEN + DCF_FILE_SFX_LEN] = 0;
    strcat(Fname, ThmName);
}

/**
 * APIs for DCF Applib Naming Rule
 */
/**
 *  Check if an Object ID is Valid
 *  @param [in]  pFilter address of filter
 *  @param [in]  ObjId Object Id
 *  @return TRUEValid, FALSE Invalid
 */
static BOOL Naming_IsIdValid(AMP_DCF_FILTER_s *pFilter, UINT32 ObjId)
{
    UINT32 Dnum, Fnum;
    Dnum = pFilter->IdToDnum(ObjId);
    if ((Dnum >= DCF_DIR_NUM_MIN) && (Dnum <= DCF_DIR_NUM_MAX)) {
        Fnum = pFilter->IdToFnum(ObjId);
        if ((Fnum >= DCF_FILE_NUM_MIN) && (Fnum <= DCF_FILE_NUM_MAX)) {
            return TRUE;
        }
    }
    return FALSE;
}

/**
 *  Get object name
 *  @param [in]  pFilter address of filter
 *  @param [in]  LastObjId last Object's id
 *  @param [in]  RootName Rootname
 *  @param [in]  ExtName extension name
 *  @param [out] ObjName address of object name
 *  @return >0 success, <0 failure
 */
static int Naming_GetObjectName(AMP_DCF_FILTER_s *pFilter, UINT32 LastObjId, const char *RootName, const char *ExtName, char *ObjName)
{
    char TmpName[MAX_FILENAME_LENGTH];
    char DirPath[MAX_FILENAME_LENGTH];
    UINT32 Dnum, Fnum;

    /**< Get Dnum and Fnum of the last object */
    if (LastObjId == 0) { /**< No object in DCF */
        Dnum = DCF_DIR_NUM_MIN - 1;
        Fnum = DCF_FILE_NUM_MAX;
    } else {
        Dnum = pFilter->IdToDnum(LastObjId);
        K_ASSERT((Dnum >= DCF_DIR_NUM_MIN) && (Dnum <= DCF_DIR_NUM_MAX));
        Fnum = pFilter->IdToFnum(LastObjId);
        K_ASSERT((Fnum >= DCF_FILE_NUM_MIN) && (Fnum <= DCF_FILE_NUM_MAX));
    }
    /**< Get Dnum and Fnum of new object */
    if (Fnum < DCF_FILE_NUM_MAX) {
        Fnum++;
        /**< Get dirctory name */
        Naming_GetDirName(Dnum, TmpName);
        strncpy(DirPath, RootName, MAX_FILENAME_LENGTH);
        strcat(DirPath, "\\");
        strcat(DirPath, TmpName);
    } else {
        Fnum = DCF_FILE_NUM_MIN;
        Dnum++;
        if (Dnum <= DCF_DIR_NUM_MAX) {
            AMP_CFS_STAT Stat;
            /**< Get dirctory name */
            Naming_GetDirName(Dnum, TmpName);
            strncpy(DirPath, RootName, MAX_FILENAME_LENGTH);
            strcat(DirPath, "\\");
            strcat(DirPath, TmpName);
            /**< Check and Create directory */
            if (AmpCFS_Stat(DirPath, &Stat) != AMP_OK) {
                if (AmpCFS_Mkdir(DirPath) != AMP_OK) {
                    Perror("Make directory Error!");
                    return -1;
                }
            }
        } else { /**< Run out of id */
            return -1;
        }
    }
    /**< Get filename */
    Naming_GetFileName(Fnum, TmpName);
    strncpy(ObjName, DirPath, MAX_FILENAME_LENGTH);
    strcat(ObjName, "\\");
    strcat(ObjName, TmpName);
    strcat(ObjName, ".");
    strcat(ObjName, ExtName);

    AmbaPrint("Naming_GetObjectName file name = %s",ObjName);	
    return 0;
}

/**
 *  Get fullname of an extended object
 *  @param [in]  pFilter address of filter
 *  @param [in]  ObjId Object Id
 *  @param [in]  RootName Rootname
 *  @param [in]  ExtType type of extended object
 *  @param [in]  SeqNum sequence number
 *  @param [in]  ExtName extension name
 *  @param [out] ObjName address of object name
 *  @return >0 success, <0 failure
 */
static int Naming_GetExtObjectName(AMP_DCF_FILTER_s *pFilter, UINT32 ObjId, const char *RootName, UINT8 ExtType, UINT8 SeqNum, const char *ExtName, char *ObjName)
{
    char TmpName[MAX_FILENAME_LENGTH]= {'\0'};
    char DirPath[MAX_FILENAME_LENGTH]= {'\0'};
    AMP_CFS_STAT Stat;
    UINT32 Dnum, Fnum;
    Dnum = pFilter->IdToDnum(ObjId);
    K_ASSERT((Dnum >= DCF_DIR_NUM_MIN) && (Dnum <= DCF_DIR_NUM_MAX));
    Fnum = pFilter->IdToFnum(ObjId);
    K_ASSERT((Fnum >= DCF_FILE_NUM_MIN) && (Fnum <= DCF_FILE_NUM_MAX));
    /**< Get dir name */
    Naming_GetDirName(Dnum, TmpName);
    strncpy(DirPath, RootName, MAX_FILENAME_LENGTH);
    strcat(DirPath, "\\");
    strcat(DirPath, TmpName);
    /**< Check and Create directory */
    if (AmpCFS_Stat(DirPath, &Stat) != AMP_OK) { /**< check if directory exist */
        if (AmpCFS_Mkdir(DirPath) != AMP_OK) {
            Perror("Create directory Error!");
            return -1;
        }
    }
    /**< Get file name */
    switch (ExtType) {
    case APPLIB_DCF_EXT_OBJECT_IMAGE_THM:
        Naming_GetThmFileName(Fnum, TmpName);
        break;
    case APPLIB_DCF_EXT_OBJECT_VIDEO_THM:
        Naming_GetThmFileName(Fnum, TmpName);
        break;
    case APPLIB_DCF_EXT_OBJECT_SPLIT_FILE:
        Naming_GetSplitFileName(SeqNum, Fnum, TmpName, "");
        break;
    case APPLIB_DCF_EXT_OBJECT_SPLIT_THM:
        Naming_GetSplitFileName(SeqNum, Fnum, TmpName, DCF_FILE_THM_STR);
        break;
    default:
        Perror("Incorrect ExtType!");
        return -1;
    }
    strncpy(ObjName, DirPath, MAX_FILENAME_LENGTH);
    strcat(ObjName, "\\");
    strcat(ObjName, TmpName);
    strcat(ObjName, ".");
    strcat(ObjName, ExtName);
    return 0;
}

/**
 * A global variable to point to functions of the Naming Rule
 */
APPLIB_DCF_NAMING_s g_AppLibDcfNaming1 = {
    Naming_IsIdValid,
    Naming_GetObjectName,
    Naming_GetExtObjectName
};

