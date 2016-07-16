/**
 *  @file Editor.h
 *
 *  Editor header
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 */
#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <format/Format.h>

/**
 * @defgroup Editor
 * @brief Editor implementation
 *
 * The implementation the Editor module
 * Editor implements the API for users to edit files.
 * Editor provides the Crop2New, Merge, Partial Delete and Recover functions.
 *
 *
 * Editor includes the following functions:
 * 1. Initiate the Editor module
 * 2. Crop2New function
 * 3. Merge function
 * 4. Partial Delete function
 * 5. Recover function
 *
 */

/**
 * @addtogroup Editor
 * @ingroup DataPipeline
 * @{
 */

/**
 * Editor event
 */
typedef enum {
    AMP_EDITOR_EVENT_OK = 0x01,     /**< The event raised once the Editor completes the operation */
    AMP_EDITOR_EVENT_ABORT = 0x02,  /**< The event raised once the Editor abort the operation */
    AMP_EDITOR_EVENT_ERROR = 0xF0   /**< The event raised once the Editor encounters an error */
} AMP_EDITOR_EVENT_e;

/**
 * Editor round mode
 */
typedef enum {
	AMP_EDITOR_ROUND_OFF,           /** Round off mode */
	AMP_EDITOR_ROUND_UP,            /** Ceiling mode */
	AMP_EDITOR_ROUND_DOWN,          /** Floor mode */
	AMP_EDITOR_ROUND_MODE_MAX
} AMP_EDITOR_ROUND_MODE_e;

/**
 * The configuration for initializing Editor
 */
typedef struct {
    UINT8 *Buffer;              /**< The work buffer of Editor */
    UINT32 BufferSize;          /**< The size of the work buffer */
    AMP_TASK_INFO_s TaskInfo;   /**< The task information */
    UINT8 MaxCmd;               /**< The maximum number of commands queued in Editor */
    UINT8 MaxSubTask;           /**< The maximum number of sub tasks held in Editor */
} AMP_EDITOR_INIT_CFG_s;

/**
* Get the default configuration for initializing the Editor module .
*
* @param [out] config The returned configuration
* @return 0 - OK, others - AMP_ER_CODE_e
* @see AMP_ER_CODE_e
*/
extern int AmpEditor_GetInitDefaultCfg(AMP_EDITOR_INIT_CFG_s *config);

/**
* Initialize the Editor module.
*
* @param [in] config The configuration for initialize the Editor module
* @return 0 - OK, others - AMP_ER_CODE_e
* @see AMP_ER_CODE_e
*/
extern int AmpEditor_Init(AMP_EDITOR_INIT_CFG_s *config);

/**
* Abort the Editor operation.
*
* @return 0 - OK, others - AMP_ER_CODE_e
* @see AMP_ER_CODE_e
*/
extern int AmpEditor_Abort(AMP_EDT_FORMAT_HDLR_s *hdlr);

/**
* Crop a segment of an input file to create a new one.
*
* @param [in] hdlr The Format handler used by Editor
* @param [in] onEvent The event callback returning an execution result
* @param [in] newTask The value used to indicate that the editing operation is worked on a new task or the main task
* @param [in] timeStart The start time of the cropped segment (ms)
* @param [in] timeEnd The end time of the cropped segment (ms)
* @param [in] trim The flag used to enable Editor to trim the output file
* @param [in] fileNameIn The name of the input file
* @param [in] fileNameOut The name of the output file
* @return 0 - OK, others - AMP_ER_CODE_e
* @see AMP_ER_CODE_e
*/
extern int AmpEditor_Crop2New(AMP_EDT_FORMAT_HDLR_s *hdlr, BOOL newTask, UINT32 timeStart, UINT32 timeEnd, BOOL trim, char *fileNameIn, char *fileNameOut);

/**
* Divide an input file into two parts according to a specified time. (The first part is retained in the input file, and the second one is stored in an output file.)
*
* @param [in] hdlr The Format handler used by Editor
* @param [in] onEvent The event callback returning an execution result
* @param [in] newTask The value used to indicate that the editing operation is worked on a new task or the main task
* @param [in] targetTime The reference time to dividing the input file (ms)
* @param [in] trim The flag used to enable Editor to trim the output file
* @param [in] roundMode The value used to indicate a method to determine a real dividing time (See AMP_EDITOR_ROUND_MODE_e.)
* @param [in] fileNameIn The name of the input file
* @param [in] fileNameOut The name of the output file
* @return 0 - OK, others - AMP_ER_CODE_e
* @see AMP_ER_CODE_e
*/
extern int AmpEditor_Divide(AMP_EDT_FORMAT_HDLR_s *hdlr, BOOL newTask, UINT32 targetTime, BOOL trim, UINT8 roundMode, char *fileNameIn, char *fileNameOut);

/**
* Merge two input files into a single one. The second input file will be appended to the first one.
*
* @param [in] hdlr The Format handler used by Editor
* @param [in] onEvent The event callback returning an execution result
* @param [in] newTask The value used to indicate that the file operation is worked on a new task or the main task
* @param [in] trim The flag used to enable Editor to trim the output file
* @param [in] directed The flag to enable Editor to directly append the second input file to the first one without any silence frames
* @param [in] fileNameIn1 The name of the first input file
* @param [in] fileNameIn2 The name of the second input file
* @return 0 - OK, others - AMP_ER_CODE_e
* @see AMP_ER_CODE_e
*/
extern int AmpEditor_Merge(AMP_EDT_FORMAT_HDLR_s *hdlr, BOOL newTask, BOOL trim, BOOL directed, char *fileNameIn1, char *fileNameIn2);

/**
* Delete a segment of an input file, and change the file name to fileNameOut.
*
* @param [in] hdlr The Format handler used by Editor
* @param [in] onEvent The event callback returning an execution result
* @param [in] newTask The value used to indicate that the file operation is worked on a new task or the main task
* @param [in] timeStart The start time of the removed segment (ms)
* @param [in] timeEnd The end time of the removed segment (ms)
* @param [in] trim The flag used to enable Editor to trim the output file
* @param [in] fileNameIn The name of the input file
* @param [in] fileNameOut The name of the output file
* @return 0 - OK, others - AMP_ER_CODE_e
* @see AMP_ER_CODE_e
*/
extern int AmpEditor_PartialDelete(AMP_EDT_FORMAT_HDLR_s *hdlr, BOOL newTask, UINT32 timeStart, UINT32 timeEnd, BOOL trim, char *fileNameIn, char *fileNameOut);

/**
* Recover a bad or abnormally closed recording file.
*
* @param [in] hdlr The Format handler used by Editor
* @param [in] onEvent The event callback returning an execution result
* @param [in] newTask The value used to indicate that the file operation is worked on a new task or the main task
* @param [in] fileName The name of the input file being recovered
* @return 0 - OK, others - AMP_ER_CODE_e
* @see AMP_ER_CODE_e
*/
extern int AmpEditor_Recover(AMP_EDT_FORMAT_HDLR_s *hdlr, BOOL newTask, char *fileName);

/**
 * @}
 */

#endif /* __EDITOR_H__ */

