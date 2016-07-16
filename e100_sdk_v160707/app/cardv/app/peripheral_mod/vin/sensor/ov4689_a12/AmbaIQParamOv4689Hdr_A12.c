/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIQParamOv4689Hdr_A12.c
 *
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Implementation of SONY OV4689 related settings.
 *
 *  @History        ::
 *      Date        Name        Comments
 *      07/16/2013  Eathan Wu Created
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "AmbaDataType.h"
#include "AmbaPrint.h"
#include "AmbaFS.h"
#include "AmbaUtility.h"

#include <imgproc/AmbaImg_Adjustment_A12.h>
#include <3a/iqparam/ApplibIQParamHandler.h>
#include "AmbaIQParamOv4689Hdr_A12_Table.h"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv4689Hdr_A12_Init
 *
 *  @Description:: Init function for OV4689 sensor
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
static int AmbaIQParamOv4689Hdr_A12_Init(void)
{
    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv4689Hdr_A12_Get_Color_Table_Path
 *
 *  @Description:: Get color table path for OV4689 sensor
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
static COLOR_TABLE_PATH_s* AmbaIQParamOv4689Hdr_A12_Get_Color_Table_Path(void)
{
    return GCcTableOV4689hdr;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv4689Hdr_A12_Get_Adj_Video_Table_Path
 *
 *  @Description:: Get adj video table path for OV4689 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv4689Hdr_A12_Get_Adj_Video_Table_Path(int paramLoaded, int tableNo)
{
    return &GVideoAdjTablePathOV4689hdr[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv4689Hdr_A12_Get_Adj_Video_High_Iso_Table_Path
 *
 *  @Description:: Get adj video high iso table path for OV4689 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv4689Hdr_A12_Get_Adj_Video_High_Iso_Table_Path(int paramLoaded, int tableNo)
{
    return &GVideoHIsoAdjTablePathOV4689hdr[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv4689Hdr_A12_Get_Adj_Photo_Table_Path
 *
 *  @Description:: Get adj photo table path for OV4689 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv4689Hdr_A12_Get_Adj_Photo_Table_Path(int paramLoaded, int tableNo)
{
    return &GPhotoAdjTablePathOV4689hdr[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv4689Hdr_A12_Get_Adj_Still_Low_Iso_Table_Path
 *
 *  @Description:: Get adj still low iso table path for OV4689 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv4689Hdr_A12_Get_Adj_Still_Low_Iso_Table_Path(int paramLoaded, int tableNo)
{
    return &GStillLIsoAdjTablePathOV4689hdr[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv4689Hdr_A12_Get_Adj_Still_High_Iso_Table_Path
 *
 *  @Description:: Get adj still high iso table path for OV4689 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv4689Hdr_A12_Get_Adj_Still_High_Iso_Table_Path(int paramLoaded, int tableNo)
{
    return &GStillHIsoAdjTablePathOV4689hdr[tableNo];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv4689Hdr_A12_Get_Scene_Data_Table_Path
 *
 *  @Description:: Get scene mode data table path for OV4689 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv4689Hdr_A12_Get_Scene_Data_Table_Path(int paramLoaded, int sceneSet)
{
    return &GSceneDataTablePathOV4689hdr[sceneSet];
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv4689Hdr_A12_Get_Param_Table_Path
 *
 *  @Description:: Get param table path for OV4689 sensor
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
static PARAM_TABLE_PATH_s* AmbaIQParamOv4689Hdr_A12_Get_Param_Table_Path(void)
{
    return NULL;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv4689Hdr_A12_Get_Img_Table_Path
 *
 *  @Description:: Get Image default img path for OV4689 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv4689Hdr_A12_Get_Img_Table_Path(void)
{
    return &GImgAdjTablePathOV4689hdr;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv4689Hdr_A12_Get_Aaa_Table_Path
 *
 *  @Description:: Get Aaa default param table path for OV4689 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv4689Hdr_A12_Get_Aaa_Table_Path( int tableNo)
{
    return &GAaaAdjTablePathOV4689hdr[tableNo];
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv4689Hdr_A12_Get_AdjTable_Table_Path
 *
 *  @Description:: Get Adj Table path for OV4689 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv4689Hdr_A12_Get_AdjTable_Table_Path(void)
{
    return &GAdjTablePathOV4689hdr;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv4689Hdr_A12_Get_Adj_Still_Idx_Info_Table_Path
 *
 *  @Description:: Get Still Idx Adj param table path for OV4689 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv4689Hdr_A12_Get_Adj_Still_Idx_Info_Table_Path(void)
{
    return &GStillIdxInfoAdjTablePathOV4689hdr;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv4689Hdr_A12_Get_Adj_Video_Idx_Info_Table_Path
 *
 *  @Description:: Get Video Idx Adj param table path for OV4689 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv4689Hdr_A12_Get_Adj_Video_Idx_Info_Table_Path(void)
{
    return &GVideoIdxInfoAdjTablePathOV4689hdr;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv4689Hdr_A12_Get_De_Table_Path
 *
 *  @Description:: Get De param table pathfor OV4689 sensor
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
static IP_TABLE_PATH_s* AmbaIQParamOv4689Hdr_A12_Get_De_Table_Path(int mode)
{
    switch(mode){
        case DE_PARAM_VIDEO:
        return &GDeVideoTablePathOV4689hdr;
            break;
    case DE_PARAM_STILL:
            return &GDeStillTablePathOV4689hdr;
        break;
    default:
        return &GDeVideoTablePathOV4689hdr;
        break;

    }

}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIQParamOv4689Hdr_A12_Register
 *
 *  @Description:: Register parameter proxy for OV4689 sensor
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
int AmbaIQParamOv4689Hdr_A12_Register(UINT32 chNo)
{
    PARAM_PROXY_s Par = {0};
    char DevName[] = {'o','v','4','6','8','9','h','d','r','_','a','1','2','\0'};

    Par.Id = 0;
    strcpy(Par.Name, DevName);

    Par.Init = AmbaIQParamOv4689Hdr_A12_Init;
    Par.GetColorTablePath = AmbaIQParamOv4689Hdr_A12_Get_Color_Table_Path;
    Par.GetParamTablePath = AmbaIQParamOv4689Hdr_A12_Get_Param_Table_Path;
    Par.GetAdjVideoTablePath = AmbaIQParamOv4689Hdr_A12_Get_Adj_Video_Table_Path;
    Par.GetAdjVideoHIsoTablePath = AmbaIQParamOv4689Hdr_A12_Get_Adj_Video_High_Iso_Table_Path;
    Par.GetAdjPhotoTablePath = AmbaIQParamOv4689Hdr_A12_Get_Adj_Photo_Table_Path;
    Par.GetAdjStillLIsoTablePath = AmbaIQParamOv4689Hdr_A12_Get_Adj_Still_Low_Iso_Table_Path;
    Par.GetAdjStillHIsoTablePath = AmbaIQParamOv4689Hdr_A12_Get_Adj_Still_High_Iso_Table_Path;
    Par.GetSceneDataTablePath = AmbaIQParamOv4689Hdr_A12_Get_Scene_Data_Table_Path;
    Par.GetImgTablePath = AmbaIQParamOv4689Hdr_A12_Get_Img_Table_Path;
    Par.GetAaaTablePath = AmbaIQParamOv4689Hdr_A12_Get_Aaa_Table_Path;
    Par.GetAdjStillIdxInfoTablePath = AmbaIQParamOv4689Hdr_A12_Get_Adj_Still_Idx_Info_Table_Path;
    Par.GetAdjVideoIdxInfoTablePath = AmbaIQParamOv4689Hdr_A12_Get_Adj_Video_Idx_Info_Table_Path;
    Par.GetDeTablePath = AmbaIQParamOv4689Hdr_A12_Get_De_Table_Path;
    Par.GetAdjTablePath = AmbaIQParamOv4689Hdr_A12_Get_AdjTable_Table_Path;

    AppLibIQParam_Attach(chNo, &Par);
    //AmbaPrint("%s, %s",__FUNCTION__, Par.Name);
    return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
