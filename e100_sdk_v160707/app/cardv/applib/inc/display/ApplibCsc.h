/**
 * @file src/app/connected/applib/inc/display/ApplibCsc.h
 *
 * Header of display csc
 *
 * History:
 *    2014/03/27 - [Eric Yen] created file
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

#ifndef APPLIB_CSC_H_
#define APPLIB_CSC_H_

/**
* @defgroup Csc
* @brief Csc Utilities
*
*/

/**
 * @addtogroup Csc
 * @{
 */

#include <display/Display.h>
#include <applib.h>

__BEGIN_C_PROTO__

/*************************************************************************
 * Csc declaration
 ************************************************************************/
/** Csc output quantization rage */
typedef enum _APPLIB_CSC_QUANTIZATION_RANGE_e_ {
    APPLIB_CSC_QUANTIZATION_RANGE_LIMITED = 0,    /**< Limited quantization range of 256 levels (16 to 235) */
    APPLIB_CSC_QUANTIZATION_RANGE_FULL,           /**< Full quantization range of 256 levels (0 to 255) */
} APPLIB_CSC_QUANTIZATION_RANGE_e;

/*************************************************************************
 * Csc APIs
 ************************************************************************/
/**
 *
 *  Get csc matrix
 *
 *  @param [in] VoutType Type of vout device @see AMBA_DSP_VOUT_TYPE_e
 *  @param [in] ColorSpaceIn Input color space @see AMBA_DSP_COLOR_SPACE_e
 *  @param [in] ColorSpaceOut Output color space @see AMBA_DSP_COLOR_SPACE_e
 *  @param [in] QRange Quantization range @see AMP_DISP_QUANTIZATION_RANGE_e
 *  @param [in] IsHDOutput Is output HD resolution, a.k.a. ActiveRowHeight >= 720
 *  @param [out] pOutMatrix Output csc matrix
 *
 *  @return frame rate
 */
extern int AppLibCsc_GetMatrix(AMP_DISP_DEV_IDX_e VoutType,
                               AMBA_DSP_COLOR_SPACE_e ColorSpaceIn,
                               AMBA_DSP_COLOR_SPACE_e ColorSpaceOut,
                               APPLIB_CSC_QUANTIZATION_RANGE_e QRangeIn,
                               APPLIB_CSC_QUANTIZATION_RANGE_e QRangeOut,
                               UINT8 IsHDOutput,
                               AMBA_DSP_VOUT_CSC_CONFIG_s *pOutMatrix);


__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_CSC_H_ */

