/**
 * @file src/app/connected/applib/src/utility/swscalar/ApplibUtility_SW_Scalar_8Bit.c
 *
 * SW scalar for 8 bits
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
 * add 1/mapx ,thus duplicate one x after mapx points  R/W = UINT8/UINT8
 */
static void AppLibUtilityScalar_8Bit_Span_AddOne(UINT8 *sx8, UINT8 *end, UINT8 *dx8, APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT32 tval, mapx = mapInfo->MappingX;

    switch (mapx) {
        case 1:
        /*  code density --->
         *
         *  2B  4B  8B        Wr DstWidth align
         * ----------------------------
         *  1B  2B  4B        Rd SrcWidth align
         *  0~1 src_nonalign number
         */
            tval = (end-sx8) & 0x01;
            if (tval) {
                dx8[1] = dx8[0] = *sx8++;
                dx8 += 2;
            }
            while (sx8 < end) {
                dx8[1] = dx8[0] = sx8[0];
                dx8[3] = dx8[2] = sx8[1];
                sx8 += 2;
                dx8 += 4;
            }
            break;

        case 2:
        /*  code density --->
         *
         *  3B  6B  12B        Wr DstWidth align
         * ----------------------------
         *  2B  4B  8B        Rd SrcWidth align
         *  0,2,4,6 src_nonalign number
         */
            tval = (end-sx8) & 0x07;
            while (tval) {
                dx8[0] = sx8[0];
                dx8[2] = dx8[1] = sx8[1];
                sx8 += 2;
                 dx8 += 3;
                tval -= 2;
            }
            while (sx8 < end) {
                dx8[0] = *sx8++;
                dx8[2] = dx8[1] = *sx8++;

                dx8[3] = *sx8++;
                dx8[5] = dx8[4] = *sx8++;
                dx8 += 6;

                dx8[0] = *sx8++;
                dx8[2] = dx8[1] = *sx8++;

                dx8[3] = *sx8++;
                dx8[5] = dx8[4] = *sx8++;
                dx8 += 6;
            }
            break;

        case 3:
        /*  code density --->
         *
         *  4B  8B  16B        Wr DstWidth align
         * ----------------------------
         *  3B  6B  12B        Rd SrcWidth align
         *  we know SrcWidth is 3B alignment ,
         *  to check src 6 alignment ,SrcWidth & 1 = 0
         */
            while (sx8 < end) {
                dx8[0] = *sx8++;
                dx8[1] = *sx8++;
                dx8[3] = dx8[2] = *sx8++;
                dx8 += 4;
            }
            break;

        default:
            while (sx8 < end) {
                memcpy(dx8, sx8, mapx);
                dx8 += mapx;
                *dx8 = *(dx8-1);
                dx8++;
                sx8 += mapx;
            }
            break;
    }
}

/**
 * Software scalar line processing function (sub one cases)
 * sub 1/mapx ,thus bypass one x after mapx points  R/W = UINT8/UINT8
 */
static void AppLibUtilityScalar_8Bit_Span_SubOne(UINT8 *sx8, UINT8 *end, UINT8 *dx8, APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT32 val, mapx = mapInfo->MappingX, mapxsubone = mapx-1;

    switch (mapxsubone) {
        case 1:
        /*  code density --->
         *
         *  1B  2B  4B        Wr DstWidth align
         * ----------------------------
         *  2B  4B  8B        Rd SrcWidth align
         *  0~7 src_nonalign number
         */
            val = (end-sx8) & 0x07;
            while (val) {
                *dx8++ = *sx8++;
                sx8 ++;
                val--;
            }
            while (sx8 < end) {
                dx8[0] = sx8[0];
                dx8[1] = sx8[2];
                dx8[2] = sx8[4];
                dx8[3] = sx8[6];
                dx8 += 4;
                sx8 += 8;
            }
            break;
        case 2:
        /*  code density --->
         *
         *  2B  4B  8B        Wr DstWidth align
         * ----------------------------
         *  3B  6B  12B        Rd SrcWidth align
         */
         //SrcWidth = 3*k ,k = 2n   ==> SrcWidth = 6n
         //SrcWidth = 3*k ,k = 2n+1 ==> SrcWidth = 6n+3
            val = (end-sx8) & 0x01;
            if (val) {
                *dx8++ = *sx8++;
                *dx8++ = *sx8++;
                sx8 ++;
            }
            while (sx8 < end) {
                dx8[0] = sx8[0];
                dx8[1] = sx8[1];
                dx8[2] = sx8[3];
                dx8[3] = sx8[4];
                dx8 += 4;
                sx8 += 6;
            }
            break;
        case 3:
        /*  code density --->
         *
         *  3B  6B  12B        Wr DstWidth align
         * ----------------------------
         *  4B  8B  16B        Rd SrcWidth align
         *  0,4,8,12 src_nonalign number
         */
             val = (end-sx8) & 0x0F;
            while (val) {
                *dx8++ = *sx8++;
                *dx8++ = *sx8++;
                *dx8++ = *sx8++;
                sx8 ++;
                val -= 4;
            }
            while (sx8 < end) {
                dx8[0] = sx8[0];
                dx8[1] = sx8[1];
                dx8[2] = sx8[2];
                dx8[3] = sx8[4];
                dx8[4] = sx8[5];
                dx8[5] = sx8[6];

                dx8 += 6;
                sx8 += 8;

                dx8[0] = sx8[0];
                dx8[1] = sx8[1];
                dx8[2] = sx8[2];
                dx8[3] = sx8[4];
                dx8[4] = sx8[5];
                dx8[5] = sx8[6];

                dx8 += 6;
                sx8 += 8;
            }
            break;
        default:
            while (sx8 < end) {
                memcpy(dx8, sx8, mapxsubone);
                sx8 += mapx;
                dx8 += mapxsubone;
            }
            break;
    }
}

/**
 * Software scalar line processing function (equal cases)
 */
static inline void AppLibUtilityScalar_8Bit_Span_Equal(UINT8 *start, UINT8 *end, UINT8 *target, APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    memcpy(target, start, end-start);
}

/**
 * Software scalar line processing function (other cases)
 */
static void AppLibUtilityScalar_8Bit_Span_Slow(UINT8 *start, UINT8 *end, UINT8 *target, APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT8 *sx;
    UINT32 repeatx, offset, w = end-start;
    /* duplicate repeatx times by table (map_w[offset]) */
    for (offset=0; offset<w; offset++) {
        sx = start + offset;
        // future work: use ones_run_len coding to do memcpy and enhance perf
        repeatx = mapInfo->MappingWidth[(info->SrcPositionX+offset)%mapInfo->MappingX];
        memset(target, *sx, repeatx);
        target += repeatx;
    }
}

/**
 * Software scalar line processing function (180 degree)
 */
static inline int AppLibUtilityScalar_8Bit_Span_Reverse(UINT8 *start, UINT8 *end, UINT8 *target, UINT8 *mapW)
{
    UINT8 *sx, *dx = target;
    UINT32 repeatx;
    /* duplicate repeatx times by table (map_w[offset])   */
    for (sx = start; sx >= end; sx--, mapW--) {
        // future work: use ones_run_len coding to do memcpy and enhance perf
        // don't use "%", it's a dividing opcode! see assembly!!
        repeatx = *mapW;
        if (repeatx == 0) continue;
        memset(target, *sx, repeatx);
        target += repeatx;
    }
    return (target - dx);
}

/**
 * Software scalar do mapping (8bits)
 */
static int AppLibUtilityScalar_8Bit_Flow_Mapping(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT8 *get, *put, repeaty;
    int sy, j;
    int signx = mapInfo->ToX - mapInfo->MappingX;
    int signy = mapInfo->ToY - mapInfo->MappingY;
    void (*gobj_sx_to_dx_best)(UINT8*, UINT8*, UINT8*, APPLIB_SW_SCALAR_s *, p_APPLIB_SW_SCALAR_MAP_s);
    int src_p = info->SrcBufferPitch;
    int dst_p = info->DstBufferPitch;
    // decide sx & put pointer
    get = (UINT8 *)info->SrcBufferAddress + (info->SrcPositionY * src_p) + info->SrcPositionX;
    put = (UINT8 *)info->DstBufferAddress + (info->DstPositionY * dst_p) + info->DstPositionX;

    // decide line processor according to relation between mapx & to_x
    switch (signx) {
    case 1:
        gobj_sx_to_dx_best = AppLibUtilityScalar_8Bit_Span_AddOne;
        break;
    case -1:
        gobj_sx_to_dx_best = AppLibUtilityScalar_8Bit_Span_SubOne;
        break;
    case 0:
        gobj_sx_to_dx_best = AppLibUtilityScalar_8Bit_Span_Equal;
        break;
    default:
        gobj_sx_to_dx_best = AppLibUtilityScalar_8Bit_Span_Slow;
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
            memcpy(put, put-dst_p, info->DstWidth);
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
            repeaty = mapInfo->MappingHeight[sy%mapInfo->MappingY];    //don't use % !!!!!
            sy++;
            if (repeaty == 0) {
                get += src_p;
                continue;
            } else {
                gobj_sx_to_dx_best(get, get+info->SrcWidth, put, info, mapInfo);
                get += src_p;
                put += dst_p;
                for (j=1; j<repeaty; j++) {
                    memcpy(put, put-dst_p, info->DstWidth);
                    put += dst_p;
                }
            }
        }
    }

    return 0;
}

/**
 * Software scalar 90 degrees (8bits)
 */
static int AppLibUtilityScalar_8Bit_Flow_Rotate90(int sx_sta, int sx_end, int sy_sta, int sy_end,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sx, sy, repeatX, repeatY;
    UINT8 *get, *put, *putSta;

    put = (UINT8 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sx = sx_sta; sx <= sx_end; sx++) {
        repeatY = selH[sx];
        if (repeatY == 0) continue;
        // handle each line
        get = (UINT8 *)info->SrcBufferAddress + (sy_end * info->SrcBufferPitch) + sx;
        putSta = put;
        for (sy = sy_end; sy >= sy_sta; sy--) {
                  repeatX = selW[sy];
            memset(put, *get, repeatX);     //repeatx
            put += repeatX;
            get -= info->SrcBufferPitch;
        }
        info->DstWidth = put - putSta;
        put += (info->DstBufferPitch - info->DstWidth);
        dy++;
        // repeat line if needed
        for (i=1; i<repeatY; i++) {         //repeaty
            memcpy(put, put-info->DstBufferPitch, info->DstWidth);
            put += info->DstBufferPitch;
            dy++;
        }
    }
    info->DstHeight = dy - dySta;

    return 0;
}

/**
 * Software scalar 270 degrees (8bits)
 */
static int AppLibUtilityScalar_8Bit_Flow_Rotate270(int sxSta, int sxEnd, int sySta, int syEnd,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sx, sy, repeatX, repeatY;
    UINT8 *get, *put, *putSta;

    put = (UINT8 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sx = sxEnd; sx >= sxSta; sx--) {
        repeatY = selH[sx];
        if (repeatY == 0) continue;
        // handle each line
        get = (UINT8 *)info->SrcBufferAddress + (sySta * info->SrcBufferPitch) + sx;
        putSta = put;
        for (sy = sySta; sy <= syEnd; sy++) {
                  repeatX = selW[sy];
            memset(put, *get, repeatX);    //repeatx
            put += repeatX;
            get += info->SrcBufferPitch;
        }
        info->DstWidth = put - putSta;
        put += (info->DstBufferPitch - info->DstWidth);
        dy++;
        // repeat line if needed
        for (i=1; i<repeatY; i++)  {         //repeaty
            memcpy(put, put-info->DstBufferPitch, info->DstWidth);
            put += info->DstBufferPitch;
            dy++;
        }
    }
    info->DstHeight = dy - dySta;

    return 0;
}

/*  _RESIZE_NEED_
 *
 * Software scalar 180 degrees (8bits)
 */
static int AppLibUtilityScalar_8Bit_Flow_Rotate180(int sxSta, int sxEnd, int sySta, int syEnd,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sy, repeaty;
    UINT8 *get, *put;

    put = (UINT8 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sy = syEnd; sy >= sySta; sy--) {
        repeaty = selH[sy];
        if (repeaty == 0) continue;
        // handle each line
        get = (UINT8 *)info->SrcBufferAddress + (sy * info->SrcBufferPitch) + sxEnd;
        info->DstWidth = AppLibUtilityScalar_8Bit_Span_Reverse(get, get-info->SrcWidth, put, &selW[sxEnd]);
        get -= info->SrcBufferPitch;
        put += info->DstBufferPitch;
        dy++;
        // repeat line if needed
        for (i=1; i<repeaty; i++) {     //repeaty
            memcpy(put, put-info->DstBufferPitch, info->DstWidth);
            put += info->DstBufferPitch;
            dy++;
        }
    }
    info->DstHeight = dy - dySta;

    return 0;
}

#define _RESIZE_NEED_
/**
 * Software scalar line processing function (other cases with tcolor checking)
 */
static void AppLibUtilityScalar_8Bit_Span_SlowTransColor(UINT8 *start, UINT8 *end, UINT8 *target, APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT8 *sx;
    UINT32 repeatX, offset, w = end-start;
    /* duplicate repeatx times by table (map_w[offset]) */
    for (offset=0; offset<w; offset++) {
        sx = start + offset;
        // future work: use ones_run_len coding to do memcpy and enhance perf
        repeatX = mapInfo->MappingWidth[(info->SrcPositionX+offset)%mapInfo->MappingX];
        if (mapInfo->TransColor != *sx) memset(target, *sx, repeatX);
        target += repeatX;
    }
}

/**
 * Software scalar line processing function (180 degree) with tcolor checking
 */
static int AppLibUtilityScalar_8Bit_Span_ReverseTransColor(UINT8 *start, UINT8 *end, UINT8 *target, UINT8 *mapW, UINT32 tcolor)
{
    UINT8 *sx, *dx = target;
    UINT32 repeatX;
    /* duplicate repeatx times by table (map_w[offset])   */
    for (sx = start; sx >= end; sx--, mapW--) {
        // future work: use ones_run_len coding to do memcpy and enhance perf
        // don't use "%", it's a dividing opcode! see assembly!!
        repeatX = *mapW;
        if (repeatX == 0) continue;
        if (tcolor != *sx) memset(target, *sx, repeatX);
        target += repeatX;
    }
    return (target - dx);
}

/**
 * Software scalar do mapping (8bits) with tcolor checking
 */
static int AppLibUtilityScalar_8Bit_Flow_ExeMappingTransColor(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    UINT8 *get, *put, repeatY;
    int sy, j;
    int srcP = info->SrcBufferPitch;
    int dstP = info->DstBufferPitch;
    // decide sx & put pointer
    get = (UINT8 *)info->SrcBufferAddress + (info->SrcPositionY * srcP) + info->SrcPositionX;
    put = (UINT8 *)info->DstBufferAddress + (info->DstPositionY * dstP) + info->DstPositionX;

    for (sy = info->SrcPositionY; sy < info->SrcPositionY + info->SrcHeight; ) {
        repeatY = mapInfo->MappingHeight[sy%mapInfo->MappingY];    //don't use % !!!!!
        sy++;
        if (repeatY == 0) {
            get += srcP;
            continue;
        } else {
            for (j=0; j<repeatY; j++) {
                AppLibUtilityScalar_8Bit_Span_SlowTransColor(get, get+info->SrcWidth, put, info, mapInfo);
                put += dstP;
            }
            get += srcP;
        }
    }

    return 0;
}

/**
 * Software scalar 90 degrees (8bits) with tcolor checking
 */
static int AppLibUtilityScalar_8Bit_Flow_Rotate90_TransColor(int sxSta, int sxEnd, int sySta, int syEnd,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sx, sy, repeatX, repeatY;
    UINT8 *get, *put, *putSta;

    put = (UINT8 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sx = sxSta; sx <= sxEnd; sx++) {
        repeatY = selH[sx];
        if (repeatY == 0) {
            continue;
        } else {
            for (i=0; i<repeatY; i++) {         //repeaty
                // handle each line
                get = (UINT8 *)info->SrcBufferAddress + (syEnd * info->SrcBufferPitch) + sx;
                putSta = put;
                for (sy = syEnd; sy >= sySta; sy--) {
                          repeatX = selW[sy];
                    if (info->TransColor != *get) memset(put, *get, repeatX);     //repeatx
                    put += repeatX;
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
 * Software scalar 270 degrees (8bits) with tcolor checking
 */
static int AppLibUtilityScalar_8Bit_Flow_Rotate270_TransColor(int sxSta, int sxEnd, int sySta, int syEnd,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sx, sy, repeatX, repeatY;
    UINT8 *get, *put, *putSta;

    put = (UINT8 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sx = sxEnd; sx >= sxSta; sx--) {
        repeatY = selH[sx];
        if (repeatY == 0) {
            continue;
        } else {
            for (i=0; i<repeatY; i++)  {         //repeaty
                // handle each line
                get = (UINT8 *)info->SrcBufferAddress + (sySta * info->SrcBufferPitch) + sx;
                putSta = put;
                for (sy = sySta; sy <= syEnd; sy++) {
                          repeatX = selW[sy];
                    if (info->TransColor != *get) memset(put, *get, repeatX);    //repeatx
                    put += repeatX;
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
 * Software scalar 180 degrees (8bits) with tcolor checking
 */
static int AppLibUtilityScalar_8Bit_Flow_Rotate180_TransColor(int sxSta, int sxEnd, int sySta, int syEnd,
                    int dxSta, int dySta, int dx, int dy, UINT8 *selW, UINT8 *selH,
                    APPLIB_SW_SCALAR_s *info)
{
    int i, sy, repeatY;
    UINT8 *get, *put;

    put = (UINT8 *)info->DstBufferAddress + (dySta * info->DstBufferPitch) + dxSta;
    for (sy = syEnd; sy >= sySta; sy--) {
        repeatY = selH[sy];
        if (repeatY == 0) {
            continue;
        } else {
            for (i=0; i<repeatY; i++) {     //repeaty
                // handle each line
                get = (UINT8 *)info->SrcBufferAddress + (sy * info->SrcBufferPitch) + sxEnd;
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
 * Software scalar for normal case with rotation
 */
int AppLibUtilityScalar_8Bit_Rotate(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo, int angle)
{
    UINT8 selW[MAX_SCALAR_WIDTH], selH[MAX_SCALAR_HEIGHT];
    int dx, dy, sxSta, sxEnd, sySta, syEnd, dxSta, dySta;
    int bx, by, bxSta, by_sta;
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
            by_sta = info->SrcPositionX;
            for (by=0; by<by_sta; by++)
                info->DstPositionY += selH[by];
            break;
        case 270:
            // decide des_x by accumulating bx_sta selected indices
            bxSta = info->SrcPositionY;
            for (bx=0; bx<bxSta; bx++)
                info->DstPositionX += selW[bx];
            // decide DstPositionY by accumulating by_sta selected indices
            by_sta = info->SrcPositionX + info->SrcWidth - 1;
            for (by=info->SrcBufferWidth; by>by_sta; by--)
                info->DstPositionY += selH[by];
            break;
        case 180:
            // decide des_x by accumulating bx_sta selected indices
            bxSta = info->SrcPositionX + info->SrcWidth - 1;
            for (bx=info->SrcBufferWidth; bx>bxSta; bx--)
                info->DstPositionX += selW[bx];
            // decide DstPositionY by accumulating by_sta selected indices
            by_sta = info->SrcPositionY + info->SrcHeight - 1;
            for (by=info->SrcBufferHeight; by>by_sta; by--)
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
            AppLibUtilityScalar_8Bit_Flow_Rotate90_TransColor(sxSta, sxEnd, sySta, syEnd, dxSta, dySta, dx, dy, selW, selH, info);
        } else {
            AppLibUtilityScalar_8Bit_Flow_Rotate90(sxSta, sxEnd, sySta, syEnd, dxSta, dySta, dx, dy, selW, selH, info);
        }
        break;
    case 270:
        if (info->TransColorEn == 1) {
            AppLibUtilityScalar_8Bit_Flow_Rotate270_TransColor(sxSta, sxEnd, sySta, syEnd, dxSta, dySta, dx, dy, selW, selH, info);
        } else {
            AppLibUtilityScalar_8Bit_Flow_Rotate270(sxSta, sxEnd, sySta, syEnd, dxSta, dySta, dx, dy, selW, selH, info);
        }
        break;
    case 180:
        if (info->TransColorEn == 1) {
            AppLibUtilityScalar_8Bit_Flow_Rotate180_TransColor(sxSta, sxEnd, sySta, syEnd, dxSta, dySta, dx, dy, selW, selH, info);
        } else {
            AppLibUtilityScalar_8Bit_Flow_Rotate180(sxSta, sxEnd, sySta, syEnd, dxSta, dySta, dx, dy, selW, selH, info);
        }
        break;
    default:
        // No rotation
        return 0;
    }

    return 0;
}

/*
 * Software scalar for normal case
 */
int AppLibUtilityScalar_8Bit_Normal(APPLIB_SW_SCALAR_s *info, p_APPLIB_SW_SCALAR_MAP_s mapInfo)
{
    AppLibUtilitySWScalar_Flow_IndexSelector(info, mapInfo, 0);
    AppLibUtilitySWScalar_Flow_SetMappingInfo(info, mapInfo);
    if (info->TransColorEn == 1)
        AppLibUtilityScalar_8Bit_Flow_ExeMappingTransColor(info, mapInfo);
    else
        AppLibUtilityScalar_8Bit_Flow_Mapping(info, mapInfo);

    return 0;
}

