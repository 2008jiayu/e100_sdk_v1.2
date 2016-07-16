/**
 * @file src/app/connected/applib/src/calibration/ApplibCalibCli.c
 *
 * command line interface for Amba calibration sample code
 *
 * History:
 *    07/10/2013  Allen Chiu Created
 *
 *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (¡°Software¡±) are protected by intellectual property rights
 * including, without limitation, U.S. and/or foreign copyrights.  This Software is also the
 * confidential and proprietary information of Ambarella, Inc. and its licensors.  You may
 * not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed license
 * agreement or nondisclosure agreement with Ambarella, Inc. or its authorized
 * affiliates.	In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-
 * INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR
 * MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <AmbaDataType.h>
#include <AmbaPrintk.h>
#include <AmbaSensor.h>
#include <calibration/ApplibCalibMgr.h>
#include <calibration/ApplibCalibCli.h>
#include <common/common.h>
#include <AmbaUtility.h>
#include <calibration/vig/ApplibCalibVig.h>

#define MAX_CLI_CMD_TOKEN       (128)
#define MAX_CMD_TOKEN_SIZE  (16)
#define CALIB_MODE0  0
#define CALIB_MODE1  1

extern Cal_Obj_s* AppLib_CalibGetObj(UINT32 CalId);

#define CALIB_TASK1_PRIORITY          (75)

char CalibOutFn1[256];
int  CalibSel;

/**
 *  @brief print the calibration result
 *
 *  print the calibration result
 *
 *  @param [in]Rval return value from the calibration result
 *  @param [in]Fmt Output string
 *
 */
static void AppLib_CalibPrint(int Rval, const char *Fmt, ...)
{
    va_list Args;
    char Buf[512];

    va_start(Args, Fmt);
    vsprintf(Buf, Fmt, Args);
    va_end(Args);   // avoid dangling pointer

    AmbaPrint("[CALMGR CLI]%s %s",
              (Rval==0)? "[OK]": "[NG]",Buf);
}


/**
 *  @brief command line interface function for calibraion
 *
 *  command line interface function for calibraion
 *
 *  @param [in]Cmd input command
 *  @param [in]MsgAvailable check the message is available
 *  @param [out]OutputStr debug message for this function
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibCli(int Argc, char *Argv[], char *OutputStr, int *MsgAvailable)
{
    int Rval=-1;
    static int RvalCalFunc = 0;

    int IsProcessed = 0;
    static char CalibOutputStr[256];
    int i;
    int CalId;
    char Buf[16] = {0};
    Cal_Stie_Status_s *PCalSite, CalSite;
    UINT8 *VigAddr;
    Cal_Obj_s *CalObj;
    int SubNum = 1;


    if (Argc == 0) {
        *MsgAvailable = 0;
        return -1;
    }

    // Processing commands
    IsProcessed = 2;
    if (strcmp(Argv[0], "_calib_reset") == 0) {
        RvalCalFunc = 0;
        CalId = atoi(Argv[1]);
        if (Argc == 3 && CalId == CAL_VIGNETTE_ID) {
            if (strcmp(Argv[2], "partial") == 0) {
                sprintf(OutputStr, "Reset calib %d NAND data (partial)", CalId);
                Rval = AppLib_CalibResetPartial(CalId);
            } else {
                sprintf(OutputStr, "Reset calib %d NAND data error!", CalId);
#if (CALIB_STORAGE == CALIB_FROM_NAND)
                Rval = AppLib_CalibNandReset(CalId);
#else //from SD card
                Rval = AppLib_CalibSDCardReset(CalId);
#endif
            }
        } else {
            sprintf(OutputStr, "Reset calib %d NAND data", CalId);
#if (CALIB_STORAGE == CALIB_FROM_NAND)
            Rval = AppLib_CalibNandReset(CalId);
#else //from SD card
            Rval = AppLib_CalibSDCardReset(CalId);
#endif
        }
    } else if (strcmp(Argv[0], "_calib_init") == 0) {
        CalId = atoi(Argv[1]);
        *MsgAvailable = 0;
        Rval = AppLib_CalibInitFunc(CalId,CALIB_LOAD, CALIB_FULL_LOAD);
        if (Rval != OK) {
            AmbaPrint("Load calib %d fail or no calibration data", CalId);
            return Rval;
        }
         Rval |= AppLib_CalibInitFunc(CalId,CALIB_INIT, 0);
        if (Rval != OK) {
            AmbaPrint("Init calib %d fail", CalId);
        }
        return Rval;
    } else if (strcmp(Argv[0], "_partial_init") == 0) {
        UINT8 SubId;
        *MsgAvailable = 0;        
        CalId = atoi(Argv[1]);
        SubId = atoi(Argv[2]); 
        AmbaPrint( "Init calib %d SubId = %d", CalId, SubId);
        sprintf(OutputStr, "Re-init calib %d SubId = %d", CalId, SubId);
        Rval = AppLib_CalibInitFunc(CalId,CALIB_LOAD, SubId);
        if (Rval != OK) {
            AmbaPrint("Load calib %d fail or no calibration data", CalId);
            return Rval;
        }
        Rval |= AppLib_CalibInitFunc(CalId,CALIB_INIT, 0);
        if (Rval != OK) {
            AmbaPrint("Init calib %d fail", CalId);
        }
        return Rval;
    } else if (strcmp(Argv[0], "_calib_save") == 0) {
        SubNum = 1;
        CalId = atoi(Argv[1]);
        CalObj = AppLib_CalibGetObj(CalId);
        VigAddr = CalObj->DramShadow;
        PCalSite = AppLib_CalibGetSiteStatus(CalId);

        if (Argc == 3 && CalId == CAL_VIGNETTE_ID) {
            if (strcmp(Argv[2], "partial") == 0) {
                PCalSite = AppLib_CalibGetSiteStatus(CalId);
                PCalSite->Status = CAL_SITE_DONE_VIGNETTE_LOAD_PARTIAL;
                PCalSite->Reserved[0] = VigAddr[CAL_VIGNETTE_ENABLE]; //vignette enable: 1 disable:0
                PCalSite->Reserved[1] = VigAddr[CAL_VIGNETTE_TABLE_COUNT]; // table count
                SubNum = VigAddr[CAL_VIGNETTE_TABLE_COUNT];
            }
        }
        sprintf(OutputStr, "Save calib %d to NAND", CalId);
        for (i=0; i<SubNum; i++) {
            if (Argc == 3 && CalId == CAL_VIGNETTE_ID) {
                if (strcmp(Argv[2], "partial") == 0) {
                    PCalSite->Reserved[i+2] = CAL_VIGNETTE_SUB_SITE_DONE;
                }
            }
            //bug
#if (CALIB_STORAGE == CALIB_FROM_NAND)
            AppLib_CalibNandSave(CalId,i);
#else //from sd card
            //save to SD card
            AppLib_CalibSDCardSave(CalId,i);
#endif
        }
        Rval = 0 ;
    } else if (strcmp(Argv[0], "_nand_init") == 0) {
        CalId = atoi(Argv[1]);
#if (CALIB_STORAGE == CALIB_FROM_NAND)
        sprintf(OutputStr, "_nand_save");
        Rval = AppLib_CalibNandInit();
#endif
    } else if (strcmp(Argv[0], "_nand_to_sd") == 0) {
        CalId = atoi(Argv[1]);
#if (CALIB_STORAGE == CALIB_FROM_NAND)//load from Nand Flash
        sprintf(OutputStr, "Load calib %d from NAND", CalId);
        Rval = AppLib_CalibNandLoad(CalId);
        if (Rval != OK) {
            return Rval;
        }
        Rval = AppLib_CalibSDCardSave(CalId,0);
        if (Rval != OK) {
            return Rval;
        }
#endif
    } else if (strcmp(Argv[0], "_sd_to_nand") == 0) {
        CalId = atoi(Argv[1]);
#if (CALIB_STORAGE == CALIB_FROM_NAND)//load from SD card
        Rval = AppLib_CalibSDCardLoad(CalId);
        if (Rval != OK) {
            return Rval;
        }

        sprintf(OutputStr, "Save calib %d to NAND", CalId);
        Rval = AppLib_CalibNandSave(CalId,0);
        if (Rval != OK) {
            return Rval;
        }
#endif
    } else if (strcmp(Argv[0], "_calib_load") == 0) {
        CalId = atoi(Argv[1]);
#if (CALIB_STORAGE == CALIB_FROM_NAND)
        sprintf(OutputStr, "Load calib %d from NAND", CalId);
        Rval = AppLib_CalibNandLoad(CalId);
#else //from sd card
        sprintf(OutputStr, "Load calib %d from SD card", CalId);
        //Load from SD card
        Rval = AppLib_CalibSDCardLoad(CalId);
#endif
    } else if (strcmp(Argv[0], "_project_name") == 0) {
        AmbaPrint("Calib_Project_Name: %s Script_Project_Name: %s", CALIB_PROJECT_NAME, Argv[1]);
        if ( strcmp(Argv[1] , CALIB_PROJECT_NAME) == 0) {
            sprintf(OutputStr, "Project_Name:%s", CALIB_PROJECT_NAME);
            Rval = 0;
        } else {
            sprintf(OutputStr, "Error, The Project ID is mismatched.");
            Rval = -1;
        }
    } else if (strcmp(Argv[0], "_rom_load")==0) {
        if (Argc ==2) {
            CalId = atoi(Argv[1]);
            Rval = AppLib_CalibROMLoad(CalId);
        } else {
            CalId = atoi(Argv[1]);
            Rval = AppLib_CalibROMLoadTable(CalId, atoi(Argv[2]),  atoi(Argv[3]));
        }
    } else if (strcmp(Argv[0], "_rom_load_head")==0) {
        CalId = atoi(Argv[1]);
        Rval = AppLib_CalibROMLoadTableHead(CalId);
    } else if (strcmp(Argv[0], "_nand_load")==0) {
        if (Argc ==2) {
            CalId = atoi(Argv[1]);
            Rval = AppLib_CalibNandLoad(CalId);
        } else {
            CalId = atoi(Argv[1]);
            Rval = AppLib_CalibNandLoadTable(CalId, atoi(Argv[2]),  atoi(Argv[3]));
        }
        sprintf(OutputStr, "Load calib %d from NAND", CalId);
    } else if (strcmp(Argv[0], "_nand_load_head")==0) {
        CalId = atoi(Argv[1]);
        Rval = AppLib_CalibNandLoadTableHead(CalId);
        sprintf(OutputStr, "Load calib %d head from NAND", CalId);
    } else if (strcmp(Argv[0], "_sd_save")==0) {
        CalId = atoi(Argv[1]);
        Rval = AppLib_CalibSDCardSave(CalId, 0 );
    } else if (strcmp(Argv[0], "_simple_init")==0) {
        CalId = atoi(Argv[1]);
        Rval = AppLib_CalibInitSimple(CalId);
    } else if (strcmp(Argv[0], "_simple_load")==0) {
        UINT8 Format = 0;    
        CalId = atoi(Argv[1]);
        if (strcmp(Argv[2], "nand") == 0) {
            Format = CALIB_FROM_NAND;
        } else if (strcmp(Argv[2], "sd") == 0 ) {
            Format = CALIB_FROM_SD_CARD;
        } else if (strcmp(Argv[2], "rom") == 0) {
            Format = CALIB_FROM_ROM;
        }
        Rval = AppLib_CalibInitLoadSimple(CalId,  Format);
    } else if (strcmp(Argv[0], "_nand_save")==0) {
        CalId = atoi(Argv[1]);
        Rval = AppLib_CalibNandSave(CalId, 0);
        sprintf(OutputStr, "Save calib %d to NAND", CalId);
    } else if (strcmp(Argv[0], "_check_size")==0) {
        Rval = AppLib_CalibCheckStructure();
        sprintf(OutputStr, "check struct size done");
    } else if (strcmp(Argv[0], "_print_status") == 0) {
        //Cal_Stie_Status_s *PCalSite;

        CalId = atoi(Argv[1]);
        PCalSite = AppLib_CalibGetSiteStatus(CalId);
        // Print site Status

        AmbaPrint("CalId #%d", CalId);
        AmbaPrint("\tstatus = 0x%04X, version=0x%04X", PCalSite->Status, PCalSite->Version);
        AmbaPrint("\tsub site Status:");
        for (i=0; i<8; i++) { // 8 sub-site
            AmbaPrint("\t\toperation #%d = 0x%04X", i, PCalSite->SubSiteStatus[i]);
        }

        AmbaPrint("\treserved: %s", PCalSite->Reserved);

        sprintf(OutputStr, "site%d %s Status:%s ver:0x%08X, sub-stat:%d,%d,%d,%d,%d,%d,%d,%d",
                CalId,
                AppLib_CalibGetObj(CalId)->Name,
                (PCalSite->Status != 0)? "OK": "NG",
                PCalSite->Version,
                PCalSite->SubSiteStatus[0]>0,
                PCalSite->SubSiteStatus[1]>0,
                PCalSite->SubSiteStatus[2]>0,
                PCalSite->SubSiteStatus[3]>0,
                PCalSite->SubSiteStatus[4]>0,
                PCalSite->SubSiteStatus[5]>0,
                PCalSite->SubSiteStatus[6]>0,
                PCalSite->SubSiteStatus[7]>0);
        if (RvalCalFunc != 0) {
            strcpy(OutputStr,CalibOutputStr);
        }
        Rval = 0;

    } else if (strcmp(Argv[0], "_ctl_init") == 0) {
        AppLibCalibAdjust_Init();
        AmbaPrint("ctl init");
        Rval = 0;
    } else if (strcmp(Argv[0], "_ctl") == 0) {
        extern void AppLibCalibAdjust_Func(void);
        
        AppLibCalibAdjust_Func();
        AmbaPrint("ctl test");
        Rval = 0;
    } else {
        IsProcessed = 0;
        for (CalId=0; CalId<NVD_CALIB_MAX_OBJS; CalId++) {
            Cal_Obj_s *CalObj = AppLib_CalibGetObj(CalId);

            if (strcmp(Argv[0], CalObj->Name) == 0) {
                if (CalObj->CalibFunc != NULL) {
                    //Cal_Stie_Status_s *PCalSite, CalSite;

                    // Get Status and have a local copy
                    PCalSite = AppLib_CalibGetSiteStatus(CalId);
                    CalSite = *PCalSite;

                    // Call calibration API
                    RvalCalFunc = CalObj->CalibFunc(Argc, Argv, OutputStr, &CalSite, CalObj);
                    if (RvalCalFunc < 0) {
                        sprintf(Buf,". ERR = %d",RvalCalFunc);
                        strcat(OutputStr,Buf);
                    }
                    strcpy(CalibOutputStr,OutputStr);
                    Rval = RvalCalFunc;
                    IsProcessed = 1;
                    // Save Status
                    *PCalSite = CalSite;
                    //bug
                    //save to Nand
#if (CALIB_STORAGE == CALIB_FROM_NAND)
                    AppLib_CalibNandSave(CAL_STATUS_ID,0);
#else
                    //save to SD card
                    AppLib_CalibSDCardSave(CAL_STATUS_ID,0);
#endif
                    break;
                }
            }
        }
    }

    if (IsProcessed > 0) {
        if (IsProcessed == 1) {
            Rval = RvalCalFunc;
        }
        AppLib_CalibPrint(Rval, OutputStr);
        *MsgAvailable = 1;
        return Rval;
    } else {
        *MsgAvailable = 0;
        return 0;
    }


}

/* ---------- */
// Calibration manager SD card IF

#define CALIB_SD_IF_COUNT_DOWN      7

/**
 *  @brief divide script line into command tokens
 *
 *  divide script line into command tokens
 *
 *  @param [in]Buf calibration script line
 *  @param [out]Argc command line token total number
 *  @param [out]Argv command line token contents
 *
 *  @return 0 success, -1 failure
 */
static int AppLib_CalibGetLineToken(char Buf[], int *Argc, char *Argv[])
{
    char *Token;
    // Parse the input string to multiple tokens
    Token = strtok(Buf, " ");
    *Argc= 0;
    while (Token != NULL) {

        Argv[*Argc] = Token;
        (*Argc)++;
        Token = strtok(NULL, " ");
    }
    return 0;
}

/**
 *  @brief read calibration information from the script
 *
 *  read calibration information from the script
 *
 *  @param [in]Fp file pointer
 *  @param [out]Buf read the calibration information to the buffer
 *
 *  @return 0 success, -1 failure
 */
static int AppLib_CalibGetLine(AMBA_FS_FILE *Fp, char Buf[])
{
    unsigned char Ch;
    // Normal state
    do {
        if (AmbaFS_fread(&Ch, 1, 1, Fp) == 0) { // read 1 byte
            return -1;
        }
        if ( (Ch == '\n') || (Ch == '\r') ) {
            break;  // go to end-of-line Status
        }
        *Buf = Ch;
        Buf++;
    } while (1);

    // End of line state
    do {
        if (AmbaFS_fread(&Ch, 1, 1, Fp) == 0) { // read 1 byte
            break;
        }

        if ( (Ch == '\n') || (Ch == '\r') ) {
            /* do nothing */
        } else {
            // Reach to next line, roll back 1 byte
            AmbaFS_fseek(Fp, -1, PF_SEEK_CUR);
            break;
        }
    } while (1);

    *Buf = '\0';
    return 0;
}


/**
 *  @brief calibration by SD card interface
 *
 *  read calibration information from the script
 *
 *
 *  @return 0 success, -1 failure
 */
static int AppLib_CalibSdIf(void)
{
    AMBA_FS_FILE *FpIn = NULL, *FpOut = NULL;
    char CalibFn[] = "C:\\cal.txt";
    char CalibOutFn[] = "C:\\cal_out.txt";
    int Err = 0;
    int Rval, MsgAvailable;
    char Buf[256], OutputStr[256];
    char change_line='\n';
    int NumToken;
    char *Tokens[MAX_CLI_CMD_TOKEN];
    // Open cal.txt & cal_out.txt

    //char uni_buf[32];

    //scardmgr_slot_info_t info;
    //slot = AMP_system_cmd(MW_GET_ACTIVE_SLOT, 0, 0);
    //scardmgr_get_one_slot_info(slot, &info);

    //asc_to_uni(CalibFn, uni_buf);
    //uni_buf[0] = app_card_get_active_drive();
    //uni_buf[0] = info.drive_letter;
    if ( (FpIn = AmbaFS_fopen(CalibFn, "r")) == NULL ) {
        Err = 1;
        AmbaPrint("can't open file %s",CalibFn);
        return -1;
    }

    //asc_to_uni(CalibOutFn, uni_buf);
    //uni_buf[0] = app_card_get_active_drive();
    //uni_buf[0] = info.drive_letter;
    if ( (FpOut = AmbaFS_fopen(CalibOutFn, "w")) == NULL ) {
        Err = 1;
        AmbaPrint("can't open file %s",CalibOutFn);
        return -1;
    }
    if (Err) {
        AmbaFS_fclose(FpIn);
        AmbaFS_fclose(FpOut);
        return -1;
    }

    while (1) {
        // Read a line from the file
        if (AppLib_CalibGetLine(FpIn, Buf) == -1) {
            break;
        }

        // Execute the command and show result
        AppLib_CalibGetLineToken(Buf, &NumToken, Tokens);
        Rval = AppLib_CalibCli(NumToken, Tokens, OutputStr, &MsgAvailable);
        if (MsgAvailable) {
            AmbaFS_fwrite(OutputStr, strlen(OutputStr), 1, FpOut);
            AmbaFS_fwrite(&change_line, 1, 1, FpOut);
            //          calib_gui_show_str(OutputStr, (Rval==0)? CALIB_GUI_BLUE: CALIB_GUI_RED);
        }


        // If any command failed, then skip all subsequent commands
        if (Rval < 0) {
            break;
        }
    }
    AmbaFS_fclose(FpIn);
    AmbaFS_fclose(FpOut);
    //close calib task,if open calib task
    //bug
    //  dly_tsk(1000);
    //app_sys_snd_msg(HMSG_STATE_CALIBTASK_DONE, 0, 0);
    return 0;
}
/* ---------- */
#if 0
static int app_calib_task(void)
{
    if (CalibSel != CALIB_MODE1) {
        AppLib_CalibSdIf();
    } else {
        AppLib_CalibPathIf(CalibOutFn1);
    }
    return 0;
}

static int app_calib_task_init(void)
{
    T_CFLG pk_cflg;
    T_CTSK pk_crcvtsk;


    pk_cflg.flgatr = TA_TFIFO | TA_WMUL | TA_CLR;
    pk_cflg.iflgptn = 0x0;
    calib_flg_id = acre_flg(&pk_cflg);
    K_ASSERT(calib_flg_id);
    /* Get stack buffer */
    get_align_buf(&calib_tsk_stack, &calib_tsk_stack_raw,
                  CALIB_TASK1_STACK_SIZE, 4, 1);

    /* create fpn_ctl_task */
    pk_crcvtsk.tskatr = (TA_ACT | TA_HLNG);
    pk_crcvtsk.exinf = 0;
    pk_crcvtsk.task = (FP) app_calib_task;
    pk_crcvtsk.itskpri = CALIB_TASK1_PRIORITY;
    pk_crcvtsk.stksz = CALIB_TASK1_STACK_SIZE;
    pk_crcvtsk.stk = (VP) calib_tsk_stack;
    calib_tsk_id = acre_tsk(&pk_crcvtsk);
    set_tsk_str(calib_tsk_id, "calib task");

    K_ASSERT(calib_tsk_id > 0);
    if (calib_tsk_id != 0) {
        return 0;
    }
    return -1;
}
#endif

/**
 *  @brief Timer handler for calibration by SD card interface
 *
 *  Timer handler for calibration by SD card interface
 *
 *
 *  @return 0 success, -1 failure
 */
void AppLib_CalibSdIfTimerHandler(void)
{
    static int Time = CALIB_SD_IF_COUNT_DOWN;
    char calib_str[80] = {0};

    Time--;
    if (Time == 0) {
        /** Unregister countdown timer first */
        Time = CALIB_SD_IF_COUNT_DOWN;
        //      app_timer_unregister(TIMER_1HZ, AppLib_CalibSdIfTimerHandler);
        CalibSel = CALIB_MODE0;
        AmbaPrint("Calibration is beginning......");
        AppLib_CalibSdIf();
        //app_calib_task_init();
    } else {
        // Show remaining Time GUI
        sprintf(calib_str, "calibration go after %d sec", Time);
        //      calib_gui_show_str(calib_str, CALIB_GUI_YELLOW);
        AmbaPrint("AppLib_CalibSdIfTimerHandler: %d", Time);
    }
}


/**
 *  @brief calibration by path interface
 *
 *  calibration by path interface
 *
 *  @param [in]Path: the path for the calibration script
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibPathIf(char* Path)
{
    AMBA_FS_FILE *FpIn = NULL, *FpOut = NULL;
    char CalibOutFn[256];
    int Err = 0;
    int Rval, MsgAvailable;
    char Buf[256], OutputStr[256];
    char change_line='\n';
    int NumToken;
    char *Tokens[MAX_CLI_CMD_TOKEN];

    AmbaPrint("AppLib_CalibPathIf");
    //int slot;
    //scardmgr_slot_info_t info;
    //slot = AMP_system_cmd(MW_GET_ACTIVE_SLOT, 0, 0);
    //scardmgr_get_one_slot_info(slot, &info);
    //asc_to_uni(Path, uni_buf);
    //uni_buf[0] = app_card_get_active_drive();
    //uni_buf[0] = info.drive_letter;
    if ((FpIn = AmbaFS_fopen(Path, "r")) == NULL ) {
        AmbaPrint("File open error %s",Path);
        AmbaFS_fclose(FpIn);
        //dly_tsk(1000);
        //app_sys_snd_msg(HMSG_STATE_CALIBTASK_DONE, 0, 0);
        return -1;
    }
    CalibOutFn[0] = '\0';
    strcat(CalibOutFn, Path);
    strcat(CalibOutFn, ".log");

    if ( (FpOut = AmbaFS_fopen(CalibOutFn, "w")) == NULL ) {
        AmbaPrint("File open error %s",CalibOutFn);
        return -1;
    }

    if (Err) {
        AmbaFS_fclose(FpIn);
        AmbaFS_fclose(FpOut);
        return -1;
    }
    while (1) {
        // Read a line from the file
        if (AppLib_CalibGetLine(FpIn, Buf) == -1) {
            break;
        }
        // Execute the command and show result
        AppLib_CalibGetLineToken(Buf, &NumToken, Tokens);
        Rval = AppLib_CalibCli(NumToken, Tokens, OutputStr, &MsgAvailable);
        if (MsgAvailable) {
            AmbaFS_fwrite(OutputStr, strlen(OutputStr), 1, FpOut);
            AmbaFS_fwrite(&change_line, 1, 1, FpOut);
            //          calib_gui_show_str(OutputStr, (Rval==0)? CALIB_GUI_BLUE: CALIB_GUI_RED);
        }
        // If any command failed, then skip all subsequent commands
        if (Rval < 0) {
            break;
        }

    }
    AmbaFS_fclose(FpIn);
    AmbaFS_fclose(FpOut);
    //close calib task,if open calib task
    //  dly_tsk(1000);
    //  app_sys_snd_msg(HMSG_STATE_CALIBTASK_DONE, 0, 0);
    return 0;

}

