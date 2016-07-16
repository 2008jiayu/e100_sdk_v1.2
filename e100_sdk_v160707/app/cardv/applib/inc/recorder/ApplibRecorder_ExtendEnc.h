/**
 * @file src/app/connected/applib/inc/recorder/ApplibRecorder_ExtendEnc.h
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

#ifndef APPLIB_EXTEND_ENC_H_
#define APPLIB_EXTEND_ENC_H_

/*************************************************************************
 * Defination
 ************************************************************************/
/**
 * Ext info callback
 */
typedef int (*APPLIB_EXTENC_GETINFO_CB_f)(UINT32 *size, UINT8** ptr);

/*************************************************************************
 * APIs
 ************************************************************************/
/**
 *  @brief Int extend encode module
 *
 *  Int extend encode module
 *
 *  @return 0 success, <1 fail
 */
extern int AppLibExtendEnc_Init(void);

/**
 *  @brief Unint extend encode module
 *
 *  Unint extend encode module
 *
 *  @return success
 */
extern int AppLibExtendEnc_UnInit(void);

/**
 *  @brief Set extend encode module info callback
 *
 *  Set extend encode module info callback
 *
 *  @return success
 */
extern int AppLibExtendEnc_SetInfoCB(APPLIB_EXTENC_GETINFO_CB_f GetInfoCB);

/**
 *  @brief Set extend encode module enable/disable status
 *
 *  Set extend encode module enable/disable status
 *
 *  @return success
 */
extern int AppLibExtendEnc_SetEnableStatus(UINT8 enableExtendEnc);

/**
 *  @brief Get extend encode module enable/disable status
 *
 *  Get extend encode module enable/disable status
 *
 *  @return 0 disable, >1 enable
 */
extern UINT8 AppLibExtendEnc_GetEnableStatus(void);

/**
 *  @brief Set video bits buffer frequency for extend data (unit: ms)
 *
 *  Set video bits buffer frequency for extend data
 *  And the unit is ms
 *
 *  @return success
 */
extern int AppLibExtendEnc_SetFrequency(UINT16 period);

/**
 *  @brief Get video bits buffer information for extend data
 *
 *  Get video bits buffer information for extend data
 *
 *  @return success
 */
extern int AppLibExtendEnc_GetConfigure(APPLIB_VIDEOENC_EXTEND_BITS_BUFFER_SETTING_s *ExtDataSettings);

#endif /* APPLIB_EXTEND_ENC_H_ */

