

#include <index/Temp.h>
#include <index/Raw.h>
#include <index/Mem.h>
#include <applib.h>

static APPLIB_INDEX_FILE_STORAGE_TYPE_e IndexFailStorageType = APPLIB_INDEX_FILE_STROAGE_TYPE_UNKNOW;

/**
 *  @brief create index file handler
 *
 *  create index file handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibIndex_CreateHdlr(AMP_INDEX_HDLR_s **indexHdlr)
{
    int ReturnValue = 0;

    if (IndexFailStorageType == APPLIB_INDEX_FILE_STROAGE_TYPE_UNKNOW) {
        AmbaPrintColor(RED, "<AppLibIndex_CreateHdlr> Please set the index file storage type first!");
        AmbaPrintColor(RED, "<AppLibIndex_CreateHdlr> The default index file storage type is TMP_IDX");
    }

    switch (IndexFailStorageType) {
        case APPLIB_INDEX_FILE_STROAGE_TYPE_MEMORY:   // MEM_IDX
            {
            	AMP_MEM_IDX_CFG_s MemCfg = {0};
        		AmpMemIdx_GetDefaultCfg(&MemCfg);
        		if (AmpMemIdx_Create(&MemCfg, indexHdlr) != AMP_OK) {
        			AmbaPrintColor(RED,"<AppLibIndex_CreateHdlr> %s:%u", __FUNCTION__, __LINE__);
        			ReturnValue = -1;
        		}
            }
            break;
        case APPLIB_INDEX_FILE_STROAGE_TYPE_NAND:     // RAW_IDX
            {
                AMP_RAW_IDX_CFG_s RawCfg = {0};
                AmpRawIdx_GetDefaultCfg(&RawCfg);
                if (AmpRawIdx_Create(&RawCfg, indexHdlr) != AMP_OK) {
                    AmbaPrintColor(RED,"<AppLibIndex_CreateHdlr> %s:%u", __FUNCTION__, __LINE__);
                    ReturnValue = -1;
                }
            }
            break;
        case APPLIB_INDEX_FILE_STROAGE_TYPE_SD_CARD:  // TMP_IDX
        default:
            {
                AMP_TEMP_IDX_CFG_s TempCfg = {0};
                AmpTempIdx_GetDefaultCfg(&TempCfg);
                if (AmpTempIdx_Create(&TempCfg, indexHdlr) != AMP_OK) {
                    AmbaPrintColor(RED,"<AppLibIndex_CreateHdlr> %s:%u", __FUNCTION__, __LINE__);
                    ReturnValue = -1;
                }
            }
            break;
    }

    return ReturnValue;
}

/**
 *  @brief delete index file handler
 *
 *  delete index file handler
 *
 *  @return >=0 success, <0 failure
 */
int AppLibIndex_DeleteHdlr(AMP_INDEX_HDLR_s *indexHdlr)
{
    int ReturnValue = 0;

    if (IndexFailStorageType == APPLIB_INDEX_FILE_STROAGE_TYPE_UNKNOW) {
        AmbaPrintColor(RED, "<AppLibIndex_DeleteHdlr> Please set the index file storage type first!");
        AmbaPrintColor(RED, "<AppLibIndex_DeleteHdlr> The default index file storage type is TMP_IDX");
    }

    switch (IndexFailStorageType) {
        case APPLIB_INDEX_FILE_STROAGE_TYPE_MEMORY:   // MEM_IDX
            ReturnValue = AmpMemIdx_Delete(indexHdlr);
            break;
        case APPLIB_INDEX_FILE_STROAGE_TYPE_NAND:     // RAW_IDX
            ReturnValue = AmpRawIdx_Delete(indexHdlr);
            break;
        case APPLIB_INDEX_FILE_STROAGE_TYPE_SD_CARD:  // TMP_IDX
        default:
            ReturnValue = AmpTempIdx_Delete(indexHdlr);
            break;
    }
    return ReturnValue;
}

/**
 *  @brief Initialize the index file
 *
 *  Initialize the index file
 *
 *  @return >=0 success, <0 failure
 */
int AppLibIndex_Init(void)
{
    int ReturnValue = 0;

    if (IndexFailStorageType == APPLIB_INDEX_FILE_STROAGE_TYPE_UNKNOW) {
        AmbaPrintColor(RED, "<AppLibIndex_Init> Please set the index file storage type first!");
        AmbaPrintColor(RED, "<AppLibIndex_Init> The default index file storage type is TMP_IDX");
        IndexFailStorageType = APPLIB_INDEX_FILE_STROAGE_TYPE_SD_CARD;
    }

    switch (IndexFailStorageType) {
        case APPLIB_INDEX_FILE_STROAGE_TYPE_MEMORY:     // MEM_IDX
            {
                AMP_MEM_IDX_INIT_CFG_s MemInitCfg;
                void *MemIdxBuf, *MemIdxBufRaw;
                memset(&MemInitCfg, 0x0, sizeof(AMP_MEM_IDX_INIT_CFG_s));

                AmpMemIdx_GetInitDefaultCfg(&MemInitCfg);
                MemInitCfg.MaxHdlr = 4;
                MemInitCfg.BufferSize = AmpMemIdx_GetRequiredBufferSize(MemInitCfg.MaxHdlr, 8 * 1024 * 1024);		// allocate 8M memory index
                ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &MemIdxBuf, &MemIdxBufRaw, MemInitCfg.BufferSize,  AMBA_CACHE_LINE_SIZE);
                if (ReturnValue < 0 ) {
                    AmbaPrintColor(RED,"<AppLibIndex_Init> %s:%u", __FUNCTION__, __LINE__);
                    ReturnValue = -1;
                } else {
                    MemInitCfg.Buffer = MemIdxBuf;
                    if (AmpMemIdx_Init(&MemInitCfg) != AMP_OK) {
                        AmbaPrintColor(RED,"<AppLibIndex_Init> %s:%u", __FUNCTION__, __LINE__);
                        ReturnValue = -1;
                    }
                }
            }
            break;
        case APPLIB_INDEX_FILE_STROAGE_TYPE_NAND:       // RAW_IDX
            {
                AMP_RAW_IDX_INIT_CFG_s RawInitCfg;
                void *RawIdxBuf, *RawIdxBufRaw;
                extern AMBA_NAND_DEV_INFO_s AmbaNAND_DevInfo;
                memset(&RawInitCfg, 0x0, sizeof(AMP_RAW_IDX_INIT_CFG_s));

                AmpRawIdx_GetInitDefaultCfg(&RawInitCfg,AMP_RAW_IDX_DEV_TYPE_NAND);
                RawInitCfg.MaxHdlr = 8;
                RawInitCfg.BufferSize = AmpRawIdx_GetRequiredBufferSize(RawInitCfg.MaxHdlr);
                ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &RawIdxBuf, &RawIdxBufRaw, RawInitCfg.BufferSize,  AMBA_CACHE_LINE_SIZE);
                if (ReturnValue < 0 ) {
                    AmbaPrintColor(RED,"<AppLibIndex_Init> %s:%u", __FUNCTION__, __LINE__);
                    ReturnValue = -1;
                } else {
                    RawInitCfg.Buffer = RawIdxBuf;
                    memcpy(&RawInitCfg.DevInfo.Nand.Info, &AmbaNAND_DevInfo, sizeof(AMBA_NAND_DEV_INFO_s));
                    if (AmpRawIdx_Init(&RawInitCfg) != AMP_OK) {
                        AmbaPrintColor(RED,"<MuxerInit> %s:%u", __FUNCTION__, __LINE__);
                        ReturnValue = -1;
                    }
                }
            }
            break;
        case APPLIB_INDEX_FILE_STROAGE_TYPE_SD_CARD:    // TMP_IDX
        default:
            {
                AMP_TEMP_IDX_INIT_CFG_s TempInitCfg;
                void *TempIdxBuf, *TempIdxBufRaw;
                memset(&TempInitCfg, 0x0, sizeof(AMP_TEMP_IDX_INIT_CFG_s));

                AmpTempIdx_GetInitDefaultCfg(&TempInitCfg);
                TempInitCfg.MaxHdlr = 8;
                TempInitCfg.BufferSize = AmpTempIdx_GetRequiredBufferSize(TempInitCfg.MaxHdlr);
                ReturnValue = AmpUtil_GetAlignedPool(APPLIB_G_MMPL, &TempIdxBuf, &TempIdxBufRaw, TempInitCfg.BufferSize,  AMBA_CACHE_LINE_SIZE);
                if (ReturnValue < 0 ) {
                    AmbaPrintColor(RED, "<AppLibIndex_Init> %s:%u", __FUNCTION__, __LINE__);
                    ReturnValue = -1;
                } else {
                    TempInitCfg.Buffer = TempIdxBuf;
                    if (AmpTempIdx_Init(&TempInitCfg) != AMP_OK) {
                        AmbaPrintColor(RED, "<AppLibIndex_Init> %s:%u", __FUNCTION__, __LINE__);
                        ReturnValue = -1;
                    }
                }
            }
            break;
    }
    return ReturnValue;
}


/**
 *  @brief Set the storage type for index file
 *
 *  Set the storage type for index file
 *
 *  @return >=0 success, <0 failure
 */
int AppLibIndex_SetStorageType(APPLIB_INDEX_FILE_STORAGE_TYPE_e indexType)
{
    IndexFailStorageType = indexType;
    return 0;
}

/**
 *  @brief Get the storage type for index file
 *
 *  Get the storage type for index file
 *
 *  @return >=0 success, <0 failure
 */
int AppLibIndex_GetStorageType(APPLIB_INDEX_FILE_STORAGE_TYPE_e *indexType)
{
    *indexType = IndexFailStorageType;
    return 0;
}


