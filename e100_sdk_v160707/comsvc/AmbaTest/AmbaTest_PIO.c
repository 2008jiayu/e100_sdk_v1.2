/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaTest_PIO.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Test program for peripheral I/O driver
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#define __TEST_IMPL__
#include "AmbaTest.h"

//#include "AmbaTest_GPIO.c"
//#include "AmbaTest_SPI.c"
//#include "AmbaTest_I2C.c"
//#include "AmbaTest_Timer.c"

void AmbaTest_PioAddCommands(void)
{
    AmbaTest_RegisterCommand("gpio", test_gpio);
    AmbaTest_RegisterCommand("i2c", test_i2c);
    AmbaTest_RegisterCommand("spi", test_spi);
    AmbaTest_RegisterCommand("timer", test_timer);
}
