/**
 * @file src/app/connected/applib/src/utility/swscalar/ApplibUtility_SW_Scalar.c
 *
 * SW scalar interface
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

#include <utility/ApplibUtility_SW_Scalar.h>
#include <common/common.h> // MIN
#include <AmbaPrintk.h>       // ASSERT

#if 0
#pragma O0
#define SWSCAL_TIME      swscal_time
#define SWSCAL_ERR(str, ...)     do { printk_co(RED,"SWSCAL_ERR " str, ##__VA_ARGS__); } while (0)
#define SWSCAL_MSG(str, ...)     do { printk_co(GREEN,"SWSCAL_MSG " str, ##__VA_ARGS__); } while (0)

static void swscal_time (int en)
{
    static SYSTIM t1, t2;
    if (en) get_tim(&t1);
    if (!en) {
        get_tim(&t2);
        SWSCAL_MSG("draw: takes %d ms\n", t2.ltime - t1.ltime);
    }
}
#else
#define SWSCAL_ERR(...)
#define SWSCAL_TIME(...)
#define SWSCAL_MSG(...)
#endif

/**
 * Software scalar gcd function
 */
static inline int AppLibUtilitySWScalar_GetGCD(int a,int d)
{
    int r = a % d;

    if (r==0)
        return d;
    else
        return AppLibUtilitySWScalar_GetGCD(d,r);
}

/**
 * Software scalar index selector
 */
int AppLibUtilitySWScalar_Flow_IndexSelector(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo, UINT8 exchange)
{
    UINT32 mapx, mapy, to_x, to_y;
    int temp, i, gcdw, gcdh;

    /* exchange SrcBufferWidth & SrcBufferHeight */
    if (exchange) {
        i = info->SrcBufferWidth;
        info->SrcBufferWidth = info->SrcBufferHeight;
        info->SrcBufferHeight = i;
    }
    /* calculate gcd w & h */
    gcdw = (info->SrcBufferWidth > info->DstBufferWidth)?
        AppLibUtilitySWScalar_GetGCD(info->SrcBufferWidth, info->DstBufferWidth):
        AppLibUtilitySWScalar_GetGCD(info->DstBufferWidth, info->SrcBufferWidth);
    gcdh = (info->SrcBufferHeight > info->DstBufferHeight)?
        AppLibUtilitySWScalar_GetGCD(info->SrcBufferHeight, info->DstBufferHeight):
        AppLibUtilitySWScalar_GetGCD(info->DstBufferHeight, info->SrcBufferHeight);

    /* scalar w index mapping */
    mapx = mapInfo->MappingX = info->SrcBufferWidth/gcdw;
    to_x = mapInfo->ToX = info->DstBufferWidth/gcdw;
    /* scalar h index mapping */
    mapy = mapInfo->MappingY = info->SrcBufferHeight/gcdh;
    to_y = mapInfo->ToY = info->DstBufferHeight/gcdh;

    memset(mapInfo->MappingWidth, 0, sizeof(UINT8)*mapx);
    memset(mapInfo->MappingHeight, 0, sizeof(UINT8)*mapy);

    /* scalar w index mapping */
    K_ASSERT(to_x <= MAX_SCALAR_WIDTH);
    for (temp=0; temp<to_x; temp++) {
        i = (temp*mapx)/to_x;
        mapInfo->MappingWidth[i]++;
    }

    /* scalar h index mapping */
    K_ASSERT(to_y <= MAX_SCALAR_HEIGHT);
    for (temp=0; temp<to_y; temp++) {
        i = (temp*mapy)/to_y;
        mapInfo->MappingHeight[i]++;
    }

    /* exchange back SrcBufferWidth & SrcBufferHeight */
    if (exchange) {
        i = info->SrcBufferWidth;
        info->SrcBufferWidth = info->SrcBufferHeight;
        info->SrcBufferHeight = i;
    }

    /* assign transparent color */
    mapInfo->TransColorEn = info->TransColorEn;
    mapInfo->TransColor = info->TransColor;

    return 0;
}

/**
 * Software scalar set mapping info
 */
int AppLibUtilitySWScalar_Flow_SetMappingInfo(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{

    UINT32 mapx = mapInfo->MappingX, to_x = mapInfo->ToX;
    UINT32 mapy = mapInfo->MappingY, to_y = mapInfo->ToY;
    /* lefmove to an anchor point relative to i*mapx
       upmove  to an anchor point relative to j*mapy
       slightly enlarge invalid_all w and h to cover this region */
    UINT32 lefmove, upmove;

    if (info->RegardDstValue == SCALAR_WRITE_DST_VALUE) {
        /* decide DstPositionX */
        lefmove = info->SrcPositionX%mapx;
        info->SrcPositionX = info->SrcPositionX - lefmove;
//        info->DstPositionX = (info->SrcPositionX * to_x)/mapx ;

        /* decide DstWidth */
        info->SrcWidth = info->SrcWidth + lefmove;
        info->SrcWidth = info->SrcWidth + mapx-(info->SrcWidth%mapx);
        info->SrcWidth = MIN(info->SrcBufferWidth - info->SrcPositionX, info->SrcWidth);
        info->DstWidth = (info->SrcWidth/mapx) * to_x;

        /* decide DstPositionY */
        upmove = info->SrcPositionY%mapy;
        info->SrcPositionY = info->SrcPositionY - upmove;
//        info->DstPositionY = (info->SrcPositionY * to_y)/mapy;

        /* decide DstHeight */
        info->SrcHeight = info->SrcHeight + upmove;
        info->SrcHeight = info->SrcHeight + mapy-(info->SrcHeight%mapy);
        info->SrcHeight = MIN(info->SrcBufferHeight - info->SrcPositionY, info->SrcHeight);
        info->DstHeight = (info->SrcHeight/mapy) * to_y;
    }

    return 0;
}

/**
 * Software scalar entry point
 */
int AppLibUtilitySWScalar_ExeScalar(APPLIB_SW_SCALAR_s *param)
{
    int rval = 0;
    int sft = 0;

    static APPLIB_SW_SCALAR_MAP_s map_info;

    memset(&map_info ,0 ,sizeof(APPLIB_SW_SCALAR_MAP_s));

    /* Josh:
       a. map_info take lots of stack, declare once only!!
    b. use recursive(1/2,1/4,1/8...) approximate bilinear and chk perform,
       see rec_pre_de.c refine latter
    c. use line base scalar(generalized,fifo,R/W callback)
    */

    SWSCAL_TIME(TRUE);

    // initialize
    if (param->RegardDstValue != SCALAR_READ_DST_VALUE) {
        switch (param->Type) {
        case SCALAR_TYPE_1_BYTE:
        case SCALAR_TYPE_1_BYTE_90:
        case SCALAR_TYPE_1_BYTE_180:
        case SCALAR_TYPE_1_BYTE_270:
        case SCALAR_TYPE_2_BYTE:
        case SCALAR_TYPE_2_BYTE_90:
        case SCALAR_TYPE_2_BYTE_180:
        case SCALAR_TYPE_2_BYTE_270:
        case SCALAR_TYPE_4_BYTE:
//            param->DstPositionX = 0;
//            param->DstPositionY = 0;
            param->DstWidth = 0;
            param->DstHeight = 0;
            break;
        default:
            rval = -1;
        }
    }

    // transfer from byte domain to pixel domain
    switch (param->Type / 1000) {
    case 1:
        sft = 0;
        break;
    case 2:
        sft = 1;
        break;
    case 4:
        sft = 2;
        break;
    }

    param->SrcBufferPitch >>= sft;
    param->SrcBufferWidth >>= sft;
    param->SrcPositionX >>= sft;
    param->SrcWidth >>= sft;
    param->DstBufferPitch >>= sft;
    param->DstBufferWidth >>= sft;
    param->DstPositionX >>= sft;
    param->DstWidth >>= sft;

    // software scaling
    switch (param->Type) {
    case SCALAR_TYPE_1_BYTE:
        rval = AppLibUtilityScalar_8Bit_Normal(param, &map_info);
        break;
    case SCALAR_TYPE_1_BYTE_90:
        rval = AppLibUtilityScalar_8Bit_Rotate(param, &map_info, 90);
        break;
    case SCALAR_TYPE_1_BYTE_180:
        rval = AppLibUtilityScalar_8Bit_Rotate(param, &map_info, 180);
        break;
    case SCALAR_TYPE_1_BYTE_270:
        rval = AppLibUtilityScalar_8Bit_Rotate(param, &map_info, 270);
        break;
    case SCALAR_TYPE_2_BYTE:
        rval = AppLibUtilityScalar_16Bit_Normal(param, &map_info);
        break;
    case SCALAR_TYPE_2_BYTE_90:
        rval = AppLibUtilityScalar_16Bit_Rotate(param, &map_info, 90);
        break;
    case SCALAR_TYPE_2_BYTE_180:
        rval = AppLibUtilityScalar_16Bit_Rotate(param, &map_info, 180);
        break;
    case SCALAR_TYPE_2_BYTE_270:
        rval = AppLibUtilityScalar_16Bit_Rotate(param, &map_info, 270);
        break;
    case SCALAR_TYPE_4_BYTE:
        rval = AppLibUtilityScalar_32Bit_Normal(param, &map_info);
        break;
    case SCALAR_TYPE_4_BYTE_90:
        rval = AppLibUtilityScalar_32Bit_Rotate(param, &map_info, 90);
        break;
    case SCALAR_TYPE_4_BYTE_180:
        rval = AppLibUtilityScalar_32Bit_Rotate(param, &map_info, 180);
        break;
    case SCALAR_TYPE_4_BYTE_270:
        rval = AppLibUtilityScalar_32Bit_Rotate(param, &map_info, 270);
        break;
    default:
        AmbaPrint("No corresponding sw scaler type 0x%X", param->Type);
        rval = -1;
    }

    // transfer back from pixel domain to byte domain
    param->SrcBufferPitch <<= sft;
    param->SrcBufferWidth <<= sft;
    param->SrcPositionX <<= sft;
    param->SrcWidth <<= sft;
    param->DstBufferPitch <<= sft;
    param->DstBufferWidth <<= sft;
    param->DstPositionX <<= sft;
    param->DstWidth <<= sft;

    SWSCAL_MSG("mode:%d", param->Type);
    SWSCAL_MSG("src_addr    dst_addr    = 0x%X,0x%X",param->SrcBufferAddress, param->DstBufferAddress);
    SWSCAL_MSG("SrcBufferPitch    DstBufferPitch    = %d, %d",param->SrcBufferPitch, param->DstBufferPitch);
    SWSCAL_MSG("SrcBufferWidth    DstBufferWidth    = %d, %d",param->SrcBufferWidth, param->DstBufferWidth);
    SWSCAL_MSG("SrcPositionX    DstPositionX        = %d, %d",param->SrcPositionX, param->DstPositionX);
    SWSCAL_MSG("SrcWidth    DstWidth        = %d, %d",param->SrcWidth, param->DstWidth);
    SWSCAL_MSG("SrcBufferHeight    DstBufferHeight    = %d, %d",param->SrcBufferHeight, param->DstBufferHeight);
    SWSCAL_MSG("SrcPositionY    DstPositionY        = %d, %d",param->SrcPositionY, param->DstPositionY);
    SWSCAL_MSG("SrcHeight    DstHeight        = %d, %d",param->SrcHeight, param->DstHeight);

    SWSCAL_TIME(FALSE);

    return rval;
}

