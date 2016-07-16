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

#ifndef _decompress_H_
#define _decompress_H_

/*-----------------------------------------------------------------------------------------------*\
 * Defined in Amba_UnZIP.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 decompress(UINT32 input_start, UINT32 input_end, UINT32 output_start);

#endif  /* _AMBA_UNZIP_H_ */
