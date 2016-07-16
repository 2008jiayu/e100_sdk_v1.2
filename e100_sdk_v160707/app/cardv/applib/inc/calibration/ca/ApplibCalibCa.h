/**
 * @file src/app/connected/applib/inc/calibration/ca/ApplibCalibCA.h
 *
 * header file for chromatic abbreation calibration
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
#include <imgcalib/AmbaCalib_Ca.h>

//Total 12388 bytes
#define CA_MAX_WIDTH  (64)
#define CA_MAX_HEIGHT (48)
#define CA_MAX_SIZE (CA_MAX_WIDTH*CA_MAX_HEIGHT)
// 64x48. Maximum size for 64  tile is 4032x3008
//                     for 128 tile is 8064x6016

#define MAX_CA_TABLE_COUNT (5)
#define CAL_CA_BASE          (0x00000000)
#define CAL_CA_DATA(x)       ((x) + CAL_CA_BASE)
#define CAL_CA_ENABLE        CAL_CA_DATA(0x00)
#define CAL_CA_TABLE_COUNT   CAL_CA_DATA(0x04)
#define CAL_CA_TABLE_BASE    CAL_CA_DATA(0x20)
#define CAL_CA_TABLE(x)      (CAL_CA_TABLE_BASE + sizeof(CA_Storage_s)*(x))

/**
 * @defgroup ApplibCalibCA
 * @brief header file for chromatic abbreation
 *
 */

/**
 * @addtogroup ApplibCalibCA
 * @{
 */

/**
 * Structure of ca storage header
 */
//warp information for each site, to be similar with IK
typedef __packed struct CA_Storage_Header_s_ {
    UINT32                              Enable;			/**< Function Enable control*/
    UINT32                              Version;        /* 0x20130101 */ /**< Structure Version */
    int                                 HorGridNum;     /**< Horizontal grid number */
    int                                 VerGridNum;     /**< Vertical grid number */
    int                                 TileWidthExp;   /**< Exponent of Tile Width. 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    int                                 TileHeightExp;  /**< Exponent of Tile Height. 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s  VinSensorGeo;   /**< Vin sensor geometry when calibrating */
    INT16                               RedScaleFactor; /**< Red scale factor */
    INT16                               BlueScaleFactor;/**< Blue scale factor */
    UINT32                              Reserved;       /**< Reserved for extension */
    UINT32                              Reserved1;      /**< Reserved for extension */
    UINT32                              CAZoomStep;     /**< Zoom step */
    UINT32                              CAChannel;      /**< channel ID */
} CA_Storage_Header_s;

/**
 * CA storage structure
 */

typedef struct CA_Storage_s_ {
    CA_Storage_Header_s CAHeader;	/**< CA Storage Header */
    //6KB for x,y each = 12KB
    AMBA_DSP_IMG_GRID_POINT_s CAVector[CA_MAX_SIZE]; /**< CA vectors */
} CA_Storage_s;

/**
 * CA table information
 */

typedef struct Cal_CA_Table_Info_s_ {
    AMBA_DSP_IMG_GRID_POINT_s *Wp;	/**< CA vectors */
    UINT32    GridW;				/**< Width of ca grid */
    UINT32    GridH;				/**< Height of ca grid */
    UINT32    TileW;				/**< Width of ca tile */
    UINT32    TileH;				/**< Height of ca tile */
    UINT32    ImgW;					/**< Image width */
    UINT32    ImgH;					/**< Image height */
    UINT32    StartX;				/**< Start coordinates on X-axis */
    UINT32    StartY;				/**< Start coordinates on Y-axis */
    INT32   OffSensorFlag;			/**< Enable flag of off sensor calibration */
} Cal_CA_Table_Info_s;

/**
 * CA table information
 */

typedef struct CA_Control_s_ {
    UINT32 Enable;			/**< Function Enable control */
    UINT32 CATableCount;	/**< Number of ca tables */
    UINT32 Debug;    
    UINT32 Reserved[5];		/**< Reserved for extension */
    CA_Storage_s *CATable[MAX_CA_TABLE_COUNT]; /**< CA table information */
} CA_Control_s;

/**
 * CA entry structure
 */

typedef struct Cal_CA_Entry_s_ {
    double RedHor;	/**< Red horizontal offset */
    double RedVer;	/**< Red vertical offset */
    double BlueHor;	/**< Blue horizontal offset */
    double BlueVer;	/**< Blue vertical offset */
} Cal_CA_Entry_s;

/**
 * CA calibration error message
 */
enum CA_CALIB_ERROR_MSG_e_ {
    CA_CALIB_OK                          =  0,
    CA_CALIB_INCORRECT_ARGUMENT_NUM   = -1,
    CA_CALIB_READSCRIPT_ERROR         = -2,
    CA_CALIB_INCORRECT_PARAMETER      = -3,
    CA_CALIB_OPEN_FILE_ERROR          = -4,
    CA_CALIB_CLOSE_FILE_ERROR         = -5,
    CA_CALIB_INCORRECT_INPUT_NUM      = -6,
    CA_CALIB_TOO_MUCH_INPUT_NUM       = -7,
    CA_CALIB_INTERVAL_NOT_FOUND       = -8,
    CA_CALIB_CREATE_BUFFER_ERROR      = -9,
    CA_CALIB_COMPENSATE_RATIO_ERROR   = -10,
    CA_CALIB_RRATIO_ERROR             = -11,
    CA_CALIB_CENTER_POSITION_ERROR    = -12,
    CA_CALIB_UNDEFINED_METHOD         = -13,
    CA_CALIB_WRITE_FILE_ERROR         = -14,
};

extern CA_Control_s AppCAControl;

extern int AppLib_CalibSiteInit(void);
extern int AppLib_CalibGetDspMode(AMBA_DSP_IMG_MODE_CFG_s *pMode);

/**
 * @}
 */
