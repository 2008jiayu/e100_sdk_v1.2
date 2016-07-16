/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDSP_ImgLowIsoFilter.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella DSP Image Kernel LISO APIs
 *-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DSP_IMG_LISO_FILTER_H_
#define _AMBA_DSP_IMG_LISO_FILTER_H_

#include "AmbaDSP_ImgDef.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaDSP_ImgLowISO.c
\*-----------------------------------------------------------------------------------------------*/
//int AmbaDSP_ImgLowIsoSetSizeInfo(AMBA_DSP_IMG_MODE_CFG_s Mode, AMBA_DSP_IMG_SIZE_INFO_s *pSizeInfo);
int AmbaDSP_ImgLowIsoDumpCfg(AMBA_DSP_IMG_CFG_INFO_s CfgInfo, char DriveLetter);

#endif  /* _AMBA_DSP_IMG_LISO_FILTER_H_ */
