/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaRomGen.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Ambarella ROM generator.
 *
 *  @History        ::
 *      Date        Name        Comments
 *      11/15/2012  Kerson      Created
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"

#define _AMBA_ROM_IMPL_
#define _AMBA_HOST_UTIL_
#include "AmbaROM.h"

extern int lineno;
extern FILE *yyin;
extern AMBA_ROM_PARSED_s g_AmbaRomParsed;

extern int yyparse(void);
int yyerror(const char *msg);

/* Alignment to 2K is general from nand and nor */
#define ROMFS_DATA_ALIGN    2048

/*-------------------------------------------------------------------------------------------------------------------*\
 * Program usage.
\*-------------------------------------------------------------------------------------------------------------------*/
static void Usage(int Argc, char **pArgv)
{
    fprintf(stderr, "Usage: %s <infile> <outfile> <partsize>\n", pArgv[0]);
}

/*-------------------------------------------------------------------------------------------------------------------*\
 * Parser error handling.
\*-------------------------------------------------------------------------------------------------------------------*/
int yyerror(const char *msg)
{
    fprintf(stderr, "syntax error: line %d %s\n", lineno, msg);
    return -1;
}

/*-------------------------------------------------------------------------------------------------------------------*\
 * Parser error handling.
\*-------------------------------------------------------------------------------------------------------------------*/
static void FreeMem(void)
{
    AMBA_ROM_LIST_s *pList;
    AMBA_ROM_LIST_s *pUsed;

    for (pList = g_AmbaRomParsed.pList; pList != NULL; pList = pList->pNext) {
        if (pList->pFile)
            free(pList->pFile);
        if (pList->pAlias)
            free(pList->pAlias);
    }

    pList = g_AmbaRomParsed.pList;
    while (pList != NULL) {
        pUsed = pList;
        pList = pList->pNext;
        if (pUsed)
            free(pUsed);
    }

    if (g_AmbaRomParsed.pTop)
        free(g_AmbaRomParsed.pTop);
}

/*-------------------------------------------------------------------------------------------------------------------*\
 * Program entry point.
\*-------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    AMBA_ROM_LIST_s *pList;
    AMBA_ROM_META_s *pMeta;
    struct stat buf;
    int Offset = 0;
    FILE *pIn = NULL, *pOut = NULL;
    char BinDat[1024];
    int FileCnt, Tmp;
    int RetVal;
    unsigned int BinSize;
	unsigned int RomSize = 0;

    if (argc != 4) {
        Usage(argc, argv);
        return -1;
    }

#if 0
	printf("\n"
		   "List File:%s\n"
		   "Partition size:%s\n"
		   "Output:%s\n", argv[1], argv[3], argv[2]);
#endif	

	 if (argv[3][1] == 'x' || argv[3][1] == 'X')
	 	RomSize = strtol(argv[3], NULL, 16);
	 else
	 	RomSize = strtol(argv[3], NULL, 10);

	if (RomSize <= 0) {
        printf("error can't get %s size in info file!\n", argv[4]);
        return -4;
	}

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror(argv[2]);
        return -2;
    }

    if (yyparse() != 0) {
        return -3;
    }

    fclose(yyin);

    pOut = fopen(argv[2], "wb");
    if (pOut == NULL) {
        fprintf(stderr, "%s: error in opening file!\n", argv[2]);
        return -4;
    }

    //printf("%s\n", g_AmbaRomParsed.pTop);
    RetVal = chdir(g_AmbaRomParsed.pTop);
    if (RetVal < 0) {
        perror("chdir");
        return -5;
    }

    Offset = ROM_META_SIZE;

    /* 1st pass: check for file existence and gather file size */
    for (pList = g_AmbaRomParsed.pList; pList; pList = pList->pNext) {
        RetVal = stat(pList->pFile, &buf);
        if (RetVal < 0) {
            perror(pList->pFile);
            return -6;
        }

        if (!S_ISREG(buf.st_mode)) {
            fprintf(stderr, "%s: invalid file!\n", pList->pFile);
            return -7;

        }

        pList->Size = buf.st_size;
        Offset += sizeof(AMBA_ROM_HEADER_s);
    }

    /* padding ROMFS header data to 2K aligned */
    Tmp = Offset;
    Tmp %= ROMFS_DATA_ALIGN;

    if (Tmp > 0)
        Tmp = ROMFS_DATA_ALIGN - Tmp;
    else
        Tmp = 0;

    Offset += Tmp;

    /* 2nd pass: calculate the Offset */
    for (pList = g_AmbaRomParsed.pList; pList; pList = pList->pNext) {
        pList->Offset = Offset;
        pList->Padding = (ROMFS_DATA_ALIGN - (pList->Size % ROMFS_DATA_ALIGN));
        Offset = Offset + pList->Size + pList->Padding;
    }

    /* Check rom size */
    BinSize = Offset;
    if (BinSize > RomSize || BinSize == 0 || RomSize == 0) {
        fclose(pOut);
		remove(argv[2]);
        fprintf(stderr, "\nERR: Partition size %d is not enough for ROMFS %d\n",
                RomSize, BinSize);
        return -10;
    }

    /* 3rd pass: output ROMFS pMeta data */
    FileCnt = 0;

    for (pList = g_AmbaRomParsed.pList; pList; pList = pList->pNext) {
        FileCnt++;
    }

    pMeta = (AMBA_ROM_META_s *) (BinDat);
    pMeta->FileCount = FileCnt;
    pMeta->Magic = ROM_META_MAGIC;

    Tmp = sizeof(AMBA_ROM_META_s) - sizeof(pMeta->Padding);

    RetVal = fwrite(pMeta, 1, Tmp, pOut);
    if (RetVal < 0) {
        perror("fwrite");
        exit(1);
    }

    Tmp = sizeof(pMeta->Padding);

    BinDat[0] = 0xff;
    while (Tmp > 0) {
        RetVal = fwrite(BinDat, 1, 1, pOut);
        if (RetVal < 0) {
            //perror("fwrite");
            exit(1);
        }
        Tmp--;
    }

    Offset = ROM_META_SIZE;

    /* 4rd pass: output ROMFS header data */
    for (pList = g_AmbaRomParsed.pList; pList; pList = pList->pNext) {
        AMBA_ROM_HEADER_s Header;

        memset(&Header, 0xff, sizeof(Header));

        if (pList->pAlias)
            strncpy(Header.Name, pList->pAlias, sizeof(Header.Name));
        else
            strncpy(Header.Name, pList->pFile, sizeof(Header.Name));

        Header.Name[sizeof(Header.Name) - 1] = '\0';
        Header.Offset   = pList->Offset;
        Header.Size     = pList->Size;
        Header.Magic    = ROM_HEADER_MAGIC;

        RetVal = fwrite(&Header, 1, sizeof(Header), pOut);
        if (RetVal < 0) {
            perror("fwrite");
            exit(1);
        }

        Offset += RetVal;
    }

    /* padding ROMFS header data to 2K aligned */
    Tmp = sizeof(AMBA_ROM_HEADER_s) * FileCnt + ROM_META_SIZE;
    Tmp %= ROMFS_DATA_ALIGN;

    if (Tmp > 0)
        Tmp = ROMFS_DATA_ALIGN - Tmp;
    else
        Tmp = 0;

    /* update Offset because maybe padding to header data */
    Offset += Tmp;

    BinDat[0] = 0xff;
    while (Tmp > 0) {
        RetVal = fwrite(BinDat, 1, 1, pOut);
        if (RetVal < 0) {
            perror("fwrite");
            exit(1);
        }
        Tmp--;
    }

    /* 5th pass: output ROMFS binary data */
    for (pList = g_AmbaRomParsed.pList; pList; pList = pList->pNext) {
        if (pList->Offset != Offset) {
            fprintf(stderr, "%s: Offset: %d != %d\n", pList->pFile, pList->Offset, Offset);
        }

        pIn = fopen(pList->pFile, "rb");
        if (pIn == NULL) {
            fprintf(stderr, "%s: error in opening file!\n", pList->pFile);
            exit(2);
        }

        /* output file content */
        do {
            RetVal = fread(BinDat, 1, sizeof(BinDat), pIn);
            if (RetVal < 0) {
                perror("fread");
                exit(3);
            }

            if (RetVal > 0) {
                RetVal = fwrite(BinDat, 1, RetVal, pOut);
                if (RetVal < 0) {
                    perror("fwrite");
                    exit(4);
                }
                Offset += RetVal;
            }
        } while (RetVal > 0);

        fclose(pIn);

        /* output padding */
        BinDat[0] = 0xff;
        while (pList->Padding > 0) {
            RetVal = fwrite(BinDat, 1, 1, pOut);
            if (RetVal < 0) {
                perror("fwrite");
                exit(5);
            }
            Offset += RetVal;
            pList->Padding--;
        }
    }

    fclose(pOut);
    FreeMem();

    return 0;
}

