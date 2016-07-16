/**
 * @file rtos/peripheral/g_sensor/AmbaGSensor.c
 *
 * Initilzation of G Sensor device drivers
 *
 * History:
 *    2015/05/20 - [Yuchi Wei] created file
 *
 * Copyright (C) 2004-2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, meinstical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#include "AmbaDataType.h"
#include "AmbaGSensor.h"

AMBA_G_SENSOR_OBJ_s *pAmbaGSensorObj[AMBA_NUM_G_SENSOR] = {
    [AMBA_G_SENSOR_0] = 0,
};

