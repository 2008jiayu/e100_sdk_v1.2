 /**
  * @file src/app/framework/appmaintask.c
  *
  * DemoApp main task entry for testing.
  *
  * History:
  *    2013/08/20 - [Martin Lai] created file
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

#include "appmaintask.h"

#include <AmbaUART_Def.h>
#include <AmbaUART.h>
#include <AmbaGPIO.h>

#include <AmbaPrintk.h>

#include <AmbaShell.h>
#include <AmbaCardManager.h>
#include <AmbaTest.h>
#include <AmbaFS.h>
#include <AmbaAudio.h>
#include <AmbaPLL.h>
#include <applib.h>
#include <AmbaNAND_Def.h>
#include <AmbaNFTL.h>
#include <bsp.h>
#include <AmbaFIO.h>
#include <AmbaSD.h>
#include <AmbaNAND.h>
#include <AmbaROM.h>
#include <imgschdlr/scheduler.h>
#include <mw.h>
#include <fifo/Fifo.h>
#include <cfs/AmpCfs.h>
#include <dcf/AmpDcf.h>
#include <AmbaRTC.h>
#include <AmbaGDMA.h>
#include <AmbaSensor.h>
#include <AmbaCalibInfo.h>
#include <AmbaLCD.h>
#include <AmbaIMU.h>
#include <AmbaAudio_CODEC.h>
#include <AmbaVer.h>  // AmbaVer_GetVerInfo
#include <AmbaADC.h>

#ifdef CONFIG_APP_AMBA_LINK
#include <AmbaLink.h>
#endif /* CONFIG_APP_AMBA_LINK */

#ifdef CONFIG_CC_CXX_SUPPORT
#include "AmbaCtorDtor.hpp"
#endif

#ifndef CONFIG_ENABLE_EMMC_BOOT
#include <AmbaFS_PrFile.h>
#include <AmbaFS_Nand.h>
#endif

#ifdef CONFIG_APP_ARD
#include "AmbaI2C.h"
#include <AmbaGSensor.h>
#include <system/app_pref.h>
#endif

#define APPMAINTASK_STACK_SIZE        0x10000
#define APPMAINTASK_NAME              "App_boot_Manager"

typedef struct _APPMAINTASK_s_ {
    UINT8 Stack[APPMAINTASK_STACK_SIZE];                         /**< Stack */
    AMBA_KAL_TASK_t Task;                                       /**< Task ID */
} APPMAINTASK_s;

/**
 * @brief Global instance of host boot manager
 */
static APPMAINTASK_s AppMainTask = {0};


AMBA_KAL_BYTE_POOL_t G_MMPL;
AMBA_KAL_BYTE_POOL_t G_NC_MMPL;
AMBA_KAL_BYTE_POOL_t  AmbaBytePool_Cached;

UINT8 *DspWorkAreaResvStart;
UINT8 *DspWorkAreaResvLimit;
UINT32 DspWorkAreaResvSize;

#ifdef CONFIG_ENABLE_EMMC_BOOT
extern void AmbaEMMC_Init(void);
#endif

/**
 *  @brief Get the maximum size of DSP
 *
 *  Get the maximum size of DSP
 *
 *  @return >=0 success, <0 failure
 */
static UINT32 Main_GetProjectDspMaxSize(void)
{
#define DSP_MAX_USAGE (CONFIG_APP_DSP_WORKBUFFER_SIZE<<20)
    return DSP_MAX_USAGE;
}

#define DspDebugLogDataSize   (0x20000)
static UINT8 *DspDebugLogDataBuf = NULL;
static UINT8 *DspDebugLogDataBufRaw = NULL;



/**
 *  @brief Config the memory allocation.
 *
 *  Config the memory allocation
 *
 *  @return >=0 success, <0 failure
 */
static UINT32 Main_MemInit(void)
{
    UINT8* CacheDdrStart;
    UINT8* CacheDdrLimit;
    UINT8* NonCacheDdrStart;
    UINT8* NonCacheDdrLimit;

    extern void *__cache_heap_start;
    extern void *__cache_heap_end;
    extern void *__non_cache_heap_start;
    extern void *__non_cache_heap_end;

    CacheDdrStart = (UINT8*) &__cache_heap_start;
#if 0 //CONFIG_APP_CONNECTED_AMBA_LINK
    do {
        extern void *__linux_start;
        extern void *__linux_end;
        UINT32 cdl_tmp;

        cdl_tmp = ((UINT32) &__cache_heap_end) - (((UINT32) &__linux_end) - ((UINT32) &__linux_start) -1);
        CacheDdrLimit = (UINT8*) cdl_tmp;
    } while (0);
#else
    CacheDdrLimit = (UINT8*) &__cache_heap_end - 1;
#endif /* CONFIG_APP_CONNECTED_AMBA_LINK */
    NonCacheDdrStart = (UINT8*) &__non_cache_heap_start;
    NonCacheDdrLimit = (UINT8*) &__non_cache_heap_end-1;

    /* Alloc mempool */
#define DSP_WORK_BUF_CACHED
#ifdef  DSP_WORK_BUF_CACHED
    // DSP Working Buffer is cached
    DspWorkAreaResvStart = (UINT8*) ALIGN_64((UINT32)CacheDdrStart);
    DspWorkAreaResvLimit = DspWorkAreaResvStart + Main_GetProjectDspMaxSize() - 1;
    DspWorkAreaResvSize = Main_GetProjectDspMaxSize();
    AmbaKAL_BytePoolCreate(&G_MMPL, DspWorkAreaResvLimit + 1, (UINT32)CacheDdrLimit - (UINT32)DspWorkAreaResvLimit);
    AmbaKAL_BytePoolCreate(&G_NC_MMPL, NonCacheDdrStart, NonCacheDdrLimit - NonCacheDdrStart + 1);
#else
    // DSP Working Buffer is non-cached
    AmbaKAL_BytePoolCreate(&G_MMPL, CacheDdrStart, CacheDdrLimit - CacheDdrStart + 1);
    DspWorkAreaResvStart = (UINT8*) ALIGN_64((UINT32)NonCacheDdrStart);
    DspWorkAreaResvLimit = DspWorkAreaResvStart + Main_GetProjectDspMaxSize() - 1;
    DspWorkAreaResvSize = Main_GetProjectDspMaxSize();
    AmbaKAL_BytePoolCreate(&G_NC_MMPL, DspWorkAreaResvLimit + 1, (UINT32)NonCacheDdrLimit - (UINT32)DspWorkAreaResvLimit);
#endif

#ifdef CONFIG_CC_CXX_SUPPORT
    AmbaCtorDtor_Init(&G_MMPL);
#endif

    if (AmpUtil_GetAlignedPool(&G_NC_MMPL, (void**)&DspDebugLogDataBuf, (void**)&DspDebugLogDataBufRaw, DspDebugLogDataSize, 64) != OK) {
        AmbaPrintColor(RED,"[AppMainTask] <MemInit> NonCacheDDR alloc failed!!!!!!!!!");
        return -1;
    }
    return 0;
}

/**
 *  @brief Print the information of Memory
 *
 *  Print the information of Memory
 *
 *  @return >=0 success, <0 failure
 */
static UINT32 Main_MemPrint(void)
{
    AmbaPrint("===========================================");
    AmbaPrint("    CACHE : [0x%08x - 0x%08x], %3d MB", G_MMPL.tx_byte_pool_start, (G_MMPL.tx_byte_pool_start + G_MMPL.tx_byte_pool_size - 1), (G_MMPL.tx_byte_pool_size)>>20);
    AmbaPrint("      DSP : [0x%08x - 0x%08x], %3d MB", (UINT32)DspWorkAreaResvStart, (UINT32)(DspWorkAreaResvLimit), ((UINT32)DspWorkAreaResvSize)>>20);
    AmbaPrint("NON-CACHE : [0x%08x - 0x%08x], %3d MB", G_NC_MMPL.tx_byte_pool_start, (G_NC_MMPL.tx_byte_pool_start + G_NC_MMPL.tx_byte_pool_size
- 1), (G_NC_MMPL.tx_byte_pool_size)>>20);
    //AmbaPrint("   G_MMPL.id: %ld", G_MMPL.tx_byte_pool_id);
    //AmbaPrint("G_NC_MMPL.id: %ld", G_NC_MMPL.tx_byte_pool_id);
    #ifdef CONFIG_APP_AMBA_LINK
    {
        extern void *__linux_start;
        extern void *__linux_end;
        UINT32 linuxSize;

        linuxSize = ((UINT32) &__linux_end -(UINT32) &__linux_start);
        AmbaPrint("    LINUX : [0x%08x - 0x%08x], %3d MB", (UINT32)&__linux_start, ((UINT32)&__linux_end)-1,
                                                            linuxSize >> 20);
    }
    #endif
    AmbaPrint("===========================================");
    return 0;
}


int AppMainTask_HookSensor(void)
{
    int ReturnValue = -1;

    AppEncChannel.Data = 0;
    AppEncChannel.Bits.VinID = 0;

#ifdef CONFIG_SENSOR_SONY_IMX117
    {
        extern AMBA_SENSOR_OBJ_s AmbaSensor_IMX117Obj;
        extern AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_IMX117Obj;
        pAmbaSensorObj[AMBA_VIN_CHANNEL0] = &AmbaSensor_IMX117Obj;
        AppEncChannel.Bits.SensorID = (1<<0);
        AmbaSensor_Hook(AppEncChannel, &AmbaSensor_IMX117Obj);
        AmbaCalibInfo_Hook(AppEncChannel, &AmbaCalibInfo_IMX117Obj);
        ReturnValue = 0;
    }
#endif

#ifdef CONFIG_SENSOR_SONY_IMX206
    {
        extern AMBA_SENSOR_OBJ_s AmbaSensor_IMX206Obj;
        extern AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_IMX206Obj;
        pAmbaSensorObj[AMBA_VIN_CHANNEL0] = &AmbaSensor_IMX206Obj;
        AppEncChannel.Bits.SensorID = (1<<0);
        AmbaSensor_Hook(AppEncChannel, &AmbaSensor_IMX206Obj);
        AmbaCalibInfo_Hook(AppEncChannel, &AmbaCalibInfo_IMX206Obj);
        ReturnValue = 0;
    }
#endif

#ifdef CONFIG_SENSOR_OV4689
    {
        extern AMBA_SENSOR_OBJ_s AmbaSensor_OV4689Obj;
        extern AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_OV4689Obj;
        pAmbaSensorObj[AMBA_VIN_CHANNEL0] = &AmbaSensor_OV4689Obj;
        AppEncChannel.Bits.SensorID = (1<<0);
        AmbaSensor_Hook(AppEncChannel, &AmbaSensor_OV4689Obj);
        AmbaCalibInfo_Hook(AppEncChannel, &AmbaCalibInfo_OV4689Obj);
        ReturnValue = 0;
    }
#endif

#ifdef CONFIG_SENSOR_AR0330_PARALLEL
    {
        extern AMBA_SENSOR_OBJ_s AmbaSensor_AR0330_PARALLELObj;
        extern AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_AR0330_PARALLELObj;
        pAmbaSensorObj[AMBA_VIN_CHANNEL0] = &AmbaSensor_AR0330_PARALLELObj;
        AppEncChannel.Bits.SensorID = (1<<0);
        AmbaSensor_Hook(AppEncChannel, &AmbaSensor_AR0330_PARALLELObj);
        AmbaCalibInfo_Hook(AppEncChannel, &AmbaCalibInfo_AR0330_PARALLELObj);
        ReturnValue = 0;
    }
#endif

#ifdef CONFIG_SENSOR_AR0230
    {
        extern AMBA_SENSOR_OBJ_s AmbaSensor_AR0230Obj;
        extern AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_AR0230Obj;
        pAmbaSensorObj[AMBA_VIN_CHANNEL0] = &AmbaSensor_AR0230Obj;
        AppEncChannel.Bits.SensorID = (1<<0);
        AmbaSensor_Hook(AppEncChannel, &AmbaSensor_AR0230Obj);
        AmbaCalibInfo_Hook(AppEncChannel, &AmbaCalibInfo_AR0230Obj);
        ReturnValue = 0;
    }
#endif

#ifdef CONFIG_SENSOR_MN34120
    {
        extern AMBA_SENSOR_OBJ_s AmbaSensor_MN34120Obj;
        extern AMBA_CALIB_INFO_OBJ_s AmbaCalibInfo_MN34120Obj;
        pAmbaSensorObj[AMBA_VIN_CHANNEL0] = &AmbaSensor_MN34120Obj;
        AppEncChannel.Bits.SensorID = (1<<0);
        AmbaSensor_Hook(AppEncChannel, &AmbaSensor_MN34120Obj);
        AmbaCalibInfo_Hook(AppEncChannel, &AmbaCalibInfo_MN34120Obj);
        ReturnValue = 0;
    }
#endif

    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[appmaintask] <HookSensor> Do not register any sensor.");
    }
    ReturnValue = AmbaSensor_Init(AppEncChannel);
    return ReturnValue;
}

int AppMainTask_HookLcd(void)
{
    int ReturnValue = -1;

#ifdef CONFIG_LCD_T20P52
    {
        extern AMBA_LCD_OBJECT_s AmbaLCD_T20P52Obj;
        pAmbaLcdObj[0] = &AmbaLCD_T20P52Obj;
        ReturnValue = 0;
    }
#endif

#ifdef CONFIG_LCD_T27P05
    {
        extern AMBA_LCD_OBJECT_s AmbaLCD_T27P05Obj;
        pAmbaLcdObj[0] = &AmbaLCD_T27P05Obj;
        ReturnValue = 0;
    }
#endif


#ifdef CONFIG_LCD_WDF9648W
    {
        extern AMBA_LCD_OBJECT_s AmbaLCD_WdF9648wObj;
        pAmbaLcdObj[0] = &AmbaLCD_WdF9648wObj;
        ReturnValue = 0;
    }
#endif

#ifdef CONFIG_LCD_SSD2828
    {
        extern AMBA_LCD_OBJECT_s AmbaLCD_SSD2828Obj;
        pAmbaLcdObj[0] = &AmbaLCD_SSD2828Obj;
        ReturnValue = 0;
    }
#endif

    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[appmaintask] <HookLcd> Do not register any LCD.");
    }

    return ReturnValue;
}

int AppMainTask_HookImu(void)
{
    int ReturnValue = -1;

#ifdef CONFIG_IMU_INVENSENSE_MPU6500
    {
        extern AMBA_IMU_OBJ_s AmbaIMU_MPU6500Obj;
        pAmbaImuObj[AMBA_IMU_SENSOR0] = &AmbaIMU_MPU6500Obj;
        ReturnValue = 0;
    }
#endif

#ifdef CONFIG_IMU_INVENSENSE_ICM20608
    {
        extern AMBA_IMU_OBJ_s AmbaIMU_ICM20608Obj;
        pAmbaImuObj[AMBA_IMU_SENSOR0] = &AmbaIMU_ICM20608Obj;
        ReturnValue = 0;
    }
#endif

#ifdef CONFIG_IMU_AMBARELLA_DMY0000
    {
        extern AMBA_IMU_OBJ_s AmbaIMU_DMY0000Obj;
        pAmbaImuObj[AMBA_IMU_SENSOR0] = &AmbaIMU_DMY0000Obj;
        ReturnValue = 0;
    }
#endif

    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[appmaintask] <HookImu> Do not register any IMU.");
    }

    return ReturnValue;
}

int AppMainTask_HookAudio(void)
{
    int ReturnValue = -1;

#ifdef CONFIG_AUDIO_TITLV320AIC3256
    {
        extern AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_Tlv320Aic3256Obj;
        pAmbaAudioCodecObj[AMBA_AUDIO_CODEC_0] = &AmbaAudio_Tlv320Aic3256Obj;
        ReturnValue = 0;
    }
#endif

#ifdef CONFIG_AUDIO_WM8974
    {
        extern AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_WM8974Obj;
        pAmbaAudioCodecObj[AMBA_AUDIO_CODEC_0] = &AmbaAudio_WM8974Obj;
        ReturnValue = 0;
    }
#endif

#ifdef CONFIG_AUDIO_AK4951EN
        {
            extern AMBA_AUDIO_CODEC_OBJ_s AmbaAudio_AK4951Obj;
            pAmbaAudioCodecObj[AMBA_AUDIO_CODEC_0] = &AmbaAudio_AK4951Obj;
            ReturnValue = 0;
        }
#endif

    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[appmaintask] <HookAudio> Do not register any audio.");
    }

    return ReturnValue;
}

#ifdef CONFIG_APP_ARD
int AppMainTask_HookGSensor(void)
{
    int ReturnValue = -1;

#ifdef CONFIG_G_SENSOR_ST_LIS3DE
    {
        extern AMBA_G_SENSOR_OBJ_s AmbaGSensor_LIS3DEObj;
        //pAmbaGSensorObj[AMBA_G_SENSOR_0] = &AmbaGSensor_LIS3DEObj;
        AmbaGSensor_Hook(AMBA_G_SENSOR_0, &AmbaGSensor_LIS3DEObj);
#ifdef CONFIG_BSP_ARIES
        if (AmbaUser_GetVersion() != AMBA_BSP_ARIES_VERSION_V00) {
            AMBA_G_SENSOR_INFO_s Info = {0};
            AmbaGSensor_GetInfo(AMBA_G_SENSOR_0, &Info);
            Info.I2CCHN = AMBA_I2C_CHANNEL2;
            AmbaGSensor_SetInfo(AMBA_G_SENSOR_0, &Info);
        }
#endif
#ifdef CONFIG_BSP_ORTHRUS
        {
            AMBA_G_SENSOR_INFO_s Info = {0};
            AmbaGSensor_GetInfo(AMBA_G_SENSOR_0, &Info);
            Info.I2CCHN = AMBA_I2C_CHANNEL2;
            AmbaGSensor_SetInfo(AMBA_G_SENSOR_0, &Info);
        }
#endif
#ifdef CONFIG_BSP_GOAT
        {
            AMBA_G_SENSOR_INFO_s Info = {0};
            AmbaGSensor_GetInfo(AMBA_G_SENSOR_0, &Info);
            Info.I2CCHN = AMBA_I2C_CHANNEL0;
            AmbaGSensor_SetInfo(AMBA_G_SENSOR_0, &Info);
        }
#endif
        ReturnValue = 0;
    }
#endif

#ifdef CONFIG_G_SENSOR_MIRA_DA380
        {
            extern AMBA_G_SENSOR_OBJ_s AmbaGSensor_DA380Obj;
            AmbaGSensor_Hook(AMBA_G_SENSOR_0, &AmbaGSensor_DA380Obj);
            ReturnValue = 0;
        }
#endif


    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[appmaintask] <HookGsensor> Do not register any g-sensor.");
    }

    return ReturnValue;
}
#endif

/**
 *  @brief Initialize the FIFO
 *
 *  Initialize the FIFO
 *
 *  @return >=0 success, <0 failure
 */
static int Main_FifoInit(void)
{
    static void* FifoBuf = NULL, *FifoBufRaw = NULL;
    int ReturnValue = 0;
#define FIFO_BUF_SIZE   (5<<20)
#define FIFO_STACK_SIZE   (0x8000)
    AMP_FIFO_INIT_CFG_s FifoCfg;

    AmpFifo_GetInitDefaultCfg(&FifoCfg);
    //AmbaPrint("[AppMainTask] AmpFifo_GetInitDefaultCfg %d", FifoCfg.NumMaxCodec);
    // alloc memory;
    if (AmpUtil_GetAlignedPool(&G_MMPL, &FifoBuf, &FifoBufRaw, FIFO_BUF_SIZE, 32) != AMP_OK) {
        AmbaPrintColor(RED, "[appmaintask] Main_FifoInit get memory fail.");
        return -1;
    }
    FifoCfg.MemoryPoolAddr = (UINT8*)FifoBuf;
    FifoCfg.MemoryPoolSize = FIFO_BUF_SIZE;
    FifoCfg.TaskInfo.StackSize = FIFO_STACK_SIZE;
    FifoCfg.TaskInfo.Priority = APP_FIFO_TASK_PRIORITY;
    FifoCfg.NumMaxCodec = 12;
    FifoCfg.NumMaxFifo = 18;

    ReturnValue = AmpFifo_Init(&FifoCfg);
    if (ReturnValue != AMP_OK) {
        AmbaPrintColor(RED, "[appmaintask] Main_FifoInit fail.");
    }

    return ReturnValue;
}

/**
 *  @brief Init Uart
 *
 *  Init Uart
 *
 *  @return >=0 success, <0 failure
 */
static int Main_UartInit(void)
{
    static UINT8 UartWorkingBuf[1024] __attribute__((section (".bss.noinit")));

    /* enable uart */
    AMBA_UART_CONFIG_s UartConfig = {9600, AMBA_UART_DATA_8_BIT, AMBA_UART_PARITY_NONE, AMBA_UART_STOP_1_BIT, AMBA_UART_FLOW_CTRL_NONE, 0, 0};
    UartConfig.MaxRxRingBufSize = 1024;
    UartConfig.pRxRingBuf = UartWorkingBuf;
    AmbaUART_Config(AMBA_UART_CHANNEL0, &UartConfig);

    return 0;
}

/* The buffer is used to queue characters sent through AmbaPrint */
static UINT8 AmbaPrintRingBuf[256 * 1024] = {0};

/**
 *  @brief Init Amba Print function
 *
 *  Init Amba Print function
 *
 *  @return >=0 success, <0 failure
 */
static int Main_PrintkInit(void)
{
    int ReturnValue = 0;
    AMBA_PRINT_CONFIG_s AmbaPrintConfig = {0};

    AmbaPrintConfig.Priority = APP_AMBA_PRINT_PRIORITY;
    AmbaPrintConfig.PrintBufSize = sizeof(AmbaPrintRingBuf);
    AmbaPrintConfig.pPrintBuf = (UINT8 *) & (AmbaPrintRingBuf[0]);
    AmbaPrintConfig.DspDebugLogAreaSize = DspDebugLogDataSize;
    AmbaPrintConfig.pDspDebugLogDataArea = DspDebugLogDataBuf;
    AmbaPrintConfig.PutCharCallBack = AmbaUserConsole_Write;

    ReturnValue = AmbaPrint_Init(&AmbaPrintConfig);

    return ReturnValue;
}

/**
 *  @brief Initialize the MW
 *
 *  Initialize the MW
 *
 *  @return >=0 success, <0 failure
 */
static int Main_MWInit(void)
{
    AMP_MW_INIT_CFG_s MwInitCfg;
    int ReturnValue = 0;
    UINT8* ImgKernelWorkArea = NULL;
    void *ImgKernelWorkAreaBufRaw = NULL;
    do {
        AmpMW_GetDefaultInitCfg(&MwInitCfg);

        //TBD memory allocation
        MwInitCfg.SizeDspWorkArea = Main_GetProjectDspMaxSize();
        MwInitCfg.DspWorkingArea = (UINT8*)DspWorkAreaResvStart;

        MwInitCfg.DspDbgLogDataArea = DspDebugLogDataBuf;
        MwInitCfg.SizeDspDbgLogDataArea = DspDebugLogDataSize;

        // ik setting
        {
            AMBA_DSP_IMG_ARCH_INFO_s ArchInfo = {0};
            AMBA_DSP_IMG_PIPE_INFO_s PipeInfo[2] = {{
                .Pipe = AMBA_DSP_IMG_PIPE_VIDEO,
                .CtxBufNum = 2,
                .CfgBufNum = 10,
            },{
                .Pipe = AMBA_DSP_IMG_PIPE_STILL,
                .CtxBufNum = 1,
                .CfgBufNum = 1,
            }};

            MwInitCfg.NumImgKernelPipe = ArchInfo.PipeNum = 2;
            MwInitCfg.ImgKernelPipeInfo = &PipeInfo[0];
            for (UINT8 t = 0; t < ArchInfo.PipeNum; t++) {
                ArchInfo.pPipeInfo[t] = &PipeInfo[t];
            }
            MwInitCfg.NumImgKernelBufNum = ArchInfo.BufNum = 2; //TBD at least 2 in singel channel
            MwInitCfg.SizeImgKernelWorkArea = AmbaDSP_ImgQueryArchMemSize(&ArchInfo);


            if (MwInitCfg.SizeImgKernelWorkArea > 0) {
                if (AmpUtil_GetAlignedPool(&G_MMPL, (void**)(&ImgKernelWorkArea), &ImgKernelWorkAreaBufRaw, MwInitCfg.SizeImgKernelWorkArea, 32) != AMP_OK) {
                    AmbaPrintColor(RED,"[AppMainTask] <MWInit> CacheDDR alloc failed!!!!!!!!!");
                    return -1;
                }
                MwInitCfg.ImgKernelWorkArea = (UINT8*)ImgKernelWorkArea;
            }
        }
        MwInitCfg.DspCmdTaskPriority = APP_DSP_CMD_PREPARE_TASK_PRIORITY;
        MwInitCfg.DspCommTaskPriority = APP_DSP_ARM_CMD_TASK_PRIORITY;
        MwInitCfg.DspMsgTaskPriority = APP_DSP_MSG_DISPATCH_TASK_PRIORITY;
        MwInitCfg.EnSeamless = 0;
        ReturnValue = AmpMW_Init(&MwInitCfg);
    } while (0);

    return ReturnValue;
}

/**
 *  @brief Initialize the DCF
 *
 *  Initialize the DCF
 *
 *  @return >=0 success, <0 failure
 */
static int Main_DcfInit(void)
{
    APPLIB_DCF_INIT_CFG_s InitCfg;
    void *DcfBufRaw = NULL;
    /**< Get default config */
    AppLibDCF_GetDefaultInitCfg(&InitCfg);
    InitCfg.CfsCfg.TaskInfo.Priority = APP_CFS_SCHDLR_PRIORITY;
    InitCfg.DcfInitCfg.TaskInfo.Priority = APP_DCF_PRIORITY;

    InitCfg.DcfInitCfg.DefTblCfg.MaxFile = 10000 * InitCfg.DcfInitCfg.DefTblCfg.MaxHdlr;
    InitCfg.BufferSize = AppLibDCF_GetRequiredBufSize(&InitCfg.CfsCfg, &InitCfg.DcfInitCfg, InitCfg.IntObjAmount);

    /**< Allocate memory */
    if (AmpUtil_GetAlignedPool(&G_MMPL, (void **) &InitCfg.Buffer, &DcfBufRaw, InitCfg.BufferSize, 32) == OK) {
        /**< Init AppLibDCF */
        if (AppLibDCF_Init(&InitCfg) >= 0) {
            AmbaPrint("[AppMainTask] DCF init ok!");
            return AMP_OK; /**< Every thing is OK! */
        }
        AmbaPrintColor(RED, "[AppMainTask] AppLib DCF init fail!");
    } else AmbaPrintColor(RED, "[AppMainTask] Allocate memory fail");
    return AMP_ERROR_GENERAL_ERROR;
}

#ifndef CONFIG_ENABLE_EMMC_BOOT
/**
 *  @brief Initialize the NTFL
 *
 *  Initialize the NTFL
 *
 *  @return >=0 success, <0 failure
 */
static int Main_NTFLInit(void)
{
    int ReturnValue = 0;
    AmbaNFTL_InitLock(NFTL_ID_IDX);
    AmbaNFTL_InitLock(NFTL_ID_PRF);
    AmbaNFTL_InitLock(NFTL_ID_CAL);
    AmbaNFTL_InitLock(NFTL_ID_STORAGE);
    if (AmbaNFTL_Init(NFTL_ID_IDX, NFTL_MODE_NO_SAVE_TRL_TBL) != OK) {
        AmbaPrintColor(RED,"[AppMainTask]  NFTL init NFTL_ID_IDX fail %d",AmbaNFTL_IsInit(NFTL_ID_IDX));
    }
    AmbaNFTL_Init(NFTL_ID_PRF, NFTL_MODE_NO_SAVE_TRL_TBL);
    AmbaNFTL_Init(NFTL_ID_CAL, NFTL_MODE_NO_SAVE_TRL_TBL);
    AmbaNFTL_Init(NFTL_ID_STORAGE, NFTL_MODE_NO_SAVE_TRL_TBL);
    AmbaFS_Nand1CheckMedia();
    return ReturnValue;
}
#endif

/**
 *  @brief Add the test command
 *
 *  Add the test command
 *
 */
static void Main_TestAddCmd(void)
{
    {
        extern void AmbaTest_DspAddCommands(void);
        AmbaTest_DspAddCommands();
    }
    {
        extern int AppLib_ImgProcTestAdd(void);
        AppLib_ImgProcTestAdd();
    }
    {
        extern __attribute__((weak)) int AmpUT_ImgkTestAdd(void);
        if (AmpUT_ImgkTestAdd) {
            AmpUT_ImgkTestAdd();
        }
    }
    {
        extern __attribute__((weak)) int AppLib_TuneTestAdd(void);
        if (AppLib_TuneTestAdd) {
            AppLib_TuneTestAdd();
        }
    }
    {
        extern int AppLib_CalibModuleInit(void);
        AppLib_CalibModuleInit();
    }
    {
        extern int test_fioprf_thruput(AMBA_SHELL_ENV_s *, int, char **);
        extern int test_fioprf_randvrfy(AMBA_SHELL_ENV_s *, int, char **);
        extern int test_sd(AMBA_SHELL_ENV_s *, int, char **);
        extern int test_sd_shmoo(AMBA_SHELL_ENV_s *, int, char **);
        extern int test_chk_low_spd(AMBA_SHELL_ENV_s *, int, char **);

        AmbaTest_RegisterCommand("fioprf_thruput", test_fioprf_thruput);
        AmbaTest_RegisterCommand("fioprf_randvrfy", test_fioprf_randvrfy);
        AmbaTest_RegisterCommand("sd", test_sd);
        AmbaTest_RegisterCommand("sd_shmoo", test_sd_shmoo);
        AmbaTest_RegisterCommand("chk_low_spd", test_chk_low_spd);
    }

    {
        extern int test_gpio(AMBA_SHELL_ENV_s *, int , char **);
        AmbaTest_RegisterCommand("gpio", test_gpio);
    }

#if defined(CONFIG_APP_AMBA_LINK)
    {
        extern int AppLib_HiberTestAdd(void);
        AppLib_HiberTestAdd();
    }
#endif

#ifdef CONFIG_BT_AMBA
    {
        extern void AmbaTest_BT_AMBA_Add(void);
        AmbaTest_BT_AMBA_Add();
    }
#endif /* CONFIG_BT_AMBA */
}


/**
 *  @brief User defined System Initializations (after OS running)
 *
 *  User defined System Initializations (after OS running)
 *
 *  @return >=0 success, <0 failure
 */
static void AppMainTask_Entry(UINT32 info)
{
    int ReturnValue = 0;

    static char *AutoExecScripts[] = {
        "c:\\autoexec.ash",
        "d:\\autoexec.ash"
    };

    /* Initialize the memory.*/
    Main_MemInit();

    /* Initialize the ADC device.*/
    AmbaADC_Init();
    AmbaADC_Start();

#ifdef CONFIG_APP_ARD
#if (defined(CONFIG_BSP_ARIES) || defined(CONFIG_BSP_GOAT) || defined(CONFIG_BSP_ORTHRUS))
    /*Get and save the BSP(HW) Ver*/
    AmbaUser_GetVersion();
#endif
#endif

    /** Initialize Application's memory manager.*/
    AppLibComSvcMemMgr_Init(&G_MMPL, &G_NC_MMPL);
    AppLibComSvcMemMgr_SetDspMemory(DspWorkAreaResvStart, DspWorkAreaResvLimit, DspWorkAreaResvSize);

    AppMainTask_HookSensor();

    AppMainTask_HookLcd();

    AppMainTask_HookImu();

    AppMainTask_HookAudio();

    #ifdef CONFIG_APP_ARD
    AppMainTask_HookGSensor();
    #endif

    {
        extern __attribute__((weak)) void AmbaUserExpGPIO_Init(void);
        AmbaUserExpGPIO_Init();  /**< Initialize all Expanded GPIOs (after OS running) */
    }

    /* Initialize uart.*/
    Main_UartInit();

    /* Initialize printk.*/
    Main_PrintkInit();
#ifdef CONFIG_APP_ARD
#if (defined(CONFIG_BSP_ARIES) || defined(CONFIG_BSP_GOAT) || defined(CONFIG_BSP_ORTHRUS))
    /*Printk the BSP(HW) Ver*/
    AmbaPrint("BspVer=%d",AmbaUser_GetVersion());
#endif
#endif

    /* Initialize MW.*/
    Main_MWInit();

    /* Print the information of memory after the Printk task enabled.*/
    Main_MemPrint();

    /* Initialize the RTC. */
    AmbaRTC_Init();

    /* Initialize the GDMA. */
    AmbaGDMA_Init();

    /* Initialize the test task.*/
    AmbaTest_Init();

    /* FIO Initializations */
    AmbaFIO_Init();
    /* Initialize the file system.*/
    AmbaFS_Init(AMBA_FS_ASCII);

    /* Initialize the card manager.*/
    AmbaSCM_Init();

#ifdef CONFIG_ENABLE_EMMC_BOOT
    #ifdef CONFIG_BSP_TAROKO
    AmbaSD_SetEmmcBusWidth(EMMC_BUSWIDTH_4BIT);
    #else
    AmbaSD_SetEmmcBusWidth(EMMC_BUSWIDTH_1BIT);
    #endif
    AmbaEMMC_Init();
#else
    /* SD card Initializations */
#ifdef CONFIG_APP_ARD
#if defined(CONFIG_BSP_ARIES)
    if (AmbaUser_GetVersion() == AMBA_BSP_ARIES_VERSION_V00) {
        AmbaSD_Init(1, 50000000); /* for 1st version Aries */
    } else if (AmbaUser_GetVersion() == AMBA_BSP_ARIES_VERSION_V01) {
        AmbaSD_Init(1, 80000000);
    }else{
        AmbaSD_Init(1, SD_UHS_MAX_FREQ);
    }
#elif defined(CONFIG_BSP_GOAT)
    if (AmbaUser_GetVersion() == AMBA_BSP_GOAT_VERSION_V00) {
        AmbaSD_Init(1, 80000000);
    }else{
        AmbaSD_Init(1, SD_UHS_MAX_FREQ);
    }
#elif defined(CONFIG_BSP_ORTHRUS)
    if (AmbaUser_GetVersion() == AMBA_BSP_ORTHRUS_VERSION_V00) {
        AmbaSD_Init(1, 80000000);
    }else{
        AmbaSD_Init(1, SD_UHS_MAX_FREQ);
    }
#else
    AmbaSD_Init(1, SD_UHS_MAX_FREQ);
#endif
#else
    AmbaSD_Init(1, SD_UHS_MAX_FREQ);
#endif
    AmbaUserSD_Adjust();

    /* NAND Initializations */
    {
        extern AMBA_NAND_DEV_INFO_s AmbaNAND_DevInfo;
        AmbaNAND_Init(&AmbaNAND_DevInfo); /* NAND Initializations */
    }
#endif

    /* Initialize ROM. */
#ifdef CONFIG_ENABLE_EMMC_BOOT
    ReturnValue = AmbaROM_Init(&G_MMPL, AMBA_ROM_DEVICE_SM); /* ROM Initializations */
#else
    ReturnValue = AmbaROM_Init(&G_MMPL, AMBA_ROM_DEVICE_NAND); /* ROM Initializations */
#endif
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[AppMainTask] AmbaROM_Init failure %%",ReturnValue);
    }

#ifndef CONFIG_ENABLE_EMMC_BOOT
    /* Initialize NTFL. */
    Main_NTFLInit();
#endif

    /* Initialize FIFO. */
    Main_FifoInit();

    /* Initialize the DCF module. */
    Main_DcfInit();

    /* Initialize the audio codec. */
    {
        extern void AmbaUserAudioCodec_Init(void);
        AmbaUserAudioCodec_Init();
    }
    /** Initialize monitor module.*/
    AppLibMonitor_Init();

    /* Print the system's information. */
    {
        /* Initialize version module. */
        AmbaVer_Init();

        AmbaPrint("---------------------------------------------");
        AmbaPrint("BSP: %s",BSPNAME);
        AmbaPrint("---------------------------------------------");
        AmbaPrint("Cortex freq:\t\t%d", AmbaPLL_GetCortexClk());
        AmbaPrint("DDR freq:\t\t%d", AmbaPLL_GetDdrClk());
        AmbaPrint("IDSP freq:\t\t%d", AmbaPLL_GetIdspClk());
        AmbaPrint("Core freq:\t\t%d", AmbaPLL_GetCoreClk());
        AmbaPrint("AXI freq:\t\t%d", AmbaPLL_GetAxiClk());
        AmbaPrint("AHB freq:\t\t%d", AmbaPLL_GetAhbClk());
        AmbaPrint("APB freq:\t\t%d", AmbaPLL_GetApbClk());
        AmbaPrint("---------------------------------------------");

        AmbaPrint("----- Version info --------------------------");
        AmbaVer_ShowAllVerCompactInfo();
        AmbaVer_ShowLinkVerInfo();
        AmbaPrint("---------------------------------------------");

#ifdef CONFIG_BSP_TAROKO
        if (AmbaUser_GetVersion() == AMBA_BSP_VERSION_V11A_09V) {
            if (AmbaPLL_GetCortexClk() != 504000000) {
                AmbaPrintColor(RED,"Cortex frequency is not correct: Should be 504000000. But get %d.",AmbaPLL_GetCortexClk());
            }
        } else {
            if (AmbaPLL_GetCortexClk() != 792000000) {
                AmbaPrintColor(RED,"Cortex frequency is not correct: Should be 792000000. But get %d.",AmbaPLL_GetCortexClk());
            }
        }
#else
        if (AmbaPLL_GetDdrClk() == 396000000) {
            if (AmbaPLL_GetCortexClk() != 504000000) {
                AmbaPrintColor(RED,"Cortex frequency is not correct: Should be 504000000. But get %d.",AmbaPLL_GetCortexClk());
            }
        } else {
            if (AmbaPLL_GetCortexClk() != 792000000) {
                AmbaPrintColor(RED,"Cortex frequency is not correct: Should be 792000000. But get %d.",AmbaPLL_GetCortexClk());
            }
        }
#endif
    }

    /* Dsp command show. */
    if (0) {
        AmbaDSP_CmdShowAll(1);
        AmbaDSP_CmdShow("0x7004", 0);
        AmbaDSP_CmdShow("0x7005", 0);
        AmbaDSP_CmdShowCat(92, 0);
        AmbaDSP_CmdShowCat(93, 0);
        AmbaDSP_CmdShowCat(94, 0);
        AmbaDSP_CmdShowCat(95, 0);
        AmbaDSP_CmdShowCat(96, 0);
    }

    /* Load ucode. */
    {
        extern int AmbaLoadDSPuCode(void);
        AmbaLoadDSPuCode(); /**< Load uCodes from NAND */
    }
    /* Initialize the button task. */
    {
        extern void BspRefButton_Init(void);
        BspRefButton_Init();
    }

    /* Enable the host control manager for application, */
    {
        extern APPLIB_HCMGR_HANDLER_s Handler;

        AppLibComSvcHcmgr_PreInit();
        AppLibComSvcHcmgr_AttachHandler(&Handler);
        AppLibComSvcHcmgr_Init();
    }
    /* Enable shell */
    {
//    AmbaShell_Init(&G_MMPL, AmbaUserConsole_Write, AmbaUserConsole_Read);
        AMBA_SHELL_CONFIG_s ShellConfig = {0};
        ShellConfig.Priority = APP_AMBA_SHELL_PRIORITY;
        ShellConfig.pCachedHeap = &G_MMPL;
        ShellConfig.ConsolePutCharCb = AmbaUserConsole_Write;
        ShellConfig.ConsoleGetCharCb = AmbaUserConsole_Read;
        AmbaShell_Init(&ShellConfig);
    }
    AmbaShell_Start(GetArraySize(AutoExecScripts), AutoExecScripts);

    /* Enable some test commands.*/
    Main_TestAddCmd();
}


/**
 *  @brief Host Control Manager initial function
 *
 *  Host Control Manager initial function
 *
 *  @return >=0 success, <0 failure
 */
int AppMainTask_Init(void)
{
    int ReturnValue = 0;

    /* Create Host Control Manager task*/
    ReturnValue = AmbaKAL_TaskCreate(&AppMainTask.Task, /* pTask */
            APPMAINTASK_NAME, /* pTaskName */
            APP_BOOT_MGR_PRIORITY, /* Priority */
            AppMainTask_Entry, /* void (*EntryFunction)(UINT32) */
            0x0, /* EntryArg */
            (void *) AppMainTask.Stack, /* pStackBase */
            APPMAINTASK_STACK_SIZE, /* StackByteSize */
            AMBA_KAL_AUTO_START); /* AutoStart */
    if (ReturnValue != OK) {
        AmbaPrintColor(RED, "[AppMainTask]Create task fail = %d", ReturnValue);
    }

    return 0;
}

void AmbaUserSysInitPreOS(void)
{
    extern void AmbaRTSL_GpioInit(AMBA_GPIO_PIN_GROUP_CONFIG_s *);
    extern AMBA_GPIO_PIN_GROUP_CONFIG_s GpioPinGrpConfig;

    AmbaRTSL_GpioInit(&GpioPinGrpConfig);   /* GPIO Initializations before OS running */
}

void AmbaUserSysInitPostOS(void)
{
    AppMainTask_Init();
}

