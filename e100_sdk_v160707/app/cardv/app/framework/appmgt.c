/**
  * @file src/app/framework/appmgt.c
  *
  * Implementation of Application Management
  *
  * History:
  *    2013/07/09 - [Martin Lai] created file
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

#include "appmgt.h"

static APP_APP_s **Apps;
static int AppMax = 0;
static int AppNum = 0;
static int AppCur = 0;
static int AppS1Cur = 0;


/**
 *  @brief Initialize the application management
 *
 *  Initialize the application management
 *
 *  @param [in] sysApps The buffer address of application.
 *  @param [in] appMaxNum Maximum number of application
 *
 *  @return >=0 success, <0 failure
 */
int AppAppMgt_Init(APP_APP_s **sysApps, int appMaxNum)
{
    Apps = sysApps;
    AppMax = appMaxNum;
    AppNum = 0;
    AppCur = 0;
    AppS1Cur = 0;
    return 0;
}

/**
 *  @brief Register application
 *
 *  Register application
 *
 *  @param [in] app Application instance
 *
 *  @return >=0 success, <0 failure
 */
int AppAppMgt_Register(APP_APP_s *app)
{
    int appId = -1;

    if (AppNum < AppMax) {
        appId = AppNum;
        app->Id = appId;
        Apps[appId] = app;
        AppNum++;
    } else {
        AmbaPrintColor(RED, "[App-AppMgt] Fatal Error! No space for more apps");
    }

    return appId;
}


/**
 *  @brief Get application instance
 *
 *  Get application instance
 *
 *  @param [in] appId Application index
 *  @param [out] app Application instance
 *
 *  @return >=0 success, <0 failure
 */
int AppAppMgt_GetApp(int appId, APP_APP_s **app)
{
    if ((appId < 0) || (appId >= AppNum)) {
        *app = NULL;
        return -1;
    } else {
        *app = Apps[appId];
        return 0;
    }
}

/**
 *  @brief Get current application instance
 *
 *  Get current application instance
 *
 *  @param [in] app application instance
 *
 *  @return Current application id
 */
int AppAppMgt_GetCurApp(APP_APP_s **app)
{
    *app = Apps[AppCur];
    return AppCur;
}


/**
 *  @brief Get tier 1 application instance
 *
 *  Get tier 1 application instance
 *
 *  @param [in] app application instance
 *
 *  @return application id
 */
int AppAppMgt_GetCurT1App(APP_APP_s **app)
{
    *app = Apps[AppS1Cur];
    return AppS1Cur;
}


/**
 *  @brief Shrink current application
 *
 *  Shrink current application
 *
 *  @param [in] appId Application id
 *
 *  @return >=0 success, <0 failure
 */
static int AppMgt_shrink(int appId)
{
    int ReturnValue = 0;
    int AppCurTmp = 0;

    if ((appId < 0) || (appId >= AppNum)) {
        AmbaPrintColor(RED, "[App-AppMgt] Fatal Error! Invalid app Id: appId = %d / AppNum = %d", appId, AppNum);
        return -1;
    }

    while (Apps[AppCur]->Tier > Apps[appId]->Tier) {
        // Remove current app aflags
        APP_REMOVEFLAGS(Apps[AppCur]->GFlags, APP_AFLAGS_START);
        APP_REMOVEFLAGS(Apps[AppCur]->GFlags, APP_AFLAGS_READY);
        // back up current app Id
        AppCurTmp = AppCur;    // AppCurTmp is old app
        // current app Id move up
        AppCur = Apps[AppCurTmp]->Parent;
        // reset current app Child app Id
        Apps[AppCur]->Child = 0;
        // clear old current status
        Apps[AppCurTmp]->Parent = 0;
        Apps[AppCurTmp]->Previous = 0;
        // Stop old current app
        Apps[AppCurTmp]->Stop();
        if (APP_CHECKFLAGS(Apps[AppCurTmp]->GFlags, APP_AFLAGS_OVERLAP)) {
            ReturnValue = 1;
        }
    }

    return ReturnValue;
}


/**
 *  @brief Switch current application
 *
 *  Switch current application
 *
 *  @param [in] appId Application id
 *
 *  @return >=0 success, <0 failure
 */
static int AppMgt_switch_to(int appId)
{
    int ReturnValue = 0;
    int AppCurTmp = 0;
    int AppParent = 0;

    if ((appId < 0) || (appId >= AppNum)) {
        AmbaPrintColor(RED, "[App-AppMgt] Fatal Error! Invalid app Id: appId = %d / AppNum = %d", appId, AppNum);
        return -1;
    }
    if (Apps[AppCur]->Tier == Apps[appId]->Tier) {
        // Remove current app aflags
        APP_REMOVEFLAGS(Apps[AppCur]->GFlags, APP_AFLAGS_START);
        APP_REMOVEFLAGS(Apps[AppCur]->GFlags, APP_AFLAGS_READY);
        // Back up current app Id
        AppCurTmp = AppCur;  // AppCurTmp is old app
        // Back up Parent app Id
        AppParent = Apps[AppCur]->Parent;
        // current app Id move to new app
        AppCur = appId;
        // set current Tier-1 app Id if needed
        if (Apps[AppCur]->Tier == 1) {
            AppS1Cur = AppCur;
        }
        // Reset Parent app Child app Id
        Apps[AppParent]->Child = AppCur;
        // Set current app status
        Apps[AppCur]->Parent = AppParent;
        Apps[AppCur]->Previous = AppCurTmp;
        // clear old current app status
        Apps[AppCurTmp]->Parent = 0;
        Apps[AppCurTmp]->Previous = 0;
        // Stop old current app
        Apps[AppCurTmp]->Stop();
        // Stop Parent app if new current app Overlap but old current app doesn't
        if (!APP_CHECKFLAGS(Apps[AppCurTmp]->GFlags, APP_AFLAGS_OVERLAP) &&
            APP_CHECKFLAGS(Apps[AppCur]->GFlags, APP_AFLAGS_OVERLAP)) {
            Apps[AppParent]->Stop();
        // Start Parent app if new current app doesn't Overlap but old current app does
        } else if (APP_CHECKFLAGS(Apps[AppCurTmp]->GFlags, APP_AFLAGS_OVERLAP) &&
            !APP_CHECKFLAGS(Apps[AppCur]->GFlags, APP_AFLAGS_OVERLAP)) {
            Apps[AppParent]->Start();
        }
        // Start new current app
        Apps[AppCur]->Start();
    }

    return ReturnValue;
}


/**
 *  @brief Switch application
 *
 *  Switch application
 *
 *  @param [in] appId Application id
 *
 *  @return >=0 success, <0 failure
 */
int AppAppMgt_SwitchApp(int appId)
{
    int ReturnValue = 0;
    int Overlap = 0;
    int AppCurTmp = 0;

    if ((appId < 0) || (appId >= AppNum)) {
        AmbaPrintColor(RED, "[App-AppMgt] Fatal Error! Invalid app Id: appId = %d / AppNum = %d", appId, AppNum);
        return -1;
    }

    if (appId == AppCur) {
        return ReturnValue;
    }
    if (Apps[AppCur]->Tier < Apps[appId]->Tier) {
        // back up current app Id
        AppCurTmp = AppCur;
        AppCur = appId;
        if (Apps[AppCur]->Tier == 1) {
            AppS1Cur = AppCur;
        }
        // set old current Child app Id
        Apps[AppCurTmp]->Child = AppCur;
        // set new current Parent app Id
        Apps[AppCur]->Parent = AppCurTmp;
        // Stop old current app (Parent) if new current app Overlap
        if (APP_CHECKFLAGS(Apps[AppCur]->GFlags, APP_AFLAGS_OVERLAP)) {
            Apps[AppCurTmp]->Stop();
        }
        // Start new current app
        Apps[AppCur]->Start();
    } else if (APP_CHECKFLAGS(Apps[appId]->GFlags, APP_AFLAGS_START)) {
        /* the new app is the ancestor of current app */
        Overlap = AppMgt_shrink(appId);
        if (Overlap) {
            // restart new curren app since it was overlapped
            Apps[AppCur]->Start();
        }
    } else if (!AppAppMgt_CheckIdle()) {
        AmbaPrint("[App-AppMgt] Stop running application first!");
        return -1;
    } else if (Apps[AppCur]->Tier == Apps[appId]->Tier) {
        AppMgt_switch_to(appId);
    } else {
        Overlap = AppMgt_shrink(appId);
        AppMgt_switch_to(appId);
    }
    if (appId != AppCur) {
        AmbaPrintColor(RED, "[App-AppMgt] Fatal Error! App switching flow error");
        return -1;
    }
    return ReturnValue;
}

/**
 *  @brief Check whether system is encoding or decoding..
 *
 *  Check whether system is encoding or decoding..
 *
 *  @return Busy status
 */
int AppAppMgt_CheckBusy(void)
{
    int busy = 0;
    APP_APP_s *app = Apps[0];    //APP_MAIN

    while ((app->Child != 0) && (!busy)) {
        app = Apps[app->Child];
        if (APP_CHECKFLAGS(app->GFlags, APP_AFLAGS_BUSY)) {
            busy = app->Id;
        }
    }

    return busy;
}

/**
 *  @brief Check whether system is doing IO actions.
 *
 *  Check whether system is doing IO actions.
 *
 *  @return IO status
 */
int AppAppMgt_CheckIo(void)
{
    int Io = 0;
    APP_APP_s *app = Apps[0];    //APP_MAIN

    while ((app->Child != 0) && (!Io)) {
        app = Apps[app->Child];
        if (APP_CHECKFLAGS(app->GFlags, APP_AFLAGS_IO)) {
          Io = app->Id;
        }
    }

    return Io;
}

/**
 *  @brief Check whether system is idling.
 *
 *  Check whether system is idling.
 *
 *  @return Idle status
 */
int AppAppMgt_CheckIdle(void)
{
    APP_APP_s *curapp = Apps[AppCur];
    return (APP_CHECKFLAGS(curapp->GFlags, APP_AFLAGS_READY) &&
    (!AppAppMgt_CheckBusy()) &&
    (!AppAppMgt_CheckIo()));
}
