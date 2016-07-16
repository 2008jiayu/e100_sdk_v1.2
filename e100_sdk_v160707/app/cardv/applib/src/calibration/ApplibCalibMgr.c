/**
 * @file src/app/connected/applib/src/calibration/ApplibCalibMgr.c
 *
 * sample code for Calibration Manager control
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
#include <calibration/ApplibCalibMgr.h>
#include <AmbaDataType.h>
#include <AmbaPrintk.h>
#include <AmbaFS.h>
#include <common/common.h>
#include <AmbaUtility.h>
#include <AmbaDSP.h>
#include <AmbaSensor.h>
#include <AmbaCardManager.h>
#include <AmbaNAND_Def.h>
#include <AmbaNFTL.h>
#include <calibration/ca/ApplibCalibCa.h>
#include <calibration/warp/ApplibCalibWarp.h>
#include <calibration/vig/ApplibCalibVig.h>
#include <calibration/bpc/ApplibCalibBpc.h>
#include <AmbaROM.h>
#ifdef CONFIG_ENABLE_EMMC_BOOT
#include <AmbaPartition_Def.h>
extern int AmbaEMMC_MediaPartRead(int ID, UINT8 *pBuf, UINT32 Sec, UINT32 Secs);
extern int AmbaEMMC_MediaPartWrite(int ID, UINT8 *pBuf, UINT32 Sec, UINT32 Secs);
#endif

UINT8 *ScriptAddr = NULL;
UINT8 *RawBuffaddr = NULL;

#define STILL_IDLE          1
#define STILL_VEVIEW        2
#define STILL_RAWCAP        3
#define STILL_RAWENC        4
#define STILL_LV_RAWENC     5


//#define CALIB_MGR_DEBUG

#ifdef CALIB_MGR_DEBUG
#define CalMgrPrint     AmbaPrint
#else
#define CalMgrPrint(...)
#endif

AMBA_DSP_IMG_MODE_CFG_s CalibImgMode;

int AppLib_CalibNandSave(UINT32 CalId , UINT32 SubId);

// define WARP table head loaded flag
static UINT8 GWarpTableHeadLoaded = 0;
static UINT8 GCATableHeadLoaded = 0;
static UINT8 GVignetteTableHeadLoaded = 0;
static UINT8 GBPCTableHeadLoaded = 0;

// define WARP table loaded flag
UINT8 GWarpTableLoaded[MAX_WARP_TABLE_COUNT] = {0};
UINT8 GCATableLoaded[MAX_CA_TABLE_COUNT] = {0};
UINT8 GVignetteTableLoaded[MAX_VIGNETTE_GAIN_TABLE_COUNT] = {0};
UINT8 GBPCTableLoaded[BPC_MAX_PAGES] = {0};
UINT8 GWBTableLoaded = 0,GBLCTableLoaded = 0, GAFTableLoaded = 0, GGyroTableLoaded = 0, GMShutterTableLoaded = 0;
UINT8 GIrisTableLoaded = 0,GISOTableLoaded = 0, GFlashTableLoaded = 0, GAudioTableLoaded = 0;

/* --------- */
UINT8 CalFormat[SITE_NUM+1] = {
    CAL_STATUS_LOAD_FORMAT,
    CAL_AF_LOAD_FORMAT,
    CAL_GYRO_LOAD_FORMAT,
    CAL_MSHUTTER_LOAD_FORMAT,
    CAL_IRIS_LOAD_FORMAT,
    CAL_VIGNETTE_LOAD_FORMAT,
    CAL_WARP_LOAD_FORMAT,
    CAL_FPN_LOAD_FORMAT,
    CAL_WB_LOAD_FORMAT,
    CAL_ISO_LOAD_FORMAT,
    CAL_BLC_LOAD_FORMAT,
    CAL_FLASH_LOAD_FORMAT,
    CAL_AUDIO_LOAD_FORMAT,
    CAL_CA_LOAD_FORMAT,
    CAL_LENSSHIFT_LOAD_FORMAT
};

/**
 *  @brief get calibration object
 *
 *  get calibration object
 *
 *  @param [in]CalId calibration ID
 *
 *  @return calibration object table
 */
Cal_Obj_s* AppLib_CalibGetObj(UINT32 CalId)
{
    return &(CalObjTable[CalId]);
}

/**
 *  @brief calculate sectors occupied in NAND
 *
 *  calculate sectors occupied in NAND
 *
 *  @param [in]Size Number of bytes
 *
 *  @return sector number
 */
UINT32 AppLib_CalibGetNandSecNum(UINT32 Size)
{
    return (Size+511)>>9;
}

/**
 *  @brief calculate start sector in NAND
 *
 *  calculate start sector in NAND
 *
 *  @param [in]CalId calibration ID
 *
 *  @return sector ID
 */
UINT32 AppLib_CalibGetNandSecStart(UINT32 CalId)
{
    UINT8 i;
    Cal_Obj_s *CalObj;
    UINT32 Sec = 0;

    for (i = 0; i < CalId; i++) {
        CalMgrPrint("%d",i);
        CalObj = AppLib_CalibGetObj(i);
        Sec += (CalObj->Size+511)>>9;
        CalMgrPrint("CalObj[%d] size = %d startsec = %d",i,CalObj->Size,Sec);
    }

    return Sec;
}

/**
 *  @brief get SD card slot
 *
 *  get SD card slot
 *
 */
char AppLib_CalibGetDriverLetter(void)
{
    AMBA_SCM_STATUS_s ScmStatus;
    char driver = { 'c' };

    AmbaSCM_GetSlotStatus(SCM_SLOT_SD0, &ScmStatus);
    if (ScmStatus.Format > 0) {
        driver = AmbaUtility_Slot2Drive(SCM_SLOT_SD0);
    } else {
        AmbaSCM_GetSlotStatus(SCM_SLOT_SD1, &ScmStatus);
        if (ScmStatus.Format > 0)
            driver = AmbaUtility_Slot2Drive(SCM_SLOT_SD1);
        else {
            AmbaPrint("%s() %d, Please insert SD Card", __func__, __LINE__);
            driver = 'z';
        }
    }
    return driver;
}

/**
 *  @brief get calibration NAND status
 *
 *  get calibration NAND status
 *
 *  @param [in]CalId calibration ID
 *  @param [out]Offset offset number of the calibration site
 *  @param [out]SectorNum number of sector for the calibration site
 *
 */
Cal_Obj_s* AppLib_CalibGetNandStatus(UINT32 CalId,UINT32 *Offset, UINT32 *SectorNum)
{
    UINT8 i;
    Cal_Obj_s *CalObj;
    UINT32 SiteSize;

    *Offset = 0;
    *SectorNum = 0;

    for (i = 0; i < CalId; i++) {
        CalObj = AppLib_CalibGetObj(i);
        SiteSize = (((CalObj->Size+511)>>9)<<9);
        *Offset += SiteSize;
    }
    CalObj = AppLib_CalibGetObj(CalId);
    *SectorNum = (CalObj->Size+511)>>9;
    *Offset = *Offset>>9;
    return 0;
}


/**
 *  @brief get calibration site status
 *
 *  get calibration NAND status
 *
 *  @param [in]CalId calibration ID
 *
 *  @return calibration site status
 */
Cal_Stie_Status_s* AppLib_CalibGetSiteStatus(UINT32 CalId)
{
    Cal_Status_s *CalStatus;

    K_ASSERT(CalId < NVD_CALIB_MAX_OBJS);
    CalStatus = (Cal_Status_s*)( AppLib_CalibGetObj(CAL_STATUS_ID)->DramShadow );

    return &( CalStatus->Site[CalId] );
}

/**
 *  @brief  reset calibration site status
 *
 *   reset calibration site status
 *
 *  @param [in]CalId calibration ID
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibResetSiteStatus(UINT32 CalId)
{
    Cal_Stie_Status_s *CalSite;

    CalSite = AppLib_CalibGetSiteStatus(CalId);
    memset(CalSite, 0, sizeof(Cal_Stie_Status_s));

    return 0;
}


/**
 *  @brief  Write Status data to NAND flash
 *
 *   Write Status data to NAND flash
 *
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibSaveStatus(void)
{
    return AppLib_CalibNandSave(CAL_STATUS_ID,0);
}

/**
 *  @brief get multiple lines from the text file
 *
 *  get multiple lines from the text file
 *
 *  @param [in]Fp file pointer
 *  @param [out]Buf output buffer
 *
 *  @return 0 success, -1 failure
 */
int AppLib_MultiGetline(AMBA_FS_FILE *Fp, char Buf[])
{
    unsigned char ch;
    // Normal state
    do {
        if (AmbaFS_fread(&ch, 1, 1, Fp) == 0) { // read 1 byte
            return -1;
        }
        if ( (ch == '\n') || (ch == '\r') ) {
            break;  // go to end-of-line status
        }
        *Buf = ch;
        Buf++;
    } while (1);
    // End of line state
    do {
        if (AmbaFS_fread(&ch, 1, 1, Fp) == 0) { // read 1 byte
            break;
        }
        if ( (ch == '\n') || (ch == '\r') ) {
            /* do nothing */
        } else {
            // Reach to next line, roll back 1 byte
            AmbaFS_fseek(Fp, -1, SEEK_CUR);
            break;
        }
    } while (1);
    *Buf = '\0';
    return 0;
}

/**
 *  @brief reset calibration data to zero
 *
 *  reset calibration data to zero
 *
 *  @param [in]CalId calibration ID
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibMemReset(UINT32 CalId)
{
    Cal_Obj_s *CalObj;
    CalObj = AppLib_CalibGetObj(CalId);

    memset(CalObj->DramShadow, 0, CalObj->Size);
    AppLib_CalibResetSiteStatus(CalId);
    return 0;
}


/**
 *  @brief Clear NVD partition with zero initialized
 *
 *  Clear NVD partition with zero initialized
 *
 *  @param [in]CalId calibration ID
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibNandReset(UINT32 CalId)
{
    Cal_Obj_s *CalObj;
    UINT32 Offset=0,SectorNum=0;
    //UINT8 *CalDramShadow;
    int Rval;

    AppLib_CalibMemReset(CalId);
    CalObj = AppLib_CalibGetObj(CalId);
    AppLib_CalibGetNandStatus(CalId,&Offset, &SectorNum);

    Rval = AppLib_CalibNandSave( CalId, 0);
    if (Rval == OK) {
        CalMgrPrint("[CAL] Reset site %s %d success", CalObj->Name, CalId);
        return 0;
    } else {
        CalMgrPrint("[CAL] Reset site %s %d fail", CalObj->Name, CalId);
        return -1;
    }
}


/**
 *  @brief Clear SD card calibration partition with zero initialized
 *
 *  Clear SD card calibration partition with zero initialized
 *
 *  @param [in]CalId calibration ID
 *
 *  @return 0 success, -1 failure
 */
 int AppLib_CalibSDCardReset(UINT32 CalId)
{
    Cal_Obj_s *CalObj;
    AMBA_FS_FILE *Fid = NULL;
    char FileName[20] = {'c',':','\\','c','a','l','i','b'};
    char tmp[10] = {0};

    AppLib_CalibMemReset(CalId);
    CalObj = AppLib_CalibGetObj(CalId);

    //write to SD card
    sprintf(tmp,"%d.bin",(int)CalId);
    strcat(FileName, tmp);
    Fid = AmbaFS_fopen(FileName,"w");
    if (Fid == NULL) {
        AmbaPrint("fopen %s fail.",FileName);
        return -1;
    }
    AmbaFS_fwrite(CalObj->DramShadow,CalObj->Size, 1, Fid);
    AmbaFS_fclose(Fid);
    AppLib_CalibRestCheckFlag(CalId);
    return 0;
}


/**
 *  @brief clear calibration data for partial load mode
 *
 *  clear calibration data for partial load mode
 *
 *  @param [in]CalId calibration ID
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibResetPartial(UINT32 CalId) //for vignette partial load
{
    Cal_Obj_s *CalObj;
    char *CalName;
    int CalSubId; //the ID has been recognize in nand
    //UINT32 CalSize;
    //UINT8 *CalDramShadow;
    K_ASSERT(CalId < NVD_CALIB_MAX_OBJS);
    CalObj = AppLib_CalibGetObj(CalId);
    CalName = CalObj->Name;

    memset(CalObj->DramShadow, 0, CalObj->Size);
    AppLib_CalibResetSiteStatus(CalId);

    if (CalId == CAL_VIGNETTE_ID) {
        for (CalSubId = CAL_VIGNETTE_PARTIAL_ID_(0); CalSubId <CAL_VIGNETTE_PARTIAL_ID_(MAX_VIGNETTE_GAIN_TABLE_COUNT); CalSubId++) {
            // The first vig table ID in NAND is CAL_VIGNETTE_PARTIAL_LOAD_START_ID (20)
            //CalDramShadow = CalObj->DramShadow + ((CalSubId-CAL_VIGNETTE_PARTIAL_ID_(0)) * sizeof(Vignette_Pack_Storage_s)) + CAL_VIGNETTE_TABLE_BASE;
            //the shift 4 byte is vignette Enable & table count, that are only appear in the start addr
            //CalSize = sizeof(Vignette_Pack_Storage_s);//for one vignette table Size
            //bug
            //if (AmbaNVD_Save(NVD_ID_CALIB, CalSubId, CalDramShadow, CalSize) == 0) {
            if (0) {
                CalMgrPrint("[CAL] Reset site %s %d partial success", CalName,CalSubId);
            } else {
                CalMgrPrint("[CAL] Reset site %s %d  partial fail", CalName,CalSubId);
                return -1;
            }
        }
    }

    return 0;
}


/**
 *  @brief Check the calibration data size with the defined size
 *
 *  Check the calibration data size with the defined size
 *
 *  @param [in]CalId calibration ID
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibCheckSize(UINT32 CalId)
{
    Cal_Obj_s *CalObj;
    char *CalName;
    int Rval = OK;
    UINT32 TotalSize;

    CalObj = AppLib_CalibGetObj(CalId);
    CalName = CalObj->Name;

    switch (CalId) {
        case CAL_VIGNETTE_ID:
            TotalSize = CAL_VIGNETTE_TABLE_BASE + sizeof(Vignette_Pack_Storage_s)*MAX_VIGNETTE_GAIN_TABLE_COUNT;
            if (TotalSize > CalObj->Size) {
                Rval = NG;
            }
            break;
        case CAL_WARP_ID: 
            TotalSize = CAL_WARP_TABLE_BASE + sizeof(Warp_Storage_s)*MAX_WARP_TABLE_COUNT;
            if (TotalSize > CalObj->Size) {
                Rval = NG;
            }
            break;
        case CAL_CA_ID: 
            TotalSize = CAL_CA_TABLE_BASE + sizeof(CA_Storage_s)*MAX_CA_TABLE_COUNT;
            if (TotalSize > CalObj->Size) {
                Rval = NG;
            }
            break;
        default:
            break;
    }
    if(Rval != OK) {
        AmbaPrintColor(RED,"[CAL]Please check the size of calibration site %s, wanted size = %d defined size = %d",CalName,TotalSize, CalObj->Size);
    }
    return Rval;
}

/**
 *  @brief Load calibration table head data from NAND to DRAM
 *
 *  Load calibration table head data from NAND to DRAM
 *
 *  @param [in]CalId calibration ID
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibNandLoadTableHead(UINT32 CalId)
{
    Cal_Obj_s *CalObj;
    char *CalName;
    UINT32 CalSize;
    UINT8 *CalDramShadow;
    //UINT8 CalSubId = CalId; //the ID has been recognize in nand
    int Rval;
    UINT32 Sec, SecNum;

    K_ASSERT(CalId < NVD_CALIB_MAX_OBJS);

    CalObj = AppLib_CalibGetObj(CalId);
    CalName = CalObj->Name;
    CalSize = CalObj->Size;
    CalDramShadow = CalObj->DramShadow;


    Sec = AppLib_CalibGetNandSecStart(CalId);
    switch (CalId) {
        case CAL_VIGNETTE_ID:
            SecNum = 1;
            break;
        case CAL_BPC_ID:
            SecNum = (sizeof(BPC_Nand_s) -4 + 511) >>9 ; //temp for head should be less than one nand block (512 bytes)
            break;
        default:
            CalMgrPrint("Only BPC and VNC load from nand");
            return -1;
            break;
    }
    CalMgrPrint("Head: Sec = %d , SecNum = %d",Sec,SecNum);
#ifdef CONFIG_ENABLE_EMMC_BOOT
    Rval = AmbaEMMC_MediaPartRead(MP_CalibrationData, CalDramShadow, Sec, SecNum);
#else
    Rval = AmbaNFTL_Read(NFTL_ID_CAL, CalDramShadow, Sec, SecNum);
#endif

    if (Rval == OK) {
        CalMgrPrint("[CAL] Site %s %d load success, Size=%d", CalName, CalId, CalSize);
        return 0;
    } else {
        CalMgrPrint("[CAL] Site %s %d load fail, Size=%d", CalName, CalId, CalSize);
        return -1;
    }
}


/**
 *  @brief Load calibration table data from NAND to DRAM
 *
 *  Load calibration table data from NAND to DRAM
 *
 *  @param [in]CalId calibration ID
 *  @param [in]TableIdx entry table id for loading
 *  @param [in]TableNums number of tables for loading
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibNandLoadTable(UINT32 CalId, UINT32 TableIdx, UINT32 TableNums)
{
    Cal_Obj_s *CalObj;
    char *CalName;
    UINT32 CalSize;
    UINT8 *CalDramShadow;
    UINT32 CalDramStart;
    UINT32 TotalSecNums=0;
    UINT32 SecStart;
    //UINT8 CalSubId = CalId; //the ID has been recognize in nand
    int Rval;
    UINT32 Sec, SecNum;
    UINT8 StartTable,Count;


    CalObj = AppLib_CalibGetObj(CalId);
    CalName = CalObj->Name;
    CalSize = CalObj->Size;
    CalDramShadow = CalObj->DramShadow;

    if (AppLib_CalibCheckHeadFlag(CalId) == 0) {
        AppLib_CalibNandLoadTableHead(CalId);
    }


    Sec = AppLib_CalibGetNandSecStart(CalId);
    StartTable = TableIdx;
    Count = TableNums;    
    switch (CalId) {
        case CAL_VIGNETTE_ID:
            SecStart = Sec + 1;
            if (TableIdx == CALIB_FULL_LOAD) {
                StartTable = 0;
                Count = MAX_VIGNETTE_GAIN_TABLE_COUNT;
            } else if ((TableIdx+TableNums) > MAX_VIGNETTE_GAIN_TABLE_COUNT) {
                Count = MAX_VIGNETTE_GAIN_TABLE_COUNT-TableIdx;
            } else if (TableIdx > MAX_VIGNETTE_GAIN_TABLE_COUNT) {
                AmbaPrint("VNC data out of range, please check the parameter");
                return -1;
            }
            SecNum = AppLib_CalibGetNandSecNum(sizeof(Vignette_Pack_Storage_s));
            SecStart += StartTable * SecNum;
            TotalSecNums = SecNum* Count;
            CalDramStart = (SecStart -Sec)* 512;
            break;
        case CAL_BPC_ID: {
            BPC_Nand_s *BPCNand =  (BPC_Nand_s *) CalObj->DramShadow;
            SecStart = Sec +((sizeof(BPC_Nand_s) -4 + 511) >>9) ;
            if (TableIdx == CALIB_FULL_LOAD) {
                StartTable = 0;
                Count = BPC_MAX_PAGES;
            } else if ((TableIdx+TableNums) > BPC_MAX_PAGES) {
                Count = BPC_MAX_PAGES-TableIdx;
            } else if (TableIdx > BPC_MAX_PAGES) {
                AmbaPrint("BPC data out of range, please check the parameter");
                return -1;
            }
            
            for (int i=0; i<StartTable; i++) {
                SecStart += AppLib_CalibGetNandSecNum(BPCNand->BPCPage[i].PageSize);
                CalMgrPrint("Pre  Table:%d PageSize:%d", i, BPCNand->BPCPage[i].PageSize);
            }
            for (int i=0; i<Count; i++) {
                if(BPCNand->BPCPage[StartTable+i].PageSize <= CAL_BPC_SIZE) {
                    TotalSecNums += AppLib_CalibGetNandSecNum(BPCNand->BPCPage[StartTable+i].PageSize);
                } else {
                    AmbaPrint("BPC PageSize incorrect %d, it should be smaller than %d",(BPCNand->BPCPage[StartTable+i].PageSize),CAL_BPC_SIZE);
                    return -1;
                }
                CalMgrPrint("Post Table:%d PageSize:%d", StartTable+i, BPCNand->BPCPage[StartTable+i].PageSize);
            }
            if(TotalSecNums > (CAL_BPC_SIZE>>9)) {
                AmbaPrint("BPC PageSize too big %d, it should be smaller than %d",(TotalSecNums<<9),CAL_BPC_SIZE);
                return -1;
            }
            
            CalDramStart = (SecStart -Sec)* 512;
            break;
        }
        default:
            SecStart = Sec;
            StartTable = 0;
            Count = 1;
            SecNum = AppLib_CalibGetNandSecNum(CalObj->Size);
            TotalSecNums = SecNum;
            CalDramStart = 0;
            break;
    }

#ifdef CONFIG_ENABLE_EMMC_BOOT
    Rval = AmbaEMMC_MediaPartRead(MP_CalibrationData, CalDramShadow + CalDramStart , SecStart, TotalSecNums);
#else
    Rval = AmbaNFTL_Read(NFTL_ID_CAL, CalDramShadow + CalDramStart , SecStart, TotalSecNums);
#endif

    if (Rval == OK) {
        CalMgrPrint("[CAL] Site %s %d load success, Size=%d", CalName, CalId, CalSize);
        Rval = 0;
    } else {
        CalMgrPrint("[CAL] Site %s %d load fail, Size=%d", CalName, CalId, CalSize);
        Rval = -1;
    }

//CheckSum
    switch (CalId) {
        case CAL_VIGNETTE_ID: {
            for (int i=StartTable; i < StartTable + Count; i++) {
                Vignette_Pack_Storage_s *VNCNand = (Vignette_Pack_Storage_s *)(CalDramShadow+ CAL_VIGNETTE_TABLE(i)) ;
                if (i >= MAX_VIGNETTE_GAIN_TABLE_COUNT) {
                    AmbaPrint("VNC data out of range, please check the parameter");
                    break;
                } 
                if ( VNCNand->Enable == 1 ) {
                    UINT32 TmpCheckSum = 0;
                    
                    if(AppLib_CalibCheckFlag(CalId, i) == 1) {
                        continue;
                    }
                    TmpCheckSum = AmbaUtility_Crc32((UINT32 *) (VNCNand), sizeof(Vignette_Pack_Storage_s));
                    if (memcmp(CalDramShadow + CAL_VIGNETTE_CHECKSUM(i), &TmpCheckSum, sizeof(UINT32)) == 0) {
                        AppLib_CalibMarkFlag(CalId, i, 1);
                        CalMgrPrint("CheckSum Pass. checksum nand:%x checksum ram:%x ",*(UINT32 *)(CalDramShadow + CAL_VIGNETTE_CHECKSUM(i)),TmpCheckSum);
                        Rval =  0;
                    } else {
                        CalMgrPrint("CheckSum Fail %x." , TmpCheckSum);
                        Rval =  -1;
                    }
                } else {
                    CalMgrPrint("Vig Page:%d not Enable", i );
                }
            }
            break;
        }
        case CAL_BPC_ID: {
            BPC_Nand_s *BPCNand =  (BPC_Nand_s *) CalObj->DramShadow;
            for (int i=StartTable; i<StartTable+Count; i++) {
                if (i >= BPC_MAX_PAGES) {
                    AmbaPrint("BPC data out of range, please check the parameter");
                    break;
                }                
                if ( BPCNand->BPCPage[i].PageValid == 1 ) {
                    UINT32 TmpCheckSum =  0;
                    if(AppLib_CalibCheckFlag(CalId, i) == 1) {
                        continue;
                    }
                    TmpCheckSum = AmbaUtility_Crc32( (UINT8 *)&BPCNand-> Data +  BPCNand->BPCPage[i].Offset  , BPCNand->BPCPage[i].PageSize);
                    CalMgrPrint("BPCNandDataAddr: %08x Offset:%d LastAddr:%08x Pagesize: %d",   &BPCNand-> Data , BPCNand->BPCPage[i].Offset, (UINT32 *)&BPCNand-> Data +  BPCNand->BPCPage[i].Offset,BPCNand->BPCPage[i].PageSize);
                    if ( BPCNand->BPCPage[i].CheckSum == TmpCheckSum) {
                        AppLib_CalibMarkFlag(CalId, i, 1);
                        CalMgrPrint("CheckSum Pass. checksum nand:%x checksum ram:%x ",BPCNand->BPCPage[i].CheckSum,TmpCheckSum );
                        Rval =  0;
                    } else {
                        CalMgrPrint("CheckSum Fail %x." , TmpCheckSum);
                        Rval =  -1;
                    }
                } else {
                    CalMgrPrint("BPC Page:%d not Enable",i );
                }
            }
            break;
        }
        default: {
            UINT32 NandCheckSum = *(UINT32 *)(CalDramShadow+ (CalObj->Size-4));
            UINT32 TmpCheckSum = 0;
            if(AppLib_CalibCheckFlag(CalId, 0) == 1) {
                break;
            }
            TmpCheckSum = AmbaUtility_Crc32(CalDramShadow, CalObj->Size - 4);
            if (NandCheckSum == TmpCheckSum) {
                CalMgrPrint("CalID:%d CheckSum Pass:%x", CalId, TmpCheckSum);
                AppLib_CalibMarkFlag(CalId, 0, 1);
                Rval =  0;
            } else {
                CalMgrPrint("CalID:%d CheckSum Fail: CalDram:%x Tmp:%x ", CalId , NandCheckSum, TmpCheckSum);
                Rval =  -1;
            }
            break;
        }
    }
    return Rval;

}



/**
 *  @brief Load calibration data from NAND to DRAM
 *
 *  Load calibration data from NAND to DRAM
 *
 *  @param [in]CalId calibration ID
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibNandLoad(UINT32 CalId)
{
    Cal_Obj_s *CalObj;
    char *CalName;
    UINT32 CalSize;
    UINT8 *CalDramShadow;
    //UINT8 CalSubId = CalId; //the ID has been recognize in nand
    UINT32 Offset=0,SectorNum=0;
    int Rval;

    CalObj = AppLib_CalibGetObj(CalId);
    CalName = CalObj->Name;
    CalSize = CalObj->Size;
    CalDramShadow = CalObj->DramShadow;

    AppLib_CalibGetNandStatus(CalId,&Offset, &SectorNum);

#ifdef CONFIG_ENABLE_EMMC_BOOT
    Rval = AmbaEMMC_MediaPartRead(MP_CalibrationData, CalDramShadow, Offset, SectorNum);
#else
    Rval = AmbaNFTL_Read(NFTL_ID_CAL, CalDramShadow, Offset, SectorNum);
#endif
    if (Rval == OK) {
        CalMgrPrint("[CAL] Site %s %d load success, Size=%d", CalName, CalId, CalSize);
        Rval = 0;
    } else {
        CalMgrPrint("[CAL] Site %s %d load fail, Size=%d", CalName, CalId, CalSize);
        Rval = -1;
    }

//CheckSum
    switch (CalId) {
        case CAL_VIGNETTE_ID: {
            for (int i=0; i < MAX_VIGNETTE_GAIN_TABLE_COUNT; i++) {
                Vignette_Pack_Storage_s *VNCNand = (Vignette_Pack_Storage_s *)(CalDramShadow+ CAL_VIGNETTE_TABLE(i)) ;
                if ( VNCNand->Enable == 1 ) {
                    UINT32 TmpCheckSum = AmbaUtility_Crc32((UINT32 *) (VNCNand), sizeof(Vignette_Pack_Storage_s));
                    if (memcmp(CalDramShadow + CAL_VIGNETTE_CHECKSUM(i), &TmpCheckSum, sizeof(UINT32)) == 0) {
                        CalMgrPrint("Vig Page:%d CheckSum Pass. checksum nand:%x checksum ram:%x ", i, *(UINT32 *)(CalDramShadow + CAL_VIGNETTE_CHECKSUM(i)),TmpCheckSum);
                    } else {
                        CalMgrPrint("Vig Page:%d CheckSum Fail %x." , i, TmpCheckSum);
                        Rval =  -1;
                    }
                } else {
                    CalMgrPrint("Vig Page:%d not Enable", i );
                }
            }
            break;
        }
        case CAL_BPC_ID: {
            BPC_Nand_s *BPCNand =  (BPC_Nand_s *) CalObj->DramShadow;
            for (int i=0; i<BPC_MAX_PAGES; i++) {
                if ( BPCNand->BPCPage[i].PageValid == 1 ) {
                    UINT32 TmpCheckSum = AmbaUtility_Crc32( (UINT8 *)&BPCNand-> Data +  BPCNand->BPCPage[i].Offset  , BPCNand->BPCPage[i].PageSize);
                    if ( BPCNand->BPCPage[i].CheckSum == TmpCheckSum) {
                        CalMgrPrint("BPC Page:%d CheckSum Pass. checksum nand:%x checksum ram:%x ",i,BPCNand->BPCPage[i].CheckSum,TmpCheckSum );
                    } else {
                        CalMgrPrint("BPC Page:%d CheckSum Fail %x." ,i, TmpCheckSum);
                        Rval =  -1;
                    }
                } else {
                    CalMgrPrint("BPC Page:%d not Enable",i );
                }
            }
            break;
        }
        default: {
            UINT32 NandCheckSum = *(UINT32 *)(CalDramShadow+ (CalObj->Size-4));
            UINT32 TmpCheckSum = AmbaUtility_Crc32(CalDramShadow, CalObj->Size - 4);
            if (NandCheckSum == TmpCheckSum) {
                CalMgrPrint("CalID:%d CheckSum Pass:%x", CalId, TmpCheckSum);
            } else {
                CalMgrPrint("CalID:%d CheckSum Fail: CalDram:%x Tmp:%x ", CalId , NandCheckSum, TmpCheckSum);
                Rval =  -1;
            }
            break;
        }
    }

    return Rval;
}

/**
 *  @brief initialize Calibration Nand
 *
 *  initialize Calibration Nand
 *
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibNandInit(void)
{
    int Rval;
    AppLib_CalibRestCheckAllFlag();
#ifdef CONFIG_ENABLE_EMMC_BOOT
    Rval = 0;
#else
    AmbaNFTL_InitLock(NFTL_ID_CAL);
    Rval = AmbaNFTL_Init(NFTL_ID_CAL,0);
#endif
    return Rval;
}

/**
 *  @brief Load calibration data from SD card to DRAM
 *
 *  Load calibration data from SD card to DRAM
 *
 *  @param [in]CalId calibration ID
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibSDCardLoad(UINT32 CalId)
{
    Cal_Obj_s *CalObj;
    //char *CalName;
    UINT32 CalSize;
    UINT8 *CalDramShadow;
    UINT8 CalSubId = CalId; //the ID has been recognize in nand
    char FileName[16] = {'c',':','\\','c','a','l','i','b'};
    char tmp[16] = {0};
    AMBA_FS_FILE *Fid = NULL;

    Cal_Stie_Status_s *PCalSite;//modify vig Status to 0xAAFFAAFF

    K_ASSERT(CalId < NVD_CALIB_MAX_OBJS);
    if (CalId >=CAL_VIGNETTE_PARTIAL_LOAD_START_ID) {
        CalSubId = CalId;
        CalId = CAL_VIGNETTE_ID;
    }
    CalObj = AppLib_CalibGetObj(CalId);
    //CalName = CalObj->Name;
    CalSize = CalObj->Size;
    CalDramShadow = CalObj->DramShadow;
    PCalSite = AppLib_CalibGetSiteStatus(CalId);

    if (PCalSite->Status == CAL_SITE_DONE_VIGNETTE_LOAD_PARTIAL && CalId == CAL_VIGNETTE_ID) {
        CalObj->DramShadow[CAL_VIGNETTE_ENABLE] = PCalSite->Reserved[0]; //set the vignette Enable from status0(NAND) to dram
        CalObj->DramShadow[CAL_VIGNETTE_TABLE_COUNT] = PCalSite->Reserved[1]; // set the table count to from status0(NAND) to dram
        // The first vig table ID in NAND is CAL_VIGNETTE_PARTIAL_LOAD_START_ID (20)
        CalDramShadow = CalObj->DramShadow + ((CalSubId-CAL_VIGNETTE_PARTIAL_LOAD_START_ID) * sizeof(Vignette_Pack_Storage_s)) + CAL_VIGNETTE_TABLE_BASE;
        //the shift 4 byte is vignette Enable & table count, that are only appear in the start addr
        CalSize = sizeof(Vignette_Pack_Storage_s);
    } else {
        CalSubId = CalId;
    }

    sprintf(tmp,"%d.bin",CalSubId);
    strcat(FileName, tmp);
    Fid = AmbaFS_fopen(FileName,"r");
    if (Fid == NULL) {
        AmbaPrint("fopen %s fail.",FileName);
        return -1;
    }
    AmbaFS_fread(CalDramShadow,CalSize, 1, Fid);
    AmbaFS_fclose(Fid);
    return 0;
}

/**
 *  @brief Load calibration table head data from ROM to DRAM
 *
 *  Load calibration table head data from ROM to DRAM
 *
 *  @param [in]CalId calibration ID
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibROMLoadTableHead(UINT32 CalId)
{
    Cal_Obj_s *CalObj;
    char FileName[16] = "";
    int FPOS = 0;
    int Rval = -1;
    int TableBase = 0;
    UINT8 *CalAddr;

    CalObj = AppLib_CalibGetObj(CalId);
    CalAddr = CalObj->DramShadow;
    sprintf(FileName, "calib%d.bin",(int)CalId);
    switch (CalId) {
        case CAL_CA_ID:
            TableBase = CAL_CA_TABLE_BASE;
            break;
        case CAL_WARP_ID:
            TableBase = CAL_WARP_TABLE_BASE;
            break;
        case CAL_VIGNETTE_ID:
            TableBase = CAL_VIGNETTE_TABLE_BASE;
            break;
        default:
            break;
    }

    Rval = AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, FileName, CalAddr, TableBase, FPOS);
    if (Rval == -1) {
        AmbaPrint("Load ROM file TableHead %s failed", FileName);
    } else {
        AppLib_CalibMarkHeadFlag(CalId, 1);
        CalMgrPrint("Load ROM file TableHead %s success", FileName);
    }

    return 0;
}

/**
 *  @brief Load calibration table data from ROM to DRAM
 *
 *  Load calibration table data from ROM to DRAM
 *
 *  @param [in]CalId calibration ID
 *  @param [in]TableIdx entry table id for loading
 *  @param [in]TableNums number of tables for loading
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibROMLoadTable(UINT32 CalId, UINT32 TableIdx, UINT32 TableNums)
{
    Cal_Obj_s *CalObj;
    char FileName[16] = "";
    int FPOS = 0;
    int TableSize = 0;
    int Rval = -1;
    int TableBase = 0;
    UINT8 *CalAddr;
    UINT8 i;
    UINT8 StartTable,Count;

    if(TableNums == 1) {
        if (AppLib_CalibCheckFlag(CalId, TableIdx) == 1) {
            AmbaPrint("Calibration site [%d] was already loaded",CalId);
            return 0;
        }
    }

    CalObj = AppLib_CalibGetObj(CalId);
    CalAddr = CalObj->DramShadow;
    if (AppLib_CalibCheckHeadFlag(CalId) == 0) {
        AppLib_CalibROMLoadTableHead(CalId);
        AppLib_CalibMarkHeadFlag(CalId, 1);
    }

    StartTable = TableIdx;
    Count = TableNums;
    switch (CalId) {
        case CAL_CA_ID:
            TableBase = CAL_CA_TABLE_BASE;
            TableSize = sizeof(CA_Storage_s);
            if (TableIdx == CALIB_FULL_LOAD) {
                StartTable = 0;
                Count = MAX_CA_TABLE_COUNT;
            }
            break;
        case CAL_WARP_ID:
            TableBase = CAL_WARP_TABLE_BASE;
            TableSize = sizeof(Warp_Storage_s);
            if (TableIdx == CALIB_FULL_LOAD) {
                StartTable = 0;
                Count = MAX_WARP_TABLE_COUNT;
            }
            break;
        case CAL_VIGNETTE_ID:
            TableBase = CAL_VIGNETTE_TABLE_BASE;
            TableSize = sizeof(Vignette_Pack_Storage_s);
            if (TableIdx == CALIB_FULL_LOAD) {
                StartTable = 0;
                Count = MAX_VIGNETTE_GAIN_TABLE_COUNT;
            }
            break;
        default:
            TableBase = 0;
            TableSize = CalObj->Size;
            StartTable = 0;
            Count = 1;
            break;
    }

    sprintf(FileName, "calib%d.bin", CalId);
    FPOS = TableBase + StartTable * TableSize;
    CalAddr += TableBase + StartTable * TableSize;
    Rval = AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, FileName, CalAddr, TableSize*Count, FPOS);
    if (Rval == -1) {
        AmbaPrint("Load ROM file %s failed", FileName);
    } else {
        for(i = StartTable; i < Count; i++) {
            AppLib_CalibMarkFlag(CalId, i, 1);
        }
    }

    return 0;
}

/**
 *  @brief Load calibration data from ROM to DRAM
 *
 *  Load calibration data from ROM to DRAM
 *
 *  @param [in]CalId calibration ID
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibROMLoad(UINT32 CalId)
{
    Cal_Obj_s *CalObj;
    UINT32 CalSize;
    char FileName[16] = "";
    int FPOS = 0;
    //int VectorSize = 0;
    int Rval;
    UINT8 *CalAddr;

    CalObj = AppLib_CalibGetObj(CalId);
    CalAddr = CalObj->DramShadow;
    sprintf(FileName, "calib%d.bin",(int) CalId);
    CalSize = CalObj -> Size;

    Rval = AmbaROM_LoadByName(AMBA_ROM_SYS_DATA, FileName, CalAddr, CalSize, FPOS);
    if (Rval == -1) {
        AmbaPrint("Load ROM file %s failed", FileName);
    } else {
        CalMgrPrint("Load ROM file %s success", FileName);
    }

    return 0;
}

/**
 *  @brief Save calibration table head from DRAM to NAND
 *
 *  Save calibration table head from DRAM to NAND
 *
 *  @param [in]CalId calibration ID
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibNandSaveTableHead(UINT32 CalId)
{
    Cal_Obj_s *CalObj;
    char *CalName;
    UINT32 CalSize;
    //int CalSubId; //the ID has been recognize in nand
    //Cal_Stie_Status_s *PCalSite;//The vignette calibration Status: 1.partial save: 0xAAFFAAFF 2.the original : 0X55FF55FF
    UINT8 *CalDramShadow;
    int Rval;
    UINT32 Sec, SecNum;

    K_ASSERT(CalId < NVD_CALIB_MAX_OBJS);
    CalObj = AppLib_CalibGetObj(CalId);
    CalName = CalObj->Name;
    CalSize = CalObj->Size;
    CalDramShadow = CalObj->DramShadow;

    Sec = AppLib_CalibGetNandSecStart(CalId);
    switch (CalId) {
        case CAL_BPC_ID:
            SecNum = ((sizeof(BPC_Page_s) * BPC_MAX_PAGES) + 511) >>9; //temp for head should be less than one nand block (512 bytes)
            break;
        default:
            SecNum = 1;
            break;
    }

    CalMgrPrint("Head: Sec = %d , SecNum = %d",Sec,SecNum);
#ifdef CONFIG_ENABLE_EMMC_BOOT
    Rval = AmbaEMMC_MediaPartWrite(MP_CalibrationData , CalDramShadow, Sec, SecNum);
#else
    Rval = AmbaNFTL_Write(NFTL_ID_CAL, CalDramShadow, Sec, SecNum);
#endif    
    CalMgrPrint("Rval = %d",Rval);

    if (Rval == OK) {
        CalMgrPrint("[CAL] Site %s %d save success, Size=%d", CalName, CalId, CalSize);
        return 0;
    } else {
        CalMgrPrint("[CAL] Site %s %d save fail, Size=%d", CalName, CalId, CalSize);
        return -1;
    }
}

/**
 *  @brief Save calibration table head from DRAM to NAND
 *
 *  Save calibration table head from DRAM to NAND
 *
 *  @param [in]CalId calibration ID
 *  @param [in]TableIdx entry table id for loading
 *  @param [in]TableNums number of tables for loading
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibNandSaveTable(UINT32 CalId, UINT32 TableIdx, UINT32 TableNums )
{
    Cal_Obj_s *CalObj;
    char *CalName;
    UINT32 CalSize;
    //int CalSubId; //the ID has been recognize in nand
    //Cal_Stie_Status_s *PCalSite;//The vignette calibration Status: 1.partial save: 0xAAFFAAFF 2.the original : 0X55FF55FF
    UINT8 *CalDramShadow;
    int Rval;
    UINT32 Sec, SecNum;
    UINT32 CalDramAdd;

    K_ASSERT(CalId < NVD_CALIB_MAX_OBJS);
    CalObj = AppLib_CalibGetObj(CalId);
    CalName = CalObj->Name;
    CalSize = CalObj->Size;
    CalDramShadow = CalObj->DramShadow;

    Sec = AppLib_CalibGetNandSecStart(CalId);

    switch (CalId) {
        case CAL_CA_ID:
            Sec++;
            SecNum = AppLib_CalibGetNandSecNum(sizeof(CA_Storage_s));
            CalDramAdd = (1+TableIdx * SecNum)*512 ;
            break;
        case CAL_WARP_ID:
            Sec++;
            SecNum = AppLib_CalibGetNandSecNum(sizeof(Warp_Storage_s));
            CalDramAdd = (1+TableIdx * SecNum)*512 ;
            break;
        case CAL_BPC_ID: {
            BPC_Nand_s *BPCNand =  (BPC_Nand_s *) CalObj->DramShadow;
            Sec+= ((sizeof(BPC_Page_s) * BPC_MAX_PAGES) + 511) >>9;
            SecNum = AppLib_CalibGetNandSecNum(BPCNand->BPCPage[TableIdx].PageSize);
            CalDramAdd = BPCNand->BPCPage[TableIdx].Offset;
            break;
        }
        case CAL_VIGNETTE_ID:
            Sec++;
            SecNum = AppLib_CalibGetNandSecNum(sizeof(Vignette_Pack_Storage_s));
            CalDramAdd = (1+TableIdx * SecNum)*512 ;
            break;
        default:
            SecNum = AppLib_CalibGetNandSecNum(CalSize);
            CalDramAdd = (1+TableIdx * SecNum)*512 ;
            break;
    }


    CalMgrPrint("Table Sec = %d , SecNum = %d",Sec,SecNum);
#ifdef CONFIG_ENABLE_EMMC_BOOT
    Rval = AmbaEMMC_MediaPartWrite(MP_CalibrationData , CalDramShadow+ CalDramAdd, Sec+TableIdx * SecNum, SecNum* TableNums); 
#else
    Rval = AmbaNFTL_Write(NFTL_ID_CAL, CalDramShadow+ CalDramAdd, Sec+TableIdx * SecNum, SecNum* TableNums);
#endif
    if (Rval == OK) {
        CalMgrPrint("[CAL] Site %s %d save success, Size=%d", CalName, CalId, CalSize);
        AppLib_CalibRestCheckFlag(CalId);
        return 0;
    } else {
        CalMgrPrint("[CAL] Site %s %d save fail, Size=%d", CalName, CalId, CalSize);
        return -1;
    }

}

/**
 *  @brief Save calibration from DRAM to NAND
 *
 *  Save calibration from DRAM to NAND
 *
 *  @param [in]CalId calibration ID
 *  @param [in]SubId sub-channel ID
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibNandSave(UINT32 CalId , UINT32 SubId)
{
    Cal_Obj_s *CalObj;
    char *CalName;
    UINT32 CalSize;
    //int CalSubId; //the ID has been recognize in nand
    //Cal_Stie_Status_s *PCalSite;//The vignette calibration Status: 1.partial save: 0xAAFFAAFF 2.the original : 0X55FF55FF
    UINT32 Offset,SectorNum;
    UINT8 *CalDramShadow;
    int Rval;

    K_ASSERT(CalId < NVD_CALIB_MAX_OBJS);
    CalObj = AppLib_CalibGetObj(CalId);
    CalName = CalObj->Name;
    CalSize = CalObj->Size;
    CalDramShadow = CalObj->DramShadow;
    //PCalSite = AppLib_CalibGetSiteStatus(CalId);
    AppLib_CalibGetNandStatus(CalId,&Offset, &SectorNum);

//CheckSum
    switch (CalId) {
        case CAL_VIGNETTE_ID: {
            for (int i=0; i < MAX_VIGNETTE_GAIN_TABLE_COUNT; i++) {
                Vignette_Pack_Storage_s *VNCNand = (Vignette_Pack_Storage_s *)(UINT8 *)(CalDramShadow+ CAL_VIGNETTE_TABLE(i)) ;
                if ( VNCNand->Enable == 1 ) {
                    UINT32 TmpCheckSum = AmbaUtility_Crc32((UINT32 *) (VNCNand), sizeof(Vignette_Pack_Storage_s));
                    memcpy(CalDramShadow + CAL_VIGNETTE_CHECKSUM(i), &TmpCheckSum, sizeof(UINT32));
                    CalMgrPrint("Vig Page:%d CheckSum:%x TmpCheckSum:%x", i,  *(UINT32*)(CalDramShadow + CAL_VIGNETTE_CHECKSUM(i)), TmpCheckSum);
                } else {
                    CalMgrPrint("Vig Page:%d not Enable",i );
                }
            }
            break;
        }
        case CAL_BPC_ID: {
            BPC_Nand_s *BPCNand =  (BPC_Nand_s *) CalObj->DramShadow;
            for (int i=0; i<BPC_MAX_PAGES; i++) {
                if ( BPCNand->BPCPage[i].PageValid == 1 ) {
                    UINT32 TmpCheckSum = AmbaUtility_Crc32( (UINT8 *)&BPCNand-> Data +  BPCNand->BPCPage[i].Offset  , BPCNand->BPCPage[i].PageSize);
                    BPCNand->BPCPage[i].CheckSum =TmpCheckSum;
                    CalMgrPrint("BPC Page:%d CheckSum:%x",i,BPCNand->BPCPage[i].CheckSum);
                } else {
                    CalMgrPrint("BPC Page:%d not Enable",i );
                }
            }
            break;
        }
        default: {
            UINT32 TmpCheckSum = AmbaUtility_Crc32(CalDramShadow, CalObj->Size - 4);
            memcpy(CalDramShadow + (CalObj->Size-4), &TmpCheckSum, sizeof(UINT32));
            CalMgrPrint("CalID:%d CheckSum:%x", CalId, TmpCheckSum);
            break;
        }
    }
    //CheckSum done

#ifdef CONFIG_ENABLE_EMMC_BOOT
    Rval = AmbaEMMC_MediaPartWrite(MP_CalibrationData , CalDramShadow, Offset, SectorNum); 
#else
    Rval = AmbaNFTL_Write(NFTL_ID_CAL, CalDramShadow, Offset, SectorNum);
#endif    
    if (Rval == OK) {
        CalMgrPrint("[CAL] Site %s %d save success, Size=%d", CalName, CalId, CalSize);
        AppLib_CalibRestCheckFlag(CalId);
        Rval =  0;
    } else {
        CalMgrPrint("[CAL] Site %s %d save fail, Size=%d", CalName, CalId, CalSize);
        Rval = -1;
    }

    return Rval;

}

/**
 *  @brief Save calibration from DRAM to SD card
 *
 *  Save calibration from DRAM to SD card
 *
 *  @param [in]CalId calibration ID
 *  @param [in]SubId sub-channel ID
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibSDCardSave(UINT32 CalId , UINT32 SubId)
{
    Cal_Obj_s *CalObj;
    //char *CalName;
    UINT32 CalSize;
    UINT8 *CalDramShadow;
    int CalSubId; //the ID has been recognize in nand
    Cal_Stie_Status_s *PCalSite;//The vignette calibration Status: 1.partial save: 0xAAFFAAFF 2.the original : 0X55FF55FF
    AMBA_FS_FILE *Fid = NULL;
    char FileName[16] = {'c',':','\\','c','a','l','i','b'};
    char tmp[16] = {0};
    K_ASSERT(CalId < NVD_CALIB_MAX_OBJS);

    CalObj = AppLib_CalibGetObj(CalId);
    //CalName = CalObj->Name;
    CalSize = CalObj->Size;
    CalDramShadow = CalObj->DramShadow;
    PCalSite = AppLib_CalibGetSiteStatus(CalId);
    if (PCalSite->Status == CAL_SITE_DONE_VIGNETTE_LOAD_PARTIAL && CalId == CAL_VIGNETTE_ID) {
        CalSubId = SubId + CAL_VIGNETTE_PARTIAL_LOAD_START_ID;
        // The first vig table ID in NAND is CAL_VIGNETTE_PARTIAL_LOAD_START_ID (20)
        CalDramShadow = CalObj->DramShadow + (SubId * sizeof(Vignette_Pack_Storage_s)) + CAL_VIGNETTE_TABLE_BASE;
        //the shift 4 byte is vignette Enable & table count, that are only appear in the start addr
        CalSize = sizeof(Vignette_Pack_Storage_s);//for one vignette table Size
    } else {
        CalSubId = CalId;
    }

    sprintf(tmp,"%d.bin",CalSubId);
    strcat(FileName, tmp);
    Fid = AmbaFS_fopen(FileName,"w");
    if (Fid == NULL) {
        AmbaPrint("fopen %s fail.",FileName);
        return -1;
    }
    AmbaFS_fwrite(CalDramShadow,CalSize, 1, Fid);
    AmbaFS_fclose(Fid);
    AppLib_CalibRestCheckFlag(CalId);
    return 0;
}

/**
 *  @brief Mark calibration tablehead loaded flag (0:empty 1: already loaded)
 *
 *  Mark calibration tablehead loaded flag (0:empty 1: already loaded)
 *
 *  @param [in]CalId calibration ID
 *  @param [in]TableIdx table ID
 *
 *  @return table index
 */
int AppLib_CalibMarkHeadFlag(UINT8 CalId, UINT8 Flag)
{
    switch (CalId) {
        case CAL_CA_ID:
            GCATableHeadLoaded = Flag;
            break;
        case CAL_WARP_ID:
            GWarpTableHeadLoaded = Flag;
            break;
        case CAL_VIGNETTE_ID:
            GVignetteTableHeadLoaded = Flag;
            break;
        case CAL_BPC_ID:
            GBPCTableHeadLoaded = Flag;
            break;
        default:
            break;
    }
            return 0;
}

/**
 *  @brief Check calibration table loaded flag (0:empty 1: already loaded)
 *
 *  Check calibration table loaded flag (0:empty 1: already loaded)
 *
 *  @param [in]CalId calibration ID
 *  @param [in]TableIdx table ID
 *
 *  @return table index
 */
int AppLib_CalibCheckHeadFlag(UINT8 CalId)
{
    switch (CalId) {
        case CAL_CA_ID:
            return GCATableHeadLoaded;
        case CAL_WARP_ID:
            return GWarpTableHeadLoaded;
        case CAL_VIGNETTE_ID:
            return GVignetteTableHeadLoaded;
        case CAL_BPC_ID:
            return GBPCTableHeadLoaded;
        default:
            return 1;
    }
}


/**
 *  @brief Mark calibration table loaded flag (0:empty 1: already loaded)
 *
 *  Mark calibration table loaded flag (0:empty 1: already loaded)
 *
 *  @param [in]CalId calibration ID
 *  @param [in]TableIdx table ID
 *
 *  @return table index
 */
int AppLib_CalibMarkFlag(UINT8 CalId, UINT8 TableIdx, UINT8 Flag)
{
    switch (CalId) {
        case CAL_CA_ID:
            GCATableLoaded[TableIdx] = Flag;
            break;
        case CAL_WARP_ID:
            GWarpTableLoaded[TableIdx] = Flag;
            break;
        case CAL_VIGNETTE_ID:
            GVignetteTableLoaded[TableIdx] = Flag;
            break;
        case CAL_BPC_ID:
            GBPCTableLoaded[TableIdx] = Flag;
            break;
        case CAL_AF_ID:
            GAFTableLoaded = Flag;
            break;
        case CAL_GYRO_ID:
            GGyroTableLoaded = Flag;
            break;
        case CAL_MSHUTTER_ID:
            GMShutterTableLoaded = Flag;
            break;
        case CAL_IRIS_ID:
            GIrisTableLoaded = Flag;
            break;
        case CAL_WB_ID:
            GWBTableLoaded = Flag;
            break;
        case CAL_ISO_ID:
            GISOTableLoaded = Flag;
            break;
        case CAL_FLASH_ID:
            GFlashTableLoaded = Flag;
            break;
        case CAL_AUDIO_ID:
            GAudioTableLoaded = Flag;
            break;
        default:
            break;
    }
    return 0;
}

/**
 *  @brief Check calibration table loaded flag (0:empty 1: already loaded)
 *
 *  Check calibration table loaded flag (0:empty 1: already loaded)
 *
 *  @param [in]CalId calibration ID
 *  @param [in]TableIdx table ID
 *
 *  @return table index
 */
int AppLib_CalibCheckFlag(UINT8 CalId, UINT8 TableIdx)
{
    switch (CalId) {
        case CAL_CA_ID:
            return GCATableLoaded[TableIdx];
        case CAL_WARP_ID:
            return GWarpTableLoaded[TableIdx];
        case CAL_VIGNETTE_ID:
            return GVignetteTableLoaded[TableIdx];
        case CAL_BPC_ID:
            return GBPCTableLoaded[TableIdx];
        case CAL_AF_ID:
            return GAFTableLoaded;
        case CAL_GYRO_ID:
            return GGyroTableLoaded;
        case CAL_MSHUTTER_ID:
            return GMShutterTableLoaded;
        case CAL_IRIS_ID:
            return GIrisTableLoaded;
        case CAL_WB_ID:
            return GWBTableLoaded;
        case CAL_ISO_ID:
            return GISOTableLoaded;
        case CAL_FLASH_ID:
            return GFlashTableLoaded;
        case CAL_AUDIO_ID:
            return GAudioTableLoaded;
            
        default:
            return 0;
    }
}

/**
 *  @brief reset All Check flag (0:empty 1: already loaded)
 *
 *  reset all Check flag (0:empty 1: already loaded)
 *
 *  @return Null
 */
void AppLib_CalibRestCheckAllFlag(void)
{
    memset(GCATableLoaded, 0, MAX_CA_TABLE_COUNT);
    memset(GWarpTableLoaded, 0, MAX_WARP_TABLE_COUNT);
    memset(GVignetteTableLoaded, 0, MAX_VIGNETTE_GAIN_TABLE_COUNT);
    memset(GBPCTableLoaded, 0, BPC_MAX_PAGES);
    GAFTableLoaded = 0;
    GGyroTableLoaded = 0;
    GMShutterTableLoaded = 0;
    GIrisTableLoaded = 0;
    GWBTableLoaded = 0;
    GISOTableLoaded = 0;
    GFlashTableLoaded = 0;
    GAudioTableLoaded = 0;
}

/**
 *  @brief Reset calibration table loaded flag (0:empty 1: already loaded)
 *
 *  Reset calibration table loaded flag (0:empty 1: already loaded)
 *
 *  @param [in]CalId calibration ID
 *
 *  @return table index
 */
void AppLib_CalibRestCheckFlag(UINT8 CalId)
{
    switch (CalId) {
        case CAL_CA_ID:
            memset(GCATableLoaded, 0, MAX_CA_TABLE_COUNT);
        break;
        case CAL_WARP_ID:
            memset(GWarpTableLoaded, 0, MAX_WARP_TABLE_COUNT);
        break;
        case CAL_VIGNETTE_ID:
            memset(GVignetteTableLoaded, 0, MAX_VIGNETTE_GAIN_TABLE_COUNT);
            break;
        case CAL_BPC_ID:
            memset(GBPCTableLoaded, 0, BPC_MAX_PAGES);
            break;
        case CAL_AF_ID:
            GAFTableLoaded = 0;
            break;
        case CAL_GYRO_ID:
            GGyroTableLoaded = 0;
            break;
        case CAL_MSHUTTER_ID:
            GMShutterTableLoaded = 0;
            break;
        case CAL_IRIS_ID:
            GIrisTableLoaded = 0;
            break;
        case CAL_WB_ID:
            GWBTableLoaded = 0;
            break;
        case CAL_ISO_ID:
            GISOTableLoaded = 0;
            break;
        case CAL_FLASH_ID:
            GFlashTableLoaded = 0;
            break;
        case CAL_AUDIO_ID:
            GAudioTableLoaded = 0;
            break;
        default:
            break;
    }

    }




/**
 *  @brief simple initial function for calibration
 *
 *  simple initial function for calibration
 *
 *  @param [in]CalId calibration ID
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibInitLoadSimple(UINT32 CalId, UINT8 Format)
{
    Cal_Stie_Status_s  *PCalSite;
    Cal_Obj_s              *CalObj;
    int Rval = 0;

    // Print site Status
    K_ASSERT(CalId < NVD_CALIB_MAX_OBJS);

    PCalSite = AppLib_CalibGetSiteStatus(CalId);
    CalObj = AppLib_CalibGetObj(CalId);
    if (CalObj->Enable == 0) {
        CalMgrPrint("[CAL] site %d didn't Enable", CalId);
        return 0;
    }
    //CalName = CalObj->Name;
    // Print site Status
    //UINT8 format = CALIB_SOURCE_NAND;
    // Format = CalFormat[CalId];

    CalMgrPrint("CalId #%d", CalId);
    // Check whether NVD Size matches
    if (((PCalSite->Status == CAL_SITE_DONE)) || (CalId == CAL_STATUS_ID)) {
        // Load warp/ca calibration table from rom file system
        if (AppLib_CalibCheckFlag(CalId, CALIB_TABLE_IDX_INIT) == 0) {
            switch (Format) {
                case CALIB_SOURCE_ROMFS:
                    Rval = AppLib_CalibROMLoad(CalId);
                    break;
                case CALIB_SOURCE_NAND:
                    Rval = AppLib_CalibNandLoad(CalId);
                    break;
                case CALIB_SOURCE_SDCard:
                    Rval = AppLib_CalibSDCardLoad(CalId);
                    break;
                default:
                    Rval = 0;
                    break;
            }
        } else {
            AmbaPrint("The calibration table %d has been loaded!", CALIB_TABLE_IDX_INIT);
            return -1;
        }
    } else {
        CalMgrPrint("no calibration Status. Need initial first");
    }

    return Rval;
}

/**
 *  @brief simple initial function for calibration
 *
 *  simple initial function for calibration
 *
 *  @param [in]CalId calibration ID
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibInitSimple(UINT32 CalId)
{

    Cal_Obj_s               *CalObj;

    // Print site Status
    K_ASSERT(CalId < NVD_CALIB_MAX_OBJS);
    CalObj = AppLib_CalibGetObj(CalId);

    if (CalObj->Enable == DISABLE) {
        CalMgrPrint("[CAL] site %d didn't Enable", CalId);
        return 0;
    }
    if (CalObj->InitFunc != NULL) {
        if ((CalObj->InitFunc)(CalObj) < 0) {
            CalMgrPrint("[CAL] Site %s %d init fail", CalObj->Name, CalId);
            return -1;
        }
    }
    CalMgrPrint("[CAL] Init site %s %d success", CalObj->Name, CalId);

    return 0;
}




/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AppLib_CalibInitFunc
 *
 *  @Description:: initial function for calibration
 *
 *  @Input      ::
 *          CalId: calibration ID
 *          JobId: load or init
 *
 *  @Output     ::
 *          None
 *  @Return     ::
 *          int: OK:0/NG:-1
\*-----------------------------------------------------------------------------------------------*/
int AppLib_CalibInitFunc(UINT32 CalId,UINT8 JobId, UINT8 SubId)
{
    Cal_Stie_Status_s   *PCalSite;
    Cal_Obj_s           *CalObj;
    char                *CalName;
    int                 Rval;


    PCalSite = AppLib_CalibGetSiteStatus(CalId);
    CalObj = AppLib_CalibGetObj(CalId);

    if (CalObj->Enable == 0) {
        CalMgrPrint("[CAL] site %d didn't Enable", CalId);
        return 0;
    }
    CalName = CalObj->Name;

    if (JobId == CALIB_LOAD) {
        // Print site Status
        UINT8 format = CalFormat[CalId];

        CalMgrPrint("CalId #%d", CalId);
        if ((PCalSite->Status == CAL_SITE_DONE) || (CalId == CAL_STATUS_ID) ||(format == CALIB_SOURCE_ROMFS)) {
            // Load warp/ca calibration table from rom file system
            AppLib_CalibCheckSize(CalId);
            if (format == CALIB_SOURCE_ROMFS) {
                Rval = AppLib_CalibROMLoadTable(CalId, SubId, 1);
                return Rval;
            } else { // Load calibration data from NAND to DRAM
                #if (CALIB_STORAGE == CALIB_FROM_NAND)
                Rval = AppLib_CalibNandLoadTable(CalId,SubId,1);// Load calibration data from NAND to DRAM
                #else
                Rval = AppLib_CalibSDCardLoad(CalId);// Load calibration data from SD card to DRAM
                #endif
                if (Rval < 0) {
                    CalMgrPrint("calib load error");
                    if (CalId == CAL_STATUS_ID) {
                        #if (CALIB_STORAGE == CALIB_FROM_NAND)
                        Rval = AppLib_CalibNandReset(CalId);
                        #else
                        Rval = AppLib_CalibSDCardReset(CalId);
                        #endif
                        if (Rval < 0) {
                            return -1;
                        }
                    }
                    if (AppLib_CalibMemReset(CalId) < 0) {
                        return -1;
                    }
                }
            }
        } else {
            CalMgrPrint("no calibration Status....");
            if (AppLib_CalibMemReset(CalId) < 0) {
                return -1;
            }
        }
    } else { //initial calibration/ Init calibration (set parameters to other components)
        if (CalObj->InitFunc != NULL) {
            if ((CalObj->InitFunc)(CalObj) < 0) {
                CalMgrPrint("[CAL] Site %s %d init fail", CalName, CalId);
                return -1;
            }
        }
        CalMgrPrint("[CAL] Init site %s %d success", CalName, CalId);
    }
    return 0;
}    

/**
 *  @brief check calibration version
 *
 *  check calibration version
 *
 *  @param [in]CalId calibration ID
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibVersionCheck(UINT32 CalId)
{
    Cal_Obj_s *CalObj;
    char *CalName;

    K_ASSERT(CalId < NVD_CALIB_MAX_OBJS);
    CalObj = AppLib_CalibGetObj(CalId);

    if (CalObj->Enable == 0) {
        CalMgrPrint("[CAL] site %d didn't Enable", CalId);
        return 0;
    }
    CalName = CalObj->Name;
    // Calibration version check
    if (CalObj->UpgradeFunc != NULL) {
        Cal_Stie_Status_s *cal_site = AppLib_CalibGetSiteStatus(CalId);

        if ((CalObj->UpgradeFunc)(CalObj, cal_site) < 0) {
            CalMgrPrint("[CAL] Site %s %d upgrade fail", CalName, CalId);
            return -1;
        }
    }
    return 0;
}

/**
 *  @brief set exposure value for AE
 *
 *  set exposure value for AE
 *
 *  @param [in]Shutter shutter time
 *  @param [in]Agc sensor gain
 *  @param [in]Dgain digital gain
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibSetExposureValue(float Shutter, float Agc, UINT32 Dgain)
{
    int Rval = 0;
    UINT8 ChNo = 0;
    UINT32 gainFactor = 0;
    UINT32 aGainCtrl = 0;
    UINT32 dGainCtrl = 0;
    UINT32 shutterCtrl = 0;
    AMBA_SENSOR_STATUS_INFO_s status;
    UINT8 exposureFrames = 0;

    AmbaSensor_ConvertGainFactor(AppEncChannel, Agc, &gainFactor, &aGainCtrl, &dGainCtrl);
    AmbaSensor_SetAnalogGainCtrl(AppEncChannel, aGainCtrl);
    AmbaSensor_SetDigitalGainCtrl(AppEncChannel, dGainCtrl);
    AmbaSensor_ConvertShutterSpeed(AppEncChannel, Shutter, &shutterCtrl);
    AmbaSensor_GetStatus(AppEncChannel, &status);
    exposureFrames = (shutterCtrl/status.ModeInfo.NumExposureStepPerFrame);
    exposureFrames = (shutterCtrl%status.ModeInfo.NumExposureStepPerFrame)? exposureFrames+1: exposureFrames;
    AmbaSensor_SetSlowShutterCtrl(AppEncChannel, exposureFrames);
    AmbaSensor_SetShutterCtrl(AppEncChannel, shutterCtrl);
    AmbaImg_Proc_Cmd(MW_IP_SET_DGAIN, (UINT32)ChNo, (UINT32)Dgain, 0);

    return Rval;
}

/**
 *  @brief the mapping for calibration table for multi-channel
 *
 *  the mapping for calibration table for multi-channel
 *
 *  @param [in]Id: table id
 *  @param [in]Channel: channel id
 *
 *  @return mapping table
 */
int AppLib_CalibTableMapping(UINT8 Channel, UINT8 Id)
{
    if (Channel == CALIB_CH_ALL) {
        return Id;
    } else {
        return (Channel+(Id*CALIB_CH_NO));
    }
}

/**
 *  @brief check whether the file exists in SD card
 *
 *  check whether the file exists in SD card
 *
 *  @param [in]Filename File path in SD card
 *
 *  @return Exists:1/Not Exists:0
 */
int AppLib_CalibSDCardFileExists(char *Filename)
{
    AMBA_FS_FILE *Fid = NULL;
    int Rval;

    Fid = AmbaFS_fopen(Filename, "r");
    if (Fid != NULL) {
        Rval = 1;
    } else {
        Rval = 0;
    }
    AmbaFS_fclose(Fid);
    return Rval;
}


/**
 *  @brief check calib structure size alignment
 *
 *  check calib structure size alignment
 *
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibCheckStructure(void)
{
#define Check512(Data)  (sizeof(Data)%512)
    CalMgrPrint("BPC structure check.");
    if (Check512(BPC_Nand_s)) {
        CalMgrPrint("BPC_Nand_s (%d bytes) not mod 512 bytes.", sizeof(BPC_Nand_s));
    }
    CalMgrPrint("VNC structure check.");
    if (Check512(Vignette_Control_s)) {
        CalMgrPrint("Vignette_Control_s size %d bytes not mod 512 bytes.", sizeof(Vignette_Control_s));
    }
    if (Check512(Vignette_Pack_Storage_s)) {
        CalMgrPrint("Vignette_Pack_Storage_s size %d bytes not mod 512 bytes.", sizeof(Vignette_Pack_Storage_s));
    }
    return 0;
}

/**
 *  @brief set DSP mode for calibration
 *
 *  set DSP mode for calibration
 *
 *  @param [in]pMode DSP image mode config
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalib_SetDspMode(AMBA_DSP_IMG_MODE_CFG_s *pMode)
{
    memcpy(&CalibImgMode, pMode, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
    return 0;
}

/**
 *  @brief get DSP mode for calibration
 *
 *  get DSP mode for calibration
 *
 *  @param [in]pMode DSP image mode config
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibGetDspMode(AMBA_DSP_IMG_MODE_CFG_s *pMode)
{
    memcpy(pMode, &CalibImgMode, sizeof(AMBA_DSP_IMG_MODE_CFG_s));
    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaUT_CalibRotateMap
 *
 *  @Description:: rotate map
 *
 *  @Input      ::
 *          SourceMap: source map
 *          MapWidth:   source map width
 *          MapHeight:  source map height
 *          Rotate:        rotate degree. AMBA_DSP_ROTATE_0/AMBA_DSP_ROTATE_90/AMBA_DSP_ROTATE_180/AMBA_DSP_ROTATE_270
 *
 *
 *  @Output     ::
 *          TargetMap: Map after rotate
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
/**
 *  @brief rotate map
 *
 *  rotate map
 *
 *  @param [in]TargetMap target address
 *  @param [in]SourceMap source address
 *  @param [in]MapWidth map width
 *  @param [in]MapHeight map height
 *  @param [in]Rotate direction
 *
 *  @return 0 success
 */
int AppLib_CalibRotateMapTest(UINT16 *TargetMap, UINT16 *SourceMap, UINT16 MapWidth, UINT16 MapHeight, UINT8 Rotate)
{
    int Width, Height;
    for (Height = 0; Height<MapHeight; ++Height) {
        for (Width =0; Width< MapWidth; ++Width) {
            if (Rotate == AMBA_DSP_ROTATE_0) { //clockwise rotate 0 degree
                TargetMap[Width+Height*MapWidth] = SourceMap[Width+Height*MapWidth];
            } else if (Rotate == AMBA_DSP_ROTATE_90) { //clockwise rotate 90 degree
                TargetMap[Width*MapHeight+(MapHeight-Height-1)] = SourceMap[Width+Height*MapWidth];
            } else if (Rotate == AMBA_DSP_ROTATE_180) { //clockwise rotate 180 degree
                TargetMap[(MapWidth-Width-1)+(MapHeight-Height-1)*MapWidth] = SourceMap[Width+Height*MapWidth];
            } else if (Rotate == AMBA_DSP_ROTATE_270) { //clockwise rotate 270 degree
                TargetMap[(MapWidth-Width-1)*MapHeight+Height] = SourceMap[Width+Height*MapWidth];
            }
        }
    }
    return 0;
}


/**
 *  @brief swap data
 *
 *  swap data
 *
 *  @param [in]A data A
 *  @param [in]B data B
 *
 */
void AppLib_CalibSwap(UINT16 * A, UINT16 * B)
{
    UINT16 C;
    C=*A;
    *A=*B;
    *B=C;
}


/**
 *  @brief rotate calibration table
 *
 *  rotate calibration table
 *
 *  @param [in]Map address of the table
 *  @param [in]MapWidth width of the map
 *  @param [in]MapHeight height of the map
 *  @param [in]Rotate rotate flag
 *
 *  @return 0 success, -1 failure
 */
int AppLib_CalibRotateMap( UINT16 *Map, UINT16 MapWidth, UINT16 MapHeight, UINT8 Rotate)
{
    int Mapsize=MapWidth*MapHeight;
    int m,n;
    switch (Rotate) {
        case AMBA_DSP_ROTATE_0:
            break;
        case AMBA_DSP_ROTATE_0_HORZ_FLIP:
            for (m=0; m<MapHeight; ++m) {
                for (n=0; n<MapWidth >> 1; ++n) {
                    AppLib_CalibSwap(&Map[n+m*MapWidth], &Map[MapWidth-1-n + m*MapWidth]);
                }
            }
            break;
        case  AMBA_DSP_ROTATE_180:
            for (n=0; n < Mapsize >> 1; ++n) {
                AppLib_CalibSwap(&Map[n], &Map[Mapsize -1-n]);
            }
            break;
        case AMBA_DSP_ROTATE_180_HORZ_FLIP:
            for (n=0; n<MapWidth; ++n) {
                for (m=0; m<MapHeight >> 1; ++m) {
                    AppLib_CalibSwap(&Map[n+m*MapWidth], &Map[n + (MapHeight - 1 - m)*MapWidth]);
                }
            }
            break;
        default:
            AmbaPrintColor(RED, "Not valid rotation mode.");
            break;
    }

    return 0;
}

/**
 *  @brief Set calibration parameter table
 *
 *  set calibration parameter table
 *
 *  @param [in]CalibTableAddr address of the calibration parameter table
 *
 *  @return 0 success, -1 failure
 */
int AppLibCalibTableSet(CALIBRATION_ADJUST_PARAM_s *CalibTableAddr)
{   
    extern CALIBRATION_ADJUST_PARAM_s *AmbaCalibParamsAddr;
    if(CalibTableAddr == NULL){
        AmbaPrintColor(RED,"[ApplibCalibParams] Error, calibration table address shouldn't be NULL");
        return -1;
    }
    AmbaCalibParamsAddr = CalibTableAddr;
    return 0;
}