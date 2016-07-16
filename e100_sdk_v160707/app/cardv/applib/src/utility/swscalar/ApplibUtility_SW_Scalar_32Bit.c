/**
 * @file src/app/connected/applib/src/utility/swscalar/ApplibUtility_SW_Scalar_8Bit.c
 *
 * SW scalar for 32 bits
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
 * add 1/mapx ,thus duplicate one x after mapx points R/W = UINT32/UINT32
 */
static void AppLibUtilityScalar_32Bit_Span_AddOne(UINT32 *sx32, UINT32 *end, UINT32 *dx32, APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT32 mapx = mapInfo->MappingX;

    switch (mapx) {
        case 1:
            while (sx32 < end) {
                dx32[1] = dx32[0] = sx32[0];
                dx32 += 2;
                sx32 += 1;

             }
            break;
        case 2:
            while (sx32 < end) {
                dx32[0] = sx32[0];
                dx32[2] = dx32[1] = sx32[1];
                dx32 += 3;
                sx32 += 2;

            }
            break;
        case 3:
            while (sx32 < end) {
                dx32[0] = sx32[0];
                dx32[1] = sx32[1];
                dx32[3] = dx32[2] = sx32[2];
                dx32 += 4;
                sx32 += 3;

            }
            break;
        default:
            while (sx32 < end) {
                memcpy(dx32, sx32, mapx<<2);//4B
                dx32[mapx] = dx32[mapx-1];
                dx32 += mapx+1;
                sx32 += mapx;
            }
            break;
    }
}

/**
 * Software scalar line processing function (sub one cases)
 * sub 1/mapx ,thus bypass one x after mapx points R/W = UINT32/UINT32
 */
static void AppLibUtilityScalar_32Bit_Span_SubOne(UINT32 *sx32, UINT32 *end, UINT32 *dx32, APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT32 mapx = mapInfo->MappingX, mapxsubone = mapx-1;

    switch (mapxsubone) {
        case 1:
            while (sx32 < end) {
                dx32[0] = sx32[0];
                dx32 += 1;
                sx32 += 2;
            }
            break;
        case 2:
            while (sx32 < end) {
                dx32[0] = sx32[0];
                dx32[1] = sx32[1];
                                dx32 += 2;
                sx32 += 3;
            }
            break;
        case 3:
            while (sx32 < end) {
                dx32[0] = sx32[0];
                dx32[1] = sx32[1];
                dx32[2] = sx32[2];
                                dx32 += 3;
                sx32 += 4;
            }
            break;
        default:
            while (sx32 < end) {
                memcpy(dx32, sx32, mapxsubone<<2);
                dx32 += mapxsubone;
                sx32 += mapx;
            }
            break;
    }
}

/**
 * Software scalar line processing function (equal cases)
 */
static inline void AppLibUtilityScalar_32Bit_Span_Equal(UINT32 *start, UINT32 *end, UINT32 *target, APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    memcpy(target, start, (end-start)<<2);
}

/**
 * Software scalar line processing function (other cases)
 */
static void AppLibUtilityScalar_32Bit_Span_Slow(UINT32 *start, UINT32 *end, UINT32 *target, APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT32 *sx;
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
 * Software scalar line processing function for 4 byte (180 degree)
 */
static inline int AppLibUtilityScalar_32Bit_Span_Reverse(UINT32 *start, UINT32 *end, UINT32 *target, UINT8 *mapW)
{
    UINT32 *sx, *dx = target;
    UINT32 repeatx;
    int i;
    /* duplicate repeatx times by table (map_w[offset])   */
    for (sx = start; sx >= end; sx--, mapW--) {
        // future work: use ones_run_len coding to do memcpy and enhance perf
        // don't use "%", it's a dividing opcode! see assembly!!
        repeatx = *mapW;
        if (repeatx == 0) continue;
        for (i=0; i<repeatx; i++, target++) { //repeatx
            *target = *sx;
        }
    }
    return (target - dx);
}

/**
 * Software scalar 0 degrees (32bits)
 */
static int AppLibUtilityScalar_32Bit_Flow_Mapping(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT32 *get, *put, repeaty;
    int sy, j, sft = 2;
    int signx = mapInfo->ToX - mapInfo->MappingX;
    int signy = mapInfo->ToY - mapInfo->MappingY;
    void (*gobj_sx_to_dx_best)(UINT32*, UINT32*, UINT32*,  APPLIB_SW_SCALAR_s*, p_APPLIB_SW_SCALAR_MAP_s);
    int src_p = info->SrcBufferPitch;
    int dst_p = info->DstBufferPitch;
    // decide sx & put pointer
    get = (UINT32 *)info->SrcBufferAddress + (info->SrcPositionY * src_p) + info->SrcPositionX;
    put = (UINT32 *)info->DstBufferAddress + (info->DstPositionY * dst_p) + info->DstPositionX;

    // decide line processor according to relation between mapx & to_x
    switch (signx) {
    case 1:
        gobj_sx_to_dx_best = AppLibUtilityScalar_32Bit_Span_AddOne;
        break;
    case -1:
        gobj_sx_to_dx_best = AppLibUtilityScalar_32Bit_Span_SubOne;
        break;
    case 0:
        gobj_sx_to_dx_best = AppLibUtilityScalar_32Bit_Span_Equal;
        break;
    default:
        gobj_sx_to_dx_best = AppLibUtilityScalar_32Bit_Span_Slow;
        break;
    }

    // decide flow according to relation between mapy & to_y
    switch (signy) {
    case 1:
        for (sy = info->SrcPositionY; sy < info->SrcPositionY + info->SrcHeight; ) {
            for (j=0; j<mapInfo->MappingY; j++) {
                gobj_sx_to_dx_best(get, get+info->SrcWidth, put, info, mapInfo);
                get += src_p;
                put += dst_p;
                sy++;
            }
            memcpy(put, put-dst_p, info->DstWidth<<sft);
            put += dst_p;
        }
        break;
    case -1:
        for (sy = info->SrcPositionY; sy < info->SrcPositionY + info->SrcHeight; ) {
            for (j=0; j<mapInfo->MappingY-1; j++) {
                gobj_sx_to_dx_best(get, get+info->SrcWidth, put, info, mapInfo);
                get += src_p;
                put += dst_p;
                sy++;
            }
            get += src_p;
            sy++;
        }
        break;
    case 0:
        sy = info->SrcHeight;
        while ((sy--)>0) {
            gobj_sx_to_dx_best(get, get+info->SrcWidth, put, info, mapInfo);
            get += src_p;
            put += dst_p;
        }
        break;
    default:
        for (sy = info->SrcPositionY; sy < info->SrcPositionY + info->SrcHeight; ) {
            repeaty = mapInfo->MappingHeight[sy%mapInfo->MappingY];
            sy++;
            if (repeaty == 0) {
                get += src_p;
                continue;
            } else {
                gobj_sx_to_dx_best(get, get+info->SrcWidth, put, info, mapInfo);
                get += src_p;
                put += dst_p;
                for (j=1; j<repeaty; j++) {
                    memcpy(put, put-dst_p, info->DstWidth<<sft);
                    put += dst_p;
                }
            }
        }
    }

    return 0;
}

/**
 * Software scalar 90 degrees (32bits)
 */
static int AppLibUtilityScalar_32Bit_Flow_Rotate90(int sxSta, int sxEnd, int sySta, int syEnd,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sx, sy, repeatx, repeaty, sft = 2;
    UINT32 *get, *put, *putSta;

    put = (UINT32 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sx = sxSta; sx <= sxEnd; sx++) {
        repeaty = selH[sx];
        if (repeaty == 0) continue;
        // handle each line
        get = (UINT32 *)info->SrcBufferAddress + (syEnd * info->SrcBufferPitch) + sx;
        putSta = put;
        for (sy = syEnd; sy >= sySta; sy--) {
                  repeatx = selW[sy];
            for (i=0; i<repeatx; i++, put++) { //repeatx
                *put = *get;
            }
            get -= info->SrcBufferPitch;
        }
        info->DstWidth = put - putSta;
        put += (info->DstBufferPitch - info->DstWidth);
        dy++;
        // repeat line if needed
        for (i=1; i<repeaty; i++) {         //repeaty
            memcpy(put, put-info->DstBufferPitch, info->DstWidth<<sft);
            put += info->DstBufferPitch;
            dy++;
        }
    }
    info->DstHeight = dy - dySta;

    return 0;
}

/**
 * Software scalar 270 degrees (32bits)
 */
static int AppLibUtilityScalar_32Bit_Flow_Rotate270(int sxSta, int sxEnd, int sySta, int syEnd,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sx, sy, repeatx, repeaty, sft = 2;
    UINT32 *get, *put, *putSta;

    put = (UINT32 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sx = sxEnd; sx >= sxSta; sx--) {
        repeaty = selH[sx];
        if (repeaty == 0) continue;
        // handle each line
        get = (UINT32 *)info->SrcBufferAddress + (sySta * info->SrcBufferPitch) + sx;
        putSta = put;
        for (sy = sySta; sy <= syEnd; sy++) {
                  repeatx = selW[sy];
            for (i=0; i<repeatx; i++, put++) { //repeatx
                *put = *get;
            }
            get += info->SrcBufferPitch;
        }
        info->DstWidth = put - putSta;
        put += (info->DstBufferPitch - info->DstWidth);
        dy++;
        // repeat line if needed
        for (i=1; i<repeaty; i++)  {         //repeaty
            memcpy(put, put-info->DstBufferPitch, info->DstWidth<<sft);
            put += info->DstBufferPitch;
            dy++;
        }
    }
    info->DstHeight = dy - dySta;

    return 0;
}

/**
 * Software scalar 180 degrees (32bits)
 */
static int AppLibUtilityScalar_32Bit_Flow_Rotate180(int sxSta, int sxEnd, int sySta, int syEnd,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sy, repeaty, sft = 1;
    UINT32 *get, *put;

    put = (UINT32 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sy = syEnd; sy >= sySta; sy--) {
        repeaty = selH[sy];
        if (repeaty == 0) continue;
        // handle each line
        get = (UINT32 *)info->SrcBufferAddress + (sy * info->SrcBufferPitch) + sxEnd;
        info->DstWidth = AppLibUtilityScalar_32Bit_Span_Reverse(get, get-info->SrcWidth, put, &selW[sxEnd]);
        get -= info->SrcBufferPitch;
        put += info->DstBufferPitch;
        dy++;
        // repeat line if needed
        for (i=1; i<repeaty; i++) {     //repeaty
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
static void AppLibUtilityScalar_32Bit_Span_SlowTransColor(UINT32 *start, UINT32 *end, UINT32 *target, APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT32 *sx;
    UINT32 repeatx, offset, w = end-start;
    int i;
    /* duplicate repeatx times by table (map_w[offset]) */
    for (offset=0; offset<w; offset++) {
        sx = start + offset;
        // future work: use ones_run_len coding to do memcpy and enhance perf
        repeatx = mapInfo->MappingWidth[(info->SrcPositionX+offset)%mapInfo->MappingX];
        if (mapInfo->TransColor != *sx) {
            for (i=0; i<repeatx; i++, target++) { //repeatx
                *target = *sx;
            }
        } else {
            target += repeatx;
        }
    }
}

/**
 * Software scalar line processing function for 4 byte (180 degree)
 */
static inline int AppLibUtilityScalar_32Bit_Span_ReverseTransColor(UINT32 *start, UINT32 *end, UINT32 *target, UINT8 *mapW, UINT32 TransColor)
{
    UINT32 *sx, *dx = target;
    UINT32 repeatx;
    int i;
    /* duplicate repeatx times by table (map_w[offset])   */
    for (sx = start; sx >= end; sx--, mapW--) {
        // future work: use ones_run_len coding to do memcpy and enhance perf
        // don't use "%", it's a dividing opcode! see assembly!!
        repeatx = *mapW;
        if (repeatx == 0) continue;
        if (TransColor != *sx) {
            for (i=0; i<repeatx; i++, target++) { //repeatx
                *target = *sx;
            }
        } else {
            target += repeatx;
        }
    }
    return (target - dx);
}

/**
 * Software scalar do mapping (32bits) with TransColor checking
 */
static int AppLibUtilityScalar_32Bit_Flow_ExeMappingTransColor(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT32 *get, *put, repeaty;
    int sy, j;
    int src_p = info->SrcBufferPitch;
    int dst_p = info->DstBufferPitch;
    // decide sx & put pointer
    get = (UINT32 *)info->SrcBufferAddress + (info->SrcPositionY * src_p) + info->SrcPositionX;
    put = (UINT32 *)info->DstBufferAddress + (info->DstPositionY * dst_p) + info->DstPositionX;

    for (sy = info->SrcPositionY; sy < info->SrcPositionY + info->SrcHeight; ) {
        repeaty = mapInfo->MappingHeight[sy%mapInfo->MappingY];
        sy++;
        if (repeaty == 0) {
            get += src_p;
            continue;
        } else {
            for (j=0; j<repeaty; j++) {
                AppLibUtilityScalar_32Bit_Span_SlowTransColor(get, get+info->SrcWidth, put, info, mapInfo);
                put += dst_p;
            }
            get += src_p;
        }
    }

    return 0;
}

/**
 * Software scalar 90 degrees (32bits) with TransColor checking
 */
static int AppLibUtilityScalar_32Bit_Flow_Rotate90_TransColor(int sxSta, int sx_end, int sySta, int sy_end,
                    int dxSta, int dySta, int dx, int dy, UINT8 *sel_w, UINT8 *sel_h,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sx, sy, repeatx, repeaty;
    UINT32 *get, *put, *putSta;

    put = (UINT32 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sx = sxSta; sx <= sx_end; sx++) {
        repeaty = sel_h[sx];
        if (repeaty == 0) {
            continue;
        } else {
            for (i=0; i<repeaty; i++) {         //repeaty
                // handle each line
                get = (UINT32 *)info->SrcBufferAddress + (sy_end * info->SrcBufferPitch) + sx;
                putSta = put;
                for (sy = sy_end; sy >= sySta; sy--) {
                          repeatx = sel_w[sy];
                    if (info->TransColor != *get) {
                        for (i=0; i<repeatx; i++, put++) { //repeatx
                            *put = *get;
                        }
                    } else {
                        put += repeatx;
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
 * Software scalar 270 degrees (32bits) with TransColor checking
 */
static int AppLibUtilityScalar_32Bit_Flow_Rotate270_TransColor(int sxSta, int sxEnd, int sySta, int syEnd,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sx, sy, repeatx, repeaty;
    UINT32 *get, *put, *putSta;

    put = (UINT32 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sx = sxEnd; sx >= sxSta; sx--) {
        repeaty = selH[sx];
        if (repeaty == 0) {
            continue;
        } else {
            for (i=0; i<repeaty; i++)  {         //repeaty
                // handle each line
                get = (UINT32 *)info->SrcBufferAddress + (sySta * info->SrcBufferPitch) + sx;
                putSta = put;
                for (sy = sySta; sy <= syEnd; sy++) {
                          repeatx = selW[sy];
                    if (info->TransColor != *get) {
                        for (i=0; i<repeatx; i++, put++) { //repeatx
                            *put = *get;
                        }
                    } else {
                        put += repeatx;
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
 * Software scalar 180 degrees (32bits) with TransColor checking
 */
static int AppLibUtilityScalar_32Bit_Flow_Rotate180_TransColor(int sxSta, int sxEnd, int sySta, int syEnd,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sy, repeaty;
    UINT32 *get, *put;

    put = (UINT32 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sy = syEnd; sy >= sySta; sy--) {
        repeaty = selH[sy];
        if (repeaty == 0) {
            continue;
        } else {
            for (i=0; i<repeaty; i++) {     //repeaty
                // handle each line
                get = (UINT32 *)info->SrcBufferAddress + (sy * info->SrcBufferPitch) + sxEnd;
                info->DstWidth = AppLibUtilityScalar_32Bit_Span_ReverseTransColor(get, get-info->SrcWidth, put, &selW[sxEnd], info->TransColor);
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
int AppLibUtilityScalar_32Bit_Normal(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    AppLibUtilitySWScalar_Flow_IndexSelector(info, mapInfo, 0);
    AppLibUtilitySWScalar_Flow_SetMappingInfo(info, mapInfo);
    if (info->TransColorEn == 1)
        AppLibUtilityScalar_32Bit_Flow_ExeMappingTransColor(info, mapInfo);
    else
        AppLibUtilityScalar_32Bit_Flow_Mapping(info, mapInfo);

    return 0;
}

/**
 * Software scalar for normal case with rotation
 */
int AppLibUtilityScalar_32Bit_Rotate(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo, int angle)
{
    UINT8 sel_w[MAX_SCALAR_WIDTH], sel_h[MAX_SCALAR_HEIGHT];
    int dx, dy, sx_sta, sx_end, sy_sta, sy_end, dx_sta, dy_sta;
    int bx, by, bx_sta, by_sta;
    UINT32 mapx, mapy;

    memset(sel_w ,0 ,MAX_SCALAR_WIDTH);
    memset(sel_h ,0 ,MAX_SCALAR_HEIGHT);

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
            sel_w[bx] = mapInfo->MappingWidth[bx%mapx];
        for (by=0; by<(info->SrcBufferWidth); by++)
            sel_h[by] = mapInfo->MappingHeight[by%mapy];
        break;
    case 180:
        for (bx=0; bx<(info->SrcBufferWidth); bx++)
            sel_w[bx] = mapInfo->MappingWidth[bx%mapx];
        for (by=0; by<(info->SrcBufferHeight); by++)
            sel_h[by] = mapInfo->MappingHeight[by%mapy];
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
            bx_sta = info->SrcPositionY + info->SrcHeight - 1;
            for (bx=info->SrcBufferHeight; bx>bx_sta; bx--)
                info->DstPositionX += sel_w[bx];
            // decide DstPositionY by accumulating by_sta selected indices
            by_sta = info->SrcPositionX;
            for (by=0; by<by_sta; by++)
                info->DstPositionY += sel_h[by];
            break;
        case 270:
            // decide des_x by accumulating bx_sta selected indices
            bx_sta = info->SrcPositionY;
            for (bx=0; bx<bx_sta; bx++)
                info->DstPositionX += sel_w[bx];
            // decide DstPositionY by accumulating by_sta selected indices
            by_sta = info->SrcPositionX + info->SrcWidth - 1;
            for (by=info->SrcBufferWidth; by>by_sta; by--)
                info->DstPositionY += sel_h[by];
            break;
        case 180:
            // decide des_x by accumulating bx_sta selected indices
            bx_sta = info->SrcPositionX + info->SrcWidth - 1;
            for (bx=info->SrcBufferWidth; bx>bx_sta; bx--)
                info->DstPositionX += sel_w[bx];
            // decide DstPositionY by accumulating by_sta selected indices
            by_sta = info->SrcPositionY + info->SrcHeight - 1;
            for (by=info->SrcBufferHeight; by>by_sta; by--)
                info->DstPositionY += sel_h[by];
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
    sx_sta = info->SrcPositionX;
    sx_end = MIN(info->SrcPositionX + info->SrcWidth - 1, info->SrcBufferWidth - 1);
    sy_sta = info->SrcPositionY;
    sy_end = MIN(info->SrcPositionY + info->SrcHeight - 1, info->SrcBufferHeight - 1);
    dx_sta = info->DstPositionX;
    dy_sta = info->DstPositionY;
    dx = info->DstPositionX;
    dy = info->DstPositionY;
    // start rotation mapping
    switch (angle) {
    case 90:
        if (info->TransColorEn == 1) {
            AppLibUtilityScalar_32Bit_Flow_Rotate90_TransColor(sx_sta, sx_end, sy_sta, sy_end, dx_sta, dy_sta, dx, dy, sel_w, sel_h, info);
        } else {
            AppLibUtilityScalar_32Bit_Flow_Rotate90(sx_sta, sx_end, sy_sta, sy_end, dx_sta, dy_sta, dx, dy, sel_w, sel_h, info);
        }
        break;
    case 270:
        if (info->TransColorEn == 1) {
            AppLibUtilityScalar_32Bit_Flow_Rotate270_TransColor(sx_sta, sx_end, sy_sta, sy_end, dx_sta, dy_sta, dx, dy, sel_w, sel_h, info);
        } else {
            AppLibUtilityScalar_32Bit_Flow_Rotate270(sx_sta, sx_end, sy_sta, sy_end, dx_sta, dy_sta, dx, dy, sel_w, sel_h, info);
        }
        break;
    case 180:
        if (info->TransColorEn == 1) {
            AppLibUtilityScalar_32Bit_Flow_Rotate180_TransColor(sx_sta, sx_end, sy_sta, sy_end, dx_sta, dy_sta, dx, dy, sel_w, sel_h, info);
        } else {
            AppLibUtilityScalar_32Bit_Flow_Rotate180(sx_sta, sx_end, sy_sta, sy_end, dx_sta, dy_sta, dx, dy, sel_w, sel_h, info);
        }
        break;
    default:
        // No rotation
        return 0;
    }

    return 0;
}
