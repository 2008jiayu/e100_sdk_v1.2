/**
 * @file src/app/connected/applib/inc/calibration/ApplibCalibCli.h
 *
 * Header file for Calibration command line interface
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
#ifndef __CALIB_CLI_H__
#define __CALIB_CLI_H__


__BEGIN_C_PROTO__

/**
* @defgroup ApplibCalibCli
* @brief Header file for Calibration command line interface
*
*/

/**
 * @addtogroup ApplibCalibCli
 * @{
 */

/**
*  Calibration manager Command Line Interface (CLI)
*
*  @param [in] Argc argument count
*  @param [in] Argv argument vector
*  @param [out] OutputStr output message
*  @param [out] MsgAvailable check the message is available
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibCli( int Argc, char *Argv[], char *OutputStr, int *MsgAvailable);
//extern void AppLib_CalibCliSdIfTimerHandler(int eid);

/**
*  Calibration by SD card interface
*
*  @param [in] Path the path for the calibration script

*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibPathIf(char* Path);

/**
*  Clear calibration data for partial load mode
*
*  @param [in] CalId calibration ID
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibResetPartial(UINT32 CalId); //for vignette partial load

/**
*  Clear calibration data for partial load mode
*
*  @param [in] CalId calibration ID
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibSDCardReset(UINT32 CalId);

/**
*  Save calibration table head from DRAM to NAND
*
*  @param [in] CalId calibration ID
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibNandSave(UINT32 CalId , UINT32 sub_id);

/**
*  Save calibration from DRAM to SD card
*
*  @param [in] CalId calibration ID
*  @param [in] sub_id sub-channel ID
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibSDCardSave(UINT32 CalId , UINT32 sub_id);

/**
*  Load calibration data from NAND to DRAM
*
*  @param [in] CalId calibration ID
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibNandLoad(UINT32 CalId);

/**
*  Load calibration data from SD card to DRAM
*
*  @param [in] CalId calibration ID
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibSDCardLoad(UINT32 CalId);

/**
*  Clear SD card calibration partition with zero initialized
*
*  @param [in] CalId calibration ID
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibSDCardReset(UINT32 CalId);

/**
*  Write Status data to NAND flash
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibSaveStatus(void);

/**
*  Init Calibration Nand
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibNandInit(void);

/**
*  Simple initial function for calibration
*
*  @param [in] CalId calibration ID
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibInitSimple(UINT32 CalId);

/**
*  Load calibration data from ROM to DRAM
*
*  @param [in] CalId calibration ID
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibROMLoad(UINT32 CalId);

/**
*  Load calibration table head data from ROM to DRAM
*
*  @param [in] CalId calibration ID
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibROMLoadTableHead(UINT32 CalId);

/**
*  Load calibration table data from ROM to DRAM
*
*  @param [in] CalId calibration ID
*  @param [in] TableIdx entry table id for loading
*  @param [in] TableNums number of tables for loading
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibROMLoadTable(UINT32 CalId, UINT32 TableIdx, UINT32 TableNums);

/**
*  Load calibration table head data from NAND to DRAM
*
*  @param [in] CalId calibration ID
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibNandLoadTableHead(UINT32 CalId);

/**
*  Load calibration table data from NAND to DRAM
*
*  @param [in] CalId calibration ID
*  @param [in] TableIdx entry table id for loading
*  @param [in] TableNums number of tables for loading
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibNandLoadTable(UINT32 CalId, UINT32 TableIdx, UINT32 TableNums);

/**
*  Save calibration table data from DRAM to NAND
*
*  @param [in] CalId calibration ID
*  @param [in] TableIdx entry table id for loading
*  @param [in] TableNums number of tables for loading
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibNandSaveTable(UINT32 CalId, UINT32 TableIdx, UINT32 TableNums);

/**
*  Save calibration table head from DRAM to NAND
*
*  @param [in] CalId calibration ID
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibNandSaveTableHead(UINT32 CalId);

/**
*  Simple initial load function for calibration
*
*  @param [in] CalId calibration ID
*  @param [in] Format calibration data source (rom, nand or sdcard)
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibInitLoadSimple(UINT32 CalId, UINT8 Format);

/**
*  Simple initial function for calibration
*
*  @param [in] CalId calibration ID
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibInitSimple(UINT32 CalId);

/**
*  Check whether the file exists in SD card
*
*  @param [in] Filename file path in SD card
*
*  @return 0 - OK, -1 - Failed
*/
extern int AppLib_CalibSDCardFileExists(char *Filename);

/**
*  check calib structure size alignment
*
*  @return 0 - OK
*/
extern int AppLib_CalibCheckStructure(void);

extern int AppLibCalibAdjust_Init(void);

__END_C_PROTO__

#endif
