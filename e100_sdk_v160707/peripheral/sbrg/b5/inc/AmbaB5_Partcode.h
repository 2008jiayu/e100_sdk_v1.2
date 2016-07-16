/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaB5_Partcode.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for B5 Partcode Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_B5_PARTCODE_H_
#define _AMBA_B5_PARTCODE_H_

typedef enum _AMBA_B5_PARTCODE_e_ {
    AMBA_B5_PARTCODE_B599 = 0,          /* engineering use */
    AMBA_B5_PARTCODE_B5ND,              /* for 2CH application */
    AMBA_B5_PARTCODE_B5NQ,              /* for 4CH application */
    AMBA_B5_PARTCODE_RESERVED,          /* reversed (do not use) */
} AMBA_B5_PARTCODE_e;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB5_Partcode.c
\*---------------------------------------------------------------------------*/
int AmbaB5_GetPartCode(void);

#endif  /* _AMBA_B5_PARTCODE_H_ */
