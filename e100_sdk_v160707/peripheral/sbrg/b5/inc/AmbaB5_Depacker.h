/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaB5_Depacker.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for B5 Depacker Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_B5_DEPACKER_H_
#define _AMBA_B5_DEPACKER_H_

typedef enum _AMBA_B5_DEPACKER_e_ {
    AMBA_B5_DEPACKER_0,
    AMBA_B5_DEPACKER_1,
    AMBA_B5_DEPACKER_2,
    AMBA_B5_DEPACKER_3
} AMBA_B5_DEPACKER_e;

typedef enum _AMBA_B5_DEPACKER_LANE_NUMBER_e_ {
    AMBA_DEPACKER_1_LANE = 0,
    AMBA_DEPACKER_2_LANES = 1,
    AMBA_DEPACKER_3_LANES = 2
} AMBA_B5_DEPACKER_LANE_NUMBER_e;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB5_Depacker.c
\*---------------------------------------------------------------------------*/
int AmbaB5_DepackerEnable(AMBA_B5_DEPACKER_e DepackerNumber);
int AmbaB5_DepackerSetLane(AMBA_B5_DEPACKER_e DepackerNumber, AMBA_B5_DEPACKER_LANE_NUMBER_e LaneNumber);

#endif  /* _AMBA_B5_DEPACKER_H_ */
