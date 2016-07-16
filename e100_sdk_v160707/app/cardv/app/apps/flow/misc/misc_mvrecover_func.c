/**
  * @file src/app/apps/flow/misc/connectedcam/misc_mvrecover_func.c
  *
  *  Functions of photo playback application
  *
  * History:
  *    2014/01/14 - [Martin Lai] created file
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

#include <apps/flow/misc/misc_mvrecover.h>
#include <system/status.h>
#include <system/app_pref.h>
#include <system/app_util.h>
#include <apps/gui/resource/gui_settle.h>
#include <apps/flow/widget/menu/widget.h>
#include <AmbaUtility.h>

static UINT8 MovieRecoverType = 0; /*0x01: DCIM, 0x02:EVENT, 0x03:DCIM & EVENT*/

static int misc_mvrecover_remove_unsavedata(void)
{
    int ReturnValue = 0;
    UINT64 FileObjID = 0;
    char CurFn[APP_MAX_FN_SIZE] = {0};
    switch (MovieRecoverType) {
        case 0x01:
    FileObjID = AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, DCIM_HDLR);
    if (FileObjID > 0) {
        AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, DCIM_HDLR, 0, CUR_OBJ(FileObjID), CurFn);
        ReturnValue = AppLibStorageDmf_DeleteFile(APPLIB_DCF_MEDIA_VIDEO, CUR_OBJ(FileObjID), DCIM_HDLR);
        AmbaPrint("[app_misc_mvrecover] Delete %s ReturnValue = %d", CurFn, ReturnValue);
    } else {
        AmbaPrint("[app_misc_mvrecover] NO FILE CAN BE DELETE");
    }
            break;
        case 0x02:
            FileObjID = AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR);
            if (FileObjID > 0) {
                AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, EVENTRECORD_HDLR, 0, CUR_OBJ(FileObjID), CurFn);
                ReturnValue = AppLibStorageDmf_DeleteFile(APPLIB_DCF_MEDIA_VIDEO, CUR_OBJ(FileObjID), EVENTRECORD_HDLR);
                AmbaPrint("[app_misc_mvrecover] Delete %s ReturnValue = %d", CurFn, ReturnValue);
            } else {
                AmbaPrint("[app_misc_mvrecover] NO FILE CAN BE DELETE");
            }
            break;
        case 0x03:
    FileObjID = AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, DCIM_HDLR);
    if (FileObjID > 0) {
        AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, DCIM_HDLR, 0, CUR_OBJ(FileObjID), CurFn);
        ReturnValue = AppLibStorageDmf_DeleteFile(APPLIB_DCF_MEDIA_VIDEO, CUR_OBJ(FileObjID), DCIM_HDLR);
        AmbaPrint("[app_misc_mvrecover] Delete %s ReturnValue = %d", CurFn, ReturnValue);
    } else {
        AmbaPrint("[app_misc_mvrecover] NO FILE CAN BE DELETE");
    }
            FileObjID = AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR);
            if (FileObjID > 0) {
                AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, EVENTRECORD_HDLR, 0, CUR_OBJ(FileObjID), CurFn);
                ReturnValue = AppLibStorageDmf_DeleteFile(APPLIB_DCF_MEDIA_VIDEO, CUR_OBJ(FileObjID), EVENTRECORD_HDLR);
                AmbaPrint("[app_misc_mvrecover] Delete %s ReturnValue = %d", CurFn, ReturnValue);
            } else {
                AmbaPrint("[app_misc_mvrecover] NO FILE CAN BE DELETE");
            }
            break;
        default:
            break;

    }
    return ReturnValue;
}

static int misc_mvrecover_init(void)
{
    int ReturnValue = 0;

    AppLibEditor_Init();
    AppLibFormat_MuxerInit();

    return ReturnValue;
}

static int misc_mvrecover_app_ready(void)
{
    int ReturnValue = 0;

    if (!APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_READY)) {
        APP_ADDFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_READY);

        // ToDo: need to remove to handler when iav completes the dsp cmd queue mechanism
        AppLibGraph_Init();
        misc_mvrecover.Func(MISC_MVRECOVER_CHANGE_OSD, 0, 0);
        misc_mvrecover.Func(MISC_MVRECOVER_GUI_INIT_SHOW, 0, 0);
        
        ReturnValue = misc_mvrecover.Func(MISC_MVRECOVER_DIALOG_SHOW_MVRECOVER, 0, 0);

        if (ReturnValue >= 0) {
            AppUtil_ReadyCheck(0);
        }

        if (!APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_READY)) {
            /* The system could switch the current app to other in the function "AppUtil_ReadyCheck". */
            return ReturnValue;
        }
    }



    return ReturnValue;
}

static int misc_mvrecover_stop(void)
{
    int ReturnValue = 0;

    /* Close the dialog. */
    if (APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_POPUP);
        ReturnValue = AppWidget_Off(WIDGET_DIALOG, 0);
    }

    /* Delete the unsaving data. */
    APP_REMOVEFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_BUSY);
    if (!APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_READY)) {
        if (app_status.MvRecover) {
            app_status.MvRecover = 0;
            misc_mvrecover_remove_unsavedata();
        }
    }

    /* Stop the warning message, because the warning could need to be updated. */
    misc_mvrecover.Func(MISC_MVRECOVER_WARNING_MSG_SHOW_STOP, 0, 0);

    /* Disable the vout. */
    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
    AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
    AppLibDisp_ChanStop(DISP_CH_FCHAN);

    /* Hide GUI */
    misc_mvrecover.Gui(GUI_HIDE_ALL, 0, 0);
    misc_mvrecover.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

static int mvrecover_dialog_ok_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;
    APP_REMOVEFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_POPUP);

    switch (sel) {
    default:
    case DIALOG_SEL_OK:
        /* To excute the functions that system block them when the Busy flag is enabled. */
        AppUtil_BusyCheck(0);
        misc_mvrecover.Func(MISC_MVRECOVER_SWITCH_APP, 0, 0);
        break;
    }

    return ReturnValue;
}

static int mvrecover_dialog_mvrecover_handler(UINT32 sel, UINT32 param1, UINT32 param2)
{
   
    int ReturnValue = 0;
    int FileExist = 0;
    char CurFn[APP_MAX_FN_SIZE] = {0};
    
    APP_REMOVEFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_POPUP);
    switch (sel) {
    case DIALOG_SEL_YES:
        if (MovieRecoverType == 0x01 ||MovieRecoverType == 0x03) {
            AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, DCIM_HDLR, 0, CUR_OBJ(AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, DCIM_HDLR)), CurFn);
        } else {
            FileExist = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, EVENTRECORD_HDLR, 0, CUR_OBJ(AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR)), CurFn);
            if (FileExist < 0) {
                int FileCount =0;
                FileCount = AppLibStorageDmf_GetObjFileCount(EVENTRECORD_HDLR, CUR_OBJ(AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR)));
                if (FileCount > 1) {
                    AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_THM, EVENTRECORD_HDLR, (FileCount-1), CUR_OBJ(AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR)), CurFn);
                }
            }
        }
        ReturnValue = AppLibEditor_MovieRecover(CurFn);
        if (ReturnValue < 0) {
            /* Delete the clip if the system fail to recovery the clip. */
            misc_mvrecover_remove_unsavedata();
            if (app_status.MvRecover == 1) {
                app_status.MvRecover = 0;
                APP_ADDFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_POPUP);
                ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_OK, DIALOG_SUB_MVRECOVER_FAIL, mvrecover_dialog_ok_handler);
                ReturnValue = AppWidget_On(WIDGET_DIALOG, 0);
            }
        } else {
            APP_ADDFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_BUSY);
            misc_mvrecover.Func(MISC_MVRECOVER_WARNING_MSG_SHOW_START, GUI_WARNING_RECOVERING, 1);
        }
        break;

    default:
    case DIALOG_SEL_NO:
        APP_ADDFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_BUSY);
        app_status.MvRecover = 0;
        misc_mvrecover_remove_unsavedata();
        misc_mvrecover.Func(MISC_MVRECOVER_OP_SUCCESS, 0, 0);
        break;
    }
    
    return ReturnValue;
}

static int misc_mvrecover_dialog_show_mvrecover(void)
{
    int ReturnValue = 0;
    #ifdef CONFIG_ECL_GUI
    int FileExist = 0;
    char CurFn[APP_MAX_FN_SIZE] = {0};
    #endif
    ReturnValue = AppLibCard_CheckStatus(CARD_CHECK_WRITE);
    MovieRecoverType = UserSetting->VideoPref.UnsavingData;
    AmbaPrintColor(CYAN,"[app_misc_mvrecover] Recover Type : %d",MovieRecoverType);
    UserSetting->VideoPref.UnsavingData = 0;
    AppPref_Save();

    if (ReturnValue == CARD_STATUS_NO_CARD) {
        AmbaPrintColor(RED,"[app_misc_mvrecover] WARNING_NO_CARD");
        misc_mvrecover.Func(MISC_MVRECOVER_SWITCH_APP, 0, 0);
    } else if (ReturnValue == CARD_STATUS_WP_CARD) {
        misc_mvrecover.Func(MISC_MVRECOVER_WARNING_MSG_SHOW_START, GUI_WARNING_CARD_PROTECTED, 0);
        AmbaPrintColor(RED,"[app_misc_mvrecover] WARNING_CARD_PROTECTED");
        misc_mvrecover.Func(MISC_MVRECOVER_SWITCH_APP, 0, 0);
    } else {
        AmbaPrintColor(GREEN,"[app_misc_mvrecover] CARD_OK");
       
        #ifndef CONFIG_ECL_GUI
        APP_ADDFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_POPUP);
        ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_Y_N, DIALOG_SUB_MVRECOVER_YES_OR_NO, mvrecover_dialog_mvrecover_handler);
        ReturnValue = AppWidget_On(WIDGET_DIALOG, 0);
        #else
        if (MovieRecoverType == 0x01 ||MovieRecoverType == 0x03) {
            AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, DCIM_HDLR, 0, CUR_OBJ(AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, DCIM_HDLR)), CurFn);
        } 
        else 
        {
            FileExist = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, EVENTRECORD_HDLR, 0, CUR_OBJ(AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR)), CurFn);
            if (FileExist < 0) {
                int FileCount =0;
                FileCount = AppLibStorageDmf_GetObjFileCount(EVENTRECORD_HDLR, CUR_OBJ(AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR)));
                if (FileCount > 1) {
                    AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_THM, EVENTRECORD_HDLR, (FileCount-1), CUR_OBJ(AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR)), CurFn);
                }
            }
        }
        ReturnValue = AppLibEditor_MovieRecover(CurFn);
        if (ReturnValue < 0) {
            /* Delete the clip if the system fail to recovery the clip. */
            misc_mvrecover_remove_unsavedata();
            if (app_status.MvRecover == 1) {
                app_status.MvRecover = 0;
               AppUtil_BusyCheck(0);
                misc_mvrecover.Func(MISC_MVRECOVER_SWITCH_APP, 0, 0);
            }
        } else {
            APP_ADDFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_BUSY);
            misc_mvrecover.Func(MISC_MVRECOVER_WARNING_MSG_SHOW_START, GUI_WARNING_RECOVERING, 1);
        }
        #endif
    }
    return ReturnValue;
}
static int DualStreamFlag = 0;

static int misc_mvrecover_op_success(UINT32 continueFlag)
{
    int ReturnValue = 0;
    char CurFn[APP_MAX_FN_SIZE] = {0};
    AmbaPrint("<misc_mvrecover_op_success>");

    if ((MovieRecoverType == 0x03) && (DualStreamFlag != 0)) {
        /**finish normal record clip recover*/
        MovieRecoverType = 0x02;
        DualStreamFlag = 0;
        mvrecover_dialog_mvrecover_handler(DIALOG_SEL_YES, 0, 0);
        return ReturnValue;
    }

    if ((DualStreamFlag == 0) && (continueFlag == 1)) {
        int FileExist = 0;
        if (MovieRecoverType == 0x01 ||MovieRecoverType == 0x03) {
            FileExist = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_THM, DCIM_HDLR, 0, CUR_OBJ(AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, DCIM_HDLR)), CurFn);
            if (FileExist < 0) {
                int FileCount =0;
                FileCount = AppLibStorageDmf_GetObjFileCount(DCIM_HDLR, CUR_OBJ(AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, DCIM_HDLR)));
                AmbaPrintColor(5,"file obj number =%d",FileCount);
                if (FileCount > 1) {
                    FileExist = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, DCIM_HDLR, (FileCount-1), CUR_OBJ(AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, DCIM_HDLR)), CurFn);
                }
            }
        } else {
            FileExist = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_THM, EVENTRECORD_HDLR, 0, CUR_OBJ(AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR)), CurFn);
            if (FileExist < 0) {
                int FileCount =0;
                FileCount = AppLibStorageDmf_GetObjFileCount(EVENTRECORD_HDLR, CUR_OBJ(AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR)));
                AmbaPrintColor(5,"file obj number =%d",FileCount);
                if (FileCount > 1) {
                    FileExist = AppLibStorageDmf_GetFileName(APPLIB_DCF_MEDIA_VIDEO, ".MP4", APPLIB_DCF_EXT_OBJECT_SPLIT_FILE, EVENTRECORD_HDLR, (FileCount-1), CUR_OBJ(AppLibStorageDmf_GetLastFilePos(APPLIB_DCF_MEDIA_VIDEO, EVENTRECORD_HDLR)), CurFn);
                }
            }

        }
        if (FileExist >= 0) {
            AmbaPrintColor(GREEN, "<misc_mvrecover_op_success> Recover the 2nd stream file.");
            DualStreamFlag++;
            ReturnValue = AppLibEditor_MovieRecover(CurFn);
            if (ReturnValue >= 0) {
                APP_ADDFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_BUSY);
                misc_mvrecover.Func(MISC_MVRECOVER_WARNING_MSG_SHOW_START, GUI_WARNING_RECOVERING, 1);
                return ReturnValue;
            }
        }
    }

    /* Release the resource of editor. */
    AppLibEditor_EditComplete();
    misc_mvrecover.Gui(GUI_WARNING_HIDE, 0, 0);
    misc_mvrecover.Gui(GUI_FLUSH, 0, 0);
    if (APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_READY)) {
        APP_REMOVEFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_BUSY);
        if (app_status.MvRecover == 1) {
            app_status.MvRecover = 0;
            if (ReturnValue < 0) {
                misc_mvrecover_remove_unsavedata();
                #ifndef CONFIG_ECL_GUI
                ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_OK, DIALOG_SUB_MVRECOVER_FAIL, mvrecover_dialog_ok_handler);
                ReturnValue = AppWidget_On(WIDGET_DIALOG, 0);
                 APP_ADDFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_POPUP);
                #endif
            } else {
                #ifndef CONFIG_ECL_GUI
                ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_OK, DIALOG_SUB_MVRECOVER_OK, mvrecover_dialog_ok_handler);
                ReturnValue = AppWidget_On(WIDGET_DIALOG, 0);
                 APP_ADDFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_POPUP);
                #endif
            }
            UserSetting->VideoPref.UnsavingData = 0;
            AppPref_Save();
            
            #ifdef CONFIG_ECL_GUI
            AppUtil_BusyCheck(0);
            misc_mvrecover.Func(MISC_MVRECOVER_SWITCH_APP, 0, 0);
            #endif

        } else { // app_editor_unsaved_movie_delete ( don't do MvRecover)
        /* To excute the functions that system block them when the Busy flag is enabled. */
            AppUtil_BusyCheck(0);
            if (APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_READY)) {
                misc_mvrecover.Func(MISC_MVRECOVER_SWITCH_APP, 0, 0);
            } else {
                return ReturnValue;    // App switched out
            }
        }
    }

    /* Reset dual stream flag */
    DualStreamFlag = 0;
    return ReturnValue;
}

static int misc_mvrecover_op_failed(void)
{
    int ReturnValue = 0;

    /* Release the resource of editor. */
    ReturnValue = AppLibEditor_EditComplete();
    /* Delete the clip if the system fail to recovery the clip. */
    misc_mvrecover_remove_unsavedata();

    misc_mvrecover.Gui(GUI_WARNING_HIDE, 0, 0);
    misc_mvrecover.Gui(GUI_FLUSH, 0, 0);
    if (APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_READY)) {
        APP_REMOVEFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_BUSY);
        if (app_status.MvRecover == 1) {
            app_status.MvRecover = 0;
            #ifndef CONFIG_ECL_GUI
            APP_ADDFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_POPUP);
            ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_OK, DIALOG_SUB_MVRECOVER_FAIL, mvrecover_dialog_ok_handler);
            ReturnValue = AppWidget_On(WIDGET_DIALOG, 0);

            #else
            AppUtil_BusyCheck(0);
            if (APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_READY)) {
                misc_mvrecover.Func(MISC_MVRECOVER_SWITCH_APP, 0, 0);
            } else {
                return ReturnValue;/**< App switched out. */
            } 
            #endif
        } else { // app_editor_unsaved_movie_delete ( don't do MvRecover)
            /* To excute the functions that system block them when the Busy flag is enabled. */
            AppUtil_BusyCheck(0);
            if (APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_READY)) {
                misc_mvrecover.Func(MISC_MVRECOVER_SWITCH_APP, 0, 0);
            } else {
                return ReturnValue;/**< App switched out. */
            }
        }
    }

    return ReturnValue;
}

static int misc_mvrecover_card_removed(void)
{
    int ReturnValue = 0;

    /*Skip the firmware update and  non-optimum format when card remove.*/
    app_status.FwUpdate = 0;
    app_status.CardFmtParam = 0;
    APP_REMOVEFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_BUSY);
    misc_mvrecover.Func(MISC_MVRECOVER_SWITCH_APP, 0, 0);

    return ReturnValue;
}


static int misc_mvrecover_card_error_removed(void)
{
    int ReturnValue = 0;

    AmbaPrint("[app_misc_mvrecover] Illegal operation: Remove card during IO/busy");
    /*Skip the firmware update and  non-optimum format when card remove.*/
    app_status.FwUpdate = 0;
    app_status.CardFmtParam = 0;
    misc_mvrecover.Func(MISC_MVRECOVER_WARNING_MSG_SHOW_STOP, 0, 0);
    if (APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_POPUP)) {
        APP_REMOVEFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_POPUP);
        ReturnValue = AppWidget_Off(WIDGET_DIALOG, 0);
    }
    if (APP_CHECKFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_READY)) {
        app_status.MvRecover = 0;
        APP_ADDFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_POPUP);
        APP_REMOVEFLAGS(app_misc_mvrecover.GFlags, APP_AFLAGS_BUSY);
        ReturnValue = AppDialog_SetDialog(DIALOG_TYPE_OK, DIALOG_SUB_MVRECOVER_FAIL, mvrecover_dialog_ok_handler);
        ReturnValue = AppWidget_On(WIDGET_DIALOG, 0);
    }

    return ReturnValue;
}

/**
 * @brief The applications switching function.
 *
 * @return >=0 success
 *         <0 failure
 */
static int misc_mvrecover_switch_app(void)
{
    int ReturnValue = 0;

    if ( app_status.LockDecMode && app_status.FchanDecModeOnly) {
        ReturnValue = AppUtil_SwitchApp(APP_THUMB_MOTION);
    } else {
        /**return to default APP*/
        AppUtil_SwitchApp(AppUtil_GetStartApp(0));
    }

    return ReturnValue;
}

/**
 *  @brief To change the vout of Fchan
 *
 *  To change the vout of Fchan
 *
 *  @param [in] msg Message ID
 *
 *  @return >=0 success, <0 failure
 */
static int misc_mvrecover_update_fchan_vout(UINT32 msg)
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
        AmbaPrint("[app_misc_mvrecover] Vout no changed");
        return 0;
        break;
    }
    ReturnValue = AppLibDisp_SelectDevice(DISP_CH_FCHAN, DISP_ANY_DEV);
    if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_CHANGE)) {
        AmbaPrint("[app_misc_mvrecover] Display FCHAN has no changed");
    } else {
        if (APP_CHECKFLAGS(ReturnValue, DISP_FCHAN_NO_DEVICE)) {
            AppLibGraph_DisableDraw(GRAPH_CH_FCHAN);
            AppLibDisp_ChanStop(DISP_CH_FCHAN);
            AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
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
                    DBGMSG("[app_misc_mvrecover] FChan Disable. Disable the fchan Window");
                    AppLibGraph_DisableDraw(GRAPH_CH_FCHAN);
                    AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
                    AppLibDisp_FlushWindow(DISP_CH_FCHAN);
                    app_status.LockDecMode = 0;
                } else {
                    /** FCHAN Window*/
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
                    AppLibDisp_FlushWindow(DISP_CH_FCHAN);
                }
            }
            AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN);
            AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN);
            AppLibGraph_FlushWindow(GRAPH_CH_FCHAN);
            misc_mvrecover.Gui(GUI_SET_LAYOUT, 0, 0);
            misc_mvrecover.Gui(GUI_FLUSH, 0, 0);
        }
    }

    return ReturnValue;
}

static int misc_mvrecover_change_display(void)
{
    int ReturnValue = 0;

    AppLibDisp_SelectDevice(DISP_CH_FCHAN | DISP_CH_DCHAN, DISP_ANY_DEV);
    AppLibDisp_ConfigMode(DISP_CH_FCHAN | DISP_CH_DCHAN, AppLibSysVout_GetVoutMode(VOUT_DISP_MODE_1080P));
    AppLibDisp_SetupChan(DISP_CH_FCHAN | DISP_CH_DCHAN);
    AppLibDisp_ChanStart(DISP_CH_FCHAN | DISP_CH_DCHAN);
    {
        AMP_DISP_WINDOW_CFG_s Window;
        AMP_DISP_INFO_s DispDev = {0};

        memset(&Window, 0x0, sizeof(AMP_DISP_WINDOW_CFG_s));

        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_misc_mvrecover] FChan Disable. Disable the fchan Window");
            AppLibDisp_DeactivateWindow(DISP_CH_FCHAN, AppLibDisp_GetWindowId(DISP_CH_FCHAN, 0));
            AppLibDisp_FlushWindow(DISP_CH_FCHAN);
            AppLibGraph_DisableDraw(GRAPH_CH_FCHAN);
            app_status.LockDecMode = 0;
        } else {
            /** FCHAN Window*/
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
            AppLibGraph_EnableDraw(GRAPH_CH_FCHAN);
        }

        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);
        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            DBGMSG("[app_misc_mvrecover] DChan Disable. Disable the Dchan Window");
            AppLibDisp_DeactivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
            AppLibDisp_FlushWindow(DISP_CH_FCHAN);
            AppLibGraph_DisableDraw(GRAPH_CH_DCHAN);
            app_status.LockDecMode = 0;
        } else {
            /** DCHAN Window*/
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
            AppLibDisp_SetWindowConfig(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0), &Window);
            AppLibDisp_ActivateWindow(DISP_CH_DCHAN, AppLibDisp_GetWindowId(DISP_CH_DCHAN, 0));
            AppLibGraph_EnableDraw(GRAPH_CH_DCHAN);
        }
        AppLibDisp_FlushWindow(DISP_CH_FCHAN | DISP_CH_DCHAN);
    }

    return ReturnValue;
}

static int misc_mvrecover_change_osd(void)
{
    int ReturnValue = 0;

    /* Update graphic Window*/
    AppLibGraph_SetWindowConfig(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_ActivateWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    AppLibGraph_FlushWindow(GRAPH_CH_FCHAN | GRAPH_CH_DCHAN);
    misc_mvrecover.Gui(GUI_SET_LAYOUT, 0, 0);
    misc_mvrecover.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

/**
 *  @brief Show gui of app
 *
 *  Show gui of app
 *
 *  @return >=0 success, <0 failure
 */
static int misc_mvrecover_start_show_gui(void)
{
    int ReturnValue = 0;

    misc_mvrecover.Gui(GUI_APP_ICON_SHOW, 0, 0);
    misc_mvrecover.Gui(GUI_FLUSH, 0, 0);

    return ReturnValue;
}

/**
 *  @brief The timer of warning message.
 *
 *  The timer of warning message.
 *
 *  @param [in] eid event id
 *
 *  @return >=0 success, <0 failure
 */
static void misc_mvrecover_warning_timer_handler(int eid)
{
    static int BlinkCount;

    if (eid == TIMER_UNREGISTER) {
        BlinkCount = 0;
        return;
    }

    BlinkCount++;

    if (BlinkCount & 0x01) {
        misc_mvrecover.Gui(GUI_WARNING_HIDE, 0, 0);
    } else {
        misc_mvrecover.Gui(GUI_WARNING_SHOW, 0, 0);
    }

    if (BlinkCount >= 5) {
        APP_REMOVEFLAGS(app_misc_mvrecover.Flags, MISC_MVRECOVER_WARNING_MSG_RUN);
        AppLibComSvcTimer_Unregister(TIMER_2HZ, misc_mvrecover_warning_timer_handler);
        misc_mvrecover.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    misc_mvrecover.Gui(GUI_FLUSH, 0, 0);

}

static int misc_mvrecover_warning_msg_show(int enable, int param1, int param2)
{
    int ReturnValue = 0;

    if (enable) {
        if (param2) {
            misc_mvrecover.Gui(GUI_WARNING_UPDATE, param1, 0);
            misc_mvrecover.Gui(GUI_WARNING_SHOW, 0, 0);
        } else {
            if (!APP_CHECKFLAGS(app_misc_mvrecover.Flags, MISC_MVRECOVER_WARNING_MSG_RUN)) {
                APP_ADDFLAGS(app_misc_mvrecover.Flags, MISC_MVRECOVER_WARNING_MSG_RUN);
                misc_mvrecover.Gui(GUI_WARNING_UPDATE, param1, 0);
                misc_mvrecover.Gui(GUI_WARNING_SHOW, 0, 0);
                AppLibComSvcTimer_Register(TIMER_2HZ, misc_mvrecover_warning_timer_handler);
            }
        }
    } else {
        if (APP_CHECKFLAGS(app_misc_mvrecover.Flags, MISC_MVRECOVER_WARNING_MSG_RUN)) {
            APP_REMOVEFLAGS(app_misc_mvrecover.Flags, MISC_MVRECOVER_WARNING_MSG_RUN);
            AppLibComSvcTimer_Unregister(TIMER_2HZ, misc_mvrecover_warning_timer_handler);
        }
        misc_mvrecover.Gui(GUI_WARNING_HIDE, 0, 0);
    }
    misc_mvrecover.Gui(GUI_FLUSH, 0, 0);
    return ReturnValue;
}


/**
 *  @brief The functions of application
 *
 *  The functions of application
 *
 *  @param[in] funcId Function id
 *  @param[in] param1 first parameter
 *  @param[in] param2 second parameter
 *
 *  @return >=0 success, <0 failure
 */
int misc_mvrecover_func(UINT32 funcId, UINT32 param1, UINT32 param2)
{
    int ReturnValue = 0;

    switch (funcId) {
    case MISC_MVRECOVER_INIT:
        ReturnValue = misc_mvrecover_init();
        break;
    case MISC_MVRECOVER_START:
        break;
    case MISC_MVRECOVER_STOP:
        ReturnValue = misc_mvrecover_stop();
        break;
    case MISC_MVRECOVER_APP_READY:
        ReturnValue = misc_mvrecover_app_ready();
        break;
    case MISC_MVRECOVER_DIALOG_SHOW_MVRECOVER:
        ReturnValue = misc_mvrecover_dialog_show_mvrecover();
        break;
    case MISC_MVRECOVER_OP_SUCCESS:
        ReturnValue = misc_mvrecover_op_success(param1);
        break;
    case MISC_MVRECOVER_OP_FAILED:
        ReturnValue = misc_mvrecover_op_failed();
        break;
    case MISC_MVRECOVER_CARD_REMOVED:
        ReturnValue = misc_mvrecover_card_removed();
        break;
    case MISC_MVRECOVER_CARD_ERROR_REMOVED:
        ReturnValue = misc_mvrecover_card_error_removed();
        break;
    case MISC_MVRECOVER_SWITCH_APP:
        ReturnValue = misc_mvrecover_switch_app();
        break;
    case MISC_MVRECOVER_UPDATE_FCHAN_VOUT:
        ReturnValue = misc_mvrecover_update_fchan_vout(param1);
        break;
    case MISC_MVRECOVER_UPDATE_DCHAN_VOUT:
        break;
    case MISC_MVRECOVER_CHANGE_DISPLAY:
        ReturnValue = misc_mvrecover_change_display();
        break;
    case MISC_MVRECOVER_CHANGE_OSD:
        ReturnValue = misc_mvrecover_change_osd();
        break;
    case MISC_MVRECOVER_GUI_INIT_SHOW:
        ReturnValue = misc_mvrecover_start_show_gui();
        break;
    case MISC_MVRECOVER_WARNING_MSG_SHOW_START:
        ReturnValue = misc_mvrecover_warning_msg_show( 1, param1, param2);
        break;
    case MISC_MVRECOVER_WARNING_MSG_SHOW_STOP:
        ReturnValue = misc_mvrecover_warning_msg_show( 0, param1, param2);
        break;
    default:
        break;
    }

    return ReturnValue;
}
