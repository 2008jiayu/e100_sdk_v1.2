 /*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: IoWrapper.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for B5 IO Wrapper Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _IO_WRAPPER_H_
#define _IO_WRAPPER_H_

#include "AmbaSPI_Def.h"
#include "AmbaI2C_Def.h"
#include "AmbaPrint.h"
#include "AmbaPrintk.h"

#define AmbaB5_Print                   AmbaPrint
#define AmbaB5_Print_Flush             AmbaPrintk_Flush

#define AmbaB5_SENSOR_SPI_SLAVE_ID     0
#define AmbaB5_SPI_SENSOR_CHANNEL      AMBA_SPI_MASTER
#define AmbaB5_I2C_CHANNEL             AMBA_I2C_CHANNEL0

/*--------------------------------------------------*\
 * Defined in IoWrapper.c
\*---------------------------------------------------------------------------*/

#endif  /* _IO_WRAPPER_H_ */
