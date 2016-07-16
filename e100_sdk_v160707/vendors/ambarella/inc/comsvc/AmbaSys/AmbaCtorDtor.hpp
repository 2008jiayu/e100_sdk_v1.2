/**
 *  @file AmbaCtorDtor.hpp
 *  
 *  **History**
 *      |Date       |Name        |Comments       |
 *      |-----------|------------|---------------|
 *      |2015/04/16 |cyweng      |Created        |
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */

/**
 * init ctor dtor using ambakal memory pool
 *
 * @param pMpl [in] memory pool used by c++ new function
 */
extern void AmbaCtorDtor_Init(AMBA_KAL_BYTE_POOL_t *pMpl);

