 /**
  * @file inc/mw/common/common.h
  *
  * Amba SDK common header
  *
  * Copyright (C) 2013, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef _COMMON_H_
#define _COMMON_H_


//typedef int AMP_ER;

typedef int BOOL;

typedef UINT8 BOOL8;

//typedef wchar_t WCHAR;


/**
 *  Aspect ratio
 */
#define VAR_ANY         0
#define VAR_1x1         (( 1 << 8) | 1)
#define VAR_4x3         (( 4 << 8) | 3)
#define VAR_3x4         (( 3 << 8) | 4)
#define VAR_3x2         (( 3 << 8) | 2)
#define VAR_2x3         (( 2 << 8) | 3)
#define VAR_15x9        ((15 << 8) | 9)
#define VAR_9x15        (( 9 << 8) |15)
#define VAR_16x9        ((16 << 8) | 9)
#define VAR_9x16        (( 9 << 8) |16)
#define VAR_32x9        ((32 << 8) | 9)
#define VAR_9x32        (( 9 << 8) |32)

#define GET_VAR_X(v)    ((v >> 8) & 0xFF)
#define GET_VAR_Y(v)    (v & 0xFF)


/**
 * Macro
 */
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MAX2(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MIN2(a,b) ((a)<(b)?(a):(b))
#define MAX3(a,b,c) (MAX((a),MAX(b,c)))
#define MIN3(a,b,c) (MIN(MIN(a,b),c))
#define ALIGN_128(x)  (((x) + 127) & 0xFFFFFF80)
#define ALIGN_64(x) (((x) + 63) & 0xFFFFFFC0)
#define ALIGN_32(x) (((x) + 31) & 0xFFFFFFE0)
#define ALIGN_16(x) (((x) + 15) & 0xFFFFFFF0)
#define ALIGN_8(x)  (((x) + 7) & 0xFFFFFFF8)
#define ALIGN_4(x)  (((x) + 3) & 0xFFFFFFFC)
#define ALIGN_2(x)  (((x) + 1) & 0xFFFFFFFE)
#define CLIP(a, max, min) ((a) > (max)) ? (max) : (((a) < (min)) ? (min) : (a))
#define TRUNCATE_16(x)  ((x) & 0xFFFFFFF0)
#define TRUNCATE_64(x)  ((x) & 0xFFFFFFC0)

#define    AMP_TASK_NAME_LEN    32

/**
 * Error code
 */
typedef enum _AMP_ER_CODE_e_ {
    AMP_OK = 0,                            ///< Execution OK
    AMP_ERROR_GENERAL_ERROR = -1,                        ///< General error
    AMP_ERROR_INCORRECT_PARAM_STRUCTURE = -2,    ///< Incorrect structure used
    AMP_ERROR_INCORRECT_PARAM_VALUE_RANGE = -3,  ///< Incorrect value range
    AMP_ERROR_OUT_OF_MEMORY = -4,                 ///< Out of memory
    AMP_ERROR_RESOURCE_INVALID = -5,              ///< Resource for the operation
    AMP_ERROR_FIFO_TYPE_MISMATCH = -6, ///< Incorrect fifo type
    AMP_ERROR_FIFO_LOCKED = -7,        ///< Try to read/write a locked fifo
    AMP_ERROR_FIFO_EMPTY = -8, ///< No entry in the fifo
    AMP_ERROR_FIFO_FULL = -9,  ///< Fifo full
    AMP_ERROR_ILLEGAL_OPERATION = -10,             ///< Illegal operation
    AMP_ERROR_ILLEGAL_CONTAIN_SOURCE = -11,         ///< Illegal container soure
    AMP_ERROR_IO_ERROR = -12,         ///< Stream io error
    AMP_ERROR_OUT_OF_STORAGE = -13,
    AMP_ERROR_OPERATION_ABORTED = -14,
    AMP_ERROR_OBJ_ALREADY_EXISTS =  -15,    ///< An object has already existed.
    AMP_ERROR_OBJ_UNAVAILABLE =     -16,    ///< A request object is unavailable.
    AMP_ERROR_OBJ_CREATION_FAILED = -17     ///< Failed to create an object.
} AMP_ER_CODE_e;

/**
 * amba callback
 */
typedef int (*AMP_CALLBACK_f)(void* hdlr, UINT32 event,
                               void* info);

typedef struct _AMP_TASK_INFO_s_{
    UINT32  Priority; ///< task priority
    UINT32  StackSize; ///< task stack size
    UINT32  CoreSelection; ///< Core selection bitmap. Bit[0] = 1 means core #0 is selected
} AMP_TASK_INFO_s;

/**
 * color format
 */
typedef enum _AMP_COLOR_FORMAT_e_ {
    AMP_YUV_420,     ///< color format yuv420
    AMP_YUV_422      ///< color format yuv422
} AMP_COLOR_FORMAT_e;

/**
 * rotation
 */
typedef enum _AMP_ROTATION_e_ {
    AMP_ROTATE_0 = 0,              /**< No rotation */
    AMP_ROTATE_0_HORZ_FLIP,        /**< No rotation and horizontal flip */
    AMP_ROTATE_90,                 /**< Clockwise 90 degree */
    AMP_ROTATE_90_VERT_FLIP,       /**< Clockwise 90 degree and vertical flip*/
    AMP_ROTATE_180,                /**< Clockwise 180 degree */
    AMP_ROTATE_180_HORZ_FLIP,      /**< Clockwise 180 degree and horizontal flip */
    AMP_ROTATE_270,                /**< Clockwise 270 degree */
    AMP_ROTATE_270_VERT_FLIP       /**< Clockwise 270 degree and vertical flip */
} AMP_ROTATION_e;

/**
 * area descriptor
 */
typedef struct _AMP_AREA_s_ {
    UINT32 X;  ///< x offset of the area
    UINT32 Y;  ///< y offset of the area
    UINT32 Width;  ///< width of the area
    UINT32 Height;  ///< height of the area
} AMP_AREA_s;

typedef struct _AMP_2D_BUFFER_s_ {
    UINT8* Buf;
    UINT32 Width;      ///< buffer width
    UINT32 Height;     ///< buffer height
    UINT32 Pitch;      ///< buffer pitch
} AMP_2D_BUFFER_s;

/**
 * yuv buffer
 */
typedef struct _AMP_YUV_BUFFER_s_ {
    AMP_COLOR_FORMAT_e ColorFmt;        ///< color format of the yuv buffer
    UINT32 Width;      ///< buffer width
    UINT32 Height;     ///< buffer height
    UINT32 Pitch;      ///< buffer pitch
    UINT8* LumaAddr;    ///< luma address
    UINT8* ChromaAddr;    ///< chroma address
    AMP_AREA_s AOI;     ///< area of interest
} AMP_YUV_BUFFER_s;

/**
 * raw buffer
 */
typedef struct _AMP_RAW_BUFFER_s_ {
    AMP_2D_BUFFER_s RawBuf; ///< raw buffer
    AMP_AREA_s AOI;     ///< area of interest
} AMP_RAW_BUFFER_s;

/**
 * yuv color
 */
typedef struct _AMP_YUV_COLOR_s_ {
    UINT8 Y;    ///< y part of the color
    UINT8 U;    ///< u part of the color
    UINT8 V;    ///< v part of the color
} AMP_YUV_COLOR_s;

/**
 * System Frequencies
 *
 * @Note Set pre-defined values to auto select by MW, or any custom values.
 */

#define AMP_SYSTEM_FREQ_POWERSAVING 0   ///< MW will choose minimum necessary frequencies to save power
#define AMP_SYSTEM_FREQ_PERFORMANCE 1   ///< MW will choose maximum frequencies to boost performance
#define AMP_SYSTEM_FREQ_KEEPCURRENT 2   ///< MW will keep current freq setting
typedef struct _AMP_SYSTEM_FREQ_s_ {
    UINT16 MaxIdspFreq;        ///< in Mhz
    UINT16 MaxCoreFreq;        ///< in Mhz
    UINT16 MaxArmCortexFreq;   ///< in Mhz
    UINT16 IdspFreq;        ///< in Mhz
    UINT16 CoreFreq;        ///< in Mhz
    UINT16 ArmCortexFreq;   ///< in Mhz
} AMP_SYSTEM_FREQ_s;

#define SET_ZERO(x) memset(&(x), 0, sizeof(x))

#endif

