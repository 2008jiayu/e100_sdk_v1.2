/**
 * @file inc/mw/fifo/fifo.h
 *
 * Amba FIFO header
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef FIFO_H_
#define FIFO_H_

#include <mw.h>

/**
 * @defgroup FIFO
 * @brief Bitstream descriptor manager
 *
 * FIFO is used to manage bits buffer descriptor.\n
 * With fifo, we could clean the hand shaking between format and codec.\n
 * We could create more then one virtual fifo to read date from base fifo.\n
 *
 */

/**
 * @addtogroup FIFO
 * @{
 */

#define AMP_FIFO_MAX_ACTIVE_FIFO    64    /**< the hard limit of active fifo count */
#define AMP_FIFO_MARK_EOS           0x00FFFFFF
#define AMP_FIFO_MARK_EOS_PAUSE     0x00FFFFFE

/**
 * fifo callback events
 *
 */
typedef enum _AMP_FIFO_EVENT_e_ {
    AMP_FIFO_EVENT_DATA_CONSUMED = AMP_FIFO_EVENT_START_NUM, /**< data consumed event to data provider (write fifo)*/
    AMP_FIFO_EVENT_DATA_EOS, /**< data end of stream to data consumer (read fifo)*/
    AMP_FIFO_EVENT_DATA_READY, /**< new data ready to data consumer (read fifo)*/
    AMP_FIFO_EVENT_GET_WRITE_POINT, /**< get write pointer info from data consumer (write fifo)*/
    AMP_FIFO_EVENT_RESET_FIFO, /**< ask data consumer (write fifo) to reset everything for bitsfifo reuse*/
    AMP_FIFO_EVENT_SYNCED_FULL /**< virtual fifo full by base sync write, or base fifo full by virtual sync write*/
} AMP_FIFO_EVENT_e;

/**
 * the data type of the entry
 */
typedef enum _AMP_FIFO_FRMAE_TYPE_e_ {
    AMP_FIFO_TYPE_MJPEG_FRAME = 0,    ///< MJPEG frame type
    AMP_FIFO_TYPE_IDR_FRAME = 1,    ///< Idr frame type
    AMP_FIFO_TYPE_I_FRAME = 2,      ///< I frame type
    AMP_FIFO_TYPE_P_FRAME = 3,      ///< P frame type
    AMP_FIFO_TYPE_B_FRAME = 4,      ///< B frame type
    AMP_FIFO_TYPE_JPEG_FRAME = 5,   ///< jpeg main frame
    AMP_FIFO_TYPE_THUMBNAIL_FRAME = 6,  ///< jpeg thumbnail frame
    AMP_FIFO_TYPE_SCREENNAIL_FRAME = 7, ///< jpeg screennail frame
    AMP_FIFO_TYPE_AUDIO_FRAME = 8,      ///< audio frame
    AMP_FIFO_TYPE_UNDEFINED = 9,        ///< others

    AMP_FIFO_TYPE_DECODE_MARK = 101, ///< used when feeding bitstream to dsp. will push out all frame. */
    AMP_FIFO_TYPE_EOS = 255,                  ///< eos bits that feed to raw buffer

    AMP_FIFO_TYPE_LAST = AMP_FIFO_TYPE_EOS
} AMP_FIFO_FRMAE_TYPE_e;

/**
 * fifo hdlr
 */
typedef struct _AMP_FIFO_s_ {
    UINT32 nFifoId; /**< the unique of the fifo */
    void *Ctx; /**< private data of the fifo */
} AMP_FIFO_HDLR_s;

/**
 * descriptor of a bitsbuffer.
 */
typedef struct _AMP_BITS_DESC_s_ {
    UINT32 SeqNum; /**< sequential number of bits buffer */
    UINT64 Pts; /**< time stamp in ticks */
    AMP_FIFO_FRMAE_TYPE_e Type; /**< data type of the entry */
    UINT8 Completed; /**< if the buffer content a complete entry */
    UINT16 Align; /** data size alignment (in bytes, align = 2^n, n is a integer )*/
    UINT8* StartAddr; /**< start address of data */
    UINT32 Size; /**< real data size */
} AMP_BITS_DESC_s;

/**
 * amp fifo config structure
 */
typedef struct _AMP_FIFO_INIT_CFG_s_ {
    UINT8* MemoryPoolAddr; /**< buffer start address for fifo module including descriptor */
    UINT32 MemoryPoolSize; /**< size of buffer */
    UINT32 NumMaxFifo; /**< max supported virtual fifo number */
    UINT32 NumMaxCodec; /**< max supported codec number */
    AMP_TASK_INFO_s TaskInfo;
} AMP_FIFO_INIT_CFG_s;

/**
 *
 */
typedef enum {
    AMP_FIFO_CFG_INIT_DISABLE = 0,  /**< no neeed for init data */
    AMP_FIFO_CFG_INIT_WITH_TIME, /**< get frames with given length */
    AMP_FIFO_CFG_INIT_WITH_NUM_FRAME, /**< get frames with given frame number. ex. if you set  NumFrame to 100, you will get 100 frames(if there are 100 frames)*/
    AMP_FIFO_CFG_INIT_WITH_START_TIME, /**< get frames with given start time in ms */
    AMP_FIFO_CFG_INIT_PAUSED, /** fifo paused on create. only take effect on virtual fifo. When create with this option, InitParam take no effect. fifo will activate by act funftion. */
} AMP_FIFO_CFG_INIT_DATA_FETCH_CONDITION_e;

/**
 * fifo initial data config
 */
typedef struct _AMP_FIFO_CFG_INIT_DATA_s_ {
    UINT8 CreateFifoWithInitData; /**< If create fifo with init data (if data valid) */

    AMP_FIFO_CFG_INIT_DATA_FETCH_CONDITION_e InitCondition; /**< define how to descript initial data required. */

    union {
        UINT64 TimeLength; /**< In normal case, the value of backward fetch is 0.\n
         * it means fifo will output frames only after fifo is created.\n
         * As the valus is not 0, fifo will try to find out the longest valid frames
         */
        UINT64 NumFrame; /**< number of frame */
        UINT64 StartTime; /**< start time in ms */
    } InitParam; /**< parameter for determinate initial data*/

    AMP_FIFO_FRMAE_TYPE_e FristFrameType; /**< Frame type of first frame for valid data, used on video stream to ensure start with Idr. */

    UINT64 OnCreateFirstFramePts; /** [OUT] Pts of first frame, 0xFFFFFFFFFFFFFFFF if first frame is not valid on create */
    UINT64 OnCreateTimeLength; /** [OUT] data length in fifo on create in ms */
} AMP_FIFO_CFG_INIT_DATA_s;

/**
 * fifo config
 */
typedef struct _AMP_FIFO_CFG_s_ {
    void *hCodec; /**< the codec the fifo is working on. */
    UINT32 NumEntries; /**< Number of entries of a fifo */
    UINT32 IsVirtual; /**< Creating virtual fifo (for data flow) or not (for codec) */
    AMP_CALLBACK_f cbEvent; /**< the callback function for fifo event */
    UINT32 EventDataConsumedThreshold; /**<
     * the threshold for event AMP_FIFO_CALLBACK_EVENT_DATA_CONSUMED\n
     * event only triggered if remain data us under the threshold.
     * 0 to disable
     */
    UINT32 EventDataReadySkipNum; /**<
     * if not 0, AMP_FIFO_CALLBACK_EVENT_DATA_READY will not be triggered every frame encoded.\n
     * it will be triggered every (eventDataReadySkipNum frame+1) frames
     */

    AMP_CALLBACK_f *cbGetWritePoint; /**<
     * the callback function invoked when we receive prepareSpace.\n
     * it should ONLY be used on fifo linked to codec \n
     * it only take effect on a codec read fifo.\n
     * the callback should be registered by a decode mgr for all decode codec\n
     */

    UINT8 SyncRpOnWrite;
    /**< if 0, rp of base fifo will be updated on remove of virtual fifo.\n
     *   if 1, rp of base fifo will be updated on write of base fifo.\n
     *   ONLY take effect on base fifo.
     */

    UINT32 RawBaseAddr; /**< when sync rp on write is on and RawBaseAddr/RawLimitAddr is given, base fifo will remove desc automatically on overwrite. \n
                        * only needed on base fifo and sync on write is on
                        */

    UINT32 RawLimitAddr;/**< when sync rp on write is on and RawBaseAddr/RawLimitAddr is given, base fifo will remove desc automatically on overwrite. \n
                        * only needed on base fifo and sync on write is on
                        */
    
    UINT64 TickPerSecond; /**< TiackPerSecond used for Pts for the fifo. \n
    Could be 0 if we whould NOT like to use init data. */

    AMP_FIFO_CFG_INIT_DATA_s InitData; /**< init status for virtual fifo
     */
} AMP_FIFO_CFG_s;

/**
 * amba fifo config structure
 */
typedef struct _AMP_FIFO_INFO_s_ {
    UINT32 TotalEntries; /**< total number of entries */
    UINT32 AvailEntries; /**< number of entries with data*/
    UINT32 LinkedFifoMinEmptyEntries; /**< number of empty entries for all linked(base and virtal) fifo*/
} AMP_FIFO_INFO_s;

/**
 * get fifo module default config for initializing
 *
 * @param [out] cfg fifo module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFifo_GetInitDefaultCfg(AMP_FIFO_INIT_CFG_s *defaultCfg);

/**
 * Initialize fifo module.\n
 * The function should only be invoked once.
 * User MUST invoke this function before using fifo module.
 * The memory pool of the module will provide by user.
 *
 * @param [in] cfg fifo module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFifo_Init(AMP_FIFO_INIT_CFG_s *cfg);

/**
 * get fifo default config
 *
 * @param [out] cfg fifo module config
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFifo_GetDefaultCfg(AMP_FIFO_CFG_s *defaultCfg);

/**
 * Create a virtual fifo on a codec.
 * It's used to manage the bits descriptor that generated by codec
 * or feeding data to codec\n
 * On creating, getNumDesc() function of codec will be invoked to alloc virtual desctiptor queue in the fifo.
 * @note only 1 wrtie fifo for a codec is allowed
 *
 * @param [in] cfg the config of the fifo
 * @param [out] fifo the fifo handler
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 *
 * @see AMP_BITS_DESC_s
 */
extern int AmpFifo_Create(AMP_FIFO_CFG_s *cfg,
                          AMP_FIFO_HDLR_s **fifo);

/**
 * close a fifo.
 *
 * @param [in] fifo fifo to close
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFifo_Delete(AMP_FIFO_HDLR_s *fifo);

/**
 * Peek entry in fifo by the given distance.\n
 * For example, there is three entries in current fifo.\n
 * (wp) [entry A] [entry B] [entry C] (rp)\n
 * AmpFifo_PeekEntry(hFifo, &desc, 0) will get entry C\n
 * AmpFifo_PeekEntry(hFifo, &desc, 1) will get entry B\n
 * AmpFifo_PeekEntry(hFifo, &desc, 2) will get entry A\n
 * AmpFifo_PeekEntry(hFifo, &desc, 3) will return failed
 *
 * @param [in] fifo the fifo to operate
 * @param [out] desc the peeked entry
 * @param [in] distanceToLastEntry distance to last entry
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFifo_PeekEntry(AMP_FIFO_HDLR_s *fifo,
                             AMP_BITS_DESC_s **desc,
                             UINT32 distanceToLastEntry);

/**
 * Peek entry in reverse order.\n
 * Peek entry in fifo by the given distance from the newest entry.\n
 * For example, there are three entries in current fifo.\n
 * (wp) [entry A] [entry B] [entry C] (rp)\n
 * AmpFifo_PeekEntryEnd(hFifo, &desc, 0) will get entry A\n
 * AmpFifo_PeekEntryEnd(hFifo, &desc, 1) will get entry B\n
 * AmpFifo_PeekEntryEnd(hFifo, &desc, 2) will get entry C\n
 * AmpFifo_PeekEntryEnd(hFifo, &desc, 3) will return failed
 *
 * @param [in]  fifo                the fifo to operate
 * @param [out] desc                the peeked entry
 * @param [in]  distanceToEnd       distance to the newest entry.
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFifo_RevPeekEntry(AMP_FIFO_HDLR_s *fifo,
                                AMP_BITS_DESC_s **desc,
                                UINT32 distanceToEnd);

/**
 * Remove last n entry.\n
 * For example, there is three entries in current fifo.\n
 * (wp) [entry A] [entry B] [entry C] (rp)\n
 * After AmpFifo_RemoveEntry(hFifo, 2)\n
 * it becomes\n
 * (wp) [entry A] (rp)\n
 *
 *
 * The remove function only works on the virtual fifo indecated by parameter.\n
 * If there are other fifos working on the same codec, their read point won't be changed.\n
 *
 * @param [in] fifo the fifo to operate
 * @param [in] n number of entry to remove
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFifo_RemoveEntry(AMP_FIFO_HDLR_s *fifo,
                               UINT32 n);

/**
 * prepare space to write data to fifo.\n
 * User MUST use this function to get the correct descriptor of the write point
 *
 * @param [in] fifo the fifo to operate
 * @param [out] desc the descriptor of the space
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFifo_PrepareEntry(AMP_FIFO_HDLR_s *fifo,
                                AMP_BITS_DESC_s *desc);

/**
 * write data to fifo\n
 * For example, there is one entries in current fifo.\n
 * (wp) [entry A]  (rp)\n
 * After AmpFifo_WriteDataToFifo(hFifo, descEntryB)\n
 * it becomes\n
 * (wp) [entry B][entry A] (rp)\n
 * All write point of fifos that working on the same codec will be updated after the function.\n
 *
 * @param [in] fifo the fifo to operate
 * @param [in] desc the descriptor of the data to write
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFifo_WriteEntry(AMP_FIFO_HDLR_s *fifo,
                              AMP_BITS_DESC_s *desc);

/**
 * erase all data in fifo. Reset the read and write point.
 *
 * @param [in] fifo the fifo to operate
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 */
extern int AmpFifo_EraseAll(AMP_FIFO_HDLR_s *fifo);

/**
 * Resume a paused fifo which is set to pause on create.
 *
 * @param [in] fifo the fifo to operate
 * @param [in] initData init data on resume 
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e 
 */
extern int AmpFifo_Resume(AMP_FIFO_HDLR_s *fifo,
                          AMP_FIFO_CFG_INIT_DATA_s *initData);

/**
 * to get fifo info
 *
 * @param [in] fifo the fifo to operate
 * @param [out] info the info of the fifo
 *
 * @return 0 - OK, others - AMP_ER_CODE_e
 * @see AMP_ER_CODE_e
 * @see AMP_FIFO_INFO_s
 */
extern int AmpFifo_GetInfo(AMP_FIFO_HDLR_s *fifo,
                           AMP_FIFO_INFO_s *info);

/**
 * @}
 */

#endif /* FIFO_H_ */
