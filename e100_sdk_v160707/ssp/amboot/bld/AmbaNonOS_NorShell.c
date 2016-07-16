/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNonOS_NorShell.c
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

#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_NOR.h"
#include "AmbaRTSL_DMA.h"
#include "AmbaNOR_PartitionTable.h"
#include "AmbaNAND_PartitionTable.h"
#include "AmbaNonOS_NOR.h"
#include "AmbaNonOS_UART.h"

#define MAX_CMDLINE_LEN                     1024    /* atag cmdling */
#define COMMAND_MAGIC                       0x436d6420
#define STATE_WHITESPACE (0)
#define STATE_WORD (1)
#define MAX_CMD_ARGS                        24
#define MAX_ARG_LEN                         128
#define MAX_CWD_LEN                         256

int AmbaNonOS_DramVerify(char * cmdline);
int AmbaNonOS_DramVerifyPattern(char *cmdline);
int AmbaNonOS_NorErasePartition(char * cmdline);
int uart_poll(void);

extern const char *AmbaNOR_PartitionName[];
extern AMBA_NOR_FW_PART_s AmbaNOR_FwPartInfo;

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
    "readw\r\n";

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
    "diag ddr [rotate|pattern] [test loop count(0 for infinite loop)] [Dram Size]"
    "diag nand verify all\r\n\r\n";

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
        AmbaNonOS_NorErasePartition(argv[1]);
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

static void NorOutputPart(int i, AMBA_NOR_PART_s *pPart)
{
    if ((pPart->ImgLen == 0) || (pPart->ImgLen == 0xffffffff))
        return;

    if (pPart->Magic != PART_MAGIC_NUM) {
        AmbaNonOS_UartPrintf("%d : partition appears damaged...\r\n", i);
        return;
    }

    AmbaNonOS_UartPrintf("%-26s:\tcrc32: 0x%08x\t", AmbaNOR_PartitionName[i], pPart->Crc32);
    AmbaNonOS_UartPrintf("mem_addr: 0x%08x\r\n", pPart->MemAddr);
}

int AmbaNonOS_CortexBoot(void)
{
    UINT32 CORTEX_CTRL_REG = 0x6001d004;

    /* Resets Cortex cores */
    WriteWord(CORTEX_CTRL_REG, 0x0000006c);

    while ((ReadWord(CORTEX_CTRL_REG) & 0x00000300) != 0x00000000);

    /* Stop to resets Cortex cores */
    WriteWord(CORTEX_CTRL_REG, 0x0000000c);

    while ((ReadWord(CORTEX_CTRL_REG) & 0x00000360) != 0x00000000);

    return 0;
}

int AmbaNonOS_CortexReset(void)
{
    int i = 0;
    UINT32 CORTEX_CTRL_REG = 0x6001d004;

    /* Resets Cortex cores */
    WriteWord(CORTEX_CTRL_REG, 0x0000006c);

    for (i = 0; i < 0xfffff; i++);

    /* Stop to resets Cortex cores */
    WriteWord(CORTEX_CTRL_REG, 0x0000000c);

    return 0;
}

int AmbaNonOS_NandLoader(UINT32 PartID)
{
    AMBA_NOR_HOST_s *pHost;
    AMBA_NOR_DEV_s *pDev;
    AMBA_NOR_PART_TABLE_s Table;
    int RetStatus = 0;
    UINT32 TargetAddr, StartBlk, NumBlks;
    int LoadedSize = 0;

    /* Read the partition table */
    memset(&Table, 0x0, sizeof(Table));
    RetStatus = AmbaNonOS_NorGetPartTable(&Table);
    if (RetStatus < 0)
        return RetStatus;

    pHost = AmbaRTSL_NorGetHost();
    pDev = &pHost->Device;

    TargetAddr  = Table.Part[PartID].MemAddr;
    StartBlk    = AmbaNOR_FwPartInfo.StartBlk[PartID];
    NumBlks     = AmbaNOR_FwPartInfo.NumBlks[PartID];

    while (NumBlks > 0) {
        RetStatus = AmbaNonOS_NorRead(StartBlk * pDev->DevLogicInfo.EraseBlockSize, pDev->DevLogicInfo.EraseBlockSize,
                                       (UINT8 *) TargetAddr);

        LoadedSize += pDev->DevLogicInfo.EraseBlockSize;
        TargetAddr += pDev->DevLogicInfo.EraseBlockSize;
        NumBlks--;
        StartBlk++;

        if (LoadedSize >= Table.Part[PartID].ImgLen) {
            AmbaNonOS_UartPrintf("\r\n\n%-26s loaded done!!\r\n", AmbaNOR_PartitionName[PartID]);
            break;
        } else if (NumBlks == 0) {
            AmbaNonOS_UartPrintf("\r\n\n%-26s Image length is too large!!\r\n", AmbaNOR_PartitionName[PartID]);
        }
    }

    return RetStatus;
}

int AmbaNonOS_NorShowPtb(void)
{
    AMBA_NOR_PART_TABLE_s Table;
    int i, rval = 0;

    /* Read the partition table */
    rval = AmbaNonOS_NorGetPartTable(&Table);
    if (rval < 0)
        return rval;

    /* Display PTB content */
    for (i = 0; i < HAS_IMG_PARTS; i++) {
        if (i == AMBA_PARTITION_TABLE)
            continue;
        NorOutputPart(i, &Table.Part[i]);
    }

    return 0;
}

int AmbaNonOS_NorShowMeta(void)
{
    int i, RetStatus = 0;
    AMBA_NAND_PART_META_s NandMeta;

    /* Read the meta */
    memset(&NandMeta, 0x0, sizeof(NandMeta));
    RetStatus = AmbaNonOS_NorGetMeta(&NandMeta);
    if (RetStatus < 0)
        return RetStatus;

    /* Display META content */
    for (i = 0; i < AMBA_NUM_FW_PARTITION; i++) {
        AmbaNonOS_UartPrintf("\r\n%-26s:\tsblk: %4d\tnblks: %4d", NandMeta.PartInfo[i].Name,
                             NandMeta.PartInfo[i].StartBlk, NandMeta.PartInfo[i].NumBlks);
    }

    AmbaNonOS_UartPrintf("\r\n\ncrc32:\t0x%08x\r\n", NandMeta.Crc32);
    AmbaNonOS_UartPrintf("\r\n\nFwprogMagic:\t0x%08x\r\n", NandMeta.FwProgMagic);
    return RetStatus;
}
int AmbaNonOS_NorEraseAll(void)
{
    int i, PartId, Blk, RetStatus = 0;
    UINT32 NumBlks;
    AMBA_NOR_DEV_s *pDev = AmbaRTSL_NorGetDev();

    for (PartId = AMBA_PARTITION_BOOTSTRAP; PartId < AMBA_NUM_FW_PARTITION; PartId++) {
        Blk     = AmbaNOR_FwPartInfo.StartBlk[PartId];
        NumBlks = AmbaNOR_FwPartInfo.NumBlks[PartId];

        if (PartId == AMBA_PARTITION_CALIBRATION_DATA) {
            AmbaNonOS_UartPrintf("\r\nSkip calibration partition");
            continue;
        }

        for (i = 0; i < NumBlks; i++, Blk++) {
            RetStatus = AmbaNonOS_NorErase(Blk * pDev->DevLogicInfo.EraseBlockSize,  pDev->DevLogicInfo.EraseBlockSize);
            if (RetStatus < 0) {
                AmbaNonOS_UartPrintf("\r\nErase failed!! (%d)", Blk);
            }
        }
    }
    AmbaNonOS_UartPrintf("\r\nErase done!!");

    return RetStatus;
}

/*int AmbaNonOS_NorEraseStg(void)*/
/*{*/
/*    AMBA_NOR_HOST_s *pHost;*/
/*    AMBA_NOR_DEV_s *pDev;*/
/*    UINT32 Blk, NumBlk;*/
/*    int RetStatus = 0;*/

/*    pHost = AmbaRTSL_NorGetHost();*/
/*    pDev = &pHost->Device;*/

/*    NumBlk = AmbaNOR_FwPartInfo.StartBlk[AMBA_PARTITION_STORAGE0] + AmbaNOR_FwPartInfo.NumBlks[AMBA_PARTITION_STORAGE0];*/
/*    for (Blk = AmbaNOR_FwPartInfo.StartBlk[AMBA_PARTITION_STORAGE0]; Blk < NumBlk; Blk++) {*/

/*        RetStatus = AmbaNonOS_NorErase(Blk * pDev->DevLogicInfo.EraseBlockSize,  pDev->DevLogicInfo.EraseBlockSize);*/
/*        if (RetStatus < 0) {*/
/*            AmbaNonOS_UartPrintf("\r\nErase failed!! (%d)", Blk);*/
/*        }*/
/*    }*/

/*    AmbaNonOS_UartPrintf("\r\nErase done!!");*/

/*    return RetStatus;*/
/*}*/

int AmbaNonOS_NorForceEraseAll(void)
{
    AMBA_NOR_HOST_s *pHost;
    AMBA_NOR_DEV_s *pDev;
    int RetStatus = 0;
    UINT32 TotalSize;

    pHost = AmbaRTSL_NorGetHost();
    pDev = &pHost->Device;

    TotalSize = pDev->DevLogicInfo.TotalByteSize;

    /* Display META content */
    RetStatus = AmbaNonOS_NorErase(0, TotalSize);
    if (RetStatus < 0) {
        AmbaNonOS_UartPrintf("\r\n Force Erase failed!!");
        return RetStatus;
    }

    AmbaNonOS_UartPrintf("\r\nErase done!!");

    return RetStatus;
}

int AmbaNonOS_NorErasePartition(char *cmdline)
{
    AMBA_NOR_HOST_s *pHost;
    AMBA_NOR_DEV_s *pDev;
    AMBA_NOR_PART_TABLE_s Table;
    int Blk, StartBlk, NumBlks, RetStatus = 0;
    int Pid;

    pHost = AmbaRTSL_NorGetHost();
    pDev = &pHost->Device;

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

    StartBlk = AmbaNOR_FwPartInfo.StartBlk[Pid];
    NumBlks  = AmbaNOR_FwPartInfo.NumBlks[Pid];

    for (Blk = StartBlk; Blk < StartBlk + NumBlks; Blk++) {

        RetStatus = AmbaNonOS_NorErase(Blk * pDev->DevLogicInfo.EraseBlockSize,
            pDev->DevLogicInfo.EraseBlockSize);
        if (RetStatus < 0) {
            AmbaNonOS_UartPrintf("\r\nErase failed!! (%d)", Blk);
        }
    }

    RetStatus = AmbaNonOS_NorGetPartTable(&Table);
    if (RetStatus < 0)
        return RetStatus;

    Table.Part[Pid].Crc32   = 0xffffffff;
    Table.Part[Pid].Flag    = 0xffffffff;
    Table.Part[Pid].ImgLen  = 0xffffffff;
    Table.Part[Pid].Magic   = 0xffffffff;
    Table.Part[Pid].MemAddr = 0xffffffff;
    Table.Part[Pid].VerDate = 0xffffffff;
    Table.Part[Pid].VerNum  = 0xffffffff;

    RetStatus = AmbaNonOS_NorSetPartTable(&Table);
    if (RetStatus < 0)
        return RetStatus;

    AmbaNonOS_UartPrintf("\r\nErase done!!");

    return RetStatus;
}

/*static int OutputProgress(int Percentage, void *pArg)*/
/*{*/
/*    char PrintBuf[64];*/
/*    snprintf(PrintBuf, sizeof(PrintBuf), "\rprogress: %d", Percentage);*/
/*    AmbaNonOS_UartPrintf(PrintBuf, sizeof(PrintBuf));*/

/*    return 0;*/
/*}*/

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
                AmbaNonOS_NorShowPtb();
            } else if (!strcmp("erase all", cmd)) {
                AmbaNonOS_NorEraseAll();
            } else if (!strcmp("erase force", cmd)) {
                AmbaNonOS_NorForceEraseAll();
            } else if (!strcmp("show meta", cmd)) {
                AmbaNonOS_NorShowMeta();
	   //} else if (!strcmp("diag nor verify all", cmd)) {
                //AmbaNonOS_NandDiagVerify();
	    } else {
                parse_command(cmd);
            }
        }
    }
}
