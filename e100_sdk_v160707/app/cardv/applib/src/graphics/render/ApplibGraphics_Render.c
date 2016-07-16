/**
 * @file src/app/connected/applib/src/graphics/ApplibGraphics_Render.c
 *
 * Implementation of render module in application Library.
 *
 * History:
 *    2014/01/15 - [Eric Yen] created file
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
#include "Render.h"
#include <graphics/render/ApplibGraphics_Render.h>

//
// speedy move cursor impl
//
static void speedyMoveTo_u8(const APPLIB_GRAPHIC_RENDER_s *render, void **dst, UINT32 x, UINT32 y)
{
    (*dst) = (void*)(render->RowStartAddr[y] + x);
}

static void speedyMoveTo_u16(const APPLIB_GRAPHIC_RENDER_s *render, void **dst, UINT32 x, UINT32 y)
{
    (*dst) = (void*)(render->RowStartAddr[y] + (x<<1));
}

static void speedyMoveTo_u24(const APPLIB_GRAPHIC_RENDER_s *render, void **dst, UINT32 x, UINT32 y)
{
    (*dst) = (void*)(render->RowStartAddr[y] + ((x<<1) + x));
}

static void speedyMoveTo_u32(const APPLIB_GRAPHIC_RENDER_s *render, void **dst, UINT32 x, UINT32 y)
{
    (*dst) = (void*)(render->RowStartAddr[y] + (x<<2));
}

//
// move cursor impl
//
static void moveTo_u8(const APPLIB_GRAPHIC_RENDER_s *render, void **dst, UINT32 x, UINT32 y)
{
    (*dst) = (void*)(((UINT8*)((UINT32)render->Buf + y*render->BufPitch)) + x);
}

static void moveTo_u16(const APPLIB_GRAPHIC_RENDER_s *render, void **dst, UINT32 x, UINT32 y)
{
    (*dst) = (void*)(((UINT16*)((UINT32)render->Buf + y*render->BufPitch)) + x);
}

static void moveTo_u24(const APPLIB_GRAPHIC_RENDER_s *render, void **dst, UINT32 x, UINT32 y)
{
    (*dst) = (void*)(((UINT8*)((UINT32)render->Buf + y*render->BufPitch)) + 3*x);
}

static void moveTo_u32(const APPLIB_GRAPHIC_RENDER_s *render, void **dst, UINT32 x, UINT32 y)
{
    (*dst) = (void*)(((UINT32*)((UINT32)render->Buf + y*render->BufPitch)) + x);
}

//
// move cursor to next few pixel impl
//
static void moveNextFew_u8(void** pen, UINT32 l)
{
    (*(UINT8**)pen) += l;
}

static void moveNextFew_u16(void** pen, UINT32 l)
{
    (*(UINT8**)pen) += (l<<1);
}

static void moveNextFew_u24(void** pen, UINT32 l)
{
    (*(UINT8**)pen) += ((l<<1)+l);
}

static void moveNextFew_u32(void** pen, UINT32 l)
{
    (*(UINT8**)pen) += (l<<2);;
}

//
// move cursor to next pixel impl
//
static void moveNext_u8(void** pen)
{
    (*(UINT8**)pen) ++;
}

static void moveNext_u16(void** pen)
{
    (*(UINT8**)pen) += 2;
}

static void moveNext_u24(void** pen)
{
    (*(UINT8**)pen) += 3;
}

static void moveNext_u32(void** pen)
{
    (*(UINT8**)pen) += 4;
}

//
// plot pixel impl
//

static void plotPixel_u8(void *dst, UINT32 color)
{
    *((UINT8*)dst) = color;
}

static void plotPixel_u16(void *dst, UINT32 color)
{
    *((UINT16*)dst) = color;
}

static void plotPixel_u24(void *dst, UINT32 color)
{
    //*((UINT16*)dst) = color;
}

static void plotPixel_u32(void *dst, UINT32 color)
{
    *((UINT32*)dst) = color;
}

//
// plot pixel at (x,y) impl
//
static void speedyPlotPixelAt_u8(const APPLIB_GRAPHIC_RENDER_s *render, UINT32 x, UINT32 y, UINT32 color)
{
    *(((UINT8*)(render->RowStartAddr[y])) + x) = color;
}

static void speedyPlotPixelAt_u16(const APPLIB_GRAPHIC_RENDER_s *render, UINT32 x, UINT32 y, UINT32 color)
{
    *(((UINT16*)(render->RowStartAddr[y])) + x) = color;
}

static void speedyPlotPixelAt_u24(const APPLIB_GRAPHIC_RENDER_s *render, UINT32 x, UINT32 y, UINT32 color)
{
    //*(((UINT16*)(render->RowStartAddr[y])) + x) = color;
}

static void speedyPlotPixelAt_u32(const APPLIB_GRAPHIC_RENDER_s *render, UINT32 x, UINT32 y, UINT32 color)
{
    *(((UINT32*)(render->RowStartAddr[y])) + x) = color;
}

static void plotPixelAt_u8(const APPLIB_GRAPHIC_RENDER_s *render, UINT32 x, UINT32 y, UINT32 color)
{
    *(((UINT8*)((UINT32)render->Buf + y*render->BufPitch)) + x) = color;
}

static void plotPixelAt_u16(const APPLIB_GRAPHIC_RENDER_s *render, UINT32 x, UINT32 y, UINT32 color)
{
    *(((UINT16*)((UINT32)render->Buf + y*render->BufPitch)) + x) = color;
}

static void plotPixelAt_u24(const APPLIB_GRAPHIC_RENDER_s *render, UINT32 x, UINT32 y, UINT32 color)
{
    //*(((UINT16*)((UINT32)render->Buf + y*render->BufPitch)) + x) = color;
}

static void plotPixelAt_u32(const APPLIB_GRAPHIC_RENDER_s *render, UINT32 x, UINT32 y, UINT32 color)
{
   *(((UINT32*)((UINT32)render->Buf + y*render->BufPitch)) + x) = color;
}

static void plotLineAt_u8(const APPLIB_GRAPHIC_RENDER_s *render, UINT32 x, UINT32 y, UINT32 w, UINT32 color)
{
    int t;
    for (t=0 ; t<w ; t++) {
        *(((UINT8*)((UINT32)render->Buf + y*render->BufPitch)) + x + t) = color;
    }
}

static void plotLineAt_u16(const APPLIB_GRAPHIC_RENDER_s *render, UINT32 x, UINT32 y, UINT32 w, UINT32 color)
{
    int t;
    for (t=0 ; t<w ; t++) {
        *(((UINT16*)((UINT32)render->Buf + y*render->BufPitch)) + x + t) = color;
    }
}

static void plotLineAt_u24(const APPLIB_GRAPHIC_RENDER_s *render, UINT32 x, UINT32 y, UINT32 w, UINT32 color)
{
    //int t;
    //for (t=0 ; t<w ; t++) {
    //    *(((UINT16*)((UINT32)render->Buf + y*render->BufPitch)) + x + t) = color;
    //}
}

static void plotLineAt_u32(const APPLIB_GRAPHIC_RENDER_s *render, UINT32 x, UINT32 y, UINT32 w, UINT32 color)
{
    int t;
    for (t=0 ; t<w ; t++) {
        *(((UINT32*)((UINT32)render->Buf + y*render->BufPitch)) + x + t) = color;
    }
}

static void speedyPlotLineAt_u8(const APPLIB_GRAPHIC_RENDER_s *render, UINT32 x, UINT32 y, UINT32 w, UINT32 color)
{
    int t;
    for (t=0 ; t<w ; t++) {
        *(((UINT8*)(render->RowStartAddr[y])) + x + t) = color;
    }
}

static void speedyPlotLineAt_u16(const APPLIB_GRAPHIC_RENDER_s *render, UINT32 x, UINT32 y, UINT32 w, UINT32 color)
{
    int t;
    for (t=0 ; t<w ; t++) {
        *(((UINT16*)(render->RowStartAddr[y])) + x + t) = color;
    }
}

static void speedyPlotLineAt_u24(const APPLIB_GRAPHIC_RENDER_s *render, UINT32 x, UINT32 y, UINT32 w, UINT32 color)
{
    //int t;
    //for (t=0 ; t<w ; t++) {
    //    *(((UINT16*)(render->RowStartAddr[y])) + x + t) = color;
    //}
}

static void speedyPlotLineAt_u32(const APPLIB_GRAPHIC_RENDER_s *render, UINT32 x, UINT32 y, UINT32 w, UINT32 color)
{
    int t;
    for (t=0 ; t<w ; t++) {
        *(((UINT32*)(render->RowStartAddr[y])) + x + t) = color;
    }
}

//
// get pixel impl
//

static void getPixel_u8(void *dst, UINT32 *color)
{
    (*color) = *((UINT8*)dst);
}

static void getPixel_u16(void *dst, UINT32 *color)
{
    (*color) = *((UINT16*)dst);
}

static void getPixel_u24(void *dst, UINT32 *color)
{
    (*color) = (*((UINT32*)dst) & 0xFFFFFF);
}

static void getPixel_u32(void *dst, UINT32 *color)
{
    (*color) = *((UINT32*)dst);
}

//
// plot line impl
//

static void plotLine_u8(void *dst, UINT32 w, UINT32 color)
{
    int t;
    for (t=0 ; t<w ; t++) {
        *(((UINT8*)dst) + t) = color;
    }
}

static void plotLine_u16(void *dst, UINT32 w, UINT32 color)
{
    int t;
    for (t=0 ; t<w ; t++) {
        *(((UINT16*)dst) + t) = color;
    }
}

static void plotLine_u24(void *dst, UINT32 w, UINT32 color)
{
    //int t;
    //for (t=0 ; t<w ; t++) {
    //    *(((UINT16*)dst) + t) = color;
    //}
}

static void plotLine_u32(void *dst, UINT32 w, UINT32 color)
{
    UINT32 t;
    for (t=0 ; t<w ; t++) {
        *(((UINT32*)dst) + t) = color;
    }
}

/**
 *  @brief Initialize Amba Render
 *
 *  Initialize Amba Render
 *  Init render, filled render->RowStartAddr for speedy(suggest),
 *
 *  @param [in] render the specidif render
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibRender_Init(APPLIB_GRAPHIC_RENDER_s *render)
{
    if (render == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_RENDER, "AppLibRender_Init error, render:0x%x invalid", render);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    } else {
        render->Cursor = render->Buf;
        // assign render function
        switch (render->BufPixelSize) {
        case 1:
            render->MoveNext_f = moveNext_u8;
            render->MoveNextFew_f = moveNextFew_u8;
            render->GetPixel_f = getPixel_u8;
            render->PlotPixel_f = plotPixel_u8;
            render->PlotHLine_f = plotLine_u8;
            break;
        case 2:
            render->MoveNext_f = moveNext_u16;
            render->MoveNextFew_f = moveNextFew_u16;
            render->GetPixel_f = getPixel_u16;
            render->PlotPixel_f = plotPixel_u16;
            render->PlotHLine_f = plotLine_u16;
            break;
        case 3:
            render->MoveNext_f = moveNext_u24;
            render->MoveNextFew_f = moveNextFew_u24;
            render->GetPixel_f = getPixel_u24;
            render->PlotPixel_f = plotPixel_u24;
            render->PlotHLine_f = plotLine_u24;
            break;
        case 4:
            render->MoveNext_f = moveNext_u32;
            render->MoveNextFew_f = moveNextFew_u32;
            render->GetPixel_f = getPixel_u32;
            render->PlotPixel_f = plotPixel_u32;
            render->PlotHLine_f = plotLine_u32;
            break;
        default:
            render->MoveNext_f = moveNext_u8;
            render->MoveNextFew_f = moveNextFew_u8;
            render->GetPixel_f = getPixel_u8;
            render->PlotPixel_f = plotPixel_u8;
            render->PlotHLine_f = plotLine_u8;
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_RENDER, "AppLibRender_Init: not supported pixel size");
            break;
        }

        if (render->RowStartAddr != NULL) {
            UINT32 tmp = (UINT32)render->Buf;
            UINT32 t;
            for (t=0 ; t<render->BufHeight ; t++) {
                render->RowStartAddr[t] = tmp;
                GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_RENDER, "rowStartAddr[%d]:0x%x",t,tmp);
                tmp += render->BufPitch;
            }
            switch (render->BufPixelSize) {
                case 1:
                    render->MoveTo_f = speedyMoveTo_u8;
                    render->PlotPixelAt_f = speedyPlotPixelAt_u8;
                    render->PlotHLineAt_f = speedyPlotLineAt_u8;
                    break;
                case 2:
                    render->MoveTo_f = speedyMoveTo_u16;
                    render->PlotPixelAt_f = speedyPlotPixelAt_u16;
                    render->PlotHLineAt_f = speedyPlotLineAt_u16;
                    break;
                case 3:
                    render->MoveTo_f = speedyMoveTo_u24;
                    render->PlotPixelAt_f = speedyPlotPixelAt_u24;
                    render->PlotHLineAt_f = speedyPlotLineAt_u24;
                    break;
                case 4:
                    render->MoveTo_f = speedyMoveTo_u32;
                    render->PlotPixelAt_f = speedyPlotPixelAt_u32;
                    render->PlotHLineAt_f = speedyPlotLineAt_u32;
                    break;
                default:
                    render->MoveTo_f = speedyMoveTo_u8;
                    render->PlotPixelAt_f = speedyPlotPixelAt_u8;
                    render->PlotHLineAt_f = speedyPlotLineAt_u8;
                    break;
                }
        } else {
            switch (render->BufPixelSize) {
                case 1:
                    render->MoveTo_f = moveTo_u8;
                    render->PlotPixelAt_f = plotPixelAt_u8;
                    render->PlotHLineAt_f = plotLineAt_u8;
                    break;
                case 2:
                    render->MoveTo_f = moveTo_u16;
                    render->PlotPixelAt_f = plotPixelAt_u16;
                    render->PlotHLineAt_f = plotLineAt_u16;
                    break;
                case 3:
                    render->MoveTo_f = moveTo_u24;
                    render->PlotPixelAt_f = plotPixelAt_u24;
                    render->PlotHLineAt_f = plotLineAt_u24;
                    break;
                case 4:
                    render->MoveTo_f = moveTo_u32;
                    render->PlotPixelAt_f = plotPixelAt_u32;
                    render->PlotHLineAt_f = plotLineAt_u32;
                    break;
                default:
                    render->MoveTo_f = moveTo_u8;
                    render->PlotPixelAt_f = plotPixelAt_u8;
                    render->PlotHLineAt_f = plotLineAt_u8;
                    break;
                }
        }
        return AMP_OK;
    }
}
