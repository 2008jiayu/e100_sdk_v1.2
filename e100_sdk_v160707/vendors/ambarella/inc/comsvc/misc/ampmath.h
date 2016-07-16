/**
 *  @file ampmath.h
 *
 *  math header
 *
 *  **History**
 *      |Date       |Name        |Comments       |
 *      |-----------|------------|---------------|
 *      |2013/05/24 |clchan      |Created        |
 *
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 * @note not thread safe, but each handler uses its own memory space
 */
#ifndef __AMPMATH_H__
#define __AMPMATH_H__


/**
 * @addtogroup Utility
 * @{
 */

#include "AmbaDataType.h"

/**
 * compute the GCD of two numbers
 * @param [in] u the 1st number
 * @param [in] v the 2nd number
 * @return the GCD of u and v
 */
extern UINT64 AmpMath_GetGCD(UINT64 u, UINT64 v);

/**
 * compute the LCM of an array of numbers
 * @param [in] v the array of numbers
 * @param [in] count the size of the array
 * @return the LCM of the numbers in the array v
 */
extern UINT64 AmpMath_GetLCM(UINT64 *v, UINT32 count);

/**
 * @}
 */

#endif

