/**
 * @file src/app/connected/applib/inc/graphics/shape/ApplibGraphics_Shape.h
 *
 * ApplibGraphics_Shape.h include Amba Shape related
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

#ifndef _APPLIB_GRAPHICS_SHAPE_H_
#define _APPLIB_GRAPHICS_SHAPE_H_

/**
* @defgroup ApplibGraphics_Shape ApplibGraphics_Shape
* @brief Shape define for graphics functions
*
* This is detailed description of Shpae
*/
/**
 * @addtogroup ApplibGraphics_Shape
 * @ingroup GraphicsObj
 * @{
 */

#include <mw.h>
#include <graphics/render/ApplibGraphics_Render.h>
#include <display/Osd.h>

/*************************************************************************
 * Shape Enum
 ************************************************************************/
/**
 * Graphic content shadow position description
 */
typedef enum _APPLIB_GRAPHIC_SHAPE_SHADOW_POSITION_e_ {
    APPLIB_GRAPHIC_SHAPE_SHADOW_TL,         /**< top-left shadow                        */
    APPLIB_GRAPHIC_SHAPE_SHADOW_TM,         /**< top-middle shadow                      */
    APPLIB_GRAPHIC_SHAPE_SHADOW_TR,         /**< top-right shadow                       */
    APPLIB_GRAPHIC_SHAPE_SHADOW_ML,         /**< middle-left shadow                     */
    APPLIB_GRAPHIC_SHAPE_SHADOW_MM,         /**< middle-middle shadow                   */
    APPLIB_GRAPHIC_SHAPE_SHADOW_MR,         /**< middle-right shadow                    */
    APPLIB_GRAPHIC_SHAPE_SHADOW_BL,         /**< bottom-left shadow                     */
    APPLIB_GRAPHIC_SHAPE_SHADOW_BM,         /**< bottom-middle shadow                   */
    APPLIB_GRAPHIC_SHAPE_SHADOW_BR,         /**< bottom-right shadow                    */
} APPLIB_GRAPHIC_SHAPE_SHADOW_POSITION_e;

/*************************************************************************
 * Shape struct
 ************************************************************************/
/**
 * Graphic content shadow description
 */
typedef struct _APPLIB_GRAPHIC_RECT_SHADOW_s_ {
    UINT8 Enable;                                       /**< Enable     */
    APPLIB_GRAPHIC_SHAPE_SHADOW_POSITION_e Postion;     /**< Postion    */
    UINT32 Distance;                                    /**< Distance   */
    UINT32 Color;                                       /**< Color      */
} APPLIB_GRAPHIC_RECT_SHADOW_s;

/**
 * Graphic content description
 */
typedef struct _APPLIB_GRAPHIC_LINE_CNT_s_ {
    UINT32 X1;                              /**< X1 coordinate of this Graphic obj      */
    UINT32 Y1;                              /**< Y1 coordinate of this Graphic obj      */
    UINT32 X2;                              /**< X2 coordinate of this Graphic obj      */
    UINT32 Y2;                              /**< Y2 coordinate of this Graphic obj      */
    UINT32 Thickness;                       /**< thickness of this Graphic obj          */
    UINT32 ColorFore;                       /**< Foreground color of this Graphic obj   */
    UINT32 ColorBack;                       /**< Background color of this Graphic obj   */
} APPLIB_GRAPHIC_LINE_CNT_s;

/**
 * Graphic content rectangle description
 */
typedef struct _APPLIB_GRAPHIC_RECT_CNT_s_ {
    UINT32 X1;                              /**< X1 coordinate of this Graphic obj      */
    UINT32 Y1;                              /**< Y1 coordinate of this Graphic obj      */
    UINT32 X2;                              /**< X2 coordinate of this Graphic obj      */
    UINT32 Y2;                              /**< Y2 coordinate of this Graphic obj      */
    UINT32 Thickness;                       /**< thickness of this Graphic obj          */
    UINT32 ColorFore;                       /**< Foreground color of this Graphic obj   */
    UINT32 ColorBack;                       /**< Background color of this Graphic obj   */
    APPLIB_GRAPHIC_RECT_SHADOW_s Shadow;    /**< Shadow settings of this Graphic obj    */
} APPLIB_GRAPHIC_RECT_CNT_s;

/**
 * Graphic content circle description
 */
typedef struct _APPLIB_GRAPHIC_CIRCLE_CNT_s_ {
    UINT32 CenterX;                         /**< center X coordinate of this obj        */
    UINT32 CenterY;                         /**< center Y coordinate of this obj        */
    UINT32 Radius;                          /**< radius length of this Graphic obj      */
    UINT32 Thickness;                       /**< thickness of this Graphic obj          */
    UINT32 ColorFore;                       /**< Foreground color of this Graphic obj   */
    UINT32 ColorBack;                       /**< Background color of this Graphic obj   */
} APPLIB_GRAPHIC_CIRCLE_CNT_s;

/**
 * Graphic content ellipse description
 */
typedef struct _APPLIB_GRAPHIC_ELLIPSE_CNT_s_ {
    UINT32 CenterX;                         /**< center X coordinate of this obj        */
    UINT32 CenterY;                         /**< center Y coordinate of this obj        */
    UINT32 RadiusH;                         /**< horizontal radius length of this obj   */
    UINT32 RadiusV;                         /**< vertical radius length of this obj     */
    UINT32 Thickness;                       /**< thickness of this Graphic obj          */
    UINT32 ColorFore;                       /**< Foreground color of this Graphic obj   */
    UINT32 ColorBack;                       /**< Background color of this Graphic obj   */
} APPLIB_GRAPHIC_ELLIPSE_CNT_s;

//**
// * Graphic content YUV image description
// */
//typedef struct _APPLIB_GRAPHIC_DESC_YUV_s_ {    //TBD
//    UINT32 Reserved;
//    UINT32 Reserved1;
//    UINT32 Reserved2;
//    UINT32 Reserved3;
//    UINT32 Reserved4;
//    UINT32 Reserved5;
//    UINT32 Reserved6;
//    UINT32 Reserved7;
//} APPLIB_GRAPHIC_DESC_YUV_s;

/*************************************************************************
 * Shape Function
 ************************************************************************/
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
//extern int AppLibLine_CalcArea(struct _APPLIB_GRAPHIC_OBJ_s_ *obj);

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
//extern int AppLibLine_Dump(struct _APPLIB_GRAPHIC_OBJ_s_ *obj);

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
//extern int AppLibLine_Draw(APPLIB_GRAPHIC_RENDER_s *render,
//                           AMP_AREA_s *drawArea,
//                           struct _APPLIB_GRAPHIC_OBJ_s_ *obj);

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
//extern int AppLibRect_CalcArea(struct _APPLIB_GRAPHIC_OBJ_s_ *obj);

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
//extern int AppLibRect_Dump(struct _APPLIB_GRAPHIC_OBJ_s_ *obj);

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
//extern int AppLibRect_Draw( APPLIB_GRAPHIC_RENDER_s *render,
//                            AMP_AREA_s *drawArea,
//                            struct _APPLIB_GRAPHIC_OBJ_s_ *obj);

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
//extern int AppLibCirc_CalcArea(struct _APPLIB_GRAPHIC_OBJ_s_ *obj);

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
//extern int AppLibCirc_Dump(struct _APPLIB_GRAPHIC_OBJ_s_ *obj);

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
//extern int AppLibCirc_Draw( APPLIB_GRAPHIC_RENDER_s *render,
//                            AMP_AREA_s *drawArea,
//                            struct _APPLIB_GRAPHIC_OBJ_s_ *obj);

#endif /* _APPLIB_GRAPHICS_SHAPE_H_ */

/**
 * @}
 */

