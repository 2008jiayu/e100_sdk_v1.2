 /**
  * @file inc/mw/utils/synssvc.h
  *
  * ISR sync service header
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

#ifndef _SYNCSVC_H_
#define _SYNCSVC_H_

/**
 * @addtogroup Utility
 * @{
 */

#include <stdio.h>
#include <AmbaDataType.h>
#include <AmbaKAL.h>

/** Service definition */
#define AMP_SYNCSVC_VIN_SOF         0
#define AMP_SYNCSVC_VIN_LASTPIXEL   1
//#define AMP_SYNCSVC_VDSP            2
//#define AMP_SYNCSVC_VCAP            3
#define AMP_SYNCSVC_VOUTDCH         4
#define AMP_SYNCSVC_VOUTFCH         5
#define AMP_SYNCSVC_VIN_VSYNC_EOF   6


#define SYNCSVC_FLAG_VIN0_SOF        1
#define SYNCSVC_FLAG_VIN0_LASTPIXEL (1 << 1)
#define SYNCSVC_FLAG_VIN1_SOF       (1 << 2)
#define SYNCSVC_FLAG_VIN1_LASTPIXEL (1 << 3)
#define SYNCSVC_FLAG_VDSP           (1 << 4)
#define SYNCSVC_FLAG_VCAP           (1 << 5)
#define SYNCSVC_FLAG_VOUT_DCH       (1 << 6)
#define SYNCSVC_FLAG_VOUT_FCH       (1 << 7)
#define SYNCSVC_FLAG_VIN0_VSYNC_EOF (1 << 8)
#define SYNCSVC_FLAG_VIN1_VSYNC_EOF (1 << 9)

/**
 * Init sync service
 *
 * @param [in] resource resource to sync
 * @param [in] cfg parameters
 *
 * @return 0 - OK, others - errnum
 */
extern int AmpSync_Init(UINT8 resource, void *cfg);

/**
 *  Wait Vin SOF syncs
 *
 *  @param [in] channel
 *  @param [in] intCount Number of interrupts
 *  @param [in] timeout Timeout period for each waiting
 *
 *  @return 0 - OK, others - error number
 */
extern int AmpSync_WaitVinSofInt(UINT32 channel, UINT32 intCount, UINT32 timeout);


/**
 *  Wait Vin EOF (last pixel) syncs
 *
 *  @param [in] channel
 *  @param [in] intCount Number of interrupts
 *  @param [in] timeout Timeout period for each waiting
 *
 *  @return 0 - OK, others - error number
 */
extern int AmpSync_WaitVinEofInt(UINT32 channel, UINT32 intCount, UINT32 timeout);

/**
 *  Wait Vin Vsync EOF syncs
 *
 *  @param [in] channel
 *  @param [in] intCount Number of interrupts
 *  @param [in] timeout Timeout period for each waiting
 *
 *  @return 0 - OK, others - error number
 */
extern int AmpSync_WaitVinVsyncEofInt(UINT32 channel, UINT32 intCount, UINT32 timeout);

/**
 *  Get Last Vin SOF System time
 *
 *  @param [in] channel
 *  @param [out] time
 *
 *  @return 0 - OK, others - error number
 */
extern int AmpSync_GetVinSofSystemTime(UINT32 channel, UINT32 *time);

/**
 *  Get Last Vin EOF System time
 *
 *  @param [in] channel
 *  @param [out] time
 *
 *  @return 0 - OK, others - error number
 */
extern int AmpSync_GetVinEofSystemTime(UINT32 channel, UINT32 *time);

/**
 *  Get Last Vin Vsync EOF System time
 *
 *  @param [in] channel
 *  @param [out] time
 *
 *  @return 0 - OK, others - error number
 */
extern int AmpSync_GetVinVsyncEofSystemTime(UINT32 channel, UINT32 *time);

/**
 *  Wait digital channel vout syncs
 *
 *  @param [in] intCount Number of interrupts
 *  @param [in] timeout Timeout period for each waiting
 *
 *  @return 0 - OK, others - error number
 */
extern int AmpSync_WaitDChanVoutInt(UINT32 intCount, UINT32 timeout);

/**
 *  Wait full function channel vout syncs
 *
 *  @param [in] intCount Number of interrupts
 *  @param [in] timeout Timeout period for each waiting
 *
 *  @return 0 - OK, others - error number
 */
extern int AmpSync_WaitFChanVoutInt(UINT32 intCount, UINT32 timeout);


/**
 * @}
 */
#endif
