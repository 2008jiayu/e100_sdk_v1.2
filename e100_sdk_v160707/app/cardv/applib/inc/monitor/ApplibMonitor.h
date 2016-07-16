/**
 * @file src/app/connected/applib/inc/monitor/ApplibMonitor.h
 *
 * Header of Monitor Utility interface.
 *
 * History:
 *    2013/09/03 - [Martin Lai] created file
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

#ifndef APPLIB_MONITOR_H_
#define APPLIB_MONITOR_H_

/**
 * @defgroup Monitor
 * @brief Monitor Utility interface.
 *
 * Implementation of
 * monitor related function
 *
 */

/**
* @defgroup ApplibMonitor
* @brief Monitor Utility interface.
*
*
*/

/**
 * @addtogroup ApplibMonitor
 * @ingroup Monitor
 * @{
 */

#include <applib.h>
__BEGIN_C_PROTO__

#define SYSTIMEDIFF(prev, curr) ((curr)>(prev))?((curr)-(prev)):(0xFFFFFFFF-(prev)+(curr)+1) /**<SYSTEM TIME DIFFERENCE*/
/**
 *
 *bit rate monitor config
 *
 */
typedef struct _Bitrate_monitor_config_s_ {
    int MonitorId;                  /**< monitor id */
    UINT8 StreamId;                 /**< stream id, 1 for primary, 2 for secondary ... */
    UINT8 Enable:1;                 /**< enable/disable brc monitor */
    UINT8 EnableDzoomChk:1;         /**< bitrate control: Dzoom */
    UINT8 EnableLumaChk:1;          /**< bitrate control: Luma */
    UINT8 EnablaFrateChg:1;         /**< bitrate control: Frame rate change */
    UINT8 EnableBWChk:1;            /**< bitrate control: bandwidth check */
    UINT8 EnableCustomChk:1;        /**< bitrate control: customer defined */
    UINT8 Reseved:2;                /**< Reseved */
    UINT8 Debug;				 /**< Debug */
    UINT8 Reserved1;             /**< Reseved */
    AMP_AVENC_HDLR_s *Hdlr;         /**< Hdlr */

    /* bitrate control: Dzoom */
    void (*DzoomCb)(UINT32 *targetBitrate, UINT32 CurrBitrate, UINT8 streamId); /**< Dzoom CB Function */
    float DzoomFactorThres; /**< Dzoom Factor Threshold */

    /* bitrate control: Luma */
    void (*LumaCb)(UINT32 *targetBitrate, UINT32 CurrBitrate, UINT8 streamId); /**< Luma CB Function */
    int LumaThres;     /**< Luma Threshold*/
    int LumaLowThres;  /**< Luma Low Threshold*/

    /* bitrate control: Frame rate change */
    UINT32 FrateUpFactorThres;/**< Frame rate Up Factor Threshold*/
    UINT32 FrateDownFactorThres;/**< Frame rate Down Factor Threshold*/
    UINT32 FrateDownFactorMax;/**< Frame rate Down Factor Max*/

    /* bitrate control: Scene complexity */
    void (*SceneComplexityCb)(UINT32 *targetBitrate, UINT32 currBitrate, UINT8 streamId);           /**<Scene Complexity Cb function*/
    int (*SceneGetDayLumaThresCb)(int mode, UINT32 *threshold);                                    /**<Scene Get Day Luma Threshold Cb function*/
    int (*SceneGetRangeCb)(int mode, UINT32 *complexMin, UINT32 *complexMid, UINT32 *complexMax);  /**<Scene Get Range Cb function*/
    void (*SceneGetPipeModeCb)(UINT8 *isVideoHdr, UINT8 *isOversample);                            /**<Scene Get Pipe Mode Cb function*/

    /* bitrate control: bandwidth */
    void (*BandwidthCb)(UINT32 *targetBitrate, UINT32 CurrBitrate, UINT8 streamId); /**<Bandwidth Cb function*/

    /* bitrate control: bandwidth */
    void (*CustomCb)(UINT32 *targetBitrate, UINT32 CurrBitrate, UINT8 streamId);/**<custom cb function*/

} Bitrate_monitor_config_s;

/**
 *  This data structure describes register AQP callback
 */
typedef struct _APP_BRC_AQP_CB_s_ {
    void (*BitRate_AQP_cb)(void);  /**<BitRate AQP cb function*/
} APP_BRC_AQP_CB_s;

/**
 *  This data structure describes register force_idr callback
 */
typedef struct _APP_BRC_FORCE_IDR_CB_s_ {
    void (*BitRate_Force_IDR_cb)(void);     /**<BitRate Force IDR cb function*/
} APP_BRC_FORCE_IDR_CB_s;

/**
 *  Config bit rate monitor
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibMonitorBitrate_Config(UINT8 mode, Bitrate_monitor_config_s *config);
#define APPLIB_BRCMON_CONFIG_BANDWIDTH (1)         /**<APPLIB_BRCMON_CONFIG_BANDWIDTH*/
#define APPLIB_BRCMON_CONFIG_DZOOM     (1<<1)      /**<APPLIB_BRCMON_CONFIG_DZOOM    */
#define APPLIB_BRCMON_CONFIG_LUMA      (1<<2)      /**<APPLIB_BRCMON_CONFIG_LUMA     */
#define APPLIB_BRCMON_CONFIG_CUSTOM    (1<<3)      /**<APPLIB_BRCMON_CONFIG_CUSTOM   */
#define APPLIB_BRCMON_CONFIG_ALL       (0xFF)      /**<APPLIB_BRCMON_CONFIG_ALL      */

/**
 *  Bit rate monitor initialization callback
 */
extern void AppLibMonitorBitrate_InitCB(void);

/**
 *  Bit rate monitor time up callback
 */
void AppLibMonitorBitrate_TimeUpCB(void);

/**
 *  Applib Monitor module initialization
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibMonitor_Init(void);

/**
 *  Initialize the HDMI Hot-Plug Monitor
 *
 *  @param [in] taskPriority priority of the collection task
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibMonitorHdmi_Init(UINT32 taskPriority);

/**
 *  Initialize the SD CARD MONITOR task.
 *
 *  @param [in] taskPriority Task priority
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibMonitorSd_Init(UINT32 taskPriority);

/**
 *  Initialize the Storage Monitor
 *
 *  @param [in] taskPriority priority of the collection task
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibMonitorStorage_Init(UINT32 taskPriority);

/**
 *  Enable monitor's flow to detect storage space.
 *
 *  @param [in] enable Enable flag
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibMonitorStorage_Enable(UINT32 enable);

/**
 *  Enable monitor's flow to send message.
 *
 *  @param [in] enable Enable flag
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibMonitorStorage_EnableMsg(UINT32 enable);


/**
 *  @brief set free space threshold
 *
 *  set free space check threshold
 *
 *  @param [in] threshold threshold size kbyte
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibMonitorStorage_SetThreshold(UINT32 threshold);


__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_MONITOR_H_ */
