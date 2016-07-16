/**
 * @file src/app/connected/applib/src/graphics/FTfont/ApplibGraphics_FTFont.c
 *
 * Implementation of freetype font module in application Library.
 *
 * History:
 *    2014/04/08 - [Hsunying Huang] created file
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
 * affiliates.  In the absence of such an agreement, you agree to promptly notify and
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
#include <AmbaDataType.h>
#include <AmbaPrintk.h>

#include <graphics/font/ApplibGraphics_FTFont.h>
#ifdef CONFIG_APP_ARD
#include "AmbaStdCLib.h"
#include "ft2build.h"
#include "freetype.h"
#include "ftglyph.h"
#include <graphics/ApplibGraphics_Common.h>
#else
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#endif

/*************************************************************************
 * Freetype font definitions
 ************************************************************************/
#define FT_ATTRIB_8BITS_MAP    0           /* 1: 8 bits/pixel; 0: 1 bit/pixel */

/*************************************************************************
 * Freetype font variables
 ************************************************************************/
int FT_GlyphSlot_Oblique(FT_GlyphSlot slot);
int FT_Outline_Embolden(FT_Outline *outline, FT_Pos strength);
static FT_Library FT_FontLibrary;
static FT_Face FT_FontFace = {0};
static APPLIB_FONT_s gFont = {0};
#ifdef CONFIG_APP_ARD
static INT16 StrHeightOffset = 0;
#endif
/*************************************************************************
 * Freetype font APIs
 ************************************************************************/
static int _FTFont_FindBMP(UINT16 ch, UINT32 h)
{
    int error = 0;
    int found = 0;

    /* Load char */
    int glyph_index = FT_Get_Char_Index(FT_FontFace, ch);
    error = FT_Load_Glyph( FT_FontFace, glyph_index, FT_LOAD_DEFAULT );
    if (error != 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "[AppLib Graph][FT FindBMP] fail");
        return found;
    }
    error = FT_Render_Glyph(FT_FontFace->glyph, ft_render_mode_normal);
    if (error != 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "[AppLib Graph][FT FindBMP] fail");
        return found;
    }

    found = 1;
    return found;
}

static int _FTFont_PutChar( const APPLIB_GRAPHIC_RENDER_s *render,
                            UINT32 X, UINT32 Y,
                            UINT16 ch,
                            APPLIB_FONT_DRAW_CONFIG_s drawConfig)
{
    /* Source data */
    FT_GlyphSlot slot = FT_FontFace->glyph;
    FT_Bitmap bmp = slot->bitmap;

#ifdef CONFIG_APP_ARD
    int Render_BufWidth = 0;
#endif

    /* Refine position */
    X += slot->bitmap_left;
#ifdef CONFIG_APP_ARD
    Y += drawConfig.h - slot->bitmap_top - StrHeightOffset;
#else
    Y += (FT_FontFace->size->metrics.ascender>>6) - slot->bitmap_top - 5;
#endif
    int rl, rt, rr, rb;
    rl = X;
    rt = Y;
    rr = bmp.width + rl - 1;
    rb = bmp.rows + rt - 1;

    /**/
    void *put;
    UINT32 loop = X, line = Y;
    UINT8 i = 0, j = 0;

#ifdef CONFIG_APP_ARD
    Render_BufWidth = render->BufPitch / render->BufPixelSize;
    if(X>=Render_BufWidth || Y >=render->BufHeight || rr>=Render_BufWidth || rb>=render->BufHeight || rr<0 || rb<0){
        AmbaPrintColor(RED,"FT_ERR:[%d,%d,%d,%d],%d %d %d %d",
        render->BufHeight,render->BufPitch,render->BufPixelSize,render->BufWidth,X,Y,rr,rb);
        AmbaPrintk_Flush();
    }
#endif

    if ( bmp.pixel_mode == FT_PIXEL_MODE_GRAY )
    {
        // bitmap->buffer is 256-gray map, one byte / pixel
        while (line <= rb) {
            loop = rl;
            render->MoveTo_f(render, &put, loop, line);

            while (loop < rr) {
                UINT32 val;
                val = bmp.buffer[j*bmp.pitch + i];

                if (val == 0) {
                } else {
                    render->PlotPixel_f(put, drawConfig.colorFore);
                }
                render->MoveNext_f(&put);
                loop++;
                i++;
            }
            i=0;
            j++;
            line++;
        }
    }
    return (slot->advance.x>>6);
}

/**
 *  @brief Get freetype total size from rom file system.
 *
 *  Get freetype total size from rom file system.
 *
 *  @param [in] fontFn The filename of freetype
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
static UINT32 _FTFont_GetSizeFromROMFS(const char* FontFn)
{
    UINT32 fileSize = 0;
    if (FontFn == NULL) {
        return 0;
    }
    gFont.FileName = FontFn;
    if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, gFont.FileName) > 0) {
        gFont.Size = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, gFont.FileName, 0x0);
    }
    return gFont.Size;
}

static UINT32 _FTFont_LoadFromROMFS(UINT8 *FontBuffer)
{
    if (FontBuffer == NULL) {
        return AMP_ERROR_GENERAL_ERROR;
    }
    gFont.Buffer = FontBuffer;
    if (AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, gFont.FileName, gFont.Buffer, gFont.Size, 0) < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "AppLibBMPFont_InitFromROMFS AmbaROM_LoadByName failed");
        return AMP_ERROR_GENERAL_ERROR;
    }
    return AMP_OK;
}

/**
 *  @brief Init freetype module from rom file system.
 *
 *  Init freetype total size from rom file system.
 *
 *  @param [in] font The font configure structure
 *  @param [in] fontFn The filename of freetype
 *  @param [in] loadAllPage Load all freetype at one time
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
static int _FTFont_Init(void)
{
    int error = 0;
    error = FT_Init_FreeType(&FT_FontLibrary);
    if (error != 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "[AppLib Graph][FT init] fail");
        return AMP_ERROR_GENERAL_ERROR;
    }

    error = FT_New_Memory_Face( FT_FontLibrary,                     /* A handle to the FreeType library instance where the face object is created */
                                gFont.Buffer,                       /* pointer to the font data buffer */
                                gFont.Size,                         /* font size in bytes */
                                0,                                  /* face_index */
                                &FT_FontFace);                      /* A pointer to the handle that will be set to describe the new face object */
    if (error != 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "[AppLib Graph][FT init] fail");
        return AMP_ERROR_GENERAL_ERROR;
    }

    error = FT_Select_Charmap( FT_FontFace, ft_encoding_unicode);
    if (error != 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "[AppLib Graph][FT init] fail(%d)", error);
        return AMP_ERROR_GENERAL_ERROR;
    }
    return AMP_OK;
}

/**
 *  @brief Get the total width of the specific string.
 *
 *  Get the total width of the specific string.
 *
 *  @param [in] *render The render for drawing
 *  @param [in] *font The font configure structure
 *  @param [in] drawConfig The configures of drawing of the specific string
 *
 *  @return the width of the string
 */
static int _FTFont_DrawString( const APPLIB_GRAPHIC_RENDER_s *render,
                               APPLIB_FONT_DRAW_CONFIG_s drawConfig )
{
    UINT16 *str = drawConfig.str;
    UINT32 X = drawConfig.updateArea.X;
    UINT32 Y = drawConfig.updateArea.Y;

#ifdef CONFIG_APP_ARD
    StrHeightOffset = 0;
    /* Find Height Offset of the String */
    while (*str != 0) {
        int found1 = 0;
        found1 = _FTFont_FindBMP((*str), drawConfig.h);
        if (found1 == 0) {
            str++;
            continue;
        }else{
            FT_GlyphSlot Slot = FT_FontFace->glyph;
            INT16 tmp;

            tmp = (Slot->bitmap.rows - Slot->bitmap_top);
            if(tmp > 0){
                if(StrHeightOffset < tmp){
                    StrHeightOffset = tmp;
                    //AmbaPrintColor(GREEN,"offset=%d",StrHeightOffset);
                }
            }
            str++;
        }
    }
    str = drawConfig.str;
#endif

    while (*str != 0) {
        /* Find char BMP */
        int found = 0;
        found = _FTFont_FindBMP((*str), drawConfig.h);
        if (found == 0) {
            str++;
            continue;
        }

        /* Draw char */
        X += _FTFont_PutChar( render, X, Y, (*str), drawConfig);

        /* Get next char */
        str++;
    }
    return 0;
}

/**
 *  @brief Get the total width of the specific string.
 *
 *  Get the total width of the specific string.
 *
 *  @param [in] *strFont The font configure structure
 *  @param [in] strSize The height of a character
 *  @param [in] *str The specific string
 *
 *  @return the width of the string
 */
static UINT32 _FTFont_GetStrWidth(UINT32 StrHeight, UINT16 *Str)
{
    UINT32 strWidth = 0;

    while (*Str != 0) {//CONFIG_APP_ARD
        /* Set size */
        int RetStatus = FT_Set_Pixel_Sizes(FT_FontFace, 0, StrHeight);
        if (RetStatus != 0) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "[AppLib Graph][FT GetStrWidth] fail");
        }

        int glyph_index = FT_Get_Char_Index(FT_FontFace, (*Str));//CONFIG_APP_ARD
        RetStatus = FT_Load_Glyph( FT_FontFace, glyph_index, FT_LOAD_DEFAULT );
        if (RetStatus != 0) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "[AppLib Graph][FT GetStrWidth] fail");
        }

        RetStatus = FT_Render_Glyph(FT_FontFace->glyph, ft_render_mode_normal);
        if (RetStatus != 0) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "[AppLib Graph][FT GetStrWidth] fail");
        }

        FT_GlyphSlot Slot;
        Slot = FT_FontFace->glyph;
        strWidth += (Slot->advance.x>>6);

        /* Get next char */
        Str++;//CONFIG_APP_ARD
    }
    return strWidth;
}

/**
 *  @brief Get the height of the specific string.
 *
 *  Get the height of the specific string.
 *
 *  @param [in] *strFont The font configure structure
 *  @param [in] strSize The height of a character
 *  @param [in] *str The specific string
 *
 *  @return the height of the string
 */
static UINT32 _FTFont_GetStrHeight(UINT32 StrHeight, UINT16 *Str)
{
    return StrHeight;
}

/**
 *  @brief Get Font functions
 *
 *  To Get Font functions
 *
 *  @param [in] pFunc the all functions of freetype font
 *
 *  @return AMP_OK
 */
int AppLibFTFont_GetFunc(APPLIB_FONT_FUNCTION_s *pFunc)
{
    gFont.Func.Init_f = _FTFont_Init;
    gFont.Func.GetFontSize_f = _FTFont_GetSizeFromROMFS;
    gFont.Func.Load_f = _FTFont_LoadFromROMFS;
    gFont.Func.Draw_f = _FTFont_DrawString;
    gFont.Func.GetStrWidth_f = _FTFont_GetStrWidth;
    gFont.Func.GetStrHeight_f = _FTFont_GetStrHeight;
    memcpy(pFunc, &gFont.Func, sizeof(APPLIB_FONT_s));
    return AMP_OK;
}

