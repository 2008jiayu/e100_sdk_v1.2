/**
 * @file src/app/connected/applib/inc/utility/ApplibUtility_SW_Scalar.h
 *
 * Header of SW scalar
 *
 * History:
 *    2014/03/05 - [Hsunying Huang] created file
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

#ifndef _APPLIB_SW_SCALAR_API_H_
#define _APPLIB_SW_SCALAR_API_H_
/**
 * @defgroup Utility
 * @brief Utility
 *
 * Implementation of
 * Format related function
 *
 */
/**
* @defgroup ApplibUtility_SWScalar
* @brief SW scalar
*
*
*/

/**
 * @addtogroup ApplibUtility_SWScalar
 * @ingroup Utility
 * @{
 */
#include <AmbaDataType.h>
#include <AmbaKAL.h>

__BEGIN_C_PROTO__

/**
 * applib software scalar
 */
typedef struct _APPLIB_SW_SCALAR_s_ {
    UINT32 SrcBufferAddress; /**<Source Buffer Address*/
    int SrcBufferPitch; /**<Source Buffer Pitch*/
    int SrcBufferWidth; /**<Source Buffer Width*/
    int SrcBufferHeight; /**<Source Buffer Height*/
    int SrcPositionX; /**<Source Position X*/
    int SrcPositionY; /**<Source Position Y*/
    int SrcWidth; /**<Source Width*/
    int SrcHeight; /**<Source Height*/

    UINT32 DstBufferAddress; /**<Destination Buffer Address*/
    int DstBufferPitch; /**<Destination Buffer Pitch*/
    int DstBufferWidth; /**<Destination Buffer Width*/
    int DstBufferHeight; /**<Destination Buffer Height*/
    int DstPositionX; /**<Destination Positio nX*/
    int DstPositionY; /**<Dst Position Y*/
    int DstWidth; /**<Destination Width*/
    int DstHeight; /**<Destination Height*/
#define MAX_SCALAR_WIDTH    1920 /**<MAX_SCALAR_WIDTH 1920*/
#define MAX_SCALAR_HEIGHT    1080 /**<MAX_SCALAR_HEIGHT 1080*/

    int Type; /**< Type*/
#define SCALAR_TYPE_1_BYTE    1000      /**<SCALAR_TYPE_1_BYTE        1000*/
#define SCALAR_TYPE_1_BYTE_90    1090   /**<SCALAR_TYPE_1_BYTE_90     1090*/
#define SCALAR_TYPE_1_BYTE_180    1180  /**<SCALAR_TYPE_1_BYTE_180    1180*/
#define SCALAR_TYPE_1_BYTE_270    1270  /**<SCALAR_TYPE_1_BYTE_270    1270*/
#define SCALAR_TYPE_2_BYTE    2000      /**<SCALAR_TYPE_2_BYTE        2000*/
#define SCALAR_TYPE_2_BYTE_90    2090   /**<SCALAR_TYPE_2_BYTE_90     2090*/
#define SCALAR_TYPE_2_BYTE_180    2180  /**<SCALAR_TYPE_2_BYTE_180    2180*/
#define SCALAR_TYPE_2_BYTE_270    2270  /**<SCALAR_TYPE_2_BYTE_270    2270*/
#define SCALAR_TYPE_4_BYTE    4000      /**<SCALAR_TYPE_4_BYTE        4000*/
#define SCALAR_TYPE_4_BYTE_90    4090   /**<SCALAR_TYPE_4_BYTE_90     4090*/
#define SCALAR_TYPE_4_BYTE_180    4180  /**<SCALAR_TYPE_4_BYTE_180    4180*/
#define SCALAR_TYPE_4_BYTE_270    4270  /**<SCALAR_TYPE_4_BYTE_270    4270*/
    int TransColorEn; /**<Trans Color En*/
    UINT32 TransColor; /**<Trans Color*/
    int RegardDstValue; /**<Regard Destination Value*/
#define SCALAR_WRITE_DST_VALUE     1 /**<SCALAR_WRITE_DST_VALUE     1*/
#define SCALAR_READ_DST_VALUE      2 /**<SCALAR_READ_DST_VALUE      2*/
} APPLIB_SW_SCALAR_s;

/**
 * applib sofware scalar map
 */
typedef struct APPLIB_SW_SCALAR_MAP_s{
    UINT32 MappingX; /**<Mapping X*/
    UINT32 ToX; /**<To X*/
    UINT32 MappingY; /**<Mapping Y*/
    UINT32 ToY; /**<To Y*/
    UINT8 MappingWidth[MAX_SCALAR_WIDTH]; /**<Mapping Width*/
    UINT8 MappingHeight[MAX_SCALAR_HEIGHT]; /**<Mapping Height*/
    int TransColorEn; /**<Trans Color En*/
    int TransColor; /**<Trans Color*/
} APPLIB_SW_SCALAR_MAP_s, *p_APPLIB_SW_SCALAR_MAP_s; /**<p_APPLIB_SW_SCALAR_MAP_s*/

/**
 * Software scalar for normal case
 *
 * @param [in] info
 * @param [in] map_info
 * @return >=0 success, <0 failure
 */
extern int AppLibUtilityScalar_8Bit_Normal(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s map_info);

/**
 * Software scalar for normal case with rotation
 *
 * @param [in] info
 * @param [in] map_info
 * @param [in] angle
 * @return >=0 success, <0 failure
 */
extern int AppLibUtilityScalar_8Bit_Rotate(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s map_info, int angle);

/**
 *  Software scalar for normal case
 *
 * @param [in] info
 * @param [in] map_info
 * @return >=0 success, <0 failure
 */
extern int AppLibUtilityScalar_16Bit_Normal(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s map_info);

/**
 * Software scalar for normal case with rotation
 *
 * @param [in] info
 * @param [in] map_info
 * @param [in] angle
 * @return >=0 success, <0 failure
 */
extern int AppLibUtilityScalar_16Bit_Rotate(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s map_info, int angle);

/**
 *  Software scalar for normal case
 *
 * @param [in] info
 * @param [in] map_info
 * @return >=0 success, <0 failure
 */
extern int AppLibUtilityScalar_32Bit_Normal(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s map_info);

/**
 * Software scalar for normal case with rotation
 *
 * @param [in] info
 * @param [in] map_info
 * @param [in] angle
 * @return >=0 success, <0 failure
 */
extern int AppLibUtilityScalar_32Bit_Rotate(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s map_info, int angle);

/**
 * Software scalar index selector
 *
 * @param [in] info
 * @param [in] map_info
 * @param [in] exchange
 * @return >=0 success, <0 failure
 */
extern int AppLibUtilitySWScalar_Flow_IndexSelector(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s map_info, UINT8 exchange);

/**
 * Software scalar set mapping info
 *
 * @param [in] info
 * @param [in] map_info
 * @return >=0 success, <0 failure
 */
extern int AppLibUtilitySWScalar_Flow_SetMappingInfo(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s map_info);

/**
 * Software scalar entry point
 *
 * @param [in] param
 * @return >=0 success, <0 failure
 */
extern int AppLibUtilitySWScalar_ExeScalar(APPLIB_SW_SCALAR_s *param);
/**
 * @}
 */
#endif /*APPLIB_HCMGR_H_*/