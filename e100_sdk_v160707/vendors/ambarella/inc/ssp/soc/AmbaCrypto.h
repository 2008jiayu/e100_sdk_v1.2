/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSPI.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for SPI Middleware APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_CRYPTO_H_
#define _AMBA_CRYPTO_H_

#include "AmbaCrypto_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaCrypto.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaCrypto_DesEncrypt(AMBA_CRYPTO_DES_CTRL_s *pEncryptCtrl);
int AmbaCrypto_DesDecrypt(AMBA_CRYPTO_DES_CTRL_s *pDecryptCtrl);
int AmbaCrypto_AesEncrypt(AMBA_CRYPTO_AES_CTRL_s *pEncryptCtrl);
int AmbaCrypto_AesDecrypt(AMBA_CRYPTO_AES_CTRL_s *pDecryptCtrl);
int AmbaCrypto_GetMD5(AMBA_CRYPTO_SHA1_MD5_CTRL_s *pMd5Ctrl);
int AmbaCrypto_GetSHA1(AMBA_CRYPTO_SHA1_MD5_CTRL_s *pSha1Ctrl);

#endif /* _AMBA_CRYPTO_H_ */
