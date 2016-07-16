/**
 * @file src/app/connected/applib/src/graphics/BMPfont/ApplibGraphics_BMPFont.c
 *
 * Implementation of BMP Font module in application Library.
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
#include "BMPFont.h"
#include <graphics/font/ApplibGraphics_BMPFont.h>

/**
* @defgroup ApplibGraphics_BMPFont
* @brief BMP font define for graphics functions
*
*
*/
/**
 * @addtogroup ApplibGraphics_BMPFont
 * @ingroup Graphics
 * @{
 */

/*************************************************************************
 * BMP font definitions
 ************************************************************************/

/*************************************************************************
 * BMP font enum
 ************************************************************************/

/*************************************************************************
 * BMP font struct
 ************************************************************************/

/*************************************************************************
 * BMP font variables
 ************************************************************************/
static APPLIB_FONT_s gFont = {0};
static UINT8* gLangTable = NULL;
static UINT8* gPageTable = NULL;

/*************************************************************************
 * BMP font APIs
 ************************************************************************/

UINT32 *(getCharW)(UINT16 *);

int AppLibBMPFont_Config(void)
{
    GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "AppLibBMPFont_Config, Not impl yet.");
    return AMP_OK;
}

int AppLibBMPFont_Dump(void)
{
    APPLIB_GRAPHIC_BMPFONT_BIN_HEADER_s* header = NULL;

    /* Get BMP font in BIN */
    header = (APPLIB_GRAPHIC_BMPFONT_BIN_HEADER_s*) gFont.Buffer;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_FONT, "font_name:%s, header_size:0x%x, size_of_font_t:0x%x, size_of_lang_t:0x%x, magic:0x%x, reversed:0x%x, version:0x%x, lang_num:%d, pages:%d",
            header->FontName, header->HeaderSize, header->SizeOfFont_s, header->SizeOfLang_s, header->Magic,
            header->Reversed, header->Version, header->LangNum, header->Pages);

    if (header->Pages > 0) {
        // process on page info
        int t = 0;
        for (t = 0; t < header->Pages; t++) {
            int offsetSize = 0, off = 0;
            APPLIB_GRAPHIC_BMPFONT_BIN_FONT_s* curPage = NULL;
            curPage = (APPLIB_GRAPHIC_BMPFONT_BIN_FONT_s*) (gPageTable + (t * header->SizeOfFont_s));
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_FONT, "page:%d 0x%x, type:0x%x, ascent:0x%x, descent:0x%x, height:%d, line_bytes:0x%x, first_char:0x%x, last_char:0x%x, offset:0x%x, next:0x%x, data:0x%x",
                    t, curPage, curPage->Type, curPage->Ascent, curPage->Descent, curPage->Height,
                    curPage->LineBytes, curPage->FirstChar, curPage->LastChar, curPage->Offset, curPage->Next, curPage->Data);
            offsetSize = curPage->LastChar - curPage->FirstChar;
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_FONT, "offsetSize:%d");
            for (off = 0; off <= offsetSize; off++) {
                GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_FONT, "off[%d]:0x%x", off, curPage->Offset[off]);
            }
        }
    }
    return AMP_OK;
}

static inline APPLIB_GRAPHIC_BMPFONT_BIN_FONT_s* findCharPage(UINT32 h, UINT16 ch)
{
    int langCnt = 0, pageCnt = 0;
    int langLim = ((APPLIB_GRAPHIC_BMPFONT_BIN_HEADER_s*) gFont.Buffer)->LangNum;
    UINT32 fontStructSize = ((APPLIB_GRAPHIC_BMPFONT_BIN_HEADER_s*) gFont.Buffer)->SizeOfFont_s;
    APPLIB_GRAPHIC_BMPFONT_BIN_LANG_s *lang;
    //int pageStart, pageEnd;
    APPLIB_GRAPHIC_BMPFONT_BIN_FONT_s *fontPage = NULL;
    UINT8 found = 0;

    for (langCnt = 0; (langCnt < langLim) && (found == 0); langCnt++) {
        lang = &((APPLIB_GRAPHIC_BMPFONT_BIN_LANG_s*) gLangTable)[langCnt];
        //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_FONT, "0x%x h:%d first:%d last:%d start:%d end:%d", lang, lang->Height, lang->First, lang->Last, lang->Start, lang->End);
        if (h == lang->Height) {
            if ((ch >= lang->First) && (ch <= lang->Last)) {
                // detect in which page the font is
                for (pageCnt = lang->Start; (pageCnt <= lang->End) && (found == 0); pageCnt++) {
                    fontPage = (APPLIB_GRAPHIC_BMPFONT_BIN_FONT_s *) (((UINT8*) gPageTable) + (pageCnt * fontStructSize));
                    //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_FONT, "first:0x%x last:0x%x h:%d", fontPage->FirstChar, fontPage->LastChar, fontPage->Height);
                    if (ch <= fontPage->LastChar) {
                        found = 1;
                    }
                }
            }
        }

    }
    if (!found) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "AppLibBMPFont_PutStr, findCharPage: cannot find font with code 0x%X & height %d lang_num:%d ", ch, h, langLim);
        return NULL ;
    }
    return fontPage;
}

static UINT32 putChar(const APPLIB_GRAPHIC_RENDER_s *render,
                             const APPLIB_GRAPHIC_BMPFONT_BIN_FONT_s* fontPage,
                             UINT32 x,
                             UINT32 y,
                             UINT16 ch,
                             APPLIB_FONT_DRAW_CONFIG_s drawConfig)
{
    UINT8 *base, data;
    int rl, rt, rr, rb;
    UINT32 charWidth = 0;
    UINT8 render_half = 0;//drawConfig.drawingUVPair;
    UINT32 val = 0;

    if (fontPage == NULL) {
        K_ASSERT(fontPage != NULL);
        return 0;
    }

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_FONT, "putChar: %x %x %x %x", fontPage->Height, fontPage->LineBytes, fontPage->FirstChar, fontPage->LastChar);
    rl = drawConfig.updateArea.X;
    rt = drawConfig.updateArea.Y;
    rr = drawConfig.updateArea.Width + rl - 1;
    rb = rt + ((drawConfig.h - 1) > drawConfig.updateArea.Height ? drawConfig.updateArea.Height: drawConfig.h-1);

    //render buffer boundary clipping
    if (rr > render->BufPitch) {
        rr = rr % render->BufPitch;
    }
    if (rb > render->BufHeight) {
        rb = rb % render->BufHeight;
    }

    val = ch;
    if (val && (x <= rr)) {
        int coff = val - fontPage->FirstChar;  // code off set
        int boff = fontPage->Offset[coff];      // bit offset (char width in pixel)
        int scan;
        int off = 0;
        charWidth = (fontPage->Offset[coff + 1] - boff) >> render_half;
        //GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_FONT, "char:0x%x rl:%d rt:%d rr:%d rb:%d coff:%d boff:%d charWidth:%d",
        //        val, rl, rt, rr, rb, coff, boff, charWidth);
        if ((x + charWidth) > rr) {
            charWidth = rr - x + 1;
        }
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_FONT, "putChar: cw:%d  data:0x%x %d %x %x",
                charWidth, fontPage->Data, coff, &fontPage->Offset[coff], &fontPage->Offset[coff+1]);

        off = fontPage->Offset[coff] / 4; // byte offset, bmp font take 2 bits per piexl
        base = fontPage->Data + off;
        base += ((rt - y) * fontPage->LineBytes);
        for (scan = rt; scan <= rb; scan++) {
            UINT8 *get;
            UINT8* put;
            int bout = 0;
            int sh = 6;

            get = base;
            sh -= ((boff & 0x03) * 2);
            data = *get;
            get++;

            render->MoveTo_f(render, (void**)&put, x, scan);
            while (bout < charWidth) {
                if (sh < 0) {
                    sh = 6;
                    data = *get;
                    get++;
                }
                if (x >= rl) {
                    //tmp[bout] = (data >> sh) & 0x03;
                    switch ((data >> sh) & 0x03) {
                    case 0:
                        break;
                    case 1:
                        render->PlotPixel_f(put, drawConfig.colorFore);
                        //*((UINT32*)put)  = colorBack;
                        break;
                    case 2:
                        render->PlotPixel_f(put, drawConfig.colorFore);
                        //*((UINT32*)put)  = colorFore;
                        break;
                    default:
                        break;
                    }
                }
                sh -= (2 << render_half);
                x++;
                render->MoveNext_f((void**)&put);
                bout++;
                if (x > rr) {
                    break;
                }
            }
            base += fontPage->LineBytes;
            x -= charWidth;
        }
        //x += charWidth;
    }

    return charWidth;
}

static int _BMPFont_ValidCheck(void)
{
    if (gFont.FileName == NULL) {
        return -1;
    }
    if (gFont.Type != TYPE_BMP) {
        return -1;
    }
    if (gFont.Size == 0) {
        return -1;
    }
    if (gFont.Buffer == NULL) {
        return -1;
    }
    return 0;
}

static UINT32 _BMPFont_GetSizeFromROMFS(const char* FontFn)
{
    if (FontFn == NULL) {
        return 0;
    }

    gFont.FileName = FontFn;
    if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, gFont.FileName) > 0) {
        gFont.Size = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, gFont.FileName, 0x0);
    }
    return gFont.Size;
}

/**
 * AppLibBMPFont_InitFromROMFS
 * Init APPLIB_GRAPHIC_BMPFONT_s from RomFS
 * @param[out] font
 * @param[in] fontFn - Font .bin file name
 * @param[in] loadAllPage - Not used yet
 * @return AMP_OK
 */
static int _BMPFont_LoadFromROMFS(UINT8 *FontBuffer)
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

static int _BMPFont_InitFromROMFS(void)
{
    APPLIB_GRAPHIC_BMPFONT_BIN_HEADER_s* header;
    APPLIB_GRAPHIC_BMPFONT_BIN_FONT_s* curPage = NULL;
    header = (APPLIB_GRAPHIC_BMPFONT_BIN_HEADER_s*) gFont.Buffer;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_FONT, "AppLibBMPFont_Init 0x%x %d 0x%x %d 0x%x",
             header->HeaderSize, header->LangNum, header->SizeOfLang_s, header->Pages, header->SizeOfFont_s);

    // parse font file
    //font->SizeFontData = fontMemSize;
    gLangTable = ((UINT8*) gFont.Buffer) + header->HeaderSize;
    gPageTable = gLangTable + (header->SizeOfLang_s * header->LangNum);

    if (header->Pages > 0) {
        // process on page info
        int t;
        for (t = 0; t < header->Pages; t++) {
            if (((APPLIB_GRAPHIC_BMPFONT_BIN_FONT_s*) gPageTable)[t].Ascent == 0) {
                UINT32 offsetSize = 0;
                // start to load data
                curPage = (APPLIB_GRAPHIC_BMPFONT_BIN_FONT_s*) (gPageTable + (t * header->SizeOfFont_s));
                curPage->Offset = (UINT16*) ((UINT32) gFont.Buffer + (UINT32) curPage->Offset); //char width for variable-width font
                GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_FONT, "==font total page:%d loading %3d, height: %2d  %x",
                        header->Pages, t, curPage->Height, curPage);

                // assign data_table pointer
                offsetSize = curPage->LastChar - curPage->FirstChar + 2;
                curPage->Data = (unsigned char *) (curPage->Offset + offsetSize);

                // page is loaded
                curPage->Ascent = curPage->Height;
            }
        }
        return AMP_OK;
    } else {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "AppLibBMPFont_Init, header->pages:%d", header->Pages);
        return AMP_ERROR_INCORRECT_PARAM_STRUCTURE;
    }
}

/**
 * AppLibBMPFont_InitFromFS
 * Init APPLIB_GRAPHIC_BMPFONT_s from file
 * @param[out] font
 * @param[in] fontFn - Font .bin file name
 * @param[in] loadAllPage - Not used yet
 * @return AMP_OK
 */
static int _BMPFont_InitFromFS(const char* fontFn, UINT8 loadAllPage)
{
    AMBA_FS_FILE *fp;
    UINT32 fileSize = 0;
    WCHAR mode[3] = { 'r', 'b', '\0' };

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_FONT, "AppLibBMPFont_InitFromFS start");

    if (fontFn == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "AppLibBMPFont_InitFromFS, fontFn:0x%x", fontFn);
        return AMP_ERROR_INCORRECT_PARAM_STRUCTURE;
    }

    fp = AmbaFS_fopen((char*) fontFn, (char*) mode);
    if (fp == NULL ) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "AppLibBMPFont_InitFromFS open failed");
        return AMP_ERROR_GENERAL_ERROR;
    }

    if (AmbaFS_fseek(fp, 0, AMBA_FS_SEEK_END) < 0) {
        AmbaFS_fclose(fp);
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "AppLibBMPFont_InitFromFS AmbaFS_fseek End failed");
        return AMP_ERROR_GENERAL_ERROR;
    }

    fileSize = AmbaFS_ftell(fp);
    if (fileSize <= 0) {
        AmbaFS_fclose(fp);
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "AppLibBMPFont_InitFromFS fileSize = 0");
        return AMP_ERROR_GENERAL_ERROR;
    }
    if (AmbaFS_fseek(fp, 0, AMBA_FS_SEEK_START) >= 0) {
        // read all
        if (AmbaFS_fread(gFont.Buffer, fileSize, 1, fp) > 0) {
            AmbaFS_fclose(fp);
            //call InitFont
            return _BMPFont_InitFromROMFS();
        } else {
            AmbaFS_fclose(fp);
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "AppLibBMPFont_InitFromFS AmbaFS_fread failed");
            return AMP_ERROR_GENERAL_ERROR;
        }
    } else {
        AmbaFS_fclose(fp);
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "AppLibBMPFont_InitFromFS AmbaFS_fseek Start failed");
        return AMP_ERROR_GENERAL_ERROR;
    }
    return AMP_OK;
}

/**
 * AppLibBMPFont_PutStr
 * Draw string by render
 * @param render - target render
 * @param font - An inited font struct
 * @param x - top left X of the font
 * @param y - top left Y of the font
 * @param h - font size
 * @param str - rendering string
 * @param colorFore - outline color of the string
 * @param colorBack - color of the string
 * @param updateArea - destination area of the sting
 * @param drawingUVPair
 * @return AMP_OK
 */
static int _BMPFont_DrawString( const APPLIB_GRAPHIC_RENDER_s *render,
                                APPLIB_FONT_DRAW_CONFIG_s drawConfig)
{

    UINT16 *str = NULL;
    UINT32 X = 0, Y = 0;

    if (render == NULL || (!drawConfig.str) ) {
        return AMP_ERROR_GENERAL_ERROR;
    }
    if (_BMPFont_ValidCheck() < 0) {
        return AMP_ERROR_GENERAL_ERROR;
    }

    str = drawConfig.str;
    X = drawConfig.updateArea.X;
    Y = drawConfig.updateArea.Y;

    while (*str != 0) { // not end of string
        APPLIB_GRAPHIC_BMPFONT_BIN_FONT_s *fontPage;
        fontPage = findCharPage(drawConfig.h, (*str));
        if (fontPage == NULL ) {
            str++;
            continue;
        }
        // put char on buffer
        X += putChar( render, fontPage, X, Y, (*str), drawConfig);
        //localUpdateArea will update after put char.
        // next char
        str++;
    }
    return AMP_OK;
}

/**
 *  @brief Calculate String's width
 *
 *  To calculate a string's width according to it's font size
 *
 *  @param [in] strSize the string's size (height)
 *  @param [in] str the string
 *
 *  @return charWidth the total width of the input string
 */
static UINT32 _BMPFont_GetStrWidth(UINT32 StrHeight, UINT16 *Str)
{
    APPLIB_GRAPHIC_BMPFONT_BIN_FONT_s *fontPage = NULL;
    UINT32 charWidth = 0;

    if (Str == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_FONT, "str empty!");
        return 0;
    }
    if (_BMPFont_ValidCheck() < 0) {
        return 0;
    }

    while (*Str != 0) {
        int coff = 0, boff = 0;

        fontPage = findCharPage(StrHeight, (*Str));
        if (fontPage == NULL ) {
            Str++;
            continue;
        }

        coff = (*Str) - fontPage->FirstChar;    // code off set
        boff = fontPage->Offset[coff];          // bit offset (char width in pixel)
        charWidth += (fontPage->Offset[coff + 1] - boff);

        Str++;
    }
    return charWidth;
}

/**
 *  @brief Calculate String's height
 *
 *  To calculate a string's height according to it's font size
 *
 *  @param [in] strFont the string's font
 *  @param [in] strSize the string's size (height)
 *  @param [in] str the string
 *
 *  @return charWidth the height of the input string
 */
static UINT32 _BMPFont_GetStrHeight(UINT32 StrHeight, UINT16 *Str)
{
    if (_BMPFont_ValidCheck() < 0) {
        return 0;
    }

    return StrHeight;
}

/**
 *  @brief Get Font functions
 *
 *  To Get Font functions
 *
 *  @param [in] pFunc the all functions of BMP font
 *
 *  @return AMP_OK
 */
int AppLibBMPFont_GetFunc(APPLIB_FONT_FUNCTION_s *pFunc)
{
    gFont.Func.Init_f = _BMPFont_InitFromROMFS;
    gFont.Func.GetFontSize_f = _BMPFont_GetSizeFromROMFS;
    gFont.Func.Load_f = _BMPFont_LoadFromROMFS;
    gFont.Func.Draw_f = _BMPFont_DrawString;
    gFont.Func.GetStrWidth_f = _BMPFont_GetStrWidth;
    gFont.Func.GetStrHeight_f = _BMPFont_GetStrHeight;
    memcpy(pFunc, &gFont.Func, sizeof(APPLIB_FONT_s));
    return AMP_OK;
}

/**
 * @}
 */

