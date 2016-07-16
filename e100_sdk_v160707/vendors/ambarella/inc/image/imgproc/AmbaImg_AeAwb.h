/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaImg_AeAwb.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Sample AE(Auto Exposure) and AWB(Auto White Balance) algorithms
 *
\*-------------------------------------------------------------------------------------------------------------------*/
#ifndef _AMBA_IMG_AE_AWB_H_
#define _AMBA_IMG_AE_AWB_H_

#define DISABLE            0
#define ENABLE             1


#define WB_UNIT_GAIN            4096
#define MAX_DGAIN               (WB_UNIT_GAIN << 2)
#define DGAIN_STEP              96//64
#define SHIFT_BIT               2

#define WB_DGAIN_UNIT           4096
#define WB_ADJ_UNIT_RATIO       4096


#define WB_Y_VALUE              63
#define AE_Y_VALUE              16383
#define AE_MAX_TILES            96
#define WB_MAX_TILES            (64*64)

/** Global gain table size */
#define GAIN_TABLE_ENTRY        449
#define GAIN_DOUBLE             64
#define GAIN_0DB                448

/** Exposure time table size */
#define EXP_TIME_TABLE_ENTRY    2048
#define EXP_TIME_DOUBLE         128

#define EXP_TIME_1_15_INDEX     884
#define EXP_TIME_1_30_INDEX     1013 //1012
#define EXP_TIME_1_60_INDEX     1141 //1140
#define EXP_TIME_1_120_INDEX    1269 //1268

#define EXP_TIME_1_12_INDEX     850
#define EXP_TIME_1_25_INDEX     978
#define EXP_TIME_1_50_INDEX     1106
#define EXP_TIME_1_100_INDEX    1234

#define AMBA_IDLE               0
#define AMBA_PROCESSING         1
#define AMBA_LOCK               2


#endif  /* _AMBA_AE_AWB_ */
