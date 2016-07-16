/**
 * system/src/bld/decompress.c
 *
 * History:
 *    2005/01/27 - [Charles Chiou] created file
 *    2005/03/09 - [Charles Chiou] cleaned up, indented the code properly
 *			and referenced crc32 externally instead.
 *    2010/07/20 - [Allen Wang] replaced with the source code of BSD license.
 *
 * Copyright (C) 2004-2010, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

/*
    gunzip.c by Pasi Ojala,	a1bert@iki.fi
				http://www.iki.fi/a1bert/

    A hopefully easier to understand guide to GZip
    (deflate) decompression routine than the GZip
    source code.

 */

//#include <ambhw.h>
//#include <bldfunc.h>
#include <stdio.h>
#include <string.h>
#include "AmbaDataType.h"

//#define GUNZIP_DEBUG
//#define GUNZIP_USE_C_ROUTINE

//extern const UINT32 crc32_tab[256];
const UINT32 crc32_tab[] = {
        0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
        0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
        0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
        0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
        0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
        0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
        0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
        0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
        0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
        0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
        0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
        0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
        0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
        0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
        0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
        0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
        0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
        0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
        0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
        0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
        0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
        0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
        0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
        0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
        0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
        0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
        0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
        0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
        0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
        0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
        0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
        0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
        0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
        0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
        0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
        0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
        0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
        0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
        0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
        0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
        0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
        0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
        0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};
#define updcrc(cp, crc) (crc32_tab[((int)crc ^ cp) & 255] ^ (crc >> 8))

UINT32 input_data_start = 0;
UINT32 input_data_end = 0;

static UINT8 *outBuf;
static int outOffset;

#ifdef ENABLE_UNZIP_DEBUG
#define DebugMSG AmbaNonOS_UartPrintf 
#else 
#define DebugMSG(...)
#endif 

//static UINT32 free_mem_ptr;
//static UINT32 free_mem_ptr_end;

/*----------------------------------------------------------------------*/

#define F_ERROR		1
#define F_VERBOSE	2

static const unsigned bitReverse[] = {
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
static const unsigned border[] = {
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

/* Copy lengths for literal codes 257..285 */
static const unsigned cplens[] = {
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
    35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};

/* Extra bits for literal codes 257..285 */
static const unsigned cplext[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
    3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 99, 99}; /* 99==invalid */

/* Copy offsets for distance codes 0..29 */
static const unsigned cpdist[] = {
    0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0007, 0x0009, 0x000d,
    0x0011, 0x0019, 0x0021, 0x0031, 0x0041, 0x0061, 0x0081, 0x00c1,
    0x0101, 0x0181, 0x0201, 0x0301, 0x0401, 0x0601, 0x0801, 0x0c01,
    0x1001, 0x1801, 0x2001, 0x3001, 0x4001, 0x6001};

/* Extra bits for distance codes */
static const unsigned cpdext[] = {
     0,  0,  0,  0,  1,  1,  2,  2,
     3,  3,  4,  4,  5,  5,  6,  6,
     7,  7,  8,  8,  9,  9, 10, 10,
    11, 11, 12, 12, 13, 13};


static void error(char *x)
{
	DebugMSG("\n\n");
	DebugMSG(x);
	DebugMSG("\n\n -- System halted");

	while(1);	/* Halt */
}

/****************************************************************
    Bit-I/O variables and routines/macros

    These routines work in the bit level because the target
    environment does not have a barrel shifter. Trying to
    handle several bits at once would've only made the code
    slower.

    If the environment supports multi-bit shifts, you should
    write these routines again (see e.g. the GZIP sources).

 ****************************************************************/

static UINT32 bb = 0; 	/* Bit buffer */
static unsigned bm = 0; 	/* Bit buffer */

static UINT8 *inBuf;
static unsigned int inOffset = 0;

#define READBYTE()  (*ci++)
#define BYTEALIGN() {ci -= (m >> 3); m = 0;}
#define BITMASK(n)  (0xffffffffUL >> (32 -(n)))
#define READBITS(n) ({				\
	while (m < (n)) {			\
		b |= ((UINT32)READBYTE()) << m;	\
		m += 8;				\
	}					\
	(bj) 	= b & BITMASK(n);		\
	b    >>= (n);				\
	m     -= (n);				\
})
#define READBITS_14BITS() ({			\
	while (m < (14)) {			\
		b |= ((UINT32)READBYTE()) << m;	\
		m += 8;				\
	}					\
	(bj) 	= b & 0x3fff;			\
	b    >>= (14);				\
	m     -= (14);				\
})
#define READBITS_2BITS() ({			\
	while (m < (2)) {			\
		b |= ((UINT32)READBYTE()) << m;	\
		m += 8;				\
	}					\
	(bj) 	= b & 0x3;			\
	b    >>= (2);				\
	m     -= (2);				\
})
#define READBITS_3BITS() ({			\
	while (m < (3)) {			\
		b |= ((UINT32)READBYTE()) << m;	\
		m += 8;				\
	}					\
	(bj) 	= b & 0x7;			\
	b    >>= (3);				\
	m     -= (3);				\
})
#define READBITS_7BITS() ({				\
	while (m < (7)) {			\
		b |= ((UINT32)READBYTE()) << m;	\
		m += 8;				\
	}					\
	(bj) 	= b & 0x7f;			\
	b    >>= (7);				\
	m     -= (7);				\
})
#define PEEKBITS_16BITS() ({			\
	while (m < (16)) {			\
		b |= ((UINT32)READBYTE()) << m;	\
		m += 8;				\
	}					\
	(bj) 	= b & 0xffff;			\
})
#define FLUSHBITS(n) ({				\
	b    	>>= (n);			\
	m    	 -= (n);			\
})

#define READBITS_REV(a) ({READBITS(a);bj=bitReverse[bj]>>(8 - (a));})


/****************************************************************
    Output/LZ77 history buffer and related routines/macros
 ****************************************************************/
static UINT32 CRC;
#define ADDBUFFER(a)  {dCRC=updcrc(a,dCRC); (*co++) =(a);}

/****************************************************************
    Huffman tree structures, variables and related routines

    These routines are one-bit-at-a-time decode routines. They
    are not as fast as multi-bit routines, but maybe a bit easier
    to understand and use a lot less memory.

    The tree is folded into a table.

 ****************************************************************/

struct HufNode {
#define HUFFTREE_NONLEAF	0x8000
    UINT16 b0;		/* 0-branch value + leaf node flag */
    UINT16 b1;		/* 1-branch value + leaf node flag */
    struct HufNode *jump;	/* 1-branch jump address */
};

#define LITERALS 288

static struct HufNode literalTree[LITERALS];
static struct HufNode distanceTree[32];
static struct HufNode *Places = NULL;

static UINT8 len = 0;
static int fpos[17];
static unsigned *flens;
static int fmax;

/* Amba */
#define HUF_TBL_IDX		8
#define HUF_TBL_IDX_MASK	0xff
#define HUF_TBL_SIZE		(1L << HUF_TBL_IDX)
#define HUF_TBL_DATA(x)		((x) & 0x01ff)
#define HUF_TBL_FLUSH_BITS_LOC	12
#define HUF_TBL_FLUSH_BITS(x)	((x) >> HUF_TBL_FLUSH_BITS_LOC)
#define HUFFTBL_NONLEAF		0x0800
#define HUFFTBL_FLUSH_DATA	(HUF_TBL_IDX << HUF_TBL_FLUSH_BITS_LOC) | \
				HUFFTBL_NONLEAF

static UINT16 tbl_huf_distance[HUF_TBL_SIZE] __attribute__ ((aligned(32)));
static UINT16 tbl_huf_literal[HUF_TBL_SIZE] __attribute__ ((aligned(32)));

#define DECVALUE(Tree) 	({					\
		struct HufNode *X = Tree;			\
		l	= 0;					\
		/* decode one symbol of the data */		\
		while (1) {					\
			l++;					\
			if (b & 1) {				\
				if (X->b1 & HUFFTREE_NONLEAF) {	\
					X = X->jump;		\
				} else {			\
				/* If leaf node, return data */	\
					j = (int)X->b1;		\
					break;			\
				}				\
			} else {				\
				if (X->b0 & HUFFTREE_NONLEAF) {	\
					X++;			\
				} else {			\
				/* If leaf node, return data */	\
					j = (int)X->b0;		\
					break;			\
				}				\
			}					\
			b >>= 1;				\
		}						\
		b >>= 1;					\
		m -= l;						\
})

#ifdef GUNZIP_USE_C_ROUTINE

static int FAST_DECVALUE(struct HufNode *tree, UINT16 *tbl)
{
	register int ti, l, j;
	register UINT32 b, bj;
	register unsigned m;
	register UINT8 *ci;

	b = bb;
	m = bm;
	ci   = &inBuf[inOffset];

	PEEKBITS_16BITS();

	/* Check if it's a leaf node */
	ti = bj & HUF_TBL_IDX_MASK;
	if (tbl[ti] & HUFFTBL_NONLEAF) {
		ti = HUF_TBL_DATA(tbl[ti]);
		FLUSHBITS(HUF_TBL_IDX);
		DECVALUE(tree + ti);
	} else {
		j = HUF_TBL_DATA(tbl[ti]);
		l = HUF_TBL_FLUSH_BITS(tbl[ti]);
		FLUSHBITS(l);
	}

	inOffset = ci - &inBuf[0];
	bb = b;
	bm = m;

	return j;
}

static int IsPat(void)
{
    while (1) {
	if (fpos[len] >= fmax)
	    return -1;
	if (flens[fpos[len]] == len)
	    return fpos[len]++;
	fpos[len]++;
    }
}

#else

#define FAST_DECVALUE_SHORT(tbl) {		        \
        int tj;						\
        PEEKBITS_16BITS();				\
        /* Check if it's a leaf node */			\
        ti = bj & HUF_TBL_IDX_MASK;                     \
        tj = tbl[ti];                                   \
        j  = HUF_TBL_DATA(tj);                          \
        ti = HUF_TBL_FLUSH_BITS(tj);                    \
        FLUSHBITS(ti);				        \
}

#define FAST_DECVALUE(tree, tbl) {			\
	int tj;						\
	PEEKBITS_16BITS();				\
	/* Check if it's a leaf node */			\
	ti = bj & HUF_TBL_IDX_MASK;			\
	tj = tbl[ti];					\
	if (tj & HUFFTBL_NONLEAF) {			\
		ti = HUF_TBL_DATA(tj);			\
		FLUSHBITS(HUF_TBL_IDX);			\
		DECVALUE(tree + ti);			\
	} else {					\
		j  = HUF_TBL_DATA(tj);			\
		ti = HUF_TBL_FLUSH_BITS(tj);		\
		FLUSHBITS(ti);				\
	}						\
}

#define IsPat()		{				\
	int pos = fpos[len];				\
	while (1) {					\
		if (pos >= fmax) {			\
			tmp = -1;			\
			break;				\
		}					\
		if (flens[pos] == len) {		\
			tmp = pos++;			\
			break;				\
		}					\
		pos++;					\
	}						\
	fpos[len] = pos;				\
}

#endif


/*
    A recursive routine which creates the Huffman decode tables

    No presorting of code lengths are needed, because a counting
    sort is perfomed on the fly.
 */

/* Maximum recursion depth is equal to the maximum
   Huffman code length, which is 15 in the deflate algorithm.
   (16 in Inflate!) */
static int Rec(void)
{
    struct HufNode *curplace = Places;
    int tmp;

    if(len == 17) {
	return -1;
    }
    Places++;
    len++;

#ifdef GUNZIP_USE_C_ROUTINE
    tmp = IsPat();
#else
    IsPat();
#endif
    if (tmp < 0) {
    	/* Not a Leaf cell */
	curplace->b0 = HUFFTREE_NONLEAF;
	if(Rec())
	    return -1;
    } else {
	curplace->b0 = tmp;	/* leaf cell for 0-bit */
    }

#ifdef GUNZIP_USE_C_ROUTINE
    tmp = IsPat();
#else
    IsPat();
#endif
    if(tmp < 0) {
	/* Not a Leaf cell */
	curplace->b1 = HUFFTREE_NONLEAF;
	curplace->jump = Places;
	if(Rec())
	    return -1;
    } else {
    	curplace->b1 = tmp;	/* leaf cell for 1-bit */
#ifdef GUNZIP_DEBUG
	curplace->jump = NULL;	/* Just for the display routine */
#endif
    }

    len--;

    return 0;
}

/*
    Note:
	The tree create and distance code trees <= 32 entries
	and could be represented with the shorter tree algorithm.
	I.e. use a X/Y-indexed table for each struct member.
 */
int CreateTree(struct HufNode *currentTree,
	       int numval, int *lengths, int curlen)
{
    /* Create the Huffman decode tree/table */
    Places = currentTree;
    flens = (unsigned *) lengths;
    fmax  = numval;

    len = curlen;

    if(Rec()) {
	error("invalid huffman tree");
	return -1;
    }

    return 0;
}

/**
 * Create Lookup table
 */
#define CREATE_HUFFTBL_ENTRIES() {					\
	register UINT16 *tblptr;						\
	ti = bitReverse[(code << (8 - len)) & 0xff]; /* reverse code */	\
	tblptr 	= &huftbl[ti];						\
	/* Lookup table*/						\
	tj  	= i;			/* decoded value */		\
	tj     |= len << HUF_TBL_FLUSH_BITS_LOC;			\
	for (j = 0; j < (1L << (HUF_TBL_IDX - len)); j++) {		\
		*(tblptr + (j << len)) = tj;				\
	}								\
}

/**
 * Create Huffman trees for remaining nodes
 */
#define CREATE_HUFFTBL_EXTTREE() {					\
	/* Get the MS bits for indexing */				\
	ti  	= (code >> (len - HUF_TBL_IDX)) & 0xff;			\
	ti  	= bitReverse[ti]; /* reverse code */			\
	tj 	= huftbl[ti];						\
	/* Check if the root exists */					\
	if (!(tj & HUFFTBL_NONLEAF)) {					\
		/* Huffman tree for lookup */				\
		tj  = Places - huftree;					\
		huftbl[ti] = tj | HUFFTBL_FLUSH_DATA;			\
		/* Build Huffman tree */				\
		CreateTree(Places, lmax, (int *) plen, HUF_TBL_IDX);		\
	}								\
}


static int create_hufftbl(struct HufNode *huftree,
		 	  UINT16 *huftbl, int lmax, unsigned *plen)
{
	int i, j, len;
	int code = 0;
	register int ti;
	register UINT16 tj;
	register int *lptr;

	Places = huftree;

	memset(&fpos[0], 0x0, 17 * sizeof(int));
	memset(huftbl, 0x0, HUF_TBL_SIZE * sizeof(UINT16));

	/* Create lookup table */
	if (lmax > 19) {
		for (len = 1; len <= HUF_TBL_IDX; len++) {
			lptr 	= (int *) &plen[0];
			for (i = 0; i < lmax; i++) {
				if (*lptr++ == len) {
					CREATE_HUFFTBL_ENTRIES();
	    				code++;
				}
			}

			/* If all nodes are visited */
			if (code >> len)
				break;

			code <<= 1; /* Increae the code length */
		}

		for (len = (HUF_TBL_IDX + 1); len < 17; len++) {
			lptr 	= (int *) &plen[0];
			for (i = 0; i < lmax; i++) {
				if (*lptr++ == len) {
					CREATE_HUFFTBL_EXTTREE();
					code++;
				}
			}

			/* If all nodes are visited */
			if (code >> len)
				break;

			code <<= 1; /* Increae the code length */
		}

	} else {
		/* Create lookup table */
		for (len = 1; len < 8; len++) {
			lptr 	= (int *) &plen[0];
			for (i = 0; i < lmax; i++) {
				if (*lptr++ == len) {
					CREATE_HUFFTBL_ENTRIES();
					code++;
				}
			}

			/* If all nodes are visited */
			if (code >> len)
				break;

			code <<= 1; /* Increae the code length */
		}
	}

	return 0;
}

int InflateLoop(void) {
    int last, type;
    register int i;
    register UINT32 b, bj;
    register int l;
    register unsigned m;
    register UINT8 *ci, *co;
    register UINT32 dCRC = (UINT32) 0xffffffffUL;

    b 	= bb;
    m 	= bm;
    co	= &outBuf[outOffset];
    ci  = &inBuf[inOffset];

    do {

	READBITS(3);
	last	= bj & 0x1;
	type  	= bj >> 1;

#ifdef GUNZIP_DEBUG
	switch(type) {
	case 0:
	    putstr("Stored\n");
	    break;
	case 1:
	    putstr("Fixed Huffman codes\n");
	    break;
	case 2:
	    putstr("Dynamic Huffman codes\n");
	    break;
	case 3:
	    putstr("Reserved block type!!\n");
	    break;
	default:
	    putstr("\ntype");
	    puthex(type);
	    putstr("\n");
	    error("Unexpected value\n");
	    break;
        }
#endif

	if(type==0) {
	    int blockLen, cSum;
	    register int c;

	    /* Stored */
	    BYTEALIGN();
	    blockLen = READBYTE();
	    blockLen |= (READBYTE()<<8);

	    cSum = READBYTE();
	    cSum |= (READBYTE()<<8);

	    if(((blockLen ^ ~cSum) & 0xffff)) {
		error("BlockLen checksum mismatch\n");
	    }
	    while(blockLen--) {
		c = READBYTE();
		ADDBUFFER(c);
	    }
	} else if(type==1) {
	    register int j;

	    /* Fixed Huffman tables -- fixed decode routine */
	    while(1) {
		/*
		    256	0000000		0
		    :   :     :
		    279	0010111		23
		    0   00110000	48
		    :	:      :
		    143	10111111	191
		    280 11000000	192
		    :	:      :
		    287 11000111	199
		    144	110010000	400
		    :	:       :
		    255	111111111	511

		    Note the bit order!
		 */
		bj 	= READBITS_REV(7);
		j 	= bj;

		if(j > 23) {
		    READBITS(1);
		    j = (j<<1) | bj;	/* 48..255 */

		    if(j > 199) {	/* 200..255 */
			j -= 128;	/*  72..127 */
			READBITS(1);
			j = (j<<1) | bj;/* 144..255 << */
		    } else {		/*  48..199 */
			j -= 48;	/*   0..151 */
			if(j > 143) {
			    j = j+136;	/* 280..287 << */
					/*   0..143 << */
			}
		    }
		} else {	/*   0..23 */
		    j += 256;	/* 256..279 << */
		}

		if(j < 256) {
		    ADDBUFFER(j);

		} else if(j == 256) {
		    /* EOF */
		    break;
		} else {
		    register int len, dist;
		    register UINT8 *co_src;

		    j -= 256 + 1;	/* bytes + EOF */
		    READBITS(cplext[j]);
		    len = bj + cplens[j];
		    READBITS_REV(5);
		    j = bj;

		    READBITS(cpdext[j]);
		    dist  = bj + cpdist[j];

		    co_src = co - dist;
		    for(j=0;j<len;j++) {
			ADDBUFFER((*co_src));
			co_src++;
		    }
		}
	    }
	} else if (type==2) {
	    register int j, n;
	    int literalCodes, distCodes, lenCodes;
	    static unsigned ll[288+32];	/* "static" just to preserve stack */
	    register int ti;

	    /* Dynamic Huffman tables */
	    READBITS_14BITS();
	    literalCodes	= 257 + (bj & 0x1f);
	    distCodes 		= 1 + ((bj >> 5) & 0x1f);
	    lenCodes 		= 4 + (bj >> 10);

	    /* Get the decode tree code lengths */
	    for(j=0; j<lenCodes; j++) {
	    	READBITS_3BITS();
		ll[border[j]] = bj;
#ifdef GUNZIP_DEBUG
		puthex(ll[border[j]]);
#endif
	    }

	    while (j < 19) {
		ll[border[j++]] = 0;
	    }

#ifdef GUNZIP_DEBUG
	    for(j=0; j<19; j++) {
		puthex(ll[j]);
		putstr("\n");
	    }
#endif
            /* build the lookup table for single level tree */
	    if (create_hufftbl(distanceTree, tbl_huf_distance, 19, ll)) {
	    	return 1;
	    }

#ifdef GUNZIP_DEBUG
putstr("Len tree ok\n");
#endif

	    /* read in literal and distance code lengths */
	    n = literalCodes + distCodes;
	    i = 0;
	while (i < n) {

#ifdef GUNZIP_USE_C_ROUTINE
		bb = b;
		bm = m;
		inOffset = ci - &inBuf[0];
		j = FAST_DECVALUE(distanceTree, tbl_huf_distance);
		ci = &inBuf[inOffset];
		b = bb;
		m = bm;
#else
		FAST_DECVALUE_SHORT(tbl_huf_distance);
#endif

		if(j<16) {	/* length of code in bits (0..15) */
#ifdef GUNZIP_DEBUG
puthex(j);
#endif
		    ll[i++] = j;
		} else if(j==16) {	/* repeat last length 3 to 6 times */
		    READBITS_2BITS();
		    j = 3 + bj;
#ifdef GUNZIP_DEBUG
puthex(j);
		    if(i+j > n) {
			return 1;
		    }
#endif

		    l = i ? ll[i-1] : 0;
		    while (j--) {
			ll[i++] = l;
		    }
		} else {
		    if(j==17) {		/* 3 to 10 zero length codes */
		        READBITS_3BITS();
		        j = 3 + bj;

		    } else {		/* j == 18: 11 to 138 zero length codes */
		        READBITS_7BITS();
			j = 11 + bj;
		    }
#ifdef GUNZIP_DEBUG
puthex(j);
		    if(i+j > n) {
			return 1;
		    }
#endif
	            memset(&ll[i], 0x0, j * sizeof (int));
		    i += j;
		}
	    }


#ifdef GUNZIP_DEBUG
	    for(j=0; j<literalCodes+distCodes; j++) {
		puthex(ll[j]);
		if ((j&7)==7)
		    putstr("\n");
	    }
	    putstr("\n");
#endif
	    /* Can overwrite tree decode tree as it is not used anymore */
	    if (create_hufftbl(literalTree, tbl_huf_literal, literalCodes, ll)) {
		return 1;
	    }
#ifdef GUNZIP_DEBUG
putstr("Literal tree ok\n");
#endif

	    if (create_hufftbl(distanceTree, tbl_huf_distance, distCodes, &ll[literalCodes])) {
		return 1;
	    }

#ifdef GUNZIP_DEBUG
putstr("Distance tree ok\n");
#endif

		while(1) {
#ifdef GUNZIP_USE_C_ROUTINE
		bb = b;
		bm = m;
		inOffset = ci - &inBuf[0];
		j = FAST_DECVALUE(literalTree, tbl_huf_literal);
		ci = &inBuf[inOffset];
		b = bb;
		m = bm;
#else
		FAST_DECVALUE(literalTree, tbl_huf_literal);
#endif
		if (j < 256) {
		    ADDBUFFER(j);
	    	} else if(j > 256) {		/* In C64: if carry set */
		    register int len, dist;
		    register UINT8 *co_src;

		    j  -= (256 + 1);
		    len = cplens[j];
		    ti  = cplext[j];

		    if (ti) {
		    	READBITS(ti);
		    	len += bj;
		    }

/*puthex(len);*/
#ifdef GUNZIP_USE_C_ROUTINE
		bb = b;
		bm = m;
		inOffset = ci - &inBuf[0];
		j = FAST_DECVALUE(distanceTree, tbl_huf_distance);
		ci = &inBuf[inOffset];
		b = bb;
		m = bm;
#else
		FAST_DECVALUE(distanceTree, tbl_huf_distance);
#endif
/*puthex(j);*/
		    READBITS(cpdext[j]);
		    dist = bj + cpdist[j];

		    co_src = (co - dist);
		    while (len--) {
			ADDBUFFER((*co_src));
			co_src++;
		    }

		} else{  /* (j == 0) */
			/* EOF */
			break;
		}
	    }
	}
    } while(!last);

    BYTEALIGN();
    inOffset 	= ci - &inBuf[0];
    outOffset	= co - &outBuf[0];
    CRC 	= dCRC;

    return 0;
}

int GZRead(void) {
    UINT16 	*magic; /* magic header */

    int 	gpflags;
    long 	size;
    UINT32  	crc;
    register UINT8 *ci;

    ci = &inBuf[inOffset];

#if 0
    static const char *flagNames[] = {
	"ASCII", "CRC16", "Extra Field", "Original Name",
	"Comment", "Reserved 5", "Reserved 6", "Reserved 7"};
#endif

    magic	= (UINT16 *)ci;
    ci 	       += 2;
    if ((*magic != 0x8b1f) && (*magic != 0x9e1f)){
	error("Magic number mismatch");
	return 20;
    }

    /* We only support method #8, DEFLATED */
    if (READBYTE() != 8) {
	error("Unknown compression method");
	return 20;
    }

#ifdef GUNZIP_DEBUG
    putstr("Compression method: Deflate\n");
#endif

    gpflags = READBYTE();

#ifdef GUNZIP_DEBUG
    for(i=0;i<8;i++) {
	if((READBYTE() & (1<<i))) {
	    putstr(flagNames[i]);
	}
    }
#endif
    if ((gpflags & ~0x1f)) {
	error("Unknown flags set!\n");
	return -1;
    }

    /* Skip file modification time (4 bytes) */
    /* Skip extra flags and operating system fields (2 bytes) */
    ci += 6;

    if((gpflags & 4)) {
	int len;

	/* Skip extra field */
	len  = READBYTE();
	len |= (READBYTE() << 8);

#ifdef GUNZIP_DEBUG
	putstr("\n\n");
	putstr("extra field size");
	puthex(len);
	putstr("\n\n");
#endif

	/* Skip bytes */
    	ci += len;

    }

    if((gpflags & 8)) {
#ifdef GUNZIP_DEBUG
	DebugMSG("Original file name: ");
#endif
	while(READBYTE() != 0);
    }

    if((gpflags & 16)) {
#ifdef GUNZIP_DEBUG
	DebugMSG("File comment: ");
#endif
	while(READBYTE() != 0);
    }

    if((gpflags & 2)) {
	error("Multi part input");
	return -1;
    }

    CRC = (UINT32) 0xffffffffUL;

    inOffset = ci - &inBuf[0];

    if (InflateLoop())
	return -1;

    ci = &inBuf[inOffset];

    	crc = READBYTE();
    	crc |= (READBYTE()<<8);
    	crc |= (READBYTE()<<16);
    	crc |= (READBYTE()<<24);

#ifdef GUNZIP_DEBUG
    	putstr("\r\n");
	puthex(crc);
	putstr("\r\n");
	puthex(~CRC);
	putstr("\r\n");
#endif

    	size = READBYTE();
    	size |= (READBYTE()<<8);
    	size |= (READBYTE()<<16);
    	size |= (READBYTE()<<24);

#ifdef GUNZIP_DEBUG
    	putstr("\r\n");
	puthex(size);
	putstr("\r\n");
	puthex(outOffset);
	putstr("\r\n");
#endif
    	/* Validate decompression */
	if (crc != ~CRC) {
		error("crc error");
		return -1;
	}

	if (size != outOffset) {
		error("length error");
		return -1;
	}

    return 0;
}

UINT32 decompress(UINT32 input_start, UINT32 input_end, UINT32 output_start)
{
	/* Reset global variables */
	inOffset 	= 0;
	outOffset 	= 0;

	input_data_start	= input_start;
	input_data_end		= input_end;
	//free_mem_ptr		= free_mem_ptr_p;
	//free_mem_ptr_end	= free_mem_ptr_end_p;

	inBuf 	= (UINT8 *) input_data_start;
	outBuf	= (UINT8 *) output_start;

#ifdef GUNZIP_DEBUG
/*        putstr("\r\n");*/
/*        puthex(input_end - input_start);*/
/*        putstr("\r\n");*/
/*        puthex(input_start);*/
/*        putstr("\r\n");*/
#endif
/*        putstr("decompressing ");*/
/*        putstr(name);*/
/*        putstr(" to ");*/
/*        puthex(output_start);*/
/*        putstr(" ...");*/
	GZRead();
/*        putstr(" done\r\n");*/

	return outOffset;
}



