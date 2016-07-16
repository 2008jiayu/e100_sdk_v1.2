/**
  * @file applib/src/graphics/canvas/ApplibGraphics_Canvas.h
  *
  * ApplibGraphics_Canvas include Amba BMP font related
  *
  * History:
  *    2013/08/13 - [Hsunying Hunag] created file
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

#include "canvas.h"

static void findObjinCanvas(APPLIB_CANVAS_s *targetCanvas,
                             APPLIB_GRAPHIC_OBJ_ID_t targetID,
                             APPLIB_GRAPHIC_OBJ_LIST_s **dstObj)
{
    APPLIB_GRAPHIC_OBJ_LIST_s *ObjArr = targetCanvas->CanvasCacheBaseAddr;
    UINT32 idx = 0;

    for (; idx <= targetCanvas->ObjIDmax; idx++) {
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "ObjArr[idx].Attr.ID %d, targetID %d", ObjArr[idx].Attr.ID, targetID);
        if (ObjArr[idx].Attr.ID == targetID) {
            *dstObj = &ObjArr[idx];
            return;
        }
    }
    //not found
    GraphicsPrint(DEBUG_WARNING, DEBUG_MODULE_CANVAS, "cannot find target Obj.");
    *dstObj = NULL;
}

//static APPLIB_CANVAS_GRAPHIC_OBJ_LIST_t* buildDrawList(APPLIB_CANVAS_t *targetCanvas)
static void buildDrawList(APPLIB_CANVAS_s *targetCanvas, APPLIB_GRAPHIC_OBJ_LIST_s **idstObj)
{
    APPLIB_GRAPHIC_OBJ_LIST_s *dstObj;
    dstObj = targetCanvas->ObjListHead;
    while (dstObj != NULL) {
        //check for going to delete
        //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "dstObj:0x%x",dstObj);
        if (dstObj->Attr.Stat == OBJ_STAT_DELETE) {
            //fill a rectangle in Area
            //TODO: call obj delete function to fill the rectangle
            extern int AppLibRect_Render(APPLIB_GRAPHIC_RENDER_s *render,
                                            UINT32 x1,
                                            UINT32 y1,
                                            UINT32 x2,
                                            UINT32 y2,
                                            INT32 thickness,
                                            UINT32 color);
            AppLibRect_Render( targetCanvas->Render, dstObj->Attr.LastDisplayBox.X, dstObj->Attr.LastDisplayBox.Y,
                               dstObj->Attr.LastDisplayBox.X + dstObj->Attr.LastDisplayBox.Width, dstObj->Attr.LastDisplayBox.Y + dstObj->Attr.LastDisplayBox.Height,
                               0, 0/*rgetCanvas->Backcolor*/);

            if (dstObj == targetCanvas->ObjListHead) {
                targetCanvas->ObjListHead = dstObj->NextObj;
            }
            if (dstObj == targetCanvas->ObjListTail) {
                targetCanvas->ObjListTail = dstObj->PrevObj;
            }
            if (targetCanvas->ObjListTail == targetCanvas->ObjListHead) {
                targetCanvas->ObjListHead = targetCanvas->ObjListTail = NULL;
            }
            dstObj->PrevObj->NextObj = dstObj->NextObj;
            dstObj->NextObj->PrevObj = dstObj->PrevObj;
            targetCanvas->ObjNum -= 1;
            //memset(dstObj, 0, sizeof(APPLIB_GRAPHIC_OBJ_LIST_t));
            dstObj->Attr.Stat = OBJ_STAT_INVALID;
        //TODO: setting hide
        } else if ( (dstObj->Attr.Stat == OBJ_STAT_HIDED) || (dstObj->Attr.Stat == OBJ_STAT_UPDATE)) {
            //fill a rectangle in Area
            //TODO: call obj delete function to fill the rectangle
            extern int AppLibRect_Render(APPLIB_GRAPHIC_RENDER_s *render,
                                            UINT32 x1,
                                            UINT32 y1,
                                            UINT32 x2,
                                            UINT32 y2,
                                            INT32 thickness,
                                            UINT32 color);
            AppLibRect_Render( targetCanvas->Render, dstObj->Attr.LastDisplayBox.X, dstObj->Attr.LastDisplayBox.Y,
                               dstObj->Attr.LastDisplayBox.X + dstObj->Attr.LastDisplayBox.Width, dstObj->Attr.LastDisplayBox.Y + dstObj->Attr.LastDisplayBox.Height,
                               0, 0/*rgetCanvas->Backcolor*/);

            dstObj->Attr.Stat = OBJ_STAT_NORMAL;
        }

        //TODO find draw head
        //if (some cond) {
        //   return dstObj as draw head
        //}
        dstObj = dstObj->NextObj;
    }
    *idstObj = targetCanvas->ObjListHead;
}

/*
static void findObjinList(APPLIB_CANVAS_t *targetCanvas,
                             APPLIB_GRAPHIC_OBJ_ID_t targetID,
                             APPLIB_GRAPHIC_OBJ_LIST_t **dstObj)
{
    *dstObj = targetCanvas->ObjListHead;
    do {
        if ((*dstObj)->objID == targetID) {
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "obj found at 0x%x", *dstObj);
            return;
        }
        *dstObj = (*dstObj)->NextObj;
    } while (*dstObj != NULL);
    //not found
    GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "cannot find target Obj.");
    *dstObj = NULL;
}*/

static int insertObjInList(APPLIB_CANVAS_s *targetCanvas,
                           APPLIB_GRAPHIC_OBJ_LIST_s *targetObj,
                           APPLIB_GRAPHIC_OBJ_s *newObj)
{
    if (targetObj->Attr.Layer > 0) {
        //TODO multi layer support
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "Doesn't support multiple layer yet");
        return -1;
    } else {
        if (targetCanvas->ObjListHead == NULL) {
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "CASE 1");
            targetCanvas->ObjListHead = targetObj;
            targetCanvas->ObjListTail = targetObj;
            targetObj->NextObj = NULL;
            targetObj->PrevObj = NULL;
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "New insert, set ObjHead to:0x%x", targetObj);
        } else {
            //GraphicsPrint(DEBUG_ONLY, "head layer %d, tail layer %d, target layer %d",
            //       targetCanvas->ObjListHead->Attr.Layer,
            //       targetCanvas->ObjListTail->Attr.Layer,
            //       newObj->Layer);
            if ( targetCanvas->ObjListTail->Attr.Layer <= newObj->Layer ) {
                GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "CASE 2");
                //GraphicsPrint(DEBUG_ONLY, "insert, h:0x%x, t:0x%x", targetCanvas->ObjListHead, targetCanvas->ObjListTail);
                targetObj->NextObj = NULL;
                targetObj->PrevObj = targetCanvas->ObjListTail;
                targetCanvas->ObjListTail->NextObj = targetObj;
                targetCanvas->ObjListTail = targetObj;
            } else if (newObj->Layer < targetCanvas->ObjListHead->Attr.Layer) {
                GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "CASE 3");
                //GraphicsPrint(DEBUG_ONLY, "insert, h:0x%x, t:0x%x", targetCanvas->ObjListHead, targetCanvas->ObjListTail);
                targetObj->NextObj = targetCanvas->ObjListHead;
                targetObj->PrevObj = NULL;
                targetCanvas->ObjListHead->PrevObj = targetObj;
                targetCanvas->ObjListHead = targetObj;
            } else {
                APPLIB_GRAPHIC_OBJ_LIST_s *tmp = targetCanvas->ObjListTail->PrevObj;
                GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "CASE 4");
                while ( tmp ) {
                    if (tmp->Attr.Layer <= newObj->Layer) {
                        targetObj->NextObj = tmp->NextObj;
                        targetObj->PrevObj = tmp;
                        tmp->NextObj->PrevObj = targetObj;
                        tmp->NextObj = targetObj;
                        break;
                    }
                    tmp = tmp->PrevObj;
                }
            }
        }
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "insert, h:0x%x, t:0x%x", targetCanvas->ObjListHead, targetCanvas->ObjListTail);
        return 0;
    }
}

void CanvasDump(APPLIB_CANVAS_s *targetCanvas)
{
    if (targetCanvas != NULL) {
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "-----------------Canvas Dump-------------------");
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "Area.", targetCanvas->Area.X, targetCanvas->Area.Y, targetCanvas->Area.Width, targetCanvas->Area.Height);
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "-----------------------------------------------");
    } else {
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "AppLibCanvas_Show(): Invalid targetCanvas:0x%x", targetCanvas);
    }
}

/*************************************************************************
 * Canvas Interface Functions
 ************************************************************************/
/**
 *  @brief The default configures of canvas.
 *
 *  Get default canvas configuration setting for create.
 *
 *  @param [out] CanvasCfg An allocated empty APPLIB_CANVAS_CFG_t instance.
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibCanvas_GetDefCfg(APPLIB_CANVAS_CFG_s *CanvasCfg)
{
    if (CanvasCfg != NULL) {
        CanvasCfg->Area.X = 0;
        CanvasCfg->Area.Y = 0;
        CanvasCfg->Area.Width = 960;
        CanvasCfg->Area.Height = 480;
        CanvasCfg->Alpha = 1;
        CanvasCfg->Backcolor = 0xFFFFFFFF;
        CanvasCfg->ObjNumMax = 512;
        CanvasCfg->ObjPartEna = 0;
        CanvasCfg->ObjPartCol = 0;
        CanvasCfg->ObjPartRow = 0;
        CanvasCfg->LayerMax = 8;
    } else {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "AppLibCanvas_GetDefConfig error, CanvasCfg:0x%x invalid", CanvasCfg);
        return AMP_ERROR_INCORRECT_PARAM_STRUCTURE;
    }
    return AMP_OK;
}

/**
 *  @brief Calculate needed memory space for a Canvas obj.
 *
 *  Calculate needed memory space for a Canvas obj.
 *
 *  @param [in] CanvasCfg Canvas configuration to calculatee.
 *  @param [out] BufSz Result buffer size.
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibCanvas_CalMemSize(APPLIB_CANVAS_CFG_s *CanvasCfg, UINT32 *BufSz)
{
    UINT32 preserve = 1024;

    if ( !CanvasCfg ) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "AppLibCanvas_CalMemSize error, CanvasCfg:0x%x invalid", CanvasCfg);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if ( (CanvasCfg->ObjNumMax == 0) || (CanvasCfg->Area.Width == 0) || (CanvasCfg->Area.Height == 0) ) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "AppLibCanvas_CalMemSize error, ObjNumMax: %d, Width:%d invalid",
                   CanvasCfg->ObjNumMax, CanvasCfg->Area.Width, CanvasCfg->Area.Height);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    *BufSz = CanvasCfg->ObjNumMax*sizeof(APPLIB_GRAPHIC_OBJ_LIST_s) + preserve;
    //TODO add layer head pointer array

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "AppLibCanvas_CalMemSize end, BufSz:0x%x", *BufSz);
    return AMP_OK;
}

/**
 *  @brief Create a canvas obj to manage graphics.
 *
 *  Create a canvas obj to manage graphics.
 *  Initialize object member, set variables.
 *
 *  @param [in] newCanvas An allocated empty APPLIB_CANVAS_t instance.
 *  @param [in] canvasCfg An APPLIB_CANVAS_CFG_s instance for create Canvas.
 *  @param [in] render The render of target OSD buffer.
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibCanvas_Create(APPLIB_CANVAS_s *newCanvas,
                           APPLIB_CANVAS_CFG_s *canvasCfg,
                           APPLIB_GRAPHIC_RENDER_s *render)
{
    if ( (!newCanvas) || (!canvasCfg) || (!render)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "[AppLib Cvs][create] error, newCanvas:0x%x, canvasCfg:0x%x, render:0x%x invalid", newCanvas, canvasCfg, render);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (canvasCfg->ObjNumMax == 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "[AppLib Cvs][create] error, ObjNumMax:%d invalid", canvasCfg->ObjNumMax);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if ( (canvasCfg->Area.Width == 0) || (canvasCfg->Area.Height == 0) ) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "[AppLib Cvs][create] error, Width:%d, Height:%d invalid", canvasCfg->Area.Width, canvasCfg->Area.Height);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if ( (!canvasCfg->CanvasCacheBaseAddr) || (canvasCfg->CanvasCacheSize == 0) ) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "[AppLib Cvs][create] error, CanvasCacheBaseAddr:0x%x, CanvasCacheSize:0x%x invalid", canvasCfg->CanvasCacheBaseAddr, canvasCfg->CanvasCacheSize);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    newCanvas->Area.X = canvasCfg->Area.X;
    newCanvas->Area.Y = canvasCfg->Area.Y;
    newCanvas->Area.Width = canvasCfg->Area.Width;
    newCanvas->Area.Height = canvasCfg->Area.Height;
    newCanvas->Alpha = canvasCfg->Alpha;
    newCanvas->Backcolor = canvasCfg->Backcolor;
    newCanvas->Render = render;
    newCanvas->CanvasCacheBaseAddr = canvasCfg->CanvasCacheBaseAddr;
    newCanvas->CanvasCacheSize = canvasCfg->CanvasCacheSize;
    newCanvas->ObjNum = 0;
    newCanvas->ObjNumMax = canvasCfg->ObjNumMax;
    newCanvas->ObjPartCol = 0;
    newCanvas->ObjPartRow = 0;
    newCanvas->ObjIDmax = 0;
    newCanvas->ObjListHead = NULL;
    newCanvas->ObjListTail = NULL;
    newCanvas->ObjAdd_f = AppLibGraphicObjList_Add;
    newCanvas->ObjDelete_f = AppLibGraphicObjList_Delete;
    newCanvas->ObjUpdate_f = AppLibGraphicObjList_Update;
    newCanvas->ObjQuery_f = AppLibGraphicObjList_Query;
    newCanvas->ObjSetShow_f = AppLibGraphicObj_SetShow;
    newCanvas->ObjHideAll_f = AppLibGraphicObj_HideAll;
    newCanvas->CanvasUpdate_f = AppLibCanvas_Update;
    newCanvas->CanvasDraw_f = AppLibCanvas_Draw;
    newCanvas->CanvasDelete_f = AppLibCanvas_Delete;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "AppLibCanvas_CalMemSize end");
    return AMP_OK;
}

/**
 *  @brief Update Canvas attribute.
 *
 *  Update objects' settings in canvas.
 *
 *  @param [in] targetCanvas Target operate canvas.
 *  @param [in] canvasCfg An APPLIB_CANVAS_CFG_s instance for updating.
 *  @param [in] newRender The new render of target OSD buffer, can be NULL.
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibCanvas_Update(APPLIB_CANVAS_s *targetCanvas,
                        const APPLIB_CANVAS_CFG_s *canvasCfg,
                        APPLIB_GRAPHIC_RENDER_s *newRender)
{
    GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "AppLibCanvas_Update, Not implement yet.");

    return AMP_ERROR_GENERAL_ERROR;
}

/**
 *  @brief Delete Canvas attribute.
 *
 *  Delete the specific canvas.
 *
 *  @param [in] *targetCanvas Target operate canvas.
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibCanvas_Delete(APPLIB_CANVAS_s *targetCanvas)
{
    if (targetCanvas != NULL) {
        memset(targetCanvas->CanvasCacheBaseAddr, 0x0, targetCanvas->CanvasCacheSize);
        targetCanvas->CanvasCacheSize = 0;
        targetCanvas->CanvasCacheBaseAddr = 0x0;
        targetCanvas->CanvasDelete_f = NULL;
        targetCanvas->CanvasDraw_f = NULL;
        targetCanvas->CanvasUpdate_f = NULL;
        targetCanvas->ObjAdd_f = NULL;
        targetCanvas->ObjDelete_f = NULL;
        targetCanvas->ObjUpdate_f = NULL;
        targetCanvas->ObjSetShow_f = NULL;
        targetCanvas->ObjHideAll_f = NULL;

        targetCanvas->ObjIDmax = 0;
        targetCanvas->ObjListHead = NULL;
        targetCanvas->ObjListTail = NULL;
        targetCanvas->ObjNum = 0;
        targetCanvas->ObjNumMax = 0;
        targetCanvas->ObjPartCol = 0;
        targetCanvas->ObjPartRow = 0;

        targetCanvas->Alpha = 0;
        targetCanvas->Area.Height = 0;
        targetCanvas->Area.Width = 0;
        targetCanvas->Area.X = 0;
        targetCanvas->Area.Y = 0;
        targetCanvas->Render = NULL;

        return AMP_OK;
    } else {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "AppLibCanvas_Delete(): Invalid targetCanvas:0x%x", targetCanvas);
        return AMP_ERROR_INCORRECT_PARAM_STRUCTURE;
    }

}

/**
 *  @brief Draw Canvas attribute.
 *
 *  Draw the specific canvas.
 *
 *  @param [in] *targetCanvas Target operate canvas.
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibCanvas_Draw(APPLIB_CANVAS_s *targetCanvas)
{
    APPLIB_GRAPHIC_OBJ_LIST_s *drawHead = NULL;

    /* Error check */
    if (targetCanvas == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "AppLibCanvas_Draw(): Invalid targetCanvas:0x%x", targetCanvas);
        return AMP_ERROR_INCORRECT_PARAM_STRUCTURE;
    }

    /* Draw */
    buildDrawList(targetCanvas, &drawHead);
    while (drawHead != NULL) {
        //TODO: setting hide
        if (drawHead->Attr.Show == 1) {
            switch (drawHead->Attr.Stat) {
                case OBJ_STAT_UPDATE:
                case OBJ_STAT_NORMAL:
                    drawHead->Attr.Stat = OBJ_STAT_DRAM;
                    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "AppLibCanvas_Draw(): draw:%d 0x%x", drawHead->Attr.ID, drawHead);
                    if (drawHead->Attr.Draw_f) {
                        drawHead->Attr.Draw_f(targetCanvas->Render, &drawHead->Attr.DisplayBox, &drawHead->Attr);
                    }
                    drawHead->Attr.Stat = OBJ_STAT_NORMAL;
                    break;
                default:
                    break;
            }
        }
        drawHead = drawHead->NextObj;
    }
    return AMP_OK;
}

/**
 *  @brief Add a graphic obj into a graphic list.
 *
 *  Initialize object member, obj-list operation.
 *  targetCanvas will copy all input attribute as an new Obj in canvas itself.
 *
 *  @param [in] *targetCanvas Target operate canvas.
 *  @param [in] newObj Obj going to add.
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibGraphicObjList_Add(APPLIB_CANVAS_s *targetCanvas,
                             APPLIB_GRAPHIC_OBJ_s *newObj)
{
    APPLIB_GRAPHIC_OBJ_LIST_s *ObjArr = NULL;
    int idx = 0;

    if ((!targetCanvas) || (!newObj)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "wrong targetCanvas.");
        return -1;
    }
    if ((!targetCanvas->CanvasCacheBaseAddr) || (targetCanvas->ObjNum == targetCanvas->ObjNumMax)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "targetCanvas number of Obj reaches the max.");
        return -1;
    }

    /* Init */
    ObjArr = targetCanvas->CanvasCacheBaseAddr;

    //find empty slot in CanvasCache
    for (idx=targetCanvas->ObjIDmax; idx<targetCanvas->ObjNumMax; idx++) {
        idx = idx % targetCanvas->ObjNumMax;
        if (ObjArr[idx].Attr.Stat == OBJ_STAT_INVALID)
            break;
    }

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "add new Obj idx:%d at 0x%x", idx, &ObjArr[idx]);
    memset(&ObjArr[idx], 0, sizeof(APPLIB_GRAPHIC_OBJ_LIST_s));
    if (insertObjInList(targetCanvas, &ObjArr[idx], newObj) == 0) {
        //TODO: mutex_impl
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "Area.X:%4d Area.Y:%4d Area.Width:%4d Area.Height:%4d Alpha:0x%x Layer:%d Group:%d Show:%d",
                newObj->DisplayBox.X, newObj->DisplayBox.Y, newObj->DisplayBox.Width, newObj->DisplayBox.Height,
                newObj->AlphaTable, newObj->Layer, newObj->Group, newObj->Show);
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "Stat:%d Contex:0x%x CalcArea_f:0x%x Dump_f:0x%x Draw_f:0x%x",
                newObj->Stat, (UINT8 *)newObj->Content,
                newObj->CalcArea_f, newObj->Dump_f, newObj->Draw_f);
        memcpy(&ObjArr[idx], newObj, sizeof(APPLIB_GRAPHIC_OBJ_s));
        if (AmbaKAL_MutexCreate(&ObjArr[idx].Attr.Mutex) != OK) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "AppLibCirc_CreateObj Create Mutex error.");
            return AMP_ERROR_RESOURCE_INVALID;
        }

        ObjArr[idx].Attr.ID = targetCanvas->ObjIDmax;
        ObjArr[idx].Attr.Stat = OBJ_STAT_UPDATE;
        ObjArr[idx].Attr.CalcArea_f(&ObjArr[idx].Attr);
        //ObjArr[idx].Attr.Dump_f(&ObjArr[idx].Attr);
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "Area.X:%4d Area.Y:%4d Area.Width:%4d Area.Height:%4d",
                ObjArr[idx].Attr.DisplayBox.X, ObjArr[idx].Attr.DisplayBox.Y, ObjArr[idx].Attr.DisplayBox.Width, ObjArr[idx].Attr.DisplayBox.Height);
        newObj->ID = targetCanvas->ObjIDmax;
        newObj->Exist = 1;
        targetCanvas->ObjIDmax += 1;
        targetCanvas->ObjNum += 1;
        return AMP_OK;
    } else {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "add new Obj fail");
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "insertObjInList(): return -1");
        return -1;
    }
}

/**
 * Delete a graphic obj from a graphic list. *
 *
 * @param[in] *targetCanvas - The Canvas is going to add Obj.
 * @param[in] targetID Obj ID is going to delete.
 *
 * @return int
 */
int AppLibGraphicObjList_Delete(APPLIB_CANVAS_s *targetCanvas,
                                   APPLIB_GRAPHIC_OBJ_ID_t targetID)
{
    APPLIB_GRAPHIC_OBJ_LIST_s *deleteObj;
    if (targetCanvas != NULL) {
        findObjinCanvas(targetCanvas, targetID, &deleteObj);
        if (deleteObj != NULL) {
            //mark valid as going_to_delete
            deleteObj->Attr.Stat = OBJ_STAT_DELETE;
            /* move to draw*/
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "delete GObj at 0x%x", deleteObj);
            /*
            if (targetCanvas->ObjListTail == targetCanvas->ObjListHead) {
                targetCanvas->ObjListHead = targetCanvas->ObjListTail = NULL;
                GraphicsPrint(DEBUG_ONLY, "delete Head=Tail");
            } else if (deleteObj == targetCanvas->ObjListHead) {
                targetCanvas->ObjListHead = deleteObj->NextObj;
                GraphicsPrint(DEBUG_ONLY, "delete Head");
            } else if (deleteObj == targetCanvas->ObjListTail) {
                targetCanvas->ObjListTail = deleteObj->PrevObj;
                GraphicsPrint(DEBUG_ONLY, "delete Tail");
            }
            deleteObj->PrevObj->NextObj = deleteObj->NextObj;
            deleteObj->NextObj->PrevObj = deleteObj->PrevObj;
            targetCanvas->ObjNum -= 1;
            memset(deleteObj, 0, sizeof(APPLIB_GRAPHIC_OBJ_LIST_t));
            return AMP_OK;*/
        }
    } else {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "wrong targetCanvas.");
        return AMP_ERROR_RESOURCE_INVALID;
    }
    return AMP_ERROR_GENERAL_ERROR;
}

/**
 *  @brief Update a graphic obj from a graphic list.
 *
 *  Update a graphic obj from a graphic list.
 *
 *  @param [in] *targetCanvas Target operate canvas.
 *  @param [in] targetID Obj ID is going to update.
 *  @param [in] newObj referent Obj.
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibGraphicObjList_Update(APPLIB_CANVAS_s *targetCanvas,
                                   const APPLIB_GRAPHIC_OBJ_ID_t targetID,
                                   const APPLIB_GRAPHIC_OBJ_s *newObj)
{
    APPLIB_GRAPHIC_OBJ_LIST_s *targetObj;
    if (targetCanvas != NULL) {
        findObjinCanvas(targetCanvas, targetID, &targetObj);
        if (targetObj != NULL) {
            //mark stat as APPLIB_GRAPHIC_STAT_UPDATE
            //TODO: mutex_impl
            if (targetObj->Attr.Stat == OBJ_STAT_NORMAL) {
                targetObj->Attr.Stat = OBJ_STAT_UPDATING;
            } else {
                //TODO:wait until normal
                targetObj->Attr.Stat = OBJ_STAT_UPDATING;
            }
            //update content
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "update GObj at %d (0x%x)", targetID, targetObj);
            targetObj->Attr.AlphaTable = newObj->AlphaTable;
            targetObj->Attr.Group = newObj->Group;
            targetObj->Attr.Content = newObj->Content;
            targetObj->Attr.CalcArea_f = newObj->CalcArea_f;
            targetObj->Attr.Dump_f = newObj->Dump_f;
            targetObj->Attr.Draw_f = newObj->Draw_f;
            //targetObj->Attr.Update_f = newObj->Update_f;
            targetObj->Attr.CalcArea_f(&targetObj->Attr);
            //TODO: multi layer support
            //if (layer != targetObj->objAttr.layer) {
                //adjust link list
            targetObj->Attr.Layer = newObj->Layer;
            targetObj->Attr.Stat = OBJ_STAT_UPDATE;
            //TODO: setting hide
            if ((targetObj->Attr.Show == 1) && (newObj->Show == 0))
                targetObj->Attr.Stat = OBJ_STAT_HIDED;
            targetObj->Attr.Show = newObj->Show;
            return AMP_OK;
        }
    } else {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "wrong targetCanvas.");
        return AMP_ERROR_RESOURCE_INVALID;
    }
    return AMP_ERROR_GENERAL_ERROR;
}

/**
 *  @brief Query a graphic obj from a graphic list.
 *
 *  Query a graphic obj from a graphic list.
 *
 *  @param [in] *targetCanvas Target operate canvas.
 *  @param [in] targetID Obj ID is going to update.
 *  @param [in] queryObj referent Obj.
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibGraphicObjList_Query( APPLIB_CANVAS_s *targetCanvas,
                                APPLIB_GRAPHIC_OBJ_ID_t targetID,
                                APPLIB_GRAPHIC_OBJ_s *queryObj)
{
    APPLIB_GRAPHIC_OBJ_LIST_s *targetObj = NULL;

    if ( !targetCanvas ) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "wrong targetCanvas.");
        return AMP_ERROR_RESOURCE_INVALID;
    }

    /* Init */
    findObjinCanvas(targetCanvas, targetID, &targetObj);

    /* Error Check */
    if ( !targetObj ) {
        // the target empty
        return AMP_ERROR_GENERAL_ERROR;
    }

    queryObj->LastDisplayBox = targetObj->Attr.LastDisplayBox;
    return AMP_OK;
}

/**
 *  @brief Set show or not for an graphic obj in canvas.
 *
 *  Set show or not for an graphic obj in canvas.
 *
 *  @param [in] *targetCanvas Target operate canvas.
 *  @param [in] targetID Obj ID is going to update.
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibGraphicObj_SetShow(APPLIB_CANVAS_s *targetCanvas,
                             APPLIB_GRAPHIC_OBJ_ID_t targetID)
{
    APPLIB_GRAPHIC_OBJ_LIST_s *targetObj;
    if (targetCanvas != NULL) {
        findObjinCanvas(targetCanvas, targetID, &targetObj);
        if (targetObj != NULL) {
            targetObj->Attr.Show = 1;
            /* move to draw*/
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "set Obj show at 0x%x", targetObj);
            return AMP_OK;
        }
    } else {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "wrong targetCanvas.");
        return AMP_ERROR_RESOURCE_INVALID;
    }
    return AMP_ERROR_GENERAL_ERROR;
}

/**
 *  @brief Hide all Objs in canvas.
 *
 *  Hide all Objs in canvas.
 *
 *  @param [in] *targetCanvas Target operate canvas.
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibGraphicObj_HideAll(APPLIB_CANVAS_s *targetCanvas)
{
    APPLIB_GRAPHIC_OBJ_LIST_s *targetObj;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "AppLibGraphicObj_HideAll start");
    if (targetCanvas != NULL) {
        targetObj = targetCanvas->ObjListHead;
        if (targetObj != NULL) {
            do {
                targetObj->Attr.Show = 0;
                targetObj->Attr.Stat = OBJ_STAT_HIDED;
                targetObj = targetObj->NextObj;
            } while (targetObj != NULL);
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "AppLibGraphicObj_HideAll end");
            return AMP_OK;
        }
    } else {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "wrong targetCanvas.");
        return AMP_ERROR_RESOURCE_INVALID;
    }
    return AMP_ERROR_GENERAL_ERROR;
}

int dumpObjList(APPLIB_CANVAS_s *targetCanvas)
{
    APPLIB_GRAPHIC_OBJ_LIST_s *tmpObj;
    if (targetCanvas == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_CANVAS, "dumpObjList(): wrong targetCanvas: 0x%x", targetCanvas);
        return -1;
    }
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "[dumpObjList] Head:0x%x, Tail:0x%x, num:%d", targetCanvas->ObjListHead, targetCanvas->ObjListTail, targetCanvas->ObjNum);
    tmpObj = targetCanvas->ObjListHead;
    if (tmpObj != NULL) {
        do {
            //GraphicsPrint(DEBUG_ONLY, "ID:%d, v:%d, pre:0x%x, next:0x%x, x:%d, y:%d, s:%d, l:%d, g:%d, a:%d, t:0x%x",
            //        tmpObj->objID, tmpObj->Attr.Stat, tmpObj->PrevObj, tmpObj->NextObj,
            //        tmpObj->objAttr.Area.X, tmpObj->objAttr.Area.Y, tmpObj->objAttr.show, tmpObj->objAttr.layer,
            //        tmpObj->objAttr.group, tmpObj->objAttr.alpha, tmpObj->objAttr.type);
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "[dumpObjList] ID:%d", tmpObj->Attr.ID);
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "[dumpObjList] Area.X:%4d Area.Y:%4d Area.Width:%4d Area.Height:%4d Alpha:0x%x Layer:%d Group:%d Show:%d",
                    tmpObj->Attr.DisplayBox.X, tmpObj->Attr.DisplayBox.Y, tmpObj->Attr.DisplayBox.Width, tmpObj->Attr.DisplayBox.Height,
                    tmpObj->Attr.AlphaTable, tmpObj->Attr.Layer, tmpObj->Attr.Group, tmpObj->Attr.Show);
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_CANVAS, "[dumpObjList] Stat:%d Contex:0x%x CalcArea_f:0x%x Dump_f:0x%x Draw_f:0x%x",
                    tmpObj->Attr.Stat, (UINT8 *)tmpObj->Attr.Content,
                    tmpObj->Attr.CalcArea_f, tmpObj->Attr.Dump_f, tmpObj->Attr.Draw_f);
            tmpObj->Attr.Dump_f(&tmpObj->Attr);
            tmpObj = tmpObj->NextObj;
        } while (tmpObj != NULL);
    }
    return 0;
}
