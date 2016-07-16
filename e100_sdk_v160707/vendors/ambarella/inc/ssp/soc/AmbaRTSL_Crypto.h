/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRTSL_SPI.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for SPI control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_RTSL_CRYPTO_H_
#define _AMBA_RTSL_CRYPTO_H_

#include "AmbaCrypto_Def.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaRTSL_Crypto.c
\*-----------------------------------------------------------------------------------------------*/
int AmbaRTSL_CryptoDesEncrypt(AMBA_CRYPTO_DES_CTRL_s* pEncryptCtrl);
int AmbaRTSL_CryptoDesDecrypt(AMBA_CRYPTO_DES_CTRL_s* pDecryptCtrl);
int AmbaRTSL_CryptoAesEncrypt(AMBA_CRYPTO_AES_CTRL_s* pEncryptCtrl);
int AmbaRTSL_CryptoAesDecrypt(AMBA_CRYPTO_AES_CTRL_s* pDecryptCtrl);
int AmbaRTSL_CryptoSha1Process(UINT8* pInitHash, UINT8* pInput, UINT8* pHash);
int AmbaRTSL_CryptoMd5Process(UINT8* pInitHash, UINT8* pInput, UINT8* pHash);

#endif /* _AMBA_RTSL_CRYPTO_H_ */
