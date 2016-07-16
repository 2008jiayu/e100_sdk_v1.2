/**
* @file src/app/connected/applib/inc/monitor/ApplibMonitor_Message.h
*
* Header of monitor's message.
*
* History:
*    2014/04/15 - [Martin Lai] created file
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
#ifndef APPLIB_MONITOR_MSG_H_
#define APPLIB_MONITOR_MSG_H_
/**
* @defgroup ApplibMonitor_Message
* @brief monitor related message
*
*
*/

/**
 * @addtogroup ApplibMonitor_Message
 * @ingroup Monitor
 * @{
 */

#include <applibhmi.h>

__BEGIN_C_PROTO__


/**********************************************************************/
/* MDL_APP_JACK_ID messsages                                          */
/**********************************************************************/
/**
* Partition: |31 - 27|26 - 24|23 - 16|15 -  8| 7 -  0|
*   |31 - 27|: MDL_APP_JACK_ID
*   |26 - 24|: MSG_TYPE_HMI
*   |23 - 16|: Jack type ID
*   |15 -  8|: Device type ID
*   | 7 -  0|: Event ID
**/
#define HMSG_JACK(x)    MSG_ID(MDL_APP_JACK_ID, MSG_TYPE_HMI, (x))
/** Sub-group:type of jack events */
#define HMSG_JACK_ID_POWER      (0x01)    /**<HMSG_JACK_ID_POWER*/
#define HMSG_JACK_ID_AV         (0x02)    /**<HMSG_JACK_ID_AV   */
#define HMSG_JACK_ID_CARD       (0x03)    /**<HMSG_JACK_ID_CARD */
#define HMSG_JACK_ID_AUDIO      (0x04)    /**<HMSG_JACK_ID_AUDIO*/

/** Power jack events */
#define HMSG_JACK_POWER(x)      HMSG_JACK(((UINT32)HMSG_JACK_ID_POWER << 16) | (x)) /**<HMSG_JACK_POWER(x)*/
#define HMSG_JACK_POWER_ID_BATTERY      (0x01)    /**<HMSG_JACK_POWER_ID_BATTERY*/
#define HMSG_JACK_POWER_ID_DC           (0x02)    /**<HMSG_JACK_POWER_ID_DC     */
#define HMSG_JACK_POWER_ID_FAIL         (0x03)    /**<HMSG_JACK_POWER_ID_FAIL   */

#define HMSG_JACK_POWER_BATTERY(x)      HMSG_JACK_POWER(((UINT32)HMSG_JACK_POWER_ID_BATTERY << 8) | (x)) /**<HMSG_JACK_POWER_BATTERY(x) */
#define HMSG_BETTERY_IN_CLR             HMSG_JACK_POWER_BATTERY(0x00)   /**<HMSG_BETTERY_IN_CLR*/
#define HMSG_BETTERY_IN_SET             HMSG_JACK_POWER_BATTERY(0x01)   /**<HMSG_BETTERY_IN_SET*/

#define HMSG_JACK_POWER_DC(x)   HMSG_JACK_POWER(((UINT32)HMSG_JACK_POWER_ID_DC << 8) | (x)) /**<HMSG_JACK_POWER_DC(x)*/
#define HMSG_DC_IN_CLR          HMSG_JACK_POWER_DC(0x00)   /**<HMSG_DC_IN_CLR*/
#define HMSG_DC_IN_SET          HMSG_JACK_POWER_DC(0x01)   /**<HMSG_DC_IN_SET*/

#define HMSG_JACK_POWER_FAIL(x) HMSG_JACK_POWER(((UINT32)HMSG_JACK_POWER_ID_FAIL << 8) | (x)) /**<HMSG_JACK_POWER_FAIL(x)*/
#define HMSG_POWER_FAIL_CLR     HMSG_JACK_POWER_FAIL(0x00)  /**<HMSG_POWER_FAIL_CLR*/
#define HMSG_POWER_FAIL_SET     HMSG_JACK_POWER_FAIL(0x01)  /**<HMSG_POWER_FAIL_SET*/

/** A/V jack events */
#define HMSG_JACK_AV(x)         HMSG_JACK(((UINT32)HMSG_JACK_ID_AV << 16) | (x)) /**<HMSG_JACK_AV(x)*/
#define HMSG_JACK_AV_ID_AV      (0x01)   /**<HMSG_JACK_AV_ID_AV   */
#define HMSG_JACK_AV_ID_HDMI    (0x02)   /**<HMSG_JACK_AV_ID_HDMI */
#define HMSG_JACK_AV_ID_CS      (0x03)   /**<HMSG_JACK_AV_ID_CS   */
#define HMSG_JACK_AV_ID_MULTI   (0x04)   /**<HMSG_JACK_AV_ID_MULTI*/

#define HMSG_JACK_AV_AV(x)      HMSG_JACK_AV(((UINT32)HMSG_JACK_AV_ID_AV << 8) | (x)) /**<HMSG_JACK_AV_AV(x)*/
#define HMSG_AV_JACK_INSERT_CLR     HMSG_JACK_AV_AV(0x00)  /**<HMSG_AV_JACK_INSERT_CLR*/
#define HMSG_AV_JACK_INSERT_SET     HMSG_JACK_AV_AV(0x01)  /**<HMSG_AV_JACK_INSERT_SET*/

#define HMSG_JACK_AV_HDMI(x)    HMSG_JACK_AV(((UINT32)HMSG_JACK_AV_ID_HDMI << 8) | (x)) /**<HMSG_JACK_AV_HDMI(x)*/
#define HMSG_HDMI_INSERT_CLR        HMSG_JACK_AV_HDMI(0x00)  /**<HMSG_HDMI_INSERT_CLR*/
#define HMSG_HDMI_INSERT_SET        HMSG_JACK_AV_HDMI(0x01)  /**<HMSG_HDMI_INSERT_SET*/

#define HMSG_JACK_AV_CS(x)      HMSG_JACK_AV(((UINT32)HMSG_JACK_AV_ID_CS << 8) | (x)) /**<HMSG_JACK_AV_CS(x)*/
#define HMSG_CS_INSERT_CLR          HMSG_JACK_AV_CS(0x00)  /**<HMSG_CS_INSERT_CLR*/
#define HMSG_CS_INSERT_SET          HMSG_JACK_AV_CS(0x01)  /**<HMSG_CS_INSERT_SET*/

#define HMSG_JACK_AV_MULTI(x)   HMSG_JACK_AV(((UINT32)HMSG_JACK_AV_ID_MULTI << 8) | (x)) /**<HMSG_JACK_AV_MULTI(x)*/
#define HMSG_MULTI_INSERT_CLR       HMSG_JACK_AV_MULTI(0x00)   /**<HMSG_MULTI_INSERT_CLR*/
#define HMSG_MULTI_INSERT_SET       HMSG_JACK_AV_MULTI(0x01)   /**<HMSG_MULTI_INSERT_SET*/
#define HMSG_ERASE_MULTI_FLAG   HMSG_JACK_AV_MULTI(0x02)       /**<HMSG_ERASE_MULTI_FLAG*/


/** Card jack events */
#define HMSG_JACK_CARD(x)       HMSG_JACK(((UINT32)HMSG_JACK_ID_CARD << 16) + (x))
/**
* Card ID and event ID use definitions in AmbaCardManager.h directly.
**/
#define HMSG_JACK_CARD_NAND0(x)      HMSG_JACK_CARD(((UINT32)SCM_SLOT_FL0 << 8) | (x))    /**<HMSG_JACK_CARD_NAND0(x*/
#define HMSG_NAND0_CARD_INSERT       HMSG_JACK_CARD_NAND0(SCM_CARD_INSERTED)              /**<HMSG_NAND0_CARD_INSERT*/
#define HMSG_NAND0_CARD_REMOVE       HMSG_JACK_CARD_NAND0(SCM_CARD_EJECTED)               /**<HMSG_NAND0_CARD_REMOVE*/
#define HMSG_NAND0_CARD_SUSPEND      HMSG_JACK_CARD_NAND0(SCM_CARD_SUSPENDED)             /**<HMSG_NAND0_CARD_SUSPEN*/
#define HMSG_NAND0_CARD_RESUME       HMSG_JACK_CARD_NAND0(SCM_CARD_RESUMED)               /**<HMSG_NAND0_CARD_RESUME*/
                                                                                          /**<                      */
#define HMSG_JACK_CARD_NAND1(x)      HMSG_JACK_CARD(((UINT32)SCM_SLOT_FL1 << 8) | (x))    /**<HMSG_JACK_CARD_NAND1(x*/
#define HMSG_NAND1_CARD_INSERT       HMSG_JACK_CARD_NAND1(SCM_CARD_INSERTED)              /**<HMSG_NAND1_CARD_INSERT*/
#define HMSG_NAND1_CARD_REMOVE       HMSG_JACK_CARD_NAND1(SCM_CARD_EJECTED)               /**<HMSG_NAND1_CARD_REMOVE*/
#define HMSG_NAND1_CARD_SUSPEND      HMSG_JACK_CARD_NAND1(SCM_CARD_SUSPENDED)             /**<HMSG_NAND1_CARD_SUSPEN*/
#define HMSG_NAND1_CARD_RESUME       HMSG_JACK_CARD_NAND1(SCM_CARD_RESUMED)               /**<HMSG_NAND1_CARD_RESUME*/
                                                                                          /**<                      */
#define HMSG_JACK_CARD_SD0(x)        HMSG_JACK_CARD(((UINT32)SCM_SLOT_SD0 << 8) | (x))    /**<HMSG_JACK_CARD_SD0(x) */
#define HMSG_SD0_CARD_INSERT         HMSG_JACK_CARD_SD0(SCM_CARD_INSERTED)                /**<HMSG_SD0_CARD_INSERT  */
#define HMSG_SD0_CARD_REMOVE         HMSG_JACK_CARD_SD0(SCM_CARD_EJECTED)                 /**<HMSG_SD0_CARD_REMOVE  */
#define HMSG_SD0_CARD_SUSPEND        HMSG_JACK_CARD_SD0(SCM_CARD_SUSPENDED)               /**<HMSG_SD0_CARD_SUSPEND */
#define HMSG_SD0_CARD_RESUME         HMSG_JACK_CARD_SD0(SCM_CARD_RESUMED)                 /**<HMSG_SD0_CARD_RESUME  */
                                                                                          /**<                      */
#define HMSG_JACK_CARD_SD1(x)       HMSG_JACK_CARD(((UINT32)SCM_SLOT_SD1 << 8) | (x))     /**<HMSG_JACK_CARD_SD1(x) */
#define HMSG_SD1_CARD_INSERT        HMSG_JACK_CARD_SD1(SCM_CARD_INSERTED)                 /**<HMSG_SD1_CARD_INSERT  */
#define HMSG_SD1_CARD_REMOVE        HMSG_JACK_CARD_SD1(SCM_CARD_EJECTED)                  /**<HMSG_SD1_CARD_REMOVE  */
#define HMSG_SD1_CARD_SUSPEND       HMSG_JACK_CARD_SD1(SCM_CARD_SUSPENDED)                /**<HMSG_SD1_CARD_SUSPEND */
#define HMSG_SD1_CARD_RESUME        HMSG_JACK_CARD_SD1(SCM_CARD_RESUMED)                  /**<HMSG_SD1_CARD_RESUME  */
                                                                                          /**<                      */
#define HMSG_JACK_CARD_RD(x)        HMSG_JACK_CARD(((UINT32)SCM_SLOT_RD << 8) | (x))      /**<HMSG_JACK_CARD_RD(x)  */
#define HMSG_RD_CARD_INSERT         HMSG_JACK_CARD_RD(SCM_CARD_INSERTED)                  /**<HMSG_RD_CARD_INSERT   */
#define HMSG_RD_CARD_REMOVE         HMSG_JACK_CARD_RD(SCM_CARD_EJECTED)                   /**<HMSG_RD_CARD_REMOVE   */
#define HMSG_RD_CARD_SUSPEND        HMSG_JACK_CARD_RD(SCM_CARD_SUSPENDED)                 /**<HMSG_RD_CARD_SUSPEND  */
#define HMSG_RD_CARD_RESUME         HMSG_JACK_CARD_RD(SCM_CARD_RESUMED)                   /**<HMSG_RD_CARD_RESUME   */


/** Audio jack events */
#define HMSG_JACK_AUDIO(x)              HMSG_JACK(((UINT32)HMSG_JACK_ID_AUDIO << 16) | (x)) /**<HMSG_JACK_AUDIO(x)*/
#define HMSG_JACK_AUDIO_ID_LINEIN       (0x01) /**<HMSG_JACK_AUDIO_ID_LINEIN */
#define HMSG_JACK_AUDIO_ID_LINEOUT      (0x02) /**<HMSG_JACK_AUDIO_ID_LINEOUT*/
#define HMSG_JACK_AUDIO_ID_HP           (0x03) /**<HMSG_JACK_AUDIO_ID_HP     */

#define HMSG_JACK_AUDIO_LINEIN(x)       HMSG_JACK_AUDIO(((UINT32)HMSG_JACK_AUDIO_ID_LINEIN << 8) | (x)) /**<HMSG_JACK_AUDIO_LINEIN(x)*/
#define HMSG_LINEIN_IN_CLR              HMSG_JACK_AUDIO_LINEIN(0x00)   /**<HMSG_LINEIN_IN_CLR*/
#define HMSG_LINEIN_IN_SET              HMSG_JACK_AUDIO_LINEIN(0x01)   /**<HMSG_LINEIN_IN_SET*/

#define HMSG_JACK_AUDIO_LINEOUT(x)      HMSG_JACK_AUDIO(((UINT32)HMSG_JACK_AUDIO_ID_LINEOUT << 8) | (x)) /**<HMSG_JACK_AUDIO_LINEOUT(x)*/
#define HMSG_LINEOUT_IN_CLR             HMSG_JACK_AUDIO_LINEOUT(0x00) /**<HMSG_LINEOUT_IN_CLR*/
#define HMSG_LINEOUT_IN_SET             HMSG_JACK_AUDIO_LINEOUT(0x01) /**<HMSG_LINEOUT_IN_SET*/

#define HMSG_JACK_AUDIO_HP(x)   HMSG_JACK_AUDIO(((UINT32)HMSG_JACK_AUDIO_ID_HP << 8) | (x)) /**<HMSG_JACK_AUDIO_HP(x)*/
#define HMSG_HP_IN_CLR          HMSG_JACK_AUDIO_HP(0x00)    /**<HMSG_HP_IN_CLR*/
#define HMSG_HP_IN_SET          HMSG_JACK_AUDIO_HP(0x01)    /**<HMSG_HP_IN_SET*/

__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_MONITOR_MSG_H_ */

