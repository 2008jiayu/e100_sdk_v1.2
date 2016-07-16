/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaHwIO.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella hardware IO
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_HWIO_H_
#define _AMBA_HWIO_H_

#ifndef __ASM__

#define __raw_writeByte(v, a)       (*(volatile UINT8  *) (a) = (v))
#define __raw_writeHalfWord(v, a)   (*(volatile UINT16 *) (a) = (v))
#define __raw_writeWord(v, a)       (*(volatile UINT32 *) (a) = (v))

#define __raw_readByte(a)           (*(volatile UINT8  *) (a))
#define __raw_readHalfWord(a)       (*(volatile UINT16 *) (a))
#define __raw_readWord(a)           (*(volatile UINT32 *) (a))

#define WriteByte(p, v)             __raw_writeByte(v, p)
#define WriteHalfWord(p, v)         __raw_writeHalfWord(v, p)
#define WriteWord(p, v)             __raw_writeWord(v, p)

#define ReadByte(p)                 __raw_readByte(p)
#define ReadHalfWord(p)             __raw_readHalfWord(p)
#define ReadWord(p)                 __raw_readWord(p)

#endif  /* !__ASM__ */

#endif  /* _AMBA_HWIO_H_ */
