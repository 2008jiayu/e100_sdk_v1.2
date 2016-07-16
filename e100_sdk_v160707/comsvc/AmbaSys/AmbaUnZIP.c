/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaUnZIP.c
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: gnu decompress API.
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include "AmbaDataType.h"
#include "AmbaUnZIP.h"


extern const UINT32 _AmbaCrc32Table[256];
#define updcrc(cp, crc) (_AmbaCrc32Table[((int)crc ^ cp) & 255] ^ (crc >> 8))

#define F_ERROR     1
#define F_VERBOSE   2

static const UINT8 bitReverse[] = {
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
    0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};



/****************************************************************
    Tables for deflate from PKZIP's appnote.txt
 ****************************************************************/

/* Order of the bit length code lengths */
static const UINT8 border[] = {
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
};

/* Copy lengths for literal codes 257..285 */
static const UINT16 cplens[] = {
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
    35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0
};

/* Extra bits for literal codes 257..285 */
static const UINT8 cplext[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
    3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 99, 99
}; /* 99==invalid */

/* Copy offsets for distance codes 0..29 */
static const UINT16 cpdist[] = {
    0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0007, 0x0009, 0x000d,
    0x0011, 0x0019, 0x0021, 0x0031, 0x0041, 0x0061, 0x0081, 0x00c1,
    0x0101, 0x0181, 0x0201, 0x0301, 0x0401, 0x0601, 0x0801, 0x0c01,
    0x1001, 0x1801, 0x2001, 0x3001, 0x4001, 0x6001
};

/* Extra bits for distance codes */
static const UINT8 cpdext[] = {
    0,  0,  0,  0,  1,  1,  2,  2,
    3,  3,  4,  4,  5,  5,  6,  6,
    7,  7,  8,  8,  9,  9, 10, 10,
    11, 11, 12, 12, 13, 13
};


/****************************************************************
    Bit-I/O variables and routines/macros

    These routines work in the bit level because the target
    environment does not have a barrel shifter. Trying to
    handle several bits at once would've only made the code
    slower.

    If the environment supports multi-bit shifts, you should
    write these routines again (see e.g. the GZIP sources).

 ****************************************************************/

static UINT8 bb = 1;

static UINT8 *inBuf;
static UINT32 inOffset = 0;

static UINT8 *outBuf;
static INT32 outOffset = 0;

#define READBYTE()  inBuf[inOffset++]

static void BYTEALIGN(void)
{
    bb = 1;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: UnZIP_READBIT
 *
 *  @Description:: Fetch next 1 bits from input buffer
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/

static INT32 UnZIP_READBIT(void)
{
    INT32 carry = (bb & 1);
    bb >>= 1;

    if (bb == 0) {
        bb = READBYTE();
        carry = (bb & 1);
        bb = (bb >> 1) | 0x80;
    }
    return carry;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: UnZIP_READBITS
 *
 *  @Description:: Fetch next a bits from input buffer
 *
 *  @Input      ::
 *            INT32 a: the fetch size in bit-level
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *            INT32 : following a bits in input buffer
\*-----------------------------------------------------------------------------------------------*/

static INT32 UnZIP_READBITS(INT32 a)
{
    INT32 res = 0, i = a;

    while (i--) {
        res = (res << 1) | UnZIP_READBIT();
    }
    if (a) {
        res = bitReverse[res] >> (8 - a);
    }
    return res;
}


/****************************************************************\
    Output/LZ77 history buffer and related routines/macros
\****************************************************************/

static unsigned long CRC;

#define ADDBUFFER(a) {dCRC=updcrc(a,dCRC);(*co++)=(a); }


/****************************************************************\
    Huffman tree structures, variables and related routines

    These routines are one-bit-at-a-time decode routines. They
    are not as fast as multi-bit routines, but maybe a bit easier
    to understand and use a lot less memory.

    The tree is folded into a table.

\****************************************************************/

struct HufNode_t {
    UINT16 b0;      /* 0-branch value + leaf node flag */
    UINT16 b1;      /* 1-branch value + leaf node flag */
    struct HufNode_t *jump;   /* 1-branch jump address */
};

#define LITERALS 288

static struct HufNode_t literalTree[LITERALS];
static struct HufNode_t distanceTree[32];
static struct HufNode_t *Places = NULL;

static UINT8 len = 0;
static INT32 fpos[17] = {0,};
static INT32 *flens;
static INT16 fmaxi;


INT32 UnZIP_IsPat(void)
{
    while (1) {
        if (fpos[len] >= fmaxi)
            return -1;
        if (flens[fpos[len]] == len)
            return fpos[len]++;
        fpos[len]++;
    }
}

INT32 UnZIP_Rec(void)
{
    struct HufNode_t *curplace = Places;
    INT32 tmp;

    if (len == 17) {
        return -1;
    }
    Places++;
    len++;

    tmp = UnZIP_IsPat();
    if (tmp >= 0) {
        curplace->b0 = tmp; /* leaf cell for 0-bit */
    } else {
        /* Not a Leaf cell */
        curplace->b0 = 0x8000;
        if (UnZIP_Rec())
            return -1;
    }
    tmp = UnZIP_IsPat();
    if (tmp >= 0) {
        curplace->b1 = tmp; /* leaf cell for 1-bit */

    } else {
        /* Not a Leaf cell */
        curplace->b1 = 0x8000;
        curplace->jump = Places;
        if (UnZIP_Rec())
            return -1;
    }
    len--;
    return 0;
}


/* In C64 return the most significant bit in Carry */
INT32 UnZIP_DecodeValue(struct HufNode_t *currentTree)
{
    struct HufNode_t *X = currentTree;

    /* decode one symbol of the data */
    while (1) {
        if (UnZIP_READBIT()) {
            if (!(X->b1 & 0x8000))
                return X->b1;   /* If leaf node, return data */
            X = X->jump;
        } else {
            if (!(X->b0 & 0x8000))
                return X->b0;   /* If leaf node, return data */
            X++;
        }
        if ( X == NULL)
            break;
    }
    return -1;
}

INT32 UnZIP_CreateTree(struct HufNode_t *currentTree,
                       INT32 numval, INT32 *lengths)
{
    INT32 i;
    /* Create the Huffman decode tree/table */
    Places = currentTree;
    flens = lengths;
    fmaxi  = numval;
    for (i = 0; i < 17; i++)
        fpos[i] = 0;
    len = 0;

    if (UnZIP_Rec()) {
        return -1;
    }

    return 0;
}


INT32 UnZIP_InflateLoop(void)
{
    INT32 last, c, type, i;
    register UINT8 *co;
    register UINT32 bj;
    register UINT32 dCRC = (UINT32) 0xffffffffUL;


    co  = &outBuf[outOffset];

    do {

        bj = UnZIP_READBITS(3);
        last    = bj & 0x1;
        type    = bj >> 1;


        if (type == 0) {
            INT32 blockLen, cSum;

            /* Stored */
            BYTEALIGN();
            blockLen = READBYTE();
            blockLen |= (READBYTE() << 8);

            cSum = READBYTE();
            cSum |= (READBYTE() << 8);

            while (blockLen--) {
                c = READBYTE();
                ADDBUFFER(c);
            }
        } else if (type == 1) {
            INT32 j;

            /* Fixed Huffman tables -- fixed decode routine */
            while (1) {

                j = (bitReverse[UnZIP_READBITS(7)] >> 1);
                if (j > 23) {
                    j = (j << 1) | UnZIP_READBIT(); /* 48..255 */

                    if (j > 199) {  /* 200..255 */
                        j -= 128;   /*  72..127 */
                        j = (j << 1) | UnZIP_READBIT();   /* 144..255 << */
                    } else {        /*  48..199 */
                        j -= 48;    /*   0..151 */
                        if (j > 143) {
                            j = j + 136; /* 280..287 << */
                            /*   0..143 << */
                        }
                    }
                } else {    /*   0..23 */
                    j += 256;   /* 256..279 << */
                }
                if (j < 256) {
                    ADDBUFFER(j);

                } else if (j == 256) {
                    /* EOF */
                    break;
                } else {
                    register INT32 len, dist;
                    register UINT8 *co_src;

                    j -= 256 + 1;   /* bytes + EOF */
                    len = UnZIP_READBITS(cplext[j]) + cplens[j];

                    j = bitReverse[UnZIP_READBITS(5)] >> 3;
                    if (cpdext[j] > 8) {
                        dist = UnZIP_READBITS(8);
                        dist |= (UnZIP_READBITS(cpdext[j] - 8) << 8);
                    } else {
                        dist = UnZIP_READBITS(cpdext[j]);
                    }
                    dist += cpdist[j];

                    /*fprintf(stderr, "@%d (l%02x,d%04x)\n", SIZE, len, dist);*/
                    co_src = co - dist;
                    for (j = 0; j < len; j++) {
                        ADDBUFFER((*co_src));
                        co_src++;
                    }
                }
            }
        } else if (type == 2) {
            register INT32 j, n;
            INT32 literalCodes, distCodes, lenCodes;
            static INT32 ll[288 + 32]; /* "static" just to preserve stack */

            /* Dynamic Huffman tables */

            literalCodes = 257 + UnZIP_READBITS(5);
            distCodes = 1 + UnZIP_READBITS(5);
            lenCodes = 4 + UnZIP_READBITS(4);


            /* Get the decode tree code lengths */

            for (j = 0; j < 19; j++) {
                ll[border[j]] = 0;
            }

            for (j = 0; j < lenCodes; j++) {
                ll[border[j]] = UnZIP_READBITS(3);
            }

            if (UnZIP_CreateTree(distanceTree, 19, ll)) {
                return 1;
            }

            /* read in literal and distance code lengths */
            n = literalCodes + distCodes;
            i = 0;
            while (i < n) {
                j = UnZIP_DecodeValue(distanceTree);
                if (j < 16) { /* length of code in bits (0..15) */

                    ll[i++] = j;
                } else if (j == 16) { /* repeat last length 3 to 6 times */
                    int l;

                    j = 3 + UnZIP_READBITS(2);

                    if (i + j > n) {
                        return 1;
                    }

                    l = i ? ll[i - 1] : 0;
                    while (j--) {
                        ll[i++] = l;
                    }
                } else {
                    if (j == 17) {  /* 3 to 10 zero length codes */
                        j = 3 + UnZIP_READBITS(3);
                    } else {        /* j == 18: 11 to 138 zero length codes */
                        j = 11 + UnZIP_READBITS(7);
                    }


                    if (i + j > n) {
                        return 1;
                    }

                    while (j--) {
                        ll[i++] = 0;
                    }
                }
            }

            /* Can overwrite tree decode tree as it is not used anymore */
            if (UnZIP_CreateTree(literalTree, literalCodes, &ll[0])) {
                return 1;
            }

            if (UnZIP_CreateTree(distanceTree, distCodes, &ll[literalCodes])) {
                return 1;
            }


            while (1) {
                j = UnZIP_DecodeValue(literalTree);
                if (j >= 256) {     /* In C64: if carry set */
                    INT32 len, dist;
                    register UINT8 *co_src;

                    j -= 256;
                    if (j == 0) {
                        /* EOF */
                        break;
                    }
                    j--;
                    len = UnZIP_READBITS(cplext[j]) + cplens[j];

                    j = UnZIP_DecodeValue(distanceTree);

                    if (cpdext[j] > 8) {
                        dist = UnZIP_READBITS(8);
                        dist |= (UnZIP_READBITS(cpdext[j] - 8) << 8);
                    } else {
                        dist = UnZIP_READBITS(cpdext[j]);
                    }
                    dist += cpdist[j];

                    while (len--) {
                        co_src = (co - dist);
                        ADDBUFFER((*co_src));
                    }
                } else {
                    ADDBUFFER(j);
                }
            }
        }
    } while (!last);

    BYTEALIGN();

    CRC = dCRC;
    outOffset   = co - &outBuf[0];

    return 0;
}

INT32 UnZIP_GZRead(void)
{

    INT32 flags, tmp[4];
    UINT16  *magic;
    long    size;
    UINT32  crc;

    /* Read mode number , 2 Bytes*/
    magic = (UINT16 *)&inBuf[inOffset];
    inOffset += 2;

    if ((*magic != 0x8b1f) && (*magic != 0x9e1f)) {
        return 20;
    }

    tmp[0] = READBYTE();
    if (tmp[0] != 8) {
        return 20;
    }

    flags = READBYTE();

    /* Skip file modification time (4 bytes) */
    inOffset += 4;
    /* Skip extra flags and operating system fields (2 bytes) */
    inOffset++;
    inOffset++;

    if ((flags & 4)) {
        INT32 len;

        /* Skip extra field */
        len = READBYTE();
        len |= (READBYTE() << 8);
        inOffset += len;
    }

    if ((flags & 8)) {

        while ((READBYTE()));
    }
    if ((flags & 16)) {
        while ((READBYTE() != 0));
    }
    if ((flags & 2)) {
        /* Skip CRC16 */
        return -1;
    }

    CRC = (UINT32) 0xffffffffUL;

    if (UnZIP_InflateLoop())
        return -1;

    crc = READBYTE();
    crc |= (READBYTE() << 8);
    crc |= (READBYTE() << 16);
    crc |= (READBYTE() << 24);

    size = READBYTE();
    size |= (READBYTE() << 8);
    size |= (READBYTE() << 16);
    size |= (READBYTE() << 24);

    if (crc != ~CRC) {
        return -1;
    }

    if (size != outOffset) {
        return -1;
    }

    return 0;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUnZIP
 *
 *  @Description:: Decompress input data to output buffer
 *
 *  @Input      ::
 *          name :          The input data's name
 *          input_start :   Input buffer address
 *          input_end :     Input buffer end address
 *          output_start :  Output buffer address
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/

int AmbaUnZIP(UINT8 *pInputBuf, UINT32 InputSize, UINT8* pOutputBuf, UINT32 *pOutputSize)
{
    /* Reset global variables */
    inOffset    = 0;
    outOffset   = 0;

    inBuf   = (UINT8 *) pInputBuf;
    outBuf  = (UINT8 *) pOutputBuf;

    if ( UnZIP_GZRead() != 0)
        return NG;

    *pOutputSize = outOffset;

    return OK;
}
