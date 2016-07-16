/**
  * @file src/app/apps/flow/thumb/connectedcam/thumb_motion_func.c
  *
  *  Functions of Player Thumbnail Basic View
  *
  * History:
  *    2013/11/08 - [Martin Lai] created file
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


#include <apps/flow/thumb/thumb_motion.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <apps/flow/disp_param/disp_param_thumb.h>
#include <AmbaROM.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/widget/menu/widget.h>
#include <AmbaUtility.h>


/** display area of each scene*/
#define NUM_SCENE   (6)

static int thumb_motion_init(void)
{
    int ReturnValue = 0;

    thumb_motion.FileInfo.MediaRoot = APPLIB_DCF_MEDIA_DCIM;
    thumb_motion.TabNum = THUMB_MOTION_TAB_NUM;
    thumb_motion.TabCur = THUMB_MOTION_TAB_DCIM;
    app_status.ThumbnailModeMediaRoot = thumb_motion.FileInfo.MediaRoot;

    return ReturnValue;
}

static int thumb_motion_start(void)
{
    int ReturnValue = 0;

    UserSetting->SystemPref.SystemMode = APP_MODE_DEC;

    /** Set menus */
    AppMenu_Reset();
    #ifndef CONFIG_ECL_GUI
    AppMenu_RegisterTab(MENU_SETUP);
    #endif
    AppMenu_RegisterTab(MENU_ECL_PBACK);

    AppLibFormat_DemuxerInit();
    AppLibThmBasic_Init();

    // ToDo: need to remove to handler when iav completes the dsp cmd queue mechanism
    AppLibGraph_Init();

    return ReturnValue;
}

static int thumb_motion_stop(void)
{
    int ReturnValue = 0;
    /* Stop the warning message, because the warning could need to be updated. */
    thumb_motion.Func(THUMB_MOTION_WARNING_MSG_SHOW_STOP, 0, 0);

    /* Close the thumbnail player. */
    AppLibThmBasic_Deinit();
    /* Close the menu or dialog. */
    AppWidget_Off(WIDGET_ALL, WIDGET_HIDE_SILENT);
    APP_REMOVEFLAGS(app_thumb_motion.GFlags, APP_AFLAGS_POPUP);

    /* Disable the vout. */
    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));	
    AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
    AppLibDisp_ChanStop(DISP_CH_FCHAN);
	
    /* Hide GUI */
    thumb_motion.Gui(GUI_HIDE_ALL, 0, 0);
    thumb_motion.Gui(GUI_FLUSH, 0, 0);
    return ReturnValue;
}

static int thumb_motion_app_ready(void)
{
    int ReturnValue = 0;
#ifdef CONFIG_APP_ARD
    APP_APP_s *curapp;
#endif
    if (!APP_CHECKFLAGS(app_thumb_motion.GFlags, APP_AFLAGS_READY)) {
        APP_ADDFLAGS(app_thumb_motion.GFlags, APP_AFLAGS_READY);

        thumb_motion.Func(THUMB_MOTION_CHANGE_OSD, 0, 0);

        AppUtil_ReadyCheck(0);
        if (!APP_CHECKFLAGS(app_thumb_motion.GFlags, APP_AFLAGS_READY)) {
            /* The system could switch the current app to other in the function "AppUtil_ReadyCheck". */
            return ReturnValue;
        }
    }

    if (APP_CHECKFLAGS(app_thumb_motion.GFlags, APP_AFLAGS_READY)) {
#ifdef CONFIG_APP_ARD
        AppAppMgt_GetCurApp(&curapp);
        if(APP_REC_CAM == curapp->Previous) {
            thumb_motion.Func(THUMB_MOTION_START_DISP_PAGE, 1, 0);
        } else {
            thumb_motion.Func(THUMB_MOTION_START_DISP_PAGE, 0, 0);
        }
#else
        thumb_motion.Func(THUMB_MOTION_START_DISP_PAGE, 0, 0);
#endif
    }

    return ReturnValue;
}

static int thumb_motion_set_app_env(void)
{
    int ReturnValue = 0;

    thumb_motion.DispCol = 3;
    thumb_motion.DispRow = 2;
    thumb_motion.MaxPageItemNum = thumb_motion.DispCol * thumb_motion.DispRow;

    return ReturnValue;
}

static int thumb_motion_update_file_info(void)
{
    int ReturnValue = 0;
    int PageItemNum = 0;

    thumb_motion.FileInfo.PageCur = thumb_motion.FileInfo.FileCur / thumb_motion.MaxPageItemNum;
    thumb_motion.FileInfo.Pre_PageItemCur = thumb_motion.FileInfo.Pre_FileCur - (thumb_motion.FileInfo.Pre_FileCur / thumb_motion.MaxPageItemNum*thumb_motion.MaxPageItemNum);
    thumb_motion.FileInfo.PageItemCur = thumb_motion.FileInfo.FileCur - (thumb_motion.FileInfo.PageCur * thumb_motion.MaxPageItemNum);
    PageItemNum = thumb_motion.FileInfo.TotalFileNum - (thumb_motion.FileInfo.PageCur * thumb_motion.MaxPageItemNum);
    if (PageItemNum > thumb_motion.MaxPageItemNum) {
        thumb_motion.FileInfo.PageItemNum = thumb_motion.MaxPageItemNum;
    } else {
        thumb_motion.FileInfo.PageItemNum = PageItemNum;
    }

    return ReturnValue;
}

#ifdef CONFIG_APP_ARD
static int thumb_motion_init_file_info(int set_to_last)
#else
static int thumb_motion_init_file_info(void)
#endif
{
    UINT64 ReturnValue = 0;

    if (APP_CHECKFLAGS(app_status.ThumbnailModeConti, THUMB_MODE_INSERT_NEW_ACTIVE_CARD)) {
        thumb_motion.FileInfo.TotalFileNum = 0;
        app_status.ThumbnailModeConti = 0;
    } else {
        ReturnValue = AppLibCard_CheckStatus(0);
        if (ReturnValue == 0) {
            thumb_motion.FileInfo.TotalFileNum = AppLibStorageDmf_GetFileAmount(thumb_motion.FileInfo.MediaRoot, app_status.PlaybackType);

        } else {
            thumb_motion.FileInfo.TotalFileNum = 0;
        }
    }

    AmbaPrintColor(GREEN, "[app_thumb_motion] thumb_motion.FileInfo.TotalFileNum: %d", thumb_motion.FileInfo.TotalFileNum);
    if (thumb_motion.FileInfo.TotalFileNum > 0) {
        int i = 0;
#ifdef CONFIG_APP_ARD
        UINT64 CurrFilePos = 0;
#else
        UINT64 CurrFilePos;
#endif

#ifdef CONFIG_APP_ARD
        if(set_to_last==0){
#endif
            CurrFilePos = AppLibStorageDmf_GetCurrFilePos(thumb_motion.FileInfo.MediaRoot, app_status.PlaybackType);
#ifdef CONFIG_APP_ARD
        }else{
#else
        if (CurrFilePos == 0) {
#endif
            CurrFilePos = AppLibStorageDmf_GetLastFilePos(thumb_motion.FileInfo.MediaRoot, app_status.PlaybackType);
        }
        thumb_motion.CurFileObjID = CurrFilePos;
        ReturnValue = AppLibStorageDmf_GetFileName(thumb_motion.FileInfo.MediaRoot, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(CurrFilePos) + 1), CUR_OBJ(CurrFilePos), thumb_motion.CurFn);
        if (ReturnValue != 0) {
            ReturnValue = AppLibStorageDmf_GetFileName(thumb_motion.FileInfo.MediaRoot, ".JPG", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(CurrFilePos) + 1), CUR_OBJ(CurrFilePos), thumb_motion.CurFn);
        }
#ifdef CONFIG_APP_ARD
    if (ReturnValue != 0) {
          ReturnValue = AppLibStorageDmf_GetFileName(thumb_motion.FileInfo.MediaRoot, ".AAC", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(CurrFilePos) + 1), CUR_OBJ(CurrFilePos), thumb_motion.CurFn);
        if(ReturnValue !=0){
                AmbaPrintColor(RED, "[app_thumb_motion] ERROR @ get file name: %d", __LINE__);
        }
    }
#endif
        ReturnValue = AppLibStorageDmf_GetLastFilePos(thumb_motion.FileInfo.MediaRoot, app_status.PlaybackType);
        for (i = (thumb_motion.FileInfo.TotalFileNum - 1) ; i >= 0; i--) {
            if (ReturnValue == CurrFilePos) {
		   thumb_motion.FileInfo.Pre_FileCur = thumb_motion.FileInfo.FileCur;					
                thumb_motion.FileInfo.FileCur = i;	
                AmbaPrintColor(GREEN, "[thumb_motion_init_file_info] thumb_motion.FileInfo.FileCur: %d", thumb_motion.FileInfo.FileCur);
                break;
            } else {
                ReturnValue = AppLibStorageDmf_GetPrevFilePos(thumb_motion.FileInfo.MediaRoot, app_status.PlaybackType);
            }
        }
        AmbaPrintColor(GREEN, "[app_thumb_motion] thumb_motion.FileInfo.FileCur: %d, CurrFilePos =%d ,index = %d", thumb_motion.FileInfo.FileCur,CUR_OBJ(CurrFilePos),CUR_OBJ_IDX(CurrFilePos) );
        if (thumb_motion.FileInfo.FileCur < 0) {
            AmbaPrintColor(RED, "[app_thumb_motion] ERROR @ thumb_motion.FileInfo.FileCur: %d", thumb_motion.FileInfo.FileCur);
            thumb_motion.FileInfo.TotalFileNum = 0;
            thumb_motion.FileInfo.FileCur = 0;
            thumb_motion.FileInfo.Pre_FileCur = 0;			
            thumb_motion.FileInfo.TotalPageNum = 0;
            thumb_motion.FileInfo.PageCur = 0;
            thumb_motion.FileInfo.PageItemNum = 0;
            thumb_motion.FileInfo.PageItemCur = 0;
            thumb_motion.FileInfo.Pre_PageItemCur = 0;			
            thumb_motion.CurFileObjID = 0;
            memset(thumb_motion.CurFn, 0, MAX_FILENAME_LENGTH*sizeof(char));
        } else {
            thumb_motion.FileInfo.TotalPageNum = thumb_motion.FileInfo.TotalFileNum / thumb_motion.MaxPageItemNum;
            if ((thumb_motion.FileInfo.TotalFileNum - (thumb_motion.FileInfo.TotalPageNum * thumb_motion.MaxPageItemNum)) > 0) {
                thumb_motion.FileInfo.TotalPageNum++;
            }
            thumb_motion_update_file_info();
        }
        thumb_motion.Gui(GUI_WARNING_HIDE, 0, 0);
    } else {
        thumb_motion.FileInfo.TotalFileNum = 0;
        thumb_motion.FileInfo.Pre_FileCur = 0;				
        thumb_motion.FileInfo.FileCur = 0;
        thumb_motion.FileInfo.TotalPageNum = 0;
        thumb_motion.FileInfo.PageCur = 0;
        thumb_motion.FileInfo.PageItemNum = 0;
        //thumb_motion.FileInfo.PageItemCur = 0;
        //thumb_motion.FileInfo.Pre_PageItemCur = 0;		
        thumb_motion.CurFileObjID = 0;
        memset(thumb_motion.CurFn, 0, MAX_FILENAME_LENGTH*sizeof(char));
        if (!APP_CHECKFLAGS(app_thumb_motion.GFlags, APP_AFLAGS_POPUP)) {
            ReturnValue = AppLibCard_CheckStatus(0);
#ifdef CONFIG_APP_ARD
            if (ReturnValue == CARD_STATUS_NO_CARD) {
                thumb_motion.Func(THUMB_MOTION_WARNING_MSG_SHOW_START, GUI_WARNING_NO_CARD, 1);
            } else if (ReturnValue != CARD_STATUS_REFRESHING) {
                thumb_motion.Func(THUMB_MOTION_WARNING_MSG_SHOW_START, GUI_WARNING_NO_FILES, 1);
            }
#else
            if (ReturnValue != CARD_STATUS_REFRESHING) {
                thumb_motion.Func(THUMB_MOTION_WARNING_MSG_SHOW_START, GUI_WARNING_NO_FILES, 1);
            }
#endif
        }
        thumb_motion.Gui(GUI_FLUSH, 0, 0);
    }
    return CUR_OBJ(ReturnValue);
}

#ifdef CONFIG_APP_ARD
static int thumb_motion_start_disp_page(int set_to_last)
#else
static int thumb_motion_start_disp_page(void)
#endif
{
    int ReturnValue = 0;

#ifdef CONFIG_APP_ARD
    ReturnValue = thumb_motion_init_file_info(set_to_last);
#else
    ReturnValue = thumb_motion_init_file_info();
#endif

    if (thumb_motion.FileInfo.TotalFileNum > 0) {
        thumb_motion.Func(THUMB_MOTION_SHOW_ITEM, 0, 0);
        thumb_motion.Func(THUMB_MOTION_GUI_INIT_SHOW, 0, 0);
        thumb_motion.Func(THUMB_MOTION_SHOW_PAGE_INFO, 0, 0);
    } else {
        AppLibThmBasic_ClearScreen();
        thumb_motion.Func(THUMB_MOTION_GUI_INIT_SHOW, 0, 0);					
#ifdef CONFIG_APP_ARD
    {
        int CardStatus;
        CardStatus = AppLibCard_CheckStatus(0);

        if (CardStatus == CARD_STATUS_NO_CARD) {
            thumb_motion.Func(THUMB_MOTION_WARNING_MSG_SHOW_START, GUI_WARNING_NO_CARD, 1);
        } else if (CardStatus != CARD_STATUS_REFRESHING) {
            thumb_motion.Func(THUMB_MOTION_WARNING_MSG_SHOW_START, GUI_WARNING_NO_FILES, 1);
        }
    }
#else
        thumb_motion.Func(THUMB_MOTION_WARNING_MSG_SHOW_START, GUI_WARNING_NO_FILES, 1);
#endif
        thumb_motion.Gui(GUI_WARNING_SHOW, 0, 0);
        thumb_motion.Gui(GUI_FLUSH, 0, 0);
    }
    return ReturnValue;
}

/**
 *  @brief Show the thumbnails.
 *
 *  Show the thumbnails.
 *
 *  @return >=0 success, <0 failure
 */
static int thumb_motion_show_thumbanil(void)
{
    APPLIB_THUMB_BASIC_FILE_s Files[NUM_SCENE];
    int i = 0;
    int ReturnValue = 0;
#ifdef CONFIG_APP_ARD
    int isAAC = 0;
#endif
    APPLIB_THUMB_BASIC_TABLE_s LocactionInfo = {
            .NumScenes = thumb_motion.MaxPageItemNum,
            .AreaNormal = basicthm_disp_dchan_3x2_h_normal,
            .AreaFocused = basicthm_disp_dchan_3x2_h_focus
    };

    /** Last item in current page */
    AmbaPrint("[app_thumb_motion] FileInfo.PageItemNum = %d, FileInfo.PageItemCur = %d", thumb_motion.FileInfo.PageItemNum, thumb_motion.FileInfo.PageItemCur);
    if ( thumb_motion.FileInfo.PageItemCur != (thumb_motion.FileInfo.PageItemNum -1)) {
        for (i=0; i < (thumb_motion.FileInfo.PageItemNum -  thumb_motion.FileInfo.PageItemCur - 1); i++) {
            AppLibStorageDmf_GetNextFilePos(thumb_motion.FileInfo.MediaRoot, app_status.PlaybackType);
        }
    }

    for (i= (thumb_motion.DispCol*thumb_motion.DispRow -1) ; i >=0 ; i--) {
        if (i < thumb_motion.FileInfo.PageItemNum) {
            UINT64 ObjId = 0;
            Files[i].FileSource = 0;
            if (i == thumb_motion.FileInfo.PageItemCur) {
                Files[i].Focused = 1;
            } else {
                Files[i].Focused = 0;
            }
            if (i == (thumb_motion.FileInfo.PageItemNum-1)) {
                ObjId = AppLibStorageDmf_GetCurrFilePos(thumb_motion.FileInfo.MediaRoot, app_status.PlaybackType);
            } else {
                ObjId = AppLibStorageDmf_GetPrevFilePos(thumb_motion.FileInfo.MediaRoot, app_status.PlaybackType);
            }
            {
                APPLIB_MEDIA_INFO_s MediaInfo;
                char TempFn[MAX_FILENAME_LENGTH] = {0};
                ReturnValue = AppLibStorageDmf_GetFileName(thumb_motion.FileInfo.MediaRoot, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(ObjId) + 1), CUR_OBJ(ObjId), TempFn);
                if (ReturnValue != 0) {
                    ReturnValue = AppLibStorageDmf_GetFileName(thumb_motion.FileInfo.MediaRoot, ".JPG", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(ObjId) + 1), CUR_OBJ(ObjId), TempFn);
#ifdef CONFIG_APP_ARD
        if (ReturnValue != 0) {
            ReturnValue = AppLibStorageDmf_GetFileName(thumb_motion.FileInfo.MediaRoot, ".AAC", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(ObjId) + 1), CUR_OBJ(ObjId), TempFn);
            if(ReturnValue ==0){
                isAAC = 1;
            }
        }
#endif
                }
#ifdef CONFIG_APP_ARD
        if(isAAC){
            strcpy(Files[i].Filename, TempFn);
        }else{
#endif
                ReturnValue = AppLibFormat_GetMediaInfo(TempFn, &MediaInfo);
                if (ReturnValue == AMP_OK) {
                    strcpy(Files[i].Filename, TempFn);
                    AmbaPrintColor(GREEN, "[thumb_motion_show_thumbanil] Type: %d, Width[0]: %d, Height[0]: %d, Width[1]: %d, Height[1]: %d",
                                MediaInfo.MediaInfoType,
                                MediaInfo.MediaInfo.Image->Frame[0].Width,
                                MediaInfo.MediaInfo.Image->Frame[0].Height,
                                MediaInfo.MediaInfo.Image->Frame[1].Width,
                                MediaInfo.MediaInfo.Image->Frame[1].Height);
                } else {
                    /*To show invalid file.*/
                    AMBA_FS_STAT Fstat;
                    AmbaPrintColor(RED, "[app_thumb_motion] Invalid file %s ReturnValue = %d", TempFn, ReturnValue);
                    ReturnValue = AmbaFS_Stat((const char *)TempFn, &Fstat);
                    if ((ReturnValue == 0) && (Fstat.Size > 0)) {
                    } else {
                        UINT8 *Tmpbuf, *TmpbufRaw;
                        UINT32 FileSize;
                        const char InvalidFn[20] = {'i', 'n', 'v', 'a', 'l', 'i', 'd', '.', 'j', 'p','g','\0'};

                        ReturnValue = AmbaROM_FileExists(AMBA_ROM_SYS_DATA, "invalid.jpg");
                        if (ReturnValue < 0) {
                            AmbaPrintColor(RED,"[app_thumb_motion] invalid.jpg is not exist.");
                            return -1;
                        }
                        FileSize = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, InvalidFn, 0x0);
                        if (FileSize == 0) {
                            AmbaPrintColor(RED,"[app_thumb_motion] invalid.jpg is not exist.");
                            return -1;
                        } else {
                            //AmbaPrintColor(GREEN,"[app_thumb_motion] FileSize = %d.",FileSize);
                            ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&Tmpbuf, (void **)&TmpbufRaw, FileSize, 32);
                            if (ReturnValue < 0) {
                                AmbaPrintColor(RED,"[app_thumb_motion] Memory fail.");
                                return -1;
                            }
                        }
                        ReturnValue = AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, InvalidFn, Tmpbuf, FileSize, 0);
                        {
                            AMBA_FS_FILE *File = NULL;
                            char mode[3] = {'w','b','\0'};
                            File = AmbaFS_fopen((char const *)TempFn,(char const *) mode);
                            if (File != NULL) {
                                AmbaFS_fwrite(Tmpbuf, FileSize, 1, File);
                                AmbaFS_FSync(File);
                                AmbaFS_fclose(File);
                            } else {
                                AmbaPrintColor(RED,"[app_thumb_motion] can't open file [%s], maybe the card has been ejected!", TempFn);
                            }
                        }

                        AmbaKAL_BytePoolFree(TmpbufRaw);
                    }

                    strcpy(Files[i].Filename, TempFn);
                    //memset(&Files[i], 0, sizeof(APPLIB_THUMB_BASIC_FILE_s));
                }
            }
#ifdef CONFIG_APP_ARD
            }
#endif
        } else {
            memset(&Files[i], 0, sizeof(APPLIB_THUMB_BASIC_FILE_s));
        }
    }

    AppLibThmBasic_Show(&LocactionInfo,
                        thumb_motion.FileInfo.PageItemNum,
                        Files,
                        0);
    for (i=0; i < thumb_motion.FileInfo.PageItemCur; i++) {
        AppLibStorageDmf_GetNextFilePos(thumb_motion.FileInfo.MediaRoot, app_status.PlaybackType);
    }
    thumb_motion.Func(THUMB_MOTION_SHOW_PAGE_INFO, 0, 0);

#ifdef CONFIG_APP_ARD
    {
        AMBA_SCM_STATUS_s info;
        ReturnValue = AmbaSCM_GetSlotStatus (SCM_SLOT_SD0, &info);
        if (ReturnValue == -1 || info.CardPresent == 0) {
            /* Simulate HMSG_SD0_CARD_REMOVE process in AppHandler_Entry() */
            APP_MESSAGE_s Msg = {0};
            AmbaPrintColor (GREEN, "[******Simulate App Handler Card Remove******] %s():%d SD card not exists now !",
                            __func__, __LINE__);
            Msg.MessageID = AMSG_ERROR_CARD_REMOVED;
            Msg.MessageData[0] = SCM_SLOT_SD0;
            Msg.MessageData[1] = SCM_CARD_EJECTED;
            AppHandler_CardRemove (Msg);
        }
    }
#endif

    return 0;
}


static int thumb_motion_show_page_info(void)
{
    int ReturnValue = 0;
    WCHAR FileName[MAX_FILENAME_LENGTH];
    AmbaUtility_Ascii2Unicode(thumb_motion.CurFn, FileName);
    if (thumb_motion.FileInfo.TotalFileNum > 0) {				
        thumb_motion.Gui(GUI_THUMB_SELECT_HIDE,thumb_motion.FileInfo.Pre_PageItemCur, 0);						
        thumb_motion.Gui(GUI_FILENAME_UPDATE, (UINT32)FileName, GUI_PB_FN_STYLE_HYPHEN);
        thumb_motion.Gui(GUI_FILENAME_SHOW, 0, 0);
        thumb_motion.Gui(GUI_THUMB_SELECT_SHOW,thumb_motion.FileInfo.PageItemCur, 0);				
    }
    thumb_motion.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int thumb_motion_get_prev_file(int file_shift_offset)
{
    int ReturnValue = 0;
    int i = 0;
    UINT64 ObjId = 0;

    AmbaPrintColor(GREEN, "[thumb_motion_get_prev_file]");
    for (i = 0; i < file_shift_offset ; i++) {
	  thumb_motion.FileInfo.Pre_FileCur = thumb_motion.FileInfo.FileCur;
        thumb_motion.FileInfo.FileCur --;
        if (thumb_motion.FileInfo.FileCur < 0) {
            ObjId = AppLibStorageDmf_GetLastFilePos(thumb_motion.FileInfo.MediaRoot, app_status.PlaybackType);
            thumb_motion.FileInfo.FileCur = thumb_motion.FileInfo.TotalFileNum - 1;
        } else {
            ObjId = AppLibStorageDmf_GetPrevFilePos(thumb_motion.FileInfo.MediaRoot, app_status.PlaybackType);
        }
    }
    thumb_motion.CurFileObjID = ObjId;
    ReturnValue = AppLibStorageDmf_GetFileName(thumb_motion.FileInfo.MediaRoot, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(ObjId) + 1), CUR_OBJ(ObjId), thumb_motion.CurFn);
    if (ReturnValue != 0) {
        ReturnValue = AppLibStorageDmf_GetFileName(thumb_motion.FileInfo.MediaRoot, ".JPG", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(ObjId) + 1), CUR_OBJ(ObjId), thumb_motion.CurFn);
    }

#ifdef CONFIG_APP_ARD
    if (ReturnValue != 0) {
       ReturnValue = AppLibStorageDmf_GetFileName(thumb_motion.FileInfo.MediaRoot, ".AAC", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(ObjId) + 1), CUR_OBJ(ObjId), thumb_motion.CurFn);
    }
#endif
    return ReturnValue;
}

static int thumb_motion_get_next_file(int file_shift_offset)
{
    int ReturnValue = 0;
    int i = 0;
    UINT64 ObjId = 0;

    AmbaPrintColor(GREEN, "[thumb_motion_get_next_file], file_shift_offset: %d, thumb_motion.FileInfo.FileCur: %d", file_shift_offset, thumb_motion.FileInfo.FileCur);
    for (i = 0; i < file_shift_offset ; i++) {
	  thumb_motion.FileInfo.Pre_FileCur = thumb_motion.FileInfo.FileCur;		
        thumb_motion.FileInfo.FileCur++;
        if (thumb_motion.FileInfo.FileCur >= thumb_motion.FileInfo.TotalFileNum ) {
            ObjId = AppLibStorageDmf_GetFirstFilePos(thumb_motion.FileInfo.MediaRoot, app_status.PlaybackType);
            thumb_motion.FileInfo.FileCur = 0;
        } else {
            ObjId = AppLibStorageDmf_GetNextFilePos(thumb_motion.FileInfo.MediaRoot, app_status.PlaybackType);
        }
    }

    thumb_motion.CurFileObjID = ObjId;
    ReturnValue = AppLibStorageDmf_GetFileName(thumb_motion.FileInfo.MediaRoot, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(ObjId) + 1), CUR_OBJ(ObjId), thumb_motion.CurFn);
    if (ReturnValue != 0) {
        ReturnValue = AppLibStorageDmf_GetFileName(thumb_motion.FileInfo.MediaRoot, ".JPG", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(ObjId) + 1), CUR_OBJ(ObjId), thumb_motion.CurFn);
#ifdef CONFIG_APP_ARD
    if (ReturnValue != 0) {
        ReturnValue = AppLibStorageDmf_GetFileName(thumb_motion.FileInfo.MediaRoot, ".AAC", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, app_status.PlaybackType, (CUR_OBJ_IDX(ObjId) + 1), CUR_OBJ(ObjId), thumb_motion.CurFn);
        if(ReturnValue !=0){
                AmbaPrintColor(RED, "[app_thumb_motion] ERROR @ get file name: %d", __LINE__);
        }
    }
#endif
    }
    return ReturnValue;
}

static int thumb_motion_shift_tab(UINT32 param1)
{
    int ReturnValue = 0;
#if 0
    int i = 0;
    for (i=0;i< thumb_motion.TabNum ;i++) {
        switch (param1) {
        case THUMB_MOTION_TAB_VIDEO:
            thumb_motion.FileInfo.MediaRoot = APPLIB_DCF_MEDIA_VIDEO;
            break;
        case THUMB_MOTION_TAB_PHOTO:
            thumb_motion.FileInfo.MediaRoot = APPLIB_DCF_MEDIA_IMAGE;
            break;
        case THUMB_MOTION_TAB_DCIM:
            thumb_motion.FileInfo.MediaRoot = APPLIB_DCF_MEDIA_DCIM;
            break;
        default:
            break;
        }
        ReturnValue = AppLibStorageDmf_GetFileAmount(thumb_motion.FileInfo.MediaRoot, DCIM_HDLR);
        if (ReturnValue > 0) {
            /*
             * When the target media tab has files, break the loop and
             * switch to the target tab.
             */
            break;
        } else {
            param1++;
            if (param1 == thumb_motion.TabNum) {
                param1 = THUMB_MOTION_TAB_VIDEO;
            }
        }
    }
#else
    switch (param1) {
#ifndef CONFIG_APP_ARD
    case THUMB_MOTION_TAB_VIDEO:
        thumb_motion.FileInfo.MediaRoot = APPLIB_DCF_MEDIA_VIDEO;
        break;
    case THUMB_MOTION_TAB_PHOTO:
        thumb_motion.FileInfo.MediaRoot = APPLIB_DCF_MEDIA_IMAGE;
        break;
#endif
    case THUMB_MOTION_TAB_DCIM:
        thumb_motion.FileInfo.MediaRoot = APPLIB_DCF_MEDIA_DCIM;
        break;
    default:
        break;
    }
#endif
    app_status.ThumbnailModeMediaRoot = thumb_motion.FileInfo.MediaRoot;
    thumb_motion.TabCur = param1;
#ifdef CONFIG_APP_ARD
    thumb_motion.Gui(GUI_TAB_UPDATE, app_status.PlaybackType,0);
#else
    thumb_motion.Gui(GUI_TAB_UPDATE, param1, 0);
#endif
    thumb_motion.Gui(GUI_MEDIA_INFO_HIDE, 0, 0);
    thumb_motion.Gui(GUI_FILENAME_HIDE, 0, 0);
    thumb_motion.Gui(GUI_FLUSH, 0, 0);
#ifdef CONFIG_APP_ARD
        thumb_motion.Func(THUMB_MOTION_START_DISP_PAGE, 1, 0);
#else
        thumb_motion.Func(THUMB_MOTION_START_DISP_PAGE, 0, 0);
#endif
    return ReturnValue;
}

static int thumb_motion_card_removed(void)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_thumb_motion.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_thumb_motion.Flags, THUMB_MOTION_DELETE_FILE_RUN);
        AppWidget_Off(WIDGET_ALL, 0);
    }
    thumb_motion.Func(THUMB_MOTION_WARNING_MSG_SHOW_STOP, 0, 0);
    thumb_motion.Gui(GUI_CARD_UPDATE, GUI_NO_CARD, 0);
    thumb_motion.Gui(GUI_MEDIA_INFO_HIDE, 0, 0);
    thumb_motion.Gui(GUI_FILENAME_HIDE, 0, 0);
    thumb_motion.Gui(GUI_FLUSH, 0, 0);
#ifdef CONFIG_APP_ARD
    thumb_motion.Func(THUMB_MOTION_START_DISP_PAGE, 1, 0);
#else
    thumb_motion.Func(THUMB_MOTION_START_DISP_PAGE, 0, 0);
#endif

    return ReturnValue;
}

static int thumb_motion_card_error_removed(void)
{
    int ReturnValue = 0;

    APP_REMOVEFLAGS(app_thumb_motion.GFlags, APP_AFLAGS_BUSY);
    thumb_motion.Func(THUMB_MOTION_CARD_REMOVED, 0, 0);

    return ReturnValue;
}

static int thumb_motion_card_new_insert(int param1)
{
    int ReturnValue = 0;

    /* Remove old card.*/
    thumb_motion.Func(THUMB_MOTION_CARD_REMOVED, 0, 0);
    app_status.ThumbnailModeConti = THUMB_MODE_INSERT_NEW_ACTIVE_CARD;
    AppLibCard_StatusSetBlock(param1, 0);
    AppLibComSvcAsyncOp_CardInsert(AppLibCard_GetSlot(param1));

    return ReturnValue;
}

static int thumb_motion_card_storage_idle(int param1)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_status.ThumbnailModeConti, THUMB_MODE_INSERT_NEW_ACTIVE_CARD)) {
        DBGMSG("Not stop player finished, re-send the storage idle");
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_STORAGE_IDLE, param1, 1);
    } else {
        thumb_motion.Func(THUMB_MOTION_SET_FILE_INDEX, 0, 0);
        AppUtil_CheckCardParam(0);
        if (!APP_CHECKFLAGS(app_thumb_motion.GFlags, APP_AFLAGS_READY)) {
            return ReturnValue;/**<  App switched out*/
        }
        thumb_motion.Gui(GUI_CARD_UPDATE, GUI_CARD_READY, 0);
        thumb_motion.Gui(GUI_FLUSH, 0, 0);
#ifdef CONFIG_APP_ARD
    AppLibThmBasic_Deinit();
    AmbaKAL_TaskSleep(50);
    AppLibThmBasic_Init();
        thumb_motion.Func(THUMB_MOTION_START_DISP_PAGE, 1, 0);
#else
        thumb_motion.Func(THUMB_MOTION_START_DISP_PAGE, 0, 0);
#endif
    }

    return ReturnValue;
}

static int thumb_motion_file_id_update(UINT32 FileID)
{
    int ReturnValue = 0;
    /**update last id for serial mode if new filw close*/
    if (FileID > UserSetting->SetupPref.DmfMixLastIdx || UserSetting->SetupPref.DMFMode == DMF_MODE_RESET) {
        UserSetting->SetupPref.DmfMixLastIdx = FileID;
    }
    return ReturnValue;
}
static int thumb_motion_set_delete_file_mode(int param)
{
    int ReturnValue = 0;

    APP_ADDFLAGS(app_thumb_motion.Flags, THUMB_MOTION_DELETE_FILE_RUN);
    thumb_motion.DeleteFileMode = param;

    return ReturnValue;
}

static int thumb_motion_select_delete_file_mode(void)
{
    int ReturnValue = 0;

    ReturnValue = AppLibCard_CheckStatus(CARD_CHECK_DELETE);
    if (ReturnValue == 0) {
        AppMenuQuick_SetItem(MENU_PBACK, MENU_PBACK_DELETE_FILE);
        ReturnValue = AppWidget_On(WIDGET_MENU_QUICK, 0);
        APP_ADDFLAGS(app_thumb_motion.GFlags, APP_AFLAGS_POPUP);
    } else if (ReturnValue == CARD_STATUS_WP_CARD) {
        thumb_motion.Func(THUMB_MOTION_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_PROTECTED, 0);
        AmbaPrintColor(RED,"[app_thumb_motion] WARNING_CARD_PROTECTED");
    } else {
        AmbaPrintColor(RED,"[app_thumb_motion] WARNING_CARD_ERROR ReturnValue = %d", ReturnValue);
    }

    return ReturnValue;
}

static int thumb_motion_dialog_del_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;
    switch (sel) {
    case DIALOG_SEL_YES:
        thumb_motion.Func(THUMB_MOTION_DELETE_FILE, CUR_OBJ(thumb_motion.CurFileObjID), 0);
        break;
    case DIALOG_SEL_NO:
    default:
        break;
    }

    return ReturnValue;
}

static int thumb_motion_dialog_del_all_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (sel) {
    case DIALOG_SEL_YES:
#ifdef CONFIG_APP_ARD
        thumb_motion.Func(THUMB_MOTION_DELETE_FILE, app_status.PlaybackType, 1);
#else
        thumb_motion.Func(THUMB_MOTION_DELETE_FILE, 0, 1);
#endif
        break;
    case DIALOG_SEL_NO:
    default:
        break;
    }

    return ReturnValue;
}

static int thumb_motion_delete_file_dialog_show(void)
{
    int ReturnValue = 0;

    if (thumb_motion.DeleteFileMode) {
        ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_Y_N, DIALOG_SUB_DEL_ALL, thumb_motion_dialog_del_all_handler);
    } else {
        ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_Y_N, DIALOG_SUB_DEL, thumb_motion_dialog_del_handler);
    }
    ReturnValue = AppWidget_On(WIDGET_DIALOG, 0);
    APP_ADDFLAGS(app_thumb_motion.GFlags, APP_AFLAGS_POPUP);

    return ReturnValue;
}

static int thumb_motion_delete_file(UINT32 FileObjID, UINT32 param)
{
    int ReturnValue = 0;
    char drive = 'A';    

    if (param) {
#ifdef CONFIG_APP_ARD
        AppLibComSvcAsyncOp_DmfFastFdelAll(thumb_motion.FileInfo.MediaRoot, app_status.PlaybackType);
#else
        AppLibComSvcAsyncOp_DmfFastFdelAll(thumb_motion.FileInfo.MediaRoot, 0);
#endif
    } else {
#ifdef CONFIG_APP_ARD
        if( thumb_motion.FileInfo.FileCur == (thumb_motion.FileInfo.TotalFileNum-1) )
            thumb_motion.DeleteFlag = 1;
        else
            thumb_motion.DeleteFlag = 0;
#endif
        AppLibComSvcAsyncOp_DmfFdel(thumb_motion.FileInfo.MediaRoot, FileObjID, app_status.PlaybackType);
        //AmbaPrintColor(YELLOW,"CurFile=%d,TotalFile=%d",thumb_motion.FileInfo.FileCur,thumb_motion.FileInfo.TotalFileNum);
    }

    APP_ADDFLAGS(app_thumb_motion.GFlags, APP_AFLAGS_BUSY);

    thumb_motion.Func(THUMB_MOTION_WARNING_MSG_SHOW_START, GUI_WARNING_PROCESSING, 1);
    
    //drive += AppLibCard_GetActiveSlot();
    //ReturnValue = AppLibStorageDmf_Refresh(drive);      //modify by ouyangjian 
    return ReturnValue;
}

static int thumb_motion_delete_file_complete(int param1, int param2)
{
    int ReturnValue = 0;

    APP_REMOVEFLAGS(app_thumb_motion.GFlags, APP_AFLAGS_BUSY);
    thumb_motion.Gui(GUI_WARNING_HIDE, 0, 0);
    thumb_motion.Gui(GUI_FLUSH, 0, 0);
    if (((int)param1) < 0) {
        DBGMSG("[app_thumb_motion] Received AMSG_MGR_MSG_OP_FAILED");
        AmbaPrintColor(RED,"[app_thumb_motion] Delete files failed: %d", param2);
    } else {
        DBGMSG("[app_thumb_motion] Received AMSG_MGR_MSG_OP_DONE");
        /** page update */
#ifdef CONFIG_APP_ARD
        thumb_motion.Func(THUMB_MOTION_START_DISP_PAGE, thumb_motion.DeleteFlag, 0);
#else
        thumb_motion.Func(THUMB_MOTION_START_DISP_PAGE, 0, 0);
#endif
    }

    return ReturnValue;
}

static int thumb_motion_widget_closed(int param1, int param2)
{
    int ReturnValue = 0;

    if (APP_CHECKFLAGS(app_thumb_motion.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_thumb_motion.GFlags, APP_AFLAGS_POPUP);
        if (APP_CHECKFLAGS(app_thumb_motion.Flags, THUMB_MOTION_DELETE_FILE_RUN)) {
            APP_REMOVEFLAGS(app_thumb_motion.Flags, THUMB_MOTION_DELETE_FILE_RUN);
            if (thumb_motion.FileInfo.TotalFileNum > 0) {
                thumb_motion.Func(THUMB_MOTION_DELETE_FILE_DIALOG_SHOW, param1, param2);
            }
        }
    }
    if (thumb_motion.FileInfo.TotalFileNum <= 0) {
#ifdef CONFIG_APP_ARD
        {
            int CardStatus;
            CardStatus = AppLibCard_CheckStatus(0);

            if (CardStatus == CARD_STATUS_NO_CARD) {
                thumb_motion.Func(THUMB_MOTION_WARNING_MSG_SHOW_START, GUI_WARNING_NO_CARD, 1);
            } else if (CardStatus != CARD_STATUS_REFRESHING) {
                thumb_motion.Func(THUMB_MOTION_WARNING_MSG_SHOW_START, GUI_WARNING_NO_FILES, 1);
            }
        }
#else
        thumb_motion.Func(THUMB_MOTION_WARNING_MSG_SHOW_START, GUI_WARNING_NO_FILES, 1);
#endif
        thumb_motion.Gui(GUI_WARNING_SHOW, 0, 0);
        thumb_motion.Gui(GUI_FLUSH, 0, 0);
    }
#ifdef CONFIG_APP_ARD
    else{
        thumb_motion.Func(THUMB_MOTION_SHOW_PAGE_INFO, 0, 0);
    }
#endif

    return ReturnValue;
}

static int thumb_motion_update_fchan_vout(UINT32 msg)
{
    int ReturnValue = 0;

    switch (msg) {
    case HMSG_HDMI_INSERT_SET:
    case HMSG_HDMI_INSERT_CLR:
        AppLibSysVout_SetJackHDMI(app_status.HdmiPluginFlag);
        break;
    case HMSG_CS_INSERT_SET:
    case HMSG_CS_INSERT_CLR:
        AppLibSysVout_SetJackCs(app_status.CompositePluginFlag);
        break;
    default:
        AmbaPrint("[app_thumb_motion] Vout no changed");
        return 0;
        break;
    }
    ReturnValue = AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
    if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_CHANGE)) {
        AmbaPrint("[app_thumb_motion] Display FCHAN has no changed");
    } else {
        AppLibThmBasic_Deinit();
        if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_DEVICE)) {
            AppLibGraph_DisableDraw(GRAPH_CH_FCHAN);
            AppLibDisp_ChanStop(DISP_CH_FCHAN);
            AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            AppLibGraph_DeactivateWindow(GRAPH_CH_FCHAN);
            AppLibDisp_FlushWindow(DISP_CH_FCHAN);

            app_status.LockDecMode = 0;
        } else {
            AppLibDisp_ConfigMode(DISP_CH_FCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
            AppLibDisp_SetupChan(DISP_CH_FCHAN);
            AppLibDisp_ChanStart(DISP_CH_FCHAN);
            {
                AMP_DISP_WINDOW_CFG_s Window;
                AMP_DISP_INFO_s DispDev = {0};

                memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));

                ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
                if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
                    DBGMSG("[app_thumb_motion] FChan Disable. Disable the fchan window");
                    AppLibGraph_DisableDraw(GRAPH_CH_FCHAN);
                    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
                    AppLibGraph_DeactivateWindow(GRAPH_CH_FCHAN);
                    AppLibDisp_FlushWindow(DISP_CH_FCHAN);
                    app_status.LockDecMode = 0;
                } else {
                    /** FCHAN window*/
                    AppLibGraph_EnableDraw(GRAPH_CH_FCHAN);
                    AppLibDisp_GetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
                    Window.Source = AMP_DISP_DEC;
                    Window.SourceDesc.Dec.DecHdlr = 0;
                    Window.CropArea.Width = 0;
                    Window.CropArea.Height = 0;
                    Window.CropArea.X = 0;
                    Window.CropArea.Y = 0;
                    Window.TargetAreaOnPlane.Width = DispDev.DeviceInfo.VoutWidth;
                    Window.TargetAreaOnPlane.Height = DispDev.DeviceInfo.VoutHeight;
                    Window.TargetAreaOnPlane.X = 0;
                    Window.TargetAreaOnPlane.Y = 0;
                    AppLibDisp_SetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
                    AppLibDisp_ActivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
                    AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN);
                    AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN);
                    AppLibDisp_FlushWindow(DISP_CH_FCHAN);

                    thumb_motion.Gui(GUI_SET_LAYOUT, 0, 0);
                    thumb_motion.Gui(GUI_FLUSH, 0, 0);

                    if (app_status.FchanDecModeOnly == 1) {
                        app_status.LockDecMode = 1;
                    }
                }
            }
        }
        /* Redraw the screen. */
        AppLibThmBasic_Init();
        thumb_motion.Func(THUMB_MOTION_START_DISP_PAGE, 0, 0);
    }

    return ReturnValue;
}

static int thumb_motion_change_display(void)
{
    int ReturnValue = 0;

    AppLibDisp_SelectDevice(DISP_CH_FCHAN | DISP_CH_DCHAN, DISP_ANY_DEV);
    AppLibDisp_ConfigMode(DISP_CH_FCHAN | DISP_CH_DCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
    AppLibDisp_SetupChan(DISP_CH_FCHAN | DISP_CH_DCHAN);
    AppLibDisp_ChanStart(DISP_CH_FCHAN | DISP_CH_DCHAN);
    {
        AMP_DISP_WINDOW_CFG_s Window;
        AMP_DISP_INFO_s DispDev = {0};

        memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));

        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_thumb_motion] FChan Disable. Disable the fchan window");
            AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            AppLibGraph_DeactivateWindow(GRAPH_CH_FCHAN);
            AppLibGraph_DisableDraw(GRAPH_CH_FCHAN);
        } else {
            /** FCHAN window*/
            AppLibDisp_GetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
            Window.Source = AMP_DISP_DEC;
            Window.SourceDesc.Dec.DecHdlr = 0;
            Window.CropArea.Width = 0;
            Window.CropArea.Height = 0;
            Window.CropArea.X = 0;
            Window.CropArea.Y = 0;
            Window.TargetAreaOnPlane.Width = DispDev.DeviceInfo.VoutWidth;
            Window.TargetAreaOnPlane.Height = DispDev.DeviceInfo.VoutHeight;//  interlance should be consider in MW
            Window.TargetAreaOnPlane.X = 0;
            Window.TargetAreaOnPlane.Y = 0;
            AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN);
            AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN);
            AppLibDisp_SetWindowConfig(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0), &Window);
            AppLibDisp_ActivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            AppLibGraph_EnableDraw(GRAPH_CH_FCHAN);
        }

        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_thumb_motion] DChan Disable. Disable the Dchan window");
            AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
            AppLibGraph_DeactivateWindow(GRAPH_CH_DCHAN);
            AppLibGraph_DisableDraw(GRAPH_CH_DCHAN);
        } else {
            /** DCHAN window*/
            AppLibDisp_GetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
            Window.Source = AMP_DISP_DEC;
            Window.SourceDesc.Dec.DecHdlr = 0;
            Window.CropArea.Width = 0;
            Window.CropArea.Height = 0;
            Window.CropArea.X = 0;
            Window.CropArea.Y = 0;
            Window.TargetAreaOnPlane.Width = DispDev.DeviceInfo.VoutWidth;
            Window.TargetAreaOnPlane.Height = DispDev.DeviceInfo.VoutHeight;
            Window.TargetAreaOnPlane.X = 0;
            Window.TargetAreaOnPlane.Y = 0;
            AppLibGraph_SetWindowConfig(GRAPH_CH_DCHAN);
            AppLibGraph_ActivateWindow(GRAPH_CH_DCHAN);
            AppLibDisp_SetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
            AppLibDisp_ActivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
            AppLibGraph_EnableDraw(GRAPH_CH_DCHAN);
        }
        AppLibDisp_FlushWindow(DISP_CH_FCHAN | DISP_CH_DCHAN);
    }

    return ReturnValue;
}

static int thumb_motion_change_osd(void)
{
    int ReturnValue = 0;

    thumb_motion.Gui(GUI_SET_LAYOUT, 0, 0);
    thumb_motion.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int thumb_motion_usb_connect(void)
{
    int ReturnValue = 0;

    switch (UserSetting->SetupPref.USBMode) {
    case USB_MODE_AMAGE:
        AppAppMgt_SwitchApp(APP_USB_AMAGE);
        break;
    case USB_MODE_MSC:
    default:
        AppAppMgt_SwitchApp(APP_USB_MSC);
        break;
    }

    return ReturnValue;
}

static int thumb_motion_start_show_gui(void)
{
    int ReturnValue = 0;
    int GuiParam = 0;

    // thumb_motion.Gui(GUI_APP_ICON_SHOW, 0, 0);
    thumb_motion.Gui(GUI_FRAME_SHOW, 0, 0);
    thumb_motion.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
    thumb_motion.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    ReturnValue = AppLibCard_CheckStatus(0);
    if (ReturnValue == CARD_STATUS_NO_CARD) {
        GuiParam = GUI_NO_CARD;
    } else {
        GuiParam = GUI_CARD_READY;
    }
    thumb_motion.Gui(GUI_CARD_UPDATE, GuiParam, 0);
    thumb_motion.Gui(GUI_CARD_SHOW, 0, 0);
#ifdef CONFIG_APP_ARD
    thumb_motion.Gui(GUI_TAB_UPDATE, app_status.PlaybackType,0);
    thumb_motion.Gui(GUI_MODE_NAME_UPDATE, app_status.PlaybackType,0);
#else
    thumb_motion.Gui(GUI_TAB_UPDATE, thumb_motion.TabCur, 0);
#endif
    if(thumb_motion.FileInfo.TotalFileNum == 0)
        thumb_motion.Gui(GUI_THUMB_SELECT_HIDE,thumb_motion.FileInfo.PageItemCur, 0);	
    thumb_motion.Gui(GUI_TAB_SHOW, 0, 0);
    thumb_motion.Gui(GUI_MODE_NAME_SHOW, 0, 0);
    thumb_motion.Gui(GUI_DEL_SHOW, 0, 0);
    thumb_motion.Gui(GUI_PROTECT_SHOW, 0, 0);
    thumb_motion.Gui(GUI_MEDIA_INFO_HIDE, 0, 0);
    thumb_motion.Gui(GUI_FILENAME_HIDE, 0, 0);
    thumb_motion.Gui(GUI_FLUSH, 0, 0);
    return 0;
}

static int thumb_motion_update_bat_power_status(int param1)
{
    int ReturnValue = 0;

    /* Update the gui of power's status. */
    if (param1 == 0) {
        /*Hide the battery gui.*/
        thumb_motion.Gui(GUI_POWER_STATE_HIDE, GUI_HIDE_POWER_EXCEPT_DC, 0);
    } else if (param1 == 1) {
        /*Update the battery gui.*/
        thumb_motion.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
        thumb_motion.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    } else if (param1 == 2) {
        /*Reset the battery and power gui.*/
        thumb_motion.Gui(GUI_POWER_STATE_HIDE, 0, 0);
        thumb_motion.Gui(GUI_POWER_STATE_UPDATE, app_status.PowerType, app_status.BatteryState);
        thumb_motion.Gui(GUI_POWER_STATE_SHOW, app_status.PowerType, app_status.BatteryState);
    }
    thumb_motion.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static void thumb_motion_warning_timer_handler(int eid)
{
    static int blink_count = 0;

    if (eid == TIMER_UNREGISTER) {
        blink_count = 0;
        return;
    }

    blink_count++;

    if (blink_count & 0x01) {
        thumb_motion.Gui(GUI_WARNING_HIDE, 0, 0);
    } else {
        thumb_motion.Gui(GUI_WARNING_SHOW, 0, 0);
    }

    if (blink_count >= 5) {
        APP_REMOVEFLAGS(app_thumb_motion.Flags, THUMB_MOTION_WARNING_MSG_RUN);
        AppLibComSvcTimer_Unregister(TIMER_2HZ, thumb_motion_warning_timer_handler);
        thumb_motion.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    thumb_motion.Gui(GUI_FLUSH, 0, 0);

}

static int thumb_motion_warning_msg_show(int enable, int param1, int param2)
{
    int ReturnValue = 0;

    if (enable) {
        if (param2) {
#ifdef CONFIG_APP_ARD
            thumb_motion.Gui(GUI_WARNING_UPDATE, param1, param2);
#else
            thumb_motion.Gui(GUI_WARNING_UPDATE, param1, 0);
#endif
            thumb_motion.Gui(GUI_WARNING_SHOW, 0, 0);
        } else {
            if (!APP_CHECKFLAGS(app_thumb_motion.Flags, THUMB_MOTION_WARNING_MSG_RUN)) {
                APP_ADDFLAGS(app_thumb_motion.Flags, THUMB_MOTION_WARNING_MSG_RUN);
                thumb_motion.Gui(GUI_WARNING_UPDATE, param1, 0);
                thumb_motion.Gui(GUI_WARNING_SHOW, 0, 0);
                AppLibComSvcTimer_Register(TIMER_2HZ, thumb_motion_warning_timer_handler);
            }
        }
    } else {
        if (APP_CHECKFLAGS(app_thumb_motion.Flags, THUMB_MOTION_WARNING_MSG_RUN)) {
            APP_REMOVEFLAGS(app_thumb_motion.Flags, THUMB_MOTION_WARNING_MSG_RUN);
            AppLibComSvcTimer_Unregister(TIMER_2HZ, thumb_motion_warning_timer_handler);
        }
        thumb_motion.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    thumb_motion.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

/**
 *  @brief The functions of this application
 *
 *  The functions of this application
 *
 *  @param[in] funcId Function id
 *  @param[in] param1 first parameter
 *  @param[in] param2 second parameter
 *
 *  @return >=0 success, <0 failure
 */
int thumb_motion_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case THUMB_MOTION_INIT:
        ReturnValue = thumb_motion_init();
        break;
    case THUMB_MOTION_START:
        ReturnValue = thumb_motion_start();
        break;
    case THUMB_MOTION_STOP:
        ReturnValue = thumb_motion_stop();
        break;
    case THUMB_MOTION_APP_READY:
        ReturnValue = thumb_motion_app_ready();
        break;
    case THUMB_MOTION_SET_APP_ENV:
        ReturnValue = thumb_motion_set_app_env();
        break;
    case THUMB_MOTION_START_DISP_PAGE:
#ifdef CONFIG_APP_ARD
        ReturnValue = thumb_motion_start_disp_page(param1);
#else
        ReturnValue = thumb_motion_start_disp_page();
#endif
        break;
    case THUMB_MOTION_SHOW_ITEM:
        ReturnValue = thumb_motion_show_thumbanil();
        break;
    case THUMB_MOTION_SHOW_PAGE_INFO:
        ReturnValue = thumb_motion_show_page_info();
        break;
    case THUMB_MOTION_SHIFT_FILE_TO_PREV:
        ReturnValue = thumb_motion_get_prev_file(param1);
        ReturnValue = thumb_motion_update_file_info();
        thumb_motion.Func(THUMB_MOTION_SHOW_ITEM, 0, 0);
        break;
    case THUMB_MOTION_SHIFT_FILE_TO_NEXT:
        ReturnValue = thumb_motion_get_next_file(param1);
        ReturnValue = thumb_motion_update_file_info();
        thumb_motion.Func(THUMB_MOTION_SHOW_ITEM, 0, 0);
        break;
    case THUMB_MOTION_SHIFT_TAB:
        ReturnValue = thumb_motion_shift_tab(param1);
        break;
    case THUMB_MOTION_CARD_REMOVED:
        ReturnValue = thumb_motion_card_removed();
        break;
    case THUMB_MOTION_CARD_ERROR_REMOVED:
        ReturnValue = thumb_motion_card_error_removed();
        break;
    case THUMB_MOTION_CARD_NEW_INSERT:
        ReturnValue = thumb_motion_card_new_insert(param1);
        break;
    case THUMB_MOTION_CARD_STORAGE_IDLE:
        ReturnValue = thumb_motion_card_storage_idle(param1);
        break;
    case THUMB_MOTION_FILE_ID_UPDATE:
        ReturnValue = thumb_motion_file_id_update(param1);
        break;
    case THUMB_MOTION_SET_DELETE_FILE_MODE:
        ReturnValue = thumb_motion_set_delete_file_mode(param2);
        break;
    case THUMB_MOTION_SELECT_DELETE_FILE_MODE:
        ReturnValue = thumb_motion_select_delete_file_mode();
        break;
    case THUMB_MOTION_DELETE_FILE_DIALOG_SHOW:
        ReturnValue = thumb_motion_delete_file_dialog_show();
        break;
    case THUMB_MOTION_DELETE_FILE:
        ReturnValue = thumb_motion_delete_file(param1,param2);
        break;
    case THUMB_MOTION_DELETE_FILE_COMPLETE:
        ReturnValue = thumb_motion_delete_file_complete(param1, param2);
        break;
    case THUMB_MOTION_STATE_WIDGET_CLOSED:
        ReturnValue = thumb_motion_widget_closed(param1, param2);
        break;
    case THUMB_MOTION_UPDATE_FCHAN_VOUT:
        ReturnValue = thumb_motion_update_fchan_vout(param1);
        break;
    case THUMB_MOTION_CHANGE_DISPLAY:
        ReturnValue = thumb_motion_change_display();
        break;
    case THUMB_MOTION_CHANGE_OSD:
        ReturnValue = thumb_motion_change_osd();
        break;
    case THUMB_MOTION_UPDATE_DCHAN_VOUT:
        break;
    case THUMB_MOTION_USB_CONNECT:
        ReturnValue = thumb_motion_usb_connect();
        break;
    case THUMB_MOTION_GUI_INIT_SHOW:
        ReturnValue = thumb_motion_start_show_gui();
        break;
    case THUMB_MOTION_UPDATE_BAT_POWER_STATUS:
        ReturnValue = thumb_motion_update_bat_power_status(param1);
        break;
    case THUMB_MOTION_WARNING_MSG_SHOW_START:
        ReturnValue = thumb_motion_warning_msg_show( 1, param1, param2);
        break;
    case THUMB_MOTION_WARNING_MSG_SHOW_STOP:
        ReturnValue = thumb_motion_warning_msg_show( 0, param1, param2);
        break;
    default:
        break;
    }

    return ReturnValue;
}
