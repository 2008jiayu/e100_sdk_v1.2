/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaVIN_Def.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for Video Input APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_VIN_DEF_H_
#define _AMBA_VIN_DEF_H_

typedef enum _AMBA_VIN_CHANNEL_e_ {
    AMBA_VIN_CHANNEL0,                          /* VIN Channel-0 */
    AMBA_VIN_CHANNEL1,                          /* VIN Channel-1 */

    AMBA_NUM_VIN_CHANNEL                        /* Number of VIN Channels */
} AMBA_VIN_CHANNEL_e;

typedef enum _AMBA_VIN_INTERFACE_e_ {
    AMBA_VIN_LVDS,                              /* LVDS/SLVS/MLVS */
    AMBA_VIN_MIPI,                              /* MIPI */
    AMBA_VIN_DVP,                               /* Digital Video Port (YUV422) ? DVP -> YUV422 or YUV ? */

    AMBA_NUM_VIN_INTERFACE                      /* Number of VIN Interfaces */
} AMBA_VIN_INTERFACE_e;

typedef struct _AMBA_VIN_MPHY_CONFIG_s_ {
    UINT8   HsSettleTime;                       /* M-PHY HS-SETTLE time */
    UINT8   HsTermTime;                         /* M-PHY HS-TERM time */
    UINT8   ClkSettleTime;                      /* M-PHY CLK-SETTLE time */
    UINT8   ClkTermTime;                        /* M-PHY CLK-TERM time */
    UINT8   ClkMissTime;                        /* M-PHY CLK-MISS time */
    UINT8   RxInitTime;                         /* M-PHY RX-INIT time */
} AMBA_VIN_MPHY_CONFIG_s;

#endif /* _AMBA_VIN_DEF_H_ */
