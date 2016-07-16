/**
 * @file src/app/connected/applib/src/graphics/ApplibGraphics.c
 *
 * Implementation of Graphics Utilities
 *
 * History:
 *    2013/09/23 - [Martin Lai] created file
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

#include <graphics/ApplibGraphics.h>
#include <graphics/ApplibGraphics_MainTask.h> /* CONFIG_APP_ARD */
#ifdef CONFIG_APP_ARD
extern UINT8 clut[256 * 3];
#endif

/*************************************************************************
 * Graphics Definitions
 ************************************************************************/
static GRAPH_CHAN_s _FchanData = {.PathActived = BOOLEAN_FALSE,
                                  .GraphicInfo = {.EnableDraw = BOOLEAN_FALSE,
                                                  .BufferIdx = 0,
                                                  .EnableSwScale = BOOLEAN_FALSE,
                                                  .pGuiTable = NULL,
                                                  .ObjectMaxAmout = 0,
                                                  .DispalyBuffer = {.Width = 0,
                                                                    .Height = 0,
                                                                    .Address = {NULL},
                                                                    .RawAddress = {NULL}},
                                                  .WorkingBuffer = {.Width = 0,
                                                                    .Height = 0,
                                                                    .Address = NULL,
                                                                    .RawAddress = NULL},
                                                  .Render = {0},
                                                  .Canvas = {0},
                                                  .ObjList = NULL,
                                                  .Resolution = 0,
                                                  .BitmapBuffer = NULL,
#ifdef CONFIG_APP_ARD
                                                  .StringBuffer = {NULL}},
#else
                                                  .StringBuffer = NULL},
#endif
                                  .OsdInfo = {AMP_OSD_8BIT_CLUT_MODE, NULL, NULL},
                                  .WindowInfo = {0}};
static GRAPH_CHAN_s _DchanData = {.PathActived = BOOLEAN_FALSE,
                                  .GraphicInfo = {.EnableDraw = BOOLEAN_FALSE,
                                                  .BufferIdx = 0,
                                                  .EnableSwScale = BOOLEAN_TRUE,
                                                  .pGuiTable = NULL,
                                                  .ObjectMaxAmout = 0,
                                                  .DispalyBuffer = {.Width = 0,
                                                                    .Height = 0,
                                                                    .Address = {NULL},
                                                                    .RawAddress = {NULL}},
                                                  .WorkingBuffer = {.Width = 0,
                                                                    .Height = 0,
                                                                    .Address = NULL,
                                                                    .RawAddress = NULL},
                                                  .Render = {0},
                                                  .Canvas = {0},
                                                  .ObjList = NULL,
                                                  .Resolution = 0,
                                                  .BitmapBuffer = NULL,
#ifdef CONFIG_APP_ARD
                                                  .StringBuffer = {NULL}},
#else
                                                  .StringBuffer = NULL},
#endif
                                  .OsdInfo = {AMP_OSD_8BIT_CLUT_MODE, NULL, NULL},
                                  .WindowInfo = {0}};
static GRAPH_CHAN_s _BlendData = {.PathActived = BOOLEAN_FALSE,
                                  .GraphicInfo = {.EnableDraw = BOOLEAN_TRUE,
                                                  .BufferIdx = 0,
                                                  .EnableSwScale = BOOLEAN_FALSE,
                                                  .pGuiTable = NULL,
                                                  .ObjectMaxAmout = 0,
                                                  .DispalyBuffer = {.Width = 0,
                                                                    .Height = 0,
                                                                    .Address = {NULL},
                                                                    .RawAddress = {NULL}},
                                                  .WorkingBuffer = {.Width = 0,
                                                                    .Height = 0,
                                                                    .Address = NULL,
                                                                    .RawAddress = NULL},
                                                  .Render = {0},
                                                  .Canvas = {0},
                                                  .ObjList = NULL,
                                                  .Resolution = 0,
                                                  .BitmapBuffer = NULL,
#ifdef CONFIG_APP_ARD
                                                  .StringBuffer = {NULL}},
#else
                                                  .StringBuffer = NULL},
#endif
                                  .OsdInfo = {AMP_OSD_8BIT_CLUT_MODE, NULL, NULL},
                                  .WindowInfo = {0}};

static APPLIB_GRAPH_INIT_CONFIG_s _InitConfig = {0};
extern int AppLibRender_Init(APPLIB_GRAPHIC_RENDER_s *render);

static UINT8 InitFlag = GRAPHICS_INIT_NONE;

/*************************************************************************
 * General Graphics APIs
 ************************************************************************/
static int _AllocateMemory(void **buffer, void **rawBuffer, UINT32 buffSize)
{
    int ReturnValue = 0;
    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, buffer, rawBuffer, buffSize, 32);
    if (ReturnValue < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[AppLib - Graphics] Allocate memory fail");
        AmbaKAL_BytePoolFree(*rawBuffer);
        *rawBuffer = NULL;
    } else {
        memset(*buffer, 0x0, buffSize);
    }
    return ReturnValue;
}

static UINT8 _CheckPixelSize(AMP_DISP_OSD_FORMAT_e param)
{
    int rval = 1;
    if (param < AMP_OSD_16BIT_VYU_RGB_565) {
        rval = 1;
    } else if (param < AMP_OSD_32BIT_AYUV_8888) {
        rval = 2;
    } else if (param <= AMP_OSD_32BIT_ARGB_8888) {
        rval = 4;
    }
    return rval;
}

//-----------------------------
static int _MwWindow_ConfigWindow(UINT32 graphChannelId, int windowId, AMP_DISP_WINDOW_CFG_s *windowCfg)
{
    if (AppLibDisp_SetWindowConfig(graphChannelId, windowId, windowCfg) < 0) {
        return BOOLEAN_FALSE;
    }
    return BOOLEAN_TRUE;
}

static int _MwWindow_RetrieveWindow(UINT32 graphChannelId, int windowId, AMP_DISP_WINDOW_CFG_s *windowCfg)
{
    if (AppLibDisp_GetWindowConfig(graphChannelId, windowId, windowCfg) < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[AppLib - Graphics] <_MwWindow_RetrieveWindow> AppLibDisp_GetWindowConfig fail");
        return BOOLEAN_FALSE;
    }
    return BOOLEAN_TRUE;
}

static int _MwWindow_ActiveWindow(UINT32 graphChannelId, int windowId)
{
    if (AppLibDisp_ActivateWindow(graphChannelId, windowId) < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[AppLib - Graphics] <_MwWindow_ActiveWindow> AppLibDisp_ActivateWindow fail");
        return BOOLEAN_FALSE;
    }
    return BOOLEAN_TRUE;
}

static int _MwWindow_DeactiveWindow(UINT32 graphChannelId, int windowId)
{
    if (AppLibDisp_DeactivateWindow(graphChannelId, windowId) < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[AppLib - Graphics] <_MwWindow_DeactiveWindow> AppLibDisp_ActivateWindow fail");
        return BOOLEAN_FALSE;
    }
    return BOOLEAN_TRUE;

}

static int _MwWindow_FlushWindow(UINT32 graphChannelId)
{
    if (AppLibDisp_FlushWindow(graphChannelId) < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[AppLib - Graphics] <_MwWindow_FlushWindow> AppLibDisp_FlushWindow fail");
        return BOOLEAN_FALSE;
    }
    return BOOLEAN_TRUE;
}

static int _AppLibGraph_BMP_Load(GRAPHICS_CHAN_e canvasChn, GRAPH_CHAN_s *ChannelData)
{
    UINT32 bmpNumber = 0;
    UINT32 Sz = 0;
    int rval = 0;
    static void *RawBMPBinBufchan[GRAPHICS_CHANNEL_NUM] = {NULL, NULL, NULL};

    if (ChannelData->GraphicInfo.BitmapBuffer) {
        return 0;
    }

    // Calculate size
    rval = AppLibBMP_GetTotalBMPSize(_InitConfig.BMPFileName, ChannelData->GraphicInfo.Resolution, &bmpNumber, &Sz);
    if (rval < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "AppLibBMP_GetTotalBMPSize fail");
        return rval;
    }

    // Allocate memory
    rval |= _AllocateMemory(&ChannelData->GraphicInfo.BitmapBuffer, &RawBMPBinBufchan[canvasChn], Sz);
    if (rval < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[AppLib - Graphics] <BMP_Load> Allocate memory fail");
        RawBMPBinBufchan[canvasChn] = NULL;
        return rval;
    }

    // Init memory
    rval = AppLibBMP_InitBMPBuffer(_InitConfig.BMPFileName, ChannelData->GraphicInfo.Resolution, ChannelData->GraphicInfo.BitmapBuffer, 1);
    if (rval < 0) {
        // Release memory
        AmbaKAL_BytePoolFree(RawBMPBinBufchan[canvasChn]);
        RawBMPBinBufchan[canvasChn] = NULL;
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "AppLibBMP_InitBMPBuffer %d fail", ChannelData->GraphicInfo.Resolution);
    }
    return rval;
}

#ifdef CONFIG_APP_ARD
BOOLEAN_e _AppLibGraph_String_Load(GRAPHICS_CHAN_e canvasChn, UINT32 langIdx, GRAPH_CHAN_s *ChannelData)
#else
static BOOLEAN_e _AppLibGraph_String_Load(GRAPHICS_CHAN_e canvasChn, UINT32 langIdx, GRAPH_CHAN_s *ChannelData)
#endif
{
    int rval = 0;
    UINT32 strSz = 0, tmpSz = 0, MsgN = 0;
    void *tmpBuf[GRAPHICS_CHANNEL_NUM] = {NULL, NULL, NULL};
    void *tmpBufRaw[GRAPHICS_CHANNEL_NUM] = {NULL, NULL, NULL};
    static void *StrBinBuf_Raw[GRAPHICS_CHANNEL_NUM] = {NULL, NULL, NULL};
#ifdef CONFIG_APP_ARD
    if (!ChannelData->GraphicInfo.StringBuffer[langIdx]) {
        int RomIdx = AmbaROM_GetIndex(AMBA_ROM_SYS_DATA, _InitConfig.StringFileName);
        int RomSz = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, _InitConfig.StringFileName, RomIdx);

        // Allocate memory
        rval = _AllocateMemory(&ChannelData->GraphicInfo.StringBuffer[langIdx], &StrBinBuf_Raw[canvasChn], RomSz);
        if (rval < 0) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[AppLib - Graphics] <String_Load> Allocate memory fail");
            return BOOLEAN_FALSE;
        }
    } else {
		ChannelData->GraphicInfo.ActiveLangId = langIdx;
		return BOOLEAN_TRUE;
	}
#else
    if (ChannelData->GraphicInfo.StringBuffer) {
        return BOOLEAN_TRUE;
    }
#endif

    // Calculate size
    rval = AppLibStr_CalcLangSizeFromROMFS(_InitConfig.StringFileName, langIdx, &MsgN, &strSz, &tmpSz);
    if (rval < 0) {
       GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "AppLibStr_CalcLangSizeFromROMFS fail");
        return BOOLEAN_FALSE;
    }

#ifndef CONFIG_APP_ARD
    // Allocate memory
    rval = _AllocateMemory(&ChannelData->GraphicInfo.StringBuffer, &StrBinBuf_Raw[canvasChn], strSz);
    if (rval < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[AppLib - Graphics] <String_Load> Allocate memory fail");
        return BOOLEAN_FALSE;
    }
#endif

    rval = _AllocateMemory(&tmpBuf[canvasChn], &tmpBufRaw[canvasChn], tmpSz);
    if (rval < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[AppLib - Graphics] <String_Load> Allocate temp memory fail");
        return BOOLEAN_FALSE;
    }

    // Init memory
#ifdef CONFIG_APP_ARD
	rval = AppLibStr_InitFromROMFS(_InitConfig.StringFileName, langIdx, ChannelData->GraphicInfo.StringBuffer[langIdx], tmpBuf[canvasChn]);
#else
    rval = AppLibStr_InitFromROMFS(_InitConfig.StringFileName, langIdx, ChannelData->GraphicInfo.StringBuffer, tmpBuf[canvasChn]);
#endif
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_GRAPHICS, "AppLibStr_InitFromROMFS %d done", 0);
    if (rval >= 0) {
        AmbaKAL_BytePoolFree(tmpBufRaw[canvasChn]);
        tmpBufRaw[canvasChn] = NULL;
    } else {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "AppLibStr_InitFromROMFS %d fail", 0);
        AmbaKAL_BytePoolFree(StrBinBuf_Raw[canvasChn]);
        AmbaKAL_BytePoolFree(tmpBufRaw[canvasChn]);
        tmpBufRaw[canvasChn] = NULL;
        StrBinBuf_Raw[canvasChn] = NULL;
        return BOOLEAN_FALSE;
    }
#ifdef CONFIG_APP_ARD
	ChannelData->GraphicInfo.ActiveLangId = langIdx;
#endif
    return BOOLEAN_TRUE;
}

//-----------------------------
static RETURN_TYPE_e _MwOsd_Init(GRAPHICS_CHAN_e ChannelId, GRAPH_CHAN_s *ChannelData)
{
    void *MemoryPoolAddrRaw = NULL;
    int ReturnValue = 0;
    AMP_OSD_CFG_s Cfg = {0};

    /* To check initialization status. */
    if ( (InitFlag & GRAPHICS_INIT_MW_OSD) == GRAPHICS_INIT_NONE) {
        /* Bulid-up working buffer */
        AMP_OSD_INIT_CFG_s InitCfg = {0};
        AmpOsd_GetDefaultInitCfg(&InitCfg);

        ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&InitCfg.MemoryPoolAddr, &MemoryPoolAddrRaw, InitCfg.MemoryPoolSize, 32);
        if (ReturnValue < 0) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_MwOsd_Init> Allocate OSD working buffer fail.");
            MemoryPoolAddrRaw = NULL;
            return RETURN_FAIL;
        }
        memset(InitCfg.MemoryPoolAddr, 0x0, InitCfg.MemoryPoolSize);

        /* Initialize */
        if (AmpOsd_Init(&InitCfg) < 0) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_MwOsd_Init> AmpOsd_Init fail.");
            MemoryPoolAddrRaw = NULL;
            return RETURN_FAIL;
        }
        InitFlag |= GRAPHICS_INIT_MW_OSD;
    }

    /* To create handler. */
    AmpOsd_GetDefaultCfg(&Cfg);
    if (ChannelData->GraphicInfo.EnableSwScale == BOOLEAN_FALSE) {  // HW scaler
        Cfg.HwScalerType = HW_OSD_RESCALER_ANY;
    } else {
        Cfg.HwScalerType = HW_OSD_RESCALER_NONE;
    }
    Cfg.OsdBufRepeatField = 0;
    Cfg.GlobalBlend = 0xFF;
    if (AmpOsd_Create(&Cfg, &ChannelData->OsdInfo.Hdlr) < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_MwOsd_Init> AmpOsd_Create fail.");
        return RETURN_FAIL;
    }

    /* To register buffer. */
    if ( (ChannelId == GRAPHICS_CHANNEL_D) || (ChannelId == GRAPHICS_CHANNEL_F) ) {
        AMP_OSD_BUFFER_CFG_s BufferCfg = {0};
        BufferCfg.BufAddr = (UINT8*)ChannelData->GraphicInfo.DispalyBuffer.Address[1];
        BufferCfg.BufWidth = ChannelData->GraphicInfo.DispalyBuffer.Width;
        BufferCfg.BufHeight = ChannelData->GraphicInfo.DispalyBuffer.Height;
        BufferCfg.BufPitch = ChannelData->GraphicInfo.DispalyBuffer.Width * _CheckPixelSize(ChannelData->OsdInfo.PixelFormat);
        BufferCfg.PixelFormat = ChannelData->OsdInfo.PixelFormat;
        if (AmpOsd_SetBufferCfg(ChannelData->OsdInfo.Hdlr, &BufferCfg) < 0) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_MwOsd_Init> AmpOsd_SetBufferCfg fail.");
            return RETURN_FAIL;
        }
    }
    return RETURN_SUCCESS;
}

static RETURN_TYPE_e _MwWindow_Init(GRAPHICS_CHAN_e ChannelId, GRAPH_CHAN_s *ChannelData, AMP_AREA_s *SourceArea, AMP_AREA_s *TargetArea)
{
    RETURN_TYPE_e ReturnValue = RETURN_SUCCESS;
    AMP_DISP_WINDOW_CFG_s Window;
    memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));
    Window.Source = AMP_DISP_OSD;
    Window.SourceDesc.Osd.OsdHdlr = (void*)ChannelData->OsdInfo.Hdlr;
    Window.CropArea.X = SourceArea->X;
    Window.CropArea.Y = SourceArea->Y;
    Window.CropArea.Width = SourceArea->Width;
    Window.CropArea.Height = SourceArea->Height;
    Window.TargetAreaOnPlane.X = TargetArea->X;
    Window.TargetAreaOnPlane.Y = TargetArea->Y;
    Window.TargetAreaOnPlane.Width = TargetArea->Width;
    Window.TargetAreaOnPlane.Height = TargetArea->Height;

    switch (ChannelId) {
        case GRAPHICS_CHANNEL_D:
            ChannelData->WindowInfo.WindowId = AppLibDisp_AddWindow(DISP_CH_DCHAN, &Window);
            InitFlag |= GRAPHICS_INIT_MW_WINDOW;
            break;
        case GRAPHICS_CHANNEL_F:
            ChannelData->WindowInfo.WindowId = AppLibDisp_AddWindow(DISP_CH_FCHAN, &Window);
            InitFlag |= GRAPHICS_INIT_MW_WINDOW;
            break;
        default:
            ReturnValue = RETURN_FAIL;
            break;
    }
    return ReturnValue;
}

static RETURN_TYPE_e _ApplibGraph_Buffer_Create(BUFFER_TYPE_e Type, GRAPH_CHAN_s *ChannelData)
{
    void **pAddr = NULL, **pRawAddr = NULL;
    UINT32 Size = 0;

    switch (Type) {
        case BUFFER_TYPE_WORKING:
            pAddr = (void**)&ChannelData->GraphicInfo.WorkingBuffer.Address;
            pRawAddr = (void**)&ChannelData->GraphicInfo.WorkingBuffer.RawAddress;
            Size = ChannelData->GraphicInfo.WorkingBuffer.Width * ChannelData->GraphicInfo.WorkingBuffer.Height * _CheckPixelSize(ChannelData->OsdInfo.PixelFormat);
            break;
        case BUFFER_TYPE_DISPLAY_1ST:
            pAddr = (void**)&ChannelData->GraphicInfo.DispalyBuffer.Address[0];
            pRawAddr = (void**)&ChannelData->GraphicInfo.DispalyBuffer.RawAddress[0];
            Size = ChannelData->GraphicInfo.DispalyBuffer.Width * ChannelData->GraphicInfo.DispalyBuffer.Height * _CheckPixelSize(ChannelData->OsdInfo.PixelFormat);
            break;
        case BUFFER_TYPE_DISPLAY_2ND:
            pAddr = (void**)&ChannelData->GraphicInfo.DispalyBuffer.Address[1];
            pRawAddr = (void**)&ChannelData->GraphicInfo.DispalyBuffer.RawAddress[1];
            Size = ChannelData->GraphicInfo.DispalyBuffer.Width * ChannelData->GraphicInfo.DispalyBuffer.Height * _CheckPixelSize(ChannelData->OsdInfo.PixelFormat);
            break;
        default:
            break;
    }

    if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, pAddr, pRawAddr, Size, 32) < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_ApplibGraph_Buffer_Create> Allocate buffer(%d) fail.", Type);
        *pRawAddr = NULL;
        return RETURN_FAIL;
    }

    memset(*pAddr, 0x0, Size);

    return RETURN_SUCCESS;
}

static RETURN_TYPE_e _ApplibGraph_Buffer_Sync(GRAPH_CHAN_s *ChannelData)
{
    if (ChannelData->GraphicInfo.EnableSwScale == BOOLEAN_FALSE) {  // HW scale
        UINT32 sz = ChannelData->GraphicInfo.WorkingBuffer.Width * ChannelData->GraphicInfo.WorkingBuffer.Height * _CheckPixelSize(ChannelData->OsdInfo.PixelFormat);
        memcpy(ChannelData->GraphicInfo.DispalyBuffer.Address[ChannelData->GraphicInfo.BufferIdx], ChannelData->GraphicInfo.WorkingBuffer.Address, sz);
    } else {
        if ((ChannelData->GraphicInfo.WorkingBuffer.Width == ChannelData->GraphicInfo.DispalyBuffer.Width)
            && (ChannelData->GraphicInfo.WorkingBuffer.Height == ChannelData->GraphicInfo.DispalyBuffer.Height)){
            UINT32 sz = ChannelData->GraphicInfo.WorkingBuffer.Width * ChannelData->GraphicInfo.WorkingBuffer.Height * _CheckPixelSize(ChannelData->OsdInfo.PixelFormat);
            memcpy(ChannelData->GraphicInfo.DispalyBuffer.Address[ChannelData->GraphicInfo.BufferIdx], ChannelData->GraphicInfo.WorkingBuffer.Address, sz);
        } else {
            APPLIB_SW_SCALAR_s scalarParam = {0};

            scalarParam.SrcBufferAddress = (UINT32)ChannelData->GraphicInfo.WorkingBuffer.Address;
            scalarParam.SrcBufferPitch = ChannelData->GraphicInfo.WorkingBuffer.Width * _CheckPixelSize(ChannelData->OsdInfo.PixelFormat);
            scalarParam.SrcBufferWidth = ChannelData->GraphicInfo.WorkingBuffer.Width;
            scalarParam.SrcBufferHeight = ChannelData->GraphicInfo.WorkingBuffer.Height;
            scalarParam.SrcPositionX = 0;
            scalarParam.SrcPositionY = 0;
            scalarParam.SrcWidth = ChannelData->GraphicInfo.WorkingBuffer.Width;
            scalarParam.SrcHeight = ChannelData->GraphicInfo.WorkingBuffer.Height;

            scalarParam.DstBufferAddress = (UINT32)ChannelData->GraphicInfo.DispalyBuffer.Address[ChannelData->GraphicInfo.BufferIdx];
            scalarParam.DstBufferPitch = ChannelData->GraphicInfo.DispalyBuffer.Width * _CheckPixelSize(ChannelData->OsdInfo.PixelFormat);
            scalarParam.DstBufferWidth = ChannelData->GraphicInfo.DispalyBuffer.Width;
            scalarParam.DstBufferHeight = ChannelData->GraphicInfo.DispalyBuffer.Height;
            scalarParam.DstPositionX = 0;
            scalarParam.DstPositionY = 0;
            scalarParam.DstWidth = ChannelData->GraphicInfo.DispalyBuffer.Width;
            scalarParam.DstHeight = ChannelData->GraphicInfo.DispalyBuffer.Height;

            scalarParam.Type = SCALAR_TYPE_4_BYTE;
#ifdef CONFIG_APP_ARD
            if((&_DchanData == ChannelData)&&(AppLibDisp_GetRotate(DISP_CH_DCHAN) == AMP_ROTATE_90)){
                scalarParam.Type = SCALAR_TYPE_1_BYTE_90;
            }
#endif
            scalarParam.TransColorEn = 0;
            scalarParam.TransColor = 0;
            scalarParam.RegardDstValue = SCALAR_WRITE_DST_VALUE;

            AppLibUtilitySWScalar_ExeScalar(&scalarParam);
        }
    }
    return RETURN_SUCCESS;
}

static RETURN_TYPE_e _ApplibGraph_Buffer_Switch(GRAPH_CHAN_s *ChannelData)
{
    AMP_OSD_BUFFER_CFG_s buffCfg = {0};

    UINT8 currIdx = ChannelData->GraphicInfo.BufferIdx;
    UINT8 nextIdx = (currIdx + 1) % 2;
    UINT8 pixelSize = _CheckPixelSize(ChannelData->OsdInfo.PixelFormat);
    UINT32 bufferSize = ChannelData->GraphicInfo.DispalyBuffer.Width * ChannelData->GraphicInfo.DispalyBuffer.Height * pixelSize;

    // clean cache buffer
    AmbaCache_Clean((void *)ChannelData->GraphicInfo.DispalyBuffer.Address[currIdx], bufferSize);

    // assign buffer
    buffCfg.BufAddr = (UINT8*)ChannelData->GraphicInfo.DispalyBuffer.Address[currIdx];
    buffCfg.BufWidth = ChannelData->GraphicInfo.DispalyBuffer.Width;
    buffCfg.BufHeight = ChannelData->GraphicInfo.DispalyBuffer.Height;
    buffCfg.BufPitch = ChannelData->GraphicInfo.DispalyBuffer.Width * _CheckPixelSize(ChannelData->OsdInfo.PixelFormat);
    buffCfg.PixelFormat = ChannelData->OsdInfo.PixelFormat;
    if (AmpOsd_SetBufferCfg(ChannelData->OsdInfo.Hdlr, &buffCfg) < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_ApplibGraph_Buffer_Switch> AmpOsd_SetBufferCfg fail.");
        return RETURN_FAIL;
    }

#ifdef CONFIG_APP_ARD
    /*wait DSP switch buffer complete*/
    AmbaKAL_TaskSleep(50);
#endif
    // update buffer index
    ChannelData->GraphicInfo.BufferIdx = nextIdx;

    // update render
    ChannelData->GraphicInfo.Render.Buf = ChannelData->GraphicInfo.WorkingBuffer.Address;
    ChannelData->GraphicInfo.Render.BufHeight = ChannelData->GraphicInfo.WorkingBuffer.Height;
    ChannelData->GraphicInfo.Render.BufPitch = ChannelData->GraphicInfo.WorkingBuffer.Width * pixelSize;
    ChannelData->GraphicInfo.Render.BufPixelSize = pixelSize;
    if (AppLibRender_Init(&ChannelData->GraphicInfo.Render) < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_ApplibGraph_Buffer_Switch> AppLibRender_Init fail.");
        AmbaKAL_BytePoolFree(ChannelData->GraphicInfo.WorkingBuffer.RawAddress);
        return RETURN_FAIL;
    }
    return RETURN_SUCCESS;
}

static RETURN_TYPE_e _ApplibGraph_Clut_Load(const char* FileName, AMP_OSD_CLUT_CFG_s *ClutCfg)
{
    int Size = 0;
    void *Addr = NULL, *RawAddr = NULL;

    if (AmbaROM_FileExists(AMBA_ROM_SYS_DATA, FileName) == 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_ApplibGraph_Clut_Load> AmbaROM_FileExists fail.");
        return RETURN_FAIL;
    }

    Size = AmbaROM_GetSize(AMBA_ROM_SYS_DATA, FileName, 0x0);
    if ( Size <= 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_ApplibGraph_Clut_Load> AmbaROM_GetSize fail.");
        return RETURN_FAIL;
    }

    if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &Addr, &RawAddr, Size, 32) < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_ApplibGraph_Clut_Load> Get Memory fail.");
        return RETURN_FAIL;
    }

    ClutCfg->Clut = (UINT8*)Addr;
    AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, FileName, Addr, Size, 0);

    return RETURN_SUCCESS;
}

static RETURN_TYPE_e _ApplibGraph_Canvas_Init(GRAPHICS_CHAN_e canvasChn, GRAPH_CHAN_s *ChannelData)
{
    UINT32 ObjSize = 0;
    void *RawBuffer = NULL;

#ifdef CONFIG_APP_ARD
    if ( (!ChannelData->GraphicInfo.BitmapBuffer) && (!ChannelData->GraphicInfo.StringBuffer[ChannelData->GraphicInfo.ActiveLangId]) ) {
#else
    if ( (!ChannelData->GraphicInfo.BitmapBuffer) && (!ChannelData->GraphicInfo.StringBuffer) ) {
#endif
        return RETURN_FAIL;
    }

    ObjSize = ChannelData->GraphicInfo.ObjectMaxAmout *sizeof(APPLIB_GRAPHIC_OBJ_s);
    if (AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void **)&ChannelData->GraphicInfo.ObjList, &RawBuffer, ObjSize, 32) < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_ApplibGraph_Canvas_Init> Allocate memory fail");
        AmbaKAL_BytePoolFree(RawBuffer);
        RawBuffer = NULL;
        return RETURN_FAIL;
    }
    memset(RawBuffer, 0x0, ObjSize);

    return RETURN_SUCCESS;

}

static RETURN_TYPE_e _ApplibGraph_Object_Check(GRAPHICS_CHAN_e canvasChn, UINT32 guiId, GRAPH_CHAN_s *ChannelData)
{
    RETURN_TYPE_e rval = RETURN_FAIL;
    if (ChannelData->GraphicInfo.ObjList[guiId].Exist) {
        return RETURN_SUCCESS;  // gui obj's already existed.
    }

    switch (ChannelData->GraphicInfo.pGuiTable[guiId]->Type) {
        case APPLIB_GRAPHIC_UIOBJ_STRING:
#ifdef CONFIG_APP_ARD
			ChannelData->GraphicInfo.pGuiTable[guiId]->Cnt.Str.FontAttr = (APPLIB_FONT_s*)&_InitConfig.Font;
            ChannelData->GraphicInfo.pGuiTable[guiId]->Cnt.Str.StrInfo = (APPLIB_GRAPHIC_STR_BIN_INFO_s *)ChannelData->GraphicInfo.StringBuffer[ChannelData->GraphicInfo.ActiveLangId];
            ChannelData->GraphicInfo.pGuiTable[guiId]->Cnt.Str.StrDesc = &((APPLIB_GRAPHIC_STR_BIN_INFO_s *)ChannelData->GraphicInfo.StringBuffer[ChannelData->GraphicInfo.ActiveLangId])->DescTable[ChannelData->GraphicInfo.pGuiTable[guiId]->Cnt.Str.MsgIdx];
#else
            ChannelData->GraphicInfo.pGuiTable[guiId]->Cnt.Str.FontAttr = (APPLIB_FONT_s*)&_InitConfig.Font;
            ChannelData->GraphicInfo.pGuiTable[guiId]->Cnt.Str.StrInfo = (APPLIB_GRAPHIC_STR_BIN_INFO_s *)ChannelData->GraphicInfo.StringBuffer;
            ChannelData->GraphicInfo.pGuiTable[guiId]->Cnt.Str.StrDesc = &((APPLIB_GRAPHIC_STR_BIN_INFO_s *)ChannelData->GraphicInfo.StringBuffer)->DescTable[ChannelData->GraphicInfo.pGuiTable[guiId]->Cnt.Str.MsgIdx];
#endif
            /** Step 1) Create Obj for canvas **/
            if (AppLibGraphic_CreateObj(ChannelData->GraphicInfo.pGuiTable[guiId], &ChannelData->GraphicInfo.ObjList[guiId]) < 0) {
                return RETURN_FAIL;
            }

            /** Step 2) Add Obj into canvas **/
            ChannelData->GraphicInfo.Canvas.ObjAdd_f(&ChannelData->GraphicInfo.Canvas, &ChannelData->GraphicInfo.ObjList[guiId]);
            rval = RETURN_SUCCESS;
            break;
        case APPLIB_GRAPHIC_UIOBJ_BMP:
            ChannelData->GraphicInfo.pGuiTable[guiId]->Cnt.Bmp.BmpInfo = (APPLIB_GRAPHIC_BMP_BIN_INFO_s *)ChannelData->GraphicInfo.BitmapBuffer;
            ChannelData->GraphicInfo.pGuiTable[guiId]->Cnt.Bmp.BmpDescPtr = &((APPLIB_GRAPHIC_BMP_BIN_INFO_s *)ChannelData->GraphicInfo.BitmapBuffer)->DescTab[ChannelData->GraphicInfo.pGuiTable[guiId]->Cnt.Bmp.BMPIdx];

            /** Step 1) Create Obj for canvas **/
            if (AppLibGraphic_CreateObj(ChannelData->GraphicInfo.pGuiTable[guiId], &ChannelData->GraphicInfo.ObjList[guiId]) < 0) {
                return RETURN_FAIL;
            }

            /** Step 2) Add Obj into canvas **/
            ChannelData->GraphicInfo.Canvas.ObjAdd_f(&ChannelData->GraphicInfo.Canvas, &ChannelData->GraphicInfo.ObjList[guiId]);
            rval = RETURN_SUCCESS;
            break;
        case APPLIB_GRAPHIC_UIOBJ_RECT:
            /** Step 1) Create Obj for canvas **/
            if (AppLibGraphic_CreateObj(ChannelData->GraphicInfo.pGuiTable[guiId], &ChannelData->GraphicInfo.ObjList[guiId])) {
                return RETURN_FAIL;
            }

            /** Step 2) Add Obj into canvas **/
            ChannelData->GraphicInfo.Canvas.ObjAdd_f(&ChannelData->GraphicInfo.Canvas, &ChannelData->GraphicInfo.ObjList[guiId]);
            rval = RETURN_SUCCESS;
            break;
        case APPLIB_GRAPHIC_UIOBJ_LINE:
            /** Step 1) Create Obj for canvas **/
            if (AppLibGraphic_CreateObj(ChannelData->GraphicInfo.pGuiTable[guiId], &ChannelData->GraphicInfo.ObjList[guiId])) {
                return RETURN_FAIL;
            }

            /** Step 2) Add Obj into canvas **/
            ChannelData->GraphicInfo.Canvas.ObjAdd_f(&ChannelData->GraphicInfo.Canvas, &ChannelData->GraphicInfo.ObjList[guiId]);
            rval = RETURN_SUCCESS;
            break;
        default:
            GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_GRAPHICS, "default type: [%d] %d", canvasChn, ChannelData->GraphicInfo.pGuiTable[guiId]->Type);
            break;
    }
    return rval;
}

static RETURN_TYPE_e _ApplibGraph_Object_UpdateColor(GRAPHICS_CHAN_e ChannelId, GRAPH_CHAN_s *ChannelData, UINT32 GuiId, UINT32 ForeColor, UINT32 BackColor)
{
    if (_ApplibGraph_Object_Check(ChannelId, GuiId, ChannelData) == RETURN_FAIL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphic][_ApplibGraph_Object_UpdateColor] NG!");
        return RETURN_FAIL;
    }

    /** Change the color for the specific GUI */
    switch (ChannelData->GraphicInfo.pGuiTable[GuiId]->Type) {
        case APPLIB_GRAPHIC_UIOBJ_STRING:
            {
                APPLIB_GRAPHIC_STR_CNT_s *cnt;
                cnt = (APPLIB_GRAPHIC_STR_CNT_s *)ChannelData->GraphicInfo.ObjList[GuiId].Content;
                cnt->ColorFore = ForeColor;
                cnt->ColorBack = BackColor;
                ChannelData->GraphicInfo.Canvas.ObjUpdate_f(&ChannelData->GraphicInfo.Canvas, ChannelData->GraphicInfo.ObjList[GuiId].ID, &ChannelData->GraphicInfo.ObjList[GuiId]);
            }
            break;
        default:
            break;
    }
    return RETURN_SUCCESS;
}

static RETURN_TYPE_e _ApplibGraph_Object_UpdatePosition(GRAPHICS_CHAN_e ChannelId, GRAPH_CHAN_s *ChannelData, UINT32 GuiId, UINT32 LeftTopX, UINT32 LeftTopY)
{
    if (_ApplibGraph_Object_Check(ChannelId, GuiId, ChannelData) == RETURN_FAIL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphic][_ApplibGraph_Object_UpdatePosition] NG!");
        return RETURN_FAIL;
    }

    /** Change the posotion for the specific GUI */
    switch (ChannelData->GraphicInfo.pGuiTable[GuiId]->Type) {
        case APPLIB_GRAPHIC_UIOBJ_RECT:
            {
                APPLIB_GRAPHIC_RECT_CNT_s *cnt = NULL;
                UINT32 width = 0, height = 0;
                cnt = (APPLIB_GRAPHIC_RECT_CNT_s*)ChannelData->GraphicInfo.ObjList[GuiId].Content;
                width = cnt->X2 - cnt->X1;
                height = cnt->Y2 - cnt->Y1;
                cnt->X1 = LeftTopX;
                cnt->Y1 = LeftTopY;
                cnt->X2 = cnt->X1 + width;
                cnt->Y2 = cnt->Y1 + height;
                if (cnt->X2 > ChannelData->GraphicInfo.Render.BufPitch) {
                    cnt->X2 = ChannelData->GraphicInfo.Render.BufPitch;
                }
                if (cnt->Y2 > ChannelData->GraphicInfo.Render.BufHeight) {
                    cnt->Y2 = ChannelData->GraphicInfo.Render.BufHeight;
                }
            }
            ChannelData->GraphicInfo.Canvas.ObjUpdate_f(&ChannelData->GraphicInfo.Canvas, ChannelData->GraphicInfo.ObjList[GuiId].ID, &ChannelData->GraphicInfo.ObjList[GuiId]);
            break;
        case APPLIB_GRAPHIC_UIOBJ_STRING:
            {
                APPLIB_GRAPHIC_STR_CNT_s *strCnt;
                strCnt = (APPLIB_GRAPHIC_STR_CNT_s *)ChannelData->GraphicInfo.ObjList[GuiId].Content;
                strCnt->Left = LeftTopX;
                strCnt->Top = LeftTopY;
                ChannelData->GraphicInfo.Canvas.ObjUpdate_f(&ChannelData->GraphicInfo.Canvas, ChannelData->GraphicInfo.ObjList[GuiId].ID, &ChannelData->GraphicInfo.ObjList[GuiId]);
            }
            break;
        case APPLIB_GRAPHIC_UIOBJ_BMP:
            {
                APPLIB_GRAPHIC_BMP_CNT_s *bmpCnt;
                bmpCnt = (APPLIB_GRAPHIC_BMP_CNT_s *)ChannelData->GraphicInfo.ObjList[GuiId].Content;
                bmpCnt->Left = LeftTopX;
                bmpCnt->Bottom = LeftTopY;
                ChannelData->GraphicInfo.Canvas.ObjUpdate_f(&ChannelData->GraphicInfo.Canvas, ChannelData->GraphicInfo.ObjList[GuiId].ID, &ChannelData->GraphicInfo.ObjList[GuiId]);
            }
            break;
        default:
            break;
    }
    return RETURN_SUCCESS;
}

static RETURN_TYPE_e _ApplibGraph_Object_UpdateSize(GRAPHICS_CHAN_e ChannelId, GRAPH_CHAN_s *ChannelData, UINT32 GuiId, UINT32 Width, UINT32 Height, UINT32 StrSize)
{
    if (_ApplibGraph_Object_Check(ChannelId, GuiId, ChannelData) == RETURN_FAIL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphic][_ApplibGraph_Object_UpdateSize] NG!");
        return RETURN_FAIL;
    }

    /** Change the size for the specific GUI */
    switch (ChannelData->GraphicInfo.pGuiTable[GuiId]->Type) {
        case APPLIB_GRAPHIC_UIOBJ_RECT:
            {
                APPLIB_GRAPHIC_RECT_CNT_s *cnt = (APPLIB_GRAPHIC_RECT_CNT_s*)ChannelData->GraphicInfo.ObjList[GuiId].Content;
                cnt->X2 = cnt->X1 + Width;
                cnt->Y2 = cnt->Y1 + Height;
                if (cnt->X2 > ChannelData->GraphicInfo.Render.BufPitch) {
                    cnt->X2 = ChannelData->GraphicInfo.Render.BufPitch;
                }
                if (cnt->Y2 > ChannelData->GraphicInfo.Render.BufHeight) {
                    cnt->Y2 = ChannelData->GraphicInfo.Render.BufHeight;
                }
            }
            ChannelData->GraphicInfo.Canvas.ObjUpdate_f(&ChannelData->GraphicInfo.Canvas, ChannelData->GraphicInfo.ObjList[GuiId].ID, &ChannelData->GraphicInfo.ObjList[GuiId]);
            break;
        case APPLIB_GRAPHIC_UIOBJ_BMP:
            {
                APPLIB_GRAPHIC_BMP_CNT_s *cnt = (APPLIB_GRAPHIC_BMP_CNT_s*)ChannelData->GraphicInfo.ObjList[GuiId].Content;
                APPLIB_GRAPHIC_BMP_s *bmp = cnt->BmpDescPtr->BmpPtr;
                bmp->Width = Width;
                bmp->Height = Height;
                ChannelData->GraphicInfo.Canvas.ObjUpdate_f(&ChannelData->GraphicInfo.Canvas, ChannelData->GraphicInfo.ObjList[GuiId].ID, &ChannelData->GraphicInfo.ObjList[GuiId]);
            }
            break;
        case APPLIB_GRAPHIC_UIOBJ_STRING:
            {
                APPLIB_GRAPHIC_STR_CNT_s *cnt = (APPLIB_GRAPHIC_STR_CNT_s*)ChannelData->GraphicInfo.ObjList[GuiId].Content;
                cnt->Width = Width;
                cnt->Height = Height;
                cnt->StrSize = StrSize;
                ChannelData->GraphicInfo.Canvas.ObjUpdate_f(&ChannelData->GraphicInfo.Canvas, ChannelData->GraphicInfo.ObjList[GuiId].ID, &ChannelData->GraphicInfo.ObjList[GuiId]);
            }
            break;
        default:
            break;
    }
    return RETURN_SUCCESS;
}

static RETURN_TYPE_e _ApplibGraph_Bitmap_UpdateId(GRAPHICS_CHAN_e ChannelId, GRAPH_CHAN_s *ChannelData, UINT32 BmpId, UINT32 GuiId)
{
    APPLIB_GRAPHIC_BMP_CNT_s *cnt = NULL;

    if (_ApplibGraph_Object_Check(ChannelId, GuiId, ChannelData) == RETURN_FAIL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphic][_ApplibGraph_Bitmap_UpdateId] NG!");
        return RETURN_FAIL;
    }
    /** Change the BMP for the specific GUI */
    cnt = (APPLIB_GRAPHIC_BMP_CNT_s *)ChannelData->GraphicInfo.ObjList[GuiId].Content;
    cnt->BMPIdx = BmpId;
    cnt->BmpDescPtr = &((APPLIB_GRAPHIC_BMP_BIN_INFO_s *)ChannelData->GraphicInfo.BitmapBuffer)->DescTab[BmpId];
    ChannelData->GraphicInfo.Canvas.ObjUpdate_f(&ChannelData->GraphicInfo.Canvas, ChannelData->GraphicInfo.ObjList[GuiId].ID, &ChannelData->GraphicInfo.ObjList[GuiId]);

    return RETURN_SUCCESS;
}

static RETURN_TYPE_e _ApplibGraph_String_UpdateId(GRAPHICS_CHAN_e ChannelId, GRAPH_CHAN_s *ChannelData, UINT32 StrId, UINT32 GuiId)
{
    APPLIB_GRAPHIC_STR_CNT_s *cnt = NULL;
    if (_ApplibGraph_Object_Check(ChannelId, GuiId, ChannelData) == RETURN_FAIL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphic][_ApplibGraph_String_UpdateId] _ApplibGraph_Object_Check NG!");
        return RETURN_FAIL;
    }
    /** Change the String for the specific GUI */
    cnt = (APPLIB_GRAPHIC_STR_CNT_s *)ChannelData->GraphicInfo.ObjList[GuiId].Content;
    cnt->MsgIdx = StrId;
#ifdef CONFIG_APP_ARD
    cnt->StrDesc = &((APPLIB_GRAPHIC_STR_BIN_INFO_s *)ChannelData->GraphicInfo.StringBuffer[ChannelData->GraphicInfo.ActiveLangId])->DescTable[StrId];
#else
    cnt->StrDesc = &((APPLIB_GRAPHIC_STR_BIN_INFO_s *)ChannelData->GraphicInfo.StringBuffer)->DescTable[StrId];
#endif
    ChannelData->GraphicInfo.Canvas.ObjUpdate_f(&ChannelData->GraphicInfo.Canvas, ChannelData->GraphicInfo.ObjList[GuiId].ID, &ChannelData->GraphicInfo.ObjList[GuiId]);

    return RETURN_SUCCESS;
}

static RETURN_TYPE_e _ApplibGraph_String_UpdateContext(GRAPHICS_CHAN_e ChannelId, GRAPH_CHAN_s *ChannelData, UINT32 LangIdx, UINT32 StrId, UINT16 *str)
{
    APPLIB_GRAPHIC_STR_BIN_DESC_s *binDesc = NULL;
#ifdef CONFIG_APP_ARD
	if ( !ChannelData->GraphicInfo.StringBuffer[LangIdx] ) {
#else
    if ( !ChannelData->GraphicInfo.StringBuffer ) {
#endif
        if (_AppLibGraph_String_Load(ChannelId, LangIdx, ChannelData) == BOOLEAN_FALSE) {
            return RETURN_FAIL;
        }
    }

#ifdef CONFIG_APP_ARD
	binDesc = &((APPLIB_GRAPHIC_STR_BIN_INFO_s *)ChannelData->GraphicInfo.StringBuffer[LangIdx])->DescTable[StrId];
#else
    binDesc = &((APPLIB_GRAPHIC_STR_BIN_INFO_s *)ChannelData->GraphicInfo.StringBuffer)->DescTable[StrId];
#endif
    if  (binDesc->Ptr != NULL) {
        memset(binDesc->Ptr, 0, sizeof(WCHAR) * w_strlen(binDesc->Ptr));
        w_strncpy(binDesc->Ptr, (WCHAR*)str, w_strlen((WCHAR*)str));
    }
    return RETURN_SUCCESS;
}

static RETURN_TYPE_e _ApplibGraph_String_GetWidth(GRAPHICS_CHAN_e ChannelId, GRAPH_CHAN_s *ChannelData, UINT32 GuiId, UINT32 StrSize, UINT32 *StrWidth)
{
    APPLIB_GRAPHIC_STR_CNT_s *cnt = NULL;
    if (_ApplibGraph_Object_Check(GRAPHICS_CHANNEL_BLEND, GuiId, ChannelData)== RETURN_FAIL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphic][_ApplibGraph_String_GetWidth] NG!");
        return RETURN_FAIL;
    }

    /** Change the String for the specific GUI */
    cnt = (APPLIB_GRAPHIC_STR_CNT_s *)ChannelData->GraphicInfo.ObjList[GuiId].Content;
    *StrWidth = _InitConfig.Font.FontFunc.GetStrWidth_f(StrSize, cnt->StrDesc->Ptr);

    return RETURN_SUCCESS;
}

static RETURN_TYPE_e _ApplibGraph_Font_Init(void)
{
    int ReturnValue = AMP_OK;
    void *MemoryPoolAddr_Raw = NULL;

    /* To check initialization status. */
    if ( (InitFlag & GRAPHICS_INIT_FONT) == GRAPHICS_INIT_FONT) {
        return RETURN_SUCCESS;
    }

    /* Error check */
    if (_InitConfig.Font.FontFileName == NULL) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_AppLibGraph_Font_Init> Filename wrong!");
        return RETURN_FAIL;
    }

    AppLibFont_SetType((APPLIB_FONT_TYPE_e)_InitConfig.Font.FontType);
    switch(_InitConfig.Font.FontType) {
        case FONT_TYPE_BMP:
            AppLibFont_GetFunc((APPLIB_FONT_FUNCTION_s*)&_InitConfig.Font.FontFunc);
            break;
        case FONT_TYPE_CUSTOMIZED:
#ifdef CONFIG_APP_ARD
            AppLibFont_GetFunc((APPLIB_FONT_FUNCTION_s*)&_InitConfig.Font.FontFunc);
#endif
            if ((_InitConfig.Font.FontFunc.GetFontSize_f == NULL) ||
                (_InitConfig.Font.FontFunc.Load_f == NULL) ||
                (_InitConfig.Font.FontFunc.Init_f == NULL) ||
                (_InitConfig.Font.FontFunc.Draw_f == NULL) ||
                (_InitConfig.Font.FontFunc.GetStrWidth_f == NULL) ||
                (_InitConfig.Font.FontFunc.GetStrHeight_f == NULL)) {
                GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_AppLibGraph_Font_Init> customized font function error!");
                return RETURN_FAIL;
            }
            break;
        default:
            return RETURN_FAIL;
    }

    /* Allocation memory */
    _InitConfig.Font.FontSize = _InitConfig.Font.FontFunc.GetFontSize_f(_InitConfig.Font.FontFileName);
    if (_InitConfig.Font.FontSize <= 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_AppLibGraph_Font_Init> %s file size = 0", _InitConfig.Font.FontFileName);
        return RETURN_FAIL;
    }

    ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, (void**)&_InitConfig.Font.FontBuffer, &MemoryPoolAddr_Raw, _InitConfig.Font.FontSize, 32);
    if (ReturnValue < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_AppLibGraph_Font_Init> Allocate Font working buffer fail.");
        //AmbaKAL_BytePoolFree(MemoryPoolAddr_Raw);
        MemoryPoolAddr_Raw = NULL;
        return RETURN_FAIL;
    }
    memset(_InitConfig.Font.FontBuffer, 0x0, _InitConfig.Font.FontSize);

    /* Load font data into allocated memory */
    ReturnValue = _InitConfig.Font.FontFunc.Load_f(_InitConfig.Font.FontBuffer);
    if (ReturnValue < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_AppLibGraph_Font_Init> Load Font fail.");
        AmbaKAL_BytePoolFree(MemoryPoolAddr_Raw);
        MemoryPoolAddr_Raw = NULL;
        return RETURN_FAIL;
    }

    /* Initialize font module */
    ReturnValue = _InitConfig.Font.FontFunc.Init_f();
    if (ReturnValue < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_AppLibGraph_Font_Init> Init Font fail.");
        AmbaKAL_BytePoolFree(MemoryPoolAddr_Raw);
        MemoryPoolAddr_Raw = NULL;
        return RETURN_FAIL;
    }

    InitFlag |= GRAPHICS_INIT_FONT;
    return RETURN_SUCCESS;
}

static RETURN_TYPE_e _ApplibGraph_Init(GRAPH_CHAN_s *ChannelData)
{
    /* To initiailze Render. */
    ChannelData->GraphicInfo.Render.Buf = ChannelData->GraphicInfo.WorkingBuffer.Address;
    ChannelData->GraphicInfo.Render.BufHeight = ChannelData->GraphicInfo.WorkingBuffer.Height;
    ChannelData->GraphicInfo.Render.BufPitch = ChannelData->GraphicInfo.WorkingBuffer.Width * _CheckPixelSize(ChannelData->OsdInfo.PixelFormat);
    ChannelData->GraphicInfo.Render.BufPixelSize = _CheckPixelSize(ChannelData->OsdInfo.PixelFormat);
    if (AppLibRender_Init(&ChannelData->GraphicInfo.Render) < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_ApplibGraph_Init> AppLibRender_Init fail.");
        return RETURN_FAIL;
    }

    /* To initialize Font. */
    _ApplibGraph_Font_Init();
    return RETURN_SUCCESS;
}

//-----------------------------
/**
 *  @brief Init graphics module
 *
 *  Init graphics module
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */
int AppLibGraph_Init(void)
{
    int ReturnValue = 0;

    if ( (InitFlag & GRAPHICS_INIT_GRAPHICS) == GRAPHICS_INIT_GRAPHICS ) {
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_GRAPHICS, "[Graphics] <Init> has done");
        return AMP_OK;
    }

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_GRAPHICS, "[Graphics] <Init> start");

    /** Fchan OSD buffer setup. */
    if (_InitConfig.FchanEnable == BOOLEAN_TRUE) {
        AMP_AREA_s SourceArea = {0};
        AMP_AREA_s TargetArea = {0};

        /* Allocatre osd buffer */
        if (_ApplibGraph_Buffer_Create(BUFFER_TYPE_WORKING, &_FchanData) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
        if (_ApplibGraph_Buffer_Create(BUFFER_TYPE_DISPLAY_1ST, &_FchanData) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
        if (_ApplibGraph_Buffer_Create(BUFFER_TYPE_DISPLAY_2ND, &_FchanData) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
        _FchanData.GraphicInfo.BufferIdx = 0;

        /* Init OSD MW */
        if (_MwOsd_Init(GRAPHICS_CHANNEL_F, &_FchanData) == RETURN_FAIL) {
            AmbaKAL_BytePoolFree(_FchanData.GraphicInfo.DispalyBuffer.RawAddress[0]);
            _FchanData.GraphicInfo.DispalyBuffer.RawAddress[0] = NULL;
            AmbaKAL_BytePoolFree(_FchanData.GraphicInfo.DispalyBuffer.RawAddress[1]);
            _FchanData.GraphicInfo.DispalyBuffer.RawAddress[1] = NULL;
            return AMP_ERROR_GENERAL_ERROR;
        }

        /* Init Clut */
        if (_FchanData.OsdInfo.PixelFormat == AMP_OSD_8BIT_CLUT_MODE) {
            AMP_OSD_CLUT_CFG_s OsdClut = {0};
            _ApplibGraph_Clut_Load(_InitConfig.ClutFileName, &OsdClut);

            if (AmpOsd_SetClutCfg(_FchanData.OsdInfo.Hdlr, &OsdClut) < 0) {
                GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <Init> F-Chan: AmpOsd_SetClutCfg fail");
                AppLibBlend_SetClutTable(NULL);
                return AMP_ERROR_GENERAL_ERROR;
            }
            AppLibBlend_SetClutTable(OsdClut.Clut);
        }

        /* Init Window MW */
        SourceArea.X = SourceArea.Y = 0;
        SourceArea.Width = _FchanData.GraphicInfo.DispalyBuffer.Width;
        SourceArea.Height = _FchanData.GraphicInfo.DispalyBuffer.Height;
        TargetArea.X = TargetArea.Y = 0;
        TargetArea.Width = 1920;
        TargetArea.Height = 1080;
        if (_MwWindow_Init(GRAPHICS_CHANNEL_F, &_FchanData, &SourceArea, &TargetArea) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }

        /* Update active flag */
        _FchanData.PathActived = BOOLEAN_TRUE;

        /* Init Graphics */
        if (_ApplibGraph_Init(&_FchanData) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }

    /** Dchan OSD buffer setup. */
    if (_InitConfig.DchanEnable == BOOLEAN_TRUE) {
        AMP_AREA_s SourceArea = {0};
        AMP_AREA_s TargetArea = {0};

        /* Allocatre osd buffer */
        if (_ApplibGraph_Buffer_Create(BUFFER_TYPE_WORKING, &_DchanData) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
        if (_ApplibGraph_Buffer_Create(BUFFER_TYPE_DISPLAY_1ST, &_DchanData) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
        if (_ApplibGraph_Buffer_Create(BUFFER_TYPE_DISPLAY_2ND, &_DchanData) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
        _DchanData.GraphicInfo.BufferIdx = 0;

        /* Init OSD MW */
        if (_MwOsd_Init(GRAPHICS_CHANNEL_D, &_DchanData) == RETURN_FAIL) {
            AmbaKAL_BytePoolFree(_DchanData.GraphicInfo.DispalyBuffer.RawAddress[0]);
            _DchanData.GraphicInfo.DispalyBuffer.RawAddress[0] = NULL;
            AmbaKAL_BytePoolFree(_DchanData.GraphicInfo.DispalyBuffer.RawAddress[1]);
            _DchanData.GraphicInfo.DispalyBuffer.RawAddress[1] = NULL;
            return AMP_ERROR_GENERAL_ERROR;
        }

        /* Init Clut */
        if (_DchanData.OsdInfo.PixelFormat == AMP_OSD_8BIT_CLUT_MODE) {
            AMP_OSD_CLUT_CFG_s OsdClut = {0};
            _ApplibGraph_Clut_Load(_InitConfig.ClutFileName, &OsdClut);

            if (AmpOsd_SetClutCfg(_DchanData.OsdInfo.Hdlr, &OsdClut) < 0) {
                GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <Init> D-Chan: AmpOsd_SetClutCfg fail");
                AppLibBlend_SetClutTable(NULL);
                return AMP_ERROR_GENERAL_ERROR;
            }
            AppLibBlend_SetClutTable(OsdClut.Clut);
        }

        /* Init Window MW */
        SourceArea.X = SourceArea.Y = 0;
        SourceArea.Width = _DchanData.GraphicInfo.DispalyBuffer.Width;
        SourceArea.Height = _DchanData.GraphicInfo.DispalyBuffer.Height;
        TargetArea.X = TargetArea.Y = 0;
        TargetArea.Width = _DchanData.GraphicInfo.DispalyBuffer.Width;
        TargetArea.Height = _DchanData.GraphicInfo.DispalyBuffer.Height;
        if (_MwWindow_Init(GRAPHICS_CHANNEL_D, &_DchanData, &SourceArea, &TargetArea) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }

        /* Update active flag */
        _DchanData.PathActived = BOOLEAN_TRUE;

        /* Init Graphics */
        if (_ApplibGraph_Init(&_DchanData) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    /** Blending buffer setup. */
    if (_InitConfig.BlendEnable == BOOLEAN_TRUE) {
        /* Allocatre osd buffer */
        if (_ApplibGraph_Buffer_Create(BUFFER_TYPE_WORKING, &_BlendData) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
        if (_ApplibGraph_Buffer_Create(BUFFER_TYPE_DISPLAY_1ST, &_BlendData) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
        if (_ApplibGraph_Buffer_Create(BUFFER_TYPE_DISPLAY_2ND, &_BlendData) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }

        /* Init OSD MW */
        if (_MwOsd_Init(GRAPHICS_CHANNEL_BLEND, &_BlendData) == RETURN_FAIL) {
            AmbaKAL_BytePoolFree(_BlendData.GraphicInfo.DispalyBuffer.RawAddress[0]);
            _BlendData.GraphicInfo.DispalyBuffer.RawAddress[0] = NULL;
            AmbaKAL_BytePoolFree(_BlendData.GraphicInfo.DispalyBuffer.RawAddress[1]);
            _BlendData.GraphicInfo.DispalyBuffer.RawAddress[1] = NULL;
            return AMP_ERROR_GENERAL_ERROR;
        }

        /* Init Clut */
        if ((!_DchanData.GraphicInfo.WorkingBuffer.Address) && (!_FchanData.GraphicInfo.WorkingBuffer.Address)){
            AMP_OSD_CLUT_CFG_s OsdClut = {0};
            _ApplibGraph_Clut_Load(_InitConfig.ClutFileName, &OsdClut);

            if (AmpOsd_SetClutCfg(_BlendData.OsdInfo.Hdlr, &OsdClut) < 0) {
                GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <Init> Blend-Chan: AmpOsd_SetClutCfg fail");
                AppLibBlend_SetClutTable(NULL);
                return AMP_ERROR_GENERAL_ERROR;
            }
            AppLibBlend_SetClutTable(OsdClut.Clut);
        }

        /* Update active flag */
        _BlendData.PathActived = BOOLEAN_TRUE;

        /* Init Graphics */
        if (_ApplibGraph_Init(&_BlendData) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }

    /* Turn off init flag */
    InitFlag |= GRAPHICS_INIT_GRAPHICS;

    /* F-Chan */
    if (_FchanData.PathActived == BOOLEAN_TRUE) {
        /* Canvas */
        {
            APPLIB_CANVAS_CFG_s TmpCvsCfg = {0};
            TmpCvsCfg.Area.Width = 1920;
            TmpCvsCfg.Area.Height = 1080;
            TmpCvsCfg.ObjNumMax = _FchanData.GraphicInfo.ObjectMaxAmout;
            TmpCvsCfg.CanvasCacheBaseAddr = 0;
            TmpCvsCfg.CanvasCacheSize = 0;
            if (AppLibCanvas_CalMemSize(&TmpCvsCfg, &TmpCvsCfg.CanvasCacheSize) >= 0) {
                void *Buffer = NULL;
                ReturnValue = _AllocateMemory(&Buffer, &TmpCvsCfg.CanvasCacheBaseAddr, TmpCvsCfg.CanvasCacheSize);
                if (ReturnValue < 0) {
                    GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <Init> Canvas Fchan: Allocate memory fail");
                    return ReturnValue;
                }
                AppLibCanvas_Create(&_FchanData.GraphicInfo.Canvas, &TmpCvsCfg, &_FchanData.GraphicInfo.Render);
            }
        }
    }
    if (_DchanData.PathActived == BOOLEAN_TRUE) {
        /* Canvas */
        {
            APPLIB_CANVAS_CFG_s TmpCvsCfg = {0};
            AppLibCanvas_GetDefCfg(&TmpCvsCfg);
            TmpCvsCfg.ObjNumMax = _DchanData.GraphicInfo.ObjectMaxAmout;
            if (AppLibCanvas_CalMemSize(&TmpCvsCfg, &TmpCvsCfg.CanvasCacheSize) >= 0) {
                void *Buffer = NULL;
                ReturnValue = _AllocateMemory(&Buffer, &TmpCvsCfg.CanvasCacheBaseAddr, TmpCvsCfg.CanvasCacheSize);
                if (ReturnValue < 0) {
                    GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[ Graphics] <Init> Canvas Dchan: Allocate memory fail");
                    return ReturnValue;
                }
                AppLibCanvas_Create(&_DchanData.GraphicInfo.Canvas, &TmpCvsCfg, &_DchanData.GraphicInfo.Render);
            }
        }
    }
    if (_BlendData.PathActived == BOOLEAN_TRUE) {
        APPLIB_CANVAS_CFG_s TmpCvsCfg = {0};
        AppLibCanvas_GetDefCfg(&TmpCvsCfg);
        TmpCvsCfg.ObjNumMax = _BlendData.GraphicInfo.ObjectMaxAmout;
        if (AppLibCanvas_CalMemSize(&TmpCvsCfg, &TmpCvsCfg.CanvasCacheSize) >= 0) {
            void *Buffer = NULL;
            ReturnValue = _AllocateMemory(&Buffer, &TmpCvsCfg.CanvasCacheBaseAddr, TmpCvsCfg.CanvasCacheSize);
            if (ReturnValue < 0) {
                GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <Init> Canvas Blend: Allocate memory fail");
                return ReturnValue;
            }
            AppLibCanvas_Create(&_BlendData.GraphicInfo.Canvas, &TmpCvsCfg, &_BlendData.GraphicInfo.Render);
        }

        AppLibGraph_InitStamp();
    }

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_GRAPHICS, "[Graphics] <Init> End");

    return 0;
}

/**
 *  @brief Configure graphics module
 *
 *  Configure graphics module
 *
 *  @return
 *  @see
 */
void AppLibGraph_SetDefaultConfig(APPLIB_GRAPH_INIT_CONFIG_s initConfig)
{
    memcpy(&_InitConfig, &initConfig, sizeof(APPLIB_GRAPH_INIT_CONFIG_s));
}

/**
 *  @brief Set OSD size
 *
 *  Set OSD size
 *
 *  @param [in] graphChannelId The channel id of Graphic
 *  @param [in] width The width of OSD
 *  @param [in] height The height of OSD
 *
 *  @return 0 - OK, others - AMP_ER_CODE_e
 *  @see AMP_ER_CODE_e
 */

int AppLibGraph_SetOsdSize(UINT32 graphChannelId, int width, int height)
{
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN)) {
        _FchanData.GraphicInfo.WorkingBuffer.Width = width;
        _FchanData.GraphicInfo.WorkingBuffer.Height = height;
        _FchanData.GraphicInfo.DispalyBuffer.Width = width;
        _FchanData.GraphicInfo.DispalyBuffer.Height = height;
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN)) {
        _DchanData.GraphicInfo.WorkingBuffer.Width = width;
        _DchanData.GraphicInfo.WorkingBuffer.Height = height;
        _DchanData.GraphicInfo.DispalyBuffer.Width = width;
        _DchanData.GraphicInfo.DispalyBuffer.Height = height;
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_BLEND)) {
        _BlendData.GraphicInfo.WorkingBuffer.Width = width;
        _BlendData.GraphicInfo.WorkingBuffer.Height = height;
        _BlendData.GraphicInfo.DispalyBuffer.Width = width;
        _BlendData.GraphicInfo.DispalyBuffer.Height = height;
    }

    return 0;
}

/**
 *  @brief Set pixel format
 *
 *  Set pixel format
 *
 *  @param [in] graphChannelId The channel id of Graphic
 *  @param [in] format Pixel format
 *
 *  @return >=0 success, <0 failure
 */
int AppLibGraph_SetPixelFormat(UINT32 graphChannelId, AMP_DISP_OSD_FORMAT_e format)
{
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN)) {
        _FchanData.OsdInfo.PixelFormat = format;
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN)) {
        _DchanData.OsdInfo.PixelFormat = format;
        if (_InitConfig.BlendEnable == BOOLEAN_TRUE) {
            _BlendData.OsdInfo.PixelFormat = format;
        }
    }
    return 0;
}

/**
 *  @brief Update the window of graphic
 *
 *  Update the window of graphic
 *
 *  @param [in] graphChannelId The channel id of Graphic
 *
 *  @return >=0 success, <0 failure
 */
int AppLibGraph_SetWindowConfig(UINT32 graphChannelId)
{
    int ReturnValue = 0;

    if ( (InitFlag & GRAPHICS_INIT_GRAPHICS) == GRAPHICS_INIT_NONE) {
        return AMP_ERROR_GENERAL_ERROR;
    }

    /** FCHAN window*/
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE)) {
        AMP_DISP_INFO_s DispDev = {0};
        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_FCHAN, &DispDev);

        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[AppLib - Graphics] <WindowUpdate> FChan Disable.");
        } else {
            AMP_DISP_WINDOW_CFG_s Window;
            memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));

            if (_MwWindow_RetrieveWindow(DISP_CH_FCHAN, _FchanData.WindowInfo.WindowId, &Window) == BOOLEAN_TRUE) {
                Window.TargetAreaOnPlane.Width = DispDev.DeviceInfo.VoutWidth;
                Window.TargetAreaOnPlane.Height = DispDev.DeviceInfo.VoutHeight;
                _MwWindow_ConfigWindow(DISP_CH_FCHAN, _FchanData.WindowInfo.WindowId, &Window);
            }
            if (_FchanData.GraphicInfo.EnableSwScale == BOOLEAN_TRUE) {
                _FchanData.GraphicInfo.DispalyBuffer.Width = DispDev.DeviceInfo.VoutWidth;
                _FchanData.GraphicInfo.DispalyBuffer.Height = DispDev.DeviceInfo.VoutHeight;
            }
        }
    }

    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE)) {
        AMP_DISP_INFO_s DispDev = {0};
        ReturnValue = AppLibDisp_GetDeviceInfo(DISP_CH_DCHAN, &DispDev);

        if ((ReturnValue < 0) || (DispDev.DeviceInfo.Enable == 0)) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[AppLib - Graphics] <WindowUpdate> DChan Disable.");
        } else {
            AMP_DISP_WINDOW_CFG_s Window;
            memset(&Window, 0, sizeof(AMP_DISP_WINDOW_CFG_s));

            if (_MwWindow_RetrieveWindow(DISP_CH_DCHAN, _DchanData.WindowInfo.WindowId, &Window) == BOOLEAN_TRUE) {
                Window.TargetAreaOnPlane.Width = DispDev.DeviceInfo.VoutWidth;
                Window.TargetAreaOnPlane.Height = DispDev.DeviceInfo.VoutHeight;
                _MwWindow_ConfigWindow(DISP_CH_DCHAN, _DchanData.WindowInfo.WindowId, &Window);
            }
            if (_DchanData.GraphicInfo.EnableSwScale == BOOLEAN_TRUE) {
                _DchanData.GraphicInfo.DispalyBuffer.Width = DispDev.DeviceInfo.VoutWidth;
                _DchanData.GraphicInfo.DispalyBuffer.Height = DispDev.DeviceInfo.VoutHeight;
            }
        }
    }

    return ReturnValue;
}

/**
 *  @brief Set max object numbers
 *
 *  To set the total number of object
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] objectNum total number of object
 *
 *  @return 0 Success
 *  @see
 */
int AppLibGraph_SetMaxObjectNum(UINT32 graphChannelId, UINT32 objectNum)
{
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_GRAPHICS, "[AppLib - Graphics] Set max object number");
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN)) {
        _FchanData.GraphicInfo.ObjectMaxAmout = objectNum;
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN)) {
        _DchanData.GraphicInfo.ObjectMaxAmout = objectNum;
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_BLEND)) {
        _BlendData.GraphicInfo.ObjectMaxAmout = objectNum;
    }
    return 0;
}

/**
 *  @brief Set GUI's layout
 *
 *  Set GUI's layout
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] layoutId the layout id for the specific channel
 *  @param[in] uiObjTable the gui table
 *
 *  @return 0 Success
 *  @see APPLIB_GRAPHIC_UIOBJ_s
 */
int AppLibGraph_SetGUILayout(UINT32 graphChannelId, UINT32 layoutId, APPLIB_GRAPHIC_UIOBJ_s *uiObjTable[], UINT32 langIdx)
{
    int rval = 0;

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_GRAPHICS, "[AppLib - Graphics] Set GUI's Layout");

    /* Init for D-Chan */
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE)) {
        if ((_DchanData.GraphicInfo.Resolution == layoutId) && (_DchanData.GraphicInfo.pGuiTable == uiObjTable)) {
            return rval;
        }
        _DchanData.GraphicInfo.Resolution = layoutId;
        _DchanData.GraphicInfo.pGuiTable = uiObjTable;

        /* Load BMP*/
        _AppLibGraph_BMP_Load(GRAPHICS_CHANNEL_D, &_DchanData);

        /* Load String */
        if (_AppLibGraph_String_Load(GRAPHICS_CHANNEL_D, langIdx, &_DchanData) == BOOLEAN_FALSE) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[ApplibGraphics] Load string fail!");
            return AMP_ERROR_IO_ERROR;
        }

        /* Init Canvas */
        if (_ApplibGraph_Canvas_Init(GRAPHICS_CHANNEL_D, &_DchanData)) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }

    /* Init for F-Chan */
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE)) {
        if ((_FchanData.GraphicInfo.Resolution == layoutId) && (_FchanData.GraphicInfo.pGuiTable == uiObjTable)) {
            return rval;
        }
        _FchanData.GraphicInfo.Resolution = layoutId;
        _FchanData.GraphicInfo.pGuiTable = uiObjTable;

        /* Load BMP*/
        _AppLibGraph_BMP_Load(GRAPHICS_CHANNEL_F, &_FchanData);

        /* Load String */
        if (_AppLibGraph_String_Load(GRAPHICS_CHANNEL_F, langIdx, &_FchanData) == BOOLEAN_FALSE) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[ApplibGraphics] Load string fail!");
            return AMP_ERROR_IO_ERROR;
        }

        /* Init Canvas */
        if (_ApplibGraph_Canvas_Init(GRAPHICS_CHANNEL_F, &_FchanData)) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }

    /* Init for Blend-Chan */
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_BLEND) && (_BlendData.PathActived == BOOLEAN_TRUE)) {
        if ((_BlendData.GraphicInfo.Resolution == layoutId) && (_BlendData.GraphicInfo.pGuiTable == uiObjTable)) {
            return rval;
        }
        _BlendData.GraphicInfo.Resolution = layoutId;
        _BlendData.GraphicInfo.pGuiTable = uiObjTable;

        /* Load BMP*/
        _AppLibGraph_BMP_Load(GRAPHICS_CHANNEL_BLEND, &_BlendData);

        /* Load String */
        if (_AppLibGraph_String_Load(GRAPHICS_CHANNEL_BLEND, langIdx, &_BlendData) == BOOLEAN_FALSE) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[ApplibGraphics] Load string fail!");
            return AMP_ERROR_IO_ERROR;
        }

        /* Init Canvas */
        if (_ApplibGraph_Canvas_Init(GRAPHICS_CHANNEL_BLEND, &_BlendData)) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    return 0;
}

/**
 *  @brief Enable draw the objecf
 *
 *  Enable draw the objecf
 *
 *  @param [in] graphChannelId The channel id of Graphic
 *
 *  @return >=0 success, <0 failure
 */
int AppLibGraph_EnableDraw(UINT32 graphChannelId)
{
   if ( (InitFlag & GRAPHICS_INIT_GRAPHICS) == GRAPHICS_INIT_NONE ) {
       return AMP_ERROR_GENERAL_ERROR;
   }
   if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE)) {
       _FchanData.GraphicInfo.EnableDraw = BOOLEAN_TRUE;
   }
   if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE)) {
       _DchanData.GraphicInfo.EnableDraw = BOOLEAN_TRUE;
   }
   return AMP_OK;
}

/**
 *  @brief Disable draw the objecf
 *
 *  Disable draw the objecf
 *
 *  @param [in] graphChannelId The channel id of Graphic
 *
 *  @return >=0 success, <0 failure
 */
int AppLibGraph_DisableDraw(UINT32 graphChannelId)
{
   if ( (InitFlag & GRAPHICS_INIT_GRAPHICS) == GRAPHICS_INIT_NONE ) {
       return AMP_ERROR_GENERAL_ERROR;
   }
   if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE)) {
       _FchanData.GraphicInfo.EnableDraw = BOOLEAN_FALSE;
   }
   if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE)) {
       _DchanData.GraphicInfo.EnableDraw = BOOLEAN_FALSE;
   }
   return AMP_OK;
}

/**
 *  @brief Active the window of graphic
 *
 *  Start the window of graphic
 *
 *  @param [in] graphChannelId The channel id of Graphic
 *
 *  @return >=0 success, <0 failure
 */
int AppLibGraph_ActivateWindow(UINT32 graphChannelId)
{
    int ReturnValue = AMP_ERROR_GENERAL_ERROR;

    if ( (InitFlag & GRAPHICS_INIT_GRAPHICS) == GRAPHICS_INIT_NONE) {
        return AMP_ERROR_GENERAL_ERROR;
    }

    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE)) {
        if (_MwWindow_ActiveWindow(GRAPH_CH_FCHAN, _FchanData.WindowInfo.WindowId) == BOOLEAN_TRUE) {
            ReturnValue = AMP_OK;
        }
    }

    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE)) {
        if (_MwWindow_ActiveWindow(GRAPH_CH_DCHAN, _DchanData.WindowInfo.WindowId) == BOOLEAN_TRUE) {
            ReturnValue = AMP_OK;
        }
    }
    return ReturnValue;
}

/**
 *  @brief Deactive the window of graphic
 *
 *  Start the window of graphic
 *
 *  @param [in] graphChannelId The channel id of Graphic
 *
 *  @return >=0 success, <0 failure
 */
int AppLibGraph_DeactivateWindow(UINT32 graphChannelId)
{
    int ReturnValue = AMP_ERROR_GENERAL_ERROR;

    if ( (InitFlag & GRAPHICS_INIT_GRAPHICS) == GRAPHICS_INIT_NONE) {
        return AMP_ERROR_GENERAL_ERROR;
    }

    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE)) {
        if (_MwWindow_DeactiveWindow(GRAPH_CH_FCHAN, _FchanData.WindowInfo.WindowId) == BOOLEAN_TRUE) {
            ReturnValue = AMP_OK;
        }
    }

    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE)) {
        if (_MwWindow_DeactiveWindow(GRAPH_CH_DCHAN, _DchanData.WindowInfo.WindowId) == BOOLEAN_TRUE) {
            ReturnValue = AMP_OK;
        }
    }
    return ReturnValue;
}

/**
 *  @brief Flush the window of graphic
 *
 *  Start the window of graphic
 *
 *  @param [in] graphChannelId The channel id of Graphic
 *
 *  @return >=0 success, <0 failure
 */
int AppLibGraph_FlushWindow(UINT32 graphChannelId)
{
    if ( (InitFlag & GRAPHICS_INIT_GRAPHICS) == GRAPHICS_INIT_NONE) {
        return AMP_ERROR_GENERAL_ERROR;
    }

    if (_MwWindow_FlushWindow(graphChannelId) == BOOLEAN_FALSE) {
        return AMP_ERROR_GENERAL_ERROR;
    }
    return AMP_OK;
}

/**
 *  @brief Get the width of the string
 *
 *  Get the width of the string
 *
 *  @param[in] strSize the string height
 *  @param[in] *str the specific string
 *
 *  @return 0 Success
 *  @see
 */
UINT32 AppLibGraph_GetStringWidth( UINT32 graphChannelId, UINT32 guiId, UINT32 strSize)
{
    UINT32 strWidth = 0;

    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_BLEND) && (_BlendData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_String_GetWidth(GRAPHICS_CHANNEL_BLEND, &_BlendData, guiId, strSize, &strWidth) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }

    return strWidth;
}

/**
 *  @brief Retrive GUI object's information
 *
 *  Retrive GUI object's information
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] guiId the gui table id
 *  @param[in] areaInfo the gui's area info
 *
 *  @return
 *  @see
 */
void AppLibGraph_RetrieveObjInfo(UINT32 graphChannelId, UINT32 guiId, AMP_AREA_s *areaInfo)
{
    APPLIB_GRAPHIC_OBJ_s queryObj;

    areaInfo->X = 0;
    areaInfo->Y = 0;
    areaInfo->Width = 0;
    areaInfo->Height = 0;

    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_BLEND) && (_BlendData.PathActived == BOOLEAN_TRUE)) {
        _BlendData.GraphicInfo.Canvas.ObjQuery_f(&_BlendData.GraphicInfo.Canvas, _BlendData.GraphicInfo.ObjList[guiId].ID, &queryObj);
    }

    if ( (queryObj.LastDisplayBox.Width == 0) && (queryObj.LastDisplayBox.Height == 0) ) {
        return;
    }

    areaInfo->X = queryObj.LastDisplayBox.X;
    areaInfo->Y = queryObj.LastDisplayBox.Y;
    areaInfo->Width = queryObj.LastDisplayBox.Width;
    areaInfo->Height = queryObj.LastDisplayBox.Height;
}

/**
 *  @brief Update BMP
 *
 *  Let user can change the specific BMP manually
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] guiId the gui table id
 *  @param[in] bmpId the modified BMP id
 *
 *  @return 0 Success, <0 Fail
 *  @see
 */
int AppLibGraph_UpdateBMP(UINT32 graphChannelId, UINT32 guiId, UINT32 bmpId)
{
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Bitmap_UpdateId(GRAPHICS_CHANNEL_D, &_DchanData, bmpId, guiId) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Bitmap_UpdateId(GRAPHICS_CHANNEL_F, &_FchanData, bmpId, guiId) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_BLEND) && (_BlendData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Bitmap_UpdateId(GRAPHICS_CHANNEL_BLEND, &_BlendData, bmpId, guiId) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    return AMP_OK;
}

/**
 *  @brief Update string
 *
 *  Let user can change the specific string manually
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] guiId the gui table id
 *  @param[in] strId the modified string id
 *
 *  @return 0 Success, <0 Fail
 *  @see
 */
int AppLibGraph_UpdateString(UINT32 graphChannelId, UINT32 guiId, UINT32 strId)
{
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_String_UpdateId(GRAPHICS_CHANNEL_D, &_DchanData, strId, guiId) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_String_UpdateId(GRAPHICS_CHANNEL_F, &_FchanData, strId, guiId) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_BLEND) && (_BlendData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_String_UpdateId(GRAPHICS_CHANNEL_BLEND, &_BlendData, strId, guiId) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }

    return AMP_OK;
}

/**
 *  @brief Update string's context in BIN
 *
 *  Change string's context
 *
 *  @param[in] langIdx the language id of the specific string
 *  @param[in] strId the specific string id
 *  @param[in] str the updated stinr
 *
 *  @return 0 Success, <0 Fail
 *  @see
 */
int AppLibGraph_UpdateStringContext(UINT32 langIdx, UINT32 strId, UINT16 *str)
{
    if (_ApplibGraph_String_UpdateContext(GRAPHICS_CHANNEL_D, &_DchanData, langIdx, strId, str) == RETURN_FAIL) {
        return AMP_ERROR_IO_ERROR;
    }

    if (_ApplibGraph_String_UpdateContext(GRAPHICS_CHANNEL_F, &_FchanData, langIdx, strId, str) == RETURN_FAIL) {
        return AMP_ERROR_IO_ERROR;
    }

    if (_ApplibGraph_String_UpdateContext(GRAPHICS_CHANNEL_BLEND, &_BlendData, langIdx, strId, str) == RETURN_FAIL) {
        return AMP_ERROR_IO_ERROR;
    }
    return AMP_OK;
}

/**
 *  @brief Update GUI object's color
 *
 *  Change GUI object's color
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] guiId the gui table id
 *  @param[in] foreColor the fore color of the gui id
 *  @param[in] backColor the back color of the gui id
 *
 *  @return 0 Success, <0 Fail
 *  @see
 */
int AppLibGraph_UpdateColor(UINT32 graphChannelId, UINT32 guiId, UINT32 foreColor, UINT32 backColor)
{
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Object_UpdateColor(GRAPHICS_CHANNEL_D, &_DchanData, guiId, foreColor, backColor) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Object_UpdateColor(GRAPHICS_CHANNEL_F, &_FchanData, guiId, foreColor, backColor) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_BLEND) && (_BlendData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Object_UpdateColor(GRAPHICS_CHANNEL_BLEND, &_BlendData, guiId, foreColor, backColor) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    return AMP_OK;
}

/**
 *  @brief Update GUI object's position
 *
 *  Change GUI object's position
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] guiId the gui table id
 *  @param[in] left X coordinate of Graphic obj's Bottom left
 *  @param[in] bottom Y coordinate of Graphic obj's Bottom right
 *
 *  @return 0 Success, <0 Fail
 *  @see
 */
int AppLibGraph_UpdatePosition(UINT32 graphChannelId, UINT32 guiId, UINT32 left, UINT32 bottom)
{
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Object_UpdatePosition(GRAPHICS_CHANNEL_D, &_DchanData, guiId, left, bottom) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Object_UpdatePosition(GRAPHICS_CHANNEL_F, &_FchanData, guiId, left, bottom) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_BLEND) && (_BlendData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Object_UpdatePosition(GRAPHICS_CHANNEL_BLEND, &_BlendData, guiId, left, bottom) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    return AMP_OK;
}

/**
 *  @brief Update GUI object's size
 *
 *  Change GUI object's size
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] guiId the gui table id
 *  @param[in] width the gui's modified width
 *  @param[in] height the gui's modified height
 *  @param[in] strSize the string's modified height
 *
 *  @return 0 Success, <0 Fail
 *  @see
 */
int AppLibGraph_UpdateSize(UINT32 graphChannelId, UINT32 guiId, UINT32 width, UINT32 height, UINT32 strSize )
{
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Object_UpdateSize(GRAPHICS_CHANNEL_D, &_DchanData, guiId, width, height, strSize) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Object_UpdateSize(GRAPHICS_CHANNEL_F, &_FchanData, guiId, width, height, strSize) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_BLEND) && (_BlendData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Object_UpdateSize(GRAPHICS_CHANNEL_BLEND, &_BlendData, guiId, width, height, strSize) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    return AMP_OK;
}

/**
 *  @brief Show shape
 *
 *  Show shape (including rectangle, line, circle)
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] uiObj the shape object
 *
 *  @return 0 Success
 *  @see
 */
int AppLibGraph_ShowShape(UINT32 graphChannelId, APPLIB_GRAPHIC_UIOBJ_s *uiObj)
{
    APPLIB_GRAPHIC_OBJ_s Obj;
    AppLibGraphic_CreateObj(uiObj, &Obj);

    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE)) {
        _DchanData.GraphicInfo.Canvas.ObjAdd_f(&_DchanData.GraphicInfo.Canvas, &Obj);
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE)) {
        _FchanData.GraphicInfo.Canvas.ObjAdd_f(&_FchanData.GraphicInfo.Canvas, &Obj);
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_BLEND) && (_BlendData.PathActived == BOOLEAN_TRUE)) {
        _BlendData.GraphicInfo.Canvas.ObjAdd_f(&_BlendData.GraphicInfo.Canvas, &Obj);
    }
    return 0;
}

/**
 *  @brief Show GUI objects
 *
 *  Set the specific gui object visible
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] guiId the specific object id
 *
 *  @return 0 Success, <0 Fail
 *  @see
 */
int AppLibGraph_Show(UINT32 graphChannelId, UINT32 guiId)
{
    int rval = -1;
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Object_Check(GRAPHICS_CHANNEL_D, guiId, &_DchanData) == RETURN_SUCCESS) {
            _DchanData.GraphicInfo.ObjList[guiId].Show = 1;
            _DchanData.GraphicInfo.Canvas.ObjUpdate_f(&_DchanData.GraphicInfo.Canvas, _DchanData.GraphicInfo.ObjList[guiId].ID, &_DchanData.GraphicInfo.ObjList[guiId]);
            rval = 0;
        }
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Object_Check(GRAPHICS_CHANNEL_F, guiId, &_FchanData) == RETURN_SUCCESS) {
            _FchanData.GraphicInfo.ObjList[guiId].Show = 1;
            _FchanData.GraphicInfo.Canvas.ObjUpdate_f(&_FchanData.GraphicInfo.Canvas, _FchanData.GraphicInfo.ObjList[guiId].ID, &_FchanData.GraphicInfo.ObjList[guiId]);
            rval = 0;
        }
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_BLEND) && (_BlendData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Object_Check(GRAPHICS_CHANNEL_BLEND, guiId, &_BlendData) == RETURN_SUCCESS) {
            _BlendData.GraphicInfo.ObjList[guiId].Show = 1;
            _BlendData.GraphicInfo.Canvas.ObjUpdate_f(&_BlendData.GraphicInfo.Canvas, _BlendData.GraphicInfo.ObjList[guiId].ID, &_BlendData.GraphicInfo.ObjList[guiId]);
            rval = 0;
        }
    }
    return rval;
}

/**
 *  @brief Hide GUI objects
 *
 *  Set the specific gui object un-visible
 *
 *  @param[in] graphChannelId the specific channel
 *  @param[in] guiId the specific object id
 *
 *  @return 0 Success, <0 Fail
 *  @see AppLibGraph_HideAll
 */
int AppLibGraph_Hide(UINT32 graphChannelId, UINT32 guiId)
{
    int rval = -1;
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Object_Check(GRAPHICS_CHANNEL_D, guiId, &_DchanData) == RETURN_SUCCESS) {
            _DchanData.GraphicInfo.ObjList[guiId].Show = 0;
            _DchanData.GraphicInfo.Canvas.ObjUpdate_f(&_DchanData.GraphicInfo.Canvas, _DchanData.GraphicInfo.ObjList[guiId].ID, &_DchanData.GraphicInfo.ObjList[guiId]);
            rval = 0;
        }
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Object_Check(GRAPHICS_CHANNEL_F, guiId, &_FchanData) == RETURN_SUCCESS) {
            _FchanData.GraphicInfo.ObjList[guiId].Show = 0;
            _FchanData.GraphicInfo.Canvas.ObjUpdate_f(&_FchanData.GraphicInfo.Canvas, _FchanData.GraphicInfo.ObjList[guiId].ID, &_FchanData.GraphicInfo.ObjList[guiId]);
            rval = 0;
        }
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_BLEND) && (_BlendData.PathActived == BOOLEAN_TRUE)) {
        if (_ApplibGraph_Object_Check(GRAPHICS_CHANNEL_BLEND, guiId, &_BlendData) == RETURN_SUCCESS) {
            _BlendData.GraphicInfo.ObjList[guiId].Show = 0;
            _BlendData.GraphicInfo.Canvas.ObjUpdate_f(&_BlendData.GraphicInfo.Canvas, _BlendData.GraphicInfo.ObjList[guiId].ID, &_BlendData.GraphicInfo.ObjList[guiId]);
            rval = 0;
        }
    }
    return rval;
}

/**
 *  @brief Hide All
 *
 *  Hide all objects on the specific channel
 *
 *  @param[in] graphChannelId the specific channel
 *
 *  @return 0 Success
 *  @see AppLibGraph_Hide
 */
int AppLibGraph_HideAll(UINT32 graphChannelId)
{
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE)) {
        _DchanData.GraphicInfo.Canvas.ObjHideAll_f(&_DchanData.GraphicInfo.Canvas);
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE)) {
        _FchanData.GraphicInfo.Canvas.ObjHideAll_f(&_FchanData.GraphicInfo.Canvas);
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_BLEND) && (_BlendData.PathActived == BOOLEAN_TRUE)) {
        _BlendData.GraphicInfo.Canvas.ObjHideAll_f(&_BlendData.GraphicInfo.Canvas);
    }
    return 0;
}

/**
 *  @brief Draw all objects on the canvas
 *
 *  Draw all objects on the canvas
 *
 *  @param[in] graphChannelId the specific channel
 *
 *  @return 0 Success
 *  @see
 */
int AppLibGraph_Draw(UINT32 graphChannelId)
{
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE) && (_DchanData.GraphicInfo.EnableDraw == BOOLEAN_TRUE)) {
        if (_DchanData.GraphicInfo.Canvas.CanvasDraw_f) {
            _DchanData.GraphicInfo.Canvas.CanvasDraw_f(&_DchanData.GraphicInfo.Canvas);
        }
        _ApplibGraph_Buffer_Sync(&_DchanData);
        if (_ApplibGraph_Buffer_Switch(&_DchanData) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE) && (_FchanData.GraphicInfo.EnableDraw == BOOLEAN_TRUE)) {
        if (_FchanData.GraphicInfo.Canvas.CanvasDraw_f) {
            _FchanData.GraphicInfo.Canvas.CanvasDraw_f(&_FchanData.GraphicInfo.Canvas);
        }
        _ApplibGraph_Buffer_Sync(&_FchanData);
        if (_ApplibGraph_Buffer_Switch(&_FchanData) == RETURN_FAIL) {
            return AMP_ERROR_GENERAL_ERROR;
        }
    }
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_BLEND) && (_BlendData.PathActived == BOOLEAN_TRUE) && (_BlendData.GraphicInfo.EnableDraw == BOOLEAN_TRUE)) {
        if (_BlendData.GraphicInfo.Canvas.CanvasDraw_f) {
            _BlendData.GraphicInfo.Canvas.CanvasDraw_f(&_BlendData.GraphicInfo.Canvas);
        }
    }
    return 0;
}

static void *BlendBufAddr = NULL;
static void *rawBlendBuf = NULL;
static UINT8 stampAreaId = 0;

/**
 *  @brief Init stamp
 *
 *  Init stamp
 *
 *  @return
 *  @see
 */
void AppLibGraph_InitStamp(void)
{
    if (_BlendData.PathActived == BOOLEAN_FALSE) {
        return;
    }
#ifdef CONFIG_APP_ARD
    UINT32 stampBufSize = _BlendData.GraphicInfo.WorkingBuffer.Width * _BlendData.GraphicInfo.WorkingBuffer.Height*4;
#else
	UINT32 stampBufSize = _BlendData.GraphicInfo.WorkingBuffer.Width * _BlendData.GraphicInfo.WorkingBuffer.Height;
#endif
	if (_AllocateMemory((void**)&BlendBufAddr, &rawBlendBuf, stampBufSize) < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[AppLib - Graphics] <InitStamp> Allocate memory fail");
        rawBlendBuf = NULL;
        return;
    }

    /* Init stamp module */
    memset(BlendBufAddr, 0x0, stampBufSize);
    AppLibStamp_Init(BlendBufAddr);
}

/**
 *  @brief Reset stamp
 *
 *  Reset stamp
 *
 *  @return
 *  @see
 */
void AppLibGraph_ResetStamp(void)
{
    UINT32 stampBufSize = 0;
    if (rawBlendBuf == NULL) {
        return;
    }

#ifdef CONFIG_APP_ARD
    /* Init stamp module */
    stampBufSize = _BlendData.GraphicInfo.WorkingBuffer.Width * _BlendData.GraphicInfo.WorkingBuffer.Height*4;
#else
	stampBufSize = _BlendData.GraphicInfo.WorkingBuffer.Width * _BlendData.GraphicInfo.WorkingBuffer.Height;
#endif
	memset(BlendBufAddr, 0x0, stampBufSize);
    AppLibStamp_Init(BlendBufAddr);
    stampAreaId = 0;
}

/**
 *  @brief Add a new stamp area
 *
 *  Add a new stamp area
 *
 *  @param[in] stampArea stamp area config
 *  @param[in] encodeFormat the encode format
 *
 *  @return
 *  @see
 */
UINT8 AppLibGraph_AddStampArea(AMP_AREA_s stampArea, APPLIB_GRAPH_ENCODE_FORMAT_e encodeFormat)
{
    APPLIB_GRAPH_COLOR_FORMAT_e colorFormat = 0;
    APPLIB_GRAPHIC_SOURCE_BUF_INFO_s sourceBufInfo = {0};
    if (_BlendData.PathActived == BOOLEAN_FALSE) {
		GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[AppLib - Graphics] <AppLibGraph_AddStampArea> fail");// to be continued
        return;
    }

    sourceBufInfo.SourceRender = _BlendData.GraphicInfo.Canvas.Render;
    sourceBufInfo.SourceDisplayBox.X = stampArea.X;
    sourceBufInfo.SourceDisplayBox.Y = stampArea.Y;
    sourceBufInfo.SourceDisplayBox.Width = stampArea.Width;
    sourceBufInfo.SourceDisplayBox.Height = stampArea.Height;

    /* Convert color format */
    switch (encodeFormat) {
        case ENCODE_FORMAT_YUV420:
            colorFormat = COLOR_FORMAT_YUV420;
            break;
        case ENCODE_FORMAT_YUV422:
        default:
            colorFormat = COLOR_FORMAT_YUV422;
            break;
    }

    /* Add one data to blending buffer */
    AppLibBlend_AddBlendArea(stampAreaId, &sourceBufInfo, colorFormat);
    stampAreaId++;
    return (stampAreaId-1);
}

/**
 *  @brief Update a specific stamp area
 *
 *  Update a specific stamp area
 *
 *  @param[in] stampAreaId stamp area Id
 *  @param[in] stampArea stamp area config
 *  @param[in] encodeFormat the encode format
 *
 *  @return
 */
void AppLibGraph_UpdateStampArea(UINT8 stampAreaId, AMP_AREA_s stampArea, APPLIB_GRAPH_ENCODE_FORMAT_e encodeFormat)
{
    APPLIB_GRAPH_COLOR_FORMAT_e colorFormat = 0;
    APPLIB_GRAPHIC_SOURCE_BUF_INFO_s sourceBufInfo = {0};
    if (_BlendData.PathActived == BOOLEAN_FALSE) {
        return;
    }
    sourceBufInfo.SourceRender = _BlendData.GraphicInfo.Canvas.Render;
    sourceBufInfo.SourceDisplayBox.X = stampArea.X;
    sourceBufInfo.SourceDisplayBox.Y = stampArea.Y;
    sourceBufInfo.SourceDisplayBox.Width = stampArea.Width;
    sourceBufInfo.SourceDisplayBox.Height = stampArea.Height;

    /* Convert color format */
    switch(encodeFormat) {
        case ENCODE_FORMAT_YUV420:
            colorFormat = COLOR_FORMAT_YUV420;
            break;
        case ENCODE_FORMAT_YUV422:
        default:
            colorFormat = COLOR_FORMAT_YUV422;
            break;
    }

    /* Add one data to blending buffer */
    AppLibBlend_UpdateBlendArea(stampAreaId, &sourceBufInfo, colorFormat);
}

/**
 *  Save the graphic as BMP file
 *
 *  @param[in] graphChannelId the specific channel
 *
 *  @return
 *  @see
 */
void AppLibGraph_SaveAsBMP(UINT32 graphChannelId)
{
    AMBA_FS_FILE *File = NULL;
    AMBA_FS_STAT Fstat;
    char FT[64] = {'\0'};
    APPLIB_GRAPHIC_RENDER_s *render = NULL;
    UINT32 FileSize = 0, bufWidth = 0;
    UINT8 *TmpbufRaw = NULL;
    static UINT8 *Tmpbuf = NULL;

    if (graphChannelId == GRAPH_CH_DCHAN) {
        sprintf(FT, "C:\\OSD_BMP_DCHAN.bmp");
        render = _DchanData.GraphicInfo.Canvas.Render;
    } else if (graphChannelId == GRAPH_CH_FCHAN) {
        sprintf(FT, "C:\\OSD_BMP_FCHAN.bmp");
        render = _FchanData.GraphicInfo.Canvas.Render;
    } else if (graphChannelId == GRAPH_CH_BLEND) {
        sprintf(FT, "C:\\OSD_BMP_BLEND.bmp");
        render = _BlendData.GraphicInfo.Canvas.Render;
    } else {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "Please choose channel id! 1: FChan, 2: DChan, 4: Blend");
        return;
    }

    {
        FileSize = render->BufHeight * render->BufPitch;
        bufWidth = render->BufPitch / render->BufPixelSize;

#ifdef CONFIG_APP_ARD
        if(1 == render->BufPixelSize) {
            FileSize = render->BufHeight * bufWidth * 3;
        }
#endif

        AmbaFS_Stat((const char *)FT, &Fstat);
        if (_AllocateMemory((void **)&Tmpbuf, (void **)&TmpbufRaw, FileSize) < 0) {
            GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[AppLib - Graphics] <SaveBMP> Allocate memory fail");
            return;
        }
    }
    {
        WCHAR mode[3] = {'w','b','\0'};
        File = AmbaFS_fopen((char const *)FT,(char const *) mode);
    }
    {
        // write the identifier to bitmap header.
        UINT8 idBuf[2] = {0x42, 0x4D};  // B, M
        // write bmp header
        int bmp_data_size = FileSize;
        int bmp_file_size = 2 + sizeof(APPLIB_GRAPH_BMPFILE_HEADER_t) + bmp_data_size;
        int bmp_img_offset = 2 + sizeof(APPLIB_GRAPH_BMPFILE_HEADER_t);
        int bmp_bpp = 24;
        int bmp_cmp = 0;
        APPLIB_GRAPH_BMPFILE_HEADER_t bmp_header;

        AmbaFS_fwrite(idBuf, sizeof(UINT8), 2, File);
        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_GRAPHICS, "BMP ID done");
        //
        memset(&bmp_header, 0, sizeof(APPLIB_GRAPH_BMPFILE_HEADER_t));
        bmp_header.file_size        = bmp_file_size; // Bitmap File Header: Bitmap file size
        bmp_header.reserved            = 0;         // Bitmap File Header: Reserved, always 0
        bmp_header.image_offset        = bmp_img_offset;// Bitmap File Header: Offset bytes of the data start
        bmp_header.header_size        = 40;         // Bitmap Info Header: Length of Bitmap Info Header
        bmp_header.xres                = bufWidth;     // Bitmap Info Header: Width of bitmap (pixels)
        bmp_header.yres                = render->BufHeight;     // Bitmap Info Header: Height of bitmap (pixels)
        bmp_header.num_of_planes    = 1;         // Bitmap Info Header: Planes, always 1
        bmp_header.bits_per_pix        = bmp_bpp;     // Bitmap Info Header: Bits per pixel, 8 bits(use 256 color palette)
        bmp_header.compression        = bmp_cmp;     // Bitmap Info Header: Compression, 0 is non-compressed
        bmp_header.bit_map_size        = bmp_data_size; // Bitmap Info Header: Bitmap data size
        bmp_header.hor_res            = 0;         // Bitmap Info Header: Horizontal resolution(pixels/meter), no need
        bmp_header.vert_res            = 0;         // Bitmap Info Header: Vertical resolution(pixels/meter), no need
        bmp_header.num_of_colors    = 0;         // Bitmap Info Header: Used palette colors, 0 means use all colors in palette
        bmp_header.num_sig_colors    = 0;         // Bitmap Info Header: Important colors, 0 means all colors are the same
        AmbaFS_fwrite(&bmp_header, sizeof(APPLIB_GRAPH_BMPFILE_HEADER_t), 1, File);
    }
    {
        // write bmp content
        int i = 0, j = 0;
        void *get;
        UINT32 val;

        GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_GRAPHICS, "BufH %d, BufW %d", render->BufHeight, bufWidth);
        for (j=render->BufHeight; j>0; j--) {
            render->MoveTo_f(render, &get, 0, j);
            for (i=0; i<bufWidth; i++) {
                render->GetPixel_f(get, &val);
                render->MoveNext_f(&get);
                Tmpbuf[i*3+0] = (val & 0x0000FF);       // B
                Tmpbuf[i*3+1] = (val & 0x00FF00) >> 8;  // G
                Tmpbuf[i*3+2] = (val & 0xFF0000) >> 16; // R
#ifdef CONFIG_APP_ARD
                if(1 == render->BufPixelSize) {
                    Tmpbuf[i*3+0] = clut[val*3+2]; // B
                    Tmpbuf[i*3+1] = clut[val*3+1]; // G
                    Tmpbuf[i*3+2] = clut[val*3];   // R
                }
#endif
            }
              AmbaFS_fwrite(Tmpbuf, sizeof(UINT8)*3, bufWidth, File);
        }
    }
    //
    AmbaFS_FSync(File);
    AmbaFS_fclose(File);
    AmbaKAL_BytePoolFree(TmpbufRaw);
}

static void _AppLibGraph_GetDirectDrawBuf(UINT32 graphChannelId, APPLIB_GRAPHIC_DIRECTDRAW_CNT_s *ddCnt)
{
    switch (graphChannelId) {
        case GRAPH_CH_FCHAN:
            {
                int FchanPixelSize = _CheckPixelSize(_FchanData.OsdInfo.PixelFormat);

                ddCnt->Width = _FchanData.GraphicInfo.DispalyBuffer.Width;
                ddCnt->Height = _FchanData.GraphicInfo.DispalyBuffer.Height;
                ddCnt->Pitch = _FchanData.GraphicInfo.DispalyBuffer.Width * FchanPixelSize;
                ddCnt->Ptr = (UINT32)_FchanData.GraphicInfo.DispalyBuffer.Address[_FchanData.GraphicInfo.BufferIdx];
            }
            break;
        case GRAPH_CH_DCHAN:
            {
                int DchanPixelSize = _CheckPixelSize(_DchanData.OsdInfo.PixelFormat);

                ddCnt->Width = _DchanData.GraphicInfo.DispalyBuffer.Width;
                ddCnt->Height = _DchanData.GraphicInfo.DispalyBuffer.Height;
                ddCnt->Pitch = _DchanData.GraphicInfo.DispalyBuffer.Width * DchanPixelSize;
                ddCnt->Ptr = (UINT32)_DchanData.GraphicInfo.DispalyBuffer.Address[_DchanData.GraphicInfo.BufferIdx];
            }
            break;
    }
}

static void _AppLibGraph_SwitchDirectDrawBuffer(GRAPH_CHAN_s *graphChn)
{
    UINT8 currIdx = graphChn->GraphicInfo.BufferIdx;
    UINT8 nextIdx = (currIdx + 1) % 2;
    UINT8 pixelSize = _CheckPixelSize(graphChn->OsdInfo.PixelFormat);
    UINT32 bufferSize = graphChn->GraphicInfo.DispalyBuffer.Width * graphChn->GraphicInfo.DispalyBuffer.Height * pixelSize;
    AMP_OSD_BUFFER_CFG_s buffCfg = {0};

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_GRAPHICS, "[Applib - Graph] <_AppLibGraph_SwitchDirectDrawBuffer> graphChn->GraphicInfo.BufferIdx %d",graphChn->GraphicInfo.BufferIdx);
    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_GRAPHICS, "[Applib - Graph] <_AppLibGraph_SwitchDirectDrawBuffer> nextIdx %d",nextIdx);

    /* clean cache buffer */
    AmbaCache_Clean((void *)graphChn->GraphicInfo.DispalyBuffer.Address[currIdx], bufferSize);

    /* assign buffer */
    buffCfg.BufAddr = (UINT8*)graphChn->GraphicInfo.DispalyBuffer.Address[currIdx];
    buffCfg.BufWidth = graphChn->GraphicInfo.DispalyBuffer.Width;
    buffCfg.BufHeight = graphChn->GraphicInfo.DispalyBuffer.Height;
    buffCfg.BufPitch = graphChn->GraphicInfo.DispalyBuffer.Width * _CheckPixelSize(graphChn->OsdInfo.PixelFormat);
    buffCfg.PixelFormat = graphChn->OsdInfo.PixelFormat;
    if (AmpOsd_SetBufferCfg(graphChn->OsdInfo.Hdlr, &buffCfg) < 0) {
        GraphicsPrint(DEBUG_ERR, DEBUG_MODULE_GRAPHICS, "[Graphics] <_AppLibGraph_SwitchDirectDrawBuffer> AmpOsd_SetBufferCfg fail");
        return;
    }

    /* update buffer index */
    graphChn->GraphicInfo.BufferIdx = nextIdx;

    /* memory sync */
    //memcpy(graphChn->GraphicInfo.DispalyBuffer.Address[nextIdx], graphChn->GraphicInfo.DispalyBuffer.Address[currIdx], bufferSize);
}

static void _AppLibGraph_DirectDrawFlush(UINT32 graphChannelId, APPLIB_GRAPHIC_DIRECTDRAW_CNT_s *ddCnt)
{
    static UINT8 showFlg = 0;

    GraphicsPrint(DEBUG_ONLY, DEBUG_MODULE_GRAPHICS, "[Applib - Graph] <_AppLibGraph_DirectDrawFlush> showFlg %d ddCnt->ShowFlg %d",showFlg, ddCnt->ShowFlg);
    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_DCHAN) && (_DchanData.PathActived == BOOLEAN_TRUE)) {
        if (ddCnt->ShowFlg == 1) {
            _AppLibGraph_SwitchDirectDrawBuffer(&_DchanData);
        } else if (showFlg != ddCnt->ShowFlg){
            memset(_DchanData.GraphicInfo.DispalyBuffer.Address[0], 0x0, (ddCnt->Pitch * ddCnt->Height));
            memset(_DchanData.GraphicInfo.DispalyBuffer.Address[1], 0x0, (ddCnt->Pitch * ddCnt->Height));
        }
    }

    if (APPLIB_CHECKFLAGS(graphChannelId, GRAPH_CH_FCHAN) && (_FchanData.PathActived == BOOLEAN_TRUE)) {
        if (ddCnt->ShowFlg == 1) {
            _AppLibGraph_SwitchDirectDrawBuffer(&_FchanData);
        } else if (showFlg != ddCnt->ShowFlg){
            memset(_FchanData.GraphicInfo.DispalyBuffer.Address[0], 0x0, (ddCnt->Pitch * ddCnt->Height));
            memset(_FchanData.GraphicInfo.DispalyBuffer.Address[1], 0x0, (ddCnt->Pitch * ddCnt->Height));
        }
    }
    showFlg = ddCnt->ShowFlg;
}

void AppLibGraph_DirectDrawAct(APPLIB_GRAPHIC_DIRECTDRAW_CNT_s *ddCnt)
{
    switch (ddCnt->Act) {
        case APPLIB_DIRECTDRAW_ACTION_GET_BUFFER:
            _AppLibGraph_GetDirectDrawBuf(ddCnt->VoutChn, ddCnt);
            break;
        case APPLIB_DIRECTDRAW_ACTION_SHOW_ON_SCREEN:
            _AppLibGraph_DirectDrawFlush(ddCnt->VoutChn, ddCnt);
            break;
    }
}
#ifdef CONFIG_APP_ARD
GRAPH_CHAN_s* AppLibGraph_GetChannelData(GRAPHICS_CHAN_e canvasChn) {
    switch(canvasChn) {
        case GRAPHICS_CHANNEL_D:
            return &_DchanData;
            break;
        case GRAPHICS_CHANNEL_F:
            return &_FchanData;
            break;
        case GRAPHICS_CHANNEL_BLEND:
            return &_BlendData;
            break;
        default:
            return 0;
            break;
    }
}
int AppLibGraph_GetFontType(void) {
    return _InitConfig.Font.FontType;
}
#endif

