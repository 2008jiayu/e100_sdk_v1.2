/**
 * @file src/app/connected/applib/inc/calibration/warp/ApplibCalibWarp.h
 *
 * header file for warp calibration
 *
 * History:
 *    07/10/2013  Allen Chiu Created
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
#include <calibration/ApplibCalibMgr.h>
#include <AmbaDSP_WarpCore.h>
#include <imgcalib/AmbaCalib_Warp.h>

//Total 12388 bytes
#define WARP_MAX_WIDTH  (80)
#define WARP_MAX_HEIGHT (80)
#define WARP_MAX_SIZE (WARP_MAX_WIDTH*WARP_MAX_HEIGHT)
// 64x48. Maximum size for 64  tile is 4032x3008
//                     for 128 tile is 8064x6016

#define MAX_WARP_TABLE_COUNT (4)
#define CAL_WARP_BASE          (0x00000000)
#define CAL_WARP_DATA(x)       ((x) + CAL_WARP_BASE)
#define CAL_WARP_ENABLE        CAL_WARP_DATA(0x00)
#define CAL_WARP_TABLE_COUNT   CAL_WARP_DATA(0x04)
#define CAL_WARP_TABLE_BASE    CAL_WARP_DATA(0x20)
#define CAL_WARP_TABLE(x)      (CAL_WARP_TABLE_BASE + sizeof(Warp_Storage_s)*(x))

/**
 * @defgroup ApplibCalibWarp
 * @brief header file for warp calibration
 *
 */

/**
 * @addtogroup ApplibCalibWarp
 * @{
 */

/**
 * Structure of warp storage header
 */

//warp information for each site, to be similar with IK
//typedef union Warp_Storage_Header_s_ {
typedef __packed struct Warp_Storage_Header_s_ {            //64 bytes
    UINT32                              Enable;			/**< Function Enable control*/
    UINT32                              Version;        /* 0x20130101 */ /**< Structure Version */
    int                                 HorGridNum;     /**< Horizontal grid number */
    int                                 VerGridNum;     /**< Vertical grid number */
    int                                 TileWidthExp;   /**< Exponent of Tile Width. 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    int                                 TileHeightExp;  /**< Exponent of Tile Height. 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  VinSensorGeo;   /**< Vin sensor geometry when calibrating */
    UINT32                              Warp2StageFlag; /**< warp 2 stage compensation */
    UINT32                              Reserved1;      /**< Reserved for extension */
    UINT32                              Reserved2;      /**< Reserved for extension */
    UINT32                              WarpZoomStep;   /**< zoom step number */
    UINT32                              WarpChannel;    /**< channel ID */
} Warp_Storage_Header_s;

/**
 * Warp storage structure
 */
typedef struct Warp_Storage_s_ {
    Warp_Storage_Header_s WarpHeader;	/**< Warp Storage Header */
    //6KB for x,y each = 12KB
    AMBA_DSP_IMG_GRID_POINT_s WarpVector[WARP_MAX_SIZE]; /**< Warp vectors */
} Warp_Storage_s;

/**
 * Warp table information
 */
typedef struct Cal_Warp_Table_Info_s_ {
    AMBA_DSP_IMG_GRID_POINT_s *Wp;	/**< Warp vectors */
    UINT32    GridW;				/**< Width of warp grid */
    UINT32    GridH;				/**< Height of warp grid */
    UINT32    TileW;				/**< Width of warp tile */
    UINT32    TileH;				/**< Height of warp tile */
    UINT32    ImgW;					/**< Image width */
    UINT32    ImgH;					/**< Image height */
    UINT32    StartX;				/**< Start coordinates on X-axis */
    UINT32    StartY;				/**< Start coordinates on Y-axis */
    INT32   OffSensorFlag;			/**< Enable flag of off sensor calibration */
} Cal_Warp_Table_Info_s;

/**
 * ca table information
 */
typedef struct Warp_Control_s_ {
    UINT32 Enable;			/**< Function Enable control */
    UINT32 WarpTableCount;	/**< Number of warp tables */
    UINT32 Debug;
    UINT32 Reserved[5];		/**< Reserved for extension */
    Warp_Storage_s *WarpTable[MAX_WARP_TABLE_COUNT]; /**< Warp table information */
} Warp_Control_s;


extern Warp_Control_s AppWarpControl;

extern int AppLib_CalibSiteInit(void);
extern int AppLib_CalibGetDspMode(AMBA_DSP_IMG_MODE_CFG_s *pMode);


/**
 * @}
 */
