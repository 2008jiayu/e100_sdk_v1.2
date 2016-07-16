/**
 * @file applib/src/graphics/uiobj/AmbalibGraphics_UIObj.h
 *
 * bmpfont.h only for Amba Graphics BMP font internal use
 *
 * History:
 *    2014/02/07 - [Eric Yen] created file
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

#include "UIobj.h"
#include <graphics/ApplibGraphics_Common.h> /* CONFIG_APP_ARD */

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
int AppLibGraphic_CreateObj(APPLIB_GRAPHIC_UIOBJ_s *descUIObj, APPLIB_GRAPHIC_OBJ_s *dstObj)
{
    int rval;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_UIOBJ, "AppLibGraphic_CreateObj start");
    if (dstObj == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_UIOBJ, "AppLibGraphic_CreateObj dstLineObj:0x%x invalid.", dstObj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (descUIObj == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_UIOBJ, "AppLibGraphic_CreateObj descUIObj:0x%x invalid.", descUIObj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (descUIObj->Type > APPLIB_GRAPHIC_UIOBJ_STRING) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_UIOBJ, "AppLibGraphic_CreateObj descUIObj->Type:0x%x error.", descUIObj->Type);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    switch (descUIObj->Type) {
        case APPLIB_GRAPHIC_UIOBJ_LINE:
            rval = AppLibLine_CreateObj(descUIObj, dstObj);
            break;
        case APPLIB_GRAPHIC_UIOBJ_RECT:
            rval = AppLibRect_CreateObj(descUIObj, dstObj);
            break;
        case APPLIB_GRAPHIC_UIOBJ_CIRCLE:
            rval = AppLibCirc_CreateObj(descUIObj, dstObj);
            break;
        case APPLIB_GRAPHIC_UIOBJ_ELLIPSE:
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_UIOBJ, "AppLibGraphic_CreateObj APPLIB_GRAPHIC_UIOBJ_ELLIPSE not support yet.");
            rval = AMP_ERROR_GENERAL_ERROR;
            break;
        case APPLIB_GRAPHIC_UIOBJ_BMP:
            rval = AppLibBMP_CreateObj(descUIObj, dstObj);
            break;
        case APPLIB_GRAPHIC_UIOBJ_STRING:
            rval = AppLibStr_CreateObj(descUIObj, dstObj);
            break;
        default:
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_UIOBJ, "AppLibGraphic_CreateObj descUIObj->Type:0x%x error.", descUIObj->Type);
            return AMP_ERROR_GENERAL_ERROR;
            break;
    }
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_UIOBJ, "AppLibGraphic_CreateObj end");
    return rval;
}
