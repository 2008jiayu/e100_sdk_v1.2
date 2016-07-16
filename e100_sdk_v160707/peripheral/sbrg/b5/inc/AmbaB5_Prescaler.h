/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaB5_Prescaler.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for B5 Prescaler Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_B5_PRESCALER_H_
#define _AMBA_B5_PRESCALER_H_

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB5_Prescaler.c
\*---------------------------------------------------------------------------*/
int AmbaB5_PrescalerInit(AMBA_B5_CHANNEL_s *pB5Chan, UINT16 InputWidth, UINT16 OutputWidth, UINT16 ReadoutMode);
int AmbaB5_PrescalerSetCoefficients(AMBA_B5_CHIP_ID_u ChipID, UINT16 *CoefAddr);

#endif  /* _AMBA_B5_PRESCALER_H_ */
