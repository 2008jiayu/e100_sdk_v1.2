/**
 * @file src/app/connected/applib/src/graphics/BMPfont/ApplibGraphics_Font.c
 *
 * Implementation of Font module in application Library.
 *
 * History:
 *    2015/02/13 - [HsunYing Huang] created file
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
#include <graphics/ApplibGraphics_Common.h> /* CONFIG_APP_ARD */

#include <graphics/font/ApplibGraphics_Font.h>
#include <graphics/font/ApplibGraphics_BMPFont.h>
#ifdef CONFIG_APP_ARD
#include <graphics/font/ApplibGraphics_FTFont.h>
extern int AppLibGraph_GetFontType(void);
#endif

/**
* @defgroup ApplibGraphics_Font
* @brief Font define for graphics functions
*
*
*/
/**
 * @addtogroup ApplibGraphics_Font
 * @ingroup Graphics
 * @{
 */

/*************************************************************************
 * Font definitions
 ************************************************************************/

/*************************************************************************
 * Font enum
 ************************************************************************/

/*************************************************************************
 * Font struct
 ************************************************************************/

/*************************************************************************
 * Font variables
 ************************************************************************/
static APPLIB_FONT_s gFont = {0};

/*************************************************************************
 * Font APIs
 ************************************************************************/
/**
 *  @brief Set font type
 *
 *  To Set font type
 *
 *  @param [in] Type the specific font type
 *
 *  @return AMP_OK
 */
int AppLibFont_SetType(APPLIB_FONT_TYPE_e Type)
{
    gFont.Type = Type;
    return AMP_OK;
}

/**
 *  @brief Get Font functions
 *
 *  To Get Font functions
 *
 *  @param [in] pFontFunc the all functions of the specific font
 *
 *  @return AMP_OK
 */
int AppLibFont_GetFunc(APPLIB_FONT_FUNCTION_s *pFontFunc)
{
    memset(&gFont, 0x0, sizeof(APPLIB_FONT_s));
#ifdef CONFIG_APP_ARD
    gFont.Type = AppLibGraph_GetFontType();
#endif

    switch (gFont.Type) {
        case TYPE_BMP:
            AppLibBMPFont_GetFunc(&gFont.Func);
            memcpy(pFontFunc, &gFont.Func, sizeof(APPLIB_FONT_FUNCTION_s));
            break;
#ifdef CONFIG_APP_ARD
        case TYPE_CUSTOMIZED:
            AppLibFTFont_GetFunc(&gFont.Func);
            memcpy(pFontFunc, &gFont.Func, sizeof(APPLIB_FONT_FUNCTION_s));
            break;
#endif
        default:
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "<AppLibFont_GetFunc> Font type error!");
            break;
    }
    return AMP_OK;
}

/**
 *  @brief Set Font functions
 *
 *  To Set Font functions
 *
 *  @param [in] pFontFunc the all functions of the specific font
 *
 *  @return AMP_OK
 */
int AppLibFont_SetFunc(APPLIB_FONT_FUNCTION_s *pFontFunc)
{
    memset(&gFont, 0x0, sizeof(APPLIB_FONT_s));
    switch (gFont.Type) {
        default:
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "<AppLibFont_GetFunc> Font type error!");
            break;
    }
    return AMP_OK;
}

/**
 * @}
 */

