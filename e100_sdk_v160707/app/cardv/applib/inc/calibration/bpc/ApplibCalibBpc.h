/**
 * @file src/app/connected/applib/inc/calibration/bpc/ApplibCalibBpc.h
 *
 * header file for bad pixel calibration
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
#include <imgcalib/AmbaCalib_BPC.h>
#include <calibration/ApplibCalibMgr.h>
// FPN_OP_UPDATE_MAP related parameter
#define BPC_UPDATE_MAP_NORMAL                   (0)
#define BPC_UPDATE_MAP_FAST                     (1)

#define BPC_CH_ALL                              0xFF
#define BPC_CH_NO                               1
#define BPC_MAP_INIT        0
#define BPC_MAP_UPDATE 1
#define BPC_TASK_PRIORITY          (75)         // Low priority task
#define BPC_TASK_LOW_PRIORITY      (90)         // Low priority task
#define BPC_MAP_PITCH           (32)    // DMA's 2D mode needs row size to be multiple of 32-byte
#define BPC_2CH_MIXED_MODE  (0xEFEF0000)

#define BPC_RECORD_BADPIXEL             (0x00000001)

#define BPC_FLIP_ON        1

#define BPC_MAX_MODES       (16)
#define BPC_MAX_BUFFER      (BPC_MAX_MODES*CALIB_CH_NO)
#define BPC_MAX_PAGES       (BPC_MAX_MODES<<1)


/**
* @defgroup ApplibCalibBpc
* @brief header file for bad pixel calibration
*
*/

/**
 * @addtogroup ApplibCalibBpc
 * @{
 */

/**
*  NAND Flash storage structure for bpc (page info)
*/
typedef struct BPC_Page_s_ {
    UINT32      PageValid;    /**< 1 when the record existed in NAND flash */
    UINT32      PageSize;     /**< bpc page size */
    UINT32      Offset;       /**< bpc page size address offset */
    UINT32      Type;         /**< bpc page type */
    UINT32      CheckSum;     /**< checksum for bpc page data */
#define BPC_RECORD_BADPIXEL             (0x00000001)
} BPC_Page_s;

/**
*  NAND Flash storage structure for bpc
*/
typedef struct BPC_Nand_s_ {
#define BPC_NAND_RESERVED (256-(5*BPC_MAX_PAGES))
    BPC_Page_s      BPCPage[BPC_MAX_PAGES];     /**< page info */
    UINT32          Reserved[BPC_NAND_RESERVED];               /**< reserved */
    void            *Data;                      /**< bpc data */
} BPC_Nand_s;


//  BPC set (combination of pages)
//typedef struct BPC_Set_s {
//    INT16    Valid;             /**< bpc */
//    INT16    BadPixelPage;      /**< bpc */
//} BPC_Set_t;

/**
*  BPC buffer management structure
*/
typedef struct BPC_Buffer_s_ {
    UINT32    MemoryAllocated;      /**< memory allocated or not */
    UINT8    *BufferAddr;           /**< address */
    UINT8   Channel;                /**< Channel id */
    UINT8   BPCMode;                /**< bpc mode */
    UINT8   Resvered;               /**< Resvered */
    UINT32    Size;                 /**< buffer size */
} BPC_Buffer_s;

/**
*  BPC buffer setup structure
*/
typedef struct BPC_Buffer_Setup_s {
    UINT32    Enable;           /**< enable flag */
    UINT32    BPCMode;          /**< bpc mode */
    UINT32    BPCPage;          /**< bpc page address */
    UINT32    ActiveBuffer;     /**< current buffer */
    UINT32    Option;           /**< bpc buffer option */
} BPC_Buffer_Setup_t;


/**
*  BPC buffer content management (the content of each buffer (content=set), lock & valid status)
*/
typedef struct BPC_Buffer_Status_s_ {
    // Following information is updated by BPC_buffer_setup_t
    int     BPCSet[BPC_MAX_BUFFER*CALIB_CH_NO];     /**< buffer's current BPC set */
    // Following infomation is BPC library internal holding information
    UINT32  Lock[BPC_MAX_BUFFER*CALIB_CH_NO];       /**< Resvered */
    int     Valid[BPC_MAX_BUFFER*CALIB_CH_NO];      /**< =1 when buffer contain map */
} BPC_Buffer_Status_s;

/* ---------- */
// Optical black Object
typedef struct BPC_OB_Offset_s_ {
    INT16   X;
    INT16   Y;
}BPC_OB_Offset_s;

/* ---------- */
// BPC Object

typedef struct BPC_Obj_s {
    // BPC related flag
    UINT32          BPCMemAllocFlag;                /**< bpc memory allocated flag */
    UINT32          BPCInitialDecompressFlag;       /**< bpc initial decompress flag */
    UINT32          BPCDebugEn;                     /**< bpc debug enable */
    UINT32          BPCDynamicPageEn;               /**< dynamic page enable */
    UINT32          BPCS3dMode;                     /**< 3d mode enable */

    BPC_Nand_s      *BPCNand;                       /**< NAND Flash storage structure */
    BPC_Buffer_s       BPCBuffer[BPC_MAX_BUFFER];      /**< BPC buffer management structure */
    BPC_Buffer_Status_s  BufferStatus;                        /**< BPC config (correlation between BPC buffer & BPC pages) */
    UINT32          ActiveBuffer[BPC_MAX_MODES*CALIB_CH_NO];    /**< Active buffer information of 1x/2x/4x map */
    int             GeometryValid[2][BPC_MAX_MODES*CALIB_CH_NO];   /**< Geometry information of 1x/2x/4x map */
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s EffectiveGeometry[2][BPC_MAX_MODES*CALIB_CH_NO];  /**< effective sensor geometry information by BPC_MODE */
    AMBA_DSP_IMG_VIN_SENSOR_GEOMETRY_s OBGeometry[2][BPC_MAX_MODES*CALIB_CH_NO];    /**< optical black sensor geometry information by BPC_MODE */
    BPC_OB_Offset_s OBOffset[2][BPC_MAX_MODES*CALIB_CH_NO];
} BPC_Obj_t;

/**
*  BPC buffer management structure
*/
typedef struct BPC_Cluster_Detection_s_ {
    int Channel;            /**< channel id */
    int Flip;               /**< flip enable */
    int BadPixelPage;       /**< bpc page id */
    int BPCMode;            /**< bpc mode */
    int BufferIndex;        /**< bpc buffer id */
    int FirstPixelGreen;    /**< first pixel green flag */
    int ReloadFromNand;     /**< reload from nand flag */
    int NgThreshold;        /**< threshold */
    int DebugFlag;          /**< debug flag */
    int DebugThd;           /**< debug threshold */
} BPC_Cluster_Detection_s;


/**
*  FPN BMP management
*/
typedef struct BMPFile_Magic_s {
    unsigned char Magic[2];     /**< 'B', 'M' */
} BMPFile_Magic_t;

/**
*  BPC buffer management structure
*/
typedef struct BMPFile_Header_s {
    UINT32 FileSize;        /**< file size */
    UINT16 Creator1;        /**< creator 1 */
    UINT16 Creator2;        /**< creator 2 */
    UINT32 BMPOffset;       /**< bmp offset */
} BMPFile_Header_t;

/**
*  BPC buffer management structure
*/
typedef struct BMPInfo_Header_s {
    UINT32 HeaderSize;      /**< bmp header size */
    int Width;              /**< bmp img width */
    int Height;             /**< bmp img height */
    UINT16 Planes;          /**< planes number */
    UINT16 Bpp;             /**< bits per pixel */
    UINT32 Compression;     /**< compression method */
    UINT32 BMPDataSize;     /**< bmp data size */
    UINT32 HResolution;     /**< horizontal resolution */
    UINT32 VResolution;     /**< vertical resolution */
    UINT32 UsedColors;      /**< used colors number */
    UINT32 ImportantColors; /**< important colors number */
} BMPInfo_Header_t;

/**
*  Bits per pixel info
*/
typedef struct BMPPalette_Bpp2_s {
    UINT32 Color[2];        /**< white or black */
} BMPPalette_Bpp2_t;

/**
*  Register calibration site
*
*  @return 0 - OK
*/
extern int AppLib_CalibSiteInit(void);

/**
*  Get dsp mode for calibration
*
*  @param [in] pMode DSP image mode config
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibGetDspMode(AMBA_DSP_IMG_MODE_CFG_s *pMode);

/**
 *  @brief update bad pixel map
 *
 *  update bad pixel map
 *
 *  @param [in]JobID initial or update BPC Map
 *
 *  @return 1 success, -1 failure
 */
extern int AppLibCalibBPC_MapUpdate(UINT8 JobID);

