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
#include "AmbaCalibInfo.h"

AMBA_CALIB_INFO_OBJ_s *pAmbaCalibInfoObj[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = 0,
};
