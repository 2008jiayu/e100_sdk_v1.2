 /**
  * @file comsvc/misc/synssvc.c
  *
  * ISR sync service
  *
  * History:
  *    2013/03/02/ - [Jenghung Luo] created file
  *
  * Copyright (C) 2013, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#include "AmbaDataType.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_Event.h"
#include "AmbaPrintk.h"
#include "syncsvc.h"
#include "AmbaINT.h"
#include "isr.h"
#include "AmbaTimer.h"

#define SYNCSVC_MAX_ISR_HDLR        1

static AMBA_DSP_EVENT_HANDLER_f ISRHdlrVinSof0[SYNCSVC_MAX_ISR_HDLR];
static AMBA_DSP_EVENT_HANDLER_f ISRHdlrVinSof1[SYNCSVC_MAX_ISR_HDLR];
static AMBA_DSP_EVENT_HANDLER_f ISRHdlrVinEof0[SYNCSVC_MAX_ISR_HDLR];
static AMBA_DSP_EVENT_HANDLER_f ISRHdlrVinEof1[SYNCSVC_MAX_ISR_HDLR];
static AMBA_DSP_EVENT_HANDLER_f ISRHdlrVinVsyncEof0[SYNCSVC_MAX_ISR_HDLR];
static AMBA_DSP_EVENT_HANDLER_f ISRHdlrVinVsyncEof1[SYNCSVC_MAX_ISR_HDLR];
static AMBA_DSP_EVENT_HANDLER_f ISRHdlrVout0[SYNCSVC_MAX_ISR_HDLR];
static AMBA_DSP_EVENT_HANDLER_f ISRHdlrVout1[SYNCSVC_MAX_ISR_HDLR];

#pragma pack(4)
typedef struct AmpSync_VAR_s_ {
    AMBA_KAL_EVENT_FLAG_t SyncFlg;
    UINT8  FlagInit;
    UINT8  VinSofInit[2];           // Chip dependent
    UINT8  VinEofInit[2];           // Chip dependent
    UINT8  VinVsyncEofInit[2];      // Chip dependent
    UINT32 VinSofSystemTime[2];     // Chip dependent
    UINT32 VinEofSystemTime[2];     // Chip dependent
    UINT32 VinVsyncEofSystemTime[2];// Chip dependent
    AMBA_KAL_MUTEX_t VinMainMutex;
    AMBA_KAL_MUTEX_t VinPipMutex;

//    UINT8 VdspInit;
//    UINT8 VcapInit;
    UINT8 VoutDchInit;
    UINT8 VoutFchInit;
} AmpSync_VAR_s;
#pragma pack()

static AmpSync_VAR_s G_AmpSync = {{0}};


#define _SYNCSVC_DEBUG__

#ifdef _SYNCSVC_DEBUG__
#define syncsvc_print    AmbaPrint
#else
#define syncsvc_print(...)
#endif


/**
 * VIN0 SOF interrupt handler
 *
 * param [in] IrqNo Irq number
 */
static int AmpSync_Vin0SOF(void *pData)
{
    AmbaKAL_MutexTake(&G_AmpSync.VinMainMutex, AMBA_KAL_WAIT_FOREVER);
    G_AmpSync.VinSofSystemTime[0] = AmbaTimer_GetSysTickCount();
    AmbaKAL_MutexGive(&G_AmpSync.VinMainMutex);
    return AmbaKAL_EventFlagGive(&G_AmpSync.SyncFlg, SYNCSVC_FLAG_VIN0_SOF);
}

/**
 * VIN0 LastPixel interrupt handler
 *
 * param [in] IrqNo Irq number
 */
static int AmpSync_Vin0LastPixel(void *pData)
{
    AmbaKAL_MutexTake(&G_AmpSync.VinMainMutex, AMBA_KAL_WAIT_FOREVER);
    G_AmpSync.VinEofSystemTime[0] = AmbaTimer_GetSysTickCount();
    AmbaKAL_MutexGive(&G_AmpSync.VinMainMutex);
    return AmbaKAL_EventFlagGive(&G_AmpSync.SyncFlg, SYNCSVC_FLAG_VIN0_LASTPIXEL);
}

/**
 * VIN0 LastPixel interrupt handler
 *
 * param [in] IrqNo Irq number
 */
static int AmpSync_Vin0VsyncEOF(void *pData)
{
    AmbaKAL_MutexTake(&G_AmpSync.VinMainMutex, AMBA_KAL_WAIT_FOREVER);
    G_AmpSync.VinVsyncEofSystemTime[0] = AmbaTimer_GetSysTickCount();
    AmbaKAL_MutexGive(&G_AmpSync.VinMainMutex);
    return AmbaKAL_EventFlagGive(&G_AmpSync.SyncFlg, SYNCSVC_FLAG_VIN0_VSYNC_EOF);
}

/**
 * VIN0 SOF interrupt handler
 *
 * param [in] IrqNo Irq number
 */
static int AmpSync_Vin1SOF(void *pData)
{
    AmbaKAL_MutexTake(&G_AmpSync.VinPipMutex, AMBA_KAL_WAIT_FOREVER);
    G_AmpSync.VinSofSystemTime[1] = AmbaTimer_GetSysTickCount();
    AmbaKAL_MutexGive(&G_AmpSync.VinPipMutex);
    return AmbaKAL_EventFlagGive(&G_AmpSync.SyncFlg, SYNCSVC_FLAG_VIN1_SOF);
}

/**
 * VIN0 LastPixel interrupt handler
 *
 * param [in] IrqNo Irq number
 */
static int AmpSync_Vin1LastPixel(void *pData)
{
    AmbaKAL_MutexTake(&G_AmpSync.VinPipMutex, AMBA_KAL_WAIT_FOREVER);
    G_AmpSync.VinEofSystemTime[1] = AmbaTimer_GetSysTickCount();
    AmbaKAL_MutexGive(&G_AmpSync.VinPipMutex);
    return AmbaKAL_EventFlagGive(&G_AmpSync.SyncFlg, SYNCSVC_FLAG_VIN1_LASTPIXEL);
}

/**
 * VIN1 LastPixel interrupt handler
 *
 * param [in] IrqNo Irq number
 */
static int AmpSync_Vin1VsyncEOF(void *pData)
{
    AmbaKAL_MutexTake(&G_AmpSync.VinPipMutex, AMBA_KAL_WAIT_FOREVER);
    G_AmpSync.VinVsyncEofSystemTime[1] = AmbaTimer_GetSysTickCount();
    AmbaKAL_MutexGive(&G_AmpSync.VinPipMutex);
    return AmbaKAL_EventFlagGive(&G_AmpSync.SyncFlg, SYNCSVC_FLAG_VIN1_VSYNC_EOF);
}

/**
 * VIN_MASTER interrupt handler
 *
 * param [in] IrqNo Irq number
 */
static int AmpSync_VoutFchISR(void *pData)
{
    return AmbaKAL_EventFlagGive(&G_AmpSync.SyncFlg, SYNCSVC_FLAG_VOUT_FCH);
}


/**
 * VIN_MASTER interrupt handler
 *
 * param [in] IrqNo Irq number
 */
static int AmpSync_VoutDchISR(void *pData)
{
    return AmbaKAL_EventFlagGive(&G_AmpSync.SyncFlg, SYNCSVC_FLAG_VOUT_DCH);
}

/**
 * Init sync service
 *
 * @param [in] resource resource to sync
 * @param [in] cfg parameters
 *
 * @return 0 - OK, others - errnum
 */
int AmpSync_Init(UINT8 resource, void *cfg)
{
    int er = 0;

    if (G_AmpSync.FlagInit == 0) {
        er = AmbaKAL_EventFlagCreate(&G_AmpSync.SyncFlg);
        if (er != OK) {
            syncsvc_print("!AmpSync_Init");
        }
        ISRHdlrVinSof0[0] = AmpSync_Vin0SOF;
        ISRHdlrVinSof1[0] = AmpSync_Vin1SOF;
        ISRHdlrVinEof0[0] = AmpSync_Vin0LastPixel;
        ISRHdlrVinEof1[0] = AmpSync_Vin1LastPixel;
        ISRHdlrVinVsyncEof0[0] = AmpSync_Vin0VsyncEOF;
        ISRHdlrVinVsyncEof1[0] = AmpSync_Vin1VsyncEOF;
        ISRHdlrVout0[0] = AmpSync_VoutDchISR;
        ISRHdlrVout1[0] = AmpSync_VoutFchISR;

        AmbaKAL_MutexCreate(&G_AmpSync.VinMainMutex);
        AmbaKAL_MutexCreate(&G_AmpSync.VinPipMutex);

        G_AmpSync.FlagInit = 1;
    }

    switch (resource) {
        case AMP_SYNCSVC_VIN_SOF:
            {
                UINT8 *channel = cfg;

                if (*channel == 0 && G_AmpSync.VinSofInit[0] == 0) {
                    if (AmbaDSP_EventHandlerCtrlConfig(AMBA_DSP_EVENT_DSP_INT_VIN0_SOF, 1, ISRHdlrVinSof0) != OK) {
                        return NG;
                    }
                    G_AmpSync.VinSofInit[0] = 1;
                } else if (*channel == 1 && G_AmpSync.VinSofInit[1] == 0) {
                    if (AmbaDSP_EventHandlerCtrlConfig(AMBA_DSP_EVENT_DSP_INT_VIN1_SOF, 1, ISRHdlrVinSof1) != OK) {
                        return NG;
                    }
                    G_AmpSync.VinSofInit[1] =  1;
                    return er;
                } else {
                    return er;
                }
            }
            break;
        case AMP_SYNCSVC_VIN_LASTPIXEL:
            {
                UINT8 *channel = cfg;

                if (*channel == 0 && G_AmpSync.VinEofInit[0] == 0) {
                    if (AmbaDSP_EventHandlerCtrlConfig(AMBA_DSP_EVENT_DSP_INT_VIN0_EOF, 1, ISRHdlrVinEof0) != OK) {
                        return er;
                    }
                    G_AmpSync.VinEofInit[0] = 1;
                } else if (*channel == 1 && G_AmpSync.VinEofInit[1] == 0) {
                    if (AmbaDSP_EventHandlerCtrlConfig(AMBA_DSP_EVENT_DSP_INT_VIN1_EOF, 1, ISRHdlrVinEof1) != OK) {
                        return er;
                    }
                    G_AmpSync.VinEofInit[1] = 1;
                    return er;
                } else {
                    return er;
                }
            }
            break;
        case AMP_SYNCSVC_VIN_VSYNC_EOF:
            {
                UINT8 *channel = cfg;

                if (*channel == 0 && G_AmpSync.VinVsyncEofInit[0] == 0) {
                    if (AmbaDSP_EventHandlerCtrlConfig(AMBA_DSP_EVENT_DSP_INT_VIN0_VSYNC_EOF, 1, ISRHdlrVinVsyncEof0) != OK) {
                        return er;
                    }
                    G_AmpSync.VinVsyncEofInit[0] = 1;
                } else if (*channel == 1 && G_AmpSync.VinVsyncEofInit[1] == 0) {
                    if (AmbaDSP_EventHandlerCtrlConfig(AMBA_DSP_EVENT_DSP_INT_VIN1_VSYNC_EOF, 1, ISRHdlrVinVsyncEof1) != OK) {
                        return er;
                    }
                    G_AmpSync.VinVsyncEofInit[1] = 1;
                    return er;
                } else {
                    return er;
                }
            }
            break;
        case AMP_SYNCSVC_VOUTDCH:
            if (G_AmpSync.VoutDchInit == 0) {
                if (AmbaDSP_EventHandlerCtrlConfig(AMBA_DSP_EVENT_DSP_INT_VOUT0, 1, ISRHdlrVout0) != OK) {
                    return er;
                }
                G_AmpSync.VoutDchInit = 1;
            }
            break;
        case AMP_SYNCSVC_VOUTFCH:
            if (G_AmpSync.VoutFchInit == 0) {
                if (AmbaDSP_EventHandlerCtrlConfig(AMBA_DSP_EVENT_DSP_INT_VOUT1, 1, ISRHdlrVout1) != OK) {
                    return er;
                }
                G_AmpSync.VoutFchInit = 1;
            }
            break;
        default:
            er = NG;
            break;
    }

    return er;
}

/**
 *  Wait Vin SOF syncs
 *
 *  @param [in] channel
 *  @param [in] intCount Number of interrupts
 *  @param [in] timeout Timeout period for each waiting
 *
 *  @return 0 - OK, others - error number
 */
int AmpSync_WaitVinSofInt(UINT32 channel, UINT32 intCount, UINT32 timeout)
{
    int i = 0;
    int er;
    UINT32 actualEvent;
    UINT32 flag = 0, op = 0;

    // For A9
    switch (channel) {
       case 0:
            flag = SYNCSVC_FLAG_VIN0_SOF;
            op = AMBA_KAL_AND_CLEAR;
            break;
       case 1:
            flag = SYNCSVC_FLAG_VIN1_SOF;
            op = AMBA_KAL_AND_CLEAR;
            break;
       default:
            AmbaPrint("No such vin channel");
            break;
    }
    if (flag == 0) {
        return -1;
    }

    AmbaKAL_EventFlagClear(&G_AmpSync.SyncFlg, flag);

    for (i = 0; i < intCount; i++) {
        er = AmbaKAL_EventFlagTake(&G_AmpSync.SyncFlg, flag, op, &actualEvent, timeout);
        if (er != OK) {
            return er;
        }
    }

    return 0;
}

/**
 *  Wait Vin EOF syncs
 *
 *  @param [in] channel
 *  @param [in] intCount Number of interrupts
 *  @param [in] timeout Timeout period for each waiting
 *
 *  @return 0 - OK, others - error number
 */
int AmpSync_WaitVinEofInt(UINT32 channel, UINT32 intCount, UINT32 timeout)
{
    int i = 0;
    int er;
    UINT32 actualEvent;
    UINT32 flag = 0, op = 0;

    // For A9
    switch (channel) {
       case 0:
            flag = SYNCSVC_FLAG_VIN0_LASTPIXEL;
            op = AMBA_KAL_AND_CLEAR;
            break;
       case 1:
            flag = SYNCSVC_FLAG_VIN1_LASTPIXEL;
            op = AMBA_KAL_AND_CLEAR;
            break;
       default:
            AmbaPrint("No such vin channel");
            break;
    }
    if (flag == 0) {
        return -1;
    }

    AmbaKAL_EventFlagClear(&G_AmpSync.SyncFlg, flag);

    for (i = 0; i < intCount; i++) {
        er = AmbaKAL_EventFlagTake(&G_AmpSync.SyncFlg, flag, op, &actualEvent, timeout);
        if (er != OK) {
            return er;
        }
    }

    return 0;
}

/**
 *  Wait Vin Vsync EOF syncs
 *
 *  @param [in] channel
 *  @param [in] intCount Number of interrupts
 *  @param [in] timeout Timeout period for each waiting
 *
 *  @return 0 - OK, others - error number
 */
int AmpSync_WaitVinVsyncEofInt(UINT32 channel, UINT32 intCount, UINT32 timeout)
{
    int i = 0;
    int er;
    UINT32 actualEvent;
    UINT32 flag = 0, op = 0;

    // For A9
    switch (channel) {
       case 0:
            flag = SYNCSVC_FLAG_VIN0_VSYNC_EOF;
            op = AMBA_KAL_AND_CLEAR;
            break;
       case 1:
            flag = SYNCSVC_FLAG_VIN1_VSYNC_EOF;
            op = AMBA_KAL_AND_CLEAR;
            break;
       default:
            AmbaPrint("No such vin channel");
            break;
    }
    if (flag == 0) {
        return -1;
    }

    AmbaKAL_EventFlagClear(&G_AmpSync.SyncFlg, flag);

    for (i = 0; i < intCount; i++) {
        er = AmbaKAL_EventFlagTake(&G_AmpSync.SyncFlg, flag, op, &actualEvent, timeout);
        if (er != OK) {
            return er;
        }
    }

    return 0;
}

/**
 *  Get Last Vin SOF System time
 *
 *  @param [in] channel
 *  @param [out] time
 *
 *  @return 0 - OK, others - error number
 */
int AmpSync_GetVinSofSystemTime(UINT32 channel, UINT32 *time)
{
    // For A9
    switch (channel) {
       case 0:
       {
           AmbaKAL_MutexTake(&G_AmpSync.VinMainMutex, AMBA_KAL_WAIT_FOREVER);
           *time = G_AmpSync.VinSofSystemTime[0];
           AmbaKAL_MutexGive(&G_AmpSync.VinMainMutex);
       }
            break;
       case 1:
       {
           AmbaKAL_MutexTake(&G_AmpSync.VinPipMutex, AMBA_KAL_WAIT_FOREVER);
           *time = G_AmpSync.VinSofSystemTime[1];
           AmbaKAL_MutexGive(&G_AmpSync.VinPipMutex);
       }
            break;
       default:
            AmbaPrint("No such vin channel");
            break;
    }
    return 0;
}

/**
 *  Get Last Vin EOF System time
 *
 *  @param [in] channel
 *  @param [out] time
 *
 *  @return 0 - OK, others - error number
 */
int AmpSync_GetVinEofSystemTime(UINT32 channel, UINT32 *time)
{
    // For A9
    switch (channel) {
       case 0:
       {
           AmbaKAL_MutexTake(&G_AmpSync.VinMainMutex, AMBA_KAL_WAIT_FOREVER);
           *time = G_AmpSync.VinEofSystemTime[0];
           AmbaKAL_MutexGive(&G_AmpSync.VinMainMutex);
       }
            break;
       case 1:
       {
           AmbaKAL_MutexTake(&G_AmpSync.VinPipMutex, AMBA_KAL_WAIT_FOREVER);
           *time = G_AmpSync.VinEofSystemTime[1];
           AmbaKAL_MutexGive(&G_AmpSync.VinPipMutex);
       }
            break;
       default:
            AmbaPrint("No such vin channel");
            break;
    }
    return 0;
}

/**
 *  Get Last Vin Vsync EOF System time
 *
 *  @param [in] channel
 *  @param [out] time
 *
 *  @return 0 - OK, others - error number
 */
int AmpSync_GetVinVsyncEofSystemTime(UINT32 channel, UINT32 *time)
{
    // For A9
    switch (channel) {
       case 0:
       {
           AmbaKAL_MutexTake(&G_AmpSync.VinMainMutex, AMBA_KAL_WAIT_FOREVER);
           *time = G_AmpSync.VinVsyncEofSystemTime[0];
           AmbaKAL_MutexGive(&G_AmpSync.VinMainMutex);
       }
            break;
       case 1:
       {
           AmbaKAL_MutexTake(&G_AmpSync.VinPipMutex, AMBA_KAL_WAIT_FOREVER);
           *time = G_AmpSync.VinVsyncEofSystemTime[1];
           AmbaKAL_MutexGive(&G_AmpSync.VinPipMutex);
       }
            break;
       default:
            AmbaPrint("No such vin channel");
            break;
    }
    return 0;
}

/**
 *  Wait digital channel vout syncs
 *
 *  @param [in] intCount Number of interrupts
 *  @param [in] timeout timeout period for each waiting
 *
 *  @return 0 - OK, others - error number
 */
int AmpSync_WaitDChanVoutInt(UINT32 intCount, UINT32 timeout)
{
    int i = 0;
    int er;
    UINT32 actualEvent;

    AmbaKAL_EventFlagClear(&G_AmpSync.SyncFlg, SYNCSVC_FLAG_VOUT_DCH);

    for (i = 0; i < intCount; i++) {
        er = AmbaKAL_EventFlagTake(&G_AmpSync.SyncFlg, SYNCSVC_FLAG_VOUT_DCH, AMBA_KAL_AND_CLEAR, &actualEvent, timeout);
        if (er != OK) {
            return er;
        }
    }

    return 0;
}


/**
 *  Wait full function channel vout syncs
 *
 *  @param [in] intCount Number of interrupts
 *  @param [in] timeout timeout period for each waiting
 *
 *  @return 0 - OK, others - error number
 */
int AmpSync_WaitFChanVoutInt(UINT32 intCount, UINT32 timeout)
{
    int i = 0;
    int er;
    UINT32 actualEvent;

    AmbaKAL_EventFlagClear(&G_AmpSync.SyncFlg, SYNCSVC_FLAG_VOUT_FCH);

    for (i = 0; i < intCount; i++) {
        er = AmbaKAL_EventFlagTake(&G_AmpSync.SyncFlg, SYNCSVC_FLAG_VOUT_FCH, AMBA_KAL_AND_CLEAR, &actualEvent, timeout);
        if (er != OK) {
            return er;
        }
    }

    return 0;
}

