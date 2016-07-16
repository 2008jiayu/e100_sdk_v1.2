/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIMU.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions for Ambarella IMU (Accelerometer and Gyroscope Devices) driver APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#include "AmbaDataType.h"
#include "AmbaIMU.h"

AMBA_IMU_OBJ_s *pAmbaImuObj[AMBA_NUM_IMU_SENSOR] = {
    [AMBA_IMU_SENSOR0] = 0,
};
