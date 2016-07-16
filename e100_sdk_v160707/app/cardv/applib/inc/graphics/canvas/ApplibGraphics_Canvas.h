/**
 * @file src/app/connected/applib/inc/graphics/canvas/ApplibGraphics_Canvas.h
 *
 * ApplibGraphics_Canvas include Amba canvas related
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
#ifndef _APPLIB_GRAPHICS_CANVAS_H_
#define _APPLIB_GRAPHICS_CANVAS_H_

/**
* @defgroup ApplibGraphics_Canvas ApplibGraphics_Canvas
* @brief Canvas define for graphics functions
*
* This is detailed description of Canvas
*/
/**
 * @addtogroup ApplibGraphics_Canvas
 * @ingroup GraphicsUtility
 * @{
 */

#include <graphics/obj/ApplibGraphics_Obj.h>

/*************************************************************************
 * Canvas Structures
 ************************************************************************/
/**
 * Graphic obj list
 */
typedef struct _APPLIB_GRAPHIC_OBJ_LIST_s_ {
    APPLIB_GRAPHIC_OBJ_s Attr;                      /**< Graphic attribute                              */
    struct _APPLIB_GRAPHIC_OBJ_LIST_s_ *PrevObj;    /**< pointer to previous graphic obj in the list    */
    struct _APPLIB_GRAPHIC_OBJ_LIST_s_*NextObj;     /**< pointer to next graphic obj in the list        */
} APPLIB_GRAPHIC_OBJ_LIST_s;

/**
 * A Canvas to manage graphic objects,
 * it can draw(attached) on to an OSD buffer, or to any buffer
 */
typedef struct _APPLIB_CANVAS_CFG_s_ {
    AMP_AREA_s Area;            /**< canvas area in buffer              */
//  UINT32 matrix[2][2];        /**< the linear transform(rotate/translate) matrix will apply when drawing to the buffer    */
    UINT32 Alpha;               /**< canvas alpha in a canvas           */
    UINT32 Backcolor;           /**< canvas backcolor in a canvas       */
    UINT32 ObjNumMax;           /**< Max number of graphic obj in list  */
    UINT8 ObjPartEna;           /**< Enable Graphic obj partition       */
    UINT8 Reserved0[3];         /**< Reserved                           */
    UINT32 ObjPartCol;          /**< Graphic obj partition columns, used for partial update/space partition     */
    UINT32 ObjPartRow;          /**< Graphic obj partition rows, used for partial update/space partition        */
    UINT32 LayerMax;            /**< Graphic obj layer maximum          */
    void *CanvasCacheBaseAddr;  /**< Cache base address for Canvas      */
    UINT32 CanvasCacheSize;     /**< Cache size for Canvas              */
} APPLIB_CANVAS_CFG_s;

/**
 * A Canvas to manage graphic objects,
 * it can draw(attached) on to an OSD buffer, or to any buffer
 */
typedef struct _APPLIB_CANVAS_s_ {
    AMP_AREA_s Area;                            /**< canvas area in buffer                      */
//  UINT32 matrix[2][2];                        /**< the linear transform(rotate/translate) matrix will apply when drawing to the buffer    */
    UINT32 Alpha;                               /**< canvas alpha in a canvas                   */
    UINT32 Backcolor;                           /**< canvas backcolor in a canvas               */
    APPLIB_GRAPHIC_RENDER_s *Render;            /**< canvas render                              */
    void *CanvasCacheBaseAddr;                  /**< The address pointing to store graphic obj list, need allocated by user */
    UINT32 CanvasCacheSize;                     /**< The address pointing to store graphic obj list, need allocated by user */
    UINT32 ObjNum;                              /**< Number of graphic obj in list              */
    UINT32 ObjNumMax;                           /**< Max number of graphic obj in list          */
    UINT32 ObjPartCol;                          /**< Graphic obj partition columns, used for partial update/space partition */
    UINT32 ObjPartRow;                          /**< Graphic obj partition rows, used for partial update/space partition    */
    APPLIB_GRAPHIC_OBJ_ID_t ObjIDmax;           /**< Max ID number of graphic obj in list       */
    APPLIB_GRAPHIC_OBJ_LIST_s *ObjListHead;     /**< The head of graphic obj list               */
    APPLIB_GRAPHIC_OBJ_LIST_s *ObjListTail;     /**< Pointer to the tail of graphic obj list    */
    int (*ObjAdd_f)(struct _APPLIB_CANVAS_s_ *targetCanvas,
                       APPLIB_GRAPHIC_OBJ_s *newObj);                   /**< Add a graphic obj into canvas      */
    int (*ObjDelete_f)(struct _APPLIB_CANVAS_s_ *targetCanvas,
                          const APPLIB_GRAPHIC_OBJ_ID_t targetGID);     /**< Delete a graphic obj in canvas     */
    int (*ObjUpdate_f)(struct _APPLIB_CANVAS_s_ *targetCanvas,
                          const APPLIB_GRAPHIC_OBJ_ID_t targetGID,
                          const APPLIB_GRAPHIC_OBJ_s *newObj);           /**< Update a graphic obj in canvas     */
    int (*ObjQuery_f)(struct _APPLIB_CANVAS_s_ *targetCanvas,
                          const APPLIB_GRAPHIC_OBJ_ID_t targetID,
                          APPLIB_GRAPHIC_OBJ_s *queryObj);              /**< Query a graphic obj in canvas      */
    int (*ObjSetShow_f)(struct _APPLIB_CANVAS_s_ *targetCanvas,
                           const APPLIB_GRAPHIC_OBJ_ID_t targetGID);    /**< Set show or not for an graphic obj in canvas   */
    int (*ObjHideAll_f)(struct _APPLIB_CANVAS_s_ *targetCanvas);        /**< Hide all Objs in canvas            */
    int (*CanvasUpdate_f)(struct _APPLIB_CANVAS_s_ *targetCanvas,
                             const APPLIB_CANVAS_CFG_s *canvasCfg,
                             APPLIB_GRAPHIC_RENDER_s *NewRender);       /**< Update canvas attribute            */
    int (*CanvasDraw_f)(struct _APPLIB_CANVAS_s_ *targetCanvas);        /**< Update all graphic obj, and draw canvas content onto buffer */
    int (*CanvasDelete_f)(struct _APPLIB_CANVAS_s_ *targetCanvas);      /**< Delete canvas, cleanup             */
} APPLIB_CANVAS_s;

/*************************************************************************
 * Canvas Functions
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
extern int AppLibCanvas_GetDefCfg(APPLIB_CANVAS_CFG_s *CanvasCfg);

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
extern int AppLibCanvas_CalMemSize(APPLIB_CANVAS_CFG_s *CanvasCfg,
                                   UINT32 *BufSz);

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
extern int AppLibCanvas_Create(APPLIB_CANVAS_s *newCanvas,
                               APPLIB_CANVAS_CFG_s *canvasCfg,
                               APPLIB_GRAPHIC_RENDER_s *render);

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
extern int AppLibCanvas_Update(APPLIB_CANVAS_s *targetCanvas,
                               const APPLIB_CANVAS_CFG_s *canvasCfg,
                               APPLIB_GRAPHIC_RENDER_s *newRender);

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
extern int AppLibCanvas_Delete(APPLIB_CANVAS_s *targetCanvas);

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
extern int AppLibCanvas_Draw(APPLIB_CANVAS_s *targetCanvas);

/**
 *  @brief Add a graphic obj into a graphic list.
 *
 *  Initialize object member, obj-list operation.
 *  targetCanvas will copy all input attribute as an new Obj in canvas itself.
 *
 *  @param [in] *targetCanvas Target operate canvas.
 *  @param [in] newObj Obj ID is going to add.
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibGraphicObjList_Add(APPLIB_CANVAS_s *targetCanvas,
                                    APPLIB_GRAPHIC_OBJ_s *newObj);

/**
 *  @brief Delete a graphic obj from a graphic list.
 *
 *  Delete a graphic obj from a graphic list.
 *
 *  @param [in] *targetCanvas Target operate canvas.
 *  @param [in] targetID Obj ID is going to delete.
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibGraphicObjList_Delete(APPLIB_CANVAS_s *targetCanvas,
                                          const APPLIB_GRAPHIC_OBJ_ID_t targetID);

/**
 *  @brief Update a graphic obj from a graphic list.
 *
 *  Update a graphic obj from a graphic list.
 *
 *  @param [in] *targetCanvas Target operate canvas.
 *  @param [in] targetID Obj ID is going to update.
 *  @param [in] *newObj referent Obj.
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibGraphicObjList_Update(APPLIB_CANVAS_s *targetCanvas,
                                       const APPLIB_GRAPHIC_OBJ_ID_t targetID,
                                       const APPLIB_GRAPHIC_OBJ_s *newObj);

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
extern int AppLibGraphicObjList_Query( APPLIB_CANVAS_s *targetCanvas,
                                       APPLIB_GRAPHIC_OBJ_ID_t targetID,
                                       APPLIB_GRAPHIC_OBJ_s *queryObj);

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
extern int AppLibGraphicObj_SetShow(APPLIB_CANVAS_s *targetCanvas,
                                    APPLIB_GRAPHIC_OBJ_ID_t targetID);

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
extern int AppLibGraphicObj_HideAll(APPLIB_CANVAS_s *targetCanvas);

#endif /* _APPLIB_CANVAS_H_ */

/**
 * @}
 */

