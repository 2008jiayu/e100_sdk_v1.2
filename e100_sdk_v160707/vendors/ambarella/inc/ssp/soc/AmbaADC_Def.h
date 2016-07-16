/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaADC_Def.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for ADC APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_ADC_DEF_H_
#define _AMBA_ADC_DEF_H_

#define AMBA_ADC_RESOLUTION     4096    /* Total Levels of Quantization */

typedef enum _AMBA_ADC_CHANNEL_e_ {
    AMBA_ADC_CHANNEL0 = 0,              /* ADC Channel-0 */
    AMBA_ADC_CHANNEL1,                  /* ADC Channel-1 */
    AMBA_ADC_CHANNEL2,                  /* ADC Channel-2 */
    AMBA_ADC_CHANNEL3,                  /* ADC Channel-3 */

    AMBA_NUM_ADC_CHANNEL                /* Number of ADC Channels */
} AMBA_ADC_CHANNEL_e;

typedef int (*AMBA_ADC_ISR_HANDLER_f)(UINT32 EventData);

extern UINT16 AmbaAdcData[AMBA_NUM_ADC_CHANNEL];

#endif /* _AMBA_ADC_DEF_H_ */
