/**
 * @file src/app/connected/applib/src/utility/swscalar/ApplibUtility_SW_Scalar_8Bit.c
 *
 * SW scalar for 16 bits
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

/**
 * Software scalar line processing function (add one cases)
 * add 1/mapx ,thus duplicate one x after mapx points R/W = UINT16/UINT32
 */
static void AppLibUtilityScalar_16Bit_Span_AddOne(UINT16 *sx16, UINT16 *end, UINT16 *dx16, APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT32 tval, val, mapx = mapInfo->MappingX;
    UINT32 *dx32;

    switch (mapx) {
        case 1:
        /*  code density --->
         *
         *  4B  8B        Wr DstWidth align
         * ----------------------------
         *  2B  4B        Rd SrcWidth align
         *  0~3 src_nonalign pixel
         */
             tval = (end-dx16) & 0x03;
            while (tval) {
                dx16[1] = dx16[0] = *sx16++;
                dx16 += 2;
                tval--;
            }
            dx32 = (UINT32 *)dx16;
            while (sx16 < end) {
                val = *sx16++;
                *dx32++  = (val<<16)|val;
                val = *sx16++;
                *dx32++  = (val<<16)|val;
             }
            break;
        case 2:
        /*  code density --->
         *
         *  6B  12B        Wr DstWidth align
         * ----------------------------
         *  4B  8B        Rd SrcWidth align
         *  0,2,4,6 src_nonalign pixel ,UINT16 pointer sub!!
         */
            tval = (end-sx16) & 0x07;
            while (tval) {
                dx16[0] = sx16[0];
                dx16[2] = dx16[1] = sx16[1];
                sx16 += 2;
                 dx16 += 3;
                tval -= 2;
            }
            dx32 = (UINT32*)dx16;
            while (sx16 < end) {
                tval = *sx16++;
                val = *sx16++;
                *dx32++  = (val<<16)|tval;
                tval = *sx16++;
                *dx32++  = (tval<<16)|val;
                val = *sx16++;
                *dx32++  = (val<<16)|val;
            }
            break;
        case 3:
        /*  code density --->
         *
         *  8B  16B        Wr DstWidth align
         * ----------------------------
         *  6B  12B        Rd SrcWidth align
         *  we know SrcWidth is 6B alignment
         */
            dx32 = (UINT32*)dx16;
            while (sx16 < end) {
                val = *sx16++;
                val |= (*sx16++)<<16;
                *dx32++  = val;

                val = *sx16++;
                *dx32++  = (val<<16)|val;
            }
            break;
        default:
            while (sx16 < end) {
                memcpy(dx16, sx16, mapx<<1);
                dx16 += mapx;
                *dx16 = *(dx16-1);
                dx16++;
                sx16 += mapx;
            }
            break;
    }
}

/**
 * Software scalar line processing function (sub one cases)
 * sub 1/mapx ,thus bypass one x after mapx points R/W = UINT16/UINT32
 */
static void AppLibUtilityScalar_16Bit_Span_SubOne(UINT16 *sx16, UINT16 *end, UINT16 *dx16, APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT32 val, mapx = mapInfo->MappingX, mapxsubone = mapx-1;
    UINT32 *dx32;

    switch (mapxsubone) {
        case 1:
        /*  code density --->
         *
         *  1p  2p  4p        Wr DstWidth align
         * ----------------------------
         *  2p  4p  8p        Rd SrcWidth align
         *  0~7 src_nonalign pixel
         */
            val = (end-sx16) & 0x07;
            while (val--) {
                *dx16++ = *sx16++;
                sx16 ++;
            }
            dx32 = (UINT32*)dx16;
            while (sx16 < end) {
                val = (sx16[0]);
                val |= (sx16[2]<<16);
                *dx32++ = val;

                val = (sx16[4]);
                val |= (sx16[6]<<16);
                *dx32++ = val;
                sx16 += 8;
            }
            break;
        case 2:
        /*  code density --->
         *
         *  2p  4p  8p        Wr DstWidth align
         * ----------------------------
         *  3p  6p  12p        Rd SrcWidth align
         *  0,2,4,6 src_nonalign pixel ,UINT16 pointer sub!!
         */
         //SrcWidth = 3*k ,k = 2n   ==> SrcWidth = 6n
         //SrcWidth = 3*k ,k = 2n+1 ==> SrcWidth = 6n+3
            val = (end-sx16) & 0x11;
            if (val) {
                *dx16++ = *sx16++;
                *dx16++ = *sx16++;
                sx16 ++;
            }
            dx32 = (UINT32*)dx16;
            while (sx16 < end) {
                val = (sx16[0]);
                val |= (sx16[1]<<16);
                *dx32++ = val;

                val = (sx16[3]);
                val |= (sx16[4]<<16);
                *dx32++ = val;
                sx16 += 6;
            }
            break;
        case 3:
        /*  code density --->
         *
         *  3p  6p  12p        Wr DstWidth align
         * ----------------------------
         *  4p  8p  16p        Rd SrcWidth align
         *  0,4,8 src_nonalign number
         */
             val = (end-sx16) & 0x07;
            while (val) {
                *dx16++ = *sx16++;
                *dx16++ = *sx16++;
                *dx16++ = *sx16++;
                sx16 ++;
                val -= 4;
            }
            dx32 = (UINT32*)dx16;
            while (sx16 < end) {
                val = (sx16[0]);
                val |= (sx16[1]<<16);
                *dx32++ = val;

                val = (sx16[2]);
                val |= (sx16[4]<<16);
                *dx32++ = val;

                val = (sx16[5]);
                val |= (sx16[6]<<16);
                *dx32++ = val;

                sx16 += 8;
            }
            break;
        default:
            while (sx16 < end) {
                memcpy(dx16, sx16, mapxsubone<<1);
                dx16 += mapxsubone;
                sx16 += mapx;
            }
            break;
    }
}

/**
 * Software scalar line processing function (equal cases)
 */
static inline void AppLibUtilityScalar_16Bit_Span_Equal(UINT16 *start, UINT16 *end, UINT16 *target, APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    memcpy(target, start, (end-start)<<1);
}

/*
 * Software scalar line processing function (other cases)
 */
static void AppLibUtilityScalar_16Bit_Span_Slow(UINT16 *start, UINT16 *end, UINT16 *target, APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT16 *sx;
    UINT32 repeatx, offset, w = end-start;
    int i;
    /* duplicate repeatx times by table (map_w[offset]) */
    for (offset=0; offset<w; offset++) {
        sx = start + offset;
        // future work: use ones_run_len coding to do memcpy and enhance perf
        repeatx = mapInfo->MappingWidth[(info->SrcPositionX+offset)%mapInfo->MappingX];
        for (i=0; i<repeatx; i++, target++) { //repeatx
            *target = *sx;
        }
    }
}

/**
 * Software scalar line processing function for 2 byte (180 degree)
 */
static inline int AppLibUtilityScalar_16Bit_Span_Reverse(UINT16 *start, UINT16 *end, UINT16 *target, UINT8 *mapW)
{
    UINT16 *sx, *dx = target;
    UINT32 repeatX;
    int i;
    /* duplicate repeatx times by table (map_w[offset])   */
    for (sx = start; sx >= end; sx--, mapW--) {
        // future work: use ones_run_len coding to do memcpy and enhance perf
        // don't use "%", it's a dividing opcode! see assembly!!
        repeatX = *mapW;
        if (repeatX == 0) continue;
        for (i=0; i<repeatX; i++, target++) { //repeatx
            *target = *sx;
        }
    }
    return (target - dx);
}

/**
 * Software scalar 0 degrees (16bits)
 */
static int AppLibUtilityScalar_16Bit_Flow_Mapping(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT16 *get, *put, repeatY;
    int sy, j, sft = 1;
    int signx = mapInfo->ToX - mapInfo->MappingX;
    int signy = mapInfo->ToY - mapInfo->MappingY;
    void (*gobj_sx_to_dx_best)(UINT16*, UINT16*, UINT16*, APPLIB_SW_SCALAR_s *, p_APPLIB_SW_SCALAR_MAP_s);
    int srcP = info->SrcBufferPitch;
    int dstP = info->DstBufferPitch;
    // decide sx & put pointer
    get = (UINT16 *)info->SrcBufferAddress + (info->SrcPositionY * srcP) + info->SrcPositionX;
    put = (UINT16 *)info->DstBufferAddress + (info->DstPositionY * dstP) + info->DstPositionX;

    // decide line processor according to relation between mapx & to_x
    switch (signx) {
    case 1:
        gobj_sx_to_dx_best = AppLibUtilityScalar_16Bit_Span_AddOne;
        break;
    case -1:
        gobj_sx_to_dx_best = AppLibUtilityScalar_16Bit_Span_SubOne;
        break;
    case 0:
        gobj_sx_to_dx_best = AppLibUtilityScalar_16Bit_Span_Equal;
        break;
    default:
        gobj_sx_to_dx_best = AppLibUtilityScalar_16Bit_Span_Slow;
        break;
    }

    // decide flow according to relation between mapy & to_y
    switch (signy) {
    case 1:
        for (sy = info->SrcPositionY; sy < info->SrcPositionY + info->SrcHeight; ) {
            for (j=0; j<mapInfo->MappingY; j++) {
                gobj_sx_to_dx_best(get, get+info->SrcWidth, put, info, mapInfo);
                get += srcP;
                put += dstP;
                sy++;
            }
            memcpy(put, put-dstP, info->DstWidth<<sft);
            put += dstP;
        }
        break;
    case -1:
        for (sy = info->SrcPositionY; sy < info->SrcPositionY + info->SrcHeight; ) {
            for (j=0; j<mapInfo->MappingY-1; j++) {
                gobj_sx_to_dx_best(get, get+info->SrcWidth, put, info, mapInfo);
                get += srcP;
                put += dstP;
                sy++;
            }
            get += srcP;
            sy++;
        }
        break;
    case 0:
        sy = info->SrcHeight;
        while ((sy--)>0) {
            gobj_sx_to_dx_best(get, get+info->SrcWidth, put, info, mapInfo);
            get += srcP;
            put += dstP;
        }
        break;
    default:
        for (sy = info->SrcPositionY; sy < info->SrcPositionY + info->SrcHeight; sy++) {
            repeatY = mapInfo->MappingHeight[sy%mapInfo->MappingY];

            if (repeatY == 0) {
                get += srcP;
                continue;
            } else {
                gobj_sx_to_dx_best(get, get+info->SrcWidth, put, info, mapInfo);
                get += srcP;
                put += dstP;
                for (j=1; j<repeatY; j++) {
                    memcpy(put, put-dstP, info->DstWidth<<sft);
                    put += dstP;
                }
            }
        }
    }

    return 0;
}

/**
 * Software scalar 90 degrees (16bits)
 */
static int AppLibUtilityScalar_16Bit_Flow_Rotate90(int sxSta, int sxEnd, int sySta, int syEnd,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sx, sy, repeatX, repeatY, sft = 1;
    UINT16 *get, *put, *putSta;

    put = (UINT16 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sx = sxSta; sx <= sxEnd; sx++) {
        repeatY = selH[sx];
        if (repeatY == 0) continue;
        // handle each line
        get = (UINT16 *)info->SrcBufferAddress + (syEnd * info->SrcBufferPitch) + sx;
        putSta = put;
        for (sy = syEnd; sy >= sySta; sy--) {
                  repeatX = selW[sy];
            for (i=0; i<repeatX; i++, put++) { //repeatx
                *put = *get;
            }
            get -= info->SrcBufferPitch;
        }
        info->DstWidth = put - putSta;
        put += (info->DstBufferPitch - info->DstWidth);
        dy++;
        // repeat line if needed
        for (i=1; i<repeatY; i++) {         //repeaty
            memcpy(put, put-info->DstBufferPitch, info->DstWidth<<sft);
            put += info->DstBufferPitch;
            dy++;
        }
    }
    info->DstHeight = dy - dySta;

    return 0;
}

/**
 * Software scalar 270 degrees (16bits)
 */
static int AppLibUtilityScalar_16Bit_Flow_Rotate270(int sxSta, int sxEnd, int sySta, int syEnd,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sx, sy, repeatX, repeatY, sft = 1;
    UINT16 *get, *put, *putSta;

    put = (UINT16 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sx = sxEnd; sx >= sxSta; sx--) {
        repeatY = selH[sx];
        if (repeatY == 0) continue;
        // handle each line
        get = (UINT16 *)info->SrcBufferAddress + (sySta * info->SrcBufferPitch) + sx;
        putSta = put;
        for (sy = sySta; sy <= syEnd; sy++) {
                  repeatX = selW[sy];
            for (i=0; i<repeatX; i++, put++) { //repeatx
                *put = *get;
            }
            get += info->SrcBufferPitch;
        }
        info->DstWidth = put - putSta;
        put += (info->DstBufferPitch - info->DstWidth);
        dy++;
        // repeat line if needed
        for (i=1; i<repeatY; i++)  {         //repeaty
            memcpy(put, put-info->DstBufferPitch, info->DstWidth<<sft);
            put += info->DstBufferPitch;
            dy++;
        }
    }
    info->DstHeight = dy - dySta;

    return 0;
}

/**
 * Software scalar 180 degrees (16bits)
 */
static int AppLibUtilityScalar_16Bit_Flow_Rotate180(int sxSta, int sxEnd, int sySta, int syEnd,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sy, repeatY, sft = 1;
    UINT16 *get, *put;

    put = (UINT16 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sy = syEnd; sy >= sySta; sy--) {
        repeatY = selH[sy];
        if (repeatY == 0) continue;
        // handle each line
        get = (UINT16 *)info->SrcBufferAddress + (sy * info->SrcBufferPitch) + sxEnd;
        info->DstWidth = AppLibUtilityScalar_16Bit_Span_Reverse(get, get-info->SrcWidth, put, &selW[sxEnd]);
        get -= info->SrcBufferPitch;
        put += info->DstBufferPitch;
        dy++;
        // repeat line if needed
        for (i=1; i<repeatY; i++) {     //repeaty
            memcpy(put, put-info->DstBufferPitch, info->DstWidth<<sft);
            put += info->DstBufferPitch;
            dy++;
        }
    }
    info->DstHeight = dy - dySta;

    return 0;
}

/*  _RESIZE_NEED_
 *
 * Software scalar line processing function (other cases with TransColor checking)
 */
static void AppLibUtilityScalar_16Bit_Span_SlowTransColor(UINT16 *start, UINT16 *end, UINT16 *target, APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT16 *sx;
    UINT32 repeatX, offset, w = end-start;
    int i;
    /* duplicate repeatx times by table (map_w[offset]) */
    for (offset=0; offset<w; offset++) {
        sx = start + offset;
        // future work: use ones_run_len coding to do memcpy and enhance perf
        repeatX = mapInfo->MappingWidth[(info->SrcPositionX+offset)%mapInfo->MappingX];
        if (mapInfo->TransColor != *sx) {
            for (i=0; i<repeatX; i++, target++) { //repeatx
                *target = *sx;
            }
        } else {
            target += repeatX;
        }
    }
}

/**
 * Software scalar line processing function for 2 byte (180 degree)
 */
static inline int AppLibUtilityScalar_8Bit_Span_ReverseTransColor(UINT16 *start, UINT16 *end, UINT16 *target, UINT8 *mapW, UINT32 TransColor)
{
    UINT16 *sx, *dx = target;
    UINT32 repeatX;
    int i;
    /* duplicate repeatx times by table (map_w[offset])   */
    for (sx = start; sx >= end; sx--, mapW--) {
        // future work: use ones_run_len coding to do memcpy and enhance perf
        // don't use "%", it's a dividing opcode! see assembly!!
        repeatX = *mapW;
        if (repeatX == 0) continue;
        if (TransColor != *sx) {
            for (i=0; i<repeatX; i++, target++) { //repeatx
                *target = *sx;
            }
        } else {
            target += repeatX;
        }
    }
    return (target - dx);
}

/**
 * Software scalar do mapping (16bits) with TransColor checking
 */
static int AppLibUtilityScalar_16Bit_Flow_ExeMappingTransColor(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT16 *get, *put, repeatY;
    int sy, j;
    int srcP = info->SrcBufferPitch;
    int dstP = info->DstBufferPitch;
    // decide sx & put pointer
    get = (UINT16 *)info->SrcBufferAddress + (info->SrcPositionY * srcP) + info->SrcPositionX;
    put = (UINT16 *)info->DstBufferAddress + (info->DstPositionY * dstP) + info->DstPositionX;

    for (sy = info->SrcPositionY; sy < info->SrcPositionY + info->SrcHeight; ) {
        repeatY = mapInfo->MappingHeight[sy%mapInfo->MappingY];
        sy++;
        if (repeatY == 0) {
            get += srcP;
            continue;
        } else {
            for (j=0; j<repeatY; j++) {
                AppLibUtilityScalar_16Bit_Span_SlowTransColor(get, get+info->SrcWidth, put, info, mapInfo);
                put += dstP;
            }
            get += srcP;
        }
    }

    return 0;
}

/**
 * Software scalar 90 degrees (16bits) with TransColor checking
 */
static int AppLibUtilityScalar_16Bit_Flow_Rotate90_TransColor(int sxSta, int sxEnd, int sySta, int syEnd,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sx, sy, repeatX, repeatY;
    UINT16 *get, *put, *putSta;

    put = (UINT16 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sx = sxSta; sx <= sxEnd; sx++) {
        repeatY = selH[sx];
        if (repeatY == 0) {
            continue;
        } else {
            for (i=0; i<repeatY; i++) {         //repeaty
                // handle each line
                get = (UINT16 *)info->SrcBufferAddress + (syEnd * info->SrcBufferPitch) + sx;
                putSta = put;
                for (sy = syEnd; sy >= sySta; sy--) {
                          repeatX = selW[sy];
                    if (info->TransColor != *get) {
                        for (i=0; i<repeatX; i++, put++) { //repeatx
                            *put = *get;
                        }
                    } else {
                        put += repeatX;
                    }
                    get -= info->SrcBufferPitch;
                }
                info->DstWidth = put - putSta;
                put += (info->DstBufferPitch - info->DstWidth);
                dy++;
            }
        }
    }
    info->DstHeight = dy - dySta;

    return 0;
}

/**
 * Software scalar 270 degrees (16bits) with TransColor checking
 */
static int AppLibUtilityScalar_16Bit_Flow_Rotate270_TransColor(int sxSta, int sxEnd, int sySta, int syEnd,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sx, sy, repeatX, repeatY;
    UINT16 *get, *put, *putSta;

    put = (UINT16 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sx = sxEnd; sx >= sxSta; sx--) {
        repeatY = selH[sx];
        if (repeatY == 0) {
            continue;
        } else {
            for (i=0; i<repeatY; i++)  {         //repeaty
                // handle each line
                get = (UINT16 *)info->SrcBufferAddress + (sySta * info->SrcBufferPitch) + sx;
                putSta = put;
                for (sy = sySta; sy <= syEnd; sy++) {
                          repeatX = selW[sy];
                    if (info->TransColor != *get) {
                        for (i=0; i<repeatX; i++, put++) { //repeatx
                            *put = *get;
                        }
                    } else {
                        put += repeatX;
                    }
                    get += info->SrcBufferPitch;
                }
                info->DstWidth = put - putSta;
                put += (info->DstBufferPitch - info->DstWidth);
                dy++;
            }
        }
    }
    info->DstHeight = dy - dySta;

    return 0;
}

/**
 * Software scalar 180 degrees (16bits) with TransColor checking
 */
static int AppLibUtilityScalar_16Bit_Flow_Rotate180_TransColor(int sxSta, int sxEnd, int sySta, int syEnd,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sy, repeatY;
    UINT16 *get, *put;

    put = (UINT16 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sy = syEnd; sy >= sySta; sy--) {
        repeatY = selH[sy];
        if (repeatY == 0) {
            continue;
        } else {
            for (i=0; i<repeatY; i++) {     //repeaty
                // handle each line
                get = (UINT16 *)info->SrcBufferAddress + (sy * info->SrcBufferPitch) + sxEnd;
                info->DstWidth = AppLibUtilityScalar_8Bit_Span_ReverseTransColor(get, get-info->SrcWidth, put, &selW[sxEnd], info->TransColor);
                get -= info->SrcBufferPitch;
                put += info->DstBufferPitch;
                dy++;
            }
        }
    }
    info->DstHeight = dy - dySta;

    return 0;
}

/**
 * Software scalar for normal case
 */
int AppLibUtilityScalar_16Bit_Normal(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    AppLibUtilitySWScalar_Flow_IndexSelector(info, mapInfo, 0);
    AppLibUtilitySWScalar_Flow_SetMappingInfo(info, mapInfo);
    if (info->TransColorEn == 1)
        AppLibUtilityScalar_16Bit_Flow_ExeMappingTransColor(info, mapInfo);
    else
        AppLibUtilityScalar_16Bit_Flow_Mapping(info, mapInfo);

    return 0;
}

/**
 * Software scalar for normal case with rotation
 */
int AppLibUtilityScalar_16Bit_Rotate(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo, int angle)
{
    UINT8 selW[MAX_SCALAR_WIDTH], selH[MAX_SCALAR_HEIGHT];
    int dx, dy, sxSta, sxEnd, sySta, syEnd, dxSta, dySta;
    int bx, by, bxSta, bySta;
    UINT32 mapx, mapy;

    memset(selW ,0 ,MAX_SCALAR_WIDTH);
    memset(selH ,0 ,MAX_SCALAR_HEIGHT);

    /**
     * Suppose we want to do rotation (a->b) then do scaling (b->c) as follows
     *                   _____
     *          180     |2   4|
     *        _________ |     |
     *       |4       3|| (b) | 270       ____________
     *       |   (b)   ||     |          |left   right|
     *       |2_______1||1___3|          |top         |
     *            _____  _________   ==> |     (c)    |
     *           |3   1||1       2|      |bottom      |
     *           |     ||   (a)   |      |____________|
     *        90 | (b) ||3_______4|           [dst]
     *           |     |   [src]
     *           |4___2|
     *
     */
    // calculate selected index
    switch (angle) {
    case 90:
    case 270:
        // select index with SrcBufferWidth, SrcBufferHeight exchanged
        AppLibUtilitySWScalar_Flow_IndexSelector(info ,mapInfo, 1);
        break;
    case 180:
        // select index without SrcBufferWidth, SrcBufferHeight exchanged
        AppLibUtilitySWScalar_Flow_IndexSelector(info ,mapInfo, 0);
        break;
    }
    mapx = mapInfo->MappingX;
    mapy = mapInfo->MappingY;

    // assign sel_w & sel_h
    switch (angle) {
    case 90:
    case 270:
        for (bx=0; bx<(info->SrcBufferHeight); bx++)
            selW[bx] = mapInfo->MappingWidth[bx%mapx];
        for (by=0; by<(info->SrcBufferWidth); by++)
            selH[by] = mapInfo->MappingHeight[by%mapy];
        break;
    case 180:
        for (bx=0; bx<(info->SrcBufferWidth); bx++)
            selW[bx] = mapInfo->MappingWidth[bx%mapx];
        for (by=0; by<(info->SrcBufferHeight); by++)
            selH[by] = mapInfo->MappingHeight[by%mapy];
        break;
    default:
        // No rotation
        return 0;
    }

    // decide des_x & des_y
    if (info->RegardDstValue != SCALAR_READ_DST_VALUE) {
        switch (angle) {
        case 90:
            // decide des_x by accumulating bx_sta selected indices
            bxSta = info->SrcPositionY + info->SrcHeight - 1;
            for (bx=info->SrcBufferHeight; bx>bxSta; bx--)
                info->DstPositionX += selW[bx];
            // decide DstPositionY by accumulating by_sta selected indices
            bySta = info->SrcPositionX;
            for (by=0; by<bySta; by++)
                info->DstPositionY += selH[by];
            break;
        case 270:
            // decide des_x by accumulating bx_sta selected indices
            bxSta = info->SrcPositionY;
            for (bx=0; bx<bxSta; bx++)
                info->DstPositionX += selW[bx];
            // decide DstPositionY by accumulating by_sta selected indices
            bySta = info->SrcPositionX + info->SrcWidth - 1;
            for (by=info->SrcBufferWidth; by>bySta; by--)
                info->DstPositionY += selH[by];
            break;
        case 180:
            // decide des_x by accumulating bx_sta selected indices
            bxSta = info->SrcPositionX + info->SrcWidth - 1;
            for (bx=info->SrcBufferWidth; bx>bxSta; bx--)
                info->DstPositionX += selW[bx];
            // decide DstPositionY by accumulating by_sta selected indices
            bySta = info->SrcPositionY + info->SrcHeight - 1;
            for (by=info->SrcBufferHeight; by>bySta; by--)
                info->DstPositionY += selH[by];
            break;
        default:
            // No rotation
            return 0;
        }
    }

    /**
     * Suppose we want to do rotation (a->b) then do scaling (b->c) as follows
     *                   _____
     *          180     |2   4|
     *        _________ |     |
     *       |4       3|| (b) | 270       ____________
     *       |   (b)   ||     |          |left   right|
     *       |2_______1||1___3|          |top         |
     *            _____  _________   ==> |     (c)    |
     *           |3   1||1       2|      |bottom      |
     *           |     ||   (a)   |      |____________|
     *        90 | (b) ||3_______4|           [dst]
     *           |     |   [src]
     *           |4___2|
     *
     * [Scan direction of dst buffer (c)]               [inner for loop][outer for loop]
     * Goal : progressively scan on (c) (l->r; t->b) => (dx_sta->dx_end; dy_sta->dy_end)
     *
     * [Scan direction of src buffer (a)]               [inner for loop][outer for loop]
     *   90 : (b) (3->1; 3->4)  =>  (a) (b->t; l->r) => (sy_end->sy_sta; sx_sta->sx_end)
     *  180 : (b) (4->3; 4->2)  =>  (a) (r->l; b->t) => (sx_end->sx_sta; sy_end->sy_sta)
     *  270 : (b) (2->4; 2->1)  =>  (a) (t->b; r->l) => (sy_sta->sy_end; sx_end->sx_sta)
     */
    sxSta = info->SrcPositionX;
    sxEnd = MIN(info->SrcPositionX + info->SrcWidth - 1, info->SrcBufferWidth - 1);
    sySta = info->SrcPositionY;
    syEnd = MIN(info->SrcPositionY + info->SrcHeight - 1, info->SrcBufferHeight - 1);
    dxSta = info->DstPositionX;
    dySta = info->DstPositionY;
    dx = info->DstPositionX;
    dy = info->DstPositionY;
    // start rotation mapping
    switch (angle) {
    case 90:
        if (info->TransColorEn == 1) {
            AppLibUtilityScalar_16Bit_Flow_Rotate90_TransColor(sxSta, sxEnd, sySta, syEnd, dxSta, dySta, dx, dy, selW, selH, info);
        } else {
            AppLibUtilityScalar_16Bit_Flow_Rotate90(sxSta, sxEnd, sySta, syEnd, dxSta, dySta, dx, dy, selW, selH, info);
        }
        break;
    case 270:
        if (info->TransColorEn == 1) {
            AppLibUtilityScalar_16Bit_Flow_Rotate270_TransColor(sxSta, sxEnd, sySta, syEnd, dxSta, dySta, dx, dy, selW, selH, info);
        } else {
            AppLibUtilityScalar_16Bit_Flow_Rotate270(sxSta, sxEnd, sySta, syEnd, dxSta, dySta, dx, dy, selW, selH, info);
        }
        break;
    case 180:
        if (info->TransColorEn == 1) {
            AppLibUtilityScalar_16Bit_Flow_Rotate180_TransColor(sxSta, sxEnd, sySta, syEnd, dxSta, dySta, dx, dy, selW, selH, info);
        } else {
            AppLibUtilityScalar_16Bit_Flow_Rotate180(sxSta, sxEnd, sySta, syEnd, dxSta, dySta, dx, dy, selW, selH, info);
        }
        break;
    default:
        // No rotation
        return 0;
    }

    return 0;
}
