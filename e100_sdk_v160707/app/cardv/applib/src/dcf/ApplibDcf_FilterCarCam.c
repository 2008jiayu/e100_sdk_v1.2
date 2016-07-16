/**
 * @file src/app/connected/applib/src/dcf/ApplibDcf_FilterCarCam.c
 *
 * Implementation of dcf filter
 *
 * History:
 *    2013/11/6 - [YC Liao] created file
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

#include <wchar.h>
#include <dcf/AmpDcf.h>
#include "AppLibDcf_FilterCarCam.h"

static void PerrorImpl(UINT32 nLine, char *ErrMsg)
{
    char FileName[MAX_FILENAME_LENGTH];
    strncpy(FileName, (strrchr(__FILE__, '\\') + 1), (strlen(__FILE__) - strlen((strrchr(__FILE__, '\\') + 1)) + 1));
    AmbaPrint("[Error]%s %u: %s", FileName, nLine, ErrMsg);
}

#define Perror(ErrMsg) {\
    PerrorImpl(__LINE__, ErrMsg);\
}

static UINT32 AppLibFilter_GetId(UINT32 Dnum, UINT32 Fnum)
{
    const UINT32 Id = (INDEX_BASE | (Dnum << DCF_DNUM_SHIFT) | Fnum);

    if (Dnum < DCF_DIR_NUM_MIN)
        return 0;
    if (Dnum > DCF_DIR_NUM_MAX)
        return 0;
    if (Fnum < DCF_FILE_NUM_MIN)
        return 0;
    if (Fnum > DCF_FILE_NUM_MAX)
        return 0;
    return Id;
}

static UINT32 AppLibFilter_IdToDnum(UINT32 Id)
{
    UINT32 num = Id & (~INDEX_BASE);  // remove INDEX_BASE

    if ((Id >> DCF_BASE_SHIFT) != (INDEX_BASE >> DCF_BASE_SHIFT))
        return 0;
    num = num >> DCF_DNUM_SHIFT;
    if (num < DCF_DIR_NUM_MIN)
        return 0;
    if (num > DCF_DIR_NUM_MAX)
        return 0;
    return num;
}

static UINT32 AppLibFilter_IdToFnum(UINT32 Id)
{
    UINT32 num = Id & (~INDEX_BASE);  // remove INDEX_BASE

    if ((Id >> DCF_BASE_SHIFT) != (INDEX_BASE >> DCF_BASE_SHIFT))
        return 0;
    num = num & DCF_FNUM_MASK;
    if (num < DCF_FILE_NUM_MIN)
        return 0;
    if (num > DCF_FILE_NUM_MAX)
        return 0;
    return num;
}

static WCHAR *AppLibFilter_GetFnum(WCHAR *Name, WCHAR *pTail, UINT32 *pFnum)
{
    UINT32 fnum;
    WCHAR *end;
    char num[DCF_FILE_NUM_LEN + 1] = {0};

    // reverse scan suffix
    end = pTail - DCF_FILE_SFX_LEN;
    if (end < Name)
        return NULL;
    while (pTail > end) {
        if (!DCF_IS_CHAR(*pTail))
            return NULL;
        pTail--;
    }
    // reverse scan fnum
    end = pTail - DCF_FILE_NUM_LEN;
    if (end < Name)
        return NULL;
    while (pTail > end) {
        if (!DCF_IS_DIGIT(*pTail))
            return NULL;
        num[pTail - end - 1] = (char)*pTail;
        pTail--;
    }
    fnum = strtoul(num, NULL, 10);
    if ((fnum < DCF_FILE_NUM_MIN) || (fnum > DCF_FILE_NUM_MAX))
        return NULL;
    // reverse scan prefix
    pTail = pTail - DCF_FILE_PFX_LEN;
    if (pTail < Name)
        return NULL;
    // check delimiter
    if (!DCF_IS_DELIM(*pTail))
        return NULL;
    pTail--;
    *pFnum = fnum;
    return pTail;
}

static WCHAR *AppLibFilter_GetDnum(WCHAR *Name, WCHAR *pTail, UINT32 *pDnum)
{
    UINT32 dnum;
    WCHAR *end;
    char num[DCF_DIR_NUM_LEN + 1] = {0};

    // reverse scan suffix
    end = pTail - DCF_DIR_SFX_LEN;
    if (end < Name)
        return NULL;
    while (pTail > end) {
        if (!DCF_IS_CHAR(*pTail))
            return NULL;
        pTail--;
    }
    // reverse scan dnum
    end = pTail - DCF_DIR_NUM_LEN;
    if (end < Name)
        return NULL;
    while (pTail > end) {
        if (!DCF_IS_DIGIT(*pTail))
            return NULL;
        num[pTail - end - 1] = (char)*pTail;
        pTail--;
    }
    dnum = strtoul(num, NULL, 10);
    if ((dnum < DCF_DIR_NUM_MIN) || (dnum > DCF_DIR_NUM_MAX))
        return NULL;
    // reverse scan prefix
    end = pTail - DCF_DIR_PFX_LEN;
    if (end < Name)
        return NULL;
    while (pTail > end) {
        if (!DCF_IS_CHAR(*pTail))
            return NULL;
        pTail--;
    }
    // check delimiter
    if (!DCF_IS_DELIM(*pTail))
        return NULL;
    pTail--;
    *pDnum = dnum;
    return pTail;
}

static UINT32 AppLibFilter_NameToDnum(WCHAR *Name)
{
    WCHAR *p;
    UINT32 dnum;
    const UINT32 len = w_strlen(Name);

    K_ASSERT(Name != NULL);
    if (len < 1 + DCF_DIR_PFX_LEN + DCF_DIR_NUM_LEN + DCF_DIR_SFX_LEN)
        return 0;
    p = &Name[len - 1];
    if (DCF_IS_DELIM(*p))
        p--;
    p = AppLibFilter_GetDnum(Name, p, &dnum);
    if (p == NULL)
        return 0;
    return dnum;
}

static UINT32 AppLibFilter_NameToId(WCHAR *Name)
{
    WCHAR *p;
    UINT32 fnum, dnum;

    K_ASSERT(Name != NULL);
    if (w_strlen(Name) <= 1 + DCF_DIR_PFX_LEN + DCF_DIR_NUM_LEN + DCF_DIR_SFX_LEN + 1 + DCF_FILE_PFX_LEN + DCF_FILE_NUM_LEN + DCF_FILE_SFX_LEN + 1)
        return 0;
    // reverse search for '.'
    p = w_strrchr(Name, L'.');
    if (p == NULL)
        return 0;
    do {
        p--;
    } while ((p > Name)&&(!DCF_IS_DIGIT(*p)));
    if (p <= Name)
        return 0;
    p = AppLibFilter_GetFnum(Name, p, &fnum);
    if (p <= Name)
        return 0;
    p = AppLibFilter_GetDnum(Name, p, &dnum);
    if (p == NULL)
        return 0;
    return AppLibFilter_GetId(dnum, fnum);
}

static int AppLibFilter_ScanDirectory(WCHAR *Path, AMP_DCF_TABLE_HDLR_s *pTable)
{
    WCHAR pattern[MAX_FILENAME_LENGTH];
    AMP_CFS_DTA dta;
    UINT32 i;

    w_strncpy(pattern, Path, MAX_FILENAME_LENGTH);
    w_strcat(pattern, L"\\");
    for (i=0; i<DCF_DIR_PFX_LEN + DCF_DIR_NUM_LEN + DCF_DIR_SFX_LEN; i++)
        w_strcat(pattern, L"?");

    if (AmpCFS_FirstDirEnt(pattern, AMP_CFS_ATTR_ALL, &dta) == AMP_OK) {
        UINT32 dnum;
        WCHAR *dn;
        WCHAR name[MAX_FILENAME_LENGTH];
        while (TRUE) {
            if (dta.LongName[0] == 0)
                dn = (WCHAR *)dta.FileName;
            else
                dn = (WCHAR *)dta.LongName;
            w_strncpy(name, Path, MAX_FILENAME_LENGTH);
            w_strcat(name, L"\\");
            w_strcat(name, dn);
            if (dta.Attribute & AMP_CFS_ATTR_DIR) {
                dnum = AppLibFilter_NameToDnum(name);
                if (dnum > 0) {
                    if (pTable->Func->AddDirectory(pTable, dnum, name) != AMP_OK) {
                        Perror("Add directory error!");
                        return AMP_ERROR_GENERAL_ERROR;
                    }
                }
            }
            if (AmpCFS_NextDirEnt(&dta) != AMP_OK)
                break;
        }
    }
    return AMP_OK;
}

static int AppLibFilter_ScanFile(WCHAR *Path, AMP_DCF_TABLE_HDLR_s *pTable)
{
    WCHAR pattern[MAX_FILENAME_LENGTH];
    AMP_CFS_DTA dta;
    UINT32 i;

    // search sub-directory
    w_strncpy(pattern, Path, MAX_FILENAME_LENGTH);
    w_strcat(pattern, L"\\");
    for (i=0; i<DCF_FILE_PFX_LEN + DCF_FILE_NUM_LEN + DCF_FILE_SFX_LEN; i++)
        w_strcat(pattern, L"?");
    w_strcat(pattern, L"*");

    if (AmpCFS_FirstDirEnt(pattern, (AMP_CFS_ATTR_ARCH | AMP_CFS_ATTR_RDONLY | AMP_CFS_ATTR_NONE), &dta) == AMP_OK) {
        UINT32 id;
        WCHAR *fn;
        WCHAR name[MAX_FILENAME_LENGTH];
        while (TRUE) {
            if (dta.LongName[0] == 0)
                fn = (WCHAR *)dta.FileName;
            else
                fn = (WCHAR *)dta.LongName;
            w_strncpy(name, Path, MAX_FILENAME_LENGTH);
            w_strcat(name, L"\\");
            w_strcat(name, fn );
            id = AppLibFilter_NameToId(name);
            if (id > 0) {
                const UINT32 dnum = AppLibFilter_IdToDnum(id);
                const UINT32 fnum = AppLibFilter_IdToFnum(id);
                K_ASSERT(dnum > 0);
                K_ASSERT(fnum > 0);
                if (pTable->Func->AddFile(pTable, dnum, fnum, name) != AMP_OK) {
                    Perror("Add file error!");
                    return AMP_ERROR_GENERAL_ERROR;
                }
            }
            if (AmpCFS_NextDirEnt(&dta) != AMP_OK)
                break;
        }
    }
    return AMP_OK;
}

AMP_DCF_FILTER_s g_AppLibDcfFlt1 = {
    AppLibFilter_NameToDnum,
    AppLibFilter_NameToId,
    AppLibFilter_GetId,
    AppLibFilter_IdToDnum,
    AppLibFilter_IdToFnum,
    AppLibFilter_ScanDirectory,
    AppLibFilter_ScanFile
};
