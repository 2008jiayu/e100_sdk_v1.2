/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDSP_Event.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella DSP Event Handler APIs
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DSP_EVENT_H_
#define _AMBA_DSP_EVENT_H_

#include "AmbaDataType.h"
#include "AmbaDSP_EventInfo.h"

typedef enum _AMBA_DSP_EVENT_ID_e_ {
    AMBA_DSP_EVENT_CFA_3A_DATA_READY = 0,            /*  0 Video CFA 3A statistic data event (AMBA_DSP_EVENT_CFA_3A_DATA_s) */
    AMBA_DSP_EVENT_RGB_3A_DATA_READY,                /*  1 Video RGB 3A statistic data event (AMBA_DSP_EVENT_RGB_3A_DATA_s) */

    AMBA_DSP_EVENT_RAW_CFA_3A_DATA_READY,            /*  2 Still CFA 3A statistic data event (AMBA_DSP_EVENT_STILL_CFA_3A_DATA_s) */
    AMBA_DSP_EVENT_RAW_RGB_3A_DATA_READY,            /*  3 Still RGB 3A statistic data event (AMBA_DSP_EVENT_STILL_RGB_3A_DATA_s )*/

    AMBA_DSP_EVENT_VIDEO_DATA_READY,                 /*  4 Video bitstream data event (AMBA_DSP_EVENT_ENC_PIC_READY_s) */
    AMBA_DSP_EVENT_JPEG_DATA_READY,                  /*  5 JPEG bitstream data event (AMBA_DSP_EVENT_ENC_PIC_READY_s) */
    AMBA_DSP_EVENT_MJPEG_DATA_READY,                 /*  6 Motion JPEG Bitstream data event (AMBA_DSP_EVENT_ENC_PIC_READY_s) */

    AMBA_DSP_EVENT_LIVEVIEW_RAW_DATA_READY,          /*  7 Liveview RAW data event (AMBA_DSP_RAW_BUF_s) */
    AMBA_DSP_EVENT_LIVEVIEW_LCD_YUV_DATA_READY,      /*  8 Liveview LCD YUV420 data event (AMBA_DSP_YUV_IMG_BUF_s) */
    AMBA_DSP_EVENT_LIVEVIEW_TV_YUV_DATA_READY,       /*  9 Liveview TV YUV420 data event (AMBA_DSP_YUV_IMG_BUF_s) */

    AMBA_DSP_EVENT_VCAP_MAIN_YUV_DATA_READY,         /* 10 Video main YUV420 data event (AMBA_DSP_YUV_IMG_BUF_s) */
    AMBA_DSP_EVENT_VCAP_2ND_YUV_DATA_READY,          /* 11 Video 2nd YUV420 data event (AMBA_DSP_YUV_IMG_BUF_s) */

    AMBA_DSP_EVENT_SCAP_RAW_CAPTURE_START,           /* 12 Still RAW capture start event*/
    AMBA_DSP_EVENT_SCAP_RAW_DATA_READY,              /* 13 Still RAW data event (AMBA_DSP_RAW_BUF_s) */
    AMBA_DSP_EVENT_SCAP_CLEAN_RAW_DATA_READY,        /* 14 Still Clean RAW data event (AMBA_DSP_RAW_BUF_s) */
    AMBA_DSP_EVENT_SCAP_YUV_DATA_READY,              /* 15 Still YUV data event (AMBA_DSP_YUV_IMG_BUF_s) */
    AMBA_DSP_EVENT_SCAP_HDR_DATA_READY,              /* 16 Still HDR data event (UINT32) */

    AMBA_DSP_EVENT_JPEG_DEC_STATUS_REPORT,           /* 17 JPEG dec status event (AMBA_DSP_EVENT_JPEG_DEC_STATUS_REPORT_s) */
    AMBA_DSP_EVENT_JPEG_DEC_YUV_DISP_REPORT,         /* 18 JPEG dec YUV disp event (AMBA_DSP_EVENT_JPEG_DEC_YUV_DISP_REPORT_s) */
    AMBA_DSP_EVENT_JPEG_DEC_COMMON_BUFFER_REPORT,    /* 19 JPEG dec common buffer event (AMBA_DSP_EVENT_JPEG_DEC_COMMON_BUFFER_REPORT_s) */
    AMBA_DSP_EVENT_H264_DEC_STATUS_REPORT,           /* 20 H264 dec status event (AMBA_DSP_EVENT_H264_DEC_STATUS_UPDATE_s) */
    
    AMBA_DSP_EVENT_VIDEO_ENC_START,                  /* 21 Video Enc start event (AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s)*/
    AMBA_DSP_EVENT_VIDEO_ENC_PAUSE,                  /* 22 Video Enc pause event (AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s)*/
    AMBA_DSP_EVENT_VIDEO_ENC_RESUME,                 /* 23 Video Enc resume event (AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s)*/
    AMBA_DSP_EVENT_VIDEO_ENC_STOP,                   /* 24 Video Enc stop event (AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s)*/

    AMBA_DSP_EVENT_ME1_DATA_READY,                   /* 25 ME1 data in hybrid mode event (AMBA_DSP_YUV_INFO_s) */
    AMBA_DSP_EVENT_HYBRID_YUV_DATA_READY,            /* 26 IsoCfg effected YUV420 data event in HybridMode (AMBA_DSP_YUV_INFO_s) */

    AMBA_DSP_EVENT_DSP_INT_VIN0_SOF,                 /* 27 */
    AMBA_DSP_EVENT_DSP_INT_VIN1_SOF,                 /* 28 */
    AMBA_DSP_EVENT_DSP_INT_VIN0_EOF,                 /* 29 */
    AMBA_DSP_EVENT_DSP_INT_VIN1_EOF,                 /* 30 */
    AMBA_DSP_EVENT_DSP_INT_VOUT0,                    /* 31 */
    AMBA_DSP_EVENT_DSP_INT_VOUT1,                    /* 32 */
    AMBA_DSP_EVENT_ISO_CFG,                          /* 33 Iso config updated event (AMBA_DSP_CFG_UPDATE_INFO_s) */
    AMBA_DSP_EVENT_HDR_CFG,                          /* 34 HDR config updated event (AMBA_DSP_CFG_UPDATE_INFO_s) */
    AMBA_DSP_EVENT_MAIN_VIN_STAT_READY,              /* 35 Main vin statistics event (AMBA_DSP_EVENT_VIN_STATS_INFO_s) */
    AMBA_DSP_EVENT_HDR_VIN_STAT_READY,               /* 36 HDR vin statistics event (AMBA_DSP_EVENT_VIN_STATS_INFO_s) */
    AMBA_DSP_EVENT_DSP_INT_VIN0_VSYNC_EOF,           /* 37 */
    AMBA_DSP_EVENT_DSP_INT_VIN1_VSYNC_EOF,           /* 38 */
    
    AMBA_DSP_EVENT_ERROR,                            /* 39 DSP error event (AMBA_DSP_ERROR_INFO_s) */
    AMBA_DSP_EVENT_ASSERTION,                        /* 40 DSP/uCode Assertion */
    
    AMBA_NUM_DSP_EVENT
} AMBA_DSP_EVENT_ID_e;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaDSP_Event.c
\*-----------------------------------------------------------------------------------------------*/

typedef int (*AMBA_DSP_EVENT_HANDLER_f)(void *pEventData);

typedef struct _AMBA_DSP_EVENT_HANDLER_CTRL_s_ {
    INT16   MaxNumHandlers;                     /* maximum number of Handlers */
    AMBA_DSP_EVENT_HANDLER_f *pEventHandlers;   /* pointer to the Event Handlers */
} AMBA_DSP_EVENT_HANDLER_CTRL_s;

extern int AmbaDSP_EventInit(void);

extern int AmbaDSP_EventHandlerCtrlReset(AMBA_DSP_EVENT_ID_e EventID);
extern int AmbaDSP_EventHandlerCtrlConfig(AMBA_DSP_EVENT_ID_e EventID,
                                          int MaxNumHandlers,
                                          AMBA_DSP_EVENT_HANDLER_f *pEventHandlers);
extern int AmbaDSP_RegisterEventHandler(AMBA_DSP_EVENT_ID_e EventID, AMBA_DSP_EVENT_HANDLER_f EventHandler);
extern int AmbaDSP_UnRegisterEventHandler(AMBA_DSP_EVENT_ID_e EventID, AMBA_DSP_EVENT_HANDLER_f EventHandler);

extern int AmbaDSP_GiveEvent(AMBA_DSP_EVENT_ID_e EventID, void *pEventInfo);

#endif  /* _AMBA_DSP_EVENT_H_ */
