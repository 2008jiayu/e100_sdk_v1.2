/**
 * @file src/app/connected/applib/src/storage/ApplibStorage_Card.c
 *
 * Implementation of Card Utility Apis
 *
 * History:
 *    2013/12/05 - [Martin Lai] created file
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

#include <applib.h>
#include <AmbaCardManager.h>
#include <AmbaSD.h>
#include <AmbaUtility.h>

//#define DEBUG_APPLIB_CARD
#if defined(DEBUG_APPLIB_CARD)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

/*************************************************************************
 * Card definitons
 ************************************************************************/
/** Card slot dictionary */
static UINT8 CardSlotDic[CARD_NUM] = {
    SCM_SLOT_SD0,/**< CARD_SD0 */
    SCM_SLOT_SD1,/**< CARD_SD1 */
    SCM_SLOT_FL0,/**< CARD_NAND0 */
    SCM_SLOT_FL1,/**< CARD_NAND1 */
    SCM_SLOT_RD/**< CARD_RD*/
};

/** Individual card status */
typedef struct _APPLIB_CARD_STATUS_s_ {
    UINT8 Flags;
#define CARD_FLAGS_PRESENT        (0x01)
#define CARD_FLAGS_INIT            (0x02)
#define CARD_FLAGS_SYNCED        (0x04)
#define CARD_FLAGS_READY        (0x08)
#define CARD_FLAGS_BLOCK_INSERT    (0x10)
#define CARD_FLAGS_ID_CHANGED    (0x20)
    UINT8 FlagsError;
#define CARD_FLAGS_ER_FORMAT    (0x01)
#define CARD_FLAGS_ER_INVALID    (0x02)
#define CARD_FLAGS_ER_FOLDER    (0x04)
    INT8 Format;
    UINT8 WriteProtect;
} APPLIB_CARD_STATUS_s;

static APPLIB_CARD_STATUS_s Cards[CARD_NUM] = {0};

static UINT8 CardInitFreespaceFlags[CARD_NUM] = {0};

/** Active card status */
typedef struct _APPLIB_CARD_ACTIVE_CARD_STATUS_s_ {
    UINT8 CardId;        /** Card id */
#define NO_ACTIVE_CARD    (0xFF)
    int CardFreeSpace;
} APPLIB_CARD_ACTIVE_CARD_STATUS_s;

static APPLIB_CARD_ACTIVE_CARD_STATUS_s ActiveCardStatus = {NO_ACTIVE_CARD, 0};
static UINT8 CardInsertState = 0;
static int InsertSlot = 0xFF;

#define CARD_MIN_NAND_STORAGE_SIZE          (64*1024*1024)    /** 64 MB */
#define CARD_FULL_THRESHOLD                 (14*1024*1024)    /** 14 MB */

/**card free space threshold default value = 14MB change by function AppLibCard_SetThreshold*/
static int CardThreshold = CARD_FULL_THRESHOLD;
/*************************************************************************
 * Card APIs
 ************************************************************************/
#ifdef CONFIG_APP_ARD
static void AppLibCard_Info(void)
 {
    AMBA_SD_HOST *pHost = NULL;
    AMBA_SD_CARD *pCard = NULL;

    pHost = AmbaSD_GetHost(SD_HOST_0);
    if(pHost){
        pCard = &pHost->Card;
        if(pCard){
            if(AmbaSD_CardINSlot(pHost, pCard)){
                AmbaPrint("Cid=%u,%u,%c%c%c%c%c[0x%x,0x%x,0x%x,0x%x,0x%x],%u,%u,%u,%u",pCard->Cid.Mid,pCard->Cid.Oid,
                         pCard->Cid.Pnm[0],pCard->Cid.Pnm[1],pCard->Cid.Pnm[2],pCard->Cid.Pnm[3],pCard->Cid.Pnm[4],
                         pCard->Cid.Pnm[0],pCard->Cid.Pnm[1],pCard->Cid.Pnm[2],pCard->Cid.Pnm[3],pCard->Cid.Pnm[4],
                         pCard->Cid.Prv,pCard->Cid.Psn,pCard->Cid.MDateYear,pCard->Cid.MDateMth);
                AmbaPrint("uhs=%u,SupportSdxc=%u,IsInit=%u,FreqMaxUhs=%u,InitLoop=%u",
                        pCard->Uhs,
                        pHost->SupportSdxc,
                        pCard->IsInit,
                        pHost->FreqMaxUhs,
                        pCard->InitLoop);
                AmbaPrint("speed=%u Hz",pHost->Ios.ActualClock);
            }else{
                AmbaPrint("not in slot");
            }
        }else{
            AmbaPrint("pCard null");
        }
    }else{
        AmbaPrint("pHost null");
    }
}
#endif

/**
 *  @brief Check card format
 *
 *  Check card format
 *
 *  @param [in] formatType Format type
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibCard_CheckFormatType(int formatType)
{
    int ReturnValue = 0;

    switch (formatType) {
    case FS_FAT32:
    case FS_FAT16:
    case FS_FAT12:
    case FS_FAT16S:
    case FS_FAT16_EXT_DOS:
    case FS_FAT32L:
    case FS_FAT16L:
    case FS_FAT16L_EXT_DOS:
    case FS_EXFAT:
        ReturnValue = 1;
        break;
    case FS_FAT_UNKNOWN:
        ReturnValue = -1;
        break;
    default:
        /** not supported Format */
        ReturnValue = 0;
        break;
    }

    DBGMSG("[AppLib - Card] <CheckFormatType> ReturnValue = %d", ReturnValue);
    return ReturnValue;
}


/**
 *  @brief Test the card.
 *
 *  Test the card.
 *
 *  @param [in] slot Card slot
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibCard_FirstAccessTest(int slot)
{
    int ReturnValue = 0;
#define WRIGHT_BLOCK_SIZE    (128*1024)
/**
 * Should be modified if the block size which recoder uses is
 * change
 **/
    int BlockSize = WRIGHT_BLOCK_SIZE;
    char Drive = (slot+'A');
    char Mode[3] = {'w','b','\0'};
    char Fnu[80];
    char TempData[16] = {0};
    AMBA_FS_FILE *File;
    AMBA_FS_DEVINF Dev;

    DBGMSGc2(BLUE, "[AppLib - Card] <FirstAccessTest> start: slot = %d", slot);
#if 0
    if (0) {
        PF_VOL_CFG VolConfig;

        /**
        * the 1st getdev takes much more time, and to check driver
        * status by the way. Enlarge FAT buffer to speed up the 1st
        * AmbaFS_GetDev()
        **/
        //pf_getvolcfg(Drive, &VolConfig);

        /* Set large fat buffer size */
        VolConfig.volume_config |= PF_SET_FAT_CACHE_SIZE;
        VolConfig.fat_buff_size <<= 3;
        //pf_setvolcfg(Drive, &VolConfig);

        ReturnValue = AmbaFS_GetDev(Drive, &Dev);

        /* Set fat buffer size back */
        VolConfig.volume_config |= PF_SET_FAT_CACHE_SIZE;
        VolConfig.fat_buff_size >>= 3;
        //pf_setvolcfg(Drive, &VolConfig);

        if (ReturnValue < 0) {
            AmbaPrint("[AppLib - Card] <FirstAccessTest> Drive is not valid");
            return -1;
        }
    }
#endif
    ReturnValue = AmbaFS_GetDev(Drive, &Dev);
    DBGMSG("[AppLib - Card] <FirstAccessTest> Write dummy file\n");
    sprintf(Fnu, "%c%s%c", Drive, ":\\.zzz", '\0');
    for (ReturnValue = 0 ; ReturnValue < 16; ReturnValue++) {
        TempData[ReturnValue] = ReturnValue;
    }


    File = AmbaFS_fopen((char const *)Fnu, (char const *)Mode);
    if (File != NULL) {
        ReturnValue = AmbaFS_fwrite(TempData, 1, BlockSize, File);
        if (ReturnValue != BlockSize) {
            DBGMSG("[AppLib - Card] <FirstAccessTest> fwrite %s fail", Fnu);
            ReturnValue = (((Dev.Ucl*Dev.Bps)>>10)*Dev.Spc); /** in KB */
            if (ReturnValue < (BlockSize>>10)) { /** error due to space run out */
                ReturnValue = 0;
            } else {
                AMBA_SCM_STATUS_s CardStatus;
                AmbaSCM_GetSlotStatus(slot, &CardStatus);
                if (CardStatus.WriteProtect) { //error due to write protect
                    ReturnValue = 0;
                } else {
                    ReturnValue = CARD_STATUS_UNFORMAT_CARD;
                }
            }
            goto access_check_done;
        }
#if 0
    ff_fseek(File,0,FF_SEEK_SET);
    ReturnValue = ff_fread((void *)TempData, 1, 16, File);
    if (ReturnValue != 16) {
        AmbaPrint("fread fail %d", pf_errnum());
        ReturnValue = CARD_INSERT_ERROR_NOT_MEM;
        goto access_check_done;
    }
    for (ReturnValue=0; ReturnValue<16; ReturnValue++) {
        if (TempData[ReturnValue] != ReturnValue) {
            AmbaPrint("file read/write check fail");
            ReturnValue = CARD_INSERT_ERROR_UNINIT;
            goto access_check_done;
        }
    }
#endif
    } else {
        AMBA_SCM_STATUS_s CardStatus;

        //DBGMSG("fopen %s error, %d", TempFileName, pf_errnum());
        //DBGMSG("[AppLib - Card] <AppLibCard_FirstAccessTest> fopen %s error", TempFileName);
        AmbaSCM_GetSlotStatus(slot, &CardStatus);
        if (CardStatus.WriteProtect) { /** error due to write protect */
            ReturnValue = 0;
        } else {
            ReturnValue = CARD_STATUS_NO_SUCH_CARD;
        }
    }

access_check_done:
    if (File != NULL) {
        AmbaFS_fclose(File);
        if (AmbaFS_remove((char const *)Fnu) < 0) {
            //AmbaPrint("[AppLib - Card] <FirstAccessTest> AmbaFS_remove fail %d", pf_errnum());
        }
    }

    DBGMSGc2(BLUE, "[AppLib - Card] <FirstAccessTest> end: ReturnValue = %d", ReturnValue);

    if (ReturnValue < 0) {
        return -1;
    }

    return ReturnValue;
}

/**
 *  @brief To set DCF root
 *
 *  To set DCF root
 *
 *  @param [in] slot Card slot
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibCard_SetRoot(int slot)
{
    int ReturnValue = 0;
    int CardId = 0;
    char Root = 'A';

    DBGMSGc2(BLUE, "[AppLib - Card] <SetRoot> start: slot = %d", slot);

    CardId = AppLibCard_GetCardId(slot);
    if (CardId < 0) {
        return -1;
    } else {
        if (!APPLIB_CHECKFLAGS(Cards[CardId].Flags, CARD_FLAGS_SYNCED)) {
#if 0
            /**
            * Do FSI sync to update FSI cache, otherwise might get wrong
            * free space value
            **/
            //fsi_sync(slot);
#else
            /** Turn off FSI to enhance write speed */
            //fsi_on_off(slot, 0);
#endif
            /** apply 1st card access checking **/
            ReturnValue = AppLibCard_FirstAccessTest(slot);
            if (ReturnValue < 0) {
                DBGMSGc2(BLUE, "[AppLib - Card] <SetRoot> FirstAccessTest fail ReturnValue = %d", ReturnValue);
                return -1;
            }
            APPLIB_ADDFLAGS(Cards[CardId].Flags, CARD_FLAGS_SYNCED);
        }
    }

    Root = slot+'A';
    ReturnValue = AppLibStorageDmf_Refresh(Root);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED, "[HY][AppLib - Card] <SetRoot> end: ReturnValue = %d", ReturnValue);
    } else {
        AmbaPrintColor(BLUE, "[HY][AppLib - Card] <SetRoot> end: ReturnValue = %d", ReturnValue);
    }

    return ReturnValue;
}

/**
 *  @brief SCM callback function
 *
 *  SCM callback function
 *
 *  @param [in] slot Card slot
 *  @param [in] eid Event id
 *
 */
static void AppLibCard_ScardmgrHandler(int slot, int eid)
{
    DBGMSGc2(BLUE, "[AppLib - Card] <ScardmgrHandler> slot = %d, eid = %d", slot, eid);
    AppLibComSvcHcmgr_SendMsgNoWait(HMSG_JACK_CARD((slot << 8) | eid), slot, eid);
}

/**
 *  @brief Initialization card module
 *
 *  Initialization card module
 *
 *  @return >=0 success, <0 failure
 */
int AppLibCard_Init(void)
{
    int ReturnValue = 0;

    AmbaSCM_Register(&AppLibCard_ScardmgrHandler);

    return ReturnValue;
}

/**
 *  @brief Polling card insert status.
 *
 *  Polling card insert status.
 *
 *  @param [in] cardId Card id.
 *
 *  @return >=0 Insert card id, <0 failure
 */
int AppLibCard_Polling(UINT32 cardId)
{
    int ReturnValue = 0;
    AMBA_SCM_STATUS_s CardStatus;

    if (cardId < CARD_NUM) {
        /* Get slot status */
        ReturnValue = AmbaSCM_GetSlotStatus(CardSlotDic[cardId], &CardStatus);
        if (ReturnValue == 0) {
            /* Check if the card is present */
            if ((CardStatus.CardPresent)&&(CardStatus.CardType&SCM_CARD_MEM)) {

                /* Set this card as present directly */
                APPLIB_ADDFLAGS(Cards[cardId].Flags, CARD_FLAGS_PRESENT);
                /* Check if the card is initialized */
                if (CardStatus.Format != FS_FAT_NO_FORMAT) {
                    /* Check if the card is a memory card */
                    if (CardStatus.CardType & SCM_CARD_MEM) {
                        /* Send card insert msg if it is a memory card */
                        AppLibCard_SndCardInsertMsg(cardId);
                    } else {
                        /* If the card is not a memory card, tag it as not present */
                        APPLIB_REMOVEFLAGS(Cards[cardId].Flags, CARD_FLAGS_PRESENT);
                    }
                }
            }
        }
    } else {
        return -1;
    }
    return ReturnValue;
}

/**
 *  @brief Get drive id
 *
 *  Get drive id
 *
 *  @param [in] cardId Card id
 *
 *  @return >=0 Drive id, <0 failure
 */
char AppLibCard_GetDrive(UINT32 cardId)
{
    if (cardId < CARD_NUM) {
        return ('A' + CardSlotDic[cardId]);
    } else {
        return -1;
    }
}

/**
 *  @brief Get card slot
 *
 *  Get card slot
 *
 *  @param [in] cardId Card id.
 *
 *  @return >=0 Slot id, <0 failure
 */
int AppLibCard_GetSlot(UINT32 cardId)
{
    if (cardId < CARD_NUM) {
        return CardSlotDic[cardId];
    } else {
        return -1;
    }
}

/**
 *  @brief Get card id
 *
 *  Get card id
 *
 *  @param [in] slot Card slot.
 *
 *  @return >=0 Card id, <0 failure
 */
int AppLibCard_GetCardId(int slot)
{
    int ReturnValue = 0;

    switch (slot) {
    case SCM_SLOT_SD0:
        ReturnValue = CARD_SD0;
        break;
    case SCM_SLOT_SD1:
        ReturnValue = CARD_SD1;
        break;
    case SCM_SLOT_FL0:
        ReturnValue = CARD_NAND0;
        break;
    case SCM_SLOT_FL1:
        ReturnValue = CARD_NAND1;
        break;
    case SCM_SLOT_RD:
        ReturnValue = CARD_RD;
        break;
    default:
        DBGMSG("[AppLib - Card] Slot %d is not implemented", slot);
        ReturnValue = -1;
        break;
    }

    return ReturnValue;
}


/**
 *  @brief Get slot id from drive id
 *
 *  Get slot id from drive id
 *
 *  @param [in] drive Drive id
 *
 *  @return >=0 Slot id, <0 failure
 */
int AppLibCard_GetSlotFromChar(char drive)
{
    int ReturnValue = 0;

    if (drive > 'Z') {
        ReturnValue = drive-'a';
        if (ReturnValue > ('z'-'a')) {
            ReturnValue = -1;
        }
    } else {
        ReturnValue = drive-'A';
    }

    DBGMSG("[AppLib - Card] Get slot %d from char %c", ReturnValue, drive);

    return ReturnValue;
}

/**
 *  @brief Format card
 *
 *  Format card
 *
 *  @param [in] slot Slot id
 *
 *  @return >=0 success, <0 failure
 */
int AppLibCard_Format(int slot)
{
    int ReturnValue = 0;
    char Drive = 'A';
    const char ParamHdd[13] = {'F','A','T','3','2',',','s','p','c','=','6','4','\0'};
    char ParamMem[13] = {0};
    //const char ParamMem[13] = {'F','A','T','1','6',',','s','p','c','=','6','4','\0'};
    AMBA_SCM_STATUS_s CardStatus = {0};

    ReturnValue = AmbaSCM_GetSlotStatus(slot, &CardStatus);
    if (ReturnValue < 0) {
        return ReturnValue;
    }

    if (CardStatus.CardPresent && !CardStatus.WriteProtect) {
        Drive = slot+'A';
        if ((Drive == 'R') || (Drive == 'r')) {
            return -1;
        }
        DBGMSG("[AppLib - Card] <Format> Start Formatting %c", Drive);
        if (CardStatus.CardType == (SCM_CARD_MECH | SCM_CARD_MEM)) {
            if ((Drive == 'U') || (Drive == 'u')) {
                //ReturnValue = mwusb_Format(Drive, &ParamHdd);
            } else {
                ReturnValue = AmpCFS_Format(Drive, ParamHdd);
            }
        } else {
            if ((Drive == 'U') || (Drive == 'u')) {
             //ReturnValue = mwusb_Format(Drive, &ParamMem);
            } else {
                ReturnValue = AmpCFS_Format(Drive, ParamMem);
            }
        }
    }

    return ReturnValue;
}

/**
 *  @brief Insert card
 *
 *  Insert card
 *
 *  @param [in] slot Card slot
 *
 *  @return >=0 success, <0 failure
 */
int AppLibCard_Insert(int slot)
{
    int ReturnValue = 0;
    int CardId = 0;
    char Drive = 'A';
    AMBA_SCM_STATUS_s CardStatus = {0};
    AMBA_FS_DEVINF Dev = {0};

    DBGMSGc2(BLUE, "[AppLib - Card] <Insert> start: slot = %d", slot);
    CardInsertState = 1;

    CardId = AppLibCard_GetCardId(slot);
    if (CardId < 0) {
        DBGMSG("[AppLib - Card] <Insert> No such card");
        ReturnValue = CARD_INSERT_ERROR_NO_CARD;
        goto _done;
    } else if (CardId == ActiveCardStatus.CardId) {
        DBGMSG("[AppLib - Card] <Insert> Card already inserted");
        ReturnValue = CARD_INSERT_ACTIVE_DONE;
        goto _done;
    } else {    /** get drive letter */
        Drive = 'A' + slot;
    }
    AmbaFS_Mount(Drive);
    DBGMSG("[AppLib - Card] <Insert> status init stage start  slot = %d", slot);

#if defined(ENABLE_EMMC_BOOT) && defined(SM_STG2_SLOT)
    /* For EMMC BOOT, there will not have Drive "A". */
    /* It might have STG2, Drive "M", instead, but there will not have any card insert message for that Drive */
    /* Therefore, we have to detect STG2 and Format it is it is unFormated while do "D" Drive's card insert. */
    if (slot == SCM_SLOT_SD0) {
        if (SM_STG2_SLOT == 3) {
            ReturnValue = AmbaSCM_GetSlotStatus(SCM_SLOT_STG2, &CardStatus);
            if (CardStatus.Format == FS_FAT_UNKNOWN) {    // not Formated
                /* For erase all, boot after burning code for the first time*/
                char Drive = 'M';
                const char ParamMem[13] = {'F','A','T','1','6',',','s','p','c','=','6','4','\0'};

                AmbaPrint("[AppLib - Card] <Insert> %s: Format m", __func__);
                AmbaFS_Format(Drive, &ParamMem);
            }
        } else {
            AmbaPrintColor(RED,"[AppLib - Card] <Insert> %s: unsupported SM_STG2_SLOT(%d)",__func__,SM_STG2_SLOT);
        }
    }
#endif

    /* Get the card status */
    ReturnValue = AmbaSCM_GetSlotStatus(slot, &CardStatus);

    /* Double check if the card to be inserted is still present */
    if ((ReturnValue < 0) || !CardStatus.CardPresent) {
        DBGMSG("[AppLib - Card] <Insert> Card removed during card insert operation.");
        memset(&Cards[CardId], 0, sizeof(APPLIB_CARD_STATUS_s));
        ReturnValue = CARD_INSERT_ERROR_NO_CARD;
        goto _done;
    }

    /* Check if the card is a memory card */
    // The judgment has to put before check if the card initialization is done.
    if ((CardStatus.CardType & SCM_CARD_MEM) == 0) {
        DBGMSG("[AppLib - Card] <Insert> It is not a memory card.");
        memset(&Cards[CardId], 0, sizeof(APPLIB_CARD_STATUS_s));
        ReturnValue = CARD_INSERT_ERROR_NOT_MEM;    // This card is not a memory card
        goto _done;
    }

    DBGMSGc2(GREEN,"CardStatus.Format = %d",CardStatus.Format);

    /* Check if the card initialization is done */
    if (CardStatus.Format == FS_FAT_NO_FORMAT) {
        DBGMSG("[AppLib - Card] <Insert> Card init not ready.");
        /*
        * Although the card initialization hasn't been done, the card can be
        * set as present in advance.
        */
        APPLIB_ADDFLAGS(Cards[CardId].Flags, CARD_FLAGS_PRESENT);
        ReturnValue = CARD_INSERT_ERROR_UNINIT;
        goto _done;
    }

    /* Check if NAND storage is enough */
    if ((CardId == CARD_NAND0) || (CardId == CARD_NAND1)) {
        DBGMSG("[AppLib - Card] <Insert> Check NAND storage.");
        if (AppLibCard_CheckNandStorage(CardId) == 0) {
            /**
            * It fix "only small NAND on board, no NoCard message at booting".
            **/
            ReturnValue = CARD_INSERT_ERROR_SMALL_NAND;
            goto _done;
        } else {
            /* If NAND storage is not Formatted, Format it */
            if (CardStatus.Format == FS_FAT_UNKNOWN) {    // not Formated
                /* For erase all, boot after burning code for the first time*/
                const char ParamMem[13] = {'F','A','T','1','6',',','s','p','c','=','6','4','\0'};
                DBGMSG("[AppLib - Card] <Insert> Format nand storage");
                AmbaFS_Format(AppLibCard_GetDrive(CardId), ParamMem);
            }
        }
    }

    /* Set this slot as present, due to the card might insert after polling */
    APPLIB_ADDFLAGS(Cards[CardId].Flags, CARD_FLAGS_PRESENT);

    /* Set Format status */
    Cards[CardId].Format = CardStatus.Format;

    /* Write protect status*/
    Cards[CardId].WriteProtect = CardStatus.WriteProtect;

    /* Set this card as initialized */
    APPLIB_ADDFLAGS(Cards[CardId].Flags, CARD_FLAGS_INIT);

    DBGMSG("[AppLib - Card] <Insert> Status init stage end");

    /* Check the primary slot */
    ReturnValue = AppLibCard_GetPrimarySlot(); //ReturnValue is the predicted active slot
    /* If the card is not the primary card, return low-priority done */
    if ((ReturnValue != slot) || (CardId > ActiveCardStatus.CardId)) {
        DBGMSG("[AppLib - Card] <Insert> Lower priority card");
        ReturnValue = CARD_INSERT_LOWPRI_DONE;
        goto _done;
    }
#ifdef CONFIG_APP_ARD
    if(Drive == 'C'){
        AppLibCard_Info();
    }
#endif
    AmbaPrint("[AppLib - Card] <Insert> Insert drive %c", Drive);

    /* Set active card*/
    ActiveCardStatus.CardId = CardId;

    /* Check if the card is invalid (initialization failed) */
    if (CardStatus.Format == FS_FAT_ERROR) {
        DBGMSG("[AppLib - Card] <Insert> Card initialization failed: CardStatus.Format = %d", CardStatus.Format);
        APPLIB_ADDFLAGS(Cards[CardId].FlagsError, CARD_FLAGS_ER_FORMAT);
        ReturnValue = CARD_INSERT_ERROR_FORMAT;
        goto _done;
    }

    /* Check if the card Format is unknown (maybe unFormatted) */
    if (CardStatus.Format == FS_FAT_UNKNOWN) {
        DBGMSG("[AppLib - Card] <Insert> UnFormatted card: slot = %d", slot);
        /**
        * When not Formated, the state of active card is present, But
        * its Format is "not Formatted"
        **/
        APPLIB_ADDFLAGS(Cards[CardId].FlagsError, CARD_FLAGS_ER_FORMAT);
        ReturnValue = CARD_INSERT_ERROR_FORMAT;
        goto _done;
    }

    /* Check if the card Format is unsupported (or corrupted) */
    if ((CardStatus.Format == FS_FAT_CONTAIN_ERROR) ||
        (!AppLibCard_CheckFormatType(CardStatus.Format))) {    // Unsupported Format type
        Cards[CardId].Format = FS_FAT_CONTAIN_ERROR;
        APPLIB_ADDFLAGS(Cards[CardId].FlagsError, CARD_FLAGS_ER_FORMAT);
        ReturnValue = CARD_INSERT_ERROR_FORMAT;
        goto _done;
    }

    /* Set root */
    InsertSlot = slot;
    ReturnValue = AppLibCard_SetRoot(slot);
    if (ReturnValue < 0) {
        /** Set root failed */
        APPLIB_ADDFLAGS(Cards[CardId].FlagsError, CARD_FLAGS_ER_INVALID);
        ReturnValue = CARD_INSERT_ERROR_SETROOT;
        goto _done;
    } else {
        APPLIB_REMOVEFLAGS(Cards[CardId].FlagsError, CARD_FLAGS_ER_INVALID);
    }

    /* Clean Format image context after set_root succeeded */
    //app_Format_clean_ctx_cache(CTX_IMAGE);

    /* Double check if the card is still there */
    /* Card might be removed during card insert operation */
    if (AmbaFS_GetDev(Drive, &Dev) < 0) {
        DBGMSG("[AppLib - Card] <Insert> AmbaFS_GetDev fail");
        AmbaSCM_GetSlotStatus(slot, &CardStatus);
        if (!CardStatus.CardPresent) {
            DBGMSG("[AppLib - Card] <Insert> Card Removed during card insert");
            memset(&Cards[CardId], 0, sizeof(APPLIB_CARD_STATUS_s));
            ReturnValue = CARD_INSERT_ERROR_NO_CARD;
            goto _done;
        }
    }

    ReturnValue = CARD_INSERT_ACTIVE_DONE;

_done:
    if ((ActiveCardStatus.CardId == CardId) && (ReturnValue < 0)) {
        DBGMSG("[AppLib - Card] <Insert> Active card insert failed");
    }
    CardInsertState = 0;
    DBGMSGc2(BLUE, "[AppLib - Card] <Insert> end: ReturnValue = %d", ReturnValue);
    return ReturnValue;
}


/**
 *  @brief Remove card
 *
 *  Remove card
 *
 *  @param [in] slot Card slot
 *
 *  @return >=0 success, <0 failure
 */
int AppLibCard_Remove(int slot)
{
    int ReturnValue = 0;
    int CardId = 0;
    char Drive = 'A';

    DBGMSGc2(BLUE, "[AppLib - Card] <Remove> start: slot = %d", slot);

    CardId = AppLibCard_GetCardId(slot);
    if (CardId < 0) {
        DBGMSG("[AppLib - Card] <Remove> No such card");
        ReturnValue = -1;
        goto _done;
    } else {    /** get drive letter */
        Drive = 'A' + slot;
    }

    AmbaPrint("[AppLib - Card] <Remove> Remove Drive %c", Drive);

    /** Reset card status */
    memset(&Cards[CardId], 0, sizeof(APPLIB_CARD_STATUS_s));
    AmbaFS_Unmount(Drive);
    if (CardId == ActiveCardStatus.CardId) {
        DBGMSG("[AppLib - Card] <Remove> Active slot %d is removed", slot);
        ActiveCardStatus.CardId = NO_ACTIVE_CARD;
        ReturnValue = CARD_REMOVE_ACTIVE;
    } else {
        DBGMSG("[AppLib - Card] <Remove> Non-active slot %d is removed", slot);
        ReturnValue = CARD_REMOVE_NONACTIVE;
    }

    _done:
    DBGMSGc2(BLUE, "[AppLib - Card] <Remove> end: ReturnValue = %d", ReturnValue);
    return ReturnValue;
}

/**
 *  @brief Set a flag for inserting slot
 *
 *  Set a flag for inserting slot
 *
 *  @param [in] slot slot id
 *
 *  @return >=0 success, <0 failure
 */
int AppLibCard_SetInsertingSlot(int slot)
{
    InsertSlot = slot;
    return 0;
}


/**
 *  @brief change free space threshold
 *
 *  @param [in] threshold value of free space threshold
 *
 *  @return 0 success
 */
int AppLibCard_SetThreahold(int threshold)
{
    CardThreshold = threshold;
    return 0;
}

/**
 *  @brief Check the card whether be inserting
 *
 *  Check the card whether be inserting
 *
 *  @return >=0 Yes, <0 No
 */
int AppLibCard_CheckInsertingCard(void)
{
    int ReturnValue = 0;

    if (InsertSlot != 0xFF) {
        if (AppLibCard_GetCardId(InsertSlot) < AppLibCard_GetActiveCardId()) {
            return -1;
        }
    }

    return ReturnValue;
}

/**
 *  @brief Get active drive
 *
 *  Get active drive
 *
 *  @return >=0 drive id, <0 failure
 */
char AppLibCard_GetActiveDrive(void)
{
    if (ActiveCardStatus.CardId != NO_ACTIVE_CARD) {
        return ('A' + CardSlotDic[ActiveCardStatus.CardId]);
    } else {
        return -1;
    }
}

/**
 *  @brief Get active slot
 *
 *  Get active slot
 *
 *  @return >=0 slot id, <0 failure
 */
int AppLibCard_GetActiveSlot(void)
{
    if (ActiveCardStatus.CardId != NO_ACTIVE_CARD) {
        return CardSlotDic[ActiveCardStatus.CardId];
    } else {
        return -1;
    }
}

/**
 *  @brief Get active card id
 *
 *  Get active card id
 *
 *  @return >=0 card id, <0 failure
 */
int AppLibCard_GetActiveCardId(void)
{
    if (ActiveCardStatus.CardId != NO_ACTIVE_CARD) {
        return ActiveCardStatus.CardId;
    } else {
        return -1;
    }
}

/**
 *  @brief Get primary slot id
 *
 *  Get primary slot id
 *
 *  @return >=0 success, <0 failure
 */
int AppLibCard_GetPrimarySlot(void)
{
    int i = 0, ReturnValue = 0;
    AMBA_SCM_STATUS_s CardStatus;

    for (i=0; i<CARD_NUM; i++) {
        if (APPLIB_CHECKFLAGS(Cards[i].Flags, CARD_FLAGS_PRESENT)) {
            ReturnValue = AmbaSCM_GetSlotStatus(CardSlotDic[i], &CardStatus);
            if ((ReturnValue < 0) || (CardStatus.CardPresent != 1)) {
                /** If the card is not present, remove the flag CARD_FLAGS_PRESENT */
                DBGMSG("[AppLib - Card] Card id  %d is not present anymore! ", i);
                APPLIB_REMOVEFLAGS(Cards[i].Flags, CARD_FLAGS_PRESENT);
            } else {
                DBGMSG("[AppLib - Card] Found existing card: id = %d", i);
                break;
            }
        }
    }
    if (i < CARD_NUM) {
        return CardSlotDic[i];
    } else {
        DBGMSG("[AppLib - Card] There are no other Cards");
        return -1;
    }
}

/**
 *  @brief Check the status of card insert.
 *
 *  Check the status of card insert.
 *
 *  @return The status of card insert.
 */
int AppLibCard_CheckInsertState(void)
{
    DBGMSG("[AppLib - Card] Card insert state: %d", CardInsertState);
    return CardInsertState;
}

/**
 *  @brief Check card format type
 *
 *  Check card format type
 *
 *  @param [in] slot Slot id
 *  @param [in] drive Drive id
 *
 *  @return >=0 success, <0 failure
 */
int AppLibCard_CheckFormatParam(int slot, char drive)
{
    int ReturnValue = 0;
    int Total = 0;
    int CardType = 0;
    int FormatType = 0;
    AMBA_FS_DEVINF Dev;

    DBGMSGc2(BLUE, "[AppLib - Card] <CheckFormatParam> start: slot = %d", slot);

    if (AmbaFS_GetDev(drive, &Dev) < 0) {
        /** return value indecate get_dev error to handle card removed **/
        return -2;
    } else {
        AMBA_SCM_STATUS_s CardStatus;

        if (AmbaSCM_GetSlotStatus(slot,&CardStatus)<0) {
            return -3;
        }
        CardType = CardStatus.CardType;
        FormatType = CardStatus.Format;
    }

    if (FormatType == FS_EXFAT) {
        if (CardType != (SCM_CARD_MEM |
            SCM_CARD_SD  |
            SCM_CARD_SDHC|
            SCM_CARD_SDXC)) {//not SDXC card
            /* Trigger app to re-Format param, if card type is not SDXC and card Format type is exFAT */
            ReturnValue = -1;
        }
    } else {
        Total = ((Dev.Cls*Dev.Bps>>10)*Dev.Spc)>>10; //in MB
        if (Total > 1024) {
            if (Dev.Spc < 64) {
                /** trigger app to re-Format Format param */
                ReturnValue = -1;
            }
        } else if (Total > 16) {
            if (Dev.Spc != 32) {
                ReturnValue = -1;
            }
        }
    }

    DBGMSGc2(BLUE, "[AppLib - Card] <CheckFormatParam> end: ReturnValue= %d", ReturnValue);

    return ReturnValue;
}


/**
 *  @brief Check card's status
 *
 *  Check card's status
 *
 *  @param [in] checkFlags Check flag.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibCard_CheckStatus(UINT32 checkFlags)
{
    int ReturnValue = 0;
    int CardId = ActiveCardStatus.CardId;
    UINT64 Freespace = 0;
    char Drive = 'A';

    if (APPLIB_CHECKFLAGS(checkFlags, CARD_CHECK_PRESENT)) {
        int i = 0;
        AMBA_SCM_STATUS_s CardStatus = {0};
        for (i=0; i<CARD_NUM; i++) {
            ReturnValue = AmbaSCM_GetSlotStatus(CardSlotDic[i], &CardStatus);
            if ((ReturnValue >= 0) && (CardStatus.CardPresent == 1)) {
                DBGMSG("[AppLib - Card] <CheckStatus> Card %d is found present", i);
                break;
            }
        }
        if (i == CARD_NUM) {
            ReturnValue = CARD_STATUS_NO_CARD;
        } else if ((i == CARD_NAND0) || (i == CARD_NAND1)) {
            /* If the present card is NAND, the size should be checked for storage */
            if (AppLibCard_CheckNandStorage(i)) {
                ReturnValue = 0;
            } else {
                ReturnValue = CARD_STATUS_NO_CARD;
            }
        } else if (APPLIB_CHECKFLAGS(checkFlags, CARD_CHECK_WRITE) &&
            (CardInitFreespaceFlags[i] == 0)) {
            /* If write check is on, but the init freespace is not enough */
            ReturnValue = CARD_STATUS_NOT_ENOUGH_SPACE;
        } else {
            ReturnValue = 0;
        }
        goto _done;
    }

    if (CardId == NO_ACTIVE_CARD) {
        ReturnValue = CARD_STATUS_NO_CARD;
        DBGMSG("[AppLib - Card] <CheckStatus> CARD_STATUS_NO_CARD");
        goto _done;
    }

    if (APPLIB_CHECKFLAGS(checkFlags, CARD_CHECK_ID_CHANGE)) {
        ReturnValue = APPLIB_CHECKFLAGS(Cards[CardId].Flags, CARD_FLAGS_ID_CHANGED);
        DBGMSG("[AppLib - Card] <CheckStatus> CARD_CHECK_ID_CHANGE");
        goto _done;
    }

    if (APPLIB_CHECKFLAGS(checkFlags, CARD_CHECK_RESET)) {
        AMBA_SCM_STATUS_s CardStatus;
        DBGMSG("[AppLib - Card] <CheckStatus> Reset active card status: id = %d", CardId);
        ReturnValue = AmbaSCM_GetSlotStatus(CardSlotDic[CardId], &CardStatus);
        if ((ReturnValue >= 0) && (CardStatus.CardPresent == 1)) {
            /** Reset Format */
            Cards[CardId].Format = CardStatus.Format;
            if (CardStatus.Format > 0) {
                if (!AppLibCard_CheckFormatType(CardStatus.Format)) {    /** Invalid Format */
                    APPLIB_ADDFLAGS(Cards[CardId].FlagsError, CARD_FLAGS_ER_FORMAT);
                    Cards[CardId].Format = FS_FAT_CONTAIN_ERROR;
                } else {
                    APPLIB_REMOVEFLAGS(Cards[CardId].FlagsError, CARD_FLAGS_ER_FORMAT);
                }
            } else {
                APPLIB_ADDFLAGS(Cards[CardId].FlagsError, CARD_FLAGS_ER_FORMAT);
            }
            /** Reset write protect */
            Cards[CardId].WriteProtect = CardStatus.WriteProtect;
        } else {
            memset(&Cards[CardId], 0, sizeof(APPLIB_CARD_STATUS_s));
            ActiveCardStatus.CardId = NO_ACTIVE_CARD;
            ReturnValue = CARD_STATUS_NO_CARD;
            DBGMSG("[AppLib - Card] Check: CARD_STATUS_NO_CARD");
            goto _done;
        }
        ReturnValue = 0;
    }

    if (APPLIB_CHECKFLAGS(checkFlags, CARD_CHECK_WRITE) ||
        APPLIB_CHECKFLAGS(checkFlags, CARD_CHECK_CONT_WRITE) ||
        APPLIB_CHECKFLAGS(checkFlags, CARD_CHECK_DELETE) ||
        APPLIB_CHECKFLAGS(checkFlags, CARD_CHECK_MODIFY)) {
        DBGMSG("[AppLib - Card] Check: CARD_CHECK_WRITE");
        if (Cards[CardId].WriteProtect) {
            ReturnValue = CARD_STATUS_WP_CARD;
            DBGMSG("[AppLib - Card] Check: CARD_STATUS_WP_CARD");
            goto _done;
        }
    }

    if (APPLIB_CHECKFLAGS(Cards[CardId].FlagsError, CARD_FLAGS_ER_FORMAT)) {
        if (Cards[CardId].Format == FS_FAT_UNKNOWN) {
            ReturnValue = CARD_STATUS_UNFORMAT_CARD;
            DBGMSG("[AppLib - Card] <CheckStatus> CARD_STATUS_UNFORMAT_CARD");
            goto _done;
        }
        if (Cards[CardId].Format == FS_FAT_ERROR) {
            ReturnValue = CARD_STATUS_INVALID_CARD;
            DBGMSG("[AppLib - Card] <CheckStatus> CARD_STATUS_INVALID_CARD");
            goto _done;
        }
        if (Cards[CardId].Format == FS_FAT_CONTAIN_ERROR) {
            ReturnValue = CARD_STATUS_INVALID_FORMAT_CARD;
            DBGMSG("[AppLib - Card] <CheckStatus> CARD_STATUS_INVALID_FORMAT_CARD");
            goto _done;
        }
    }

    if (APPLIB_CHECKFLAGS(Cards[CardId].FlagsError, CARD_FLAGS_ER_INVALID)) {
        ReturnValue = CARD_STATUS_INVALID_CARD;
        DBGMSG("[AppLib - Card] <CheckStatus> CARD_STATUS_INVALID_CARD");
        goto _done;
    }

    if (!APPLIB_CHECKFLAGS(Cards[CardId].Flags, CARD_FLAGS_READY)) {
        ReturnValue = CARD_STATUS_REFRESHING;
        DBGMSG("[AppLib - Card] <CheckStatus> CARD_STATUS_REFRESHING");
        goto _done;
    }

    /** check card status when write */
    if (APPLIB_CHECKFLAGS(checkFlags, CARD_CHECK_WRITE)) {
        if (APPLIB_CHECKFLAGS(Cards[CardId].FlagsError, CARD_FLAGS_ER_FOLDER)) {
            ReturnValue=CARD_STATUS_ERROR_FORMAT;
            DBGMSG("[AppLib - Card] <CheckStatus> CARD_STATUS_ERROR_FORMAT");
            goto _done;
        }

        /** If it is the first to write, check the freespace. */
        Drive = AppLibCard_GetActiveDrive();
        Freespace = AppLibCard_GetFreeSpace(Drive);
        if (Freespace < CardThreshold) {
            ReturnValue = CARD_STATUS_NOT_ENOUGH_SPACE;
            DBGMSG("[AppLib - Card] <CheckStatus> CARD_STATUS_NOT_ENOUGH_SPACE");
            goto _done;
        }
    } else if (APPLIB_CHECKFLAGS(checkFlags, CARD_CHECK_CONT_WRITE)) {
        /*
        * This case only for cont. capture mode. The 2nd, 3rd, etc capture
        * freespace will use API AppLibCard_CheckFreespace to check.
        */
        if (APPLIB_CHECKFLAGS(Cards[CardId].FlagsError, CARD_FLAGS_ER_FOLDER)) {
            ReturnValue=CARD_STATUS_ERROR_FORMAT;
            DBGMSG("[AppLib - Card] <CheckStatus> CARD_STATUS_ERROR_FORMAT");
            goto _done;
        }
    }

_done:
    DBGMSGc2(BLUE, "[AppLib - Card] <CheckStatus> ReturnValue = %d", ReturnValue);
    return ReturnValue;
}


/**
 *  @brief Set the flag about card refreshing.
 *
 *  Set the flag about card refreshing.
 *
 *  @param [in] refreshing Enable
 *  @param [in] slot Slot id
 *
 *  @return >=0 success, <0 failure
 */
int AppLibCard_StatusSetRefreshing(int slot, int refreshing)
{
    int CardId = 0;

    DBGMSG("[AppLib - Card] <StatusSetRefreshing> %d", refreshing);
    CardId = AppLibCard_GetCardId(slot);
    if (refreshing) {
        APPLIB_REMOVEFLAGS(Cards[CardId].Flags, CARD_FLAGS_READY);
    } else {
        APPLIB_ADDFLAGS(Cards[CardId].Flags, CARD_FLAGS_READY);
    }
    return 0;
}

/**
 *  @brief Set the flag to block card inserted flow.
 *
 *  Set the flag to block card inserted flow.
 *
 *  @param [in] cardId Card id
 *  @param [in] en Enable
 *
 *  @return >=0 success, <0 failure
 */
int AppLibCard_StatusSetBlock(int cardId, int en)
{
    DBGMSG("[AppLib - Card] <StatusSetBlock> Set block slot: %d, en: %d", cardId, en);
    if (en) {
        APPLIB_ADDFLAGS(Cards[cardId].Flags, CARD_FLAGS_BLOCK_INSERT);
    } else {
        APPLIB_REMOVEFLAGS(Cards[cardId].Flags, CARD_FLAGS_BLOCK_INSERT);
    }

    return 0;
}

/**
 *  @brief Check the card that be blocked.
 *
 *  Check the card that be blocked.
 *
 *  @return >=0 card id, <0 failure
 */
int AppLibCard_StatusCheckHighProrityBlock(void)
{
    int i = 0;
    int ReturnValue = -1;

    for (i = 0; i < CARD_NUM; i++) {
        if (APPLIB_CHECKFLAGS(Cards[i].Flags, CARD_FLAGS_BLOCK_INSERT)) {
            DBGMSG("[AppLib - Card] <StatusCheckHighProrityBlock> Check highest priority insert block CardId: %d", i);
            ReturnValue = i;
            break;
        }
    }

    return ReturnValue;
}

/**
 *  @brief Check the card that could be blocked.
 *
 *  Check the card that could be blocked.
 *
 *  @param [in] CardId card id
 *
 *  @return >=0 blocked, <0 Not blocked.
 */
int AppLibCard_StatusCheckBlock(int cardId)
{
    int ReturnValue = -1;

    if (APPLIB_CHECKFLAGS(Cards[cardId].Flags, CARD_FLAGS_BLOCK_INSERT)) {
        DBGMSG("[AppLib - Card] <StatusCheckBlock> Check insert block cardId: %d", cardId);
        ReturnValue = 0;
    }

    return ReturnValue;
}

/**
 *  @brief Send message of card inserted.
 *
 *  Send message of card inserted.
 *
 *  @param [in] cardId Card id
 *
 *  @return >=0 success, <0 failure
 */
int AppLibCard_SndCardInsertMsg(int cardId)
{
    AppLibComSvcHcmgr_SendMsgNoWait(HMSG_JACK_CARD((CardSlotDic[cardId] << 8) | SCM_CARD_INSERTED), CardSlotDic[cardId], SCM_CARD_INSERTED);
    return 0;
}


/**
 *  @brief Check the free space of card
 *
 *  Check the free space of card
 *
 *  @param [in] drive Drive id
 *
 *  @return the size of freespace
 */
UINT64 AppLibCard_GetFreeSpace(char drive)
{
    UINT64 Size = 0;
    AMP_CFS_DEVINF DevIn;
    int ReturnValue = 0;
    ReturnValue = AmpCFS_GetDev(drive, &DevIn);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[AppLib - Card] <GetFreeSpace> Fail.");
    } else {
        Size = (UINT64)DevIn.Ucl * DevIn.Bps * DevIn.Spc;
    }
    return Size;
}

/**
 *  @brief Check the total space of card
 *
 *  Check the toal space of card
 *
 *  @param [in] drive Drive id
 *
 *  @return the size of freespace
 */
UINT64 AppLibCard_GetTotalSpace(char drive)
{
    UINT64 Size = 0;
    AMP_CFS_DEVINF DevIn;
    int ReturnValue = 0;
    ReturnValue = AmpCFS_GetDev(drive, &DevIn);
    if (ReturnValue < 0) {
        AmbaPrintColor(RED,"[AppLib - Card] <GetFreeSpace> Fail.");
    } else {
        Size = (UINT64)DevIn.Cls * DevIn.Bps * DevIn.Spc;
    }
    return Size;
}


/**
 *  @brief Check the valid size of NAND storage
 *
 *  Check the valid size of NAND storage
 *
 *  @param [in] cardId Card id
 *
 *  @return >=0 success, <0 failure
 */
int AppLibCard_CheckNandStorage(int cardId)
{
    int size = 0;
    AMBA_SCM_STATUS_s CardStatus;

    AmbaSCM_GetSlotStatus(AppLibCard_GetSlot(cardId), &CardStatus);

    /** Don't use small NAND as storage */
    size = CardStatus.SecsCnt*CardStatus.SecSize;
    DBGMSG("[AppLib - Card] <CheckNandStorage> Nand size = %d (min = %d)", size, CARD_MIN_NAND_STORAGE_SIZE);
    if (size < CARD_MIN_NAND_STORAGE_SIZE) {
        return 0;
    } else {
        return 1;
    }
}

/**
 *  @brief Check the free space of card
 *
 *  Check the free space of card
 *
 *  @return >=0 Enough space, <0 Not enough space
 */
int AppLibCard_CheckFreespace(void)
{
    int ReturnValue = 0;
    UINT64 Freespace = 0;
    char Drive = 'A';

    Drive = AppLibCard_GetActiveDrive();
    Freespace = AppLibCard_GetFreeSpace(Drive);
    if (Freespace < CardThreshold) {
        ActiveCardStatus.CardFreeSpace = -1;
        DBGMSG("[AppLib - Card] <CheckFreespace> CARD_STATUS_NOT_ENOUGH_SPACE");
    } else {
        ActiveCardStatus.CardFreeSpace = 0;
        DBGMSG("[AppLib - Card] <CheckFreespace> CARD_STATUS_ENOUGH_SPACE");
    }

    return ReturnValue;
}

/**
 *  @brief Get the flag of free space.
 *
 *  Get the flag of free space.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibCard_GetFreespaceFlag(void)
{
    DBGMSG( "[AppLib - Card] <GetFreespaceFlag> ActiveCardStatus.CardFreeSpace = %d", ActiveCardStatus.CardFreeSpace);
    return ActiveCardStatus.CardFreeSpace;
}

#if 0
/***************************
* FIO init callback function implementation
* It provide a way to modify the FIO init order and change delay time of each slot.
* params:
*    CardPresent - the card presence of each slot. it will be set to 1 if card is inserted in that slot.
*    card_seq - the FIO init order. card_seq[0] is inited first.
*    card_init_delay - the init delay (ms) of each slot. set to 0 means not delay.
****************************/
void fio_set_init_flow(const UINT8 *CardPresent, int *card_seq, int *card_init_delay)
{
    int i,slot;

//1.assign fio init priority
#if 0
//Default priority: (already done in fio_amb.c)
    card_seq[0]=SCM_SLOT_SD0;
    card_seq[2]=SCM_SLOT_SD1;
    card_seq[5]=SCM_SLOT_FL;
    card_seq[6]=SCM_SLOT_FL1;
#endif


//2.assign slot init delay
#if 0
//Set to default value. (already done in stktask.c)
#if defined(SD_INIT_DELAY_MS)
    card_init_delay[SCM_SLOT_SD0]=SD_INIT_DELAY_MS;
#endif
#if defined(SD2_INIT_DELAY_MS)
    card_init_delay[SCM_SLOT_SD1]=SD2_INIT_DELAY_MS;
#endif
#if defined(FL_INIT_DELAY_MS)
    card_init_delay[SCM_SLOT_FL]=FL_INIT_DELAY_MS;
#endif
#if defined(FL2_INIT_DELAY_MS)
    card_init_delay[SCM_SLOT_FL1]=FL2_INIT_DELAY_MS;
#endif
#endif

//3.check card present to adjust the delay time
    //The order to check present is based on the priority of Card.
    for (i=0;i<SCM_SLOT_CARDS;i++) {
        slot=card_seq[i];
        if (CardPresent[slot]) { //higher priority Card present
            card_init_delay[slot]=0;
            break;
        }
    }

    if (slot==SCM_SLOT_FL) {
        if (CardPresent[SCM_SLOT_FL1]) { //FL2 present at boot
            card_init_delay[SCM_SLOT_FL1]=0;
        }
    }
}

/**
 * @brief Check left space callback function implementation
 *        (this is a weak-linked function for stktask)
 * @param data inFormation for slot and event (0xaabb: 0xaa is the slot / 0xbb is the eid)
 */
void hook_fs_check_left_space_done(UINT32 data)
{
    /* Check if the slot has left space larger than 10 MB */
    DBGMSG("[AppLib - Card] <hook_fs_check_left_space_done> data = 0x%X", data);
    if ((data & 0xFF) == SCARDMGR_CARD_CHK_LEFT_SPACE) {
        /*
         * If the slot has free space larger than 10 MB, set the correspondingfree-space flag
         * free-space flag.
         */
        int slot = (data >> 8) & 0xFF;
        int cardId = AppLibCard_GetCardId(slot);
        DBGMSG("<SCARDMGR_CARD_CHK_LEFT_SPACE> slot = %d", slot);
        if (cardId >= 0) {
            DBGMSG("CardInitFreespaceFlags[%d] = 1", cardId);
            CardInitFreespaceFlags[cardId] = 1;
        }
    }
}

/***************
* disable FIO init delay of each slot after boot finish
***************/
void AppLibCard_disable_boottime_dly(void)
{
#if defined(SD_INIT_DELAY_MS)
    stk_set_init_delay(SCM_SLOT_SD0,0);
#endif
#if defined(SD2_INIT_DELAY_MS)
    stk_set_init_delay(SCM_SLOT_SD1,0);
#endif
#if defined(FL_INIT_DELAY_MS)
    stk_set_init_delay(SCM_SLOT_FL,0);
#endif
#if defined(FL2_INIT_DELAY_MS)
    stk_set_init_delay(SCM_SLOT_FL1,0);
#endif
}
#endif
