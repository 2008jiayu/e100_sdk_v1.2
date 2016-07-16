/**
 * @file app/connected/app/inc/peripheral_mode/win/sensor/AmbaIQParamMn34120_A12.c
 *
 * Implementation of SONY MN34120 related settings.
 *
 * History:
 *    2013/07/16 - [Eathan Wu] created file
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <AmbaDataType.h>
#include <AmbaPrintk.h>
#include <AmbaFS.h>
#include <AmbaUtility.h>

#include <imgproc/AmbaImg_Adjustment_A12.h>
#include <3a/iqparam/ApplibIQParamHandler.h>
#include "AmbaIQParamMn34120_A12_Table.h"


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamMn34120_A12_Init
 *
 *  @Description:: Init function for MN34120 sensor
 *
 *  @Input      ::
 *    None
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    int : 0(OK)/-1(NG)
\*-----------------------------------------------------------------------------------------------*/
static int AmbaIQParamMn34120_A12_Init(void)
{
    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamMn34120_A12_Get_Color_Table_Path
 *
 *  @Description:: Get color table path for MN34120 sensor
 *
 *  @Input      ::
 *    None
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    COLOR_TABLE_PATH_s * : Return color table path address.
\*-----------------------------------------------------------------------------------------------*/
static COLOR_TABLE_PATH_s* AmbaIQParamMn34120_A12_Get_Color_Table_Path(void)
{
    return GCcTableMN34120;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamMn34120_A12_Get_Adj_Video_Table_Path
 *
 *  @Description:: Get adj video table path for MN34120 sensor
 *
 *  @Input      ::
 *    None
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    IP_TABLE_PATH_s * : Return IQ table path address.
\*-----------------------------------------------------------------------------------------------*/
static IP_TABLE_PATH_s* AmbaIQParamMn34120_A12_Get_Adj_Video_Table_Path(int paramLoaded, int tableNo)
{
    return &GVideoAdjTablePathMN34120[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamMN34120_A12_Get_Adj_Video_High_Iso_Table_Path
 *
 *  @Description:: Get adj video high iso table path for MN34120 sensor
 *
 *  @Input      ::
 *    None
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    IP_TABLE_PATH_s * : Return IQ table path address.
\*-----------------------------------------------------------------------------------------------*/
static IP_TABLE_PATH_s* AmbaIQParamMn34120_A12_Get_Adj_Video_High_Iso_Table_Path(int paramLoaded, int tableNo)
{
    return &GVideoHIsoAdjTablePathMN34120[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamMn34120_A12_Get_Adj_Photo_Table_Path
 *
 *  @Description:: Get adj photo table path for MN34120 sensor
 *
 *  @Input      ::
 *    None
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    IP_TABLE_PATH_s * : Return IQ table path address.
\*-----------------------------------------------------------------------------------------------*/
static IP_TABLE_PATH_s* AmbaIQParamMn34120_A12_Get_Adj_Photo_Table_Path(int paramLoaded, int tableNo)
{
    return &GPhotoAdjTablePathMN34120[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamMn34120_A12_Get_Adj_Still_Low_Iso_Table_Path
 *
 *  @Description:: Get adj still low iso table path for MN34120 sensor
 *
 *  @Input      ::
 *    None
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    IP_TABLE_PATH_s * : Return IQ table path address.
\*-----------------------------------------------------------------------------------------------*/
static IP_TABLE_PATH_s* AmbaIQParamMn34120_A12_Get_Adj_Still_Low_Iso_Table_Path(int paramLoaded, int tableNo)
{
    return &GStillLIsoAdjTablePathMN34120[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamMn34120_A12_Get_Adj_Still_High_Iso_Table_Path
 *
 *  @Description:: Get adj still high iso table path for MN34120 sensor
 *
 *  @Input      ::
 *    None
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    IP_TABLE_PATH_s * : Return IQ table path address.
\*-----------------------------------------------------------------------------------------------*/
static IP_TABLE_PATH_s* AmbaIQParamMn34120_A12_Get_Adj_Still_High_Iso_Table_Path(int paramLoaded, int tableNo)
{
    return &GStillHIsoAdjTablePathMN34120[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamMn34120_A12_Get_Scene_Data_Table_Path
 *
 *  @Description:: Get scene mode data table path for MN34120 sensor
 *
 *  @Input      ::
 *    None
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    IP_TABLE_PATH_s * : Return IQ table path address.
\*-----------------------------------------------------------------------------------------------*/
static IP_TABLE_PATH_s* AmbaIQParamMn34120_A12_Get_Scene_Data_Table_Path(int paramLoaded, int sceneSet)
{
    return &GSceneDataTablePathMN34120[sceneSet];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamMn34120_A12_Get_Param_Table_Path
 *
 *  @Description:: Get param table path for MN34120 sensor
 *
 *  @Input      ::
 *    None
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    PARAM_TABLE_PATH_s * : Return param table path address.
\*-----------------------------------------------------------------------------------------------*/
static PARAM_TABLE_PATH_s* AmbaIQParamMn34120_A12_Get_Param_Table_Path(void)
{
    return NULL;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamMn34120_A12_Get_Img_Table_Path
 *
 *  @Description:: Get Image default img path for Mn34120 sensor
 *
 *  @Input      ::
 *
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    IP_TABLE_PATH_s* : Return img table path address.
\*-----------------------------------------------------------------------------------------------*/
static IP_TABLE_PATH_s* AmbaIQParamMn34120_A12_Get_Img_Table_Path(void)
{
    return &GImgAdjTablePathMN34120;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamMn34120_A12_Get_Aaa_Table_Path
 *
 *  @Description:: Get Aaa default param table path for Mn34120 sensor
 *
 *  @Input      ::
 *
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    IP_TABLE_PATH_s * : Return Aaa table path address.
\*-----------------------------------------------------------------------------------------------*/
static IP_TABLE_PATH_s* AmbaIQParamMn34120_A12_Get_Aaa_Table_Path(int tableNo)
{
    return &GAaaAdjTablePathMN34120[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamMn34120_A12_Get_AdjTable_Table_Path
 *
 *  @Description:: Get Adj Table path for Mn34120 sensor
 *
 *  @Input      ::
 *
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    IP_TABLE_PATH_s * : Return AdjTable table path address.
\*-----------------------------------------------------------------------------------------------*/
static IP_TABLE_PATH_s* AmbaIQParamMn34120_A12_Get_AdjTable_Table_Path(void)
{
    return &GAdjTablePathMN34120;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamMn34120_A12_Get_Adj_Still_Idx_Info_Table_Path
 *
 *  @Description:: Get Still Idx Adj param table path for Mn34120 sensor
 *
 *  @Input      ::
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    IP_TABLE_PATH_s * : Return adj still idx info table table path address.
\*-----------------------------------------------------------------------------------------------*/
static IP_TABLE_PATH_s* AmbaIQParamMn34120_A12_Get_Adj_Still_Idx_Info_Table_Path(void)
{
    return &GStillIdxInfoAdjTablePathMN34120;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamMn34120_A12_Get_Adj_Video_Idx_Info_Table_Path
 *
 *  @Description:: Get Video Idx Adj param table path for Mn34120 sensor
 *
 *  @Input      ::
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    IP_TABLE_PATH_s * : Return video idx info table path address.
\*-----------------------------------------------------------------------------------------------*/
static IP_TABLE_PATH_s* AmbaIQParamMn34120_A12_Get_Adj_Video_Idx_Info_Table_Path(void)
{
    return &GVideoIdxInfoAdjTablePathMN34120;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamMn34120_A12_Get_De_Table_Path
 *
 *  @Description:: Get De param table pathfor Mn34120 sensor
 *
 *  @Input      ::
 *    int mode      : 0: Video mode, 1: still mode
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    IP_TABLE_PATH_s * : Return De table path address.
\*-----------------------------------------------------------------------------------------------*/
static IP_TABLE_PATH_s* AmbaIQParamMn34120_A12_Get_De_Table_Path(int mode)
{
    switch(mode){
        case DE_PARAM_VIDEO:
        return &GDeVideoTablePathMN34120;
            break;
    case DE_PARAM_STILL:
            return &GDeStillTablePathMN34120;
        break;
    default:
        return &GDeVideoTablePathMN34120;
        break;

    }

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamMn34120_A12_Register
 *
 *  @Description:: Register parameter proxy for MN34120 sensor
 *
 *  @Input      ::
 *    None
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    int : 0(OK)/-1(NG)
\*-----------------------------------------------------------------------------------------------*/
int AppIQParamMn34120_A12_Register(UINT32 chNo)
{
    PARAM_PROXY_s Par = {0};
    char DevName[] = {'i','m','x','1','1','7','_','a','1','2','\0'};

    Par.Id = 0;
    strcpy(Par.Name, DevName);

    Par.Init = AmbaIQParamMn34120_A12_Init;

    Par.GetColorTablePath = AmbaIQParamMn34120_A12_Get_Color_Table_Path;
    Par.GetParamTablePath = AmbaIQParamMn34120_A12_Get_Param_Table_Path;
    Par.GetAdjVideoTablePath = AmbaIQParamMn34120_A12_Get_Adj_Video_Table_Path;
    Par.GetAdjVideoHIsoTablePath = AmbaIQParamMn34120_A12_Get_Adj_Video_High_Iso_Table_Path;
    Par.GetAdjPhotoTablePath = AmbaIQParamMn34120_A12_Get_Adj_Photo_Table_Path;
    Par.GetAdjStillLIsoTablePath = AmbaIQParamMn34120_A12_Get_Adj_Still_Low_Iso_Table_Path;
    Par.GetAdjStillHIsoTablePath = AmbaIQParamMn34120_A12_Get_Adj_Still_High_Iso_Table_Path;
    Par.GetSceneDataTablePath = AmbaIQParamMn34120_A12_Get_Scene_Data_Table_Path;
    Par.GetImgTablePath = AmbaIQParamMn34120_A12_Get_Img_Table_Path;
    Par.GetAaaTablePath = AmbaIQParamMn34120_A12_Get_Aaa_Table_Path;
    Par.GetAdjStillIdxInfoTablePath = AmbaIQParamMn34120_A12_Get_Adj_Still_Idx_Info_Table_Path;
    Par.GetAdjVideoIdxInfoTablePath = AmbaIQParamMn34120_A12_Get_Adj_Video_Idx_Info_Table_Path;
    Par.GetDeTablePath = AmbaIQParamMn34120_A12_Get_De_Table_Path;
    Par.GetAdjTablePath = AmbaIQParamMn34120_A12_Get_AdjTable_Table_Path;

    AppLibIQParam_Attach(chNo, &Par);

    return 0;
}

