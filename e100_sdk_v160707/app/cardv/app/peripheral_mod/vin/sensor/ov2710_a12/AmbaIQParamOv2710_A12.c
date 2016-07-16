/**
 * @file app/cardv/app/inc/peripheral_mode/win/sensor/AmbaIQParamOv2710_A12.c
 *
 * Implementation of OV 2710 related settings.
 *
 * History:
 *  2015/02/06 - [Jim Meng] created file
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
#include "AmbaIQParamOv2710_A12_Table.h"


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv2710_A12_Init
 *
 *  @Description:: Init function for Ov2710 sensor
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
static int AmbaIQParamOv2710_A12_Init(void)
{
    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv2710_A12_Get_Color_Table_Path
 *
 *  @Description:: Get color table path for Ov2710 sensor
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
static COLOR_TABLE_PATH_s* AmbaIQParamOv2710_A12_Get_Color_Table_Path(void)
{
    return GCcTableOv2710;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv2710_A12_Get_Adj_Video_Table_Path
 *
 *  @Description:: Get adj video table path for Ov2710 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv2710_A12_Get_Adj_Video_Table_Path(int paramLoaded, int tableNo)
{
    return &GVideoAdjTablePathOv2710[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv2710_A12_Get_Adj_Video_High_Iso_Table_Path
 *
 *  @Description:: Get adj video high iso table path for Ov2710 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv2710_A12_Get_Adj_Video_High_Iso_Table_Path(int paramLoaded, int tableNo)
{
    return &GVideoHIsoAdjTablePathOv2710[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv2710_A12_Get_Adj_Photo_Table_Path
 *
 *  @Description:: Get adj photo table path for Ov2710 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv2710_A12_Get_Adj_Photo_Table_Path(int paramLoaded, int tableNo)
{
    return &GPhotoAdjTablePathOv2710[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv2710_A12_Get_Adj_Still_Low_Iso_Table_Path
 *
 *  @Description:: Get adj still low iso table path for Ov2710 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv2710_A12_Get_Adj_Still_Low_Iso_Table_Path(int paramLoaded, int tableNo)
{
    return &GStillLIsoAdjTablePathOv2710[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv2710_A12_Get_Adj_Still_High_Iso_Table_Path
 *
 *  @Description:: Get adj still high iso table path for Ov2710 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv2710_A12_Get_Adj_Still_High_Iso_Table_Path(int paramLoaded, int tableNo)
{
    return &GStillHIsoAdjTablePathOv2710[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv2710_A12_Get_Scene_Data_Table_Path
 *
 *  @Description:: Get scene mode data table path for Ov2710 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv2710_A12_Get_Scene_Data_Table_Path(int paramLoaded, int sceneSet)
{
    return &GSceneDataTablePathOv2710[sceneSet];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv2710_A12_Get_Param_Table_Path
 *
 *  @Description:: Get param table path for Ov2710 sensor
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
static PARAM_TABLE_PATH_s* AmbaIQParamOv2710_A12_Get_Param_Table_Path(void)
{
    return NULL;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv2710_A12_Get_Img_Table_Path
 *
 *  @Description:: Get Image default img path for Ov2710 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv2710_A12_Get_Img_Table_Path(void)
{
    return &GImgAdjTablePathOv2710;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv2710_A12_Get_Aaa_Table_Path
 *
 *  @Description:: Get Aaa default param table path for Ov2710 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv2710_A12_Get_Aaa_Table_Path(int tableNo)
{
    return &GAaaAdjTablePathOv2710[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv2710_A12_Get_AdjTable_Table_Path
 *
 *  @Description:: Get Adj Table path for Ov2710 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv2710_A12_Get_AdjTable_Table_Path(void)
{
    return &GAdjTablePathOv2710;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv2710_A12_Get_Adj_Still_Idx_Info_Table_Path
 *
 *  @Description:: Get Still Idx Adj param table path for Ov2710 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv2710_A12_Get_Adj_Still_Idx_Info_Table_Path(void)
{
    return &GStillIdxInfoAdjTablePathOv2710;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv2710_A12_Get_Adj_Video_Idx_Info_Table_Path
 *
 *  @Description:: Get Video Idx Adj param table path for Ov2710 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv2710_A12_Get_Adj_Video_Idx_Info_Table_Path(void)
{
    return &GVideoIdxInfoAdjTablePathOv2710;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv2710_A12_Get_De_Table_Path
 *
 *  @Description:: Get De param table pathfor Ov2710 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv2710_A12_Get_De_Table_Path(int mode)
{
    switch(mode){
        case DE_PARAM_VIDEO:
        return &GDeVideoTablePathOv2710;
            break;
    case DE_PARAM_STILL:
            return &GDeStillTablePathOv2710;
        break;
    default:
        return &GDeVideoTablePathOv2710;
        break;

    }

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv2710_A12_Register
 *
 *  @Description:: Register parameter proxy for Ov2710 sensor
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
int AppIQParamOv2710_A12_Register(UINT32 chNo)
{
    PARAM_PROXY_s Par = {0};
    char DevName[] = {'o','v','2','7','1','0','_','a','1','2','\0'};

    Par.Id = 0;
    strcpy(Par.Name, DevName);

    Par.Init = AmbaIQParamOv2710_A12_Init;
    Par.GetColorTablePath = AmbaIQParamOv2710_A12_Get_Color_Table_Path;
    Par.GetParamTablePath = AmbaIQParamOv2710_A12_Get_Param_Table_Path;
    Par.GetAdjVideoTablePath = AmbaIQParamOv2710_A12_Get_Adj_Video_Table_Path;
    Par.GetAdjVideoHIsoTablePath = AmbaIQParamOv2710_A12_Get_Adj_Video_High_Iso_Table_Path;
    Par.GetAdjPhotoTablePath = AmbaIQParamOv2710_A12_Get_Adj_Photo_Table_Path;
    Par.GetAdjStillLIsoTablePath = AmbaIQParamOv2710_A12_Get_Adj_Still_Low_Iso_Table_Path;
    Par.GetAdjStillHIsoTablePath = AmbaIQParamOv2710_A12_Get_Adj_Still_High_Iso_Table_Path;
    Par.GetSceneDataTablePath = AmbaIQParamOv2710_A12_Get_Scene_Data_Table_Path;

    Par.GetImgTablePath = AmbaIQParamOv2710_A12_Get_Img_Table_Path;
    Par.GetAaaTablePath = AmbaIQParamOv2710_A12_Get_Aaa_Table_Path;
    Par.GetAdjStillIdxInfoTablePath = AmbaIQParamOv2710_A12_Get_Adj_Still_Idx_Info_Table_Path;
    Par.GetAdjVideoIdxInfoTablePath = AmbaIQParamOv2710_A12_Get_Adj_Video_Idx_Info_Table_Path;
    Par.GetDeTablePath = AmbaIQParamOv2710_A12_Get_De_Table_Path;
    Par.GetAdjTablePath = AmbaIQParamOv2710_A12_Get_AdjTable_Table_Path;

    AppLibIQParam_Attach(chNo, &Par);

    return 0;
}

