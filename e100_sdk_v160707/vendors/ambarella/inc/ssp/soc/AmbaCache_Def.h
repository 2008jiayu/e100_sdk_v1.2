/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaCache_Def.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Cache Control
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_CACHE_DEF_H_
#define _AMBA_CACHE_DEF_H_

#define AMBA_CACHE_LINE_SIZE                32
#define AMBA_CACHE_LINE_MASK                (~(AMBA_CACHE_LINE_SIZE - 1))
#define AMBA_CACHE_SINGLE_OPERATION_SIZE    512 /* Split L2 cache operation size into smaller ones for preventing to block IRQ too long */

#endif /* _AMBA_CACHE_DEF_H_ */
