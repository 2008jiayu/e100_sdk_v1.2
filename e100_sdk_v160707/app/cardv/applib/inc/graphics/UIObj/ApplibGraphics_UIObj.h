/**
 * @file src/app/connected/applib/inc/graphics/UIobj/ApplibGraphics_UIObj.h
 *
 * ApplibGraphics_UIObj include Amba ui object module related
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
#ifndef _APPLIB_GRAPHICS_UIOBJ_H_
#define _APPLIB_GRAPHICS_UIOBJ_H_

/**
* @defgroup ApplibGraphics_UIObj ApplibGraphics_UIObj
* @brief UI Object define for graphics functions
*
* This is detailed description of UI object
*/
/**
 * @addtogroup ApplibGraphics_UIObj
 * @ingroup UIObj
 * @{
 */

//#include <mw.h>
//#include <graphics\graphics.h>
//#include <display\Osd.h>
#include <graphics/string/ApplibGraphics_String.h>
#include <graphics/bmp/ApplibGraphics_Bmp.h>
#include <graphics/shape/ApplibGraphics_Shape.h>

/*************************************************************************
 * UI Object Union
 ************************************************************************/
/**
 * Graphic content description union
 */
typedef union _APPLIB_GRAPHIC_UIOBJ_DESC_u_ {
    APPLIB_GRAPHIC_LINE_CNT_s Line;            /**< union item - Line       */
    APPLIB_GRAPHIC_RECT_CNT_s Rect;            /**< union item - Rectangle  */
    APPLIB_GRAPHIC_CIRCLE_CNT_s Circle;        /**< union item - Circle     */
    APPLIB_GRAPHIC_ELLIPSE_CNT_s Ellipse;      /**< union item - Ellipse    */
    APPLIB_GRAPHIC_BMP_CNT_s Bmp;              /**< union item - BMP        */
    //APPLIB_GRAPHIC_YUV_CNT_t Yuv;            /**< union item - YUV        */
    APPLIB_GRAPHIC_STR_CNT_s Str;              /**< union item - String     */
} APPLIB_GRAPHIC_UIOBJ_DESC_u;

/*************************************************************************
 * Shape Enum
 ************************************************************************/
/**
 * Graphic content type enumerate
 */
typedef enum _APPLIB_GRAPHIC_UIOBJ_e_ {
    APPLIB_GRAPHIC_UIOBJ_LINE,                  /**< string object type      */
    APPLIB_GRAPHIC_UIOBJ_RECT,                  /**< rectangle object type   */
    APPLIB_GRAPHIC_UIOBJ_CIRCLE,                /**< circle object type      */
    APPLIB_GRAPHIC_UIOBJ_ELLIPSE,               /**< ellipse object type     */
    APPLIB_GRAPHIC_UIOBJ_BMP,                   /**< BMP object type         */
    //APPLIB_GRAPHIC_UIOBJ_YUV,                 /**< YUV object type         */
    APPLIB_GRAPHIC_UIOBJ_STRING,                /**< string object type      */
} APPLIB_GRAPHIC_UIOBJ_e;

/**
 * Configuration for creation, describe graphic attributes
 */
typedef struct _APPLIB_GRAPHIC_UIOBJ_s_ {
    AMP_AREA_s UIObjDisplayBox;                 /**< the position and limit bounding of the ui object   */
    UINT32* AlphaTable;                         /**< the alpha table of the ui object                   */
    UINT32 Layer;                               /**< canvas layer which graphic obj drew on             */
    UINT32 Group;                               /**< graphic content group                              */
    UINT8 DefaultShow;                          /**< graphic shown or not                               */
//  UINT32 matrix[2][2];                        /**< the linear transform(rotate/translate) matrix will apply when drawing to a canvas buffer*/
    APPLIB_GRAPHIC_UIOBJ_e Type;                /**< graphic types                                      */
    APPLIB_GRAPHIC_UIOBJ_DESC_u Cnt;            /**< graphic content descriptor                         */
} APPLIB_GRAPHIC_UIOBJ_s;

/*************************************************************************
 * UI object Function
 ************************************************************************/
/**
 *  @brief Creator entry function
 *
 *  The transform function between UI object struct and Object struct
 *
 *  @param [in] *descUIObj The configures of the UI object
 *  @param [out] *dstObj The configures of the object
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibGraphic_CreateObj(APPLIB_GRAPHIC_UIOBJ_s *descUIObj,
                                   APPLIB_GRAPHIC_OBJ_s *dstObj);

/**
 *  @brief Creator entry function of Line
 *
 *  The transform function between Line UI object struct and Line Object struct
 *
 *  @param [in] *descUIObj The configures of the Line UI object
 *  @param [out] *dstLineObj The configures of the object
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibLine_CreateObj(APPLIB_GRAPHIC_UIOBJ_s *descUIObj,
                                APPLIB_GRAPHIC_OBJ_s *dstLineObj);

/**
 *  @brief Creator entry function of Rectangle
 *
 *  The transform function between Rectangle UI object struct and Rectangle Object struct
 *
 *  @param [in] *descUIObj The configures of the Rectangle UI object
 *  @param [out] *dstRectObj The configures of the object
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibRect_CreateObj(APPLIB_GRAPHIC_UIOBJ_s *descUIObj,
                                APPLIB_GRAPHIC_OBJ_s *dstRectObj);

/**
 *  @brief Creator entry function of Circle
 *
 *  The transform function between Circle UI object struct and Circle Object struct
 *
 *  @param [in] *descUIObj The configures of the Circle UI object
 *  @param [out] *dstCircObj The configures of the object
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibCirc_CreateObj(APPLIB_GRAPHIC_UIOBJ_s *descUIObj,
                                APPLIB_GRAPHIC_OBJ_s *dstCircObj);

/**
 *  @brief Creator entry function of BMP
 *
 *  The transform function between BMP UI object struct and BMP Object struct
 *
 *  @param [in] *descUIObj The configures of the BMP UI object
 *  @param [out] *dstBmpObj The configures of the object
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibBMP_CreateObj(APPLIB_GRAPHIC_UIOBJ_s *descUIObj,
                               APPLIB_GRAPHIC_OBJ_s *dstBmpObj);

/**
 *  @brief Creator entry function of String
 *
 *  The transform function between String UI object struct and String Object struct
 *
 *  @param [in] *descUIObj The configures of the String UI object
 *  @param [out] *dstStrObj The configures of the object
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
extern int AppLibStr_CreateObj(APPLIB_GRAPHIC_UIOBJ_s *descUIObj,
                               APPLIB_GRAPHIC_OBJ_s *dstStrObj);

#endif /* _APPLIB_GRAPHICS_GOBJ_H_ */

/**
 * @}
 */

