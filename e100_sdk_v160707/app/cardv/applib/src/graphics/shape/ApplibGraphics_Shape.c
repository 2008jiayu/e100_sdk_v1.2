/**
 * @file src/app/connected/applib/src/graphics/shape/ApplibGraphics_Shape.c
 *
 * Implementation of graphic shape in application Library.
 *
 * History:
 *    2013/12/06 - [Eric Yen] created file
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

#include "Shape.h"
#include <graphics/shape/ApplibGraphics_Shape.h>

/**
 * Graphic content shadow description
 */
typedef struct _RECT_SHADOW_s_ {
    UINT32 X1;
    UINT32 Y1;
    UINT32 X2;
    UINT32 Y2;
} RECT_SHADOW_s;

static UINT32 _ABS(INT32 a)
{
    UINT32 rval = 0;
    if (a<0) {
        rval = 0 - a;
    } else {
        rval = a;
    }
    return rval;
}

static void _Swap(UINT32 *a, UINT32 *b) /* CONFIG_APP_ARD */
{
    UINT32 tmp = 0;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

int AppLibLine_Render(APPLIB_GRAPHIC_RENDER_s *render,
                         UINT32 x1,
                         UINT32 y1,
                         UINT32 x2,
                         UINT32 y2,
                         UINT32 thickness,
                         UINT32 color)
{
    int i, drawStart, drawEnd;
    int tStart, tEnd;
    UINT32 tx1;

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "L (%d,%d)->(%d,%d) t:%d", x1,y1,x2,y2, thickness);
    //render buffer boundary clipping
    if (x1 > render->BufPitch) {
        x1 = x1 % render->BufPitch;
    }
    if (y1 > render->BufHeight) {
        y1 = y1 % render->BufHeight;
    }
    if (x2 > render->BufPitch) {
        x2 = x2 % render->BufPitch;
    }
    if (y2 > render->BufHeight) {
        y2 = y2 % render->BufHeight;
    }

    if (thickness>1) {
        if (thickness%2 == 1) { // 5/2=2, start from x1+2 ~ x1-2, tStart=2, tEnd=-2;
            tStart = thickness/2;
            tEnd = -1*tStart;
        } else {                // 4/2=2, start from x1+2 ~ x1-1, tStart=2, tEnd=-1;
            tStart = thickness/2;
            tEnd = -1*tStart +1;
        }
    } else {
        tStart = 0;
        tEnd = 0;
    }

    // check for horizontal line
    if (y1 == y2) {
        int tx2 = 0;
        int t = 0;
        if (x2 > x1) {
            tx1 = x1;
            tx2 = x2 - x1;
        } else {
            tx1 = x2;
            tx2 = x1 - x2;
        }
        for (t=tStart; t>=tEnd; t--) {
            int ty1 = 0;
            ty1 = y2 + t;
            render->PlotHLineAt_f(render, tx1, ty1, tx2, color);
        }
    } else if (x1 == x2) {
        if (y2 - y1 > 0) {
            drawStart = y1;
            drawEnd = y2;
        } else {
            drawStart = y2;
            drawEnd = y1;
        }
        tx1 = x1+tEnd;
        for (i = drawStart; i <= drawEnd; i++) {
            render->PlotHLineAt_f(render, tx1, i, thickness, color);
        }
    } else {
        UINT32 tx1 = x1, tx2 = x2;
        UINT32 ty1 = y1, ty2 = y2;
        UINT8 steep = (_ABS(y2-y1) > _ABS(x2-x1)) ? 1 : 0;  // slope > 1

        if (steep) {
            _Swap(&tx1, &ty1); /* CONFIG_APP_ARD */
            _Swap(&tx2, &ty2);
        }
        if (tx1 > tx2) {
            _Swap(&tx1, &tx2); /* CONFIG_APP_ARD */
            _Swap(&ty1, &ty2);
        }

        {
            INT32 deltaX = x2 - x1;
            INT32 deltaY = _ABS(y2 - y1);
            INT32 error = deltaX >> 1;

            drawStart = tx1;
            drawEnd = tx2;
            for (i = drawStart; i <= drawEnd; i++) {
                if (steep) {
                    if (thickness == 0) {
                        render->PlotPixelAt_f(render, ty1, i, color);
                    } else {
                        UINT32 thickLoop = thickness >> 1;
                        int j = 0;
                        for (j=0; j<thickness; j++) {
                            render->PlotPixelAt_f(render, (ty1-thickLoop)+j, i, color);
                        }
                    }
                } else {
                    if (thickness == 0) {
                        render->PlotPixelAt_f(render, i, ty1, color);
                    } else {
                        UINT32 thickLoop = thickness >> 1;
                        int j = 0;
                        for (j=0; j < thickness; j++) {
                            render->PlotPixelAt_f(render, (i-thickLoop)+j, ty1, color);
                        }
                    }
                }

                error -= deltaY;
                if (error < 0) {
                    if (y1 < y2) {
                        ty1++;
                    } else {
                        ty1--;
                    }
                    error += deltaX;
                }
            }
        }
    }

    return AMP_OK;
}

/**
 *  @brief Calculate the position and the size of the line
 *
 *  Calculate the position and the size of the line
 *
 *  @param [in] *obj The configures of the line
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibLine_CalcArea(APPLIB_GRAPHIC_OBJ_s *obj)
{
    APPLIB_GRAPHIC_LINE_CNT_s *line = NULL;
    if (obj == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "obj:0x%x invalid", obj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (obj->Content == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "obj->Content:0x%x invalid", obj->Content);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    line = (APPLIB_GRAPHIC_LINE_CNT_s *)obj->Content;
    obj->DisplayBox.X = ((line->X1 <= line->X2)? line->X1 : line->X2) - line->Thickness/2 - 1;
    obj->DisplayBox.Y = ((line->Y1 <= line->Y2)? line->Y1 : line->Y2) - line->Thickness/2 - 1;
    obj->DisplayBox.Width  = line->Thickness + 1 +
                       ((line->X1 <= line->X2)? (line->X2 - line->X1) : (line->X1 - line->X2));
    obj->DisplayBox.Height = line->Thickness + 1 +
                       ((line->Y1 <= line->Y2)? (line->Y2 - line->Y1) : (line->Y1 - line->Y2));
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "(%d, %d) w(%d, %d)", obj->DisplayBox.X, obj->DisplayBox.Y, obj->DisplayBox.Width, obj->DisplayBox.Height);
    return AMP_OK;
}

/**
 *  @brief Dump the infomation of the line object
 *
 *  Dump the infomation of the line object
 *
 *  @param [in] *obj The configures of the line
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibLine_Dump(APPLIB_GRAPHIC_OBJ_s *obj)
{
    APPLIB_GRAPHIC_LINE_CNT_s *lineCnt = NULL;
    if (obj == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "obj:0x%x invalid", obj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (obj->Content == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "obj->Content:0x%x invalid", obj->Content);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    lineCnt = (APPLIB_GRAPHIC_LINE_CNT_s *)obj->Content;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "Line X1:%d Y1:%d X2:%d Y2:%d Thickness:0x%x ColorFore:0x%x ColorBack:0x%x",
            lineCnt->X1, lineCnt->Y1, lineCnt->X2, lineCnt->Y2, lineCnt->Thickness, lineCnt->ColorFore, lineCnt->ColorBack);
    return AMP_OK;
}

/**
 *  @brief Dump the infomation of the line object
 *
 *  Dump the infomation of the line object
 *
 *  @param [in] *render The OSD render is going to draw on
 *  @param [in] *drawArea Draw area of OSD buffer
 *  @param [in] *obj The graphic object is going to operate
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibLine_Draw(APPLIB_GRAPHIC_RENDER_s *render,
                       AMP_AREA_s *drawArea,
                       APPLIB_GRAPHIC_OBJ_s *obj)
{
    APPLIB_GRAPHIC_LINE_CNT_s *lineCnt = NULL;

    if ( (!render) || (!obj)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "render: 0x%X, obj: 0x%X invalid", render, obj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if ( (!obj->Content)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "obj->Content:0x%x invalid", obj->Content);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    lineCnt = (APPLIB_GRAPHIC_LINE_CNT_s *)obj->Content;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "AppLibLine_Draw, 0x%X: %d %d %d %d %d 0x%x", render->Buf,
                      lineCnt->X1, lineCnt->Y1, lineCnt->X2, lineCnt->Y2, lineCnt->Thickness, lineCnt->ColorFore);
    AppLibLine_Render(render, lineCnt->X1, lineCnt->Y1, lineCnt->X2, lineCnt->Y2, lineCnt->Thickness, lineCnt->ColorFore);

    /* Update display area */
    obj->LastDisplayBox.X = obj->DisplayBox.X;
    obj->LastDisplayBox.Y = obj->DisplayBox.Y;
    obj->LastDisplayBox.Width = obj->DisplayBox.Width;
    obj->LastDisplayBox.Height = obj->DisplayBox.Height;

    return AMP_OK;
}

/**
 *  @brief Creator entry function of Line
 *
 *  The transform function between Line UI object struct and Line Object struct
 *
 *  @param [in] *descUIObj The configures of the Line UI object
 *  @param [out] *dstObj The configures of the object
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibLine_CreateObj(APPLIB_GRAPHIC_UIOBJ_s *descUIObj,
                            APPLIB_GRAPHIC_OBJ_s *dstLineObj)
{
    APPLIB_GRAPHIC_LINE_CNT_s *lineCnt = NULL;

    if (dstLineObj == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "AppLibLine_CreateObj dstLineObj:0x%x invalid.", dstLineObj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (descUIObj == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "AppLibLine_CreateObj descUIObj:0x%x invalid.", descUIObj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (descUIObj->Type != APPLIB_GRAPHIC_UIOBJ_LINE) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "AppLibLine_CreateObj descUIObj->Type:0x%x error.", descUIObj->Type);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    lineCnt = &descUIObj->Cnt.Line;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "X1:%d Y1:%d X2:%d Y2:%d Thickness:0x%x ColorFore:0x%x ColorBack:0x%x",
            lineCnt->X1, lineCnt->Y1, lineCnt->X2, lineCnt->Y2, lineCnt->Thickness, lineCnt->ColorFore, lineCnt->ColorBack);
    dstLineObj->DisplayBox.X = descUIObj->UIObjDisplayBox.X;
    dstLineObj->DisplayBox.Y = descUIObj->UIObjDisplayBox.Y;
    dstLineObj->DisplayBox.Width = descUIObj->UIObjDisplayBox.Width;     //pass original setting into new obj
    dstLineObj->DisplayBox.Height = descUIObj->UIObjDisplayBox.Height;   //canvas will altered then
    dstLineObj->LastDisplayBox.X = descUIObj->UIObjDisplayBox.X;
    dstLineObj->LastDisplayBox.Y = descUIObj->UIObjDisplayBox.Y;
    dstLineObj->LastDisplayBox.Width = descUIObj->UIObjDisplayBox.Width;      //pass original setting into new obj
    dstLineObj->LastDisplayBox.Height = descUIObj->UIObjDisplayBox.Height;    //canvas will altered then
    dstLineObj->AlphaTable = descUIObj->AlphaTable;
    dstLineObj->Layer = descUIObj->Layer;
    dstLineObj->Group = descUIObj->Group;
    dstLineObj->Show = descUIObj->DefaultShow;
    dstLineObj->Stat = OBJ_STAT_NORMAL;
    dstLineObj->Content = (void *)lineCnt;
    dstLineObj->CalcArea_f = AppLibLine_CalcArea;
    dstLineObj->Dump_f = AppLibLine_Dump;
    dstLineObj->Draw_f = AppLibLine_Draw;
    return AMP_OK;
}

int AppLibRect_Render(APPLIB_GRAPHIC_RENDER_s *render,
                            UINT32 x1,
                            UINT32 y1,
                            UINT32 x2,
                            UINT32 y2,
                            INT32 thickness,
                            UINT32 color)
{
    int i, drawStart, drawEnd, startX;
    int dx;

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "R (%d,%d)->(%d,%d) t:%d", x1,y1,x2,y2,thickness);
    //render buffer boundary clipping
    if (x1 <= 1) {
        x1 = 1;
    }
    if (y1 <= 1) {
        y1 = 1;
    }
    if (x2 <= 1) {
        x2 = 1;
    }
    if (y2 <= 1) {
        y2 = 1;
    }

	if (x1 >= render->BufPitch) {
        x1 = render->BufPitch;
    }
    if (y1 >= render->BufHeight) {
        y1 = render->BufHeight;
    }
    if (x2 >= render->BufPitch) {
        x2 = render->BufPitch;
    }
    if (y2 >= render->BufHeight) {
        y2 = render->BufHeight;
    }

    x1 -= 1;
    y1 -= 1;
    x2 -= 1;
    y2 -= 1;

    // check for horizontal line
    if (y1 == y2 || x1 == x2) {
        AppLibLine_Render(render, x1, y1, x2, y2, thickness, color);
    } else if (thickness <= 0) {
        dx = abs(x1 - x2);
        if (y2 - y1 > 0) {
            drawStart = y1;
            drawEnd = y2;
        } else {
            drawStart = y2;
            drawEnd = y1;
        }
        startX = (x1 < x2) ? x1 : x2;
        for (i = drawStart; i <= drawEnd; i++) {
            render->PlotHLineAt_f(render, startX, i, dx, color);
        }
    } else {
        int tl=thickness/2;
        int tr=tl;
        if (thickness%2 != 1) { // 4/2=2, tl=1, tr=2
            tl -= 1;
        }
        //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "tl:%d tr:%d", tl, tr);
        //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "L %d %d %d %d", x1   , y1-tl, x1   , y2+tr);
        //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "R %d %d %d %d", x2   , y1-tl, x2   , y2+tr);
        //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "T %d %d %d %d", x1-tl, y1   , x2+tr, y1   );
        //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "B %d %d %d %d", x1-tl, y2   , x2+tr, y2   );
        AppLibLine_Render(render, x1   , y1-tl, x1   , y2+tr, thickness, color);     //L
        AppLibLine_Render(render, x2   , y1-tl, x2   , y2+tr, thickness, color);     //R
        AppLibLine_Render(render, x1-tl, y1   , x2+tr, y1   , thickness, color);     //T
        AppLibLine_Render(render, x1-tl, y2   , x2+tr, y2   , thickness, color);     //B
    }

    return AMP_OK;
}

/**
 *  @brief Calculate the position and the size of the rectangle
 *
 *  Calculate the position and the size of the rectangle
 *
 *  @param [in] *obj The configures of the rectangle
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibRect_CalcArea(APPLIB_GRAPHIC_OBJ_s *obj)
{
    APPLIB_GRAPHIC_RECT_CNT_s *rect = NULL;

    if ( (!obj) || (!obj->Content) ) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "obj:0x%x invalid", obj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    rect = (APPLIB_GRAPHIC_RECT_CNT_s *)obj->Content;
    obj->DisplayBox.X = rect->X1 - rect->Thickness/2;
    obj->DisplayBox.Y = rect->Y1 - rect->Thickness/2;
    obj->DisplayBox.Width  = rect->Thickness + 1 +
                       ((rect->X2 > rect->X1) ? (rect->X2 - rect->X1) : (rect->X1 - rect->X2));
    obj->DisplayBox.Height = rect->Thickness + 1 +
                       ((rect->Y2 > rect->Y1) ? (rect->Y2 - rect->Y1) : (rect->Y1 - rect->Y2));
    return AMP_OK;
}

/**
 *  @brief Dump the infomation of the rectangle object
 *
 *  Dump the infomation of the rectangle object
 *
 *  @param [in] *obj The configures of the rectangle
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibRect_Dump(APPLIB_GRAPHIC_OBJ_s *obj)
{
    APPLIB_GRAPHIC_RECT_CNT_s *rect = NULL;

    if (obj == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "obj:0x%x invalid", obj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (obj->Content == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "obj->Content:0x%x invalid", obj->Content);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    rect = (APPLIB_GRAPHIC_RECT_CNT_s *)obj->Content;
    GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "Rect X1:%d Y1:%d X2:%d Y2:%d Thickness:0x%x ColorFore:0x%x ColorBack:0x%x",
            rect->X1, rect->Y1, rect->X2, rect->Y2, rect->Thickness, rect->ColorFore, rect->ColorBack);
    return AMP_OK;
}

/**
 *  @brief Dump the infomation of the rectangle object
 *
 *  Dump the infomation of the rectangle object
 *
 *  @param [in] *render The OSD render is going to draw on
 *  @param [in] *drawArea Draw area of OSD buffer
 *  @param [in] *obj The graphic object is going to operate
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibRect_Draw(APPLIB_GRAPHIC_RENDER_s *render,
                       AMP_AREA_s *drawArea,
                       APPLIB_GRAPHIC_OBJ_s *obj)
{
    APPLIB_GRAPHIC_RECT_CNT_s *rect = NULL;

    if ((!render) || (!obj)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "render:0x%x, obj:0x%x invalid", render, obj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (!obj->Content) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "obj->Content:0x%x invalid", obj->Content);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    rect = (APPLIB_GRAPHIC_RECT_CNT_s *)obj->Content;

    /* Issue shadow */
    if (rect->Shadow.Enable) {
        RECT_SHADOW_s shadowInfo = {0};

        switch (rect->Shadow.Postion) {
            case APPLIB_GRAPHIC_SHAPE_SHADOW_TL:
                /* Update shadow area */
                shadowInfo.X1 = obj->DisplayBox.X - rect->Shadow.Distance;
                shadowInfo.Y1 = obj->DisplayBox.Y - rect->Shadow.Distance;
                /* Update display area */
                obj->LastDisplayBox.X = shadowInfo.X1;
                obj->LastDisplayBox.Y = shadowInfo.Y1;
                obj->LastDisplayBox.Width = obj->DisplayBox.Width  + rect->Shadow.Distance;
                obj->LastDisplayBox.Height = obj->DisplayBox.Height + rect->Shadow.Distance;
                break;
            case APPLIB_GRAPHIC_SHAPE_SHADOW_TM:
                /* Update shadow area */
                shadowInfo.X1 = obj->DisplayBox.X;
                shadowInfo.Y1 = obj->DisplayBox.Y - rect->Shadow.Distance;
                /* Update display area */
                obj->LastDisplayBox.X = shadowInfo.X1;
                obj->LastDisplayBox.Y = shadowInfo.Y1;
                obj->LastDisplayBox.Width = obj->DisplayBox.Width;
                obj->LastDisplayBox.Height = obj->DisplayBox.Height + rect->Shadow.Distance;
                break;
            case APPLIB_GRAPHIC_SHAPE_SHADOW_TR:
                /* Update shadow area */
                shadowInfo.X1 = obj->DisplayBox.X + rect->Shadow.Distance;
                shadowInfo.Y1 = obj->DisplayBox.Y - rect->Shadow.Distance;
                /* Update display area */
                obj->LastDisplayBox.X = obj->DisplayBox.X;
                obj->LastDisplayBox.Y = shadowInfo.Y1;
                obj->LastDisplayBox.Width = obj->DisplayBox.Width  + rect->Shadow.Distance;
                obj->LastDisplayBox.Height = obj->DisplayBox.Height + rect->Shadow.Distance;
                break;
            case APPLIB_GRAPHIC_SHAPE_SHADOW_ML:
                /* Update shadow area */
                shadowInfo.X1 = obj->DisplayBox.X - rect->Shadow.Distance;
                shadowInfo.Y1 = obj->DisplayBox.Y;
                /* Update display area */
                obj->LastDisplayBox.X = shadowInfo.X1;
                obj->LastDisplayBox.Y = shadowInfo.Y1;
                obj->LastDisplayBox.Width = obj->DisplayBox.Width + rect->Shadow.Distance;
                obj->LastDisplayBox.Height = obj->DisplayBox.Height;
                break;
            case APPLIB_GRAPHIC_SHAPE_SHADOW_MM:
                /* Update shadow area */
                shadowInfo.X1 = obj->DisplayBox.X;
                shadowInfo.Y1 = obj->DisplayBox.Y;
                /* Update display area */
                obj->LastDisplayBox.X = obj->DisplayBox.X;
                obj->LastDisplayBox.Y = obj->DisplayBox.Y;
                obj->LastDisplayBox.Width = obj->DisplayBox.Width;
                obj->LastDisplayBox.Height = obj->DisplayBox.Height;
                break;
            case APPLIB_GRAPHIC_SHAPE_SHADOW_MR:
                /* Update shadow area */
                shadowInfo.X1 = obj->DisplayBox.X + rect->Shadow.Distance;
                shadowInfo.Y1 = obj->DisplayBox.Y;
                /* Update display area */
                obj->LastDisplayBox.X = obj->DisplayBox.X;
                obj->LastDisplayBox.Y = obj->DisplayBox.Y;
                obj->LastDisplayBox.Width = obj->DisplayBox.Width + rect->Shadow.Distance;
                obj->LastDisplayBox.Height = shadowInfo.Y2;
                break;
            case APPLIB_GRAPHIC_SHAPE_SHADOW_BL:
                /* Update shadow area */
                shadowInfo.X1 = obj->DisplayBox.X - rect->Shadow.Distance;
                shadowInfo.Y1 = obj->DisplayBox.Y + rect->Shadow.Distance;
                /* Update display area */
                obj->LastDisplayBox.X = shadowInfo.X1;
                obj->LastDisplayBox.Y = obj->DisplayBox.Y;
                obj->LastDisplayBox.Width = obj->DisplayBox.Width  + rect->Shadow.Distance;
                obj->LastDisplayBox.Height = obj->DisplayBox.Height + rect->Shadow.Distance;
                break;
            case APPLIB_GRAPHIC_SHAPE_SHADOW_BM:
                /* Update shadow area */
                shadowInfo.X1 = obj->DisplayBox.X;
                shadowInfo.Y1 = obj->DisplayBox.Y + rect->Shadow.Distance;
                /* Update display area */
                obj->LastDisplayBox.X = obj->DisplayBox.X;
                obj->LastDisplayBox.Y = obj->DisplayBox.Y;
                obj->LastDisplayBox.Width = obj->DisplayBox.Width;
                obj->LastDisplayBox.Height = obj->DisplayBox.Height + rect->Shadow.Distance;
                break;
            case APPLIB_GRAPHIC_SHAPE_SHADOW_BR:
                /* Update shadow area */
                shadowInfo.X1 = obj->DisplayBox.X + rect->Shadow.Distance;
                shadowInfo.Y1 = obj->DisplayBox.Y + rect->Shadow.Distance;
                /* Update display area */
                obj->LastDisplayBox.X = obj->DisplayBox.X;
                obj->LastDisplayBox.Y = obj->DisplayBox.Y;
                obj->LastDisplayBox.Width = obj->DisplayBox.Width  + rect->Shadow.Distance;
                obj->LastDisplayBox.Height = obj->DisplayBox.Height + rect->Shadow.Distance;
                break;
        }
        shadowInfo.X2 = shadowInfo.X1 + obj->DisplayBox.Width;
        shadowInfo.Y2 = shadowInfo.Y1 + obj->DisplayBox.Height;
        AppLibRect_Render(render, shadowInfo.X1, shadowInfo.Y1, shadowInfo.X2, shadowInfo.Y2, 0, rect->Shadow.Color);
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "[AppLib RectDraw][Shadow] X1 %d, Y1 %d, X2 %d, Y2 %d, color 0x%x",
                   shadowInfo.X1, shadowInfo.Y1, shadowInfo.X2, shadowInfo.Y2, rect->Shadow.Color);
    } else {
        obj->LastDisplayBox.X = obj->DisplayBox.X;
        obj->LastDisplayBox.Y = obj->DisplayBox.Y;
        obj->LastDisplayBox.Width = obj->DisplayBox.Width;
        obj->LastDisplayBox.Height = obj->DisplayBox.Height;
    }

    /* Issue object */
    AppLibRect_Render(render, rect->X1, rect->Y1, rect->X2, rect->Y2, rect->Thickness, rect->ColorFore);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "AppLibLine_Draw, %d %d %d %d %d 0x%x",
                    rect->X1, rect->Y1, rect->X2, rect->Y2, rect->Thickness, rect->ColorFore);
    return AMP_OK;
}

/**
 *  @brief Creator entry function of Rectangle
 *
 *  The transform function between Rectangle UI object struct and Rectangle Object struct
 *
 *  @param [in] *descUIObj The configures of the Rectangle UI object
 *  @param [out] *dstObj The configures of the object
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibRect_CreateObj(APPLIB_GRAPHIC_UIOBJ_s *descUIObj,
                            APPLIB_GRAPHIC_OBJ_s *dstRectObj)
{
    APPLIB_GRAPHIC_RECT_CNT_s *rect = NULL;

    if ((!descUIObj) || (!dstRectObj)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "AppLibRect_CreateObj descUIObj:0x%x, dstRectObj:0x%x invalid.", descUIObj, dstRectObj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (descUIObj->Type != APPLIB_GRAPHIC_UIOBJ_RECT) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "AppLibRect_CreateObj descUIObj->Type:0x%x error.", descUIObj->Type);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    rect = &descUIObj->Cnt.Rect;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "X1:%d Y1:%d X2:%d Y2:%d Thickness:0x%x ColorFore:0x%x ColorBack:0x%x",
            rect->X1, rect->Y1, rect->X2, rect->Y2, rect->Thickness, rect->ColorFore, rect->ColorBack);
    dstRectObj->DisplayBox.X = descUIObj->UIObjDisplayBox.X;
    dstRectObj->DisplayBox.Y = descUIObj->UIObjDisplayBox.Y;
    dstRectObj->DisplayBox.Width = descUIObj->UIObjDisplayBox.Width;     //pass original setting into new obj
    dstRectObj->DisplayBox.Height = descUIObj->UIObjDisplayBox.Height;   //canvas will altered then
    dstRectObj->LastDisplayBox.X = descUIObj->UIObjDisplayBox.X;
    dstRectObj->LastDisplayBox.Y = descUIObj->UIObjDisplayBox.Y;
    dstRectObj->LastDisplayBox.Width = descUIObj->UIObjDisplayBox.Width;      //pass original setting into new obj
    dstRectObj->LastDisplayBox.Height = descUIObj->UIObjDisplayBox.Height;    //canvas will altered then
    dstRectObj->AlphaTable = descUIObj->AlphaTable;
    dstRectObj->Layer = descUIObj->Layer;
    dstRectObj->Group = descUIObj->Group;
    dstRectObj->Show = descUIObj->DefaultShow;
    dstRectObj->Stat = OBJ_STAT_NORMAL;
    dstRectObj->Content = (void *)rect;
    dstRectObj->CalcArea_f = AppLibRect_CalcArea;
    dstRectObj->Dump_f = AppLibRect_Dump;
    dstRectObj->Draw_f = AppLibRect_Draw;
    return AMP_OK;
}

#define PxlInArea(xl, xr, yt, yb, x, y)   ((x>=xl)&&(x<=xr)&&(y>=yt)&&(y<=yb))

// google midpoint for the algorithm if you want to know how the implementation work.
int AppLibCirc_Render(APPLIB_GRAPHIC_RENDER_s *render,
                              UINT32 centerX,
                              UINT32 centerY,
                              UINT32 radius,
                              INT32 thickness,
                              UINT32 color,
                              AMP_AREA_s *updateArea)
{
    int error = -radius;
    int x = radius;
    int y = 0;
    int x2, y1, y2;
    int xl, xr, yt, yb;
    int tt=thickness-1;

    if (!updateArea) {
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (render == NULL) {
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    #define setPixel(x, y)   render->PlotPixelAt_f(render, x, y, color);

    xl = updateArea->X;
    yt = updateArea->Y;
    xr = xl + updateArea->Width - 1;
    yb = yt + updateArea->Height - 1;
    //render buffer boundary clipping
    if (xl > render->BufPitch) {
        xl = xl % render->BufPitch;
    }
    if (yt > render->BufHeight) {
        yt = yt % render->BufHeight;
    }
    if (xr > render->BufPitch) {
        xr = xr % render->BufPitch;
    }
    if (yb > render->BufHeight) {
        yb = yb % render->BufHeight;
    }
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "cx:%d cy:%d r:%d t:%d color:0x%x u.x:%d u.y:%d u.w:%d u.h:%d", centerX, centerY, radius, thickness, color,
            updateArea->X, updateArea->Y, updateArea->Width, updateArea->Height);

    // TBD performance fine tune in asm
    if (thickness > 0) {
        if (thickness>1) {
            int tl = 0, tr = 0;;
            if (thickness%2 == 1) { // 5/2=2, tl=tr=2
                tl = tr = thickness/2;
            } else {                // 4/2=2, tl=1 tr=2
                tr = thickness/2;
                tl = thickness - tr;
            }
            error -= tr;
            x += tr;
            xl -= tl;
            yt -= tl;
            xr += tr;
            yb += tr;
            tt=thickness;
        }
        while (x >= y) {
            int x1 = centerX + x;
            x2 = centerX - x;
            y1 = centerY + y;
            y2 = centerY - y;
            if (PxlInArea(xl, xr, yt, yb, x1, y1)) {
                render->PlotHLineAt_f(render, x1-tt, y1, thickness, color);
            }
            if (PxlInArea(xl, xr, yt, yb, x1, y2)) {
                render->PlotHLineAt_f(render, x1-tt, y2, thickness, color);
            }
            if (PxlInArea(xl, xr, yt, yb, x2, y1)) {
                render->PlotHLineAt_f(render, x2, y1, thickness, color);
            }
            if (PxlInArea(xl, xr, yt, yb, x2, y2)) {
                render->PlotHLineAt_f(render, x2, y2, thickness, color);
            }

            // switch x, y and do it again
            x1 = centerX + y;
            x2 = centerX - y;
            y1 = centerY + x;
            y2 = centerY - x;
            if (PxlInArea(xl, xr, yt, yb, x1, y1)) {
                int t = 0;
                for (t=thickness-1; t>=0; t--) setPixel(x1, y1-t);
            }
            if (PxlInArea(xl, xr, yt, yb, x1, y2)) {
                int t = 0;
                for (t=0; t<=thickness-1; t++) setPixel(x1, y2+t);
            }
            if (PxlInArea(xl, xr, yt, yb, x2, y1)) {
                int t = 0;
                for (t=thickness-1; t>=0; t--) setPixel(x2, y1-t);
            }
            if (PxlInArea(xl, xr, yt, yb, x2, y2)) {
                int t = 0;
                for (t=0; t<=thickness-1; t++) setPixel(x2, y2+t);
            }

            error += y;
            ++y;
            error += y;

            if (error >= 0) {
                error -= x;
                --x;
                error -= x;
            }
        }
    } else {
        // filled circle
        /// TBD assume full update
        while (x >= y) {
            //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "x:%d y:%d err:%d",x,y, error);
            //x1 = centerX + x;
            x2 = centerX - x;
            y1 = centerY + y;
            y2 = centerY - y;
            render->PlotHLineAt_f(render, x2, y1, x<<1, color);
            render->PlotHLineAt_f(render, x2, y2, x<<1, color);

            // switch x, y and do it again
            //x1 = centerX + x;
            x2 = centerX - y;
            y1 = centerY + x;
            y2 = centerY - x;
            render->PlotHLineAt_f(render, x2, y1, y<<1, color);
            render->PlotHLineAt_f(render, x2, y2, y<<1, color);

            error += y;
            ++y;
            error += y;

            if (error >= 0) {
                error -= x;
                --x;
                error -= x;
            }
        }
    }
    return AMP_OK;
}

/**
 *  @brief Calculate the position and the size of the circle
 *
 *  Calculate the position and the size of the circle
 *
 *  @param [in] *obj The configures of the circle
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibCirc_CalcArea(APPLIB_GRAPHIC_OBJ_s *obj)
{
    APPLIB_GRAPHIC_CIRCLE_CNT_s *circ = NULL;

    if (obj == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "obj:0x%x invalid", obj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (obj->Content == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "obj->Content:0x%x invalid", obj->Content);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    circ = (APPLIB_GRAPHIC_CIRCLE_CNT_s *)obj->Content;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "Circ CenterX:%d CenterY:%d Radius:%d Thickness:0x%x ColorFore:0x%x ColorBack:0x%x",
            circ->CenterX, circ->CenterY, circ->Radius, circ->Thickness, circ->ColorFore, circ->ColorBack);
    obj->DisplayBox.X = circ->CenterX - circ->Radius - circ->Thickness/2 - 1;
    obj->DisplayBox.Y = circ->CenterY - circ->Radius - circ->Thickness/2 - 1;
    obj->DisplayBox.Width  = circ->Thickness + 1 + circ->CenterX + circ->Radius;
    obj->DisplayBox.Height = circ->Thickness + 1 + circ->CenterY + circ->Radius;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "(%d, %d) w(%d, %d)", obj->DisplayBox.X, obj->DisplayBox.Y, obj->DisplayBox.Width, obj->DisplayBox.Height);
    return AMP_OK;
}

/**
 *  @brief Dump the infomation of the circlue object
 *
 *  Dump the infomation of the circlue object
 *
 *  @param [in] *obj The configures of the circle
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibCirc_Dump(APPLIB_GRAPHIC_OBJ_s *obj)
{
    APPLIB_GRAPHIC_CIRCLE_CNT_s *circ = NULL;
    if (obj == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "obj:0x%x invalid", obj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (obj->Content == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "obj->Content:0x%x invalid", obj->Content);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    circ = (APPLIB_GRAPHIC_CIRCLE_CNT_s *)obj->Content;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "Circ CenterX:%d CenterY:%d Radius:%d Thickness:0x%x ColorFore:0x%x ColorBack:0x%x",
            circ->CenterX, circ->CenterY, circ->Radius, circ->Thickness, circ->ColorFore, circ->ColorBack);
    return AMP_OK;
}

/**
 *  @brief Dump the infomation of the circlue object
 *
 *  Dump the infomation of the circlue object
 *
 *  @param [in] *render The OSD render is going to draw on
 *  @param [in] *drawArea Draw area of OSD buffer
 *  @param [in] *obj The graphic object is going to operate
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibCirc_Draw(APPLIB_GRAPHIC_RENDER_s *render,
                       AMP_AREA_s *drawArea,
                       APPLIB_GRAPHIC_OBJ_s *obj)
{
    APPLIB_GRAPHIC_CIRCLE_CNT_s *circ = NULL;

    if ((!render) || (!obj) || (!drawArea)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "render:0x%x, obj:0x%x invalid", render, obj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (!obj->Content) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "obj->Content:0x%x invalid", obj->Content);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    circ = (APPLIB_GRAPHIC_CIRCLE_CNT_s *)obj->Content;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "AppLibLine_Draw, %d %d %d %d 0x%x %d %d %d %d",
            circ->CenterX, circ->CenterY, circ->Radius, circ->Thickness, circ->ColorFore,
            drawArea->X, drawArea->Y, drawArea->Width, drawArea->Height);
    AppLibCirc_Render(render, circ->CenterX, circ->CenterY, circ->Radius, circ->Thickness, circ->ColorFore, drawArea);
    return AMP_OK;
}

/**
 *  @brief Creator entry function of Circle
 *
 *  The transform function between Circle UI object struct and Circle Object struct
 *
 *  @param [in] *descUIObj The configures of the Circle UI object
 *  @param [out] *dstObj The configures of the object
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibCirc_CreateObj(APPLIB_GRAPHIC_UIOBJ_s *descUIObj,
                            APPLIB_GRAPHIC_OBJ_s *dstCircObj)
{
    APPLIB_GRAPHIC_CIRCLE_CNT_s *circ = NULL;

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "AppLibCirc_CreateObj start");
    if (dstCircObj == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "AppLibCirc_CreateObj dstLineObj:0x%x invalid.", dstCircObj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (descUIObj == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "AppLibCirc_CreateObj descUIObj:0x%x invalid.", descUIObj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (descUIObj->Type != APPLIB_GRAPHIC_UIOBJ_CIRCLE) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_SHAPE, "AppLibCirc_CreateObj descUIObj->Type:0x%x error.", descUIObj->Type);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    circ = &descUIObj->Cnt.Circle;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_SHAPE, "CenterX:%d CenterY:%d Radius:%d Thickness:0x%x ColorFore:0x%x ColorBack:0x%x",
                 circ->CenterX, circ->CenterY, circ->Radius, circ->Thickness, circ->ColorFore, circ->ColorBack);
    dstCircObj->DisplayBox.X = descUIObj->UIObjDisplayBox.X;
    dstCircObj->DisplayBox.Y = descUIObj->UIObjDisplayBox.Y;
    dstCircObj->DisplayBox.Width = descUIObj->UIObjDisplayBox.Width;     //pass original setting into new obj
    dstCircObj->DisplayBox.Height = descUIObj->UIObjDisplayBox.Height;   //canvas will altered then
    dstCircObj->AlphaTable = descUIObj->AlphaTable;
    dstCircObj->Layer = descUIObj->Layer;
    dstCircObj->Group = descUIObj->Group;
    dstCircObj->Show = descUIObj->DefaultShow;
    dstCircObj->Stat = OBJ_STAT_NORMAL;
    dstCircObj->Content = (void *)circ;
    dstCircObj->CalcArea_f = AppLibCirc_CalcArea;
    dstCircObj->Dump_f = AppLibCirc_Dump;
    dstCircObj->Draw_f = AppLibCirc_Draw;
    return AMP_OK;
}

#if 0
static void findGobjinCanvas(APPLIB_CANVAS_t *targetCanvas,
                             APPLIB_GRAPHIC_OBJ_ID_t targetID,
                             APPLIB_GRAPHIC_OBJ_LIST_t **dstObj)
{
    APPLIB_GRAPHIC_OBJ_LIST_t *GobjArr = targetCanvas->CanvasCacheBaseAddr;
    UINT32 idx = 0;

    for (; idx <= targetCanvas->GobjIDmax; idx++) {
        if (GobjArr[idx].objID == targetID) {
            *dstObj = &GobjArr[idx];
            return;
        }
    }
    //not found
    appgoj_perror(0, AMP_ERROR_GENERAL_ERROR, "cannot find target Obj.");
    *dstObj = NULL;
}
/*
static void findGobjinList(APPLIB_CANVAS_t *targetCanvas,
                             APPLIB_GRAPHIC_OBJ_ID_t targetID,
                             APPLIB_GRAPHIC_OBJ_LIST_t **dstObj)
{
    *dstObj = targetCanvas->GobjListHead;
    do {
        if ((*dstObj)->objID == targetID) {
            appgoj_print("obj found at 0x%x", *dstObj);
            return;
        }
        *dstObj = (*dstObj)->nextObj;
    } while (*dstObj != NULL);
    //not found
    appgoj_perror(0, AMP_ERROR_GENERAL_ERROR, "cannot find target Obj.");
    *dstObj = NULL;
}*/

static int insertGobjInList(APPLIB_CANVAS_t *targetCanvas,
                     APPLIB_GRAPHIC_OBJ_LIST_t *targetObj)
{
    if (targetObj->objAttr.layer > 0) {
        //TODO multi layer support
        appgoj_print("Doesn't support multiple layer yet");
        return -1;
    } else {
        if (targetCanvas->GobjListHead == NULL) {
            targetCanvas->GobjListHead = targetObj;
            targetCanvas->GobjListTail = targetObj;
            targetObj->nextObj = NULL;
            appgoj_print("New insert, set GobjHead to:0x%x", targetObj);
        } else {
            targetObj->nextObj = NULL;
            targetObj->prevObj = targetCanvas->GobjListTail;
            targetCanvas->GobjListTail->nextObj = targetObj;
            targetCanvas->GobjListTail = targetObj;
        }
        appgoj_print("insert, h:0x%x, t:0x%x", targetCanvas->GobjListHead, targetCanvas->GobjListTail);
        return 0;
    }
}

static void calcArea(APPLIB_CANVAS_t *targetCanvas,
             APPLIB_GRAPHIC_OBJ_s *targetObj)
{
    AMP_AREA_s calcedArea = {0, 0, 0, 0};
    switch (targetObj->type) {
        case APPLIB_GRAPHIC_CNT_LINE:
        {
            APPLIB_GRAPHIC_DESC_LINE_t *line = (APPLIB_GRAPHIC_DESC_LINE_t *)(&targetObj->cnt);
            appgoj_print("(%d, %d) 2(%d, %d) c(%d, %d) t:%d", line->x1, line->y1, line->x2, line->y2, line->colorFore, line->colorBack, line->thickness);
            calcedArea.X = ((line->x1 <= line->x2)? line->x1 : line->x2) - line->thickness/2 - 1;
            calcedArea.Y = ((line->y1 <= line->y2)? line->y1 : line->y2) - line->thickness/2 - 1;;
            calcedArea.Width  = line->thickness + 1 +
                                ((line->x1 <= line->x2)? (line->x2 - line->x1) : (line->x1 - line->x2));
            calcedArea.Height = line->thickness + 1 +
                                ((line->y1 <= line->y2)? (line->y2 - line->y1) : (line->y1 - line->y2));
        }
            break;
        case APPLIB_GRAPHIC_CNT_RECT:
        {
            APPLIB_GRAPHIC_DESC_RECT_t *rect = (APPLIB_GRAPHIC_DESC_RECT_t *)(&targetObj->cnt);
            appgoj_print("(%d, %d) w(%d, %d) c(%d, %d) t:%d", rect->x1, rect->y1, rect->x2, rect->y2, rect->colorFore, rect->colorBack, rect->thickness);
            calcedArea.X = rect->x1 - rect->thickness/2 - 1;
            calcedArea.Y = rect->y1 - rect->thickness/2 - 1;
            calcedArea.Width  = rect->thickness + 1 +
                                ((rect->x2 > rect->x1) ? (rect->x2 - rect->x1) : (rect->x1 - rect->x2));
            calcedArea.Height = rect->thickness + 1 +
                                ((rect->y2 > rect->y1) ? (rect->y2 - rect->y1) : (rect->y1 - rect->y2));
        }
            break;
        case APPLIB_GRAPHIC_CNT_CIRCLE:
        {
            APPLIB_GRAPHIC_DESC_CIRCLE_t *c = (APPLIB_GRAPHIC_DESC_CIRCLE_t *)(&targetObj->cnt);
            appgoj_print("(%d, %d) r(%d    ) c(%d, %d) t:%d", c->centerX, c->centerY, c->radius, c->colorFore, c->colorBack, c->thickness);
            calcedArea.X = c->centerX - c->radius - c->thickness/2 - 1;
            calcedArea.Y = c->centerY - c->radius - c->thickness/2 - 1;;
            calcedArea.Width  = c->thickness+1 +
                                c->centerX + c->radius;
            calcedArea.Height = c->thickness+1 +
                                c->centerY + c->radius;
        }
            break;
        case APPLIB_GRAPHIC_CNT_ELLIPSE:
        {
            APPLIB_GRAPHIC_DESC_ELLIPSE_t *e = (APPLIB_GRAPHIC_DESC_ELLIPSE_t *)(&targetObj->cnt);
            //appgoj_print("(%d, %d) r(%d, %d) c(%d, %d)", e->centerX, e->centerY, e->radiusH, e->radiusV, e->colorFore, e->colorBack);
            calcedArea.X = e->centerX - e->radiusH;
            calcedArea.Y = e->centerY - e->radiusV;
            calcedArea.Width  = e->centerX + e->radiusH;
            calcedArea.Height = e->centerY + e->radiusV;
        }
            break;
        case APPLIB_GRAPHIC_CNT_BMP:
        {
            APPLIB_GRAPHIC_DESC_BMP_t *bmp = (APPLIB_GRAPHIC_DESC_BMP_t *)(&targetObj->cnt);
            //appgoj_print("(%d, %d) w(%d, %d) flg:%d, bits:%d, pxf:0x%x, tc:%0xx, ptr:0x%x",
            //        bmp->x1, bmp->y1, bmp->width, bmp->height, bmp->flags, bmp->bits, bmp->pxf, bmp->tcolor, bmp->ptr);
            calcedArea.X = bmp->x1;
            calcedArea.Y = bmp->y1;
            calcedArea.Width  = bmp->width;
            calcedArea.Height = bmp->height;
        }
            break;
        case APPLIB_GRAPHIC_CNT_YUV:
        {
            //APPLIB_GRAPHIC_DESC_YUV_t *line = &(tmpObj->objAttr.cnt);
            appgoj_print("YUV type, not ready");
        }
            break;
        case APPLIB_GRAPHIC_CNT_STRING:
        {
            APPLIB_GRAPHIC_DESC_STR_t *s = (APPLIB_GRAPHIC_DESC_STR_t *)(&targetObj->cnt);
            //appgoj_print("(%d, %d) h(%d   ) c(%d, %d)", s->x1, s->y1, s->height, s->colorFore, s->colorBack);
            calcedArea.X = s->x1;
            calcedArea.Y = s->y1;
            calcedArea.Width  = s->CharNum * s->sizeHeight;
            calcedArea.Height = s->sizeHeight;
        }
            break;
    }
    targetObj->area.X = calcedArea.X;
    targetObj->area.Y = calcedArea.Y;
    targetObj->area.Width = calcedArea.Width;
    targetObj->area.Height = calcedArea.Height;
    appgoj_print("x:%d, y:%d, width:%d, height:%d", targetObj->area.X, targetObj->area.Y, targetObj->area.Width, targetObj->area.Height);
}


static void dumpGobj(APPLIB_GRAPHIC_OBJ_s *obj)
{
    APPLIB_GRAPHIC_CNT_e type = obj->type;
    APPLIB_GRAPHIC_DESC_u *cnt = &obj->cnt;
    //appgoj_print("type:%d, cnt:0x%x", type, cnt);
    switch (type) {
        case APPLIB_GRAPHIC_CNT_LINE:
        {
            APPLIB_GRAPHIC_DESC_LINE_t *line = (APPLIB_GRAPHIC_DESC_LINE_t *)cnt;
            appgoj_print("LINE:(%d, %d) 2(%d, %d) c(%d, %d)", line->x1, line->y1, line->x2, line->y2, line->colorFore, line->colorBack);
        }
            break;
        case APPLIB_GRAPHIC_CNT_RECT:
        {
            APPLIB_GRAPHIC_DESC_RECT_t *rect = (APPLIB_GRAPHIC_DESC_RECT_t *)cnt;
            appgoj_print("RECT:(%d, %d) 2(%d, %d) c(%d, %d)", rect->x1, rect->y1, rect->x2, rect->y2, rect->colorFore, rect->colorBack);
        }
            break;
        case APPLIB_GRAPHIC_CNT_CIRCLE:
        {
            APPLIB_GRAPHIC_DESC_CIRCLE_t *c = (APPLIB_GRAPHIC_DESC_CIRCLE_t *)cnt;
            appgoj_print("CIRCLE:(%d, %d) r(%d    ) c(%d, %d)", c->centerX, c->centerY, c->radius, c->colorFore, c->colorBack);
        }
            break;
        case APPLIB_GRAPHIC_CNT_ELLIPSE:
        {
            APPLIB_GRAPHIC_DESC_ELLIPSE_t *e = (APPLIB_GRAPHIC_DESC_ELLIPSE_t *)cnt;
            appgoj_print("ELLIPSE:(%d, %d) r(%d, %d) c(%d, %d)", e->centerX, e->centerY, e->radiusH, e->radiusV, e->colorFore, e->colorBack);
        }
            break;
        case APPLIB_GRAPHIC_CNT_BMP:
        {
            APPLIB_GRAPHIC_DESC_BMP_t *bmp = (APPLIB_GRAPHIC_DESC_BMP_t *)cnt;
            appgoj_print("BMP:(%d, %d) w(%d, %d) flg:%d, bits:%d, pxf:0x%x, tc:%0xx, ptr:0x%x",
                    bmp->x1, bmp->y1, bmp->width, bmp->height, bmp->flags, bmp->bits, bmp->pxf, bmp->tcolor, bmp->ptr);
        }
            break;
        case APPLIB_GRAPHIC_CNT_YUV:
        {
            //APPLIB_GRAPHIC_DESC_YUV_t *line = &(tmpObj->objAttr.cnt);
            appgoj_print("YUV:YUV type, not ready");
        }
            break;
        case APPLIB_GRAPHIC_CNT_STRING:
        {
            APPLIB_GRAPHIC_DESC_STR_t *s = (APPLIB_GRAPHIC_DESC_STR_t *)cnt;
            appgoj_print("draw STR:(%d, %d) h(%d   ) c(%d, %d) str:0x%x num:%d",
                    s->x1, s->y1, s->sizeHeight, s->colorFore, s->colorBack, s->str, s->CharNum);
        }
            break;

    }
}


int dumpGobjList(APPLIB_CANVAS_t *targetCanvas)
{
    APPLIB_GRAPHIC_OBJ_LIST_t *tmpObj;
    if (targetCanvas != NULL) {
        appgoj_print("Head:0x%x, Tail:0x%x, num:%d", targetCanvas->GobjListHead, targetCanvas->GobjListTail, targetCanvas->GobjNum);
        tmpObj = targetCanvas->GobjListHead;
        if (tmpObj != NULL) {
            do {
                //appgoj_print("ID:%d, v:%d, pre:0x%x, next:0x%x, x:%d, y:%d, s:%d, l:%d, g:%d, a:%d, t:0x%x",
                //        tmpObj->objID, tmpObj->stat, tmpObj->prevObj, tmpObj->nextObj,
                //        tmpObj->objAttr.area.X, tmpObj->objAttr.area.Y, tmpObj->objAttr.show, tmpObj->objAttr.layer,
                //        tmpObj->objAttr.group, tmpObj->objAttr.alpha, tmpObj->objAttr.type);
                appgoj_print("ID:%d", tmpObj->objID);
                dumpGobj(&tmpObj->objAttr);
                tmpObj = tmpObj->nextObj;
            } while (tmpObj != NULL);
        }
    } else {
        appgoj_print("dumpGobjList(): wrong targetCanvas: 0x%x", targetCanvas);
        return -1;
    }
    return -1;
}

int AppLibGraphicObj_Draw(APPLIB_GRAPHIC_RENDER_s *render, APPLIB_GRAPHIC_OBJ_s *obj)
{
    if (render != NULL) {
        if (obj != NULL) {
            switch (obj->type) {
                case APPLIB_GRAPHIC_CNT_LINE:
                {
                    APPLIB_GRAPHIC_DESC_LINE_t *line = (APPLIB_GRAPHIC_DESC_LINE_t *)&(obj->cnt);
                    appgoj_print("draw line:(%d, %d) 2(%d, %d) t(%d) c(%d, %d)", line->x1, line->y1, line->x2, line->y2, line->thickness, line->colorFore, line->colorBack);
                    AmpGraphics_DrawLine(render, line->x1, line->y1, line->x2, line->y2, line->thickness, line->colorFore);
                }
                    break;
                case APPLIB_GRAPHIC_CNT_RECT:
                {
                    APPLIB_GRAPHIC_DESC_RECT_t *rect = (APPLIB_GRAPHIC_DESC_RECT_t *)&(obj->cnt);
                    appgoj_print("draw rect:(%d, %d) 2(%d, %d) t(%d) c(%d, %d)", rect->x1, rect->y1, rect->x2, rect->y2, rect->thickness, rect->colorFore, rect->colorBack);
                    AmpGraphics_DrawRect(render, rect->x1, rect->y1, rect->x2, rect->y2, rect->thickness, rect->colorFore);
                }
                    break;
                case APPLIB_GRAPHIC_CNT_CIRCLE:
                {
                    AMP_AREA_s calcedArea = {0, 0, 0, 0};
                    APPLIB_GRAPHIC_DESC_CIRCLE_t *c = (APPLIB_GRAPHIC_DESC_CIRCLE_t *)&(obj->cnt);
                    appgoj_print("draw circle:(%d, %d) r(%d) t(%d) c(%d, %d)", c->centerX, c->centerY, c->radius, c->thickness, c->colorFore, c->colorBack);
                    calcedArea.X = c->centerX - c->radius;
                    calcedArea.Y = c->centerY - c->radius;
                    calcedArea.Width  = c->centerX + c->radius;
                    calcedArea.Height = c->centerY + c->radius;
                    AmpGraphics_DrawCircle(render, c->centerX, c->centerY, c->radius, c->thickness, c->colorFore, &calcedArea);
                }
                    break;
                case APPLIB_GRAPHIC_CNT_ELLIPSE:
                {
                    APPLIB_GRAPHIC_DESC_ELLIPSE_t *e = (APPLIB_GRAPHIC_DESC_ELLIPSE_t *)&(obj->cnt);
                    appgoj_print("draw ellipse:(%d, %d) r(%d, %d) c(%d, %d)", e->centerX, e->centerY, e->radiusH, e->radiusV, e->colorFore, e->colorBack);
                }
                    break;
                case APPLIB_GRAPHIC_CNT_BMP:
                {
                    APPLIB_GRAPHIC_DESC_BMP_t *bmp = (APPLIB_GRAPHIC_DESC_BMP_t *)&(obj->cnt);
                    appgoj_print("draw BMP:(%d, %d) w(%d, %d) flg:%d, bits:%d, pxf:0x%x, tc:%0xx, ptr:0x%x",
                            bmp->x1, bmp->y1, bmp->width, bmp->height, bmp->flags, bmp->bits, bmp->pxf, bmp->tcolor, bmp->ptr);
                }
                    break;
                case APPLIB_GRAPHIC_CNT_YUV:
                {
                    //APPLIB_GRAPHIC_DESC_YUV_t *line = &(tmpObj->objAttr.cnt);
                    appgoj_print("draw yuv:YUV type, not ready");
                }
                    break;
                case APPLIB_GRAPHIC_CNT_STRING:
                {
                    APPLIB_GRAPHIC_DESC_STR_t *s = (APPLIB_GRAPHIC_DESC_STR_t *)&(obj->cnt);
                    appgoj_print("draw str:(%d, %d) h(%d   ) c(%d, %d) str:0x%x num:%d fonts:0x%x",
                            s->x1, s->y1, s->sizeHeight, s->colorFore, s->colorBack, s->str, s->CharNum, s->fontAttr);
                    AmpGraphics_PutStr(render, &s->fontAttr, s->x1, s->y1, s->sizeHeight, s->str, s->colorFore, s->colorBack, &obj->area, 0);
                }
                    break;

            }
        } else {
            appgoj_print("drawGobj():wrong obj: 0x%x", obj);
        }
    } else {
        appgoj_print("drawGobj():wrong render: 0x%x", render);
    }
    return AMP_OK;
}


/**
 * Add a graphic obj into a graphic list. *
 * Initialize object member, obj-list operation.
 *
 * @param[in] *targetCanvas - The Canvas is going to add Gobj.
 * @param[in] type - Gobj type.
 * @param[in] cnt - Gobj content.
 * @param[in] alpha - Gobj alpha.
 * @param[in] layer - Which layer this Gobj belongs to.
 * @param[in] group - Gobj group.
 * @param[in] show - Default showing status of this Gobj.
 *
 * @return APPLIB_CANVAS_GRAPHIC_OBJ_ID_t
 */
APPLIB_GRAPHIC_OBJ_ID_t AppLibGraphicObjList_Add(APPLIB_CANVAS_t *targetCanvas,
                                                        const APPLIB_GRAPHIC_CNT_e type,
                                                        const APPLIB_GRAPHIC_DESC_u cnt,
                                                        UINT32 talpha,
                                                        UINT32 layer,
                                                        UINT32 group,
                                                        UINT8 show)
{
    APPLIB_GRAPHIC_OBJ_LIST_t *ObjArr = targetCanvas->CanvasCacheBaseAddr;
    if (targetCanvas != NULL) {
        if ((targetCanvas->GobjNum != targetCanvas->GobjNumMax) &&
            (targetCanvas->CanvasCacheBaseAddr != NULL)) {
            /*memset(&ObjArr[targetCanvas->GobjIDmax], 0, sizeof(APPLIB_GRAPHIC_OBJ_LIST_t));
            appgoj_print("add new GObj at 0x%x", &ObjArr[targetCanvas->GobjIDmax]);
            ObjArr[targetCanvas->GobjIDmax].objID = targetCanvas->GobjIDmax;
            ObjArr[targetCanvas->GobjIDmax].objAttr.type = type;
            ObjArr[targetCanvas->GobjIDmax].objAttr.cnt = cnt;
            calcArea(targetCanvas, &(ObjArr[targetCanvas->GobjIDmax].objAttr));
            ObjArr[targetCanvas->GobjIDmax].objAttr.alpha = talpha;
            ObjArr[targetCanvas->GobjIDmax].objAttr.layer = layer;
            ObjArr[targetCanvas->GobjIDmax].objAttr.group = group;
            ObjArr[targetCanvas->GobjIDmax].objAttr.show = show;*/
            //find empty slot in CanvasCache
            int idx;
            for (idx=targetCanvas->GobjIDmax; idx<targetCanvas->GobjIDmax; idx++) {
                idx = idx % targetCanvas->GobjNumMax;
                if (ObjArr[idx].stat == OBJ_STAT_INVALID)
                    break;
            }
            memset(&ObjArr[idx], 0, sizeof(APPLIB_GRAPHIC_OBJ_LIST_t));
            appgoj_print("add new GObj idx:%d at 0x%x", idx, &ObjArr[idx]);
            ObjArr[idx].stat = OBJ_STAT_UPDATE;
            ObjArr[idx].objID = targetCanvas->GobjIDmax;
            ObjArr[idx].objAttr.type = type;
            ObjArr[idx].objAttr.cnt = cnt;
            calcArea(targetCanvas, &(ObjArr[idx].objAttr));
            ObjArr[idx].objAttr.alpha = talpha;
            ObjArr[idx].objAttr.layer = layer;
            ObjArr[idx].objAttr.group = group;
            ObjArr[idx].objAttr.show = show;
            dumpGobj(&ObjArr[idx].objAttr);
            if (insertGobjInList(targetCanvas, &ObjArr[idx]) == 0) {
                appgoj_print("add new GObj success", &ObjArr[idx]);
                targetCanvas->GobjIDmax += 1;
                targetCanvas->GobjNum += 1;
                return ObjArr[idx].objID;
            } else {
                appgoj_print("add new GObj fail");
                appgoj_perror(0, AMP_ERROR_GENERAL_ERROR, "insertGobjInList(): return -1");
                return -1;
            }
        } else {
            appgoj_perror(0, AMP_ERROR_RESOURCE_INVALID, "targetCanvas number of Gobj reaches the max.");
            return -1;
        }
    } else {
        appgoj_perror(0, AMP_ERROR_RESOURCE_INVALID, "wrong targetCanvas.");
        return -1;
    }
}



/**
 * Delete a graphic obj from a graphic list. *
 *
 * @param[in] *targetCanvas - The Canvas is going to add Gobj.
 * @param[in] targetID - Gobj ID is going to delete.
 *
 * @return APPLIB_CANVAS_GRAPHIC_OBJ_ID_t
 */
int AppLibGraphicObjList_Delete(APPLIB_CANVAS_t *targetCanvas,
                                   APPLIB_GRAPHIC_OBJ_ID_t targetID)
{
    APPLIB_GRAPHIC_OBJ_LIST_t *deleteObj;
    if (targetCanvas != NULL) {
        findGobjinCanvas(targetCanvas, targetID, &deleteObj);
        if (deleteObj != NULL) {
            //mark valid as going_to_delete
            deleteObj->stat = OBJ_STAT_DELETE;
            /* move to draw*/
            appgoj_print("delete GObj at 0x%x", deleteObj);
            /*
            if (targetCanvas->GobjListTail == targetCanvas->GobjListHead) {
                targetCanvas->GobjListHead = targetCanvas->GobjListTail = NULL;
                appgoj_print("delete Head=Tail");
            } else if (deleteObj == targetCanvas->GobjListHead) {
                targetCanvas->GobjListHead = deleteObj->nextObj;
                appgoj_print("delete Head");
            } else if (deleteObj == targetCanvas->GobjListTail) {
                targetCanvas->GobjListTail = deleteObj->prevObj;
                appgoj_print("delete Tail");
            }
            deleteObj->prevObj->nextObj = deleteObj->nextObj;
            deleteObj->nextObj->prevObj = deleteObj->prevObj;
            targetCanvas->GobjNum -= 1;
            memset(deleteObj, 0, sizeof(APPLIB_GRAPHIC_OBJ_LIST_t));
            return AMP_OK;*/
        }
    } else {
        appgoj_perror(0, AMP_ERROR_RESOURCE_INVALID, "wrong targetCanvas.");
        return AMP_ERROR_RESOURCE_INVALID;
    }
    return AMP_ERROR_GENERAL_ERROR;
}



/**
 * Update a graphic obj from a graphic list. *
 *
 * @param[in] *targetCanvas - The Canvas is going to add Gobj.
 * @param[in] targetID - Gobj ID is going to update.
 *
 * @return APPLIB_CANVAS_GRAPHIC_OBJ_ID_t
 */
int AppLibGraphicObjList_Update(APPLIB_CANVAS_t *targetCanvas,
                                   APPLIB_GRAPHIC_OBJ_ID_t targetGID,
                                   const APPLIB_GRAPHIC_CNT_e type,
                                   const APPLIB_GRAPHIC_DESC_u cnt,
                                   UINT32 talpha,
                                   UINT32 layer,
                                   UINT32 group,
                                   UINT8 show)
{
    APPLIB_GRAPHIC_OBJ_LIST_t *targetObj;
    if (targetCanvas != NULL) {
        findGobjinCanvas(targetCanvas, targetGID, &targetObj);
        if (targetObj != NULL) {
            //mark stat as APPLIB_GRAPHIC_STAT_UPDATE
            if (targetObj->stat == OBJ_STAT_NORMAL) {
                targetObj->stat = OBJ_STAT_UPDATING;
            } else {
                //TODO:wait until normal
                targetObj->stat = OBJ_STAT_UPDATING;
            }
            //update content
            appgoj_print("update GObj at 0x%x", targetObj);
            targetObj->objAttr.type = type;
            targetObj->objAttr.cnt = cnt;
            calcArea(targetCanvas, &(targetObj->objAttr));
            targetObj->objAttr.alpha = talpha;
            //TODO: multi layer support
            //if (layer != targetObj->objAttr.layer) {
                //adjust link list
            targetObj->objAttr.layer = layer;
            targetObj->objAttr.group = group;
            targetObj->objAttr.show = show;
            targetObj->stat = OBJ_STAT_UPDATE;
            return AMP_OK;
        }
    } else {
        appgoj_perror(0, AMP_ERROR_RESOURCE_INVALID, "wrong targetCanvas.");
        return AMP_ERROR_RESOURCE_INVALID;
    }
    return AMP_ERROR_GENERAL_ERROR;
}
#endif
