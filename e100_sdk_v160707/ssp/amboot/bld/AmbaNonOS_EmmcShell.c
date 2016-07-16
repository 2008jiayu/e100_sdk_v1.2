/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNonOS_EmmcShell.c
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

#include "AmbaRTSL_SD.h"
#include "AmbaEMMC_Def.h"
#include "AmbaNonOS_EMMC.h"

#define MAX_CMDLINE_LEN                     1024    /* atag cmdling */
#define COMMAND_MAGIC                       0x436d6420
#define STATE_WHITESPACE (0)
#define STATE_WORD (1)
#define MAX_CMD_ARGS                        24
#define MAX_ARG_LEN                         128
#define MAX_CWD_LEN                         256

#define ERASE_BUF_SIZE     512 * 512             /* 512 sector */
static UINT8 EraseBuf[ERASE_BUF_SIZE];
#define EMMC_BLOCK_SIZE                     0x80000
static UINT8 PtbMetaBuffer[EMMC_BLOCK_SIZE] __POST_ATTRIB_NOINIT__ __POST_ATTRIB_ALIGN__(32);
int AmbaNonOS_EmmcErasePartition(char * cmdline);
int uart_poll(void);

extern const char *AmbaNAND_PartitionName[];

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
        AmbaNonOS_EmmcErasePartition(argv[1]);
        return 0;
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

static void OutputPart(int i, AMBA_NAND_PART_s *pPart)
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

int AmbaNonOS_EmmcShowMeta(void)
{
    int i, RetStatus = 0;
    AMBA_NAND_PART_META_s NandMeta;

    /* Read the meta */
    memset(&NandMeta, 0x0, sizeof(NandMeta));
    RetStatus = AmbaNonOS_EmmcGetMeta(&NandMeta);
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

int AmbaNonOS_EmmcShowPtb(void)
{
    AMBA_NAND_PART_TABLE_s Table;
    int i, rval = 0;

    /* Read the partition table */
    rval = AmbaNonOS_EmmcGetPartTable(&Table);
    if (rval < 0)
        return rval;

    /* Display PTB content */
    for (i = 0; i < HAS_IMG_PARTS; i++) {
        if (i == AMBA_PARTITION_TABLE)
            continue;
        OutputPart(i, &Table.Part[i]);
    }

    return 0;
}

int AmbaNonOS_EmmcEraseAll(void)
{
    int PartId, Blk, RetStatus = 0, i;
    UINT32 NumBlks;
    AMBA_NAND_PART_META_s NandMeta;
    UINT32 Max_Secs = 512, EraseSecs;

    /* Read the meta */
    memset(&NandMeta, 0x0, sizeof(NandMeta));
    RetStatus = AmbaNonOS_EmmcGetMeta(&NandMeta);
    if (RetStatus < 0)
        return RetStatus;
    memset(EraseBuf, 0x0, sizeof(EraseBuf));

    for (PartId = AMBA_PARTITION_BOOTSTRAP; PartId < AMBA_NUM_FW_PARTITION; PartId++) {
        Blk     = NandMeta.PartInfo[PartId].StartBlk;
        NumBlks = NandMeta.PartInfo[PartId].NumBlks;

        if (PartId == AMBA_PARTITION_CALIBRATION_DATA || PartId == AMBA_PARTITION_STORAGE0 ||
            PartId == AMBA_PARTITION_STORAGE1) {
            if (PartId == AMBA_PARTITION_CALIBRATION_DATA)
                AmbaNonOS_UartPrintf("\r\nSkip calibration partition");
            else
                AmbaNonOS_UartPrintf("\r\nSkip storage partition");
            continue;
        }
        for (i = 0; i < NumBlks; i += Max_Secs) {
            if ((NumBlks - i) > Max_Secs)
               EraseSecs = Max_Secs;
            else
               EraseSecs = (NumBlks - i);

            RetStatus = AmbaNonOS_SdWriteSector(SD_HOST_0, EraseBuf, Blk + i, EraseSecs);
            if (RetStatus < 0) {
                AmbaNonOS_UartPrintf("\r\nErase failed!! (%d)", Blk);
            }
        }

    }

    AmbaNonOS_UartPrintf("\r\nErase done!!");

    return RetStatus;
}

int AmbaNonOS_NandEraseStg(void)
{
    AMBA_NAND_PART_META_s NandMeta;
    int Blk, RetStatus = 0;

    /* Read the meta */
    memset(&NandMeta, 0x0, sizeof(NandMeta));
    RetStatus = AmbaNonOS_EmmcGetMeta(&NandMeta);
    if (RetStatus < 0)
        return RetStatus;

    for (Blk = NandMeta.PartInfo[AMBA_PARTITION_STORAGE0].StartBlk; Blk < NandMeta.PartInfo[AMBA_PARTITION_STORAGE0].NumBlks; Blk++) {

        RetStatus = AmbaNonOS_SdEraseSectror(SD_HOST_0, Blk, 1);
        if (RetStatus < 0) {
            AmbaNonOS_UartPrintf("\r\nErase failed!! (%d)", Blk);
        }
    }

    AmbaNonOS_UartPrintf("\r\nErase done!!");

    return RetStatus;
}

int AmbaNonOS_EmmcErasePartition(char *cmdline)
{
    AMBA_NAND_PART_META_s EmmcMeta;
    AMBA_NAND_PART_TABLE_s Table;
    int StartBlk, NumBlks, RetStatus = 0, i;
    int Pid;
    UINT32 Max_Secs = 512, EraseSecs;

    /* Read the meta */
    memset(&EmmcMeta, 0x0, sizeof(EmmcMeta));
    RetStatus = AmbaNonOS_EmmcGetMeta(&EmmcMeta);
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

    memset(EraseBuf, 0x0, sizeof(EraseBuf));

    StartBlk = EmmcMeta.PartInfo[Pid].StartBlk;
    NumBlks  = EmmcMeta.PartInfo[Pid].NumBlks;
    for (i = 0; i < NumBlks; i += Max_Secs) {
         if ((NumBlks - i) > Max_Secs)
             EraseSecs = Max_Secs;
         else
             EraseSecs = (NumBlks - i);

         RetStatus = AmbaNonOS_SdWriteSector(SD_HOST_0, EraseBuf, StartBlk + i, EraseSecs);
         if (RetStatus < 0) {
             AmbaNonOS_UartPrintf("\r\nErase failed!! (%d)", StartBlk);
         }
    }

    RetStatus = AmbaNonOS_EmmcGetPartTable(&Table);
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
    memcpy(PtbMetaBuffer + sizeof(AMBA_NAND_PART_TABLE_s), &EmmcMeta, sizeof(AMBA_NAND_PART_META_s));

    RetStatus = AmbaNonOS_EmmcProgBlock(PtbMetaBuffer, sizeof(AMBA_NAND_PART_TABLE_s) + sizeof(AMBA_NAND_PART_META_s),
                                     EmmcMeta.PartInfo[AMBA_PARTITION_TABLE].StartBlk,
                                     EmmcMeta.PartInfo[AMBA_PARTITION_TABLE].NumBlks,
                                     NULL, NULL);
    if (RetStatus < 0)
        return RetStatus;

    AmbaNonOS_UartPrintf("\r\nErase done!!");

    return RetStatus;
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
                AmbaNonOS_EmmcShowPtb();
            } else if (!strcmp("show meta", cmd)) {
                AmbaNonOS_EmmcShowMeta();
            } else if (!strcmp("erase all", cmd)) {
                AmbaNonOS_EmmcEraseAll();
            } else {
                parse_command(cmd);
            }
        }
    }
}

