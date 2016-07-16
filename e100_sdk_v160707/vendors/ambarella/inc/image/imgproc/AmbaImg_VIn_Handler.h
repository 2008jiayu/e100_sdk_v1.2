/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaImg_Vin_Handler.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Amba Image VDsp Handler Main.
 *
 \*-------------------------------------------------------------------------------------------------------------------*/

#include "AmbaDSP_EventInfo.h"

#ifndef __AMBA_IMG_VIN_HANDLER_H__
#define __AMBA_IMG_VIN_HANDLER_H__

#define IMG_VIN_INVALID  0
#define IMG_VIN_VALID    1
#define IMG_VIN_PRIOR    2
#define IMG_VIN_POST     3
#define IMG_VIN_UNKNOW   255

/*-----------------------------------------------------------------------------------------------*\
 * Defined in ?
\*-----------------------------------------------------------------------------------------------*/
int Amba_Img_VIn_Invalid(void *hdlr, UINT32 *Info);
int Amba_Img_VIn_Valid(void *hdlr, UINT32 *Info);
int Amba_Img_VIn_Changed_Prior(void *hdlr, UINT32 *Info);
int Amba_Img_VIn_Changed_Post(void *hdlr, UINT32 *Info);
int Amba_Img_VIn_Set_Status(UINT8 Status);
int Amba_Img_VIn_Get_Status(UINT8 *Status);

#endif  /* __AMBA_IMG_VIN_HANDLER_H__ */
