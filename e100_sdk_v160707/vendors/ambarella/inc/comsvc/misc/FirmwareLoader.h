/**
 *  @file FirmwareLoader.h
 *
 *  Format common function header
 *
 *  **History**
 *      |Date       |Name        |Comments       |
 *      |-----------|------------|---------------|
 *      |2014/09/15 |clchan      |Created        |
 *
 *
 *  @copyright 2014 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */

#ifndef __FIRMWARE_LOADER_H__
#define __FIRMWARE_LOADER_H__

#include <AmbaDataType.h>

typedef int BOOL;

/**
 * The firmware region id (start from 1, max: 32)
 */
typedef enum {
    AMP_KERNEL_REGION1 = 1, /**< region 1 contains the essentail objects in quick boot mode */
    AMP_KERNEL_REGION2,     /**< region 2 "usually" contains the 3rd party libraries */
    AMP_KERNEL_REGION3,     /**< region 3 "usually" contains the recorder used objects */
    AMP_KERNEL_REGION4,     /**< region 4 "usually" contains the player used objects */
    AMP_KERNEL_REGION5,     /**< region 5 "usually" contains the rarely used objects */
    AMP_UCODE_REGION_BOOT,  /**< the ucode for booting */
    AMP_UCODE_REGION_LVIEW, /**< the ucode for photo live view */
    AMP_UCODE_REGION_ENC,   /**< the ucode for encode */
    AMP_UCODE_REGION_DEC,   /**< the ucode for decode */
    AMP_FIRMWARE_REGION_MAX /**< the max region */
} AMP_FIRMWARE_REGION_e;

/**
 * Initiate Firmware Loader
 * @param [in] stack the stack of Firmware Loader
 * @param [in] size the size of the stack
 * @return 0 - OK, -1 - failed
 */
int AmpFwLoader_Init(UINT8 *stack, UINT32 size);

/**
 * Wait until a region is completely loaded.
 * @param [in] id the firmware ID (see AMP_KERNEL_REGION_e)
 * @param [in] timeOut the time out interval (ms)
 * @return 0 - OK, -1 - failed
 */
int AmpFwLoader_Wait(UINT32 id, UINT32 timeOut);

/**
 * Wait until all the regions are completely loaded. (will also delete task)
 * @param [in] timeOut the time out interval (ms)
 * @return 0 - OK, -1 - failed
 */
int AmpFwLoader_WaitComplete(UINT32 timeOut);

/**
 * Suspend the Firmware Loader.
 * @return 0 - OK, -1 - failed
 */
int AmpFwLoader_Suspend(void);

/**
 * Resume the Firmware Loader.
 * @return 0 - OK, -1 - failed
 */
int AmpFwLoader_Resume(void);

#endif /* __FIRMWARE_LOADER_H__ */

