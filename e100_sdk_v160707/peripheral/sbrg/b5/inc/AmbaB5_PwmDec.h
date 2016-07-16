/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaB5_PwmDec.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for B5 PWM decoder Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_B5_PWMDEC_H_
#define _AMBA_B5_PWMDEC_H_

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB5_PWMDEC.c
\*---------------------------------------------------------------------------*/
int AmbaB5_PwmDecSetCredit(AMBA_B5_CHIP_ID_u ChipID);
int AmbaB5_PwmDecSetIntStatus(AMBA_B5_CHIP_ID_u ChipID);
int AmbaB5_PwmDecSetIntMask(AMBA_B5_CHIP_ID_u ChipID);


#endif  /* _AMBA_B5_PWMDEC_H_ */
