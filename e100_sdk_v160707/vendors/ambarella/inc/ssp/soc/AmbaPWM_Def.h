/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaPWM_Def.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for PWM APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_PWM_DEF_H_
#define _AMBA_PWM_DEF_H_

typedef enum _AMBA_PWM_CHANNEL_e_ {
    AMBA_PWM_CHANNEL0,              /* PWM Channel-0 */
    AMBA_PWM_CHANNEL1,              /* PWM Channel-1 */
    AMBA_PWM_CHANNEL2,              /* PWM Channel-2 */
    AMBA_PWM_CHANNEL3,              /* PWM Channel-3 */
    AMBA_PWM_CHANNELB0,             /* PWM Channel-B0 */
    AMBA_PWM_CHANNELB1,             /* PWM Channel-B1 */
    AMBA_PWM_CHANNELC0,             /* PWM Channel-C0 */
    AMBA_PWM_CHANNELC1,             /* PWM Channel-C1 */

    AMBA_NUM_PWM_CHANNEL            /* Number of PWM Channels */
} AMBA_PWM_CHANNEL_e;

/*-----------------------------------------------------------------------------------------------*\
 * RTSL PWM Parameter Structure
\*-----------------------------------------------------------------------------------------------*/
typedef enum _AMBA_PWM_OPMODE_e_ {
    AMBA_PWM_OPMODE_INDIVIDUAL,     /* PWM signal is configured individually */
    AMBA_PWM_OPMODE_COMPLEMENTARY   /* One PWM signal is a complementary of the configured one */
} AMBA_PWM_OPMODE_e;

typedef struct _AMBA_PWM_CONFIG_s_ {
    UINT16  OnTicks;                /* Output logic high duration ticks */
    UINT16  OffTicks;               /* Output logic low duration ticks */
    UINT32  ClockDivider;           /* Divider for PWM source clock */
    UINT32  PulseDivider;           /* Divider for PWM signal output */
    AMBA_PWM_OPMODE_e   OpMode;     /* PWM Operation Mode */
} AMBA_PWM_CONFIG_s;

#endif /* _AMBA_PWM_DEF_H_ */
