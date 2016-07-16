/**
 * @file src/app/connected/applib/inc/calibration/ApplibCalibMgr.h
 *
 * header file for Calibration Manager control
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
#ifndef __CALIB_MGR_H__
#define __CALIB_MGR_H__

#include <AmbaDataType.h>
#include <imgcalib/AmbaCalib_VIG.h>
#include <imgproc/AmbaImg_AeAwb.h>
#include <imgproc/AmbaImg_AaaDef.h>
#include <imgproc/AmbaImg_Impl_Cmd.h>
#include <AmbaFS.h>
#include <recorder/VideoEnc.h>
#include <recorder/StillEnc.h>
#include <system/ApplibSys_Sensor.h>
#include <AmbaPrintk.h>
#include <AmbaDSP.h>
#include <AmbaSensor.h>
#include <AmbaUtility.h>
#include <calibration/ApplibCalib_Adjustment.h>

/**
* @defgroup ApplibCalibMgr
* @brief Header file for Calibration Manager control
*
*/

/**
 * @addtogroup ApplibCalibMgr
 * @{
 */
#define CALIB_TKP

//#define APP_CAL_DBG
#ifdef APP_CAL_DBG
#define CAL_PRINT        AmbaPrint
#else
#define CAL_PRINT(...)
#endif

#define CALIB_SCRIPT "bad.txt"

//projectID
#define CALIB_PROJECT_NAME "A12_ARD"

#define CALIB_CH_ALL 0xFF

#define MAX_VIGNETTE_ROM_TABLE_COUNT 0
#define MAX_VIGNETTE_NAND_TABLE_COUNT 10
#define MAX_VIGNETTE_GAIN_TABLE_COUNT (MAX_VIGNETTE_NAND_TABLE_COUNT + MAX_VIGNETTE_ROM_TABLE_COUNT)
#define VIGNETTE_STORAGE_BLEND_NUM_MAX (MAX_VIGNETTE_GAIN_TABLE_COUNT)

#define CALIB_PARTIAL_LOAD 0
#define CALIB_FULL_LOAD 0xFF

/**
*  Vignette map config info
*/
typedef struct Vignette_Global_Map_Config_Info_s_ {
    UINT8  NumVignetteTable;            /**< vignette table count */
    //UINT16 BlendRatio[VIGNETTE_STORAGE_BLEND_NUM_MAX]; //0~256
    UINT16 BlendRatio;                  /**< vignette blend ratio */
    UINT8 Id[2];                        /**< index */
    UINT8 Padding[3];                   /**< reserved */
    UINT32 Strength;                    /**< vignette effect strength */
    Vignette_Pack_Storage_s *VignetteStorageAddress[VIGNETTE_STORAGE_BLEND_NUM_MAX];   /**< vignette data address */
} Vignette_Global_Map_Config_Info_s;

#ifdef MULTI_CHANNEL_VIN
//#define CALIB_CH_NO 5 //for B5bub
#define CALIB_CH_NO 9 //for B5bub
#else
#define CALIB_CH_NO 1 //for single chan
#endif
#define SITE_NUM (15)



/* Define Calibration source */
#define CALIB_SOURCE_NAND (0)
#define CALIB_SOURCE_ROMFS (1)
#define CALIB_SOURCE_BOTH (2)
#define CALIB_SOURCE_SDCard (3)

/*Calibration load data format*/

#define CAL_STATUS_LOAD_FORMAT         (CALIB_SOURCE_NAND)
#define CAL_AF_LOAD_FORMAT          (CALIB_SOURCE_NAND)
#define CAL_GYRO_LOAD_FORMAT        (CALIB_SOURCE_NAND)
#define CAL_MSHUTTER_LOAD_FORMAT    (CALIB_SOURCE_NAND)
#define CAL_IRIS_LOAD_FORMAT          (CALIB_SOURCE_NAND)
#define CAL_VIGNETTE_LOAD_FORMAT    (CALIB_SOURCE_NAND)
#define CAL_FPN_LOAD_FORMAT          (CALIB_SOURCE_NAND)
#define CAL_WB_LOAD_FORMAT          (CALIB_SOURCE_NAND)
#define CAL_ISO_LOAD_FORMAT          (CALIB_SOURCE_NAND)
#define CAL_BLC_LOAD_FORMAT          (CALIB_SOURCE_NAND)
#define CAL_FLASH_LOAD_FORMAT          (CALIB_SOURCE_NAND)
#define CAL_AUDIO_LOAD_FORMAT          (CALIB_SOURCE_NAND)
#define CAL_WARP_LOAD_FORMAT          (CALIB_SOURCE_ROMFS)
#define CAL_CA_LOAD_FORMAT             (CALIB_SOURCE_ROMFS)
#define CAL_LENSSHIFT_LOAD_FORMAT    (CALIB_SOURCE_NAND)

/* ---------- */
// ID, SIZE, VER of each calibration item

#define CAL_STATUS_ID                   (0)
#define CAL_STATUS_SIZE                 (4096)
#define CAL_STATUS_VER                  (0)

#define CAL_AF_ID                       (1)
#define CAL_AF_SIZE                     (2048)
#define CAL_AF_VER                      (0x20130730)

#define CAL_GYRO_ID                     (2)
#define CAL_GYRO_SIZE                   (512)
#define CAL_GYRO_VER                    (0x20130730)

#define CAL_MSHUTTER_ID                 (3)
#define CAL_MSHUTTER_SIZE               (512)
#define CAL_MSHUTTER_VER                (0x20130730)

#define CAL_IRIS_ID                     (4)
#define CAL_IRIS_SIZE                   (512)
#define CAL_IRIS_VER                    (0x20130730)

#define CAL_VIGNETTE_ID                 (5)
#define CAL_VIGNETTE_SIZE               (17*1024*MAX_VIGNETTE_GAIN_TABLE_COUNT)
#define CAL_VIGNETTE_VER                AMBA_DSP_IMG_VIG_VER_1_0

#define CAL_WARP_ID                     (6)
#define CAL_WARP_SIZE                   (128*1024)
#define CAL_WARP_VER                    AMBA_DSP_IMG_WARP_VER_1_0

#define CAL_BPC_ID                      (7)
#define CAL_BPC_SIZE                    (512*1024)
#define CAL_BPC_VER                     AMBA_DSP_IMG_SBP_VER_1_0

#define CAL_WB_ID                       (8)
#define CAL_WB_SIZE                     (512)
#define CAL_WB_VER                      (0x20130730)

#define CAL_ISO_ID                      (9)
#define CAL_ISO_SIZE                    (512)
#define CAL_ISO_VER                     (0x20130730)

#define CAL_BLC_ID                      (10)
#define CAL_BLC_SIZE                    (512)
#define CAL_BLC_VER                     (0x20130730)

#define CAL_FLASH_ID                    (11)
#define CAL_FLASH_SIZE                  (1024)
#define CAL_FLASH_VER                   (0x20130730)

#define CAL_AUDIO_ID                    (12)
#define CAL_AUDIO_SIZE                  (8*1024+512)
#define CAL_AUDIO_VER                   (0x20130730)

#define CAL_CA_ID                       (13)
#define CAL_CA_SIZE                     (64*1024)
#define CAL_CA_VER                      AMBA_DSP_IMG_CAWARP_VER_1_0

#define CAL_LENSSHIFT_ID                (14)
#define CAL_LENSSHIFT_SIZE              (512)
#define CAL_LENSSHIFT_VER               (0x20130730)



#define CAL_VIGNETTE_PARTIAL_LOAD_START_ID   (20)
#define CAL_VIGNETTE_PARTIAL_ID_(x)                 (CAL_VIGNETTE_PARTIAL_LOAD_START_ID+(x))

//define the storage interfance for calibration
#define CALIB_FROM_NAND 0
#define CALIB_FROM_SD_CARD 1
#define CALIB_FROM_ROM 2
#define CALIB_STORAGE  CALIB_FROM_NAND


#define CALIB_LOAD          0
#define CALIB_INIT       1
#define CALIB_POST_LOAD 100
#define CALIB_LOAD_ROM_AFT_NAND 200
// Define calibration partial load parameters
#define CALIB_TABLE_IDX_INIT    0
#define CALIB_TABLE_IDX_POST    1
#define CALIB_TABLE_COUNT_INIT    1
// Just a flag, it's not actually load 20 tables
#define CALIB_TABLE_COUNT_POST    0

/* ---------- */
// Calibration status definition

#define CAL_MAX_SUBSITE      (8)
#define NVD_CALIB_MAX_OBJS    32

/**
*  Calibration site status
*/
typedef struct Cal_Site_Status_s_ {
    UINT32 Version;     /**< version */
    UINT32 Status;      /**< two state: reset/done */
#define CAL_SITE_DONE_VIGNETTE_LOAD_ALL         (0x55FF55FF)
#define CAL_SITE_DONE_VIGNETTE_LOAD_PARTIAL     (0xAAFFAAFF)
#define CAL_VIGNETTE_SUB_SITE_DONE              (0x01)
#define CAL_SITE_DONE                           (0x55FF55FF)
#define CAL_SITE_RESET                          (0x00000000)
    UINT32 SubSiteStatus[CAL_MAX_SUBSITE];  /**< two state: reset/done */
    UINT8  Reserved[88];                    /**< reserved */ //for vignette partial save/load [0]:vignette enable [1]:tablecount [2...]:table0... dis/enable:0x00/0xFF
} Cal_Stie_Status_s; // 128byte per entry

/**
*  Calibration status
*/
typedef  struct Cal_Status_s_ {
    Cal_Stie_Status_s    Site[NVD_CALIB_MAX_OBJS];      /**< calibration site status */
}  Cal_Status_s; // 4096 byte

/**
*  Calibration manager table
*/
typedef struct Cal_Obj_s_ {
    UINT32   Enable;                        /**< Enable flag */
    UINT32   Id;                            /**< Index */
    UINT32   Size;                          /**< calibration obj size */
    UINT32   Version;                       /**< calibration obj version */
    char       Name[16];                    /**< calibration obj name */
    UINT8    *DramShadow;                   /**< calibration obj address */
    int (*InitFunc)(struct Cal_Obj_s_*);    /**< function pointer to each calibration initial function */
    int (*UpgradeFunc)(struct Cal_Obj_s_*, struct Cal_Site_Status_s_*); /**< function pointer to each calibration upgrade function */
    int (*CalibFunc)(int Argc, char *Argv[], char *OutputStr, Cal_Stie_Status_s *CalSite, struct Cal_Obj_s_ *CalObj);   /**< function pointer to each calibration functions */
    UINT8    Reserved[3];                   /**< Reserved */
} Cal_Obj_s;

/**
*  Calibration raw capture controler
*/
typedef struct Calib_Raw_Cap_Ctrl_s_ {
    AMBA_KAL_EVENT_FLAG_t   Flag;       /**< raw capture event flag */
} Calib_Raw_Cap_Ctrl_s;

extern Calib_Raw_Cap_Ctrl_s AmbaMonitorCalibRawCap;
extern Cal_Obj_s CalObjTable[NVD_CALIB_MAX_OBJS];


/* ---------- */
// Function prototype
// Calibration manager
/**
*  Get calibration site status
*
*  @param [in] CalId calibration id
*
*  @return none
*/
extern Cal_Stie_Status_s* AppLib_CalibGetSiteStatus(UINT32 CalId);

/**
*  Get calibration site status
*
*  @param [in] CalId calibration id
*  @param [in] JobId load or init
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibInit(UINT32 CalId,UINT8 JobId);

/**
*  Get calibration object
*
*  @param [in] CalId calibration ID
*
*  @return calibration object table
*/
extern Cal_Obj_s* AppLib_CalibGetObj(UINT32 CalId);

/**
*  Set exposure value for AE
*
*  @param [in] Shutter shutter time
*  @param [in] Agc sensor gain
*  @param [in] Dgain digital gain
*
*  @return 0 - OK
*/
extern int AppLib_CalibSetExposureValue(float Shutter, float Agc, UINT32 Dgain);

/**
*  Reset calibration data to zero
*
*  @param [in] CalId calibration ID
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibMemReset(UINT32 CalId);

/**
*  Get multi lines from the text file
*
*  @param [in] Fp file pointer
*  @param [in] Buf buffer
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_MultiGetline(AMBA_FS_FILE *Fp, char Buf[]);

/**
*  The mapping for calibration table for multi-channel
*
*  @param [in] Channel channel id
*  @param [in] Id table id
*
*  @return table map id
*/
extern int AppLib_CalibTableMapping(UINT8 Channel, UINT8 Id);

/**
*  Calculate CalId object offset and required sectors in nand
*
*  @param [in] CalId calibration ID
*  @param [in] Offset nand offset for current CalId object
*  @param [in] SectorNum the required sectors in nand for CalId object
*
*  @return 0 - OK
*/
extern Cal_Obj_s* AppLib_CalibGetNandStatus(UINT32 CalId,UINT32 *Offset, UINT32 *SectorNum);

/**
*  Clear nand partition with zero initialized
*
*  @param [in] CalId calibration ID
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibNandReset(UINT32 CalId);

/**
*  Initial Calibration Nand
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibNandInit(void);
//extern int AppLib_CalibNandSaveTest(UINT32 Offset, UINT32 SecNum,UINT8 Value);
//extern int AppLib_CalibNandSaveTest1(UINT32 Offset, UINT32 SecNum,UINT8 Value);

/**
*  Check calibration table loaded flag
*
*  @param [in] CalId calibration ID
*  @param [in] TableIdx talbe ID
*
*  @return 0 - Empty, 1 - Already loaded
*/
int AppLib_CalibCheckFlag(UINT8 CalId, UINT8 TableIdx);

/**
*  Check calibration table head loaded flag
*
*  @param [in] CalId calibration ID
*  @param [in] TableIdx talbe ID
*
*  @return 0 - Empty, 1 - Already loaded
*/
int AppLib_CalibCheckHeadFlag(UINT8 CalId);

/**
*  Mark calibration table loaded flag
*
*  @param [in] CalId calibration ID
*  @param [in] TableIdx talbe ID
*
*  @return 0 - Empty, 1 - Already loaded
*/
int AppLib_CalibMarkFlag(UINT8 CalId, UINT8 TableIdx, UINT8 Flag);

/**
*  Mark calibration table head loaded flag
*
*  @param [in] CalId calibration ID
*  @param [in] TableIdx talbe ID
*
*  @return 0 - Empty, 1 - Already loaded
*/
extern int AppLib_CalibMarkHeadFlag(UINT8 CalId, UINT8 Flag);
extern void AppLib_CalibRestCheckAllFlag(void);

/**
*  Rotate 2-d array
*
*  @param [in] Map target 2-d array
*  @param [in] MapWidth map width
*  @param [in] MapHeight map height
*  @param [in] Rotate vertical rotate or horizontal flip
*
*  @return 0 - OK
*/
extern int AppLib_CalibRotateMap( UINT16 *Map, UINT16 MapWidth, UINT16 MapHeight, UINT8 Rotate);

/**
 * Get aligned byte pool
 *
 * @param [in] BytePool Byte pool
 * @param [out] AlignedPool Pool address after alignment
 * @param [out] Pool Pool address before alignment
 * @param [in] Size Pool size
 * @param [in] Alignment Alignment
 *
 * @return 0 - OK, others - error
 */
extern int AmpUtil_GetAlignedPool(AMBA_KAL_BYTE_POOL_t * BytePool, void ** AlignedPool, void ** Pool, UINT32 Size, UINT32 Alignment);

/**
 *  @brief Load the calibration data
 *
 *  Load the calibration data
 *
 *  @return 0
 */
extern int AppLibCalib_Load(void);

/**
 * Calibration initalization
 *
 *
 * @return 0 - OK, others - error
 */
extern int AppLibCalib_Init(void);

/**
 *  @brief set DSP mode for calibration
 *
 *  set DSP mode for calibration
 *
 *  @param [in]pMode DSP image mode config
 *
 *  @return 0 success, -1 failure
 */
extern int AppLibCalib_SetDspMode(AMBA_DSP_IMG_MODE_CFG_s *pMode);

extern char AppLib_CalibGetDriverLetter(void);
extern UINT8 AmpCalib_GetOpticalBlackFlag(void);
extern void AppLib_CalibRestCheckFlag(UINT8 CalId);
extern int AppLib_CalibInitFunc(UINT32 CalId,UINT8 JobId, UINT8 SubId);

/**
 *  @brief Set calibration parameter table
 *
 *  set calibration parameter table
 *
 *  @param [in]CalibTableAddr address of the calibration parameter table
 *
 *  @return 0 success, -1 failure
 */
extern int AppLibCalibTableSet(CALIBRATION_ADJUST_PARAM_s *CalibTableAddr);
#endif //__CALIB_MGR_H__
