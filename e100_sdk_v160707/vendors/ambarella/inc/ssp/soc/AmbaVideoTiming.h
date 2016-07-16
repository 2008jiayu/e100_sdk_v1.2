/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaVideoTiming.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for Standard Video Timing Requirements
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_VIDEO_TIMING_H_
#define _AMBA_VIDEO_TIMING_H_

/*-----------------------------------------------------------------------------------------------*\
 * Definition of video ID code defined in CEA-861 standard
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_CEA861_VIDEO_ID_CODE_e_ {
    /* Code Short Name     ID    Aspect Ratio              HxV @ Frate */
    AMBA_VIDEO_ID_RESERVED = 0, /*   N/A */
    AMBA_VIDEO_ID_DMT0659,      /*   4:3,            640x480p @ 59.94/60Hz */
    AMBA_VIDEO_ID_480P,         /*   4:3,            720x480p @ 59.94/60Hz */
    AMBA_VIDEO_ID_480P_WIDE,    /*  16:9,            720x480p @ 59.94/60Hz */
    AMBA_VIDEO_ID_720P,         /*  16:9,           1280x720p @ 59.94/60Hz */
    AMBA_VIDEO_ID_1080I,        /*  16:9,          1920x1080i @ 59.94/60Hz */
    AMBA_VIDEO_ID_480I,         /*   4:3,      720(1440)x480i @ 59.94/60Hz */
    AMBA_VIDEO_ID_480I_WIDE,    /*  16:9,      720(1440)x480i @ 59.94/60Hz */
    AMBA_VIDEO_ID_240P,         /*   4:3,      720(1440)x240p @ 59.94/60Hz */
    AMBA_VIDEO_ID_240P_WIDE,    /*  16:9,      720(1440)x240p @ 59.94/60Hz */
    AMBA_VIDEO_ID_480I4X,       /*   4:3,         (2880)x480i @ 59.94/60Hz */
    AMBA_VIDEO_ID_480I4X_WIDE,  /*  16:9,         (2880)x480i @ 59.94/60Hz */
    AMBA_VIDEO_ID_240P4X,       /*   4:3,         (2880)x240p @ 59.94/60Hz */
    AMBA_VIDEO_ID_240P4X_WIDE,  /*  16:9,         (2880)x240p @ 59.94/60Hz */
    AMBA_VIDEO_ID_480P2X,       /*   4:3,           1440x480p @ 59.94/60Hz */
    AMBA_VIDEO_ID_480P2X_WIDE,  /*  16:9,           1440x480p @ 59.94/60Hz */
    AMBA_VIDEO_ID_1080P,        /*  16:9,          1920x1080p @ 59.94/60Hz */
    AMBA_VIDEO_ID_576P,         /*   4:3,            720x576p @ 50Hz */
    AMBA_VIDEO_ID_576P_WIDE,    /*  16:9,            720x576p @ 50Hz */
    AMBA_VIDEO_ID_720P50,       /*  16:9,           1280x720p @ 50Hz */
    AMBA_VIDEO_ID_1080I25,      /*  16:9,          1920x1080i @ 50Hz (V-Total 1125) */
    AMBA_VIDEO_ID_576I,         /*   4:3,      720(1440)x576i @ 50Hz */
    AMBA_VIDEO_ID_576I_WIDE,    /*  16:9,      720(1440)x576i @ 50Hz */
    AMBA_VIDEO_ID_288P,         /*   4:3,      720(1440)x288p @ 50Hz */
    AMBA_VIDEO_ID_288P_WIDE,    /*  16:9,      720(1440)x288p @ 50Hz */
    AMBA_VIDEO_ID_576I4X,       /*   4:3,         (2880)x576i @ 50Hz */
    AMBA_VIDEO_ID_576I4X_WIDE,  /*  16:9,         (2880)x576i @ 50Hz */
    AMBA_VIDEO_ID_288P4X,       /*   4:3,         (2880)x288p @ 50Hz */
    AMBA_VIDEO_ID_288P4X_WIDE,  /*  16:9,         (2880)x288p @ 50Hz */
    AMBA_VIDEO_ID_576P2X,       /*   4:3,           1440x576p @ 50Hz */
    AMBA_VIDEO_ID_576P2X_WIDE,  /*  16:9,           1440x576p @ 50Hz */
    AMBA_VIDEO_ID_1080P50,      /*  16:9,          1920x1080p @ 50Hz */
    AMBA_VIDEO_ID_1080P24,      /*  16:9,          1920x1080p @ 23.98/24Hz */
    AMBA_VIDEO_ID_1080P25,      /*  16:9,          1920x1080p @ 25Hz */
    AMBA_VIDEO_ID_1080P30,      /*  16:9,          1920x1080p @ 29.97/30Hz */
    AMBA_VIDEO_ID_480P4X,       /*   4:3,         (2880)x480p @ 59.94/60Hz */
    AMBA_VIDEO_ID_480P4X_WIDE,  /*  16:9,         (2880)x480p @ 59.94/60Hz */
    AMBA_VIDEO_ID_576P4X,       /*   4:3,         (2880)x576p @ 50Hz */
    AMBA_VIDEO_ID_576P4X_WIDE,  /*  16:9,         (2880)x576p @ 50Hz */
    AMBA_VIDEO_ID_1080I25_AS,   /*  16:9,          1920x1080i @ 50Hz (V-Total 1250) */
    AMBA_VIDEO_ID_1080I50,      /*  16:9,          1920x1080i @ 100Hz */
    AMBA_VIDEO_ID_720P100,      /*  16:9,           1280x720p @ 100Hz */
    AMBA_VIDEO_ID_576P100,      /*   4:3,            720x576p @ 100Hz */
    AMBA_VIDEO_ID_576P100_WIDE, /*  16:9,            720x576p @ 100Hz */
    AMBA_VIDEO_ID_576I50,       /*   4:3,      720(1440)x576i @ 100Hz */
    AMBA_VIDEO_ID_576I50_WIDE,  /*  16:9,      720(1440)x576i @ 100Hz */
    AMBA_VIDEO_ID_1080I60,      /*  16:9,          1920x1080i @ 119.88/120Hz */
    AMBA_VIDEO_ID_720P120,      /*  16:9,           1280x720p @ 119.88/120Hz */
    AMBA_VIDEO_ID_480P119,      /*   4:3,            720x480p @ 119.88/120Hz */
    AMBA_VIDEO_ID_480P119_WIDE, /*  16:9,            720x480p @ 119.88/120Hz */
    AMBA_VIDEO_ID_480I59,       /*   4:3,      720(1440)x480i @ 119.88/120Hz */
    AMBA_VIDEO_ID_480I59_WIDE,  /*  16:9,      720(1440)x480i @ 119.88/120Hz */
    AMBA_VIDEO_ID_576P200,      /*   4:3,            720x576p @ 200Hz */
    AMBA_VIDEO_ID_576P200_WIDE, /*  16:9,            720x576p @ 200Hz */
    AMBA_VIDEO_ID_576I100,      /*   4:3,      720(1440)x576i @ 200Hz */
    AMBA_VIDEO_ID_576I100_WIDE, /*  16:9,      720(1440)x576i @ 200Hz */
    AMBA_VIDEO_ID_480P239,      /*   4:3,            720x480p @ 239.76/240Hz */
    AMBA_VIDEO_ID_480P239_WIDE, /*  16:9,            720x480p @ 239.76/240Hz */
    AMBA_VIDEO_ID_480I119,      /*   4:3,      720(1440)x480i @ 239.76/240Hz */
    AMBA_VIDEO_ID_480I119_WIDE, /*  16:9,      720(1440)x480i @ 239.76/240Hz */
    AMBA_VIDEO_ID_720P24,       /*  16:9,           1280x720p @ 23.98/24Hz */
    AMBA_VIDEO_ID_720P25,       /*  16:9,           1280x720p @ 25Hz */
    AMBA_VIDEO_ID_720P30,       /*  16:9,           1280x720p @ 29.97/30Hz */
    AMBA_VIDEO_ID_1080P120,     /*  16:9,          1920x1080p @ 119.88/120Hz */
    AMBA_VIDEO_ID_1080P100,     /*  16:9,          1920x1080p @ 100Hz */

    AMBA_VIDEO_ID_HDMI_EXT = 128,   /* VICs after this offset are all the extended resolution formats */
    AMBA_VIDEO_ID_2160P30,          /*  16:9,          3840x2160p @ 29.97/30Hz */
    AMBA_VIDEO_ID_2160P25,          /*  16:9,          3840x2160p @ 25Hz */
    AMBA_VIDEO_ID_2160P24,          /*  16:9,          3840x2160p @ 23.98/24Hz */
    AMBA_VIDEO_ID_2160P24_SMPTE,    /*  16:9,          4096x2160p @ 24Hz (SMPTE) */

    AMBA_NUM_VIDEO_ID,              /* Total Number of pre-defined video ID codes */

    AMBA_VIDEO_ID_CUSTOM = 32768    /*  User-Defined, i.e. display mode of LCD panel */
} AMBA_CEA861_VIDEO_ID_CODE_e;

typedef enum _AMBA_CEA861_FIELD_RATE_e_ {
    AMBA_VIDEO_FIELD_RATE_UNKNOWN = 0,
    AMBA_VIDEO_FIELD_RATE_24HZ,     /* 24 or 24/1.001 Hz */
    AMBA_VIDEO_FIELD_RATE_25HZ,     /* 25 Hz */
    AMBA_VIDEO_FIELD_RATE_30HZ,     /* 30 or 30/1.001 Hz */
    AMBA_VIDEO_FIELD_RATE_50HZ,     /* 50 Hz */
    AMBA_VIDEO_FIELD_RATE_60HZ,     /* 60 or 60/1.001 Hz */
    AMBA_VIDEO_FIELD_RATE_100HZ,    /* 100 Hz */
    AMBA_VIDEO_FIELD_RATE_120HZ,    /* 120 or 120/1.001 Hz */
    AMBA_VIDEO_FIELD_RATE_200HZ,    /* 200 Hz */
    AMBA_VIDEO_FIELD_RATE_240HZ,    /* 240 or 240/1.001 Hz */
} AMBA_CEA861_FIELD_RATE_e;

/*-----------------------------------------------------------------------------------------------*\
 * Video Timing Structure
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_VIDEO_SYNC_TYPE_e_ {
    AMBA_VIDEO_SYNC_EMBEDDED = 0,           /* BT.656, embedded sync mode */
    AMBA_VIDEO_SYNC_H_POSITIVE_V_POSITIVE,  /* BT.601, Positive H-sync pulse, Positive V-sync pulse */
    AMBA_VIDEO_SYNC_H_POSITIVE_V_NEGATIVE,  /* BT.601, Positive H-sync pulse, Negative V-sync pulse */
    AMBA_VIDEO_SYNC_H_NEGATIVE_V_POSITIVE,  /* BT.601, Negative H-sync pulse, Positive V-sync pulse */
    AMBA_VIDEO_SYNC_H_NEGATIVE_V_NEGATIVE   /* BT.601, Negative H-sync pulse, Negative V-sync pulse */
} AMBA_VIDEO_SYNC_TYPE_e;

typedef enum _AMBA_VIDEO_SCAN_FORMAT_e_ {
    AMBA_VIDEO_SCAN_PROGRESSIVE = 0,        /* Progressive scan format */
    AMBA_VIDEO_SCAN_INTERLACED              /* Interlaced scan format */
} AMBA_VIDEO_SCAN_FORMAT_e;

typedef struct _AMBA_VIDEO_ID_s_ {
    UINT16  Mode16_9;                       /* Video ID Code for 16:9 aspect ratio */
    UINT16  Mode4_3;                        /* Video ID Code for 4:3 aspect ratio */
} AMBA_VIDEO_ID_s;

typedef struct _AMBA_VIDEO_TIMING_s_ {
    AMBA_VIDEO_ID_s VideoID;                /* Video ID Codes for differenct aspect ratio */
    UINT32  PixelClock;                     /* Unit: KHz */
    UINT16  HSyncFrontPorch;                /* Horizontal synchronization front porch */
    UINT16  HSyncPulseWidth;                /* Horizontal synchronization pulse width */
    UINT16  HSyncBackPorch;                 /* Horizontal synchronization back porch */
    UINT16  VSyncFrontPorch;                /* Vertical synchronization front porch */
    UINT16  VSyncPulseWidth;                /* Vertical synchronization pulse width */
    UINT16  VSyncBackPorch;                 /* Vertical synchronization back porch */
    UINT16  ActivePixels;                   /* Unit: cycles */
    UINT16  ActiveLines;                    /* Unit: lines */
    AMBA_CEA861_FIELD_RATE_e FieldRate;     /* Field rate for informational purposes only */
    AMBA_VIDEO_SYNC_TYPE_e   SyncPolarity;  /* Polarity of H Sync and V Sync */
    AMBA_VIDEO_SCAN_FORMAT_e ScanFormat;    /* Video scanning format */
    UINT8   PixelRepeat;                    /* 1:Pixel data send 2 times */
} AMBA_VIDEO_TIMING_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaVideoTiming.c
\*-----------------------------------------------------------------------------------------------*/
int   AmbaVideoTiming_Tag(AMBA_VIDEO_TIMING_s *pVideoTiming);
void *AmbaVideoTiming_Get(AMBA_CEA861_VIDEO_ID_CODE_e VideoID);

#endif /* _AMBA_VIDEO_TIMING_H_ */
