/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSensor.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions for Ambarella sensor driver APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#include "AmbaDataType.h"
#include "AmbaYuv.h"

AMBA_YUV_OBJ_s *pAmbaYuvObj[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = 0,
};
