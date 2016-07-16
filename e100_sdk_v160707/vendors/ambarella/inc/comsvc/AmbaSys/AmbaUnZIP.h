/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaUnZIP.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: gnu decompress API header file.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_UNZIP_H_
#define _AMBA_UNZIP_H_

/*-----------------------------------------------------------------------------------------------*\
 * Defined in Amba_UnZIP.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaUnZIP(UINT8 *pInputBuf, UINT32 InputSize, UINT8* pOutputBuf, UINT32 *pOutputSize);

#endif  /* _AMBA_UNZIP_H_ */
