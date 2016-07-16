/**
 * @file src/app/connected/applib/inc/storage/ApplibStorage_Card.h
 *
 * Header of Card Utility Apis
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

#ifndef APPLIB_STORAGE_CARD_H_
#define APPLIB_STORAGE_CARD_H_

/**
* @defgroup ApplibStorage_Card
* @brief Card Utility Apis
*
*
*/

/**
 * @addtogroup ApplibStorage_Card
 * @ingroup Storage
 * @{
 */
__BEGIN_C_PROTO__

/*************************************************************************
 * Card definitons
 ************************************************************************/
/** Card slot definitions */
typedef enum _APPLIB_CARD_ID_e_ {
    CARD_SD0 = 0,
    CARD_SD1,
    CARD_NAND0,
    CARD_NAND1,
    CARD_RD,
    CARD_NUM
} APPLIB_CARD_ID_e;

/*************************************************************************
 * Card APIs
 ************************************************************************/

/**
 *  Initialization card module
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibCard_Init(void);

/**
 *  Polling card insert status.
 *
 *  @param [in] cardId Card id.
 *
 *  @return >=0 Insert card id, <0 failure
 */
extern int AppLibCard_Polling(UINT32 cardId);

/**
 *  Get drive id
 *
 *  @param [in] cardId Card id
 *
 *  @return >=0 Drive id, <0 failure
 */
extern char AppLibCard_GetDrive(UINT32 cardId);

/**
 *  Get card slot
 *
 *  @param [in] cardId Card id.
 *
 *  @return >=0 Slot id, <0 failure
 */
extern int AppLibCard_GetSlot(UINT32 cardId);

/**
 *  Get card id
 *
 *  @param [in] slot Card slot.
 *
 *  @return >=0 Card id, <0 failure
 */
extern int AppLibCard_GetCardId(int slot);

/**
 *  Get slot id from drive id
 *
 *  @param [in] drive Drive id
 *
 *  @return >=0 Slot id, <0 failure
 */
extern int AppLibCard_GetSlotFromChar(char drive);

/**
 *  Format card
 *
 *  @param [in] slot Slot id
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibCard_Format(int slot);

/**
 *  Insert card
 *
 *  @param [in] slot Card slot
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibCard_Insert(int slot);
/* Card insert return values */
#define CARD_INSERT_ACTIVE_DONE         (0)
#define CARD_INSERT_LOWPRI_DONE         (1)
#define CARD_INSERT_ERROR_NO_CARD       (-1)
#define CARD_INSERT_ERROR_SMALL_NAND    (-2)
#define CARD_INSERT_ERROR_NOT_MEM       (-3)
#define CARD_INSERT_ERROR_UNINIT        (-4)
#define CARD_INSERT_ERROR_FORMAT        (-5)
#define CARD_INSERT_ERROR_SETROOT       (-6)

/**
 *  Remove card
 *
 *  @param [in] slot Card slot
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibCard_Remove(int slot);
/* Card remove return values */
#define CARD_REMOVE_ACTIVE      (0)
#define CARD_REMOVE_NONACTIVE   (1)

/**
 *  Set a flag for inserting slot
 *
 *  @param [in] slot slot id
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibCard_SetInsertingSlot(int slot);

/**
 *  @brief change free space threshold
 *
 *  @param [in] threshold value of free space threshold
 *
 *  @return 0 success
 */
extern int AppLibCard_SetThreahold(int threshold);

/**
 *  Check the card whether be inserting
 *
 *  @return >=0 Yes, <0 No
 */
extern int AppLibCard_CheckInsertingCard(void);

/**
 *  Get active drive
 *
 *  @return >=0 drive id, <0 failure
 */
extern char AppLibCard_GetActiveDrive(void);

/**
 *  @brief Get active slot
 *
 *  Get active slot
 *
 *  @return >=0 slot id, <0 failure
 */
extern int AppLibCard_GetActiveSlot(void);

/**
 *  Get active card id
 *
 *  @return >=0 card id, <0 failure
 */
extern int AppLibCard_GetActiveCardId(void);

/**
 *  Get primary slot id
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibCard_GetPrimarySlot(void);

/**
 *  Check the status of card insert.
 *
 *  @return The status of card insert.
 */
extern int AppLibCard_CheckInsertState(void);

/**
 *  Check card format type
 *
 *  @param [in] slot Slot id
 *  @param [in] drive Drive id
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibCard_CheckFormatParam(int slot, char drive);

/**
 *  Check card's status
 *
 *  @param [in] checkFlags Check flag.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibCard_CheckStatus(UINT32 checkFlags);

/* Card status check flags */
#define CARD_CHECK_WRITE        (0x00000001)
#define CARD_CHECK_DELETE       (0x00000002)
#define CARD_CHECK_ID_CHANGE    (0x00000004)
#define CARD_CHECK_MODIFY       (0x00000008)
#define CARD_CHECK_CONT_WRITE   (0x0000000A)
#define CARD_CHECK_PRESENT      (0x40000000)
#define CARD_CHECK_RESET        (0x80000000)
/* Card status check return values */
#define CARD_STATUS_CHECK_PASS          (0)
#define CARD_STATUS_NO_CARD             (-1)
#define CARD_STATUS_UNFORMAT_CARD       (-2)
#define CARD_STATUS_SMALL_NAND_CARD     (-3)
#define CARD_STATUS_NOT_ENOUGH_SPACE    (-4)
#define CARD_STATUS_NOT_ENOUGH_SPACE_P  (-5)
#define CARD_STATUS_NO_SUCH_CARD        (-6)
#define CARD_STATUS_NOT_MEM_CARD        (-7)
#define CARD_STATUS_UN_INIT_CARD        (-8)
#define CARD_STATUS_INVALID_CARD        (-9)
#define CARD_STATUS_INVALID_FORMAT_CARD (-10)
#define CARD_STATUS_WP_CARD             (-11)
#define CARD_STATUS_REFRESHING          (-12)
#define CARD_STATUS_ERROR_FORMAT        (-13)

#define NULL_SLOT       (-1)

/**
 *  Set the flag about card refreshing.
 *
 *  @param [in] refreshing Enable
 *  @param [in] slot Slot id
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibCard_StatusSetRefreshing(int slot, int refreshing);

/**
 *  Set the flag to block card inserted flow.
 *
 *  @param [in] cardId Card id
 *  @param [in] en Enable
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibCard_StatusSetBlock(int cardId, int en);

/**
 *  Check the card that be blocked.
 *
 *  @return >=0 card id, <0 failure
 */
extern int AppLibCard_StatusCheckHighProrityBlock(void);

/**
 *  Check the card that could be blocked.
 *
 *  @param [in] cardId card id
 *
 *  @return >=0 blocked, <0 Not blocked.
 */
extern int AppLibCard_StatusCheckBlock(int cardId);

/**
 *  @brief Send message of card inserted.
 *
 *  Send message of card inserted.
 *
 *  @param [in] cardId Card id
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibCard_SndCardInsertMsg(int cardId);

/**
 *  Check the valid size of NAND storage
 *
 *  @param [in] cardId Card id
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibCard_CheckNandStorage(int cardId);

/**
 *  Check the free space of card
 *
 *  @return >=0 Enough space, <0 Not enough space
 */
extern UINT64 AppLibCard_GetFreeSpace(char drive);

/**
 *  Check the total space of card
 *
 *  @return >=0 Enough space, <0 Not enough space
 */
extern UINT64 AppLibCard_GetTotalSpace(char drive);

/**
 *  Check the free space of card
 *
 *  @return >=0 Enough space, <0 Not enough space
 */
extern int AppLibCard_CheckFreespace(void);

/**
 *  Get the flag of free space.
 *
 *  @return >=0 success, <0 failure
 */
extern int AppLibCard_GetFreespaceFlag(void);


__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_STORAGE_CARD_H_ */
