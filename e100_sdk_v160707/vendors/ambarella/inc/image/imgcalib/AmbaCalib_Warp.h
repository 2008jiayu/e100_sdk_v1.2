/**
 * @file vendor/ambarella/inc/image/imgcalib/AmbaCalib_Warp.h
 *
 * header file for Amba warp calibration
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#include "AmbaDSP_ImgFilter.h"
#include "AmbaCalibInfo.h"

/**
* @defgroup AmbaCalib_Warp
* @brief Warp calibration
*
* Warp provide function to calculate warp.
*/

/**
 * @addtogroup AmbaCalib_Warp
 * @ingroup Calibration
 * @{
 */

#define IDEA_WARP   0
#define IDEA_WARP_RAWENC   (IDEA_WARP+1)
#define SWA_XY_WARP   20
#define SWA_XY_WARP_RAWENC   (SWA_XY_WARP+1)
#define DISTORTION_RAW   99
#define TOP_VIEW_WARP   0
#define TOP_VIEW_WARP_RAWENC   (TOP_VIEW_WARP+1)
#define TOP_VIEW_WARP_WAY1   10
#define TOP_VIEW_WARP_WAY2   20
#define WARP_ENABLE 1
#define COUNT_WARP_TABLE 1
#define MAX_CMD_TOKEN  128
#define MAX_HOR_GRID_NUM  2944
#define MAX_VER_GRID_NUM  1664
#define RESERVED1   0
#define RESERVED2   0
#define RESERVED3   0
#define Front       0
#define Back        1
#define Left        2
#define Right       3
#define MainFront   4
#define MainBack    5
#define MainLeft    6
#define MainRight   7

/**
*  Top view raw point coordinate
*/
typedef struct Top_View_Raw_s_ {
    double   X;          /**< x-axis */
    double   Y;          /**< y-axis */
} Top_View_Raw_s;

/**
*  Top view input point coordinate
*/
typedef struct Top_View_Input_s_ {
    double   X;          /**< x-axis */
    double   Y;          /**< y-axis */
} Top_View_Input_s;

/**
*  Top view output point coordinate
*/
typedef struct Top_View_Output_s_ {
    double   X;          /**< x-axis */
    double   Y;          /**< y-axis */
} Top_View_Output_s;

/**
*  Top view transfer matrix
*/
typedef struct Top_View_Hmatrix_s_ {
    double   H[3][3];    /**< transfer matrix */
} Top_View_Hmatrix_s;

/**
*  Top view points for computation and results
*/
typedef struct Top_View_Point_s_ {
    Top_View_Raw_s       Raw[4];     /**< Top view raw points */
    Top_View_Input_s     Input[4];   /**< Top view input points */
    Top_View_Output_s   Output[4];   /**< Top view output points */
} Top_View_Point_s;

/**
*  Warp point coordinate
*/
typedef struct Warp_TOOL_IMG_GRID_POINT_s_ {
    INT32   X;              /**< x-axis */
    INT32   Y;              /**< y-axis */
} Warp_TOOL_IMG_GRID_POINT_s;

/**
*  calibration Window information
*/
typedef struct Warp_TOOL_CAL_WIN_INFO_s_ {
    INT32 ImgWidth ;                    /**< calibration window width */
    INT32 ImgHeight ;                   /**< calibration window height */
    INT32 StartX ;                      /**< calibration window startx */
    INT32 StartY ;                      /**< calibration window starty */
    INT32 CenterX ;                     /**< optical center on x-axis of calibration window */
    INT32 CenterY ;                     /**< optical center on y-axis of calibration window */
} Warp_TOOL_CAL_WIN_INFO_s;

/**
*  ROI Window information
*/
typedef struct Warp_TOOL_ROI_WIN_INFO_s_ {
    INT32 ImgWidth ;                    /**< ROI window width */
    INT32 ImgHeight ;                   /**< ROI window height */
    INT32 StartX ;                      /**< ROI window startx */
    INT32 StartY ;                      /**< ROI window starty */
    INT32 ShiftX;                       /**< the diff with calbration & ROI window (ROIstartx-CALstartx) */
    INT32 ShiftY;                       /**< the diff with calbration & ROI window (ROIstarty-CALstarty) */
    INT32 OpticalCenterX ;              /**< optical center on x-axis of calibration window */
    INT32 OpticalCenterY ;              /**< optical center on y-axis of calibration window */
} Warp_TOOL_ROI_WIN_INFO_s;

/**
*  Warp tile information
*/
typedef struct Warp_TOOL_TILE_INFO_s_ {
    INT16 TileWidth  ;                  /**< coarse tile width */
    INT16 TileHeight ;                  /**< coarse tile height */
    INT32 HorGridNum;                   /**< coarse grid number on horizontal */
    INT32 VerGridNum;                   /**< coarse grid number on vertical */
    INT32 AddTableGridNumH;             /**< for increase coarse grid number on horizontal */
    INT32 AddTableGridNumW;             /**< for increase coarse grid number on vertical */
} Warp_TOOL_TILE_INFO_s;

/**
*  window information
*/
typedef struct Warp_TOOL_WIN_INFO_s_ {
    INT32 StartX ;                      /**< Bird eye window startx */
    INT32 StartY ;                      /**< Bird eye window starty */
    INT32 Width ;                    /**< Bird eye window width */
    INT32 Height ;                   /**< Bird eye window height */
} Warp_TOOL_WIN_INFO_s;

/**
* Bird eye information
*/
typedef struct Warp_TOOL_BirdEye_INFO_s_ {
    Warp_TOOL_WIN_INFO_s Birdeye;
    Warp_TOOL_WIN_INFO_s Car;
    Warp_TOOL_IMG_GRID_POINT_s BlendPoint[4];
} Warp_TOOL_BirdEye_INFO_s;

/**
*  Warp information date structure
*/
typedef struct Cal_Warp_Tool_Info_s_ {
    Warp_TOOL_IMG_GRID_POINT_s *Wp;     /**< warp grid point */
    Warp_TOOL_IMG_GRID_POINT_s *OldWp;  /**< old warp grid point */
    AMBA_DSP_IMG_GRID_POINT_s *MwWp;    /**< middle ware warp grid point */
    Warp_TOOL_TILE_INFO_s Tile;
    Warp_TOOL_CAL_WIN_INFO_s CAL;       /**< calibration window info */
    Warp_TOOL_ROI_WIN_INFO_s ROI[8];       /**< ROI window info */
    Warp_TOOL_BirdEye_INFO_s BirdEyeINFO;
    double *Expect;                     /**< expect buffer address */
    double *Real;                       /**< real buffer address */
    double Threshold;                   /**< xpoint thredhold */
    double UnitCellSize;                /**< unit size factor between real and raw image */
    UINT8 IdxNum;                       /**< input spec numbers */
    INT32 RatioR ;                      /**< input img ratio */
    INT32 CompensateRatio ;             /**< The compensatation for warp tool - normal dewarp*/
    INT32 CompensateRatioSwaX ;         /**< The compensatation for warp tool - keep the view angle horizontal dewarp */
    INT32 CompensateRatioSwaY ;         /**< The compensatation for warp tool - keep the view angle vertical dewarp */
    UINT32 Warp2StageFlag;              /**< The compensatation for warp tool,0:disable,1:enable warp 2 stage compensation*/
} Cal_Warp_Tool_Info_s;

/**
*  Warp error massage
*/
typedef enum _WARP_CALIB_ERROR_MSG_e_ {
    WARP_CALIB_OK            = 0,   /**< OK */
    WARP_CALIB_INCORRECT_ARGUMENT_NUM   = -1,   /**< input argument number dismatch  */
    WARP_CALIB_READSCRIPT_ERROR         = -2,   /**< read script failed */
    WARP_CALIB_INCORRECT_PARAMETER      = -3,   /**< parameter value error */
    WARP_CALIB_OPEN_FILE_ERROR          = -4,   /**< open file failed */
    WARP_CALIB_CLOSE_FILE_ERROR         = -5,   /**< close file failed */
    WARP_CALIB_INCORRECT_INPUT_NUM      = -6,   /**< input spec number wrong  */
    WARP_CALIB_TOO_MUCH_INPUT_NUM       = -7,   /**< input spec number too many */
    WARP_CALIB_INTERVAL_NOT_FOUND       = -8,   /**< can't find the interval from the script */
    WARP_CALIB_DIV0                     = -9    /**< divide 0 */
} WARP_CALIB_ERROR_MSG_e;

/**
*  Set Warp data address
*
*  @param [in] Channel channel ID
*  @param [in] pVignetteCalcInfo address of Warp calibration buffer
*
*  @return none
*/

/**
*  Get Warp data address
*
*  @param [in] Channel channel ID
*  @param [in] pVignetteCalcInfo address of Warp calibration buffer
*
*  @return 0 - OK, others - WARP_CALIB_ERROR_MSG_e
*  @see WARP_CALIB_ERROR_MSG_e
*/
//extern void AmpCalib_GetWarpInfoAddr(UINT8 Channel,UINT32 *pVignetteCalcInfo);

/**
*  Transfer the int32 to int16
*
*  @param [in] Input the number with int32 format
*
*  @return 0 - OK, others - WARP_CALIB_ERROR_MSG_e
*  @see WARP_CALIB_ERROR_MSG_e
*/
//extern INT16 AmbaCalib_INT32_to_INT16 (INT32 Input);

/**
*  Calculate the idea/keep view angle warp table
*
*  @param [in] ToolInfo the information for the warp table
*  @param [in] RawEncodeTextFlag the way to generate the warp table (0: idea 20: keep view angle)
*
*  @return ToolInfo - warp tool information
*/
extern int AmbaCalib_CalWarpTable(Cal_Warp_Tool_Info_s* ToolInfo, INT32 RawEncodeTextFlag);

/**
*  Calculate the topview idea/keep view angle warp table
*
*  @param [in] ToolInfo the information for the warp table
*  @param [in] Point warp reference point found directly from raw image          //TBD
*  @param [in] Matrix transfer matrix                                                                       //TBD local varible
*  @param [in] Debugflag flag for transfer matrix debug usage
*
*  @return ToolInfo - warp tool information
*/
extern int AmbaCalib_CalTopViewTable(Cal_Warp_Tool_Info_s* ToolInfo, Top_View_Point_s* Point, Top_View_Hmatrix_s Matrix,UINT8 Debugflag, UINT8 ChId);

//extern int AmbaCalib_CalWarpTableTwopathConpensation(Cal_Warp_Tool_Info_s* ToolInfo);
/**
 * @}
 */
