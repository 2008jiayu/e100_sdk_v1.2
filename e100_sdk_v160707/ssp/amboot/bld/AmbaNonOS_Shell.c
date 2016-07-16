/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNonOS_Shell.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: ThreadX additional core entry.
 *
 *  @History        ::
 *      Date        Name        Comments
 *      06/01/2013  Evan Chen   Created
 *      11/06/2013  W.Shi       Replaced g_PartStr[] with AmbaNAND_PartitionName[]
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "AmbaDataType.h"
#include "AmbaHwIO.h"
#include "AmbaCortexA9.h"
#include "AmbaSysCtrl.h"

#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_NAND.h"
#include "AmbaRTSL_FIO.h"
#include "AmbaRTSL_DMA.h"
#include "AmbaNAND_PartitionTable.h"
#include "AmbaNonOS_NAND.h"
#include "AmbaNonOS_NandBBT.h"
#include "AmbaNonOS_UART.h"

#define MAX_CMDLINE_LEN                     1024    /* atag cmdling */
#define COMMAND_MAGIC                       0x436d6420
#define STATE_WHITESPACE (0)
#define STATE_WORD (1)
#define MAX_CMD_ARGS                        24
#define MAX_ARG_LEN                         128
#define MAX_CWD_LEN                         256
#define NAND_BLOCK_SIZE                     0x20000
static UINT8 PtbMetaBuffer[NAND_BLOCK_SIZE] __POST_ATTRIB_NOINIT__ __POST_ATTRIB_ALIGN__(32);

int AmbaNonOS_DramVerify(char * cmdline);
int AmbaNonOS_DramVerifyPattern(char *cmdline);
int AmbaNonOS_NandErasePartition(char * cmdline);
int uart_poll(void);

extern const char *AmbaNAND_PartitionName[];
extern UINT32 AmbaNonOS_NandDiagVerify(void);
extern void AmbaNonOS_NandDiagShowBB(void);

typedef int(*cmdfunc_t)(int, char **);

typedef struct cmdlist_s {
    UINT32              magic;
    char                *name;
    char                *help;
    cmdfunc_t           fn;
    struct cmdlist_s    *next;
} cmdlist_t;

cmdlist_t *commands;

static char HelpList[] =
    "show\r\n"
    "erase\r\n"
    "diag\r\n"
    "writew\r\n"
    "readw\r\n"
    "hotboot\r\n";

static char HelpHelp[] =
    "help [command]\r\n"
    "Get help on [command], "
    "or a list of supported commands if a command is omitted.\r\n";

static char HelpShow[] =
    "\r\n"
    "show ptb       - flash partition table\r\n"
    "show meta      - flash meta table\r\n";

static char HelpErase[] =
    "Erase a parition as specified\r\n"
    "erase [bst|ptb|bld|fw|sys|rom|dsp|linux|rfx|hiber|stg1|stg2|vri|pref|cal|all]\r\n"
    "      [all]  means full chip.\r\n";

static char HelpDiag[] =
    "Do Device diag\r\n"
    "diag ddr [rotate|pattern] [test loop count(0 for infinite loop)] [Dram Size]\r\n"
    "diag nand showbb\r\n"
    "diag nand verify all\r\n";

static char HelpHotboot[] =
    "hotboot [pattern]\r\n"
    "This command is equavalent to boot() with hotboot pattern set\r\n";

static void AmbaNonOS_CmdHelp(char *cmdline)
{
    if (cmdline == NULL) {
        AmbaNonOS_UartPrintf(HelpHelp, strlen(HelpHelp));
        AmbaNonOS_UartPrintf("The following commands are supported:\r\n");
        AmbaNonOS_UartPrintf(HelpList, strlen(HelpList));
        return;
    } else if (!strncmp("show", cmdline, 4)) {
        AmbaNonOS_UartPrintf(HelpShow, strlen(HelpShow));
    } else if (!strncmp("erase", cmdline, 5)) {
        AmbaNonOS_UartPrintf(HelpErase, strlen(HelpErase));
    } else if (!strncmp("diag", cmdline, 4)) {
        AmbaNonOS_UartPrintf(HelpDiag, strlen(HelpDiag));
    } else if (!strncmp("hotboot", cmdline, 7)) {
        AmbaNonOS_UartPrintf(HelpHotboot, strlen(HelpHotboot));
    }
}

static void parse_args(char *cmdline, int *argc, char **argv)
{
    char *c;
    int state = STATE_WHITESPACE;
    int i;

    *argc = 0;

    if (strlen(cmdline) == 0)
        return;

    c = cmdline;
    while (*c != '\0') {
        if (*c == '\t')
            *c = ' ';
        c++;
    }

    c = cmdline;
    i = 0;

    while (*c != '\0') {
        if (state == STATE_WHITESPACE) {
            if (*c != ' ') {
                argv[i] = c;
                i++;
                state = STATE_WORD;
            }
        } else {
            if (*c == ' ') {
                *c = '\0';
                state = STATE_WHITESPACE;
            }
        }

        c++;
    }

    *argc = i;
}

int parse_command(char *cmdline)
{
    int argc;
    UINT32 Tmp1, Tmp2;
    char buf[MAX_CMD_ARGS][MAX_ARG_LEN];
    char *argv[MAX_CMD_ARGS];
    char Cmd[MAX_CMDLINE_LEN];

    for (argc = 0; argc < MAX_CMD_ARGS; argc++)
        argv[argc] = &buf[argc][0];

    parse_args(cmdline, &argc, argv);

    if (argc <= 1) {
        AmbaNonOS_CmdHelp(NULL);
        return 0;
    }

    if (!strcmp("readw", argv[0])) {
        sscanf(argv[1], "%x", &Tmp1);
        AmbaNonOS_UartPrintf("%s: 0x%08x\r\n", argv[1], ReadWord(Tmp1));
        return 0;
    } else if (!strcmp("writew", argv[0])) {
        sscanf(argv[1], "%x", &Tmp1);
        sscanf(argv[2], "%x", &Tmp2);
        WriteWord(Tmp1, Tmp2);
        AmbaNonOS_UartPrintf("%s: 0x%08x\r\n", argv[1], ReadWord(Tmp1));
        return 0;
    } else if (!strcmp("erase", argv[0])) {
        if (!strcmp("cal", argv[1])) {
            int uart_getcmd(char*, int, int);
            AmbaNonOS_UartPrintf("[y|n] : ");
            uart_getcmd(Cmd, sizeof(Cmd), 0);
            if ((strcmp("y", Cmd) != 0) && (strcmp("Y", Cmd) != 0))
                return 0;
        }
        AmbaNonOS_NandErasePartition(argv[1]);
        return 0;
    } else if (!strcmp("diag", argv[0]) && !strcmp("ddr", argv[1])) {
        /* diag ddr rotate 1 0x20000000 */
        int Loop;
        int (*Func)(char * cmdline) = NULL;
        if (!strcmp("rotate", argv[2]))
            Func = AmbaNonOS_DramVerify;
        else if (!strcmp("pattern", argv[2]))
            Func = AmbaNonOS_DramVerifyPattern;

        Loop = strtoul(argv[3], NULL, 10);
        if (Loop == 0) {
            while (1) {
                Func(argv[4]);
            }
        }
        for (; Loop > 0; Loop --)
            Func(argv[4]);
    } else if (!strcmp("hotboot", argv[0])) {
        UINT32 pattern;
        void Boot(void);
        sscanf(argv[1], "%d", &pattern);
        if (pattern < 0) {
            AmbaNonOS_UartPrintf("invalid parameter!\r\n");
            return 0;
        }
        SetHotbootValid(1);
        SetHotbootPattern(pattern);
        Boot();
    }

    AmbaNonOS_CmdHelp(argv[1]);

    return -3;
}

/**
 * Get command from UART.
 */
int uart_getcmd(char *cmd, int n, int timeout)
{
    int c;
    int i;
    int nread;
    int maxread = n - 1;
    UINT32 t_now = 0;

    //timer_reset_count(TIMER1_ID);

    for (nread = 0, i = 0; nread < maxread;) {
        while (uart_poll() == 0) {
            //t_now = timer_get_count(TIMER1_ID);

            if ((timeout > 0) && (t_now > timeout * 10)) {
                cmd[i++] = '\0';
                return nread;
            }
        }

        c = AmbaNonOS_UartGetChar();
        if (c < 0) {
            cmd[i++] = '\0';
            AmbaNonOS_UartPutChar('\n');
            return c;
        }

        if ((c == '\r') || (c == '\n')) {
            cmd[i++] = '\0';

            AmbaNonOS_UartPrintf("\r\n");
            return nread;
        } else if (c == 0x08) {
            if (i > 0) {
                i--;
                nread--;
                AmbaNonOS_UartPrintf("\b \b");
            }
        } else {
            cmd[i++] = c;
            nread++;

            AmbaNonOS_UartPutChar(c);
        }
    }

    return nread;
}

static void NandOutputPart(int i, AMBA_NAND_PART_s *pPart)
{
    if ((pPart->ImgLen == 0) || (pPart->ImgLen == 0xffffffff))
        return;

    if (pPart->Magic != PART_MAGIC_NUM) {
        AmbaNonOS_UartPrintf("%d : partition appears damaged...\r\n", i);
        return;
    }

    AmbaNonOS_UartPrintf("%-26s:\tcrc32: 0x%08x\t", AmbaNAND_PartitionName[i], pPart->Crc32);
    AmbaNonOS_UartPrintf("mem_addr: 0x%08x\r\n", pPart->MemAddr);
}

int AmbaNonOS_NandLoader(UINT32 PartID)
{
    AMBA_NAND_HOST_s *pHost;
    AMBA_NAND_DEV_s *pDev;
    AMBA_NAND_PART_TABLE_s Table;
    AMBA_NAND_PART_META_s NandMeta;
    int RetStatus = 0;
    UINT32 TargetAddr, StartBlk, NumBlks;
    int LoadedSize = 0;

    /* Read the partition table */
    memset(&Table, 0x0, sizeof(Table));
    RetStatus = AmbaNonOS_NandGetPartTable(&Table);
    if (RetStatus < 0)
        return RetStatus;

    /* Read the meta */
    memset(&NandMeta, 0x0, sizeof(NandMeta));
    RetStatus = AmbaNonOS_NandGetMeta(&NandMeta);
    if (RetStatus < 0)
        return RetStatus;

    pHost = AmbaRTSL_NandGetHost();
    pDev = &pHost->Device;

    TargetAddr  = Table.Part[PartID].MemAddr;
    StartBlk    = NandMeta.PartInfo[PartID].StartBlk;
    NumBlks     = NandMeta.PartInfo[PartID].NumBlks;

    while (NumBlks > 0) {
        if (AmbaNonOS_NandBbtIsBadBlock(StartBlk)) {
            StartBlk++;
            continue;
        }

        RetStatus = AmbaNonOS_NandRead(StartBlk, 0, pDev->pNandInfo->PagesPerBlock,
                                       (UINT8 *) TargetAddr, NULL, NAND_MAIN_ECC);

        LoadedSize += pDev->DevLogicInfo.MainSize * pDev->pNandInfo->PagesPerBlock;
        TargetAddr += pDev->DevLogicInfo.MainSize * pDev->pNandInfo->PagesPerBlock;
        NumBlks--;
        StartBlk++;

        if (LoadedSize >= Table.Part[PartID].ImgLen) {
            AmbaNonOS_UartPrintf("\r\n\n%-26s loaded done!!\r\n", NandMeta.PartInfo[PartID].Name);
            break;
        } else if (NumBlks == 0) {
            AmbaNonOS_UartPrintf("\r\n\n%-26s Image length is too large!!\r\n", NandMeta.PartInfo[PartID].Name);
        }
    }

    return RetStatus;
}

int AmbaNonOS_NandShowMeta(void)
{
    int i, RetStatus = 0;
    AMBA_NAND_PART_META_s NandMeta;

    /* Read the meta */
    memset(&NandMeta, 0x0, sizeof(NandMeta));
    RetStatus = AmbaNonOS_NandGetMeta(&NandMeta);
    if (RetStatus < 0)
        return RetStatus;

    /* Display META content */
    for (i = 0; i < AMBA_NUM_FW_PARTITION; i++) {
        AmbaNonOS_UartPrintf("\r\n%-26s:\tsblk: %4d\tnblks: %4d", NandMeta.PartInfo[i].Name,
                             NandMeta.PartInfo[i].StartBlk, NandMeta.PartInfo[i].NumBlks);
    }

    AmbaNonOS_UartPrintf("\r\n\ncrc32:\t0x%08x\r\n", NandMeta.Crc32);

    return RetStatus;
}

int AmbaNonOS_NandShowPtb(void)
{
    AMBA_NAND_PART_TABLE_s Table;
    int i, rval = 0;

    /* Read the partition table */
    rval = AmbaNonOS_NandGetPartTable(&Table);
    if (rval < 0)
        return rval;

    /* Display PTB content */
    for (i = 0; i < HAS_IMG_PARTS; i++) {
        if (i == AMBA_PARTITION_TABLE)
            continue;
        NandOutputPart(i, &Table.Part[i]);
    }

    return 0;
}

int AmbaNonOS_NandEraseAll(void)
{
    int i, PartId, Blk, RetStatus = 0;
    UINT32 NumBlks;
    AMBA_NAND_PART_META_s NandMeta;

    /* Read the meta */
    memset(&NandMeta, 0x0, sizeof(NandMeta));
    RetStatus = AmbaNonOS_NandGetMeta(&NandMeta);
    if (RetStatus < 0)
        return RetStatus;

    for (PartId = AMBA_PARTITION_BOOTSTRAP; PartId < AMBA_NUM_FW_PARTITION; PartId++) {
        Blk     = NandMeta.PartInfo[PartId].StartBlk;
        NumBlks = NandMeta.PartInfo[PartId].NumBlks;

        if (PartId == AMBA_PARTITION_CALIBRATION_DATA) {
            AmbaNonOS_UartPrintf("\r\nSkip calibration partition");
            continue;
        }

        for (i = 0; i < NumBlks; i++, Blk++) {
            RetStatus = AmbaNonOS_NandBbtIsBadBlock(Blk);
            if (RetStatus & NAND_INIT_BAD_BLOCK) {
                AmbaNonOS_UartPrintf("\r\nSkip initial bad block (%d)", Blk);
                continue;
            }

            RetStatus = AmbaNonOS_NandErase(Blk);
            if (RetStatus < 0) {
                AmbaNonOS_UartPrintf("\r\nErase failed!! (%d)", Blk);
            }
        }
    }

    AmbaNonOS_NandBbtErase();

    AmbaNonOS_UartPrintf("\r\nErase done!!");

    return RetStatus;
}

int AmbaNonOS_NandEraseStg(void)
{
    AMBA_NAND_PART_META_s NandMeta;
    int Blk, RetStatus = 0;

    /* Read the meta */
    memset(&NandMeta, 0x0, sizeof(NandMeta));
    RetStatus = AmbaNonOS_NandGetMeta(&NandMeta);
    if (RetStatus < 0)
        return RetStatus;

    for (Blk = NandMeta.PartInfo[AMBA_PARTITION_STORAGE0].StartBlk; Blk < NandMeta.PartInfo[AMBA_PARTITION_STORAGE0].NumBlks; Blk++) {
        RetStatus = AmbaNonOS_NandBbtIsBadBlock(Blk);
        if (RetStatus & NAND_INIT_BAD_BLOCK) {
            AmbaNonOS_UartPrintf("\r\nSkip initial bad block (%d)", Blk);
            continue;
        }

        RetStatus = AmbaNonOS_NandErase(Blk);
        if (RetStatus < 0) {
            AmbaNonOS_UartPrintf("\r\nErase failed!! (%d)", Blk);
        }
    }

    AmbaNonOS_UartPrintf("\r\nErase done!!");

    return RetStatus;
}

int AmbaNonOS_NandForceEraseAll(void)
{
    AMBA_NAND_HOST_s *pHost;
    AMBA_NAND_DEV_s *pDev;
    int Blk, RetStatus = 0;
    UINT32 NumBlks;

    pHost = AmbaRTSL_NandGetHost();
    pDev = &pHost->Device;

    NumBlks = pDev->DevLogicInfo.TotalBlocks;

    /* Display META content */
    for (Blk = 0; Blk < NumBlks; Blk++) {

        RetStatus = AmbaNonOS_NandErase(Blk);
        if (RetStatus < 0) {
            AmbaNonOS_UartPrintf("\r\nErase failed!! (%d)", Blk);
        }
    }

    AmbaNonOS_UartPrintf("\r\nErase done!!");

    return RetStatus;
}

int AmbaNonOS_NandErasePartition(char *cmdline)
{
    AMBA_NAND_PART_META_s NandMeta;
    AMBA_NAND_PART_TABLE_s Table;
    int Blk, StartBlk, NumBlks, RetStatus = 0;
    int Pid;

    /* Read the meta */
    memset(&NandMeta, 0x0, sizeof(NandMeta));
    RetStatus = AmbaNonOS_NandGetMeta(&NandMeta);
    if (RetStatus < 0)
        return RetStatus;

    Pid = (!strcmp("bst",   cmdline)) ?  AMBA_PARTITION_BOOTSTRAP :
          (!strcmp("ptb",   cmdline)) ?  AMBA_PARTITION_TABLE :
          (!strcmp("bld",   cmdline)) ?  AMBA_PARTITION_BOOTLOADER :
          (!strcmp("fw",    cmdline)) ?  AMBA_PARTITION_SD_FIRMWARE_UPDATE :
          (!strcmp("sys",   cmdline)) ?  AMBA_PARTITION_SYS_SOFTWARE :
          (!strcmp("dsp",   cmdline)) ?  AMBA_PARTITION_DSP_uCODE :
          (!strcmp("rom",   cmdline)) ?  AMBA_PARTITION_SYS_ROM_DATA :
          (!strcmp("linux", cmdline)) ?  AMBA_PARTITION_LINUX_KERNEL :
          (!strcmp("rfx",   cmdline)) ?  AMBA_PARTITION_LINUX_ROOT_FS :
          (!strcmp("hiber", cmdline)) ?  AMBA_PARTITION_LINUX_HIBERNATION_IMAGE :
          (!strcmp("stg1",  cmdline)) ?  AMBA_PARTITION_STORAGE0 :
          (!strcmp("stg2",  cmdline)) ?  AMBA_PARTITION_STORAGE1 :
          (!strcmp("vri",   cmdline)) ?  AMBA_PARTITION_VIDEO_REC_INDEX :
          (!strcmp("pref",  cmdline)) ?  AMBA_PARTITION_USER_SETTING :
          (!strcmp("cal",   cmdline)) ?  AMBA_PARTITION_CALIBRATION_DATA : -1;

    if (Pid < 0)
        return -1;

    StartBlk = NandMeta.PartInfo[Pid].StartBlk;
    NumBlks  = NandMeta.PartInfo[Pid].NumBlks;

    for (Blk = StartBlk; Blk < StartBlk + NumBlks; Blk++) {

        RetStatus = AmbaNonOS_NandBbtIsBadBlock(Blk);
        if (RetStatus & NAND_INIT_BAD_BLOCK) {
            AmbaNonOS_UartPrintf("\r\nSkip initial bad block (%d)", Blk);
            continue;
        }

        RetStatus = AmbaNonOS_NandErase(Blk);
        if (RetStatus < 0) {
            AmbaNonOS_UartPrintf("\r\nErase failed!! (%d)", Blk);
        }
    }

    RetStatus = AmbaNonOS_NandGetPartTable(&Table);
    if (RetStatus < 0)
        return RetStatus;

    Table.Part[Pid].Crc32   = 0xffffffff;
    Table.Part[Pid].Flag    = 0xffffffff;
    Table.Part[Pid].ImgLen  = 0xffffffff;
    Table.Part[Pid].Magic   = 0xffffffff;
    Table.Part[Pid].MemAddr = 0xffffffff;
    Table.Part[Pid].VerDate = 0xffffffff;
    Table.Part[Pid].VerNum  = 0xffffffff;

    memcpy(PtbMetaBuffer, &Table, sizeof(AMBA_NAND_PART_TABLE_s));
    memcpy(PtbMetaBuffer + sizeof(AMBA_NAND_PART_TABLE_s), &NandMeta, sizeof(AMBA_NAND_PART_META_s));

    RetStatus = AmbaNonOS_NandProgBlock(PtbMetaBuffer, sizeof(AMBA_NAND_PART_TABLE_s) + sizeof(AMBA_NAND_PART_META_s),
                                     NandMeta.PartInfo[AMBA_PARTITION_TABLE].StartBlk,
                                     NandMeta.PartInfo[AMBA_PARTITION_TABLE].NumBlks,
                                     NULL, NULL);

    AmbaNonOS_UartPrintf("\r\nErase done!!");

    return RetStatus;
}

#if 0
static int OutputProgress(int Percentage, void *pArg)
{
    char PrintBuf[64];
    snprintf(PrintBuf, sizeof(PrintBuf), "\rprogress: %d", Percentage);
    AmbaNonOS_UartPrintf(PrintBuf, sizeof(PrintBuf));

    return 0;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaNonOS_DramVerify
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *-----------------------------------------------------------------------------------------------*/
#define DRAM_TEST_PATTERN_1 0x55555555
#define DRAM_TEST_PATTERN_2 0xAAAAAAAA
#define DRAM_TEST_PATTERN_3 0x5555AAAA

int AmbaNonOS_DramVerifyPattern(char *cmdline)
{
#if 0
    extern UINT32 __ICFEDIT_region_RAM_end__;
    UINT32 TestSize, TestStartAddr, Addr;
    UINT32 *DramAddr;
    UINT32 DramSize = strtoul(cmdline, NULL, 16);
    int Percentage = 0, PercentageTemp = 0;

    TestStartAddr = (UINT32)&__ICFEDIT_region_RAM_end__ + 1;
    TestSize      = DramSize - (UINT32)&__ICFEDIT_region_RAM_end__ - 1;

    memset((void *)TestStartAddr, DRAM_TEST_PATTERN_1, TestSize);
    for (Addr = TestStartAddr; Addr < DramSize; Addr += 4) {
        DramAddr = (UINT32 *)Addr;
        if ((*DramAddr) != DRAM_TEST_PATTERN_1) {
            AmbaNonOS_UartPrintf("\r\n Pattern1 0x%x, fail on 0x%x(0x%x)", DRAM_TEST_PATTERN_1, Addr);
        }

        if (Addr >= DramSize) {
            Percentage = 100;
            OutputProgress(Percentage, NULL);
        } else {
            Percentage = (Addr - TestStartAddr) / ((DramSize - TestStartAddr) / 100);
            if (Percentage > PercentageTemp) {
                OutputProgress(Percentage, NULL);
                PercentageTemp = Percentage;
            }
        }
    }
    AmbaNonOS_UartPrintf("done pattern 1, 0x%x\r\n", DRAM_TEST_PATTERN_1);
    Percentage = PercentageTemp = 0;

    memset((void *)TestStartAddr, DRAM_TEST_PATTERN_2, TestSize);
    for (Addr = TestStartAddr; Addr < DramSize; Addr += 4) {
        DramAddr = (UINT32 *)Addr;
        if ((*DramAddr) != DRAM_TEST_PATTERN_2) {
            AmbaNonOS_UartPrintf("\r\n Pattern1 0x%x, fail on 0x%x(0x%x)", DRAM_TEST_PATTERN_2, Addr);
        }

        if (Addr >= DramSize) {
            Percentage = 100;
            OutputProgress(Percentage, NULL);
        } else {
            Percentage = (Addr - TestStartAddr) / ((DramSize - TestStartAddr) / 100);
            if (Percentage > PercentageTemp) {
                OutputProgress(Percentage, NULL);
                PercentageTemp = Percentage;
            }
        }
    }
    AmbaNonOS_UartPrintf("done pattern 2, 0x%x\r\n", DRAM_TEST_PATTERN_2);
    Percentage = PercentageTemp = 0;

    for (Addr = TestStartAddr; Addr < DramSize; Addr += 4) {
        DramAddr = (UINT32 *)Addr;
        (*DramAddr) = DRAM_TEST_PATTERN_3;
    }
    for (Addr = TestStartAddr; Addr < DramSize; Addr += 4) {
        DramAddr = (UINT32 *)Addr;
        if ((*DramAddr) != DRAM_TEST_PATTERN_3) {
            AmbaNonOS_UartPrintf("\r\n Pattern1 0x%x, fail on 0x%x(0x%x)", DRAM_TEST_PATTERN_3, Addr);
        }

        if (Addr >= DramSize) {
            Percentage = 100;
            OutputProgress(Percentage, NULL);
        } else {
            Percentage = (Addr - TestStartAddr) / ((DramSize - TestStartAddr) / 100);
            if (Percentage > PercentageTemp) {
                OutputProgress(Percentage, NULL);
                PercentageTemp = Percentage;
            }
        }
    }
    AmbaNonOS_UartPrintf("done pattern 3, 0x%x\r\n", DRAM_TEST_PATTERN_3);
    Percentage = PercentageTemp = 0;
#endif
    return OK;
}

int AmbaNonOS_DramVerify(char *cmdline)
{
#if 0
    extern UINT32 __ICFEDIT_region_RAM_end__;
    UINT32 TestStartAddr, Addr, TotalError = 0;
    UINT32 *DramAddr;
    UINT32 DramSize = strtoul(cmdline, NULL, 16);
    UINT32 Pattern = 0, Shift, ShiftCnt, testpattern, ShiftCntBase;
    int LoopCnt, Percentage = 0, PercentageTemp = 0;

    TestStartAddr = (UINT32)&__ICFEDIT_region_RAM_end__ + 1;

    for (Addr = TestStartAddr; Addr < DramSize; Addr += 4) {

        testpattern = 0;
        ShiftCntBase = 32;
        for (LoopCnt = 31; LoopCnt >= 0; LoopCnt--) {
            ShiftCnt = ShiftCntBase;
            for (Shift = 0;  ShiftCnt > 0; ShiftCnt--, Shift++) {
                Pattern = testpattern + (1 << Shift);
                /* compare */
                DramAddr = (UINT32 *)Addr;
                (*DramAddr) = Pattern;
                if ((*DramAddr) != Pattern) {
                    TotalError += 1;
                    AmbaNonOS_UartPrintf("\r\n Pattern: 0x%x, fail on 0x%x(0x%x), error count: %d", Pattern, Addr, (*DramAddr), TotalError);
                }
            }
            ShiftCntBase --;
            testpattern = testpattern + (1 << LoopCnt);
        }

        if (Addr >= DramSize) {
            Percentage = 100;
            OutputProgress(Percentage, NULL);
        } else {
            Percentage = (Addr - TestStartAddr) / ((DramSize - TestStartAddr) / 100);
            if (Percentage > PercentageTemp) {
                OutputProgress(Percentage, NULL);
                PercentageTemp = Percentage;
            }
        }
    }

    AmbaNonOS_UartPrintf("done\r\n");
#endif
    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: main
 *
 *  @Description:: main entry
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *-----------------------------------------------------------------------------------------------*/
void AmbaNonOS_Shell(void)
{
    int i, len;
    char cmd[MAX_CMDLINE_LEN];

    AmbaNonOS_UartPrintf("\x1b[4l"); /* Set terminal to replacement mode */
    AmbaNonOS_UartPrintf("\r\n");    /* First, output a blank line to UART */

    /************************/
    /* Endless command loop */
    /************************/
    for (i = 0; ; i++) {
        AmbaNonOS_UartPrintf("\r\nAmbaTools> ");
        len = uart_getcmd(cmd, sizeof(cmd), 0);
        if (len > 0) {
            if (!strcmp("boot", cmd)) {
                void Boot(void);
                Boot();
            } else if (!strcmp("show ptb", cmd)) {
                AmbaNonOS_NandShowPtb();
            } else if (!strcmp("show meta", cmd)) {
                AmbaNonOS_NandShowMeta();
            } else if (!strcmp("erase all", cmd)) {
                AmbaNonOS_NandEraseAll();
            } else if (!strcmp("erase force", cmd)) {
                AmbaNonOS_NandForceEraseAll();
            } else if (!strcmp("diag nand verify all", cmd)) {
                AmbaNonOS_NandDiagVerify();
            } else if (!strcmp("diag nand showbb", cmd)) {
                AmbaNonOS_NandDiagShowBB();
            } else {
                parse_command(cmd);
            }
        }
    }
}
