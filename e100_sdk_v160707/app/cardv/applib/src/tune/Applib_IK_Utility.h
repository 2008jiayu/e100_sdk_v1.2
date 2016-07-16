#include "AmbaDSP_ImgDef.h"

#ifndef NULL
#define NULL (void*)0
#endif
#ifndef MAX_FILE_PATH
#define MAX_FILE_PATH (64)
#endif
typedef enum _Applib_IK_ER_CODE_e_ {
    Applib_IK_OK = 0,                   //!< Execution OK
    Applib_IK_ERROR_GENERAL_ERROR = -1, //!< General error
} Applib_IK_ER_CODE_e;

typedef enum _Applib_IK_CMD_ {
	Applib_IK_CMD_INIT = 0x00 ,
	Applib_IK_CMD_UNINIT,
	Applib_IK_CMD_PRINT_LOG ,
	Applib_IK_CMD_STILL_SAVE_ISOCONFIG ,
	Applib_IK_CMD_STILL_CMP_ISOCONFIG ,
	Applib_IK_CMD_MAX,
} Applib_IK_CMD_e;

typedef enum _Applib_IK_MODE_ {
	Applib_IK_MODE_STILL = 0x00 ,
	Applib_IK_MODE_VIDEO,
	Applib_IK_MODE_UNKNOW,
} Applib_IK_MODE_e;

typedef enum _Applib_IK_CMP_RESULT_ {
	Applib_IK_RESULT_NG = 0x00,
	Applib_IK_RESULT_PASS,
	Applib_IK_RESULT_UNKNOW,
} Applib_IK_CMP_RESULT_e;

typedef struct {
	UINT32 CompareNum;
	Applib_IK_MODE_e Mode;
} INIT_INFO_s;

typedef struct {
	char* pInItunerFileName;
	char* pOutBinFileName;
} STILL_SAVE_ISOCONFIG_INFO_S_s;

typedef struct {
	char* pInItunerFileName;
	char* pInBinFileName;
} STILL_CMP_ISOCONFIG_INFO_S_s;

typedef union {
	INIT_INFO_s InitData;
	INIT_INFO_s UninitData;
	INIT_INFO_s PrintLogData;
	//STILL
	STILL_SAVE_ISOCONFIG_INFO_S_s SaveIsoCfgData;
	STILL_CMP_ISOCONFIG_INFO_S_s CmpIsoCfgData;
	//VIDEO

	//TBD
} Applib_IK_INPUTS_u;

typedef struct {
	Applib_IK_CMP_RESULT_e Result;
} Applib_IK_STILL_RESULT_INFO_s;

typedef struct {
	UINT32 IsoCfgSize;
	char ItunerFileName[MAX_FILE_PATH];
	char BinFileName[MAX_FILE_PATH];
	//TBD
} Applib_IK_STILL_PROCESS_INFO_s;

typedef struct {
	Applib_IK_MODE_e Mode;
	union {
		Applib_IK_STILL_RESULT_INFO_s Still;
		//TBD
	} CmpData;
} Applib_IK_RESULT_INFO_s;
typedef struct {
	Applib_IK_MODE_e Mode;
	AMBA_KAL_BYTE_POOL_t *MemPool;
	union {
		Applib_IK_STILL_PROCESS_INFO_s Still;
		//TBD
	} ProcessInfo;
	union {
		Applib_IK_STILL_RESULT_INFO_s Still;
		//TBD
	} ResultInfo;
} Applib_IK_DATA_CONTAINER_s;
/**
 * @brief Iamge Kernel Unit Test
 * @param[in] Applib_IK_INPUT *input: IK unit test inputs
 *
 * return 0 (Success) / -1 (Failure)
 */
int Applib_IK_Test( Applib_IK_CMD_e cmd, Applib_IK_INPUTS_u *input, AMBA_KAL_BYTE_POOL_t *memoryPool );
