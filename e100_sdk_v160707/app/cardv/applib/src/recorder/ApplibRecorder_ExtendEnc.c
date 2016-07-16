/**
 * @file src/app/connected/applib/src/recorder/ApplibRecorder_ExtendEnc.c
 *
 * Header of extend encode Utilities
 *
 * History:
 *    2015/03/10 - [HsunYing Huang] created file
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

#include <fifo/Fifo.h>
#include <comsvc/ApplibComSvc_MemMgr.h>

#include <recorder/ExtEnc.h>   // extend data, such as GPS
#include "recorder/ApplibRecorder_ExtendEnc.h"

/* Extend Enc */
#define EXTEND_ENC_INFO_SIZE        (156)//(128)
#define EXTEND_ENC_BITS_BUUFER_SIZE (16 << 10)      // >= 16KB
#define EXTEND_ENC_DESC_BUUFER_SIZE (8 << 10)        // 256

static void *ExtendEncWorkingBuffer = NULL;
static void *ExtendEncWorkingBufferRaw = NULL;
static void *ExtendEncBitsBuffer = NULL;
static void *ExtendEncBitsBufferRaw = NULL;
static void *ExtendEncDescBuffer = NULL;
static void *ExtendEncDescBufferRaw = NULL;

static UINT16 ExtendEncFramerateScale = 100;//1000; // TEST: 10 fps
static UINT8 ExtendEncEventDataReadySkipNum = 0;

static UINT8 ExtendEncEnableFlag = 1;
static UINT8 ExtendEncInitFlag = 0;  // for extend encode module
AMP_AVENC_HDLR_s *VideoEncExtendHdlr = NULL;

static char ExtendEncInfoData[EXTEND_ENC_INFO_SIZE] = {0};
static AMP_EXTENC_HDLR_CFG_s ExtendEncHdlrCfg = {0};
static APPLIB_EXTENC_GETINFO_CB_f GetInfoCB = NULL;
/*************************************************************************
 * APIs
 ************************************************************************/
static int _ExtendEnc_EventCB(void *hdlr,UINT32 event, void *info)
{
    switch (event) {
        case AMP_ENC_EVENT_DATA_OVER_RUNOUT_THRESHOLD:
            //AmbaPrint("<_ExtendEnc_EventCB> AMP_ENC_EVENT_DATA_OVER_RUNOUT_THRESHOLD");
            break;
        case AMP_ENC_EVENT_DATA_OVERRUN:
            //AmbaPrint("<_ExtendEnc_EventCB> AMP_ENC_EVENT_DATA_OVERRUN");
            break;
        case AMP_ENC_EVENT_DESC_OVERRUN:
            //AmbaPrint("<_ExtendEnc_EventCB> AMP_ENC_EVENT_DESC_OVERRUN");
            break;
        default:
            AmbaPrint("<_ExtendEnc_EventCB> Unknown 0x%X info: %x", event, info);
            break;
    }
    return AMP_OK;
}

static int _ExtendEnc_GetInfoCB(UINT32 *size, UINT8** ptr)
{
    int len = 0;
    int i;
    extern UINT8 gps_raw_data_gprmc[128];
    UINT8 radar_radar_data[sizeof(K_RADAR_OBJ_INFO)*2+1];//2+16];
    memset(ExtendEncInfoData, 0, EXTEND_ENC_INFO_SIZE);
#ifdef CONFIG_ECL_RADAR_MODEL
    #if 0
    static K_RADAR_OBJ_INFO *     pRadarObjData;
    pRadarObjData=AppLibSysGps_GetObjData();
    len=sizeof(K_RADAR_OBJ_INFO)*2;
    for(i=0;i<sizeof(K_RADAR_OBJ_INFO);i++)
    {
        UINT8 dataval = ((UINT8 *)pRadarObjData)[i];
      
        radar_radar_data[2*i] =(dataval&0x0f)+48 ;
        radar_radar_data[2*i+1] = ((dataval>>4)&0x0f)+48;
       
    }
    radar_radar_data[len]='\0';
    strncpy(ExtendEncInfoData,radar_radar_data,len+1);
    #else
    sprintf(ExtendEncInfoData," \n");
    #endif

#else
    len = strlen(gps_raw_data_gprmc);
    if(len == 0){
        sprintf(ExtendEncInfoData,"X0000.0000Y0000.0000Z0000.0000G0000.0000$GPRMC,000125,V,,,,,000.0,,280908,002.1,N*71~, %d  \n", AmbaSysTimer_GetTickCount());
    }else if(len < EXTEND_ENC_INFO_SIZE) {
        strncpy(ExtendEncInfoData,gps_raw_data_gprmc,len);
        ExtendEncInfoData[len] = '\0';
    }else {
        strncpy(ExtendEncInfoData,gps_raw_data_gprmc,EXTEND_ENC_INFO_SIZE-1);
        ExtendEncInfoData[EXTEND_ENC_INFO_SIZE-1] = '\0';
    }
#endif
    //AmbaPrintColor(GREEN, "GPS %s", ExtendEncInfoData);
    (*size) = strlen(ExtendEncInfoData);
    (*ptr) = (UINT8 *)ExtendEncInfoData;
    return AMP_OK;
}

static int _ExtendEnc_EraseFifoCB(void *hdlr, UINT32 event, void* info)
{
    switch (event) {
        case AMP_FIFO_EVENT_DATA_READY:
            //AmbaPrint("<_ExtendEnc_EraseFifoCB> AMP_FIFO_EVENT_DATA_READY");
            break;
        case AMP_FIFO_EVENT_DATA_EOS:
            //AmbaPrint("<_ExtendEnc_EraseFifoCB> AMP_FIFO_EVENT_DATA_EOS");
            break;
        default:
            AmbaPrint("<_ExtendEnc_EraseFifoCB> Unknown 0x%X info: %x", event, info);
            break;
    }
    return 0;
}

/**
 *  @brief Int extend encode module
 *
 *  Int extend encode module
 *
 *  @return 0 success, <1 fail
 */
int AppLibExtendEnc_Init(void)
{
    int ReturnValue = AMP_OK;
    AMP_EXTENC_INIT_CFG_s ExtendEncInitCfg = {0};

    /* Error check. */
    if (!ExtendEncEnableFlag) {
        AmbaPrintColor(BLUE, "<_ExtendEnc_Init> Disable extend encode module.");
        return AMP_OK;
    }
    if (ExtendEncInitFlag) {
        AmbaPrintColor(BLUE, "<_ExtendEnc_Init> Extend encode module is alreay inited.");
        return AMP_OK;
    }
    if (VideoEncExtendHdlr) {
        AmbaPrintColor(BLUE, "<_ExtendEnc_Init> Extend encode module alreay exists.");
        return AMP_OK;
    }

    /* To initialize extend encode module. */
    AmpExtEnc_GetInitDefaultCfg(&ExtendEncInitCfg);
    if (ExtendEncWorkingBuffer == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&ExtendEncWorkingBuffer, (void **)&ExtendEncWorkingBufferRaw, ExtendEncInitCfg.MemoryPoolSize, 32);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "<_ExtendEnc_Init> Out of memory for ext!!");
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    ExtendEncInitCfg.MemoryPoolAddr = ExtendEncWorkingBuffer;
    AmpExtEnc_Init(&ExtendEncInitCfg);
    ExtendEncInitFlag = 1;

    /* To create extend encode object. */
    AmpExtEnc_GetDefaultCfg(&ExtendEncHdlrCfg);
    ExtendEncHdlrCfg.EventDataReadySkipNum = ExtendEncEventDataReadySkipNum;
    ExtendEncHdlrCfg.EncoderTaskPriority = 15; // TBD
    ExtendEncHdlrCfg.InputTaskPriority = 12; // TBD
    ExtendEncHdlrCfg.MainTaskInfo.Priority = 31; // TBD
    ExtendEncHdlrCfg.MainTaskInfo.StackSize = 0x2000; //TBD
    ExtendEncHdlrCfg.rate = 1000;
    ExtendEncHdlrCfg.scale = ExtendEncFramerateScale;
    ExtendEncHdlrCfg.cbEvent = _ExtendEnc_EventCB;
    if (GetInfoCB == NULL) {
        ExtendEncHdlrCfg.cbExtGetInfo =_ExtendEnc_GetInfoCB;
    } else {
        ExtendEncHdlrCfg.cbExtGetInfo = GetInfoCB;
    }

    // Assign bitstream/descriptor buffer
    if (ExtendEncBitsBuffer == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&ExtendEncBitsBuffer, (void **)&ExtendEncBitsBufferRaw, EXTEND_ENC_BITS_BUUFER_SIZE, 32);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "<_ExtendEnc_Init> Out of memory for ext!!");
        }
    }
    ExtendEncHdlrCfg.BitsBufCfg.BitsBufAddr = ExtendEncBitsBuffer;
    ExtendEncHdlrCfg.BitsBufCfg.BitsBufSize = EXTEND_ENC_BITS_BUUFER_SIZE;
    if (ExtendEncDescBuffer == NULL) {
        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&ExtendEncDescBuffer, (void **)&ExtendEncDescBufferRaw, EXTEND_ENC_DESC_BUUFER_SIZE, 32);
        if (ReturnValue != AMP_OK) {
            AmbaPrintColor(RED, "<_ExtendEnc_Init> Out of memory for ext!!");
        }
    }
    ExtendEncHdlrCfg.BitsBufCfg.DescBufAddr = ExtendEncDescBuffer;
    ExtendEncHdlrCfg.BitsBufCfg.DescBufSize = EXTEND_ENC_DESC_BUUFER_SIZE;
    ExtendEncHdlrCfg.BitsBufCfg.BitsRunoutThreshold = ExtendEncHdlrCfg.BitsBufCfg.BitsBufSize - (1 << 10);  // leave 1 KB
    // Assign bitstream-specific configs
    AmpExtEnc_Create(&ExtendEncHdlrCfg, &VideoEncExtendHdlr);

    return AMP_OK;
}

/**
 *  @brief Unint extend encode module
 *
 *  Unint extend encode module
 *
 *  @return success
 */
int AppLibExtendEnc_UnInit(void)
{
	AMP_FIFO_HDLR_s *TempExtVirtualFifoHdlr = NULL;
	AMP_FIFO_CFG_s ExtFifoDefCfg = {0};

    /* Error Check. */
    if (!ExtendEncInitFlag) {
        AmbaPrintColor(BLUE, "<_ExtendEnc_UnInit> Extend encode module doesn't exist.");
        return AMP_OK;
    }

    /* To create temp FIFO. */
	AmpFifo_GetDefaultCfg(&ExtFifoDefCfg);
	ExtFifoDefCfg.hCodec = VideoEncExtendHdlr;
	ExtFifoDefCfg.IsVirtual = 1;
	ExtFifoDefCfg.NumEntries = 1024;
	ExtFifoDefCfg.cbEvent = _ExtendEnc_EraseFifoCB;
	AmpFifo_Create(&ExtFifoDefCfg, &TempExtVirtualFifoHdlr);

    /* To delete the temp FIFO. */
	AmpFifo_EraseAll(TempExtVirtualFifoHdlr);
	AmpFifo_Delete(TempExtVirtualFifoHdlr);

    return AMP_OK;
}

/**
 *  @brief Set extend encode info callback
 *
 *  Set extend encode module info callback
 *
 *  @return success
 */
int AppLibExtendEnc_SetInfoCB(APPLIB_EXTENC_GETINFO_CB_f InfoCB)
{
    GetInfoCB = InfoCB;
    return AMP_OK;
}

/**
 *  @brief Set extend encode module enable/disable status
 *
 *  Set extend encode module enable/disable status
 *
 *  @return success
 */
int AppLibExtendEnc_SetEnableStatus(UINT8 enableExtendEnc)
{
    ExtendEncEnableFlag = enableExtendEnc;
    AmbaPrint("<_ExtendEnc_SetEnableStatus> Enable flag: %d", ExtendEncEnableFlag);
    return AMP_OK;
}

/**
 *  @brief Get extend encode module enable/disable status
 *
 *  Get extend encode module enable/disable status
 *
 *  @return 0 disable, >1 enable
 */
UINT8 AppLibExtendEnc_GetEnableStatus(void)
{
    AmbaPrint("<_ExtendEnc_GetEnableStatus> Enable flag: %d", ExtendEncEnableFlag);
    return ExtendEncEnableFlag;
}

/**
 *  @brief Set video bits buffer frequency for extend data (unit: ms)
 *
 *  Set video bits buffer frequency for extend data
 *  And the unit is ms
 *
 *  @return success
 */
int AppLibExtendEnc_SetFrequency(UINT16 period)
{
    AMP_EXTENC_CFG_s cfg = {0};

    /* Error Check. */
    if (!ExtendEncInitFlag) {
        AmbaPrintColor(BLUE, "<_ExtendEnc_SetFrequency> Extend encode module doesn't exist.");
        return AMP_OK;
    }

    ExtendEncFramerateScale = period;

    cfg.EventDataReadySkipNum = ExtendEncEventDataReadySkipNum;
    cfg.rate = 1000;
    cfg.scale = ExtendEncFramerateScale;
    AmbaPrintColor(BLUE, "<_ExtendEnc_SetFrequency> %d ms.", period);

    AmpExtEnc_Config(VideoEncExtendHdlr, &cfg);


    return AMP_OK;
}

/**
 *  @brief Get video bits buffer information for extend data
 *
 *  Get video bits buffer information for extend data
 *
 *  @return success
 */
int AppLibExtendEnc_GetConfigure(APPLIB_VIDEOENC_EXTEND_BITS_BUFFER_SETTING_s *ExtDataSettings)
{
    ExtDataSettings->BufferAddress = ExtendEncBitsBuffer;
    ExtDataSettings->BufferSize = EXTEND_ENC_BITS_BUUFER_SIZE;
    ExtDataSettings->FrameRate = 1000;
    ExtDataSettings->FrameRateScale = ExtendEncFramerateScale;
    return AMP_OK;
}
