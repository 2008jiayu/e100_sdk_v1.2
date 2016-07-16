/**
 * @file src/app/connected/applib/inc/graphics/font/ApplibGraphics_FTFont.h
 *
 * ApplibGraphics_FTFont include Amba BMP font related
 *
 * History:
 *    2014/04/07 - [Hsunying Hunag] created file
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
#ifndef _APPLIB_GRAPHICS_FTFONT_H_
#define _APPLIB_GRAPHICS_FTFONT_H_

/**
* @defgroup ApplibGraphics_FTFont ApplibGraphics_FTFont
* @brief Freetype font define for graphics functions
*
* This is detailed description of freetype font
*/
/**
 * @addtogroup ApplibGraphics_FTFont
 * @ingroup ApplibGraphics_String
 * @{
 */

#include <AmbaKAL.h>
#ifndef CONFIG_APP_ARD
#include <AmbaGLib.h>
#endif
#include "AmbaROM.h"
#include <common/common.h>
#include <graphics/font/ApplibGraphics_Font.h>

/*************************************************************************
 * Freetype font Enum
 ************************************************************************/

/*************************************************************************
 * Freetype font Structure
 ************************************************************************/

/*************************************************************************
 * Freetype font Functions
 ************************************************************************/
/**
 *  @brief Get Font functions
 *
 *  To Get Font functions
 *
 *  @param [in] pFunc the all functions of BMP font
 *
 *  @return AMP_OK
 */
extern int AppLibFTFont_GetFunc(APPLIB_FONT_FUNCTION_s *pFunc);

#endif /* _APPLIB_GRAPHICS_FTFONT_H_ */

/**
 * @}
 */


