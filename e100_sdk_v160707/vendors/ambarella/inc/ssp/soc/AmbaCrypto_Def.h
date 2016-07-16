/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSPI_Def.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Common Definitions & Constants for SPI APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_CRYPTO_DEF_H_
#define _AMBA_CRYPTO_DEF_H_

typedef enum _AMBA_CRYPTO_AES_KEY_SIZE_e_ {
    AMBA_CRYPTO_KEY_SIZE_128_BITS,
    AMBA_CRYPTO_KEY_SIZE_192_BITS,
    AMBA_CRYPTO_KEY_SIZE_256_BITS,
} AMBA_CRYPTO_AES_KEY_SIZE_e;

typedef struct _AMBA_CRYPTO_DES_CTRL_s_ {
    UINT8 *pKey;      /* DES Key (64-bits) */
    UINT8 *pInput;    /* DES Input String (64-bits) */
    UINT8 *pOutput;   /* DES Output String (64-bits) */
} AMBA_CRYPTO_DES_CTRL_s;

typedef struct _AMBA_CRYPTO_AES_CTRL_s_ {
    AMBA_CRYPTO_AES_KEY_SIZE_e KeySize; /* Key size (128/192/256 bits) */
    UINT8 *pKey;                        /* AES key (128/192/256 bits) */
    UINT8 *pInput;                      /* AES Input String (128-bits) */
    UINT8 *pOutput;                     /* AES Output String (128-bits) */
} AMBA_CRYPTO_AES_CTRL_s;

typedef struct _AMBA_CRYPTO_SHA1_MD5_CTRL_s_ {
    UINT32 Size;        /* Input String Size (in byte) */
    UINT8 *pInput;      /* Input String */
    UINT8 *pHash;       /* Output String (Hash) */
} AMBA_CRYPTO_SHA1_MD5_CTRL_s;

#endif /* _AMBA_CRYPTO_DEF_H_ */
