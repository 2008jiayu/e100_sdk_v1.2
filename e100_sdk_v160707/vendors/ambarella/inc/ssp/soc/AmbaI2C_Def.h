/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaI2C_Def.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for I2C APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_I2C_DEF_H_
#define _AMBA_I2C_DEF_H_

typedef enum _AMBA_I2C_CHANNEL_e_ {
    AMBA_I2C_CHANNEL0,                  /* I2C Channel-0 */
    AMBA_I2C_CHANNEL1,                  /* I2C Channel-1 */
    AMBA_I2C_CHANNEL2,                  /* I2C Channel-2 */

    AMBA_NUM_I2C_CHANNEL,                       /* Total Number of I2C Channels */

    AMBA_I2C_HDMI_CHANNEL   = AMBA_I2C_CHANNEL1 /*The specified channel is used by HDMI device to skip ACK/NACK check */
} AMBA_I2C_CHANNEL_e;

typedef enum _AMBA_I2C_SPEED_e_ {
    AMBA_I2C_SPEED_STANDARD,            /* I2C Standard speed: 100Kbps */
    AMBA_I2C_SPEED_FAST,                /* I2C Fast speed: 400Kbps */
    AMBA_I2C_SPEED_FAST_PLUS,           /* I2C Fast-mode Plus speed: 1Mbps */
    AMBA_I2C_SPEED_HIGH                 /* I2C High-speed mode: 3.4Mbps */
} AMBA_I2C_SPEED_e;

#define AMBA_I2C_RESTART_FLAG   0x8000  /* To generate a Restart (Repeated Start) condition before sending the data */

#endif /* _AMBA_I2C_DEF_H_ */
