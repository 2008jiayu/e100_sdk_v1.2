/**
 * @file applib/src/graphics/string/ApplibGraphics_String.c
 *
 * Header of string
 *
 * History:
 *    2013/11/15 - [Eric Yen] created file
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

#include <w_char.h>

#include <graphics/ApplibGraphics_Common.h> /* CONFIG_APP_ARD */

#include "String.h"
#include <graphics/string/ApplibGraphics_String.h>
#include <graphics/UIObj/ApplibGraphics_UIObj.h>

/*************************************************************************
 * String definitions
 ************************************************************************/
#define ABS(a)      (((a) < 0) ? -(a) : (a))

/*************************************************************************
 * String APIs
 ************************************************************************/
static int _AppLibStr_GetDisplayBoxPosition(APPLIB_GRAPHIC_OBJ_s* obj)
{
    UINT32 deltaX = 0, deltaY = 0;
    APPLIB_GRAPHIC_STR_CNT_s *appStrLayout;
    appStrLayout = (APPLIB_GRAPHIC_STR_CNT_s *)obj->Content;

    /* Calculate X-shift */
    switch (appStrLayout->Alignment) {
        case APPLIB_GRAPHIC_STRING_ALIGN_TM:
        case APPLIB_GRAPHIC_STRING_ALIGN_MM:
        case APPLIB_GRAPHIC_STRING_ALIGN_BM:
            deltaX = (appStrLayout->Width - obj->DisplayBox.Width) >> 1;
            break;
        case APPLIB_GRAPHIC_STRING_ALIGN_TR:
        case APPLIB_GRAPHIC_STRING_ALIGN_MR:
        case APPLIB_GRAPHIC_STRING_ALIGN_BR:
            deltaX = appStrLayout->Width - obj->DisplayBox.Width;
            break;
        case APPLIB_GRAPHIC_STRING_ALIGN_TL:
        case APPLIB_GRAPHIC_STRING_ALIGN_ML:
        case APPLIB_GRAPHIC_STRING_ALIGN_BL:
        default:
            deltaX = 0;
            break;
    }

    /* Calculate Y-shift */
    switch (appStrLayout->Alignment) {
        case APPLIB_GRAPHIC_STRING_ALIGN_BL:
        case APPLIB_GRAPHIC_STRING_ALIGN_BM:
        case APPLIB_GRAPHIC_STRING_ALIGN_BR:
            deltaY = appStrLayout->Height- obj->DisplayBox.Height;
            break;
        case APPLIB_GRAPHIC_STRING_ALIGN_TL:
        case APPLIB_GRAPHIC_STRING_ALIGN_TM:
        case APPLIB_GRAPHIC_STRING_ALIGN_TR:
            deltaY = 0;
            break;
        case APPLIB_GRAPHIC_STRING_ALIGN_ML:
        case APPLIB_GRAPHIC_STRING_ALIGN_MM:
        case APPLIB_GRAPHIC_STRING_ALIGN_MR:
        default:
            deltaY = (appStrLayout->Height- obj->DisplayBox.Height) >> 1;
            break;
    }

    /* Fine-tune */
    obj->DisplayBox.X = appStrLayout->Left + deltaX;
    obj->DisplayBox.Y = appStrLayout->Top + deltaY;

    return AMP_OK;
}

static int _AppLibStr_DrawShadow(APPLIB_GRAPHIC_RENDER_s *render, APPLIB_GRAPHIC_OBJ_s *obj, INT32 *deltaX, INT32 *deltaY)
{
    APPLIB_GRAPHIC_STR_CNT_s *appStrLayout = NULL;
    APPLIB_FONT_DRAW_CONFIG_s drawConfig = {0};
    APPLIB_FONT_s *strFontAttr = NULL;

    appStrLayout = (APPLIB_GRAPHIC_STR_CNT_s *)obj->Content;

    /* Calculate X-shift */
    switch (appStrLayout->Shadow.Postion) {
        case APPLIB_GRAPHIC_SHAPE_SHADOW_TL:
        case APPLIB_GRAPHIC_SHAPE_SHADOW_ML:
        case APPLIB_GRAPHIC_SHAPE_SHADOW_BL:
            (*deltaX) = 0 - appStrLayout->Shadow.Distance;
            break;
        case APPLIB_GRAPHIC_SHAPE_SHADOW_TM:
        case APPLIB_GRAPHIC_SHAPE_SHADOW_MM:
        case APPLIB_GRAPHIC_SHAPE_SHADOW_BM:
            (*deltaX) = 0;
            break;
        case APPLIB_GRAPHIC_SHAPE_SHADOW_TR:
        case APPLIB_GRAPHIC_SHAPE_SHADOW_MR:
        case APPLIB_GRAPHIC_SHAPE_SHADOW_BR:
        default:
            (*deltaX) = appStrLayout->Shadow.Distance;
            break;
    }

    /* Calculate Y-shift */
    switch (appStrLayout->Shadow.Postion) {
        case APPLIB_GRAPHIC_SHAPE_SHADOW_TL:
        case APPLIB_GRAPHIC_SHAPE_SHADOW_TM:
        case APPLIB_GRAPHIC_SHAPE_SHADOW_TR:
            (*deltaY) = 0 - appStrLayout->Shadow.Distance;
            break;
        case APPLIB_GRAPHIC_SHAPE_SHADOW_ML:
        case APPLIB_GRAPHIC_SHAPE_SHADOW_MM:
        case APPLIB_GRAPHIC_SHAPE_SHADOW_MR:
            (*deltaY) = 0;
            break;
        case APPLIB_GRAPHIC_SHAPE_SHADOW_BL:
        case APPLIB_GRAPHIC_SHAPE_SHADOW_BM:
        case APPLIB_GRAPHIC_SHAPE_SHADOW_BR:
        default:
            (*deltaY) = appStrLayout->Shadow.Distance;
            break;
    }

    drawConfig.h = appStrLayout->StrSize;
    drawConfig.str = (UINT16*)appStrLayout->StrDesc->Ptr;
    drawConfig.colorFore = appStrLayout->ColorBack;
    drawConfig.colorBack = appStrLayout->ColorFore;
    drawConfig.updateArea.X = obj->DisplayBox.X + (*deltaX);
    drawConfig.updateArea.Y = obj->DisplayBox.Y + (*deltaY);
    drawConfig.updateArea.Width = obj->DisplayBox.Width;
    drawConfig.updateArea.Height = obj->DisplayBox.Height;
    drawConfig.drawingUVPair = 0;

    strFontAttr = (APPLIB_FONT_s *)appStrLayout->FontAttr;
    if (strFontAttr->Func.Draw_f) {
        strFontAttr->Func.Draw_f(render, drawConfig);
    }

    return AMP_OK;
}

static int _AppLibStr_DrawString(APPLIB_GRAPHIC_RENDER_s *render, APPLIB_GRAPHIC_OBJ_s *obj)
{
    APPLIB_GRAPHIC_STR_CNT_s *appStrLayout = NULL;
    APPLIB_FONT_s *strFontAttr = NULL;
    APPLIB_FONT_DRAW_CONFIG_s drawConfig = {0};

    if ((render == NULL) || (obj == NULL)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "_AppLibStr_DrawString: render 0x%X, obj 0x%X empty!", render, obj);
        return AMP_ERROR_GENERAL_ERROR;
    }
    if (obj->Content == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "_AppLibStr_DrawString: obj->Content 0x%X empty!", obj->Content);
        return AMP_ERROR_GENERAL_ERROR;
    }

    appStrLayout = (APPLIB_GRAPHIC_STR_CNT_s *)obj->Content;
    drawConfig.h = appStrLayout->StrSize;
    drawConfig.str = (UINT16*)appStrLayout->StrDesc->Ptr;
    drawConfig.colorFore = appStrLayout->ColorFore;
    drawConfig.colorBack = appStrLayout->ColorBack;
    drawConfig.updateArea.X = obj->DisplayBox.X;
    drawConfig.updateArea.Y = obj->DisplayBox.Y;
    drawConfig.updateArea.Width = obj->DisplayBox.Width;
    drawConfig.updateArea.Height = obj->DisplayBox.Height;
    drawConfig.drawingUVPair = 0;

    strFontAttr = (APPLIB_FONT_s *)appStrLayout->FontAttr;
    if (strFontAttr->Func.Draw_f) {
        strFontAttr->Func.Draw_f(render, drawConfig);
    }
    return AMP_OK;
}

/**
 *  @brief Calculate String's display info
 *
 *  To calculate a string's display position, width and height
 *
 *  @param [in] obj the string object
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
static int _AppLibStr_CalcArea(APPLIB_GRAPHIC_OBJ_s *obj)
{
    APPLIB_GRAPHIC_STR_CNT_s *strCnt = NULL;

    /* Bounding Check */
    if ((obj == NULL) || (obj->Content == NULL)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "obj:0x%X, obj->Content:0x%X invalid", obj, obj->Content);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    strCnt = (APPLIB_GRAPHIC_STR_CNT_s*)obj->Content;

    /* Error Check */
    if (strCnt->FontAttr->Type >= TYPE_NUM) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "[AppLib Graph]<_AppLibStr_CalcArea> FontType: %d invalid", strCnt->FontAttr->Type);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    if (strCnt->FontAttr->Func.GetStrWidth_f) {
        obj->DisplayBox.Width = strCnt->FontAttr->Func.GetStrWidth_f(strCnt->StrSize, (UINT16*)strCnt->StrDesc->Ptr);
    }
    if (strCnt->FontAttr->Func.GetStrHeight_f) {
        obj->DisplayBox.Height = strCnt->FontAttr->Func.GetStrHeight_f(strCnt->StrSize, (UINT16*)strCnt->StrDesc->Ptr);
    }
    _AppLibStr_GetDisplayBoxPosition(obj);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "(%d, %d) w(%d, %d)", obj->DisplayBox.X, obj->DisplayBox.Y, obj->DisplayBox.Width, obj->DisplayBox.Height);

    return AMP_OK;
}

/**
 *  @brief Dump String's display info
 *
 *  To dump a string's display position, width and height in canvas
 *
 *  @param [in] obj the string object
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
static int _AppLibStr_Dump(APPLIB_GRAPHIC_OBJ_s *obj)
{
    WCHAR *str = NULL;
    char str_ch[512];
    APPLIB_GRAPHIC_STR_CNT_s *StrCnt = NULL;
    APPLIB_FONT_s *font = NULL;

    if ((obj == NULL) || (obj->Content == NULL)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "obj:0x%x, obj->Content:0x%x invalid", obj, obj->Content);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    StrCnt = (APPLIB_GRAPHIC_STR_CNT_s *)obj->Content;
    str = StrCnt->StrDesc->Ptr;
    w_uni2asc(str_ch, str, w_strlen(str));
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "[AppLibStr_Dump] List[%d]", obj->ID);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "[AppLibStr_Dump] Str LangIdx: %d, MsgIdx: %d", StrCnt->LangIdx, StrCnt->MsgIdx);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "[AppLibStr_Dump] Str Left:%d Top:%d SizeHeight:%d ColorFore:0x%x ColorBack:0x%x FontAttr:0x%x Alignment:%d Str:(%s)0x%x",
            StrCnt->Left, StrCnt->Top, StrCnt->StrSize, StrCnt->ColorFore, StrCnt->ColorBack,
            StrCnt->FontAttr, StrCnt->Alignment, str_ch, StrCnt->StrDesc->Ptr);

    font = StrCnt->FontAttr;
    if (font != NULL) {
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "[AppLibStr_Dump] font FontData:0x%x SizeFontData:%d",
                font->Buffer, font->Size);
    }
    return AMP_OK;
}

/**
 *  @brief Draw String's display info
 *
 *  To draw a string
 *
 *  @param [in] render The OSD render is going to draw on
 *  @param [in] drawArea Draw area of OSD buffer
 *  @param [in] obj The graphic object is going to operate
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
static int _AppLibStr_Draw(APPLIB_GRAPHIC_RENDER_s *render, AMP_AREA_s *drawArea, APPLIB_GRAPHIC_OBJ_s *obj)
{
    APPLIB_GRAPHIC_STR_CNT_s *strCnt = NULL;
    INT32 deltaX = 0, deltaY = 0;

    if ( (render == NULL) || (obj == NULL) ) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "render: 0x%X, obj: 0x%X invalid", render, obj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if ( obj->Content == NULL ) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "obj->Content:0x%x invalid", obj->Content);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    /* Get STR context */
    strCnt = (APPLIB_GRAPHIC_STR_CNT_s *)obj->Content;

    /* Draw shadow */
    if (strCnt->Shadow.Enable) {
        _AppLibStr_DrawShadow(render, obj, &deltaX, &deltaY);
    }

    /* Draw string */
    _AppLibStr_DrawString(render, obj);

    /* Update Last Display Box */
    obj->LastDisplayBox.X = obj->DisplayBox.X;// + deltaX;
    obj->LastDisplayBox.Y = obj->DisplayBox.Y;// + deltaY;
    obj->LastDisplayBox.Width = obj->DisplayBox.Width + ABS(deltaX);
    obj->LastDisplayBox.Height = obj->DisplayBox.Height + ABS(deltaY);

    return AMP_OK;
}

/**
 *  @brief Calculate the total size of string from rom file system
 *
 *  Calculate needed Str buffer size for some language from String.bin in ROMFS
 *  This Buffer includes needed index/header of the String bin
 *
 *  @param [in] *fileName Indicated which .bin file generated by AmbaGUIGen
 *  @param [in] langIdx Indicate which language is going to calculate
 *  @param [out] *msgNum Returned message number of that language
 *  @param [out] *strBufSize Returned needed Str buffer size
 *  @param [out] *tmpBufSize Returned needed string tmp buffer size
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibStr_CalcLangSizeFromROMFS(const char *fileName,
                                    UINT32 langIdx,
                                    UINT32 *msgNum,
                                    UINT32 *strBufSize,
                                    UINT32 *tmpBufSize)
{
    APPLIB_GRAPHIC_STR_BIN_HEADER_s Header = {0};
    UINT32 LangSize = 0;

    /* Error check */
    if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, fileName) == 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "AppLibStr_CalcLangSizeFromROMFS %s is not exist.", fileName);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    /* Get msg total number */
    {
        int RomIdx = AmbaROM_GetIndex(AMBA_ROM_SYS_DATA, fileName);
        int RomSz = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, fileName, RomIdx);
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "AppLibStr_GetIndexFromROMFS: file idx = %d", RomIdx);
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "AppLibStr_GetFileSizeFromROMFS: file size = %d", RomSz);
    }
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&Header, sizeof(APPLIB_GRAPHIC_STR_BIN_HEADER_s), 0);
    *msgNum = Header.MsgNum;

    /* Boundary check */
    if (langIdx >= Header.LangNum) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "AppLibStr_CalcLangSizeFromROMFS langIdx(%d) >= Header.LangNum(%d) invalid.", langIdx, Header.LangNum);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    /* Load file */
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&LangSize, sizeof(UINT32),
                       sizeof(APPLIB_GRAPHIC_STR_BIN_HEADER_s) + langIdx*sizeof(UINT32));
    *strBufSize = sizeof(APPLIB_GRAPHIC_STR_BIN_INFO_s)
                  + Header.MsgNum * sizeof(APPLIB_GRAPHIC_STR_BIN_DESC_s)   //DescTable
                  + LangSize;   //MsgData
    *tmpBufSize = 2 * Header.LangNum * sizeof(UINT32)   //LangSizeTable, LangOffsetTable
                  + Header.MsgNum * sizeof(UINT32);   //MsgOffsetTable
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "LangNum:%d MsgNum:%d LangSize:0x%x strBufferSize:0x%x tmpBufSize:0x%x",
                  Header.LangNum, *msgNum, LangSize, *strBufSize, *tmpBufSize);
    return AMP_OK;
}

/**
 *  @brief Initialize one resolution of BMPs in BMP.bin.
 *
 *  Initialize one language of strings in str.bin.
 *  Construct indexes in strBuf, the index includes content of APPLIB_GRAPHIC_BMP_BIN_INFO_t
 *
 *  @param [in] *fileName Indicated which .bin file generated by AmbaGUIGen
 *  @param [in] langIdx Specify which language to init
 *  @param [in] *strBuf An allocated buffer for loading string, and string index,
 *  @param [in] *tmpBuf An allocated buffer for temporary used when loading messages,
 *                      can be released immediately after function called.
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibStr_InitFromROMFS( const char *fileName,
                             UINT32 langIdx,
                             void *strBuf,
                             void *tmpBuf)
{
    APPLIB_GRAPHIC_STR_BIN_HEADER_s Header = {0};
    APPLIB_GRAPHIC_STR_BIN_INFO_s *SInfo;
    UINT32 Offset;
    UINT32 *MsgOffTable, *LangSizeTable, *LangOffsetTable;
    WCHAR *MsgBase;
    UINT32 MIdx = 0;
    APPLIB_GRAPHIC_STR_BIN_DESC_s *Desc = NULL;

    /* Error check */
    if ( (!strBuf) || (!tmpBuf) || (!fileName)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "[AppLib Graphic][InitStr] strBuf: 0x%x, tmpBuf: 0x%x invalid.", strBuf, tmpBuf);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, fileName) == 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "[AppLib Graphic][InitStr] %s is not exist.", fileName);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&Header, sizeof(APPLIB_GRAPHIC_STR_BIN_HEADER_s), 0);

    /* Error check */
    if ( langIdx >= Header.LangNum ) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "[AppLib Graphic][InitStr] langIdx: %d invalid.", langIdx);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    /* Init strBuf */
    SInfo = (APPLIB_GRAPHIC_STR_BIN_INFO_s *)strBuf;
    strncpy(SInfo->BinFileName, fileName, strlen(fileName));
    SInfo->LangIdx = langIdx;
    SInfo->MsgNum = Header.MsgNum;
    SInfo->DescTable = (APPLIB_GRAPHIC_STR_BIN_DESC_s *)((UINT8 *)strBuf + sizeof(APPLIB_GRAPHIC_STR_BIN_INFO_s));

    /*Init BMPBuf, descArr */
    LangSizeTable = (UINT32*)tmpBuf;
    LangOffsetTable = (UINT32*)((UINT8*)LangSizeTable + Header.LangNum*sizeof(UINT32));
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "SInfo->BinFileName:%s LangIdx:%d MsgNum:%d DescTable:0x%x",
            SInfo->BinFileName, SInfo->LangIdx, SInfo->MsgNum, SInfo->DescTable);
    Offset = sizeof(APPLIB_GRAPHIC_STR_BIN_HEADER_s);

    /* Load data from ROM */
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)LangSizeTable, 2*Header.LangNum*sizeof(UINT32), Offset);

    //load MsgOffTable from bin
    Offset = LangOffsetTable[langIdx];
    MsgOffTable = (UINT32*)((UINT8*)LangOffsetTable + Header.LangNum*sizeof(UINT32));
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)MsgOffTable, SInfo->MsgNum*sizeof(UINT32), Offset);
    MsgBase = (WCHAR*)((UINT8*)SInfo->DescTable + SInfo->MsgNum*sizeof(APPLIB_GRAPHIC_STR_BIN_DESC_s));
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "LangSizeTable:0x%x LangOffsetTable:0x%x MsgOffTable:0x%x MsgBase:0x%x",
            LangSizeTable, LangOffsetTable, MsgOffTable, MsgBase);

    for (MIdx=0; MIdx<SInfo->MsgNum-1; MIdx++) {
        //Init String desc
        Desc = &SInfo->DescTable[MIdx];
        Desc->Count = 1;
        Desc->Flags = APPLIB_GRAPHIC_STR_BIN_INFO_LOAD;
        Desc->Ptr = (WCHAR*)((UINT8*)MsgBase + MsgOffTable[MIdx]);
        Desc->Offset = LangSizeTable[langIdx] + MsgOffTable[MIdx];
        Desc->Size = MsgOffTable[MIdx+1] - MsgOffTable[MIdx];
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "MIdx:%3d Desc:0x%x Off:0x%4x size:0x%4x Ptr:0x%x MsgOff:0x%x",
                MIdx, Desc, Desc->Offset, Desc->Size, Desc->Ptr, MsgOffTable[MIdx]);
    }
    MIdx = SInfo->MsgNum-1;
    Desc = &SInfo->DescTable[MIdx];
    Desc->Count = 1;
    Desc->Flags = APPLIB_GRAPHIC_STR_BIN_INFO_LOAD;
    Desc->Ptr = (WCHAR*)((UINT8*)MsgBase + MsgOffTable[MIdx]);
    Desc->Offset = LangSizeTable[langIdx] + MsgOffTable[MIdx];
    Desc->Size = LangSizeTable[langIdx] - MsgOffTable[MIdx];
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "MIdx:%3d Desc:0x%x Off:0x%4x size:0x%4x Ptr:0x%x MsgOff:0x%x",
            MIdx, Desc, Desc->Offset, Desc->Size, Desc->Ptr, MsgOffTable[MIdx]);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "AppLibStr_InitFromROMFS init done");

    //load msg
    Offset += SInfo->MsgNum*sizeof(UINT32);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "dstPtr:0x%x srcSize:0x%x srcOff:0x%x", MsgBase, LangSizeTable[langIdx], Offset);
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8 *)MsgBase, LangSizeTable[langIdx], Offset);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "AppLibStr_InitFromROMFS load done");
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "SInfo->BinFileName:%s LangIdx:%d MsgNum:%d DescTable:0x%x",
            SInfo->BinFileName, SInfo->LangIdx, SInfo->MsgNum, SInfo->DescTable);

    return AMP_OK;
}

/**
 *  @brief Creator entry function of String
 *
 *  The transform function between String UI object struct and String Object struct
 *
 *  @param [in] *descUIObj The configures of the String UI object
 *  @param [out] *dstObj The configures of the object
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibStr_CreateObj(APPLIB_GRAPHIC_UIOBJ_s *descUIObj, APPLIB_GRAPHIC_OBJ_s *dstStrObj)
{
    APPLIB_GRAPHIC_STR_CNT_s *strCnt = NULL;
    APPLIB_GRAPHIC_STR_BIN_INFO_s *SInfo = NULL;

    if ((descUIObj == NULL) || (dstStrObj == NULL)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "[AppLib Str][CreateObj] descUIObj: 0x%X, dstStrObj: 0x%X invalid.", descUIObj, dstStrObj);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    if (descUIObj->Type != APPLIB_GRAPHIC_UIOBJ_STRING) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "[AppLib Str][CreateObj] descUIObj->Type:0x%x error.", descUIObj->Type);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    strCnt = &(descUIObj->Cnt.Str);

    //a single bmp
    if ((strCnt->LangIdx == 0xFFFFFFFF) && (strCnt->MsgIdx == 0xFFFFFFFF) && (strCnt->StrDesc != 0x0)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "AppLibStr_CreateObj Not support create from single APPLIB_GRAPHIC_STR_BIN_DESC_s yet.");
        return AMP_ERROR_GENERAL_ERROR;
    }

    //Bin Info ready
    if ( !strCnt->StrInfo->DescTable ) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "AppLibStr_CreateObj Error, Left:%d Bottom:%d SizeHeight:%d ColorFore:0x%x ColorBack:0x%x",
                strCnt->Left, strCnt->Top, strCnt->StrSize, strCnt->ColorFore, strCnt->ColorBack);
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "                         , FontAttr:0x%x Alignment:%d LangIdx:%d MsgIdx:%d StrInfo:0x%x StrDesc:0x%x",
                strCnt->FontAttr, strCnt->Alignment, strCnt->LangIdx, strCnt->MsgIdx,
                strCnt->StrInfo, strCnt->StrDesc);
        if (strCnt->StrInfo != 0x0) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "                         , StrInfo->BinFileName:%s LangIdx:%d MsgNum:%d DescTab:0x%x",
                    strCnt->StrInfo->BinFileName, strCnt->StrInfo->LangIdx, strCnt->StrInfo->MsgNum,
                    strCnt->StrInfo->DescTable);
        }
        if (strCnt->StrDesc != 0x0) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "                         , StrDesc->Offset:0x%x Size:0x%x Flags:0x%x Count:%d Ptr:0x%x",
                    strCnt->StrDesc->Offset, strCnt->StrDesc->Size, strCnt->StrDesc->Flags,
                    strCnt->StrDesc->Count, strCnt->StrDesc->Ptr);
        }
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    SInfo = descUIObj->Cnt.Str.StrInfo;
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "LangIdx:%d MsgIdx:%d", strCnt->LangIdx, strCnt->MsgIdx);

    if ((strCnt->MsgIdx < SInfo->MsgNum) && (SInfo->LangIdx == SInfo->LangIdx))  {
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "StrSize:0x%x StrOff:0x%x", descUIObj->Cnt.Str.StrDesc->Size, descUIObj->Cnt.Str.StrDesc->Offset);

        dstStrObj->DisplayBox.X = descUIObj->UIObjDisplayBox.X;
        dstStrObj->DisplayBox.Y = descUIObj->UIObjDisplayBox.Y;
        dstStrObj->DisplayBox.Width = descUIObj->UIObjDisplayBox.Width;      //pass original setting into new obj
        dstStrObj->DisplayBox.Height = descUIObj->UIObjDisplayBox.Height;    //canvas will altered then
        dstStrObj->LastDisplayBox.X = descUIObj->UIObjDisplayBox.X;
        dstStrObj->LastDisplayBox.Y = descUIObj->UIObjDisplayBox.Y;
        dstStrObj->LastDisplayBox.Width = descUIObj->UIObjDisplayBox.Width;      //pass original setting into new obj
        dstStrObj->LastDisplayBox.Height = descUIObj->UIObjDisplayBox.Height;

        dstStrObj->AlphaTable = descUIObj->AlphaTable;
        dstStrObj->Layer = descUIObj->Layer;
        dstStrObj->Group = descUIObj->Group;
        dstStrObj->Show = descUIObj->DefaultShow;
        dstStrObj->Stat = OBJ_STAT_NORMAL;
        dstStrObj->Content = (void *)strCnt;
        dstStrObj->CalcArea_f = _AppLibStr_CalcArea;
        dstStrObj->Dump_f = _AppLibStr_Dump;
        dstStrObj->Draw_f = _AppLibStr_Draw;
        //dstBmpObj->Update_f = AppLibStr_Update;
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "AppLibStr_CreateObj end");
        return AMP_OK;
    } else {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "LangIdx:%d MsgIdx:%d error", strCnt->LangIdx, strCnt->MsgIdx);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
}

#if 0
/**
 * AppLibStr_CalcALLSizeFromROMFS
 * Calculate needed String buffer size from string.bin in ROMFS
 * This Buffer includes needed index/header of the String bin
 * String buffer structure:
 *      APPLIB_GRAPHIC_STR_BIN_INFO_s     String bin info
 *      LangOffsetTable                 1D Language offset array(LangNum*UINT32)
 *      LangSizeTable                   1D Language size array(LangNum*UINT32)
 *      StrDescTable                    2D String description array, [Lang0Str0 Lang0Str1 Lang0Str2 Lang1Str0 Lang1Str1 Lang1Str2]
 *      MsgData                         Message raw data
 * @param[in] fileName - indicated which .bin file generated by AmbaGUIGen
 * @param[out] langNum - Returned language number in String bin file.
 * @param[out] msgNum - Returned message number in String bin file.
 * @param[out] strBufferSize - Returned string buffer size
 * @param[out] tmpBufferSize - Returned string tmp buffer size
 * @return AMP_OK on success
 */
int AppLibStr_CalcALLSizeFromROMFS(const char *fileName,
                                      UINT32 *langNum,
                                      UINT32 *msgNum,
                                      UINT32 *strBufferSize,
                                      UINT32 *tmpBufferSize)
{
    APPLIB_GRAPHIC_STR_BIN_HEADER_s Header;
    UINT32 FileSize;

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "AppLibStr_CalcALLSizeFromROMFS start");
    if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, fileName) == 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "AppLibStr_CalcALLSizeFromROMFS %s is not exist.", fileName);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    } else {
        FileSize = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, fileName, 0x0);
        AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&Header, sizeof(APPLIB_GRAPHIC_STR_BIN_HEADER_s), 0);
        *langNum = Header.LangNum;
        *msgNum = Header.MsgNum;

//      *strBufferSize = sizeof(APPLIB_GRAPHIC_STR_BIN_INFO_s)
//                       + 2*Header.LangNum*sizeof(UINT32)  //LangSizeTable, LangOffsetTable
//                       + Header.LangNum*Header.MsgNum*sizeof(APPLIB_GRAPHIC_STR_BIN_DESC_s)     //StrDescTable
//                       + MsgDataSize  //MsgData
//      MsgDataSize = FileSize
//                    - sizeof(APPLIB_GRAPHIC_STR_BIN_HEADER_s)
//                    - 2*Header.LangNum*sizeof(UINT32)             //LangSizeTable, LangOffsetTable
//                    - Header.LangNum*Header.MsgNum*sizeof(UINT32) //Message offset table
        *strBufferSize = sizeof(APPLIB_GRAPHIC_STR_BIN_INFO_s)
                         - sizeof(APPLIB_GRAPHIC_STR_BIN_HEADER_s)
                         + Header.LangNum*Header.MsgNum*(sizeof(APPLIB_GRAPHIC_STR_BIN_DESC_s) - sizeof(UINT32))
                         + FileSize;
        *tmpBufferSize = Header.MsgNum*sizeof(UINT32);
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "File:0x%x LangNum:%d MsgNum:%d strBufferSize:0x%x",
                FileSize, *langNum, *msgNum, *strBufferSize);
    }//AmbaROM_FileExists(AMBA_ROM_SYS_DATA, fileName) == 0

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "AppLibStr_CalcALLSizeFromROMFS does Not support anymore!");
    return AMP_OK;
}

/**
 * AppLibStr_CreateAllObjFromROMFS
 * Load all strings from .bin in ROMFS,
 * Use descUIObj to describe dstBmpObj struct
 * the String content will NOT be loaded until drawing.
 * Construct indexes in strBuf
 * @param[in] fileName - .bin file name generated by AmbaGUIGen.
 * @param[in] strBuf - An allocated buffer for loading strings, and build string index,
 *                     including: APPLIB_GRAPHIC_STR_BIN_HEADER_s, LangOffsetTable, LangSizeTable, StrDescTable
 * @param[in] tmpBuf - An allocated buffer for loading strings table, can be release after this function returned.
 * @param[in] descUIObj - Reference UI obj array, contains String description.
 * @param[out] dstBmpObj - Allocated destination Graphic object array.
 * @return int
 */
int AppLibStr_CreateAllObjFromROMFS(const char *fileName,
                                       void *strBuf,
                                       void *tmpBuf,
                                       APPLIB_GRAPHIC_UIOBJ_s *descUIObj,
                                       APPLIB_GRAPHIC_OBJ_s *dstStrObjArr)
{
    /* Error check */
    if ((strBuf == NULL) || (dstStrObjArr == NULL)) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "AppLibStr_CreateAllObjFromROMFS BMPBuf:0x%x, dstBmpObjArr:0x%x invalid.", strBuf, dstStrObjArr);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }
    APPLIB_GRAPHIC_STR_BIN_HEADER_s Header;
    APPLIB_GRAPHIC_STR_BIN_INFO_s *SInfo;
    APPLIB_GRAPHIC_STR_BIN_DESC_s *Desc;
    APPLIB_GRAPHIC_STR_CNT_s *StrCnt;
    UINT32 LIdx, MIdx, Offset, MsgObjIdx, srcSize, srcOff;
    UINT32 *MsgOffTable;
    UINT8 *dstPtr;
    WCHAR *MsgBase;

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "AppLibStr_CreateAllObjFromROMFS start");
    if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, fileName) == 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STING, "AppLibStr_CreateAllObjFromROMFS %s is not exist.", fileName);
        return AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE;
    }

    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&Header, sizeof(APPLIB_GRAPHIC_STR_BIN_HEADER_s), 0);

    //Init strBuf,
    SInfo = (APPLIB_GRAPHIC_STR_BIN_INFO_s *)strBuf;
    strncpy(SInfo->BinFileName, fileName, strlen(fileName));
    SInfo->LangNum = Header.LangNum;
    SInfo->MsgNum = Header.MsgNum;
    //Init BMPBuf, descArr
    SInfo->LangSizeTable = (UINT32*)((UINT8*)strBuf + sizeof(APPLIB_GRAPHIC_STR_BIN_INFO_s));
    SInfo->LangOffsetTable = (UINT32*)((UINT8*)SInfo->LangSizeTable + SInfo->LangNum*sizeof(UINT32));
    SInfo->StrDescTable = (APPLIB_GRAPHIC_STR_BIN_DESC_s *)((UINT8*)SInfo->LangOffsetTable + SInfo->LangNum*sizeof(UINT32));
    MsgBase = (WCHAR*)((UINT8*)SInfo->StrDescTable + SInfo->LangNum*SInfo->MsgNum*sizeof(APPLIB_GRAPHIC_STR_BIN_DESC_s));
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "S:0x%x L:%d(0x%x) M:%d(0x%x) LangSizeTable:0x%x LangOffsetTable:0x%x StrDescTable:0x%x",
            SInfo, SInfo->LangNum, &SInfo->LangNum, SInfo->MsgNum, &SInfo->MsgNum,
            SInfo->LangSizeTable, SInfo->LangOffsetTable, SInfo->StrDescTable);
    Offset = sizeof(APPLIB_GRAPHIC_STR_BIN_HEADER_s);
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8 *)SInfo->LangSizeTable, 2*SInfo->LangNum*sizeof(UINT32), Offset);
    //AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&SInfo->LangSizeTable, SInfo->LangNum*sizeof(UINT32), Offset);
    //SInfo->LangOffsetTable = (UINT32*)(SInfo->LangSizeTable + SInfo->LangNum*sizeof(UINT32));
    //Offset += SInfo->LangNum*sizeof(UINT32);
    //AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)&SInfo->LangOffsetTable, SInfo->LangNum*sizeof(UINT32), Offset);

    //Init msgTable
    for (LIdx=0; LIdx < (SInfo->LangNum); LIdx++) {
        //load msg offset table from bin
        Offset = SInfo->LangOffsetTable[LIdx];
        AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, (UINT8*)tmpBuf, SInfo->MsgNum*sizeof(UINT32), Offset);
        MsgOffTable = (UINT32*)tmpBuf;
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "L:%d t:0x%x off:0x%x M:0x%x MOT:0x%x", LIdx, tmpBuf, Offset, MsgBase, MsgOffTable);

        for (MIdx=0; MIdx < (SInfo->MsgNum); MIdx++) {
            //Create String Obj
            MsgObjIdx = LIdx*SInfo->MsgNum+MIdx;
            Desc = &(SInfo->StrDescTable[MsgObjIdx]);
            Desc->Count = 0;
            Desc->Flags = APPLIB_GRAPHIC_STR_BIN_INFO_LOAD;
            Desc->Ptr = (WCHAR*)((UINT8*)MsgBase + MsgOffTable[MIdx]);
//                Desc->Offset = SInfo->LangSizeTable[LIdx] + MsgOffTable[MIdx];
            Desc->Offset = MsgOffTable[MIdx];
            //Desc->Size = MsgOffset[MIdx+1] - MsgOffset[MIdx];
            if (MIdx < SInfo->MsgNum-1) {
                Desc->Size = MsgOffTable[MIdx+1] - MsgOffTable[MIdx];
            } else {
                //Desc->Size = MsgOffTable[MIdx];
                Desc->Size = SInfo->LangSizeTable[LIdx] - (MsgOffTable[MIdx] - MsgOffTable[0]);
            }
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "LIdx:%d MIdx:%3d MsgObjIdx:%d Desc:0x%x UI:0x%x dst:0x%x Off:0x%8x size:0x%8x Ptr:0x%x",
                    LIdx, MIdx, MsgObjIdx, Desc, &descUIObj[MsgObjIdx], &dstStrObjArr[MsgObjIdx],
                    Desc->Offset, Desc->Size, Desc->Ptr);
            descUIObj[MsgObjIdx].Cnt.Str.StrDesc = Desc;
            descUIObj[MsgObjIdx].Cnt.Str.StrInfo = SInfo;

            dstStrObjArr[MsgObjIdx].Area.X = descUIObj[MsgObjIdx].Area.X;
            dstStrObjArr[MsgObjIdx].Area.Y = descUIObj[MsgObjIdx].Area.Y;
            dstStrObjArr[MsgObjIdx].Area.Width = descUIObj[MsgObjIdx].Area.Width;      //pass original setting into new obj
            dstStrObjArr[MsgObjIdx].Area.Height = descUIObj[MsgObjIdx].Area.Height;    //canvas will altered then
            dstStrObjArr[MsgObjIdx].Alpha = descUIObj[MsgObjIdx].Alpha;
            dstStrObjArr[MsgObjIdx].Layer = descUIObj[MsgObjIdx].Layer;
            dstStrObjArr[MsgObjIdx].Group = descUIObj[MsgObjIdx].Group;
            dstStrObjArr[MsgObjIdx].Show = descUIObj[MsgObjIdx].Show;
            dstStrObjArr[MsgObjIdx].Stat = OBJ_STAT_NORMAL;
            dstStrObjArr[MsgObjIdx].Content = (void *)&descUIObj[MsgObjIdx].Cnt.Str;
            dstStrObjArr[MsgObjIdx].CalcArea_f = _AppLibStr_CalcArea;
            dstStrObjArr[MsgObjIdx].Dump_f = _AppLibStr_Dump;
            dstStrObjArr[MsgObjIdx].Draw_f = _AppLibStr_Draw;
        }
    }
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "AppLibStr_CreateAllObjFromROMFS init done");

    //load all
    for (LIdx=0; LIdx<SInfo->LangNum; LIdx++) {
        dstPtr = (UINT8 *)((UINT8 *)MsgBase + LIdx*SInfo->MsgNum*sizeof(APPLIB_GRAPHIC_STR_BIN_DESC_s));
        srcOff = SInfo->LangOffsetTable[LIdx] + SInfo->MsgNum*sizeof(UINT32);
        srcSize = SInfo->LangSizeTable[LIdx];
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "dstPtr:0x%x srcSize:0x%x srcOff:0x%x", dstPtr, srcSize, srcOff);
        AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, fileName, dstPtr, srcSize, srcOff);
    }

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_STING, "AppLibBMP_InitALLFromROMFS load done");
    return AMP_OK;
}
#endif

