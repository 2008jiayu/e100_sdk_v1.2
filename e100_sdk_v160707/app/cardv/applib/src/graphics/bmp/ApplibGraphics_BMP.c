/**
  * @file applib/src/graphics/bmp/ApplibGraphics_BMP.c
  *
  * ApplibGraphics_BMP source code
  *
  * History:
  *    2013/11/15 - [Eric Yen] created file
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

#include "bmp.h"
#include <graphics/bmp/ApplibGraphics_Bmp.h>
#include <graphics/render/ApplibGraphics_Render.h>
#include <graphics/obj/ApplibGraphics_Obj.h>
#include <graphics/UIObj/ApplibGraphics_UIObj.h>
#include <utility/ApplibUtility_SW_Scalar.h>

extern int AppLibRender_Init(APPLIB_GRAPHIC_RENDER_s *render);

#if 0
static int AppLibBMP_Resize(APPLIB_GRAPHIC_RENDER_s *srcRender, APPLIB_GRAPHIC_RENDER_s *targetRender, AMP_AREA_s *drawArea)
{
    APPLIB_SW_SCALAR_s scalarParam = {0};

    scalarParam.SrcBufferAddress = (UINT32)srcRender->Buf;
    scalarParam.SrcBufferPitch = srcRender->BufPitch;
    scalarParam.SrcBufferWidth = srcRender->BufPitch;
    scalarParam.SrcBufferHeight = srcRender->BufHeight;
    scalarParam.SrcPositionX = 0;
    scalarParam.SrcPositionY = 0;
    scalarParam.SrcWidth = drawArea->Width * srcRender->BufPixelSize;
    scalarParam.SrcHeight = drawArea->Height;

    scalarParam.DstBufferAddress = (UINT32)targetRender->Buf;
    scalarParam.DstBufferPitch = targetRender->BufPitch;
    scalarParam.DstBufferWidth = drawArea->Width * targetRender->BufPixelSize;
    scalarParam.DstBufferHeight = drawArea->Height;
    scalarParam.DstPositionX = drawArea->X * targetRender->BufPixelSize;
    scalarParam.DstPositionY = drawArea->Y;
    scalarParam.DstWidth = drawArea->Width * targetRender->BufPixelSize;
    scalarParam.DstHeight = drawArea->Height;

    scalarParam.Type = targetRender->BufPixelSize * 1000;
    scalarParam.TransColorEn = 0;
    scalarParam.TransColor = 0xFFFFFFFF;
    scalarParam.RegardDstValue = SCALAR_WRITE_DST_VALUE;

    AppLibUtilitySWScalar_ExeScalar(&scalarParam);
    return 0;
}

static int AppLibBMP_RescaleBMP(void)
{
    void *tmpBuf = 0, *tmpRawBuf;
    UINT32 tmpSZ = bmp->Width * bmp->Height * bmp->Bits / 8;
    int ReternValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &tmpBuf, &tmpRawBuf, tmpSZ, 32);
    if (ReturnValue != OK) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "[Applib - StillEnc] <SingleCapture> NC_DDR alloc scriptAddr fail (%u)!", 128*10);
        return -1;
    }
    memset(tmpBuf, 0x0, tmpSZ);

    APPLIB_GRAPHIC_RENDER_s tmpRender = {0};
    tmpRender.Buf = tmpBuf;
    tmpRender.BufPitch = bmp->Width * bmp->Bits / 8;
    tmpRender.BufHeight = bmp->Height;
    tmpRender.BufPixelSize = bmp->Bits / 8;
    AppLibRender_Init(&tmpRender);
    AMP_AREA_s tmpArea = {0};
    tmpArea.X = 0;
    tmpArea.Y = 0;
    tmpArea.Width = bmp->Width;
    tmpArea.Height = bmp->Height;
    AppLibBMP_RLSDecode(&tmpRender, bmp, 0, 0, &tmpArea);

    drawArea->Width <<= 1;
    drawArea->Height <<= 1;
    AppLibBMP_Resize(&tmpRender, render, drawArea);
}
#endif

static int AppLibBMP_CheckPlotPixelValid(UINT32 val, AMP_DISP_OSD_FORMAT_e pixFormat)
{
    int ReturnValue = 1;

    switch (pixFormat) {
    case AMP_OSD_32BIT_AYUV_8888:
    case AMP_OSD_32BIT_ABGR_8888:
    case AMP_OSD_32BIT_ARGB_8888:
        if ((val & 0xFF000000) == 0) {
            ReturnValue = 0;
        }
        break;
    case AMP_OSD_32BIT_RGBA_8888:
    case AMP_OSD_32BIT_BGRA_8888:
        if ((val & 0x000000FF) == 0) {
            ReturnValue = 0;
        }
        break;
    case AMP_OSD_16BIT_AYUV_4444:
    case AMP_OSD_16BIT_ABGR_4444:
    case AMP_OSD_16BIT_ARGB_4444:
        if ((val & 0x0000F000) == 0) {
            ReturnValue = 1;
        }
        break;
    case AMP_OSD_16BIT_AYUV_1555:
    case AMP_OSD_16BIT_ABGR_1555:
    case AMP_OSD_16BIT_ARGB_1555:
        if ((val & 0x00008000) == 0) {
            ReturnValue = 1;
        }
        break;
    case AMP_OSD_16BIT_RGBA_5551:
    case AMP_OSD_16BIT_BGRA_5551:
        if ((val & 0x00000001) == 0) {
            ReturnValue = 1;
        }
        break;
    case AMP_OSD_8BIT_CLUT_MODE:
        if (val ==  0) {
            ReturnValue = 1;
        }
        break;
    case AMP_OSD_16BIT_VYU_RGB_565:
    case AMP_OSD_16BIT_UYV_BGR_565:
    case AMP_OSD_16BIT_YUV_1555:
    default:
        break;
    }
    return ReturnValue;
}

/**
 * BMP rendering implementations of APPLIB_GRAPHIC_BMP_BMF_RLE
 * @param param
 * @return
 */
static int AppLibBMP_RLSDecode(APPLIB_GRAPHIC_RENDER_s *render,
                        const APPLIB_GRAPHIC_BMP_s *bmp,
                        const UINT32 X,
                        const UINT32 Y,
                        const AMP_AREA_s *updateArea)
{
    UINT8 trans = 0;
    UINT32 line = 0, width = 0, end = 0, rl = 0, rt = 0, rr = 0, rb = 0, loop = 0;
    UINT32 val = 0, tColor = 0, cnt = 0;
    APPLIB_GRAPHIC_RENDER_s srcRender = {0};
    void *get = NULL, *put = NULL;

    if ( (render == NULL) || (bmp == NULL) || (updateArea == NULL) ) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "render 0x%X, bmp 0x%X, updateArea 0x%x", render, bmp, updateArea);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if ( (!render->Buf) || (!render->MoveTo_f) || (!render->PlotPixel_f) ) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "buf 0x%X, render moveto_f 0x%X, plotpixel_f 0x%X", render->Buf, render->MoveTo_f, render->PlotPixel_f);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "area x:%d y:%d w:%d h:%d", updateArea->X, updateArea->Y, updateArea->Width, updateArea->Height);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "AppLibBMP_RLSDecode BMP:0x%x flg:0x%x bits:0x%x width:%d height:%d pxf:%d res:0x%x tcolor:0x%x Ptr:0x%x",
                    bmp, bmp->Flags, bmp->Bits, bmp->Width, bmp->Height, bmp->Pxf, bmp->Reserve, bmp->TColor, bmp->Ptr);

    rl = updateArea->X;
    rt = updateArea->Y;
    rr = updateArea->X + bmp->Width - 1;   // start from 0
    rb = updateArea->Y + bmp->Height - 1;  // start from 0
    line = updateArea->Y;
    width = bmp->Width;
    end = updateArea->X + width;

    //render boundery clipping
    if (rl > render->BufPitch) {
        rl = render->BufPitch;
    }
    if (rt > render->BufHeight) {
        rt = render->BufHeight;
    }
    if (rr > render->BufPitch) {
        rr = render->BufPitch;
    }
    if (rb > render->BufHeight) {
        rb = render->BufHeight;
    }
    if (width > render->BufPitch) {
        width = render->BufPitch;
    }
    if (end > render->BufPitch) {
        end = render->BufPitch;
    }
    if (line > render->BufHeight) {
        line = render->BufHeight;
    }

    cnt = 0;
    srcRender.Buf = get = (void *)bmp->Ptr;
    srcRender.BufHeight = bmp->Height;
    srcRender.BufPitch = bmp->Width;
    srcRender.BufPixelSize = bmp->Bits / 8;
    AppLibRender_Init(&srcRender);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "srcRender buf:0x%x h:%d p:%d pix:%d",
                    srcRender.Buf, srcRender.BufHeight, srcRender.BufPitch, srcRender.BufPixelSize);
    trans = bmp->Flags & APPLIB_GRAPHIC_BMP_BMF2_HAS_TRANS;
    tColor = (trans)? bmp->TColor:0xFFFFFFFF;

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "rl:%d rt:%d rr:%d rb:%d line:%d width:%d end:%d trans:%d tcolo:0x%x get:0x%x",
                    rl, rt, rr, rb, line, width, end, trans, tColor, get);

    while (line < rt) {
        cnt = 0;
        while (cnt < width) {
            srcRender.GetPixel_f(get, &val); //len
            srcRender.MoveNext_f(&get);   //co
            srcRender.MoveNext_f(&get);
            val = (val & 0x7F) + 1;
            //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "cnt:%d val:%d get:0x%x", cnt, val, get);
            cnt += val;
        }
        line++;
    }
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "line:%d cnt:%d val:%d get:0x%x", line, cnt, val, get);

    if ((X>=rl) && ((X+width-1)<=rr)) {
        // no clipping needed in inner loop
        if (trans) {
            while (line <= rb) {
                render->MoveTo_f(render, &put, X, line);
                loop = X;
                while (loop < end) {
                    srcRender.GetPixel_f(get, &cnt);
                    srcRender.MoveNext_f(&get);
                    cnt = (cnt & 0x7F) + 1;

                    srcRender.GetPixel_f(get, &val);
                    srcRender.MoveNext_f(&get);

                    if (val == tColor) {  // the defaul transparency color
                        loop += cnt;
                        render->MoveNextFew_f(&put, cnt);
                    } else {
                        while (cnt--) {
                            // alpha
                            if ( AppLibBMP_CheckPlotPixelValid(val, bmp->Pxf) ) {
                                render->PlotPixel_f(put, val);
                            }
                            render->MoveNext_f(&put);
                            loop++;
                        }
                    }
                }
                line++;
            } //while (line <= rb)
        } else {
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "no clipping no trans");
            while (line <= rb) {
                render->MoveTo_f(render, &put, X, line);
                loop = X;
                while (loop < end) {
                    srcRender.GetPixel_f(get, &cnt);
                    srcRender.MoveNext_f(&get);
                    cnt = (cnt & 0x7F) + 1;

                    srcRender.GetPixel_f(get, &val);
                    srcRender.MoveNext_f(&get);
                    //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "try to draw: (%d %d) + %d", X, line, cnt);
                    while (cnt--) {
                        //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "draw at    : (%d %d)", loop, line);
                        if ( AppLibBMP_CheckPlotPixelValid(val, bmp->Pxf) ) {  // alpha
                            render->PlotPixel_f(put, val);
                        }
                        render->MoveNext_f(&put);
                        loop++;
                    }
                }
                line++;
            } //while (line <= rb)
        } //if (trans)
    } else {
        if (trans) {
            while (line <= rb) {
                render->MoveTo_f(render, &put, X, line);
                loop = X;
                while (loop < end) {
                    srcRender.GetPixel_f(get, &cnt);
                    srcRender.MoveNext_f(&get);
                    cnt = (val & 0x7F) + 1;

                    srcRender.GetPixel_f(get, &val);
                    srcRender.MoveNext_f(&get);
                    if (val == tColor) {  // the default transparency color
                        loop += cnt;
                        render->MoveNextFew_f(&put, cnt);
                    } else {
                        while (cnt--) {
                            if ((loop>=rl) && (loop<=rr) && AppLibBMP_CheckPlotPixelValid(val, bmp->Pxf)) {  // alpha
                                render->PlotPixel_f(put, val);
                            }
                            render->MoveNext_f(&put);
                            loop++;
                        }
                    }
                }
                line++;
            } //while (line <= rb)
        } else {
            while (line <= rb) {
                render->MoveTo_f(render, &put, X, line);
                loop = X;
                while (loop < end) {
                    srcRender.GetPixel_f(get, &cnt);
                    srcRender.MoveNext_f(&get);
                    cnt = (val & 0x7F) + 1;

                    srcRender.GetPixel_f(get, &val);
                    srcRender.MoveNext_f(&get);
                    while (cnt--) {
                        if ((loop>=rl) && (loop<=rr) && AppLibBMP_CheckPlotPixelValid(val, bmp->Pxf)) { // alpha
                            render->PlotPixel_f(put, val);
                        }
                        render->MoveNext_f(&put);
                        loop++;
                    }
                }
                line++;
            } //while (line <= rb)
        }
    }

    return 0;
}


/**
 * AppLibBMP_CalcArea
 * BMP type impl of CalcArea_f
 * @param[in] obj - The graphic object is going to operate
 * @return int
 */
int AppLibBMP_CalcArea(APPLIB_GRAPHIC_OBJ_s *obj)
{
    APPLIB_GRAPHIC_BMP_CNT_s *bmpCnt = NULL;
    APPLIB_GRAPHIC_BMP_s *bmp = NULL;

    if (obj == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "obj:0x%x invalid", obj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (obj->Content == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "obj->Content:0x%x invalid", obj->Content);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    /* Init */
	bmpCnt = (APPLIB_GRAPHIC_BMP_CNT_s *)obj->Content;
	bmp = bmpCnt->BmpDescPtr->BmpPtr;

    obj->DisplayBox.X = bmpCnt->Left;
    obj->DisplayBox.Y = bmpCnt->Bottom;
    if (bmpCnt->BmpDescPtr->Flags == BMP_STATUS_LOADED) {
        obj->DisplayBox.Width  = bmp->Width;
        obj->DisplayBox.Height = bmp->Height;
    } else if (bmpCnt->BmpDescPtr->Flags == BMP_STATUS_INIT) {
        obj->DisplayBox.Width  = 0;
        obj->DisplayBox.Height = 0;
    }
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "(%d, %d) w(%d, %d)", obj->DisplayBox.X, obj->DisplayBox.Y, bmp->Width, bmp->Height);
    return AMP_OK;
}

/**
 * AppLibBMP_Dump
 * BMP type impl of Dump_f
 * @param[in] obj - The graphic object is going to operate
 * @return int
 */
int AppLibBMP_Dump(APPLIB_GRAPHIC_OBJ_s *obj)
{
    APPLIB_GRAPHIC_BMP_CNT_s *bmpCnt;
    APPLIB_GRAPHIC_BMP_BIN_INFO_s *bmpInfo;
    APPLIB_GRAPHIC_BMP_BIN_DESC_s *bmpDescPtr;
    APPLIB_GRAPHIC_BMP_s *bmp;

    //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "AppLibBMP_Dump start");
    if (obj == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "obj:0x%x invalid", obj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (obj->Content == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "obj->Content:0x%x invalid", obj->Content);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[AppLibBMP_Dump] List[%d]", obj->ID);
    bmpCnt = (APPLIB_GRAPHIC_BMP_CNT_s *)obj->Content;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[AppLibBMP_Dump] BMP Left:%d Bottom:%d ResIdx:%d BMPIdx:%d BmpInfo:0x%x BmpDescPtr:0x%x",
                    bmpCnt->Left, bmpCnt->Bottom, bmpCnt->ResIdx, bmpCnt->BMPIdx, bmpCnt->BmpInfo, bmpCnt->BmpDescPtr);
    bmpInfo = bmpCnt->BmpInfo;
    bmpDescPtr = bmpCnt->BmpDescPtr;
    if (bmpDescPtr != NULL) {
        bmp = bmpDescPtr->BmpPtr;
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[AppLibBMP_Dump] bmpDescPtr Offset:0x%x Size:%d Flag:%d Count:%d BmpPtr:0x%x",
                    bmpDescPtr->Offset, bmpDescPtr->Size, bmpDescPtr->Flags, bmpDescPtr->Count, bmpDescPtr->BmpPtr);
    }
    return AMP_OK;
}


/**
 * AppLibBMP_Draw
 * BMP type impl of Draw_f
 * @param[in] obj - The graphic object is going to operate
 * @return int
 */
int AppLibBMP_Draw(APPLIB_GRAPHIC_RENDER_s *render,
                   AMP_AREA_s *drawArea,
                      APPLIB_GRAPHIC_OBJ_s *obj)
{
    int ReturnValue = AMP_OK;
    APPLIB_GRAPHIC_BMP_CNT_s *bmpCnt = NULL;
    APPLIB_GRAPHIC_BMP_BIN_DESC_s *bmpDescPtr = NULL;
    APPLIB_GRAPHIC_BMP_s *bmp = NULL;

    if ( (render == NULL) || (obj == NULL) || (obj->Content == NULL)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "[AppLibBMP_Draw] render: 0x%X, obj: 0x%X, obj->Content:0x%X invalid", render, obj, obj->Content);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    /* Init */
	bmpCnt = (APPLIB_GRAPHIC_BMP_CNT_s *)obj->Content;
    bmpDescPtr = bmpCnt->BmpDescPtr;
    //bmp = bmpDescPtr->BmpPtr; /* CONFIG_APP_ARD */

    if (bmpDescPtr == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "[AppLibBMP_Draw] obj->Content->BmpDescPtr:0x%x invalid", bmpDescPtr);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
	bmp = bmpDescPtr->BmpPtr; /* CONFIG_APP_ARD */

    if (bmp == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "[AppLibBMP_Draw] Flags %d, BmpInfo:0x%x, BmpPtr:0x%x invalid", bmpDescPtr->Flags, bmpCnt->BmpInfo, bmp);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    switch (bmpDescPtr->Flags) {
        case BMP_STATUS_LOADED:
            {
                AMP_AREA_s drawArea = {0};
                drawArea.X = bmpCnt->Left;
                drawArea.Y = bmpCnt->Bottom;
                drawArea.Width = bmp->Width;
                drawArea.Height = bmp->Height;
                AppLibBMP_RLSDecode(render, bmp, bmpCnt->Left, bmpCnt->Bottom, &drawArea);

                //update display area
                obj->LastDisplayBox.X = obj->DisplayBox.X + 1;
                obj->LastDisplayBox.Y = obj->DisplayBox.Y + 1;
                obj->LastDisplayBox.Width = obj->DisplayBox.Width;
                obj->LastDisplayBox.Height = obj->DisplayBox.Height;

                #if 0             // for resize feature
                AppLibBMP_RescaleBMP();
                #endif
            }
            break;
        case BMP_STATUS_INIT:
            {
                if (bmpCnt->BmpInfo == NULL) {
                    ReturnValue = AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
                } else {
                    APPLIB_GRAPHIC_BMP_BIN_INFO_s *bmpInfo = bmpCnt->BmpInfo;
                    if (AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, bmpInfo->BinFileName, (UINT8*)bmp, bmpDescPtr->Size, bmpDescPtr->Offset)){
                        AMP_AREA_s drawArea = {0};

                        bmp->Ptr = (UINT8 *)((UINT8 *)bmp + sizeof(APPLIB_GRAPHIC_BMP_s));
                        bmpDescPtr->Flags = BMP_STATUS_LOADED;
                        AppLibBMP_CalcArea(obj);

                        drawArea.X = obj->DisplayBox.X;
                        drawArea.Y = obj->DisplayBox.Y;
                        drawArea.Width = obj->DisplayBox.Width;
                        drawArea.Height = obj->DisplayBox.Height;
                        AppLibBMP_RLSDecode(render, bmp, bmpCnt->Left, bmpCnt->Bottom, (const AMP_AREA_s*)&drawArea);

                        //update display area
                        obj->LastDisplayBox.X = obj->DisplayBox.X;
                        obj->LastDisplayBox.Y = obj->DisplayBox.Y;
                        obj->LastDisplayBox.Width = obj->DisplayBox.Width;
                        obj->LastDisplayBox.Height = obj->DisplayBox.Height;
                    } else {
                        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "AmbaROM_LoadByName fail");
                        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "AmbaROM_LoadByName %d %s 0x%x 0x%x 0x%x",
                                      AMBA_ROM_SYS_DATA, bmpInfo->BinFileName, (UINT8*)bmpDescPtr->BmpPtr, bmpDescPtr->Size, bmpDescPtr->Offset);
                        ReturnValue = AMP_ERROR_GENERAL_ERROR;
                    }
                }
            }
            break;
        default:
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "obj->Content->bmpDescPtr->Flag:0x%x", bmpDescPtr->Flags);
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "obj->Content->BmpInfo:0x%x invalid", bmpCnt->BmpInfo);
            ReturnValue = AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
            break;
    }
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "AppLibBMP_Draw end");
    return ReturnValue;
}

int AppLibBMP_Update(const APPLIB_GRAPHIC_OBJ_s *obj,
                          AMP_AREA_s *resArea)
{
    return AMP_OK;
}

/**
 *  @brief Get the specific BMP size from ROM
 *
 *  Get the specific BMP size of the specific resoltuion in BIN from ROM
 *
 *  @param [in] fileName Indicated which xxx.bin generated by AmbaGUIGen
 *  @param [in] resIdx Indicated which resolution in BMP bin file is going to calculate.
 *  @param [in] bmpIdx Returned total BMP number in BMP bin file.
 *  @param [out] bmpSize Returned the specific BMP size of the specific resolution
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibBMP_GetOneBMPSize( const char *fileName,
                             UINT32 resIdx,
                             UINT32 bmpIdx,
                             UINT32 *bmpSize)
{
    APPLIB_GRAPHIC_BMP_BIN_HEADER_s Header = {0};
    UINT32 BMPHeaderSZ = sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s);
    UINT32 BMPDescSZ = sizeof(APPLIB_GRAPHIC_BMP_BIN_DESC_s);
    UINT32 Offset = BMPHeaderSZ + (resIdx * Header.BmpNum * BMPDescSZ) + (bmpIdx * BMPDescSZ);
    APPLIB_GRAPHIC_BMP_BIN_DESC_s Desc = {0};

    /* Bounding Check */
    if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, fileName) == 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "[APPLIB BMP][GetOneBMPSize] %s is not exist!", fileName);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    /* Get BMP BIN Header from ROM */
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&Header, sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s), 0);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[APPLIB BMP][GetOneBMPSize] total num: %d, total res:%d, total Sz:0x%x, filename %s",
                  Header.BmpNum, Header.ResNum, AmbaROM_GetSize(AMBA_ROM_SYS_DATA, fileName, 0), fileName);

    /* Error Check */
    if ( (resIdx >= Header.ResNum) || (bmpIdx >= Header.BmpNum) ) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "[APPLIB BMP][GetOneBMPSize] bmpIdx: %d, resIdx: %d is out of range!", bmpIdx, resIdx);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    /* Assign bufferSize in BMP BIN */
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[APPLIB BMP][GetOneBMPSize] Offset:%d = %d + (%d * %d * %d) + (%d * %d)",
                  Offset, BMPHeaderSZ, resIdx, Header.BmpNum,
                  BMPDescSZ, bmpIdx, BMPDescSZ);
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&Desc, BMPDescSZ, Offset);
    *bmpSize = Desc.Size;

    return AMP_OK;
}

/**
 *  @brief Get BMP information from ROM
 *
 *  Get the specific resoltuion BMP information in BIN from ROM
 *  This Buffer includes needed index/header of the BMP bin
 *  BMP buffer structure:
 *      APPLIB_GRAPHIC_BMP_BIN_INFO_t     BMP bin info
 *      APPLIB_GRAPHIC_BMP_BIN_DESC_s     BMP desc Array(Index)
 *      APPLIB_GRAPHIC_BMP_s              BMP data Array(BMPbuffer)
 *
 *  @param [in] fileName Indicated which xxx.bin generated by AmbaGUIGen
 *  @param [in] resIdx Indicated which resolution in BMP bin file is going to calculate.
 *  @param [out] bmpTotalNum Returned total BMP number in BMP bin file.
 *  @param [out] bufferSize Returned total BMP buffer size
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibBMP_GetTotalBMPSize( const char *fileName,
                               UINT32 resIdx,
                               UINT32 *bmpTotalNum,
                               UINT32 *bufferSize)
{
    APPLIB_GRAPHIC_BMP_BIN_HEADER_s Header = {0};
    UINT32 BMPHeaderSZ = sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s);
    UINT32 BMPInfoSZ = sizeof(APPLIB_GRAPHIC_BMP_BIN_INFO_s);
    UINT32 BMPDescSZ = sizeof(APPLIB_GRAPHIC_BMP_BIN_DESC_s);
    UINT32 Offset = 0;
    APPLIB_GRAPHIC_BMP_BIN_DESC_s FirstDesc = {0};
    APPLIB_GRAPHIC_BMP_BIN_DESC_s LastDesc = {0};

    /* Bounding Check */
    if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, fileName) == 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "[APPLIB BMP][GetTotalBMPSize] %s is not exist!", fileName);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    /* Get BMP BIN Header from ROM */
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&Header, sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s), 0);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[APPLIB BMP][GetTotalBMPSize] total num: %d, total res:%d, total Sz:0x%x, filename %s",
                  Header.BmpNum, Header.ResNum, AmbaROM_GetSize(AMBA_ROM_SYS_DATA, fileName, 0), fileName);

    /* Error Check */
    if (resIdx >= Header.ResNum) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "[APPLIB BMP][GetTotalBMPSize] ResIdx:%d is out of range!", resIdx);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    /* Assign BMP Total Num in BMP BIN */
    *bmpTotalNum = Header.BmpNum;

    /* Assign bufferSize in BMP BIN */
    // get first descriptor in BIN for the specific resoltuion
    Offset = BMPHeaderSZ + resIdx * Header.BmpNum * BMPDescSZ;
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&FirstDesc, BMPDescSZ, Offset);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[APPLIB BMP][GetTotalBMPSize] First Desc: BMPSize = 0x%X, offset = 0x%X", FirstDesc.Size, FirstDesc.Offset);

    // get last descriptor in BIN for the specific resoltuion
    Offset += (Header.BmpNum-1) * sizeof(APPLIB_GRAPHIC_BMP_BIN_DESC_s);
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&LastDesc, sizeof(APPLIB_GRAPHIC_BMP_BIN_DESC_s), Offset);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[APPLIB BMP][GetTotalBMPSize] Last Desc: BMPSize = 0x%X, offset = 0x%X", LastDesc.Size, LastDesc.Offset);

    *bufferSize = BMPInfoSZ + (Header.BmpNum * BMPDescSZ) + (LastDesc.Offset + LastDesc.Size - FirstDesc.Offset);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[APPLIB BMP][GetTotalBMPSize] bufferSize: 0x%x = 0x%x + (%d * 0x%x) + (0x%x + 0x%x - 0x%x)",
                 *bufferSize, BMPInfoSZ, Header.BmpNum,
                 BMPDescSZ, LastDesc.Offset, LastDesc.Size, FirstDesc.Offset);

    return AMP_OK;
}

/**
 *  @brief Get BMP all information from ROM
 *
 *  Get the all information in BIN from ROM
 *
 *  @param [in] fileName xxx.bin generated by AmbaGUIGen
 *  @param [out] resNum total resolution numbers in BMP bin file
 *  @param [out] bmpNum total bmp numbers in BMP bin file
 *  @param [out] bufferSize the whole bmp data size in BMP bin file
 *
 *  @return
 */
int AppLibBMP_GetAllBMPBINInfo( const char *fileName,
                                UINT32 *resNum,
                                UINT32 *bmpNum,
                                UINT32 *bufferSize)
{
    UINT32 FileSize = 0;
    APPLIB_GRAPHIC_BMP_BIN_HEADER_s Header = {0};

    /* Bounding Check */
    if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, fileName) == 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "[APPLIB BMP][GetAllBMPBINInfo] %s is not exist!", fileName);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    /* Get BMP BIN total size in ROM */
    FileSize = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, fileName, 0x0);

    /* Get BMP BIN Header from ROM */
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&Header, sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s), 0);

    /* Assign total resolution in BMP BIN */
    *resNum = Header.ResNum;

    /* Assign total BMP number in BMP BIN */
    *bmpNum = Header.BmpNum;

    /* Assign total data size in BMP BIN */
    *bufferSize = FileSize - sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s) + sizeof(APPLIB_GRAPHIC_BMP_BIN_INFO_s);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "ResNum:%d BmpNum:%d BufferSize:0x%x = 0x%x - 0x%x + 0x%x", *resNum, *bmpNum,
                 *bufferSize, FileSize, sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s), sizeof(APPLIB_GRAPHIC_BMP_BIN_INFO_s));

    return AMP_OK;
}

/**
 *  @brief Get one BMP
 *
 *  Load a BMP data from BMP BIN
 *
 *  @param [in] fileName Indicated which xxx.bin generated by AmbaGUIGen
 *  @param [in] resIdx the specific resolution in BMP BIN
 *  @param [in] bmpIdx the specific BMP index in BMP BIN
 *  @param [out] BMPBuf the wanted BMP buffer
 *  @param [out] bmp the wanted BMP data
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibBMP_LoadBMP( const char *fileName,
                       UINT32 resIdx,
                       UINT32 bmpIdx,
                       void *bmpBuf,
                       APPLIB_GRAPHIC_BMP_s **bmp)
{
    APPLIB_GRAPHIC_BMP_BIN_HEADER_s Header = {0};
    UINT32 BMPHeaderSZ = sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s);
    UINT32 BMPDescSZ = sizeof(APPLIB_GRAPHIC_BMP_BIN_DESC_s);
    UINT32 Offset = BMPHeaderSZ + (resIdx * Header.BmpNum * BMPDescSZ) + (bmpIdx * BMPDescSZ);
    APPLIB_GRAPHIC_BMP_BIN_DESC_s Desc = {0};
    APPLIB_GRAPHIC_BMP_s *bmpPtr = NULL;

    /* Bounding Check */
    if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, fileName) == 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "[APPLIB BMP][LoadBMP] %s is not exist!", fileName);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if ( (bmpBuf == NULL) || (bmp == NULL) ) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "[APPLIB BMP][LoadBMP] BMPBuf:0x%x, bmp:0x%X invalid.", bmpBuf, bmp);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    /* Header Getton */
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&Header, sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s), 0);

    /* Error Check */
    if ( (bmpIdx >= Header.BmpNum) || (resIdx >= Header.ResNum)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "[APPLIB BMP][LoadBMP] BMPIdx:%d ResIdx:%d error", bmpIdx, resIdx);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[APPLIB BMP][LoadBMP] Offset:%d = %d + (%d * %d * %d) + (%d * %d)",
                  Offset, BMPHeaderSZ, resIdx, Header.BmpNum,
                  BMPDescSZ, bmpIdx, BMPDescSZ);

    /* BMP Data Getton */
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&Desc, BMPDescSZ, Offset);
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)bmpBuf, Desc.Size, Desc.Offset);

    bmpPtr = (APPLIB_GRAPHIC_BMP_s *)bmpBuf;
    bmpPtr->Ptr = (UINT8 *)((UINT8 *)bmpBuf + sizeof(APPLIB_GRAPHIC_BMP_s));
    *bmp = bmpPtr;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[APPLIB BMP][LoadBMP] BMP:0x%x flg:0x%x bits:0x%x width:%d height:%d pxf:%d res:0x%x tcolor:0x%x Ptr:0x%x",
                  bmpPtr, bmpPtr->Flags, bmpPtr->Bits, bmpPtr->Width, bmpPtr->Height,
                  bmpPtr->Pxf, bmpPtr->Reserve, bmpPtr->TColor, bmpPtr->Ptr);

    return AMP_OK;
}

/**
 *  @brief Initialize one resolution of BMPs in BMP.bin.
 *
 *  Construct indexes in Buf, the index includes APPLIB_GRAPHIC_BMP_BIN_HEADER_s & APPLIB_GRAPHIC_BMP_BIN_INFO_t
 *
 *  @param [in] fileName Indicated which xxx.bin generated by AmbaGUIGen
 *  @param [in] resIdx the specific resolution in BMP BIN
 *  @param [in] bmpBuf the BMP buffer
 *  @param [in] loadBMP load the whole BMP from BIN or not
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibBMP_InitBMPBuffer( const char *fileName,
                             UINT32 resIdx,
                             void *bmpBuf,
                             UINT8 loadBMP)
{
    APPLIB_GRAPHIC_BMP_BIN_HEADER_s Header = {0};
    UINT32 BMPHeaderSZ = sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s);
    UINT32 BMPInfoSZ = sizeof(APPLIB_GRAPHIC_BMP_BIN_INFO_s);
    UINT32 BMPDescSZ = sizeof(APPLIB_GRAPHIC_BMP_BIN_DESC_s);
    APPLIB_GRAPHIC_BMP_BIN_INFO_s *BInfo = NULL;
    APPLIB_GRAPHIC_BMP_BIN_DESC_s *Desc = NULL;
    UINT32 Bidx = 0;

    /* Bounding Check */
    if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, fileName) == 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "[APPLIB BMP][InitBMP] %s is not exist!", fileName);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (bmpBuf == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "[APPLIB BMP][InitBMP] BMPBuf:0x%x, bmp:0x%X invalid.", bmpBuf);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    /* Header Getton */
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&Header, sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s), 0);

    /* Error Check */
    if (resIdx >= Header.ResNum) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "[APPLIB BMP][LoadBMP] ResIdx:%d error", resIdx);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    /* Buffer Initialization */
    BInfo = (APPLIB_GRAPHIC_BMP_BIN_INFO_s *)bmpBuf;
    strncpy(BInfo->BinFileName, fileName, strlen(fileName));
    BInfo->ResIdx = resIdx;
    BInfo->BmpNum = Header.BmpNum;
    BInfo->DescTab = (APPLIB_GRAPHIC_BMP_BIN_DESC_s *)((UINT32)bmpBuf + BMPInfoSZ);
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)BInfo->DescTab,
                       Header.BmpNum*BMPDescSZ,
                       BMPHeaderSZ + resIdx*Header.BmpNum*BMPDescSZ);

    /* BMP Load */
    if (loadBMP == 1) {
        UINT32 srcSize = 0;
        APPLIB_GRAPHIC_BMP_BIN_DESC_s FirstDesc, LastDesc;
        UINT8 *dstPtr = (UINT8 *)((UINT32)BInfo->DescTab + Header.BmpNum * BMPDescSZ);
        UINT32 offset = BMPHeaderSZ + resIdx * Header.BmpNum * BMPDescSZ;
        AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&FirstDesc, BMPDescSZ, offset);

        offset += (Header.BmpNum - 1)*BMPDescSZ;
        AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&LastDesc, BMPDescSZ, offset);

        srcSize = LastDesc.Offset + LastDesc.Size - FirstDesc.Offset;
        AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, dstPtr, srcSize, FirstDesc.Offset);
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[APPLIB BMP][LoadBMP] F BMPSize:0x%x BMPoff:0x%x", FirstDesc.Size, FirstDesc.Offset);
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[APPLIB BMP][LoadBMP] L BMPSize:0x%x BMPoff:0x%x", LastDesc.Size, LastDesc.Offset);
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[APPLIB BMP][LoadBMP] dstPtr:0x%x srcSize:0x%x srcOff:0x%x", dstPtr, srcSize, FirstDesc.Offset);
    }

    /* BMP Initialization */
    Desc = &BInfo->DescTab[0];
    Desc->Count = 1;
    Desc->Flags = BMP_STATUS_LOADED;
    Desc->BmpPtr = (APPLIB_GRAPHIC_BMP_s *)((UINT32)BInfo->DescTab + Header.BmpNum*BMPDescSZ);
    Desc->BmpPtr->Ptr = (UINT8*)((UINT8*)Desc->BmpPtr + sizeof(APPLIB_GRAPHIC_BMP_s));
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[APPLIB BMP][LoadBMP] Bidx:%3d Desc:0x%x off:0x%x size:0x%x BPtr:0x%x Ptr:0x%x",
                  0, Desc, Desc->Offset, Desc->Size, Desc->BmpPtr, Desc->BmpPtr->Ptr);

    for (Bidx=1; Bidx<Header.BmpNum; Bidx++) {
        APPLIB_GRAPHIC_BMP_BIN_DESC_s *pDesc = &BInfo->DescTab[Bidx-1];
        Desc = &BInfo->DescTab[Bidx];
        Desc->Count = 1;
        Desc->Flags = BMP_STATUS_LOADED;
        Desc->BmpPtr = (APPLIB_GRAPHIC_BMP_s *)((UINT8*)(pDesc->BmpPtr) + pDesc->Size);
        Desc->BmpPtr->Ptr = (UINT8*)((UINT8*)Desc->BmpPtr + sizeof(APPLIB_GRAPHIC_BMP_s));
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[APPLIB BMP][LoadBMP] Bidx:%3d Desc:0x%x off:0x%x size:0x%x BPtr:0x%x W:%d H:%d Ptr:0x%x",
                      Bidx, Desc, Desc->Offset, Desc->Size, Desc->BmpPtr,
                      Desc->BmpPtr->Width, Desc->BmpPtr->Height, Desc->BmpPtr->Ptr);
    }
    return AMP_OK;
}

/**
 * AppLibBMP_CreateAllObjFromROMFS
 * Load all BMPs file from .bin in ROMFS,
 * Use descUIObj to filed dstBmpObj struct
 * the BMP content will NOT be loaded until drawing.
 * Construct indexes in Buf, the index includes APPLIB_GRAPHIC_BMP_BIN_HEADER_s & APPLIB_GRAPHIC_BMP_BIN_INFO_t
 * @param[in] fileName - .bin file name generated by AmbaGUIGen.
 * @param[in] ResIdx - Specify which resolution to create.
 * @param[in] Buf - An allocated buffer for loading BMPs, and build BMP index,
 *                  including: APPLIB_GRAPHIC_BMP_BIN_HEADER_s & APPLIB_GRAPHIC_BMP_BIN_INFO_t
 * @param[in] descUIObj - Reference UI obj array, contains BMP description.
 * @param[out] dstBmpObj - Allocated destination Graphic object array.
 * @return int
 */
int AppLibBMP_CreateResObjFromROMFS(const char *fileName,
                                       UINT32 ResIdx,
                                       void *BMPBuf,
                                       APPLIB_GRAPHIC_UIOBJ_s *descUIObj,
                                       APPLIB_GRAPHIC_OBJ_s *dstBmpObjArr)
{
    APPLIB_GRAPHIC_BMP_BIN_HEADER_s Header;
    APPLIB_GRAPHIC_BMP_BIN_INFO_s *BInfo;
    UINT32 Ridx, Bidx;
    APPLIB_GRAPHIC_BMP_BIN_DESC_s *Desc;

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "[AppLibBMP] fileName:%s", fileName);
    if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, fileName) == 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "AppLibBMP_CreateResObjFromROMFS %s is not exist.", fileName);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (BMPBuf == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "AppLibBMP_CreateResObjFromROMFS BMPBuf:0x%x invalid.", BMPBuf);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (dstBmpObjArr == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "AppLibBMP_CreateResObjFromROMFS dstBmpObjArr:0x%x invalid.", dstBmpObjArr);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&Header, sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s), 0);
    //Init BMPBuf,
    BInfo = (APPLIB_GRAPHIC_BMP_BIN_INFO_s *)BMPBuf;
    strncpy(BInfo->BinFileName, fileName, strlen(fileName));
    BInfo->ResIdx = ResIdx;
    BInfo->BmpNum = Header.BmpNum;
    //BInfo->DescNum = Header.ResNum*Header.BmpNum;
    //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "BInfo->DescNum:%d = %d * %d", BInfo->DescNum, Header.ResNum, Header.BmpNum);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "ResNum:%d BMPNum:%d", Header.ResNum, Header.BmpNum);
    //Init BMPBuf, descArr
    BInfo->DescTab = (APPLIB_GRAPHIC_BMP_BIN_DESC_s *)((UINT32)BMPBuf + sizeof(APPLIB_GRAPHIC_BMP_BIN_INFO_s));
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "BInfo->DescTab:0x%x", BInfo->DescTab);
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA,
                       fileName,
                       (UINT8*)BInfo->DescTab,
                       //BInfo->DescNum*sizeof(APPLIB_GRAPHIC_BMP_BIN_DESC_s),
                       1*Header.BmpNum*sizeof(APPLIB_GRAPHIC_BMP_BIN_DESC_s),
                       sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s));
    //Init BMPBuf, descArr content
    for (Ridx=0; Ridx<Header.ResNum; Ridx++) {
        for (Bidx=0; Bidx<Header.BmpNum; Bidx++) {
            //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "Bidx:%d", Bidx);
            Desc = &BInfo->DescTab[Ridx*Header.BmpNum+Bidx];
            //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "Desc:0x%x", Desc);
            Desc->Count = 0;
            Desc->Flags = BMP_STATUS_INIT;
            //BMPPtr = BMPBuf +
            //         sizeof(APPLIB_GRAPHIC_BMP_BIN_INFO_t) +
            //         DescTab[BInfo->DescNum] +
            //         BMPOffset
            //BMPOffset = BINFile offset -
            //         sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s) -
            //         sizeof(DescTab[BInfo->DescNum]) -
            Desc->BmpPtr = (APPLIB_GRAPHIC_BMP_s *)((UINT8*)BInfo->DescTab +
                               Desc->Offset -
                               sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s));
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "Bidx:%3d BmpBinOff:0x%8x size:0x%8x BmpPtr:0x%8x", Bidx, Desc->Offset, Desc->Size, Desc->BmpPtr);
            descUIObj[Bidx].Cnt.Bmp.BmpInfo = BInfo;
            descUIObj[Bidx].Cnt.Bmp.BmpDescPtr = Desc;
            dstBmpObjArr[Bidx].DisplayBox.X = descUIObj[Bidx].UIObjDisplayBox.X;
            dstBmpObjArr[Bidx].DisplayBox.Y = descUIObj[Bidx].UIObjDisplayBox.Y;
            dstBmpObjArr[Bidx].DisplayBox.Width = descUIObj[Bidx].UIObjDisplayBox.Width;      //pass original setting into new obj
            dstBmpObjArr[Bidx].DisplayBox.Height = descUIObj[Bidx].UIObjDisplayBox.Height;    //canvas will altered then
            dstBmpObjArr[Bidx].LastDisplayBox.X = descUIObj[Bidx].UIObjDisplayBox.X;
            dstBmpObjArr[Bidx].LastDisplayBox.Y = descUIObj[Bidx].UIObjDisplayBox.Y;
            dstBmpObjArr[Bidx].LastDisplayBox.Width = descUIObj[Bidx].UIObjDisplayBox.Width;      //pass original setting into new obj
            dstBmpObjArr[Bidx].LastDisplayBox.Height = descUIObj[Bidx].UIObjDisplayBox.Height;    //canvas will altered then

            dstBmpObjArr[Bidx].AlphaTable = descUIObj[Bidx].AlphaTable;
            dstBmpObjArr[Bidx].Layer = descUIObj[Bidx].Layer;
            dstBmpObjArr[Bidx].Group = descUIObj[Bidx].Group;
            dstBmpObjArr[Bidx].Show = descUIObj[Bidx].DefaultShow;
            dstBmpObjArr[Bidx].Stat = OBJ_STAT_NORMAL;
            dstBmpObjArr[Bidx].Content = (void *)&descUIObj[Bidx].Cnt.Bmp;
            dstBmpObjArr[Bidx].CalcArea_f = AppLibBMP_CalcArea;
            dstBmpObjArr[Bidx].Dump_f = AppLibBMP_Dump;
            dstBmpObjArr[Bidx].Draw_f = AppLibBMP_Draw;

            //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "%d BmpInfo:0x%x BmpDescPtr:0x%x off:0x%x size:0x%x BPtr:0x%x",
            //        Bidx, BInfo, Desc, Desc->Offset, Desc->Size, Desc->BmpPtr);
        }
    }
/*            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "AppLibBMP_InitALLFromROMFS init done");
    //load all
    dstPtr = (UINT8 *)((UINT32)BInfo->DescTab + BInfo->DescNum*sizeof(APPLIB_GRAPHIC_BMP_BIN_DESC_s));
    srcOff = sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s) + BInfo->DescNum*sizeof(APPLIB_GRAPHIC_BMP_BIN_DESC_s);
    srcSize = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, fileName, 0x0) - srcOff;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "dstPtr:0x%x srcSize:0x%x srcOff:0x%x",
            dstPtr, srcSize, srcOff);
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, dstPtr, srcSize, srcOff);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "AppLibBMP_InitALLFromROMFS copy done");
    for (Bidx=0; Bidx<Header.BmpNum; Bidx++) {
        Desc = &BInfo->DescTab[InitResIdx*Header.BmpNum+Bidx];
        Desc->Count += 1;
        Desc->Flags = APPLIB_GRAPHIC_BMP_BIN_INFO_LOAD;
        Desc->BmpPtr->Ptr = (UINT8*)((UINT32)Desc->BmpPtr + sizeof(APPLIB_GRAPHIC_BMP_s));
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "L %d BmpInfo:0x%x BmpDescPtr:0x%x off:0x%x size:0x%x BPtr:0x%x",
                Bidx, BInfo, Desc, Desc->Offset, Desc->Size, Desc->BmpPtr);
    }

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "AppLibBMP_InitALLFromROMFS load done");
*/
    return AMP_OK;
}

/**
 * AppLibBMP_CreateObjFromROMFS
 * Load a BMP file from .bin in ROMFS into BMPBuf,
 * Use descUIObj to filed dstBmpObj struct
 * @param[in] fileName - .bin file name generated by AmbaGUIGen.
 * @param[in] descUIObj - Reference UI obj, contains BMP description.
 * @param[in] BMPBuf - An allocated buffer for loading BMPs.
 * @param[out] dstBmpObj - Destination Graphic object.
 * @return int
 */
int AppLibBMP_CreateObjFromROMFS(const char *fileName,
                                    APPLIB_GRAPHIC_UIOBJ_s *descUIObj,
                                    APPLIB_GRAPHIC_BMP_s *BMPBuf,
                                    APPLIB_GRAPHIC_OBJ_s *dstBmpObj)
{
    UINT32 BMPIdx, ResIdx, Offset;
    APPLIB_GRAPHIC_BMP_BIN_HEADER_s Header;

    if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, fileName) == 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "AppLibBMP_CreateObjFromROMFS %s is not exist.", fileName);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (BMPBuf == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "AppLibBMP_CreateObjFromROMFS BMPBuf:0x%x invalid.", BMPBuf);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (dstBmpObj == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "AppLibBMP_CreateObjFromROMFS dstBmpObj:0x%x invalid.", dstBmpObj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (descUIObj == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "AppLibBMP_CreateObjFromROMFS descUIObj:0x%x invalid.", descUIObj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (descUIObj->Type != APPLIB_GRAPHIC_UIOBJ_BMP) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "AppLibBMP_CreateObjFromROMFS descUIObj->Type:0x%x error.", descUIObj->Type);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (descUIObj->Cnt.Bmp.BmpDescPtr == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "AppLibBMP_CreateObjFromROMFS descUIObj->Cnt.Bmp.BmpDescPtr:0x%x invalid.", descUIObj->Cnt.Bmp.BmpDescPtr);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "fileName:%s ResIdx:%d BMPIdx:%d", fileName, descUIObj->Cnt.Bmp.ResIdx, descUIObj->Cnt.Bmp.BMPIdx);
    ResIdx = descUIObj->Cnt.Bmp.ResIdx;
    BMPIdx = descUIObj->Cnt.Bmp.BMPIdx;
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&Header, sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s), 0);
    if ((BMPIdx < Header.BmpNum) && (ResIdx < Header.ResNum))  {
        Offset = sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s)
                 + ResIdx*Header.BmpNum*sizeof(APPLIB_GRAPHIC_BMP_BIN_DESC_s)
                 + BMPIdx*sizeof(APPLIB_GRAPHIC_BMP_BIN_DESC_s);
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "Offset:%d = %d + %d*%d*%d + %d*%d",
                      Offset, sizeof(APPLIB_GRAPHIC_BMP_BIN_HEADER_s), ResIdx, Header.BmpNum,
                      sizeof(APPLIB_GRAPHIC_BMP_BIN_DESC_s), BMPIdx, sizeof(APPLIB_GRAPHIC_BMP_BIN_DESC_s));
        AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)descUIObj->Cnt.Bmp.BmpDescPtr, sizeof(APPLIB_GRAPHIC_BMP_BIN_DESC_s), Offset);
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "BMPSize:0x%x BMPoff:0x%x", descUIObj->Cnt.Bmp.BmpDescPtr->Size, descUIObj->Cnt.Bmp.BmpDescPtr->Offset);
        Offset = descUIObj->Cnt.Bmp.BmpDescPtr->Offset;
        AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)BMPBuf, descUIObj->Cnt.Bmp.BmpDescPtr->Size, Offset);
        descUIObj->Cnt.Bmp.BmpDescPtr->Flags = BMP_STATUS_LOADED;
        descUIObj->Cnt.Bmp.BmpDescPtr->Count = 0;
        descUIObj->Cnt.Bmp.BmpDescPtr->BmpPtr = BMPBuf;
        descUIObj->Cnt.Bmp.BmpDescPtr->BmpPtr->Ptr = (UINT8 *)((UINT8*)BMPBuf + sizeof(APPLIB_GRAPHIC_BMP_s));
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "BMP:0x%x flg:0x%x bits:0x%x width:%d height:%d pxf:%d res:0x%x tcolor:0x%x Ptr:0x%x",
                descUIObj->Cnt.Bmp.BmpDescPtr->BmpPtr,
                descUIObj->Cnt.Bmp.BmpDescPtr->BmpPtr->Flags,
                descUIObj->Cnt.Bmp.BmpDescPtr->BmpPtr->Bits,
                descUIObj->Cnt.Bmp.BmpDescPtr->BmpPtr->Width,
                descUIObj->Cnt.Bmp.BmpDescPtr->BmpPtr->Height,
                descUIObj->Cnt.Bmp.BmpDescPtr->BmpPtr->Pxf,
                descUIObj->Cnt.Bmp.BmpDescPtr->BmpPtr->Reserve,
                descUIObj->Cnt.Bmp.BmpDescPtr->BmpPtr->TColor,
                descUIObj->Cnt.Bmp.BmpDescPtr->BmpPtr->Ptr);
        dstBmpObj->DisplayBox.X = descUIObj->UIObjDisplayBox.X;
        dstBmpObj->DisplayBox.Y = descUIObj->UIObjDisplayBox.Y;
        dstBmpObj->DisplayBox.Width = descUIObj->UIObjDisplayBox.Width;      //pass original setting into new obj
        dstBmpObj->DisplayBox.Height = descUIObj->UIObjDisplayBox.Height;    //canvas will altered then
        dstBmpObj->LastDisplayBox.X = descUIObj->UIObjDisplayBox.X;
        dstBmpObj->LastDisplayBox.Y = descUIObj->UIObjDisplayBox.Y;
        dstBmpObj->LastDisplayBox.Width = descUIObj->UIObjDisplayBox.Width;      //pass original setting into new obj
        dstBmpObj->LastDisplayBox.Height = descUIObj->UIObjDisplayBox.Height;    //canvas will altered then
        dstBmpObj->AlphaTable = descUIObj->AlphaTable;
        dstBmpObj->Layer = descUIObj->Layer;
        dstBmpObj->Group = descUIObj->Group;
        dstBmpObj->Show = descUIObj->DefaultShow;
        dstBmpObj->Stat = OBJ_STAT_NORMAL;
        dstBmpObj->Content = (void *)&descUIObj->Cnt.Bmp;
        dstBmpObj->CalcArea_f = AppLibBMP_CalcArea;
        dstBmpObj->Dump_f = AppLibBMP_Dump;
        dstBmpObj->Draw_f = AppLibBMP_Draw;
        //dstBmpObj->Update_f = AppLibBMP_Update;
        return AMP_OK;
    } else {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "BMPIdx:%d ResIdx:%d error", BMPIdx, ResIdx);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
}

/**
 *  @brief Creator entry function of BMP
 *
 *  The transform function between BMP UI object struct and BMP Object struct
 *
 *  @param [in] *descUIObj The configures of the BMP UI object
 *  @param [out] *dstObj The configures of the object
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibBMP_CreateObj(APPLIB_GRAPHIC_UIOBJ_s *descUIObj,
                           APPLIB_GRAPHIC_OBJ_s *dstBmpObj)
{
    APPLIB_GRAPHIC_BMP_CNT_s *bmpCnt = NULL;
    APPLIB_GRAPHIC_BMP_BIN_INFO_s *BmpInfo = NULL;

    /* Bounding Check */
    if ( (!descUIObj) || (!dstBmpObj)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "[AppLib]AppLibBMP_CreateObj descUIObj:0x%X, dstBmpObj:0x%X invalid.", descUIObj, dstBmpObj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (descUIObj->Type != APPLIB_GRAPHIC_UIOBJ_BMP) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "AppLibBMP_CreateObj descUIObj->Type:0x%x error.", descUIObj->Type);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    /* a single bmp */
    bmpCnt = &(descUIObj->Cnt.Bmp);

    /* Error Check */
    if ((bmpCnt->ResIdx == 0xFFFFFFFF) && (bmpCnt->BMPIdx == 0xFFFFFFFF) && (bmpCnt->BmpDescPtr != 0x0)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "AppLibBMP_CreateObj Not support create from single APPLIB_GRAPHIC_STR_BIN_DESC_s yet.");
        return AMP_ERROR_GENERAL_ERROR;
    }
    if (!bmpCnt->BmpInfo->DescTab) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "AppLibBMP_CreateObj Error, Left:%d Bottom:%d ResIdx:%d BMPIdx:%d BmpInfo:0x%x BmpDescPtr:0x%x",
                bmpCnt->Left, bmpCnt->Bottom, bmpCnt->BmpInfo, bmpCnt->BmpDescPtr);
        if (bmpCnt->BmpInfo != 0x0) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "                         , BmpInfo->BinFileName:%s ResIdx:%d BmpNum:%d DescTab:0x%x",
                    bmpCnt->BmpInfo->BinFileName, bmpCnt->BmpInfo->ResIdx,
                    bmpCnt->BmpInfo->BmpNum, bmpCnt->BmpInfo->DescTab);
        }
        if (bmpCnt->BmpDescPtr != 0x0) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "                         , BmpDescPtr->Offset:0x%x Size:0x%x Flags:0x%x Count:%d BmpPtr:0x%x",
                    bmpCnt->BmpDescPtr->Offset, bmpCnt->BmpDescPtr->Size,
                    bmpCnt->BmpDescPtr->Flags, bmpCnt->BmpDescPtr->Count, bmpCnt->BmpDescPtr->BmpPtr);
        }
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    BmpInfo = descUIObj->Cnt.Bmp.BmpInfo;
    if ((bmpCnt->BMPIdx < BmpInfo->BmpNum) && (bmpCnt->ResIdx == BmpInfo->ResIdx))  {
        //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_BITMAP, "BMPSize:0x%x BMPoff:0x%x", descUIObj->Cnt.Bmp.BmpDescPtr->Size, descUIObj->Cnt.Bmp.BmpDescPtr->Offset);
        dstBmpObj->DisplayBox.X = descUIObj->UIObjDisplayBox.X;
        dstBmpObj->DisplayBox.Y = descUIObj->UIObjDisplayBox.Y;
        dstBmpObj->DisplayBox.Width = descUIObj->UIObjDisplayBox.Width;      //pass original setting into new obj
        dstBmpObj->DisplayBox.Height = descUIObj->UIObjDisplayBox.Height;    //canvas will altered then
        dstBmpObj->LastDisplayBox.X = descUIObj->UIObjDisplayBox.X;
        dstBmpObj->LastDisplayBox.Y = descUIObj->UIObjDisplayBox.Y;
        dstBmpObj->LastDisplayBox.Width = descUIObj->UIObjDisplayBox.Width;      //pass original setting into new obj
        dstBmpObj->LastDisplayBox.Height = descUIObj->UIObjDisplayBox.Height;    //canvas will altered then
        dstBmpObj->AlphaTable = descUIObj->AlphaTable;
        dstBmpObj->Layer = descUIObj->Layer;
        dstBmpObj->Group = descUIObj->Group;
        dstBmpObj->Show = descUIObj->DefaultShow;
        dstBmpObj->Stat = OBJ_STAT_NORMAL;
        dstBmpObj->Content = (void *)bmpCnt;
        dstBmpObj->CalcArea_f = AppLibBMP_CalcArea;
        dstBmpObj->Dump_f = AppLibBMP_Dump;
        dstBmpObj->Draw_f = AppLibBMP_Draw;
        //dstBmpObj->Update_f = AppLibBMP_Update;
        return AMP_OK;
    } else {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_BITMAP, "BMPIdx:%d, BmpNum: %d, ResIdx:%d error", bmpCnt->BMPIdx, BmpInfo->BmpNum, bmpCnt->ResIdx);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
}

