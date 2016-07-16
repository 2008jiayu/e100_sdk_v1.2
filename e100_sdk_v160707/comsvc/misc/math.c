/** 
 *  @file Math.c
 *
 *  math implementation
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
#include "ampmath.h"

UINT64 AmpMath_GetGCD(UINT64 u, UINT64 v)
{
    int shift;
    /* GCD(0,x) := x */
    if (u == 0 || v == 0)
        return u | v;
    /* Let shift := lg K, where K is the greatest power of 2 dividing both u and v. */
    for (shift = 0; ((u | v) & 1) == 0; ++shift) {
        u >>= 1;
        v >>= 1;
    }
    while ((u & 1) == 0)
        u >>= 1;
    /* From here on, u is always odd. */
    do {
        while ((v & 1) == 0)    /* Loop X */
            v >>= 1;
        /* Now u and v are both odd, so diff(u, v) is even. Let u = min(u, v), v = diff(u, v)/2. */
        if (u < v) {
            v -= u;
        } else {
            UINT64 diff = u - v;
            u = v;
            v = diff;
        }
        v >>= 1;
    } while (v != 0);
    return (u << shift);
}

/**
 * a helper funciton to compute the LCM
 * @param [in] v the array of numbers
 * @param [in] count the size of the array
 * @return the LCM of the numbers in v
 */
UINT64 AmpMath_GetLCM(UINT64 *v, UINT32 count)
{
    UINT32 i;
    UINT64 l = v[0];
    for (i=1; i<count; i++)
        l = (l * v[i]) / AmpMath_GetGCD(l, v[i]);
    return l;
}

