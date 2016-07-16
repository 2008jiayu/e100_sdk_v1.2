/**
 * @file applib/src/graphics/stamp/ApplibGraphics_Stamp.c
 *
 * Header of stamp
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

#include "Stamp.h"
#include <applib.h>
#include <graphics/stamp/ApplibGraphics_Stamp.h>
#include <AmbaUtility.h>
extern int AppLibRender_Init(APPLIB_GRAPHIC_RENDER_s *render);

//static APPLIB_GRAPHIC_BLEND_BUF_INFO_s BlendBuf[32] = {0};

/* Variables */
static UINT32 *BlendingBufAddr = NULL;
//static UINT32 *YBufCurrAddr = NULL;   // for temp record
//static UINT32 *UVBufCurrAddr = NULL;  // for temp record

static APPLIB_GRAPHIC_BLEND_BUF_INFO_s blendBufInfo[32] = {0};
#ifdef CONFIG_APP_ARD
static APPLIB_GRAPHIC_RENDER_s blendRenderY[3], blendRenderUV[3], blendRenderAlphaY[3], blendRenderAlphaUV[3];
#else
static APPLIB_GRAPHIC_RENDER_s blendRenderY, blendRenderUV, blendRenderAlphaY, blendRenderAlphaUV;
#endif
static UINT8 *clut = NULL;

static UINT8 dumpClutFlg = 0;

#define BLENDING_BUF_NUM 32
#define TRANSPARENT_COLOR_INDEX_8BIT 0

static void _AppLibBlendBuffer_SaveY(UINT8 blendBufId)
{
    void *get;
    UINT32 val = 0;
    WCHAR TempFn[64];
    char FT[64] = {'\0'};
    AMBA_FS_FILE *File = NULL;
    AMBA_FS_STAT Fstat;
    APPLIB_GRAPHIC_RENDER_s *render = NULL;
    UINT32 FileSize = 0, bufWidth = 0;
    UINT8 *TmpbufRaw;
    static UINT8 *Tmpbuf;
    UINT32 i, j;
    int ReturnValue = 0;
    WCHAR mode[3] = {'w','b','\0'};

    // .y
    AmbaUtility_Unicode2Ascii(TempFn, FT);
    AmbaUtility_Ascii2Unicode("C:\\Stamp.y", TempFn);
#ifdef CONFIG_APP_ARD	
    render = &blendRenderY[blendBufId];
#else
	render = &blendRenderY;
#endif
    AmbaFS_Stat((const char *)TempFn, &Fstat);
    FileSize = blendBufInfo[blendBufId].Width * blendBufInfo[blendBufId].Height;
    bufWidth = blendBufInfo[blendBufId].Width;

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&Tmpbuf, (void **)&TmpbufRaw, FileSize, 32);
    if (ReturnValue != OK) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STAMP, "[Applib - StillEnc] <SingleCapture> NC_DDR alloc scriptAddr fail (%u)!", 128*10);
        return;
    }

    memset(Tmpbuf, 0x0, FileSize);
    File = AmbaFS_fopen((char const *)TempFn,(char const *) mode);

    for (j=0; j<render->BufHeight; j++) {
        render->MoveTo_f(render, &get, 0, j);
        for (i=0; i<bufWidth; i++) {
            render->GetPixel_f(get, &val);
            render->MoveNext_f(&get);
            Tmpbuf[i] = val;
        }
          AmbaFS_fwrite(Tmpbuf, sizeof(UINT8), bufWidth, File);
    }
    AmbaFS_FSync(File);
    AmbaFS_fclose(File);
    AmbaKAL_BytePoolFree(TmpbufRaw);

    // .txt
    AmbaUtility_Unicode2Ascii(TempFn, FT);
    AmbaUtility_Ascii2Unicode("C:\\StampY.txt", TempFn);
#ifdef CONFIG_APP_ARD	
	render = &blendRenderY[blendBufId];
#else
    render = &blendRenderY;
#endif

    AmbaFS_Stat((const char *)TempFn, &Fstat);
#ifdef CONFIG_APP_ARD	
	FileSize = blendRenderY[blendBufId].BufPitch * blendRenderY[blendBufId].BufHeight;//blendBufInfo[blendBufId].Width * blendBufInfo[blendBufId].Height;
    bufWidth = blendRenderY[blendBufId].BufPitch;//blendBufInfo[blendBufId].Width;
#else
	FileSize = blendRenderY.BufPitch * blendRenderY.BufHeight;//blendBufInfo[blendBufId].Width * blendBufInfo[blendBufId].Height;
	bufWidth = blendRenderY.BufPitch;//blendBufInfo[blendBufId].Width;
#endif
	ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&Tmpbuf, (void **)&TmpbufRaw, FileSize, 32);
    if (ReturnValue != OK) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STAMP, "[Applib - StillEnc] <SingleCapture> NC_DDR alloc scriptAddr fail (%u)!", 128*10);
        return;
    }

    memset(Tmpbuf, 0x0, FileSize);
    File = AmbaFS_fopen((char const *)TempFn,(char const *) mode);

    for (j=0; j<render->BufHeight; j++) {
        render->MoveTo_f(render, &get, 0, j);
        for (i=0; i<bufWidth; i++) {
            render->GetPixel_f(get, &val);
            render->MoveNext_f(&get);
            Tmpbuf[i] = val;
        }
          AmbaFS_fwrite(Tmpbuf, sizeof(UINT8), bufWidth, File);
    }
    AmbaFS_FSync(File);
    AmbaFS_fclose(File);
    AmbaKAL_BytePoolFree(TmpbufRaw);
}

static void _AppLibBlendBuffer_SaveUV(UINT8 blendBufId)
{
    void *get;
    UINT32 val = 0;
    WCHAR TempFn[64];
    char FT[64] = {'\0'};
    AMBA_FS_FILE *File = NULL;
    AMBA_FS_STAT Fstat;
    APPLIB_GRAPHIC_RENDER_s *render = NULL;
    int ReturnValue = 0;
    UINT32 i, j;    
    WCHAR mode[3] = {'w','b','\0'};
#ifdef CONFIG_APP_ARD	
    UINT32 FileSize = blendRenderUV[blendBufId].BufPitch * blendRenderUV[blendBufId].BufHeight;//blendBufInfo[blendBufId].Width * blendBufInfo[blendBufId].Height;
    UINT32 bufWidth = blendRenderUV[blendBufId].BufPitch;//blendBufInfo[blendBufId].Width;
#else
	UINT32 FileSize = blendRenderUV.BufPitch * blendRenderUV.BufHeight;//blendBufInfo[blendBufId].Width * blendBufInfo[blendBufId].Height;
	UINT32 bufWidth = blendRenderUV.BufPitch;//blendBufInfo[blendBufId].Width;
#endif
    UINT8 *TmpbufRaw;
    static UINT8 *Tmpbuf;

    AmbaUtility_Unicode2Ascii(TempFn, FT);
    AmbaUtility_Ascii2Unicode("C:\\Stamp.uv", TempFn);
#ifdef CONFIG_APP_ARD		
    render = &blendRenderUV[blendBufId];
#else
	render = &blendRenderUV;
#endif

    AmbaFS_Stat((const char *)TempFn, &Fstat);
    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&Tmpbuf, (void **)&TmpbufRaw, FileSize, 32);
    if (ReturnValue != OK) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_STAMP, "[Applib - StillEnc] <SingleCapture> NC_DDR alloc scriptAddr fail (%u)!", 128*10);
        return;
    }

    memset(Tmpbuf, 0x0, FileSize);
    File = AmbaFS_fopen((char const *)TempFn,(char const *) mode);

    for (j=0; j<render->BufHeight; j++) {
        render->MoveTo_f(render, &get, 0, j);
        for (i=0; i<bufWidth; i++) {
            render->GetPixel_f(get, &val);
            render->MoveNext_f(&get);
            Tmpbuf[i] = val;
        }
          AmbaFS_fwrite(Tmpbuf, sizeof(UINT8), bufWidth, File);
    }
    AmbaFS_FSync(File);
    AmbaFS_fclose(File);
    AmbaKAL_BytePoolFree(TmpbufRaw);
}

#if 0
static void _AppLibBlendBuffer_DumpRaw(UINT8 blendBufId)
{
    void *putY, *putUV;

    // boundary
    UINT32 loop = 0;
    UINT32 line = 0;

    // for Y
    UINT32 W = (blendRenderY.BufPitch%32) ? (blendRenderY.BufPitch/32+1)*32 : blendRenderY.BufPitch;
    while (line < blendRenderY.BufHeight) {
        loop = 0;
        blendRenderY.MoveTo_f(&blendRenderY, &putY, 0, line);

        while (loop < W) {
            // put Y
            blendRenderY.PlotPixel_f(putY, 0xFF);
            blendRenderY.MoveNext_f(&putY);
            loop++;
        }
        line++;
    }

    // put UV
    W = (blendRenderUV.BufPitch%32) ? (blendRenderUV.BufPitch/32+1)*32 : blendRenderUV.BufPitch;
    line = 0;
    while (line < blendRenderUV.BufHeight) {
        loop = 0;
        blendRenderUV.MoveTo_f(&blendRenderUV, &putUV, 0, line);

        while (loop < W) {
            if (loop<130) {
                blendRenderUV.PlotPixel_f(putUV, 0xFF);
                blendRenderUV.MoveNext_f(&putUV);
            } else {
                blendRenderUV.PlotPixel_f(putUV, 0x0);
                blendRenderUV.MoveNext_f(&putUV);
            }

            loop++;
        }
        line++;
    }

    //_AppLibBlendBuffer_SaveY(blendBufId);
    //_AppLibBlendBuffer_SaveUV(blendBufId);
}
#endif

static void _AppLibBlend_DumpClut(void)
{
    WCHAR TempFn[64];
    char FT[64] = {'\0'};
    AMBA_FS_FILE *File = NULL;
    WCHAR mode[3] = {'w','b','\0'};
    UINT32 j;

    if (dumpClutFlg == 0) {
        return;
    }
    AmbaUtility_Unicode2Ascii(TempFn, FT);
    AmbaUtility_Ascii2Unicode("C:\\Clut.txt", TempFn);

    File = AmbaFS_fopen((char const *)TempFn,(char const *) mode);
    for(j=0; j<(256*4); j++){
        AmbaFS_fwrite((const void*)clut, sizeof(UINT8), 1, File);
        clut++;
    }
    AmbaFS_FSync(File);
    AmbaFS_fclose(File);
}

static void _AppLibBlendBuffer_CovertColorSpace_RGB2YUV420(UINT8 blendBufId, APPLIB_GRAPHIC_SOURCE_BUF_INFO_s *srcInfo)
{
    APPLIB_GRAPHIC_RENDER_s *srcRender = srcInfo->SourceRender;

    void *get, *putY, *putUV, *putAlphaY, *putAlphaUV;
    UINT32 dataY, dataU, dataV;
    UINT32 val;
    UINT8 dataR, dataG, dataB;
    UINT32 X1 = srcInfo->SourceDisplayBox.X;
    UINT32 Y1 = srcInfo->SourceDisplayBox.Y;
    UINT32 X2 = X1 + srcInfo->SourceDisplayBox.Width;
    UINT32 Y2 = Y1 + srcInfo->SourceDisplayBox.Height;
    // boundary
    UINT32 loop = 0;
    UINT32 end = 0;
    UINT32 line = 0;
    UINT8 switchIdx = 0;
    UINT32 targetLine = 0;

    // for Y
    loop = X1;
    end = X2;
    line = Y1;

    while (line < Y2) {
        loop = X1;
        switchIdx = 0;
        srcRender->MoveTo_f(srcInfo->SourceRender, &get, loop, line);
#ifdef CONFIG_APP_ARD
		blendRenderY[blendBufId].MoveTo_f(&blendRenderY[blendBufId], &putY, 0, (line-Y1));
		blendRenderUV[blendBufId].MoveTo_f(&blendRenderUV[blendBufId], &putUV, 0, targetLine);
		blendRenderAlphaY[blendBufId].MoveTo_f(&blendRenderAlphaY[blendBufId], &putAlphaY, 0, targetLine);
		blendRenderAlphaUV[blendBufId].MoveTo_f(&blendRenderAlphaUV[blendBufId], &putAlphaUV, 0, targetLine);
#else
        blendRenderY.MoveTo_f(&blendRenderY, &putY, 0, (line-Y1));
        blendRenderUV.MoveTo_f(&blendRenderUV, &putUV, 0, targetLine);
        blendRenderAlphaY.MoveTo_f(&blendRenderAlphaY, &putAlphaY, 0, targetLine);
        blendRenderAlphaUV.MoveTo_f(&blendRenderAlphaUV, &putAlphaUV, 0, targetLine);
#endif		

        if ((line % 2) == 0) {
            while (loop < end) {
                /* even */
                srcRender->GetPixel_f(get, &val);
                srcRender->MoveNext_f(&get);
                dataR = (UINT8)((val & 0x00FF0000) >> 16); // R
                dataG = (UINT8)((val & 0x0000FF00) >> 8);  // G
                dataB = (UINT8)(val & 0x000000FF);       // B

                // put Y
                dataY = ( (66 * dataR + 129 * dataG + 25 * dataB) >> 8 ) + 16;
                dataY = (dataY > 255) ? 255 : dataY;
#ifdef CONFIG_APP_ARD				
                blendRenderY[blendBufId].PlotPixel_f(putY, dataY);
                blendRenderY[blendBufId].MoveNext_f(&putY);
#else
				blendRenderY.PlotPixel_f(putY, dataY);
				blendRenderY.MoveNext_f(&putY);
#endif

                // put U and V
                dataU = ( (-38 * dataR - 74 * dataG + 112 * dataB) >> 8 ) + 128;
                dataU = (dataU > 255) ? 255 : dataU;
                dataV = ( (112 * dataR - 94 * dataG - 18 * dataB) >> 8 ) + 128;
                dataV = (dataV > 255) ? 255 : dataV;
                if ((switchIdx % 2) == 0) {
#ifdef CONFIG_APP_ARD						
                    blendRenderUV[blendBufId].PlotPixel_f(putUV, dataU);
                    blendRenderUV[blendBufId].MoveNext_f(&putUV);
#else
					blendRenderUV.PlotPixel_f(putUV, dataU);
					blendRenderUV.MoveNext_f(&putUV);
#endif
                } else {
#ifdef CONFIG_APP_ARD		
					blendRenderUV[blendBufId].PlotPixel_f(putUV, dataV);
					blendRenderUV[blendBufId].MoveNext_f(&putUV);
#else
                    blendRenderUV.PlotPixel_f(putUV, dataV);
                    blendRenderUV.MoveNext_f(&putUV);
#endif					
                }
#ifdef CONFIG_APP_ARD		
                // put alpha Y
                blendRenderAlphaY[blendBufId].PlotPixel_f(putAlphaY, 0x0);
                blendRenderAlphaY[blendBufId].MoveNext_f(&putAlphaY);

                // put alpha UV
                blendRenderAlphaUV[blendBufId].PlotPixel_f(putAlphaUV, 0x0);
                blendRenderAlphaUV[blendBufId].MoveNext_f(&putAlphaUV);
#else
				// put alpha Y
				blendRenderAlphaY.PlotPixel_f(putAlphaY, 0x0);
				blendRenderAlphaY.MoveNext_f(&putAlphaY);

				// put alpha UV
				blendRenderAlphaUV.PlotPixel_f(putAlphaUV, 0x0);
				blendRenderAlphaUV.MoveNext_f(&putAlphaUV);
#endif
                // update index
                switchIdx++;
                loop++;
            }
            targetLine++;
        } else {
            while (loop < end) {
                srcRender->GetPixel_f(get, &val);
                srcRender->MoveNext_f(&get);
                dataR = (UINT8)((val & 0x00FF0000) >> 16); // R
                dataG = (UINT8)((val & 0x0000FF00) >> 8);  // G
                dataB = (UINT8)(val & 0x000000FF);       // B

                // put Y
                dataY = ( (66 * dataR + 129 * dataG + 25 * dataB) >> 8 ) + 16;
                dataY = (dataY > 255) ? 255 : dataY;
#ifdef CONFIG_APP_ARD					
                blendRenderY[blendBufId].PlotPixel_f(putY, dataY);
                blendRenderY[blendBufId].MoveNext_f(&putY);

                // put alpha Y
                blendRenderAlphaY[blendBufId].PlotPixel_f(putAlphaY, 0x0);
                blendRenderAlphaY[blendBufId].MoveNext_f(&putAlphaY);
#else
				blendRenderY.PlotPixel_f(putY, dataY);
				blendRenderY.MoveNext_f(&putY);

				// put alpha Y
				blendRenderAlphaY.PlotPixel_f(putAlphaY, 0x0);
				blendRenderAlphaY.MoveNext_f(&putAlphaY);
#endif

                // update index
                loop += 1;
            }
        }
        line++;
    }

    //_AppLibBlendBuffer_SaveY(blendBufId);
    //_AppLibBlendBuffer_SaveUV(blendBufId);
}

static void _AppLibBlendBuffer_CovertColorSpace_RGB2YUV420_8bit(UINT8 blendBufId, APPLIB_GRAPHIC_SOURCE_BUF_INFO_s *srcInfo)
{
    APPLIB_GRAPHIC_RENDER_s *srcRender = srcInfo->SourceRender;

    void *get, *putY, *putUV, *putAlphaY, *putAlphaUV;
    UINT32 dataY, dataU, dataV;
    UINT32 val;
    UINT32 X1 = srcInfo->SourceDisplayBox.X;
    UINT32 Y1 = srcInfo->SourceDisplayBox.Y;
    UINT32 X2 = X1 + srcInfo->SourceDisplayBox.Width;
    UINT32 Y2 = Y1 + srcInfo->SourceDisplayBox.Height;
    // boundary
    UINT32 loop = 0;
    UINT32 end = 0;
    UINT32 line = 0;
    UINT8 switchIdx = 0;
    UINT32 targetLine = 0;

    // for Y
    loop = X1;
    end = X2;
    line = Y1;

    while(line < Y2){
        loop = X1;
        switchIdx = 0;
        srcRender->MoveTo_f(srcInfo->SourceRender, &get, loop, line);
#ifdef CONFIG_APP_ARD			
        blendRenderY[blendBufId].MoveTo_f(&blendRenderY[blendBufId], &putY, 0, (line-Y1));
        blendRenderUV[blendBufId].MoveTo_f(&blendRenderUV[blendBufId], &putUV, 0, targetLine);
        blendRenderAlphaY[blendBufId].MoveTo_f(&blendRenderAlphaY[blendBufId], &putAlphaY, 0, targetLine);
        blendRenderAlphaUV[blendBufId].MoveTo_f(&blendRenderAlphaUV[blendBufId], &putAlphaUV, 0, targetLine);
#else
		blendRenderY.MoveTo_f(&blendRenderY, &putY, 0, (line-Y1));
		blendRenderUV.MoveTo_f(&blendRenderUV, &putUV, 0, targetLine);
		blendRenderAlphaY.MoveTo_f(&blendRenderAlphaY, &putAlphaY, 0, targetLine);
		blendRenderAlphaUV.MoveTo_f(&blendRenderAlphaUV, &putAlphaUV, 0, targetLine);
#endif

        if((line % 2) == 0){
            while(loop < end){
                /* even */
                srcRender->GetPixel_f(get, &val);
                srcRender->MoveNext_f(&get);

                // put Y
                dataY = clut[(val * 4) + 2];
#ifdef CONFIG_APP_ARD					
                blendRenderY[blendBufId].PlotPixel_f(putY, dataY);
                blendRenderY[blendBufId].MoveNext_f(&putY);
#else
				blendRenderY.PlotPixel_f(putY, dataY);
				blendRenderY.MoveNext_f(&putY);
#endif

                // put U and V
                dataU = clut[(val * 4) + 1];
                dataV = clut[(val * 4)];
                if((switchIdx % 2) == 0){
#ifdef CONFIG_APP_ARD		
					blendRenderUV[blendBufId].PlotPixel_f(putUV, dataU);
					blendRenderUV[blendBufId].MoveNext_f(&putUV);
#else
                    blendRenderUV.PlotPixel_f(putUV, dataU);
                    blendRenderUV.MoveNext_f(&putUV);
#endif					
                } else{
#ifdef CONFIG_APP_ARD	
					blendRenderUV[blendBufId].PlotPixel_f(putUV, dataV);
					blendRenderUV[blendBufId].MoveNext_f(&putUV);
#else
                    blendRenderUV.PlotPixel_f(putUV, dataV);
                    blendRenderUV.MoveNext_f(&putUV);
#endif					
                }

#ifdef CONFIG_APP_ARD	
                // put alpha Y
                if (val == TRANSPARENT_COLOR_INDEX_8BIT) {
                    blendRenderAlphaY[blendBufId].PlotPixel_f(putAlphaY, 0xFF);
                } else {
                    blendRenderAlphaY[blendBufId].PlotPixel_f(putAlphaY, 0x0);
                }
                blendRenderAlphaY[blendBufId].MoveNext_f(&putAlphaY);

                // put alpha UV
                if (val == TRANSPARENT_COLOR_INDEX_8BIT) {
                    blendRenderAlphaUV[blendBufId].PlotPixel_f(putAlphaUV, 0xFF);
                } else {
                    blendRenderAlphaUV[blendBufId].PlotPixel_f(putAlphaUV, 0x0);
                }
                blendRenderAlphaUV[blendBufId].MoveNext_f(&putAlphaUV);
#else
				// put alpha Y
				if (val == TRANSPARENT_COLOR_INDEX_8BIT) {
					blendRenderAlphaY.PlotPixel_f(putAlphaY, 0xFF);
				} else {
					blendRenderAlphaY.PlotPixel_f(putAlphaY, 0x0);
				}
				blendRenderAlphaY.MoveNext_f(&putAlphaY);

				// put alpha UV
				if (val == TRANSPARENT_COLOR_INDEX_8BIT) {
					blendRenderAlphaUV.PlotPixel_f(putAlphaUV, 0xFF);
				} else {
					blendRenderAlphaUV.PlotPixel_f(putAlphaUV, 0x0);
				}
				blendRenderAlphaUV.MoveNext_f(&putAlphaUV);
#endif
                // update index
                switchIdx++;
                loop++;
            }
            targetLine++;
        } else {
            while(loop < end){
                srcRender->GetPixel_f(get, &val);
                srcRender->MoveNext_f(&get);

#ifdef CONFIG_APP_ARD	
                // put Y
                dataY = clut[(val * 4) + 2];
                blendRenderY[blendBufId].PlotPixel_f(putY, dataY);
                blendRenderY[blendBufId].MoveNext_f(&putY);

                // put alpha Y
                if (val == TRANSPARENT_COLOR_INDEX_8BIT) {
                    blendRenderAlphaY[blendBufId].PlotPixel_f(putAlphaY, 0xFF);
                } else {
                    blendRenderAlphaY[blendBufId].PlotPixel_f(putAlphaY, 0x0);
                }
                blendRenderAlphaY[blendBufId].MoveNext_f(&putAlphaY);
#else
				// put Y
				dataY = clut[(val * 4) + 2];
				blendRenderY.PlotPixel_f(putY, dataY);
				blendRenderY.MoveNext_f(&putY);

				// put alpha Y
				if (val == TRANSPARENT_COLOR_INDEX_8BIT) {
					blendRenderAlphaY.PlotPixel_f(putAlphaY, 0xFF);
				} else {
					blendRenderAlphaY.PlotPixel_f(putAlphaY, 0x0);
				}
				blendRenderAlphaY.MoveNext_f(&putAlphaY);
#endif


                // update index
                loop += 1;
            }
        }
        line++;
    }

    //_AppLibBlendBuffer_SaveY(blendBufId);
    //_AppLibBlendBuffer_SaveUV(blendBufId);
}

static void _AppLibBlendBuffer_CovertColorSpace_RGB2YUV422(UINT8 blendBufId, APPLIB_GRAPHIC_SOURCE_BUF_INFO_s *srcInfo)
{
    APPLIB_GRAPHIC_RENDER_s *srcRender = srcInfo->SourceRender;

    void *get, *putY, *putUV, *putAlphaY, *putAlphaUV;
    UINT32 dataY, dataU, dataV;
    UINT32 val;
    UINT8 dataR, dataG, dataB;
    UINT32 X1 = srcInfo->SourceDisplayBox.X;
    UINT32 Y1 = srcInfo->SourceDisplayBox.Y;
    UINT32 X2 = X1 + srcInfo->SourceDisplayBox.Width;
    UINT32 Y2 = Y1 + srcInfo->SourceDisplayBox.Height;
    // boundary
    UINT32 loop = 0;
    UINT32 end = 0;
    UINT32 line = 0;
    UINT8 switchIdx = 0;

    // for Y
    loop = X1;
    end = X2;
    line = Y1;

    while (line < Y2) {
        loop = X1;
        srcRender->MoveTo_f(srcInfo->SourceRender, &get, loop, line);
#ifdef CONFIG_APP_ARD			
        blendRenderY[blendBufId].MoveTo_f(&blendRenderY[blendBufId], &putY, 0, (line-Y1));
        blendRenderUV[blendBufId].MoveTo_f(&blendRenderUV[blendBufId], &putUV, 0, (line-Y1));
        blendRenderAlphaY[blendBufId].MoveTo_f(&blendRenderAlphaY[blendBufId], &putAlphaY, 0, (line-Y1));
        blendRenderAlphaUV[blendBufId].MoveTo_f(&blendRenderAlphaUV[blendBufId], &putAlphaUV, 0, (line-Y1));
#else
		blendRenderY.MoveTo_f(&blendRenderY, &putY, 0, (line-Y1));
		blendRenderUV.MoveTo_f(&blendRenderUV, &putUV, 0, (line-Y1));
		blendRenderAlphaY.MoveTo_f(&blendRenderAlphaY, &putAlphaY, 0, (line-Y1));
		blendRenderAlphaUV.MoveTo_f(&blendRenderAlphaUV, &putAlphaUV, 0, (line-Y1));
#endif

        while (loop < end) {
            srcRender->GetPixel_f(get, &val);
            srcRender->MoveNext_f(&get);
            dataR = (UINT8)((val & 0x00FF0000) >> 16); // R
            dataG = (UINT8)((val & 0x0000FF00) >> 8);  // G
            dataB = (UINT8)(val & 0x000000FF);       // B

            // put Y
            dataY = ( (66 * dataR + 129 * dataG + 25 * dataB) >> 8 ) + 16;
            dataY = (dataY > 255) ? 255 : dataY;
#ifdef CONFIG_APP_ARD				
            blendRenderY[blendBufId].PlotPixel_f(putY, dataY);
            blendRenderY[blendBufId].MoveNext_f(&putY);
#else
			blendRenderY.PlotPixel_f(putY, dataY);
			blendRenderY.MoveNext_f(&putY);
#endif

            // put U and V
            dataU = ( (-38 * dataR - 74 * dataG + 112 * dataB) >> 8 ) + 128;
            dataU = (dataU > 255) ? 255 : dataU;
            dataV = ( (112 * dataR - 94 * dataG - 18 * dataB) >> 8 ) + 128;
            dataV = (dataV > 255) ? 255 : dataV;

#ifdef CONFIG_APP_ARD	
            if ((switchIdx % 2) == 0) {
                blendRenderUV[blendBufId].PlotPixel_f(putUV, dataU);
                blendRenderUV[blendBufId].MoveNext_f(&putUV);
            } else {
                blendRenderUV[blendBufId].PlotPixel_f(putUV, dataV);
                blendRenderUV[blendBufId].MoveNext_f(&putUV);
            }

            // put alpha Y
            blendRenderAlphaY[blendBufId].PlotPixel_f(putAlphaY, 0x0);
            blendRenderAlphaY[blendBufId].MoveNext_f(&putAlphaY);

            // put alpha uv
            blendRenderAlphaUV[blendBufId].PlotPixel_f(putAlphaUV, 0x0);
            blendRenderAlphaUV[blendBufId].MoveNext_f(&putAlphaUV);
#else
			if ((switchIdx % 2) == 0) {
				blendRenderUV.PlotPixel_f(putUV, dataU);
				blendRenderUV.MoveNext_f(&putUV);
			} else {
				blendRenderUV.PlotPixel_f(putUV, dataV);
				blendRenderUV.MoveNext_f(&putUV);
			}

			// put alpha Y
			blendRenderAlphaY.PlotPixel_f(putAlphaY, 0x0);
			blendRenderAlphaY.MoveNext_f(&putAlphaY);

			// put alpha uv
			blendRenderAlphaUV.PlotPixel_f(putAlphaUV, 0x0);
			blendRenderAlphaUV.MoveNext_f(&putAlphaUV);
#endif

            // update index
            loop++;
            switchIdx++;
        }
        line++;
    }

    //_AppLibBlendBuffer_SaveY(blendBufId);
    //_AppLibBlendBuffer_SaveUV(blendBufId);
}

static void _AppLibBlendBuffer_CovertColorSpace_RGB2YUV422_8bit(UINT8 blendBufId, APPLIB_GRAPHIC_SOURCE_BUF_INFO_s *srcInfo)
{
    APPLIB_GRAPHIC_RENDER_s *srcRender = srcInfo->SourceRender;

    void *get, *putY, *putUV, *putAlphaY, *putAlphaUV;
    UINT32 dataY, dataU, dataV;
    UINT32 val;
    UINT32 X1 = srcInfo->SourceDisplayBox.X;
    UINT32 Y1 = srcInfo->SourceDisplayBox.Y;
    UINT32 X2 = X1 + srcInfo->SourceDisplayBox.Width;
    UINT32 Y2 = Y1 + srcInfo->SourceDisplayBox.Height;
    // boundary
    UINT32 loop = 0;
    UINT32 end = 0;
    UINT32 line = 0;
    UINT8 switchIdx = 0;

    // for Y
    loop = X1;
    end = X2;
    line = Y1;

    while(line < Y2){
        loop = X1;
        srcRender->MoveTo_f(srcInfo->SourceRender, &get, loop, line);
#ifdef CONFIG_APP_ARD		
        blendRenderY[blendBufId].MoveTo_f(&blendRenderY[blendBufId], &putY, 0, (line-Y1));
        blendRenderUV[blendBufId].MoveTo_f(&blendRenderUV[blendBufId], &putUV, 0, (line-Y1));
        blendRenderAlphaY[blendBufId].MoveTo_f(&blendRenderAlphaY[blendBufId], &putAlphaY, 0, (line-Y1));
        blendRenderAlphaUV[blendBufId].MoveTo_f(&blendRenderAlphaUV[blendBufId], &putAlphaUV, 0, (line-Y1));
#else
		blendRenderY.MoveTo_f(&blendRenderY, &putY, 0, (line-Y1));
		blendRenderUV.MoveTo_f(&blendRenderUV, &putUV, 0, (line-Y1));
		blendRenderAlphaY.MoveTo_f(&blendRenderAlphaY, &putAlphaY, 0, (line-Y1));
		blendRenderAlphaUV.MoveTo_f(&blendRenderAlphaUV, &putAlphaUV, 0, (line-Y1));
#endif

        while(loop < end){
            srcRender->GetPixel_f(get, &val);
            srcRender->MoveNext_f(&get);

            // put Y
            dataY = clut[(val * 4) + 2];
#ifdef CONFIG_APP_ARD					
            blendRenderY[blendBufId].PlotPixel_f(putY, dataY);
            blendRenderY[blendBufId].MoveNext_f(&putY);
#else
			blendRenderY.PlotPixel_f(putY, dataY);
			blendRenderY.MoveNext_f(&putY);
#endif

            // put U and V
            dataU = clut[(val * 4) + 1];
            dataV = clut[(val * 4)];

#ifdef CONFIG_APP_ARD	
            if((switchIdx % 2) == 0){
                blendRenderUV[blendBufId].PlotPixel_f(putUV, dataU);
                blendRenderUV[blendBufId].MoveNext_f(&putUV);
            } else{
                blendRenderUV[blendBufId].PlotPixel_f(putUV, dataV);
                blendRenderUV[blendBufId].MoveNext_f(&putUV);
            }

            // put alpha Y
            if (val == TRANSPARENT_COLOR_INDEX_8BIT) {
                blendRenderAlphaY[blendBufId].PlotPixel_f(putAlphaY, 0xFF);
            } else {
                blendRenderAlphaY[blendBufId].PlotPixel_f(putAlphaY, 0x0);
            }
            blendRenderAlphaY[blendBufId].MoveNext_f(&putAlphaY);

            // put alpha uv
            if (val == TRANSPARENT_COLOR_INDEX_8BIT) {
                blendRenderAlphaUV[blendBufId].PlotPixel_f(putAlphaUV, 0xFF);
            } else {
                blendRenderAlphaUV[blendBufId].PlotPixel_f(putAlphaUV, 0x0);
            }
            blendRenderAlphaUV[blendBufId].MoveNext_f(&putAlphaUV);
#else
			if((switchIdx % 2) == 0){
				blendRenderUV.PlotPixel_f(putUV, dataU);
				blendRenderUV.MoveNext_f(&putUV);
			} else{
				blendRenderUV.PlotPixel_f(putUV, dataV);
				blendRenderUV.MoveNext_f(&putUV);
			}

			// put alpha Y
			if (val == TRANSPARENT_COLOR_INDEX_8BIT) {
				blendRenderAlphaY.PlotPixel_f(putAlphaY, 0xFF);
			} else {
				blendRenderAlphaY.PlotPixel_f(putAlphaY, 0x0);
			}
			blendRenderAlphaY.MoveNext_f(&putAlphaY);

			// put alpha uv
			if (val == TRANSPARENT_COLOR_INDEX_8BIT) {
				blendRenderAlphaUV.PlotPixel_f(putAlphaUV, 0xFF);
			} else {
				blendRenderAlphaUV.PlotPixel_f(putAlphaUV, 0x0);
			}
			blendRenderAlphaUV.MoveNext_f(&putAlphaUV);
#endif

            // update index
            loop++;
            switchIdx++;
        }
        line++;
    }

    //_AppLibBlendBuffer_SaveY(blendBufId);
    //_AppLibBlendBuffer_SaveUV(blendBufId);
}

/**
 *  @brief Initialize stamp module
 *
 *  Initialize stamp module
 *
 *  @param [in] *blendingBufAddress the specidif blending buffer
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
void AppLibStamp_Init(void *blendingBufAddress)
{
    UINT32 i = 0;
    for (i=0; i<32; i++)
    {
        memset(&blendBufInfo[i], 0x0, sizeof(APPLIB_GRAPHIC_BLEND_BUF_INFO_s));
    }

    BlendingBufAddr = blendingBufAddress;
    //YBufCurrAddr = blendingBufAddress;
    //UVBufCurrAddr = blendingBufAddress;
}

/**
 *  @brief Set clut table
 *
 *  Set clut table
 *
 *  @param [in] *clutTable the clut table
 *
 *  @return
 *  @see
 */
void AppLibBlend_SetClutTable(UINT8 *clutTable)
{
    clut = clutTable;
}

/**
 *  @brief Add one new buffer for blending
 *
 *  Add one new buffer for blending
 *
 *  @param [in] stampAreaId the specific id for blending buffer
 *  @param [in] *sourceBufInfo the blending buffer
 *  @param [in] colorFormat the color format of source buffer
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
void AppLibBlend_AddBlendArea(UINT8 stampAreaId, APPLIB_GRAPHIC_SOURCE_BUF_INFO_s *sourceBufInfo, APPLIB_GRAPH_COLOR_FORMAT_e colorFormat)
{
    UINT8 i = 0;
    UINT32 offset = 0;
    UINT32 AlignedWidth = 0;
    UINT32 renderBufW = 0, renderBufH = 0;

    if (blendBufInfo[stampAreaId].LengthY != 0) {
        return;
    }

    /* Init blendBufInfo */
    blendBufInfo[stampAreaId].BlendBufId = stampAreaId;
    blendBufInfo[stampAreaId].X = sourceBufInfo->SourceDisplayBox.X;
    blendBufInfo[stampAreaId].Y = sourceBufInfo->SourceDisplayBox.Y;
    blendBufInfo[stampAreaId].Width = sourceBufInfo->SourceDisplayBox.Width;
    blendBufInfo[stampAreaId].Height = sourceBufInfo->SourceDisplayBox.Height;
    blendBufInfo[stampAreaId].BufPixelSize = 1;
    for (i=0; i<BLENDING_BUF_NUM; i++) {
#ifdef CONFIG_APP_ARD			
        offset += blendBufInfo[i].LengthY;
        offset += blendBufInfo[i].LengthUV;
        offset += blendBufInfo[i].LengthAlphaY;
        offset += blendBufInfo[i].LengthAlphaUV;
#else
		offset += blendBufInfo[stampAreaId].LengthY;
		offset += blendBufInfo[stampAreaId].LengthUV;
		offset += blendBufInfo[stampAreaId].LengthAlphaY;
		offset += blendBufInfo[stampAreaId].LengthAlphaUV;
#endif
    }
    AlignedWidth = (blendBufInfo[stampAreaId].Width%32) ? (((blendBufInfo[stampAreaId].Width>>5)+1)<<5) : (blendBufInfo[stampAreaId].Width);
    blendBufInfo[stampAreaId].LengthY = AlignedWidth * blendBufInfo[stampAreaId].Height;
    blendBufInfo[stampAreaId].LengthAlphaY = AlignedWidth * blendBufInfo[stampAreaId].Height;
    switch (colorFormat) {
        case COLOR_FORMAT_YUV420:
            blendBufInfo[stampAreaId].LengthUV = (AlignedWidth * blendBufInfo[stampAreaId].Height) >> 1;
            blendBufInfo[stampAreaId].LengthAlphaUV = (AlignedWidth * blendBufInfo[stampAreaId].Height) >> 1;
            break;
        case COLOR_FORMAT_YUV422:
        default:
            blendBufInfo[stampAreaId].LengthUV = AlignedWidth * blendBufInfo[stampAreaId].Height;
            blendBufInfo[stampAreaId].LengthAlphaUV = AlignedWidth * blendBufInfo[stampAreaId].Height;
            break;
    }

    blendBufInfo[stampAreaId].AddressY = ((UINT8*)BlendingBufAddr) + offset;
    blendBufInfo[stampAreaId].AddressUV = ((UINT8*)BlendingBufAddr) + offset + blendBufInfo[stampAreaId].LengthY;
    blendBufInfo[stampAreaId].AddressAlphaY = ((UINT8*)BlendingBufAddr + offset + blendBufInfo[stampAreaId].LengthY + blendBufInfo[stampAreaId].LengthUV);
    blendBufInfo[stampAreaId].AddressAlphaUV = ((UINT8*)BlendingBufAddr + offset + blendBufInfo[stampAreaId].LengthY + blendBufInfo[stampAreaId].LengthUV + blendBufInfo[stampAreaId].LengthAlphaY);

    /* Init render */
    switch (colorFormat) {
        case COLOR_FORMAT_YUV420:
            renderBufW = AlignedWidth;
            renderBufH = blendBufInfo[stampAreaId].Height >> 1;
            break;
        case COLOR_FORMAT_YUV422:
        default:
            renderBufW = AlignedWidth;
            renderBufH = blendBufInfo[stampAreaId].Height;
            break;
    }
#ifdef CONFIG_APP_ARD	
    blendRenderY[stampAreaId].Buf = (void*)blendBufInfo[stampAreaId].AddressY;
    blendRenderY[stampAreaId].BufPitch = renderBufW * blendBufInfo[stampAreaId].BufPixelSize;
    blendRenderY[stampAreaId].BufWidth = blendBufInfo[stampAreaId].Width;
    blendRenderY[stampAreaId].BufHeight = blendBufInfo[stampAreaId].Height;
    blendRenderY[stampAreaId].BufPixelSize = 1;
    AppLibRender_Init(&blendRenderY[stampAreaId]);
    memset(blendRenderY[stampAreaId].Buf, 0x0, blendRenderY[stampAreaId].BufPitch * blendRenderY[stampAreaId].BufHeight);
    AmbaCache_Clean(blendRenderY[stampAreaId].Buf, blendRenderY[stampAreaId].BufPitch * blendRenderY[stampAreaId].BufHeight);

    blendRenderUV[stampAreaId].Buf = (void*)blendBufInfo[stampAreaId].AddressUV;
    blendRenderUV[stampAreaId].BufPitch = renderBufW * blendBufInfo[stampAreaId].BufPixelSize;
    blendRenderUV[stampAreaId].BufWidth = blendBufInfo[stampAreaId].Width;
    blendRenderUV[stampAreaId].BufHeight = renderBufH;
    blendRenderUV[stampAreaId].BufPixelSize = 1;
    AppLibRender_Init(&blendRenderUV[stampAreaId]);
    memset(blendRenderUV[stampAreaId].Buf, 0x0, blendRenderUV[stampAreaId].BufPitch * blendRenderUV[stampAreaId].BufHeight);
    AmbaCache_Clean(blendRenderUV[stampAreaId].Buf, blendRenderUV[stampAreaId].BufPitch * blendRenderUV[stampAreaId].BufHeight);

    blendRenderAlphaY[stampAreaId].Buf = (void*)blendBufInfo[stampAreaId].AddressAlphaY;
    blendRenderAlphaY[stampAreaId].BufPitch = renderBufW * blendBufInfo[stampAreaId].BufPixelSize;
    blendRenderAlphaY[stampAreaId].BufWidth = blendBufInfo[stampAreaId].Width;
    blendRenderAlphaY[stampAreaId].BufHeight = blendBufInfo[stampAreaId].Height;
    blendRenderAlphaY[stampAreaId].BufPixelSize = 1;
    AppLibRender_Init(&blendRenderAlphaY[stampAreaId]);
    memset(blendRenderAlphaY[stampAreaId].Buf, 0xFF, blendRenderAlphaY[stampAreaId].BufPitch * blendRenderAlphaY[stampAreaId].BufHeight);
    AmbaCache_Clean(blendRenderAlphaY[stampAreaId].Buf, blendRenderAlphaY[stampAreaId].BufPitch * blendRenderAlphaY[stampAreaId].BufHeight);

    blendRenderAlphaUV[stampAreaId].Buf = (void*)blendBufInfo[stampAreaId].AddressAlphaUV;
    blendRenderAlphaUV[stampAreaId].BufPitch = renderBufW * blendBufInfo[stampAreaId].BufPixelSize;
    blendRenderAlphaUV[stampAreaId].BufWidth = blendBufInfo[stampAreaId].Width;
    blendRenderAlphaUV[stampAreaId].BufHeight = renderBufH;
    blendRenderAlphaUV[stampAreaId].BufPixelSize = 1;
    AppLibRender_Init(&blendRenderAlphaUV[stampAreaId]);
    memset(blendRenderAlphaUV[stampAreaId].Buf, 0xFF, blendRenderAlphaUV[stampAreaId].BufPitch * blendRenderAlphaUV[stampAreaId].BufHeight);
    AmbaCache_Clean(blendRenderAlphaUV[stampAreaId].Buf, blendRenderAlphaUV[stampAreaId].BufPitch * blendRenderAlphaUV[stampAreaId].BufHeight);
#else
	blendRenderY.Buf = (void*)blendBufInfo[stampAreaId].AddressY;
	blendRenderY.BufPitch = renderBufW * blendBufInfo[stampAreaId].BufPixelSize;
	blendRenderY.BufWidth = blendBufInfo[stampAreaId].Width;
	blendRenderY.BufHeight = blendBufInfo[stampAreaId].Height;
	blendRenderY.BufPixelSize = 1;
	AppLibRender_Init(&blendRenderY);
	memset(blendRenderY.Buf, 0x0, blendRenderY.BufPitch * blendRenderY.BufHeight);
	AmbaCache_Clean(blendRenderY.Buf, blendRenderY.BufPitch * blendRenderY.BufHeight);

	blendRenderUV.Buf = (void*)blendBufInfo[stampAreaId].AddressUV;
	blendRenderUV.BufPitch = renderBufW * blendBufInfo[stampAreaId].BufPixelSize;
	blendRenderUV.BufWidth = blendBufInfo[stampAreaId].Width;
	blendRenderUV.BufHeight = renderBufH;
	blendRenderUV.BufPixelSize = 1;
	AppLibRender_Init(&blendRenderUV);
	memset(blendRenderUV.Buf, 0x0, blendRenderUV.BufPitch * blendRenderUV.BufHeight);
	AmbaCache_Clean(blendRenderUV.Buf, blendRenderUV.BufPitch * blendRenderUV.BufHeight);

	blendRenderAlphaY.Buf = (void*)blendBufInfo[stampAreaId].AddressAlphaY;
	blendRenderAlphaY.BufPitch = renderBufW * blendBufInfo[stampAreaId].BufPixelSize;
	blendRenderAlphaY.BufWidth = blendBufInfo[stampAreaId].Width;
	blendRenderAlphaY.BufHeight = blendBufInfo[stampAreaId].Height;
	blendRenderAlphaY.BufPixelSize = 1;
	AppLibRender_Init(&blendRenderAlphaY);
	memset(blendRenderAlphaY.Buf, 0xFF, blendRenderAlphaY.BufPitch * blendRenderAlphaY.BufHeight);
	AmbaCache_Clean(blendRenderAlphaY.Buf, blendRenderAlphaY.BufPitch * blendRenderAlphaY.BufHeight);

	blendRenderAlphaUV.Buf = (void*)blendBufInfo[stampAreaId].AddressAlphaUV;
	blendRenderAlphaUV.BufPitch = renderBufW * blendBufInfo[stampAreaId].BufPixelSize;
	blendRenderAlphaUV.BufWidth = blendBufInfo[stampAreaId].Width;
	blendRenderAlphaUV.BufHeight = renderBufH;
	blendRenderAlphaUV.BufPixelSize = 1;
	AppLibRender_Init(&blendRenderAlphaUV);
	memset(blendRenderAlphaUV.Buf, 0xFF, blendRenderAlphaUV.BufPitch * blendRenderAlphaUV.BufHeight);
	AmbaCache_Clean(blendRenderAlphaUV.Buf, blendRenderAlphaUV.BufPitch * blendRenderAlphaUV.BufHeight);
#endif
    /* RGB->YUV */
    switch (colorFormat) {
        case COLOR_FORMAT_YUV420:
            _AppLibBlend_DumpClut();
            if (clut != NULL) {  // 8-bit
                _AppLibBlendBuffer_CovertColorSpace_RGB2YUV420_8bit(stampAreaId, sourceBufInfo);
            } else {
                _AppLibBlendBuffer_CovertColorSpace_RGB2YUV420(stampAreaId, sourceBufInfo);
            }
            break;
        case COLOR_FORMAT_YUV422:
        default:
            _AppLibBlend_DumpClut();
            if (clut != NULL) {  // 8-bit
                _AppLibBlendBuffer_CovertColorSpace_RGB2YUV422_8bit(stampAreaId, sourceBufInfo);
            } else {
                _AppLibBlendBuffer_CovertColorSpace_RGB2YUV422(stampAreaId, sourceBufInfo);
            }
            break;
    }
}

/**
 *  @brief Update one specific buffer for blending
 *
 *  Update one new specific for blending
 *
 *  @param [in] stampAreaId the specific id for blending buffer
 *  @param [in] *sourceBufInfo the blending buffer
 *  @param [in] colorFormat the color format of source buffer
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
void AppLibBlend_UpdateBlendArea(UINT8 stampAreaId, APPLIB_GRAPHIC_SOURCE_BUF_INFO_s *sourceBufInfo, APPLIB_GRAPH_COLOR_FORMAT_e colorFormat)
{
    if (blendBufInfo[stampAreaId].LengthY == 0) {
        return;
    }

    /* RGB->YUV */
    switch(colorFormat) {
        case COLOR_FORMAT_YUV420:
            _AppLibBlend_DumpClut();
            if (clut != NULL) {  // 8-bit
                _AppLibBlendBuffer_CovertColorSpace_RGB2YUV420_8bit(stampAreaId, sourceBufInfo);
            } else {
                _AppLibBlendBuffer_CovertColorSpace_RGB2YUV420(stampAreaId, sourceBufInfo);
            }
            break;
        case COLOR_FORMAT_YUV422:
        default:
             _AppLibBlend_DumpClut();
            if (clut != NULL) {  // 8-bit
                _AppLibBlendBuffer_CovertColorSpace_RGB2YUV422_8bit(stampAreaId, sourceBufInfo);
            } else {
                _AppLibBlendBuffer_CovertColorSpace_RGB2YUV422(stampAreaId, sourceBufInfo);
            }
            break;
    }
}

/**
 *  @brief Get a buffer for blending
 *
 *  Get a buffer for blending
 *
 *  @param [in] blendBufId the specific id for blending buffer
 *  @param [out] *bufInfo the all info of blending buffer
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
void AppLibStamp_GetBlendBuf(UINT32 stampAreaId, APPLIB_GRAPHIC_STAMP_BUF_CONFIG_s *bufInfo)
{
    // return data
    bufInfo->OffsetX = blendBufInfo[stampAreaId].X;
    bufInfo->OffsetY = blendBufInfo[stampAreaId].Y;
    bufInfo->Width = blendBufInfo[stampAreaId].Width;
    bufInfo->Height = blendBufInfo[stampAreaId].Height;
    bufInfo->YAddr = (UINT8*)blendBufInfo[stampAreaId].AddressY;
    bufInfo->UVAddr = (UINT8*)blendBufInfo[stampAreaId].AddressUV;
    bufInfo->AlphaYAddr = (UINT8*)blendBufInfo[stampAreaId].AddressAlphaY;
    bufInfo->AlphaUVAddr = (UINT8*)blendBufInfo[stampAreaId].AddressAlphaUV;
}

