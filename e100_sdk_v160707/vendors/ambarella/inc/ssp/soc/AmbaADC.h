/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaADC.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for ADC Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_ADC_H_
#define _AMBA_ADC_H_

#include "AmbaADC_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaADC.c
\*-----------------------------------------------------------------------------------------------*/
void AmbaADC_Init(void);
int  AmbaADC_Start(void);
int  AmbaADC_Stop(void);
int  AmbaADC_DataCollection(void);

#endif /* _AMBA_ADC_H_ */
